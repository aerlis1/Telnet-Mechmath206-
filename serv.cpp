
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <unistd.h>

#include <iostream>
#include <cstdlib>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define PORT 1234

void work(int sc);

int main(void){
    int s, sc;
    struct sockaddr_in addr, caddr;
    long val;
    socklen_t csiz;
    struct linger linger_opt = {1, 0};
    s = socket (AF_INET, SOCK_STREAM, 0);
    if (s < 0){
        printf ("Socket create error %d\n", s);
        perror("");
        return -1;
    }
    memset (&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    if(bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0){
        perror("bind()");
        return -1;
    }
    setsockopt(s, SOL_SOCKET, SO_LINGER, &linger_opt, sizeof(linger_opt));

    if (listen(s, 5) < 0){
        perror("listen()");
        return -1;
    }
    printf("Wait for connection...\n");
    memset(&caddr, 0, sizeof(caddr));
    csiz = sizeof(caddr);
    sc = accept(s, (struct sockaddr*)&caddr, &csiz);
    if(sc < 0){
        perror("accept()");
        return -1;
    }else{
        printf("Connected.\n");
        if( addr.sin_family == AF_INET){
            printf("Peer address %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
        }
        work(sc);
        printf("Press any key\n");
        getchar();
        shutdown(sc, SHUT_RDWR);
        sleep(1);
        close(sc);
        printf("Connection has closed\n");
    }
    close(s);
    return 0;
}




void work(int sc)
{
    //длина переданных/принятых данных
    int len = 1;
    //строка для передачи информации. BUFSIZ какая-то встроенная константа
    char buf[BUFSIZ];
    //через этот поток будут выполняться команды, и в него будет записываться результат их выполнения
    FILE* pipe;
    
    //пока мы что-то получаем от клиента, и пока это что-то не quit
    while(((len = read(sc, buf, sizeof(buf) - 1)) > 0)&&(strncmp (buf, "quit", 4) != 0))//почему sizeof - 1 читать ниже
    {
    	//здесь какие-то махинации с данными, типо если в конце символьного массива
    	//стоит символ '\0', то массив считается строкой
    	//но я не уверен, что это необходимо, так как от клиента он приходит, вроде бы, и так уже строкой
        buf[len] = '\0';
        std::cout << "From client: " << buf <<"\n";
        //отдельный случай для cd, не уверен, но скорее всего он не работает в рамках общего случая как раз из-за труб
        //то есть меняет директорию не стандартный поток ввода/вывода, а временный,
        //который мы используем для выполнения в нём одной команды и получения в него вывода,
        //после чего закрываем
        	//возможно есть и другие bush команды, которые не будут работать по схожим причинам
        if(strncmp (buf, "cd", 2) == 0)
        {
            //избавляемся от cd в начале строки, так как функции chdir нужен только путь
            buf[0] = ' ';
            buf[1] = ' ';
            
            int res = chdir(buf);
	        if (res == 0) 
	        {
	        	//к слову, весь данный код скорее набросок, ну и printf тут временно
	        	//вместо него нужно отправлять клиенту, что всё хорошо, или всё плохо
		        printf("chdir is success\n");
	        }
        }
        else  //для всех остальных команд
        {
            //мы открываем поток для команды, указанной в buf, исполняем её там, получаем её вывод,
            //запихиваем его в buf с помощью fgets, и отправляем клиенту
            
	    if((pipe = popen(buf, "r")) == NULL)
			    perror("=(\n");
			    
	    while (fgets (buf, BUFSIZ, pipe) != NULL)
		  write(sc, buf, sizeof(buf));
	    //закрываем временный поток	        
	    pclose(pipe);
        }
    }
    //здесь код, оставшийся от сервера, написанного на семинаре
    
    /*    
    int len;
    char buf[512+1];
    len = read(sc, buf, sizeof(buf) - 1);
    printf("read[%d] of %ld\n", len, sizeof(buf) - 1);
    if(len > 0)
    {
        buf[len] = '\0';
        printf("<%s>\n", buf);
    }
    len = write(sc, "THank you", 10);
    printf("write: %d of %d\n", len, 10 );
    */

}

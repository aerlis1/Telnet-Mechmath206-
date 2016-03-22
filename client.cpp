
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

#define SRV_ADDR "127.0.0.1"
#define SRV_NAME "localhost"

#define SRV_PORT 1234

void work(int sc);

int main(void)
{
    int s;
    struct sockaddr_in addr;
    
    //struct hostent *host;
    
    s = socket(AF_INET,SOCK_STREAM,0);
    if(s<0) {
        printf("Socket create error %d\n", s);
        return -1;
    }
    
    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SRV_PORT);
    
    addr.sin_addr.s_addr = inet_addr(SRV_ADDR);

    if( connect(s,(struct sockaddr*)&addr,sizeof(addr))<0) {
        perror("connect()"); return -1;
    }
    else {
        printf("Connected. \n");
        
        work(s);
        
        printf("Press any key\n"); getchar();
    }
    
    shutdown(s, SHUT_RDWR);
    
    sleep(1);
    
    close(s);
    printf("Connection has losted\n");
    
    return 0;
}



void work(int s)
{
    //длина переданных/принятых данных
    int len;
    //строка для передачи информации. BUFSIZ какая-то встроенная константа
    char buf[BUFSIZ];
    
    //пока не брейкнимся
    while(true)
    {
        //кидаем в buf то, что хотим отправить
        std::cin >> buf;
        //отправляем серверу, попутно узнавая длину отправленного
        len = write(s, buf, sizeof(buf));
        //если решили выйти, брейкаемся (а серверу мы об этом сообщили в предыдущей строчке)
        if(strncmp (buf, "quit", 4)==0)
            break;
        //пока от сервера что-то приходит - читаем
        while((len = read(s, buf, sizeof(buf) - 1)) > 0)
        {
            //махинации с буфером (о них написано в комментах к серверу)
            buf[len]= '\0';
            //выводим на экран то, что получили от сервера
            std::cout << "From Server: " << buf << "\n";
            //вообще говоря, наверное где-то здесь буфер надо чистить (и, наверное, не только здесь)
        }
    }
    
    //здесь код, оставшийся от клиента, написанного на семинаре
    /*
    int len;
    char buf[512+1];
    len = write(s, "Hello",10);
    printf("write: %d of %d\n", len, 10);
    len=read(s,buf, sizeof(buf)-1);
    printf("read[%d]\n", len);
    if(len>0)
    {
        buf[len]= '\0';
        printf("<%s>\n",buf);
    }
    */
}

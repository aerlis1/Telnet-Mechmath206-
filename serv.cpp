
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
        printf("Connection has closed");
    }
    close(s);
    return 0;
}




void work(int sc)
{
    int len;
    char buf[BUFSIZ];
    FILE* pipe;
    
    
    while(((len = read(sc, buf, sizeof(buf) - 1)) > 0)&&(buf[0] != 0)); //0 == quit
    {
        buf[len] = '\0';
        
        if(strncmp (buf, "cd", 2)==0)
        {
            buf[0] = " ";
            buf[1] = " ";
            
            int res = chdir(buf);
	        if (res == 0) 
	        {
		        printf("chdir is success\n");
	        }
            //почему-то cd не работает с помощью нижеследующего способа
            //нужно использовать chdir
        }
        else
        {
            if((pipe = popen("buf", "r")) == NULL)
			    perror("=(\n");
			    
			while (fgets (buf, BUFSIZ, pipe) != NULL)
		        write(sc, buf, sizeof(buf));
		        
		    pclose(pipe);
        }
    }
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

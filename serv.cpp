 

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/select.h>
#include <iostream>
#include <cstdlib>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

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


int len = 1;
char buf[512];
int fdin[2], fdout[2], fderr[2];
pid_t pid;
pipe(fdin), pipe(fdout), pipe(fderr);

fd_set rfds1, rfds1a;
FD_ZERO(&rfds1a);
FD_SET(fdout[0], &rfds1a);

/*
fd_set rfds2;
FD_ZERO(&rfds2);*/
FD_SET(fderr[0], &rfds1a);

rfds1 = rfds1a;

struct timeval tv;
tv.tv_sec = 2;
tv.tv_usec = 0;

pid = fork();
if(pid == 0)
{
dup2(fdin[0], 0);
dup2(fdout[1], 1);
dup2(fderr[1], 2);
close(fdin[1]); close(fdout[0]); close(fderr[0]);
execl("/bin/bash"," ", NULL);
exit(1);
}
else
{
close(fdin[0]); close(fdout[1]); close(fderr[1]);
memset(buf, 0, sizeof(buf));
write(sc, "SrvIsReady", 10);
//while(((select(sc+1, &rfds, NULL, NULL, NULL)) > 0)&&((len = read(sc, buf, sizeof(buf) - 1)) > 0)&&(strncmp (buf, "quit", 4) != 0))
while(((len = read(sc, buf, sizeof(buf))) > 0)&&(strncmp (buf, "quit", 4) != 0))
{/*
    if(len>0)
    {
        write(sc, "11", 2);
    }*/
std::cout<<"\nFrom client: \n"<<buf<<"\n";
write(fdin[1], buf, sizeof(buf));
memset(buf, 0, sizeof(buf));

rfds1 = rfds1a;
if((select(fderr[0]+fdout[0]+1, &rfds1, NULL, NULL, &tv)) > 0)
{
    read(fderr[0], buf, sizeof(buf));
    write(sc, buf, sizeof(buf));
}/*
else if((select(fdout[0]+1, &rfds1, NULL, NULL, &tv)) > 0)
{
    read(fdout[0], buf, sizeof(buf));
    write(sc, buf, sizeof(buf));
}*/

//read(fdout[0], buf, sizeof(buf));
//write(sc, buf, sizeof(buf));
//if(len>0)
//    printf("%d", len);
memset(buf, 0, sizeof(buf));
write(sc, "SrvIsReady", 10);
}
}
}

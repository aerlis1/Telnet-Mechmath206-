#include <sys/select.h>

#include <iostream>
#include <cstdlib>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main() 
{
        char buf[256];
        int fdin[2], fdout[2], fderr[2];
        pid_t pid;
        
        pipe(fdin), pipe(fdout), pipe(fderr);
 //       dup2(fdout[1], 1);
 //       dup2(fderr[1], 2);
        
        pid = fork();
        
        if(pid == 0)
        {
           dup2(fdin[0],0);
           close(fdin[1]);
   //         close(fdout[0]); close(fderr[0]);
            execl("/bin/bash","ls", NULL);
            exit(1);
        }
        else
        {
            close(fdin[0]); 
   //         close(fdout[1]); close(fderr[1]);
            
            std::cin>>buf;
   //         write(fdin[1], buf, sizeof(buf));
            write(fdin[1],"ls -l\n",7);
            read(fdout[0], buf, sizeof(buf));
   //         std::cout<<buf<<"\n";
        }
        return 0;
}

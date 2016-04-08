#include <sys/select.h>

#include <iostream>
#include <cstdlib>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


int main() 
{
        char buf[256];
        int fdin[2], fdout[2], fderr[2];
        pid_t pid;
        
        pipe(fdin), pipe(fdout), pipe(fderr);
	
        pid = fork();
        
        if(pid == 0)
        {
            dup2(fdin[0], 0);
	    dup2(1, fdout[1]);
	    dup2(2, fderr[1]);
	    
            close(fdin[1]); close(fdout[0]); close(fderr[0]);
	    
            execl("/bin/bash"," ", NULL);
            exit(1);
        }
        else
        {
            close(fdin[0]); close(fdout[1]); close(fderr[1]);
            
	    memset(buf,  0, sizeof(buf));

	    fgets(buf, sizeof(buf), stdin);
	    
	    
            write(fdin[1], buf, sizeof(buf));
            
            read(fdout[0], buf, sizeof(buf));
            std::cout<<"\nOur result: \n"<<buf<<"\n";
        }
        return 0;
}

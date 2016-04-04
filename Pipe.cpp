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
                //исправил перенаправление оставшихся потоков
                //теперь вроде всё работает
            dup2(fdin[0], 0);
	    dup2(1, fdout[1]);
	    dup2(2, fderr[1]);
	    
            close(fdin[1]);
            close(fdout[0]); close(fderr[0]);
	    
            execl("/bin/bash"," ", NULL);
            exit(1);
        }
        else
        {
            close(fdin[0]); 
            close(fdout[1]); close(fderr[1]);
            
            //не смог сделать так, чтобы выполняло то, что мы кладём в буфер
            //там видимо какая-то хрень со строками и символьными массивами
            //так что пока что выполняется только то, что пишем в кавычках вручную
            
            //std::cin>>buf;

           //write(fdin[1], buf, sizeof(buf));
            
	    write(fdin[1],"ls -l\n",7);
            read(fdout[0], buf, sizeof(buf));
            std::cout<<"\nOur result: \n"<<buf<<"\n";
        }
        return 0;
}

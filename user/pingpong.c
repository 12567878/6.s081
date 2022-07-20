//
// Created by 吴斐 on 2022/5/12.
//
#include "kernel/types.h"
#include "user/user.h"

int 
main (void)
{
    
    int descriptors[2];
    pipe(descriptors);
    int pid=fork();
    int my_pid=getpid();
    if(pid == 0){//child p
        char buf[20];
        read(descriptors[0],buf,5);
        close(descriptors[0]);
        printf("%d: received %s\n",my_pid,buf);
        if(write(descriptors[1],"pong",5)!=5){
            printf("error");
            close(descriptors[1]);
            exit(1);
        } else{
            close(descriptors[1]);
            exit(0);
        }
    } else if(pid > 0){
        if(write(descriptors[1],"ping",5)!=5){
            printf("error");
            close(descriptors[1]);
            exit(1);
        } else{
            char buf[20];
            int status=0;
            close(descriptors[1]);
            wait(&status);
            read(descriptors[0],buf,5);
            close(descriptors[0]);
            printf("%d: received %s\n",my_pid,buf);
            exit(0);
        }
    } else{
        printf("error");
        exit(1);
    }
    exit(0);
}
//
// Created by 吴斐 on 2022/5/12.
//
#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    //检查输入参数合法性，有且只能有1个
    if(argc < 2){
        fprintf(2,"usage: sleep for certain time [time]...\n");
        exit(1);
    }
 
    if(argc > 2){
        fprintf(2,"too many arguments, only receive 1.\n");
        exit(1);
    }
    
    int time = atoi(argv[1]);
    sleep(time);
    exit(0);
}
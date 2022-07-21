//
// Created by 吴斐 on 2022/7/21.
//
#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"
int
main(int argc, char *argv[])
{
    char temp;

    char *argvs[MAXARG];
    for (int i = 1; i < argc; ++i) {
        argvs[i-1]=argv[i];
    }

    int i=argc-1;
    argvs[i]=(char*)malloc(sizeof (char)*MAXPATH);
    char *p_arg=argvs[i];
    while(read(0,&temp,sizeof (char))!=0){
        if(temp-'\n'==0) {//表示一个参数结束
            *p_arg='\0';
            if(i>MAXARG) {
                printf("xargs:too many args\n");
                exit(1);
            }
            argvs[++i]=(char*)malloc(sizeof (char)*MAXPATH);
            p_arg=argvs[i];
            continue;
        }
        *p_arg++=temp;
    }
    int status=0;
    if(fork()==0){
        if(exec(argv[1],argvs)!=0){//执行命令
            fprintf(2,"xargs: exec %s failed\n",argv[1]);
            exit(1);
        }
        exit(0);
    }
    else wait(&status);
    exit(0);
}
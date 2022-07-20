//
// Created by 吴斐 on 2022/6/30.
//
#include "kernel/types.h"
#include "user/user.h"

int main(void){
    int nums[35] = {0};
    for (int i = 2; i < 36; ++i) {
        nums[i-2]=i;
    }
    nums[34]=-1;
    int descriptors[2];

    while(1){
        pipe(descriptors);
        int pid=fork();
        if(pid>0){
            printf("prime %d\n",nums[0]);
            close(descriptors[0]);
            int status=0;
            for (int i = 1;nums[i]!=-1 ; ++i) {//写入数据，结尾为-1
                if(nums[i]%nums[0]!=0 ){
                    if(write(descriptors[1],nums+i,sizeof (int))!=sizeof (int)){
                        printf("error");
                        close(descriptors[1]);
                        exit(1);
                    }
                }
            }
            close(descriptors[1]);
            wait(&status);//旧进程等fork后的进程完成后再exit
            exit(0);
        }
        if(pid==0){//新进程读取后通过循环到前面代码进行写入
            close(descriptors[1]);
            int temp=0;
            int i=0;
            while (1){
                if(read(descriptors[0],&temp,sizeof (int))==0) {
                    nums[i]=-1;
                    break;
                }
                else{
                    nums[i++]=temp;
                }
            }
            if(nums[0]==-1) exit(0);//没有数字了则结束
        }
    }
}
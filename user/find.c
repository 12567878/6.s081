//
// Created by 吴斐 on 2022/7/19.
//
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
//#include "string.h"

char*
fmtname(char *path)//输入路径，返回文件名或目录名
{
    static char buf[DIRSIZ+1];
    char *p;

    // Find first character after last slash.
    for(p=path+strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;

    // Return blank-padded name.
    if(strlen(p) >= DIRSIZ)
        return p;
    memmove(buf, p, strlen(p));
    memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
    return buf;
}

void
find(char *path, char *file)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

  if((fd = open(path, 0)) < 0){
    fprintf(2, "ls: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

    switch(st.type){//find第一个参数为路径就按照路径来，比较现有路径
        case T_FILE:
            if(strcmp(path, file)==0){
                printf("./%s",file);
            }
            break;

        case T_DIR:
            if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
                printf("find:a path is too long\n");
                break;
            }

            strcpy(buf, path);
            p = buf+strlen(buf);
            *p++ = '/';


            while(read(fd, &de, sizeof(de)) == sizeof(de)){//读取目录中文件的信息
                if(de.inum == 0)
                    continue;
                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;
                if(stat(buf, &st) < 0){
                    printf("find: cannot stat %s\n", buf);
                    continue;
                }


                switch (st.type) {
                    case T_FILE:
                        if(strcmp(de.name, file)==0){//比较文件名
                            printf("%s/%s\n",path,de.name);
                        }
                        break;
                    case T_DIR:
                        if(strcmp(de.name, ".")==0 || strcmp(de.name, "..")==0) {//对.和..不递归
                            continue;
                        }
                        find(buf,file);
                        break;
                }
            }
            break;
    }
    close(fd);
}

int main(int argc, char *argv[]){
    if(argc!=3){
        fprintf(2,"invalid nums of arguments, only receive 2.\n");
        exit(1);
    }

    find(argv[1],argv[2]);
    exit(0);
}


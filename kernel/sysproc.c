#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"



uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  backtrace();
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_sigalarm(void){
    int ntick;
//    uint64 addr_f;
    void(*handler)(void)=0;

    if(argint(0, &ntick) < 0)
        return -1;
    if(argaddr(1,(uint64 *)&handler)<0)
        return -1;

    myproc()->ntick=ntick;
    myproc()->handler=handler;
    return 0;
}

uint64
sys_sigreturn(void){

    myproc()->trapframe->a0=myproc()->a0;
    myproc()->trapframe->a1=myproc()->a1;
    myproc()->trapframe->a2=myproc()->a2;
    myproc()->trapframe->a3=myproc()->a3;
    myproc()->trapframe->a4=myproc()->a4;
    myproc()->trapframe->a5=myproc()->a5;
    myproc()->trapframe->a6=myproc()->a6;
    myproc()->trapframe->a7=myproc()->a7;

    myproc()->trapframe->t0=myproc()->t0;
    myproc()->trapframe->t1=myproc()->t1;
    myproc()->trapframe->t2=myproc()->t3;
    myproc()->trapframe->t3=myproc()->t4;
    myproc()->trapframe->t4=myproc()->t5;
    myproc()->trapframe->t5=myproc()->t6;
    myproc()->trapframe->t6=myproc()->t2;
    myproc()->trapframe->epc=myproc()->epc;
    myproc()->trapframe->sp=myproc()->sp;
    myproc()->trapframe->s0=myproc()->s0;
    myproc()->trapframe->ra=myproc()->ra;
    *(myproc()->handling)=0;

    return 0;
}
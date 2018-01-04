#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "container.h"
#include "fs.h"

#define NULL ((void*)0)


int
sys_fork(void)
{
  int x = find(myproc()->cont->name);
  if(x >= 0){
    int before = get_curr_proc(x);
    set_curr_proc(1, x);
    int after = get_curr_proc(x);
    if(after == before){
      cstop_container_helper(myproc()->cont);
      return -1;
    }
  }
  return fork();
}

int
sys_exit(void)
{
  int x = find(myproc()->cont->name);
  if(x >= 0){
    set_curr_proc(-1, x);
  }
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
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

int
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
  return 0;
}

void sys_cstop(){

  char* name;
  argstr(0, &name);

  if(myproc()->cont != NULL){
    struct container* cont = myproc()->cont;
    if(strlen(name) == strlen(cont->name) && strncmp(name, cont->name, strlen(name)) == 0){
      cstop_container_helper(cont);
      //stop the processes
    }
    else{
      cprintf("You are not authorized to do this.\n");
    }
  }
  else{
    cstop_helper(name);
  }

  //kill the processes with name as the id

}

void sys_set_root_inode(void){

  char* name;
  argstr(0,&name);

  set_root_inode(name);
  cprintf("success\n");

}

void sys_ps(void){

  struct container* cont = myproc()->cont;

  if(cont == NULL){
    procdump();
  }
  else{
    // cprintf("passing in %s as name for c_procdump.\n", cont->name);
    c_procdump(cont->name);
  }
}

void sys_container_init(){
  container_init();
}

int sys_is_full(void){
  return is_full();
}

int sys_find(void){
  char* name;
  argstr(0, &name);

  return find(name);
}

void sys_get_name(void){

  int vc_num;
  char* name;
  argint(0, &vc_num);
  argstr(1, &name);

  get_name(vc_num, name);
}

int sys_get_max_proc(void){
  int vc_num;
  argint(0, &vc_num);
  return get_max_proc(vc_num);  
}

int sys_get_os(void){
  return get_os();
}

int sys_get_max_mem(void){
  int vc_num;
  argint(0, &vc_num);


  return get_max_mem(vc_num);
}

int sys_get_max_disk(void){
  int vc_num;
  argint(0, &vc_num);


  return get_max_disk(vc_num);

}

int sys_get_curr_proc(void){
  int vc_num;
  argint(0, &vc_num);


  return get_curr_proc(vc_num);
}

int sys_get_curr_mem(void){
  int vc_num;
  argint(0, &vc_num);


  return get_curr_mem(vc_num);
}

int sys_get_curr_disk(void){
  int vc_num;
  argint(0, &vc_num);


  return get_curr_disk(vc_num);
}

void sys_set_name(void){
  char* name;
  argstr(0, &name);

  int vc_num;
  argint(1, &vc_num);

  // myproc()->cont = get_container(vc_num);
  // cprintf("succ");

  set_name(name, vc_num);
  //cprintf("Done setting name.\n");
}

void sys_cont_proc_set(void){

  int vc_num;
  argint(0, &vc_num);

  // cprintf("")

  // cprintf("before getting container\n");

  //So I can get the name, but I can't get the corresponding container
  // cprintf("In sys call proc set, container name is %s.\n", get_container(vc_num)->name);
  myproc()->cont = get_container(vc_num);
  // cprintf("MY proc container name = %s.\n", myproc()->cont->name);

  // cprintf("after getting container\n");
}

void sys_set_max_mem(void){
  int mem;
  argint(0, &mem);

  int vc_num;
  argint(1, &vc_num);

  set_max_mem(mem, vc_num);
}
void sys_set_os(void){
  int os;
  argint(0, &os);
  set_os(os);
}

void sys_set_max_disk(void){
  int disk;
  argint(0, &disk);

  int vc_num;
  argint(1, &vc_num);

  set_max_disk(disk, vc_num);
}

void sys_set_max_proc(void){
  int proc;
  argint(0, &proc);

  int vc_num;
  argint(1, &vc_num);

  set_max_proc(proc, vc_num);
}

void sys_set_curr_mem(void){
  int mem;
  argint(0, &mem);

  int vc_num;
  argint(1, &vc_num);

  set_curr_mem(mem, vc_num);
}

void sys_reduce_curr_mem(void){
  int mem;
  argint(0, &mem);

  int vc_num;
  argint(1, &vc_num);

  set_curr_mem(mem, vc_num);
}

void sys_set_curr_disk(void){
  int disk;
  argint(0, &disk);

  int vc_num;
  argint(1, &vc_num);

  set_curr_disk(disk, vc_num);
}

void sys_set_curr_proc(void){
  int proc;
  argint(0, &proc);

  int vc_num;
  argint(1, &vc_num);

  set_curr_proc(proc, vc_num);
}

void sys_container_reset(void){
  int vc_num;
  argint(1, &vc_num);
  container_reset(vc_num);
}
// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int
sys_getticks(void){
  return myproc()->ticks;
}

int sys_max_containers(void){
  return max_containers();
}


void sys_df(void){
  struct container* cont = myproc()->cont;
  struct superblock sb;
  readsb(1, &sb);

  int used = 0;
  if(cont == NULL){
    int max = max_containers();
    int i;
    for(i = 0; i < max; i++){
      used = used + (int)(get_curr_disk(i) / 1024);
      if(i == 0){
        used += (int)(get_os() / 1024);
      }
    }
    cprintf("~%d used out of %d available.\n", used, sb.nblocks);
  }
  else{
    int x = find(cont->name);
    int used = (int)(get_curr_disk(x) / 1024);
    cprintf("~%d used out of %d available.\n", used,  get_max_disk(x));
  }
}

void
sys_pause(void){
  char *name;
  argstr(0, &name);
  pause(name);
}

void
sys_resume(void){
  char *name;
  argstr(0, &name);
  resume(name);
}

int
sys_tmem(void){
  struct container* cont = myproc()->cont;

  if(cont == NULL){
    return mem_usage();
  }
  return get_curr_mem(find(cont->name));
}

int
sys_amem(void){
  struct container* cont = myproc()->cont;

  if(cont == NULL){
    return mem_avail();
  }
  return get_max_mem(find(cont->name));
}

void sys_c_ps(void){
  char *name;
  argstr(0, &name);
  c_proc_data(name);
  // c_procdump(name);
}

int sys_get_used(void){
  int x; 
  argint(0, &x);
  return get_used(x);
}

int sys_get_cticks(void){
  return get_cticks();
}

void sys_tick_reset2(void){
  tick_reset2();
}







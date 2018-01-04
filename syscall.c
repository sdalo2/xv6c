#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "syscall.h"

// User code makes a system call with INT T_SYSCALL.
// System call number in %eax.
// Arguments on the stack, from the user call to the C
// library system call function. The saved user %esp points
// to a saved program counter, and then the first argument.

// Fetch the int at addr from the current process.
int
fetchint(uint addr, int *ip)
{
  struct proc *curproc = myproc();

  if(addr >= curproc->sz || addr+4 > curproc->sz)
    return -1;
  *ip = *(int*)(addr);
  return 0;
}

// Fetch the nul-terminated string at addr from the current process.
// Doesn't actually copy the string - just sets *pp to point at it.
// Returns length of string, not including nul.
int
fetchstr(uint addr, char **pp)
{
  char *s, *ep;
  struct proc *curproc = myproc();

  if(addr >= curproc->sz)
    return -1;
  *pp = (char*)addr;
  ep = (char*)curproc->sz;
  for(s = *pp; s < ep; s++){
    if(*s == 0)
      return s - *pp;
  }
  return -1;
}

// Fetch the nth 32-bit system call argument.
int
argint(int n, int *ip)
{
  return fetchint((myproc()->tf->esp) + 4 + 4*n, ip);
}

// Fetch the nth word-sized system call argument as a pointer
// to a block of memory of size bytes.  Check that the pointer
// lies within the process address space.
int
argptr(int n, char **pp, int size)
{
  int i;
  struct proc *curproc = myproc();
 
  if(argint(n, &i) < 0)
    return -1;
  if(size < 0 || (uint)i >= curproc->sz || (uint)i+size > curproc->sz)
    return -1;
  *pp = (char*)i;
  return 0;
}

// Fetch the nth word-sized system call argument as a string pointer.
// Check that the pointer is valid and the string is nul-terminated.
// (There is no shared writable memory, so the string can't change
// between this check and being used by the kernel.)
int
argstr(int n, char **pp)
{
  int addr;
  if(argint(n, &addr) < 0)
    return -1;
  return fetchstr(addr, pp);
}

extern int sys_chdir(void);
extern int sys_close(void);
extern int sys_dup(void);
extern int sys_exec(void);
extern int sys_exit(void);
extern int sys_fork(void);
extern int sys_fstat(void);
extern int sys_getpid(void);
extern int sys_kill(void);
extern int sys_link(void);
extern int sys_mkdir(void);
extern int sys_mknod(void);
extern int sys_open(void);
extern int sys_pipe(void);
extern int sys_read(void);
extern int sys_sbrk(void);
extern int sys_sleep(void);
extern int sys_unlink(void);
extern int sys_wait(void);
extern int sys_write(void);
extern int sys_uptime(void);
extern int sys_getticks(void);
extern int sys_get_name(void);
extern int sys_get_max_proc(void);
extern int sys_get_max_mem(void);
extern int sys_get_max_disk(void);
extern int sys_get_curr_proc(void);
extern int sys_get_curr_mem(void);
extern int sys_get_curr_disk(void);
extern int sys_set_name(void);
extern int sys_set_max_mem(void);
extern int sys_set_max_disk(void);
extern int sys_set_max_proc(void);
extern int sys_set_curr_mem(void);
extern int sys_set_curr_disk(void);
extern int sys_set_curr_proc(void);
extern int sys_find(void);
extern int sys_is_full(void);
extern int sys_container_init(void);
extern int sys_cont_proc_set(void);
extern int sys_ps(void);
extern int sys_reduce_curr_mem(void);
extern int sys_set_root_inode(void);
extern int sys_cstop(void);
extern int sys_df(void);
extern int sys_max_containers(void);
extern int sys_container_reset(void);
extern int sys_pause(void);
extern int sys_resume(void);
extern int sys_tmem(void);
extern int sys_amem(void);
extern int sys_c_ps(void);
extern int sys_get_used(void);
extern int sys_get_os(void);
extern int sys_set_os(void);
extern int sys_get_cticks(void);
extern int sys_tick_reset2(void);


static int (*syscalls[])(void) = {
[SYS_fork]    sys_fork,
[SYS_exit]    sys_exit,
[SYS_wait]    sys_wait,
[SYS_pipe]    sys_pipe,
[SYS_read]    sys_read,
[SYS_kill]    sys_kill,
[SYS_exec]    sys_exec,
[SYS_fstat]   sys_fstat,
[SYS_chdir]   sys_chdir,
[SYS_dup]     sys_dup,
[SYS_getpid]  sys_getpid,
[SYS_sbrk]    sys_sbrk,
[SYS_sleep]   sys_sleep,
[SYS_uptime]  sys_uptime,
[SYS_open]    sys_open,
[SYS_write]   sys_write,
[SYS_mknod]   sys_mknod,
[SYS_unlink]  sys_unlink,
[SYS_link]    sys_link,
[SYS_mkdir]   sys_mkdir,
[SYS_close]   sys_close,
[SYS_getticks] sys_getticks,
[SYS_get_name] sys_get_name,
[SYS_get_max_proc] sys_get_max_proc,
[SYS_get_max_mem] sys_get_max_mem,
[SYS_get_max_disk] sys_get_max_disk,
[SYS_get_curr_proc] sys_get_curr_proc,
[SYS_get_curr_mem] sys_get_curr_mem,
[SYS_get_curr_disk] sys_get_curr_disk,
[SYS_set_name] sys_set_name,
[SYS_set_max_mem] sys_set_max_mem,
[SYS_set_max_disk] sys_set_max_disk,
[SYS_set_max_proc] sys_set_max_proc,
[SYS_set_curr_mem] sys_set_curr_mem,
[SYS_set_curr_disk] sys_set_curr_disk,
[SYS_set_curr_proc] sys_set_curr_proc,
[SYS_find] sys_find,
[SYS_is_full] sys_is_full,
[SYS_container_init] sys_container_init,
[SYS_cont_proc_set] sys_cont_proc_set,
[SYS_ps] sys_ps,
[SYS_reduce_curr_mem] sys_reduce_curr_mem,
[SYS_set_root_inode] sys_set_root_inode,
[SYS_cstop] sys_cstop,
[SYS_df] sys_df,
[SYS_max_containers] sys_max_containers,
[SYS_container_reset] sys_container_reset,
[SYS_pause] sys_pause,
[SYS_resume] sys_resume,
[SYS_tmem] sys_tmem,
[SYS_amem] sys_amem,
[SYS_c_ps] sys_c_ps,
[SYS_get_used] sys_get_used,
[SYS_get_os] sys_get_os,
[SYS_set_os] sys_set_os,
[SYS_get_cticks] sys_get_cticks,
[SYS_tick_reset2] sys_tick_reset2,
};

void
syscall(void)
{
  int num;
  struct proc *curproc = myproc();

  num = curproc->tf->eax;
  if(num > 0 && num < NELEM(syscalls) && syscalls[num]) {
    curproc->tf->eax = syscalls[num]();
  } else {
    cprintf("%d %s: unknown sys call %d\n",
            curproc->pid, curproc->name, num);
    curproc->tf->eax = -1;
  }
}

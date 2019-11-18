#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H


#include "threads/thread.h"

#include <stdbool.h>

typedef int pid_t;

void syscall_init (void);
void syscall_halt(void);
void syscall_exit(void);
void shutdown_power_off(void);
void close (int fd);
int syscall_wait (tid_t pid);
pid_t syscall_exec (const char *file);
int syscall_write (unsigned int *pntr);
int syscall_read (unsigned int *pntr);

bool syscall_create (const char *file, unsigned initial_size);

bool syscall_remove (const char *file);

int syscall_filesize (int fd);
//void syscall_seek (int fd, unsigned position);

//int syscall_open (const char *file);
//unsigned syscall_tell (int fd);
//void syscall_close (int fd);

int systemExec(const char *cmd_line);



#endif /* userprog/syscall.h */


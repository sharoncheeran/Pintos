/*=============================================================================
 |      Editors:  Martyn Fitzgerald - 16025948
 |                Sharon Cheeran    - 17012330
 |                Dawid Koleczko    - 17024154
 |
 |     Language:  C
 |     Compiler:  GCC
 |
 |  Module Code:  UFCFWK-15-2
 |        Class:  Operating Systems
 |                
 |   Instructor:  Dr Shancang Li
 |     Due Date:  06/12/2018
 |
 |  Description:  Adding the functionality all of the system calls
 |
 *===========================================================================*/
#include "userprog/syscall.h"
#include "lib/stdio.h"
#include "lib/kernel/stdio.h"
#include "lib/syscall-nr.h"
#include <string.h>
#include "threads/interrupt.h"
#include "threads/init.h"
#include "threads/malloc.h"
#include "threads/palloc.h"
#include "threads/vaddr.h"
#include "threads/flags.h"
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "userprog/process.h"
#include "userprog/pagedir.h"
#include "devices/input.h"
#include "devices/shutdown.h"


static void syscall_handler (struct intr_frame *);


char status;
void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}


static void
syscall_handler (struct intr_frame *f UNUSED)
{
  /* 
    Gets the system call number
  */
  uint32_t *pntr = f->esp; 

  /* 
    || Testing Purposes || Check whats inside the system stack
  */
  //printf("ESP %p\n", &pntr);
  //printf("ESP Value %d\n",(*(int *)(pntr)));
  //hex_dump(pntr,pntr,64, true);


  /* 
    Find starting memory space for user
  */
  if (*pntr < 0xbfffefec)
  {
    switch(*pntr){
      //System Call - Halt
      case SYS_HALT:{
        syscall_halt();
        break;
      }
      //System Call - Write
      case SYS_WRITE:{
        syscall_write (pntr);
        break;
      }
      //System Call - Read
      case SYS_READ:{
        syscall_read(pntr);
        break;
      }
      //System Call - Exit
      case SYS_EXIT:{
        syscall_exit();
        break;
      }
      //System Call - EXEC
      case SYS_EXEC:{
        syscall_exec(*(char**)(pntr + 8));
        break;  
      }
      //System Call - CREATE
      case SYS_CREATE:{
        syscall_create ((*(char**)*pntr + 8), (*(char**)*pntr + 8));
        break;  
      }
      //System Call - REMOVE
      case SYS_REMOVE:{
        syscall_remove(pntr + 8);
        break;
      }
      //If the cases are not selected, fall through to this one.
      default:
      printf ("SYS_CALL %p has not been implemented \n",pntr);
      thread_exit ();
    }
  }
  else 
  {
    printf("Out of memory\n"); //give kernel panic 
  }
}

/* 
  Halt System Call - Using the shutdown function
*/
void syscall_halt(void){
  printf("Pintos System Call: HALT has been implemented \n");
  shutdown_power_off();
}

/* 
  Exit System Call - Using the exit code status and printing to the console.
*/
void syscall_exit(void){
  struct thread * current = thread_current();
  current->exit_code = status;

  //Debugging Purposes 
  printf("Pintos System Call: EXIT has been implemented \n");
  printf("Current Thread { %s } Status { %d }\n", current->name, current->exit_code);

  //exit the thread
  thread_exit();
}

/* 
  WRITE System Call - Takes the pntr and works out the values of the fd, buffer and length.
*/
int syscall_write (unsigned int *pntr)
{
  int fd;
  const void *buffer;
  unsigned int length;

  pntr++;// +4
  fd = *pntr;
  
  pntr++;// +8
  buffer = (int *)*pntr;
  
  pntr++;// +12
  length = *pntr;
  
  if(fd == STDOUT_FILENO)
  {
    //output to the console 
    putbuf((const char*)buffer,(unsigned )length);
  }
  else
  {
    printf("Pintos System Call: WRITE does not support file descriptor output \n");
  }
  return 0;
}

/* 
  READ System Call - Takes the pntr and works out the values of the fd, buffer and length.
*/
int syscall_read (unsigned int *pntr) 
{
  int fd;
  const void *buffer;
  unsigned int length;

  pntr++;//+ 4
  fd = *pntr;
  
  pntr++;//+ 8
  buffer = (int *)*pntr;
  
  pntr++;//+ 12
  length = *pntr;

  //struct file* fd2 = fd; 

  if(fd == STDIN_FILENO)
  {
    printf("Pintos System Call: READ has been implemented \n");

    //Read the file contents using the file read() function from filesys.c
    file_read((struct file*)fd,(void*)buffer,(unsigned )length);

    //Prototype funcions
    //file_read(fd,buffer,length);
    // getbuf((const char*)buffer,(unsigned )length);
  }
  else
  {
    printf("Pintos System Call: READ does not support file descriptor output \n");
  }
  return 0;
}

/* 
  EXEC System Call - Takes the cmd_line and sends it to the process_execute.
*/
pid_t syscall_exec (const char *cmd_line)
{  
  printf("Pintos System Call: EXEC has been implemented \n");
  //call the process execute from process.c. with the *cmd line and assign to the pid
  return process_execute(cmd_line);
}

/* 
  WAIT System Call - Takes the pid that we send to the process_wait
*/
int syscall_wait (tid_t pid)
{
  printf("Pintos System Call: WAIT has been implemented \n");

  //Need to implement process_wait in process.c 
  return process_wait(pid);
}

/* 
  REMOVE System Call - Takes the file and sends it to the filesys_Remove
*/
bool syscall_remove (const char *file)
{
  printf("Pintos System Call: REMOVE has been implemented \n");

  //Call the function fielsys_remove from filesys.c .to remove the file 
  return filesys_remove(file);
}

/* 
  CREATE System Call - Takes the file and the size of the initial_size and send it to the filesys_create
*/
bool syscall_create (const char *file, unsigned initial_size)
{
  printf("Pintos System Call: CREATE has been implemented \n");

  //create the file using the file filesys_create called from filesys.c
  return filesys_create(file, initial_size);
}

/* 
  Filesize System Call - Takes the fd into the function and sends it to the file_length
*/
int syscall_filesize (int fd)
{
  printf("Pintos System Call: FILESIZE has been implemented \n");


  //attempt to grab the file length  
  //file_length(fd);

  //return the fd
  return fd;
}



/*

//In progress attempted but not complete

void close (int fd)
{
  printf("Pintos System Call: CLOSE has been implemented\n");


  struct file_descriptor *file_desc = fd;

  //struct file* f = file;
  
//Need tp find the file and close it
  //file_close(file_desc->file);
  free(file_desc);



}

//use file_seek()
void syscall_seek (int fd, unsigned position)
{
  printf("Pintos System Call: SEEK has been NOT implemented\n");

}
//use file_0pen()
int syscall_open (const char *file)
{
  printf("Pintos System Call: OPEN has been NOT implemented\n");
  return 1;

}

//use file_tell()
unsigned syscall_tell (int fd)
{
  printf("Pintos System Call: TELL has been NOT implemented\n");
  return fd;

}

*/


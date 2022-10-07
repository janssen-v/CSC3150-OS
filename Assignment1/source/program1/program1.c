#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void handle_sigchld(int sig) {
  printf("Parent process receives SIGCHLD signal\n");
}

int main(int argc, char *argv[]) {

  printf("Process fork start\n");

  pid_t pid = fork();
  int exit_status;

  if (pid == -1) {
    perror("Failed to fork");
    exit(EXIT_FAILURE);

  } else if (pid > 0) {
    // This is the parent process
    printf("I'm the Parent Process, my pid = %d\n", getpid());
    // Wait for child process to terminate
    signal(SIGCHLD, handle_sigchld);
    waitpid(-1, &exit_status, WUNTRACED);

  } else {
    // This is the child process
    printf("I'm the Child Process, my pid = %d\n", getpid());
    // Execute test program exec()
    execv(argv[1], argv);
    printf("Original child process is running!\n");
  }

  // Print termination status of child process (Normal)
  if (WIFEXITED(exit_status)) {
    printf("Normal termination with EXIT STATUS =  %d\n",
           WEXITSTATUS(exit_status));
    // SIGSTOP handling
  } else if (WIFSTOPPED(exit_status)) {
    printf("CHILD PROCESS STOPPED\nChild raised SIGSTOP signal\n");
    // Print how child process terminates if not normal
  } else if (WIFSIGNALED(exit_status)) {
    psignal(WTERMSIG(exit_status), "Child terminated due to ");
  }

  // Print out signal raised in child process

  switch (exit_status) {
  case 1:
    // SIGHUP
    printf("Child raised SIGHUP signal\n");
    break;
  case 2:
    // SIGINT
    printf("Child raised SIGINT signal\n");
    break;
  case 3:
    // SIGQUIT
    printf("Child raised SIGQUIT signal\n");
    break;
  case 4:
    // SIGILL
    printf("Child raised SIGILL signal\n");
    break;
  case 5:
    // SIGTRAP
    printf("Child raised SIGTRAP signal\n");
    break;
  case 6:
    // SIGABRT
    printf("Child raised SIGABRT signal\n");
    break;
  case 7:
    // SIGBUS
    printf("Child raised SIGBUS signal\n");
    break;
  case 8:
    // SIGFPE
    printf("Child raised SIGFPE signal\n");
    break;
  case 9:
    // SIGKILL [cannot be caught, blocked or ignored]
    printf("Child raised SIGKILL signal\n");
    break;
  case 10:
    // SIGUSR1
    printf("Child raised SIGUSR1 signal\n");
    break;
  case 11:
    // SIGSEGV
    printf("Child raised SIGSEGV signal\n");
    break;
  case 12:
    // SIGUSR2
    printf("Child raised SIGUSR2 signal\n");
    break;
  case 13:
    // SIGPIPE
    printf("Child raised SIGPIPE signal\n");
    break;
  case 14:
    // SIGALRM
    printf("Child raised SIGALRM signal\n");
    break;
  case 15:
    // SIGTERM
    printf("Child raised SIGTERM signal\n");
    break;
  case 16:
    // SIGSTKFLT
    printf("Child raised SIGSTKFLT signal\n");
    break;
  case 17:
    // SIGCHLD
    printf("Child raised SIGCHLD signal\n");
    break;
  case 18:
    // SIGCONT
    printf("Child raised SIGCONT signal\n");
    break;
  case 19:
    // SIGSTOP [cannot be caught, blocked or ignored]
    printf("Child raised SIGSTOP signal\n");
    break;
  case 20:
    // SIGTSTP
    printf("Child raised SIGTSTP signal\n");
    break;
  case 21:
    // SIGTTIN
    printf("Child raised SIGTTIN signal\n");
    break;
  case 22:
    // SIGTTOU
    printf("Child raised SIGTTOU signal\n");
    break;
  case 23:
    // SIGURG
    printf("Child raised SIGURG signal\n");
    break;
  case 24:
    // SIGXCPU
    printf("Child raised SIGXCPU signal\n");
    break;
  case 25:
    // SIGXFSZ
    printf("Child raised SIGXFSZ signal\n");
    break;
  case 26:
    // SIGVTALRM
    printf("Child raised SIGVTALRM signal\n");
    break;
  case 27:
    // SIGPROF
    printf("Child raised SIGPROF signal\n");
    break;
  case 28:
    // SIGWINCH
    printf("Child raised SIGWINCH signal\n");
    break;
  case 29:
    // SIGIO
    printf("Child raised SIGIO signal\n");
    break;
  case 30:
    // SIGPWR
    printf("Child raised SIGPWR signal\n");
    break;
  case 31:
    // SIGSYS
    printf("Child raised SIGSYS signal\n");
    break;
  }

  printf("\n");
  /* fork a child process */

  /* execute test program */

  /* wait for child process terminates */

  /* check child process'  termination status */
  return 0;
}

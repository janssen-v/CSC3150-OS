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
  }
	// Print termination status of child process (Normal)
  if (WIFEXITED(exit_status)) {
    printf("Normal termination with EXIT STATUS =  %d\n",
           WEXITSTATUS(exit_status));
	// SIGSTOP handling
  } else if (WIFSTOPPED(exit_status)) {
    printf("CHILD PROCESS STOPPED\n");
	// Print how child process terminates if not normal
  } else if (WIFSIGNALED(exit_status)) {
    psignal(WTERMSIG(exit_status), "Child terminated due to ");
  }
	// Print out signal raised in child process

  printf("\n");
  /* fork a child process */

  /* execute test program */

  /* wait for child process terminates */

  /* check child process'  termination status */
  return 0;
}

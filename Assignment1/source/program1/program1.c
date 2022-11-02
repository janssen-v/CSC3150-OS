#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void handle_sigchld(int sig)
{
	printf("Parent process receives SIGCHLD signal\n");
}

int main(int argc, char *argv[])
{
	printf("Process start to fork\n");

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
		printf("Child process get SIGSTOP signal\n");
		// Print how child process terminates if not normal
	} else if (WIFSIGNALED(exit_status)) {
		psignal(WTERMSIG(exit_status), "Child terminated due to ");
		//printf("WTERMSIG OUTPUT = %d\n", WTERMSIG(exit_status));
	}

	// Print out signal raised in child process

	switch (WTERMSIG(exit_status)) {
	case 1:
		// SIGHUP
		printf("Child process get SIGHUP signal\n");
		break;
	case 2:
		// SIGINT
		printf("Child process get SIGINT signal\n");
		break;
	// case 131: // 131, originally 3
	// 	// SIGQUIT
	// 	printf("Child raised SIGQUIT signal\n");		When WTERMSIG is not used to evaluate exit_status, the values do not
	// 	break;											always match POSIX signal numbering, although they were consistent.
	// case 132: // 132, originally 4
	// 	// SIGILL
	// 	printf("Child raised SIGILL signal\n");
	// 	break;
	// case 133: // 133, originally 5
	// 	// SIGTRAP
	// 	printf("Child raised SIGTRAP signal\n");
	// 	break;
	// case 134: // 134, originally 6
	// 	// SIGABRT
	// 	printf("Child raised SIGABRT signal\n");
	// 	break;
	// case 135: // 135, originally 7
	// 	// SIGBUS
	// 	printf("Child raised SIGBUS signal\n");
	// 	break;
	// case 136: // 136, originally 8
	// 	// SIGFPE
	// 	printf("Child raised SIGFPE signal\n");
	// 	break;
	//  case 139: // 139, originally 11
	//	// SIGSEGV
	//	printf("Child raised SIGSEGV signal\n");
	//	break;
	case 3: // 131
		// SIGQUIT
		printf("Child process get SIGQUIT signal\n");
		break;
	case 4: // 132
		// SIGILL
		printf("Child process get SIGILL signal\n");
		break;
	case 5: // 133
		// SIGTRAP
		printf("Child process get SIGTRAP signal\n");
		break;
	case 6: // 134
		// SIGABRT
		printf("Child process get SIGABRT signal\n");
		break;
	case 7: // 135
		// SIGBUS
		printf("Child process get SIGBUS signal\n");
		break;
	case 8: // 136
		// SIGFPE
		printf("Child process get SIGFPE signal\n");
		break;
	case 9:
		// SIGKILL [cannot be caught, blocked or ignored]
		printf("Child process get SIGKILL signal\n");
		break;
	case 10:
		// SIGUSR1
		printf("Child process get SIGUSR1 signal\n");
		break;
	case 11: // 139
		// SIGSEGV
		printf("Child process get SIGSEGV signal\n");
		break;
	case 12:
		// SIGUSR2
		printf("Child process get SIGUSR2 signal\n");
		break;
	case 13:
		// SIGPIPE
		printf("Child process get SIGPIPE signal\n");
		break;
	case 14:
		// SIGALRM
		printf("Child process get SIGALRM signal\n");
		break;
	case 15:
		// SIGTERM
		printf("Child process get SIGTERM signal\n");
		break;
	case 16:
		// SIGSTKFLT
		printf("Child process get SIGSTKFLT signal\n");
		break;
	case 17:
		// SIGCHLD
		printf("Child process get SIGCHLD signal\n");
		break;
	case 18:
		// SIGCONT
		printf("Child process get SIGCONT signal\n");
		break;
	// case 19: UNUSED
	// 	// SIGSTOP [cannot be caught, blocked or ignored]
	// 	printf("Child process get SIGSTOP signal\n");
	// 	break;
	case 20:
		// SIGTSTP
		printf("Child process get SIGTSTP signal\n");
		break;
	case 21:
		// SIGTTIN
		printf("Child process get SIGTTIN signal\n");
		break;
	case 22:
		// SIGTTOU
		printf("Child process get SIGTTOU signal\n");
		break;
	case 23:
		// SIGURG
		printf("Child process get SIGURG signal\n");
		break;
	case 24:
		// SIGXCPU
		printf("Child process get SIGXCPU signal\n");
		break;
	case 25:
		// SIGXFSZ
		printf("Child process get SIGXFSZ signal\n");
		break;
	case 26:
		// SIGVTALRM
		printf("Child process get SIGVTALRM signal\n");
		break;
	case 27:
		// SIGPROF
		printf("Child process get SIGPROF signal\n");
		break;
	case 28:
		// SIGWINCH
		printf("Child process get SIGWINCH signal\n");
		break;
	case 29:
		// SIGIO
		printf("Child process get SIGIO signal\n");
		break;
	case 30:
		// SIGPWR
		printf("Child process get SIGPWR signal\n");
		break;
	case 31:
		// SIGSYS
		printf("Child process get SIGSYS signal\n");
		break;
	}

	printf("\n");
	return 0;
}

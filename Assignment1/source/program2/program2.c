#include <linux/err.h>
#include <linux/fs.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/kmod.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/pid.h>
#include <linux/printk.h>
#include <linux/sched.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");

static struct task_struct *task;

// MACROS FOR SIGNAL STATUS
#define __WEXITSTATUS(status) (((status)&0xff00) >> 8)
/* If WIFSIGNALED(STATUS), the terminating signal.  */
#define __WTERMSIG(status) ((status)&0x7f)
/* If WIFSTOPPED(STATUS), the signal that stopped the child.  */
#define __WSTOPSIG(status) __WEXITSTATUS(status)
/* Nonzero if STATUS indicates normal termination.  */
#define __WIFEXITED(status) (__WTERMSIG(status) == 0)
/* Nonzero if STATUS indicates termination by a signal.  */
#define __WIFSIGNALED(status) (((signed char)(((status)&0x7f) + 1) >> 1) > 0)
/* Nonzero if STATUS indicates the child is stopped.  */
#define __WIFSTOPPED(status) (((status)&0xff) == 0x7f)

struct wait_opts {
	enum pid_type wo_type;
	int wo_flags; // Wait options -> WUNTRACED, maybe WEXITED
	struct pid *wo_pid; // Kernel internal PID
	struct waitid_info *wo_info; // Signal info
	int wo_stat; // Child process termination status
	struct rusage *wo_rusage;
	wait_queue_entry_t child_wait; // Wait queue
	int notask_error;
};

extern long do_wait(struct wait_opts *wo);
extern pid_t kernel_clone(struct kernel_clone_args *args);
extern pid_t kernel_thread(int (*fn)(void *), void *arg, unsigned long flags);
extern struct filename *getname_kernel(const char *filename);
extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);

// implement exec function

int my_exec(void)
{
	const char path[] = "/tmp/test";
	//const char path[] = "/home/vagrant/code/CSC3150-OS/Assignment1/source/program2/test";  fortesting
	int ret;
	struct filename *filename = getname_kernel(path);
	ret = do_execve(filename, NULL, NULL);
	if (!ret) {
		return 0;
	}
	do_exit(ret);
}

// implement wait function

void my_wait(pid_t pid)
{
	enum pid_type type;
	type = PIDTYPE_PID; // Ref. /linux/pid.h
	int exit_status;
	struct pid *wo_pid = NULL; // Struct PID -> kernel internal PID
	wo_pid = find_get_pid(pid);
	struct wait_opts wo;
	wo.wo_type = type;
	wo.wo_flags = WEXITED | WUNTRACED;
	wo.wo_pid = wo_pid;
	wo.wo_info = NULL;
	wo.wo_stat = exit_status;
	wo.wo_rusage = NULL;

	// Wait for child
	printk("[program2] : Child process\n");
	int retval = do_wait(&wo);

	// Termination Status Handling
	exit_status = wo.wo_stat;
	// printk("[program2] : EXIT STATUS RAW : %d\n", exit_status);
	if (__WIFEXITED(exit_status)) {
		// Normal Termination
		printk("[program2] : Child process terminated normally with EXIT STATUS = "
		       "%d\n",
		       __WEXITSTATUS(exit_status));
		printk("[program2] : The return signal is : %d\n",
		       __WTERMSIG(exit_status));
	} else if (__WIFSTOPPED(exit_status)) {
		// SIGSTOP Handling
		printk("[program2] : Child process is stopped by SIGSTOP signal\n");
	} else if (__WIFSIGNALED(exit_status)) {
		// Print signal raised in child
		switch (__WTERMSIG(exit_status)) {
		case 1:
			// SIGHUP
			printk("[program2] : Child process get SIGHUP signal\n");
			break;
		case 2:
			// SIGINT
			printk("[program2] : Child process get SIGINT signal\n");
			break;
		case 3: // 131
			// SIGQUIT
			printk("[program2] : Child process get SIGQUIT signal\n");
			break;
		case 4: // 132
			// SIGILL
			printk("[program2] : Child process get SIGILL signal\n");
			break;
		case 5: // 133
			// SIGTRAP
			printk("[program2] : Child process get SIGTRAP signal\n");
			break;
		case 6: // 134
			// SIGABRT
			printk("[program2] : Child process get SIGABRT signal\n");
			break;
		case 7: // 135
			// SIGBUS
			printk("[program2] : Child process get SIGBUS signal\n");
			break;
		case 8: // 136
			// SIGFPE
			printk("[program2] : Child process get SIGFPE signal\n");
			break;
		case 9:
			// SIGKILL [cannot be caught, blocked or ignored]
			printk("[program2] : Child process get SIGKILL signal\n");
			break;
		case 10:
			// SIGUSR1
			printk("[program2] : Child process get SIGUSR1 signal\n");
			break;
		case 11: // 139
			// SIGSEGV
			printk("[program2] : Child process get SIGSEGV signal\n");
			break;
		case 12:
			// SIGUSR2
			printk("[program2] : Child process get SIGUSR2 signal\n");
			break;
		case 13:
			// SIGPIPE
			printk("[program2] : Child process get SIGPIPE signal\n");
			break;
		case 14:
			// SIGALRM
			printk("[program2] : Child process get SIGALRM signal\n");
			break;
		case 15:
			// SIGTERM
			printk("[program2] : Child process get SIGTERM signal\n");
			break;
		case 16:
			// SIGSTKFLT
			printk("[program2] : Child process get SIGSTKFLT signal\n");
			break;
		case 17:
			// SIGCHLD
			printk("[program2] : Child process get SIGCHLD signal\n");
			break;
		case 18:
			// SIGCONT
			printk("[program2] : Child process get SIGCONT signal\n");
			break;
		case 20:
			// SIGTSTP
			printk("[program2] : Child process get SIGTSTP signal\n");
			break;
		case 21:
			// SIGTTIN
			printk("[program2] : Child process get SIGTTIN signal\n");
			break;
		case 22:
			// SIGTTOU
			printk("[program2] : Child process get SIGTTOU signal\n");
			break;
		case 23:
			// SIGURG
			printk("[program2] : Child process get SIGURG signal\n");
			break;
		case 24:
			// SIGXCPU
			printk("[program2] : Child process get SIGXCPU signal\n");
			break;
		case 25:
			// SIGXFSZ
			printk("[program2] : Child process get SIGXFSZ signal\n");
			break;
		case 26:
			// SIGVTALRM
			printk("[program2] : Child process get SIGVTALRM signal\n");
			break;
		case 27:
			// SIGPROF
			printk("[program2] : Child process get SIGPROF signal\n");
			break;
		case 28:
			// SIGWINCH
			printk("[program2] : Child process get SIGWINCH signal\n");
			break;
		case 29:
			// SIGIO
			printk("[program2] : Child process get SIGIO signal\n");
			break;
		case 30:
			// SIGPWR
			printk("[program2] : Child process get SIGPWR signal\n");
			break;
		case 31:
			// SIGSYS
			printk("[program2] : Child process get SIGSYS signal\n");
			break;
		}
		printk("[program2] : Child process terminated\n");
		printk("[program2] : The return signal is : %d\n",
		       __WTERMSIG(exit_status));
	}

	put_pid(wo_pid);
	return;
}

// implement fork function

int my_fork(void *argc)
{
	// set default sigaction for current process
	int i;
	struct k_sigaction *k_action = &current->sighand->action[0];
	for (i = 0; i < _NSIG; i++) {
		k_action->sa.sa_handler = SIG_DFL;
		k_action->sa.sa_flags = 0;
		k_action->sa.sa_restorer = NULL;
		sigemptyset(&k_action->sa.sa_mask);
		k_action++;
	}

	/* fork a process using kernel_clone or kernel_thread */

	struct kernel_clone_args args = {
		.flags = SIGCHLD, // Bit mask
		.child_tid = NULL, // Where to store child TID in child mem
		.parent_tid = NULL, // Where to store child TID in parent mem
		.exit_signal = SIGCHLD, // Signal to parent on child term
		.stack = (unsigned long)&my_exec, // Ptr to stack of execd fn
		.stack_size = 0,
		.tls = 0
	};

	/* execute a test program in child process */
	/* wait until child process terminates */

	pid_t child_pid;
	child_pid = kernel_clone(&args);
	if (child_pid == 0) {
		my_exec();
	} else {
		printk("[program2] : The child process has pid = %d\n",
		       child_pid);
		printk("[program2] : This is the parent process, pid = %d\n",
		       current->pid);
		my_wait(child_pid);
	}

	return 0;
};

static int __init program2_init(void)
{
	printk("[program2] : Module_init {Vincentius Janssen} {119010518}\n");
	printk("[program2] : Module_init create kthread start\n");
	// Create a kthread
	task = kthread_create(&my_fork, NULL, "MyThread");
	// Wake up kthread if functioning
	if (!IS_ERR(task)) {
		printk("[program2] : Module_init kthread start\n");
		wake_up_process(task);
	}

	/* create a kernel thread to run my_fork */

	return 0;
}

static void __exit program2_exit(void)
{
	printk("[program2] : Module_exit\n");
}

module_init(program2_init);
module_exit(program2_exit);

#include <linux/module.h>
#include <linux/sched.h>
#include <linux/pid.h>
#include <linux/kthread.h>
#include <linux/kernel.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/printk.h>
#include <linux/jiffies.h>
#include <linux/kmod.h>
#include <linux/fs.h>

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
	int wo_flags; // Wait options
	struct pid *wo_pid; // Kernel internal P
	struct waitid_info *wo_info; // Signal info
	int wo_stat; // Child process termination status
	struct rusage *wo_rusage; // Resource usage
	wait_queue_entry_t child_wait; // Task wait queue
	int notask_error;
};

extern long do_wait(struct wait_opts *wo);
extern pid_t kernel_clone(struct kernel_clone_args *args);
extern pid_t kernel_thread(int (*fn)(void *), void *arg, unsigned long flags);
extern struct filename *getname_kernel(const char *filename);
extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);

//implement exec function

int my_exec(void)
{
	const char path[] = "/tmp/test";
	int ret;
	struct filename *filename = getname_kernel(path);
	ret = do_execve(filename, NULL, NULL);
	if (!ret) {
		return 0;
	}
	do_exit(ret);
}

//implement wait function

void my_wait(pid_t pid)
{
	enum pid_type type;
	struct pid *wo_pid = NULL; // Struct PID -> kernel internal PID
	type = PIDTYPE_PID; // Ref. /linux/pid.h
	int exit_status;
	struct wait_opts wo = {.wo_type = type,
			       .wo_flags = WEXITED | WUNTRACED,
			       .wo_pid = wo_pid,
			       .wo_info = NULL,
			       .wo_stat = exit_status,
			       .wo_rusage = NULL };

	// Wait for child
	int retval = do_wait(&wo);

	// Termination Status Handling
	if (__WIFEXITED(exit_status)) {
		// Normal Termination
		printk("[program2] : Child process terminated\n");
		printk("[program2] : The return signal is : %d\n",
		       __WTERMSIG(exit_status));
	} else if (__WIFSTOPPED(exit_status)) {
		// SIGSTOP Handling
		printk("[program2] : Child process is stopped\n");
	} else if (__WIFSIGNALED(exit_status)) {
		// Print signal raised in child
		printk("[program2] : The return signal is : %d\n",
		       __WTERMSIG(exit_status));
	}

	put_pid(wo_pid);
	return;
}

//implement fork function

int my_fork(void *argc)
{
	//set default sigaction for current process
	int i;
	struct k_sigaction *k_action = &current->sighand->action[0];
	for (i = 0; i < _NSIG; i++) {
		k_action->sa.sa_handler = SIG_DFL;
		k_action->sa.sa_flags = 0;
		k_action->sa.sa_restorer = NULL;
		sigemptyset(&k_action->sa.sa_mask);
		k_action++;
	};

	/* fork a process using kernel_clone or kernel_thread */

	struct kernel_clone_args args = {
		.flags = SIGCHLD, // Bit mask
		.child_tid = NULL, // Where to store child TID in child mem
		.parent_tid = NULL, // Where to store child TID in parent mem
		.exit_signal = SIGCHLD, // Signal to parent on child term
		.stack = (unsigned long)&my_exec, // Ptr to stack of execd fn
		.stack_size = 0,
		.tls = 0,
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
		//wake_up_process(task);
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

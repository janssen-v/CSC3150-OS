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


struct wait_opts {
	enum pid_type wo_type;
	int	wo_flags;					// Wait options
	struct pid *wo_pid;				// Kernel internal P
	struct waitid_info *wo_info;	// Signal info
	int	wo_stat;					// Child process termination status
	struct rusage *wo_rusage;		// Resource usa
	wait_queue_entry_t child_wait;	// Task wait queue
	int	notask_error;
};

//long do_wait(struct wait_opts *wo);

//implement fork function

static struct task_struct *task;

int my_fork(void *argc) {
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

		/* execute a test program in child process */

		/* wait until child process terminates */

		return 0;
};



static int __init program2_init(void)
{
	printk("[program2] : Module_init {Vincentius Janssen} {119010518}\n");
	printk("[program2] : Module_init create kthread start\n");
	// Create a kthread
	task = kthread_create(&my_fork, NULL, "Parent Thread");
	// Wake up kthread if functioning
	if(!IS_ERR(task)){
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

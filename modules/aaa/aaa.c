#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/module.h>

static long my_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	void *parg = (void *)arg;
	pr_warn("%s\n", __func__);
	print_hex_dump(KERN_DEBUG, "D ", DUMP_PREFIX_ADDRESS, 16, 1,
		       parg, 128, true);
	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = my_ioctl,
};

static struct miscdevice misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "klp",
	.fops = &fops,
};

static int init1(void)
{
	char *mb = (void *)0xffffffff95e74b70;
	pr_info("sz=%.10s\n", mb);
	return -EINVAL;
	return misc_register(&misc);
}

static void exit1(void)
{
	misc_deregister(&misc);
}

module_init(init1);
module_exit(exit1);

MODULE_LICENSE("GPL");

#include <linux/debugfs.h>
#include <linux/fs.h>
#include <linux/ftrace.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>

#include <linux/capability.h>

static notrace void cb1(unsigned long ip, unsigned long parent_ip,
		struct ftrace_ops *op, struct pt_regs *regs)
{
	pr_info("%s %pF %pF %p\n", __func__, (void *)ip, (void *)parent_ip, regs);
}

static notrace void cb2(unsigned long ip, unsigned long parent_ip,
		struct ftrace_ops *op, struct pt_regs *regs)
{
	pr_info("%s %pF %pF %p\n", __func__, (void *)ip, (void *)parent_ip, regs);
}

static ssize_t filter_read(struct file *filp, char __user *buf, size_t cnt,
		loff_t *offp)
{
	return 0;
}

static ssize_t filter_write(struct file *filp, const char __user *buf,
		size_t cnt, loff_t *offp)
{
	static struct ftrace_ops ops1 = {
		.func = cb1,
	}, ops2 = {
		.func = cb2,
	};
	int ret = 0;
	char x;

	if (get_user(x, buf))
		return -EFAULT;

	switch (x) {
	case '1':
		ret = ftrace_set_filter_ip(&ops1, (unsigned long)filter_read, 0, 0);
		break;
	case '2':
		ret = ftrace_set_filter_ip(&ops2, (unsigned long)filter_read, 0, 0);
		break;
	case '8':
		ret = ftrace_set_filter_ip(&ops1, (unsigned long)filter_read, 1, 0);
		break;
	case '9':
		ret = ftrace_set_filter_ip(&ops2, (unsigned long)filter_read, 1, 0);
		break;
	case 'a':
		ret = register_ftrace_function(&ops1);
		break;
	case 'b':
		ret = register_ftrace_function(&ops2);
		break;
	case 'x':
		ret = unregister_ftrace_function(&ops1);
		break;
	case 'y':
		ret = unregister_ftrace_function(&ops2);
		break;
	default:
		return -EINVAL;
	}

	return ret ? ret : cnt;
}

static struct file_operations filter_fops = {
	.owner = THIS_MODULE,
	.open =	simple_open,
	.read = filter_read,
	.write = filter_write,
	.llseek = no_llseek,
};

static struct dentry *dir;

static int __init ftr_test_init(void)
{
	dir = debugfs_create_dir("ftr_test", NULL);
	if (!dir)
		goto err;

	if (!debugfs_create_file("filter", 0600, dir, NULL, &filter_fops))
		goto err_rm;

	return 0;
err_rm:
	debugfs_remove_recursive(dir);
err:
	return -EIO;
}

static void __exit ftr_test_exit(void)
{
	debugfs_remove_recursive(dir);
}

module_init(ftr_test_init);
module_exit(ftr_test_exit);

MODULE_LICENSE("GPL");


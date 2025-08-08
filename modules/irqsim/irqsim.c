// SPDX-License-Identifier: GPL-2.0-or-later

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/cleanup.h>
#include <linux/debugfs.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/irq_sim.h>
#include <linux/module.h>
#include <linux/nmi.h>

static struct dentry *debug_dentry;
static struct fwnode_handle *fwnode;
static struct irq_domain *irqd;

static const unsigned irqs = 128;
static bool irq_verbose;
static u32 write_count = 1;

static ssize_t gen_irq_write(struct file *filp, const char __user *buf, size_t count, loff_t *off)
{
	DECLARE_BITMAP(irq_bm, irqs);
	int ret = bitmap_parselist_user(buf, count, irq_bm, irqs);
	if (ret)
		return ret;

	for (unsigned i = 0; i < write_count; i++) {
		unsigned hwirq;

		for_each_set_bit(hwirq, irq_bm, irqs) {
			int irq = irq_find_mapping(irqd, hwirq);
			if (!irq)
				return count;

			irq_set_irqchip_state(irq, IRQCHIP_STATE_PENDING, true);
			if (irq_verbose)
				printk(KERN_DEBUG "%s: hwirq=%u irq=%u\n", __func__, hwirq, irq);
		}
		if (!(i % 10000))
			touch_softlockup_watchdog();
	}

	return count;
}

static struct file_operations fops = {
	.write = gen_irq_write,
};

static irqreturn_t irq_han(int irq, void *)
{
	if (irq_verbose)
		printk(KERN_DEBUG "%s: irq=%d\n", __func__, irq);
	return IRQ_HANDLED;
}

static int __init irqsim_init(void)
{
	debug_dentry = debugfs_create_dir("irqsim", NULL);
	if (IS_ERR(debug_dentry))
		return PTR_ERR(debug_dentry);
	debugfs_create_file("gen_irq", 0600, debug_dentry, NULL, &fops);
	debugfs_create_u32("write_count", 0600, debug_dentry, &write_count);
	debugfs_create_bool("verbose", 0600, debug_dentry, &irq_verbose);

	int ret;
	fwnode = fwnode_create_software_node(NULL, NULL);
	if (IS_ERR(fwnode)) {
		ret = PTR_ERR(fwnode);
		goto err_debugfs;
	}

	irqd = irq_domain_create_sim_full(fwnode, irqs, NULL, NULL);
	if (IS_ERR(irqd)) {
		ret = PTR_ERR(irqd);
		goto err_fwnode;
	}

	unsigned hwirq;
	for (hwirq = 0; hwirq < irqs; hwirq++) {
		if (!irq_create_mapping(irqd, hwirq)) {
			ret = -EIO;
			goto err_irqd;
		}

		unsigned irq = irq_find_mapping(irqd, hwirq);
		ret = request_irq(irq, irq_han, 0, "sim-irq", NULL);
		if (ret) {
			irq_dispose_mapping(irq_find_mapping(irqd, hwirq));
			goto err_mapping;
		}
	}

	pr_info("%s: irqs=%u-%u\n", __func__, irq_find_mapping(irqd, 0),
		irq_find_mapping(irqd, irqs - 1));

	return 0;
err_mapping:
	for (; hwirq > 0; hwirq--) {
		unsigned irq = irq_find_mapping(irqd, hwirq - 1);
		free_irq(irq, NULL);
		irq_dispose_mapping(irq_find_mapping(irqd, hwirq - 1));
	}
err_irqd:
	irq_domain_remove_sim(irqd);
err_fwnode:
	fwnode_remove_software_node(fwnode);
err_debugfs:
	debugfs_remove(debug_dentry);
	return ret;
}
module_init(irqsim_init);

static void __exit irqsim_exit(void)
{
	for (unsigned hwirq = 0; hwirq < irqs; hwirq++) {
		unsigned irq = irq_find_mapping(irqd, hwirq);
		free_irq(irq, NULL);
		irq_dispose_mapping(irq_find_mapping(irqd, hwirq));
	}

	irq_domain_remove_sim(irqd);
	fwnode_remove_software_node(fwnode);
	debugfs_remove(debug_dentry);
}
module_exit(irqsim_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("IRQ SIM");

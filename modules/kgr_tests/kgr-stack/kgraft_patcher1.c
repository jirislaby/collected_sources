#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kgraft.h>

#include <linux/capability.h>

static bool kgr_new_capable(int cap)
{
	printk(KERN_DEBUG "kgr-patcher: this is even newer capable()\n");

	return ns_capable(&init_user_ns, cap);
}

static struct kgr_patch patch = {
	.name = "sample_patcher1",
	.owner = THIS_MODULE,
	.replace_all = true,
	.patches = {
		KGR_PATCH(capable, kgr_new_capable, true),
		KGR_PATCH_END
	}
};

static int __init kgr_patcher_init(void)
{
	return kgr_patch_kernel(&patch);
}

static void __exit kgr_patcher_cleanup(void)
{
	kgr_patch_remove(&patch);
}

module_init(kgr_patcher_init);
module_exit(kgr_patcher_cleanup);

MODULE_LICENSE("GPL");


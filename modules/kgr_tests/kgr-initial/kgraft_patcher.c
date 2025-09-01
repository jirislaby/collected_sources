/*
 * kgraft_patch -- initial SLE Live Patching patch
 *
 * Patch uname to show kGraft in version string
 *
 *  Copyright (c) 2014 SUSE
 *  Author: Libor Pechacek
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kgraft.h>
#include <linux/syscalls.h>
#include <linux/personality.h>
#include <linux/utsname.h>
#include <generated/utsrelease.h>
#include <linux/version.h>
#include <linux/ctype.h>
#include <linux/string.h>
#include <asm/uaccess.h>

#ifdef COMPAT_UTS_MACHINE
#define override_architecture(name) \
	(personality(current->personality) == PER_LINUX32 && \
	 copy_to_user(name->machine, COMPAT_UTS_MACHINE, \
		      sizeof(COMPAT_UTS_MACHINE)))
#else
#define override_architecture(name)	0
#endif

/*
 * Work around broken programs that cannot handle "Linux 3.0".
 * Instead we map 3.x to 2.6.40+x, so e.g. 3.0 would be 2.6.40
 */
static int override_release(char __user *release, size_t len)
{
	int ret = 0;

	if (current->personality & UNAME26) {
		const char *rest = UTS_RELEASE;
		char buf[65] = { 0 };
		int ndots = 0;
		unsigned v;
		size_t copy;

		while (*rest) {
			if (*rest == '.' && ++ndots >= 3)
				break;
			if (!isdigit(*rest) && *rest != '.')
				break;
			rest++;
		}
		v = ((LINUX_VERSION_CODE >> 8) & 0xff) + 40;
		copy = clamp_t(size_t, len, 1, sizeof(buf));
		copy = scnprintf(buf, copy, "2.6.%u%s", v, rest);
		ret = copy_to_user(release, buf, copy + 1);
	}
	return ret;
}

#define KGR_TAG "/kGraft"

struct rw_semaphore *kgr_uts_sem;

asmlinkage long kgr_sys_newuname(struct new_utsname __user *name)
{
	int errno = 0;
	char *right_brace;

	down_read(kgr_uts_sem);
	if (copy_to_user(name, utsname(), sizeof *name))
		errno = -EFAULT;
	up_read(kgr_uts_sem);

	/* add "/kGraft" to the git commit id */
	if (errno)
		goto override;
	if (strlen(name->version) + strlen(KGR_TAG) >= sizeof(name->version)) {
		WARN_ONCE(1, "kgraft-patch: not enough space for utsname.version extension");
		goto override;
	}
	right_brace = strchr(name->version, ')');
	if (!right_brace) {
		WARN_ONCE(1, "kgraft-patch: did not find the commit id");
		goto override;
	}
	memmove(right_brace + strlen(KGR_TAG), right_brace,
		strlen(right_brace) + 1);
	memcpy(right_brace, KGR_TAG, strlen(KGR_TAG));

override:
	if (!errno && override_release(name->release, sizeof(name->release)))
		errno = -EFAULT;
	if (!errno && override_architecture(name))
		errno = -EFAULT;
	return errno;
}

static struct kgr_patch patch = {
	.name = "initial_patch",
	.owner = THIS_MODULE,
	.patches = {
		KGR_PATCH(SyS_newuname, kgr_sys_newuname, true),
		KGR_PATCH_END
	}
};

static int __init kgr_patcher_init(void)
{
	unsigned long addr;

	pr_info("kgraft-patch: initializing\n");

	addr = kallsyms_lookup_name("uts_sem");
	if (!addr) {
		pr_err("kgraft-patch: symbol uts_sem not resolved\n");
		return -EFAULT;
	}
	kgr_uts_sem = (struct rw_semaphore *) addr;

	return kgr_patch_kernel(&patch);
}

static void __exit kgr_patcher_cleanup(void)
{
	pr_info("kgraft-patch: removed\n");
	kgr_patch_remove(&patch);
}

module_init(kgr_patcher_init);
module_exit(kgr_patcher_cleanup);

MODULE_LICENSE("GPL");

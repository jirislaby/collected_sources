/*
 * kgr_patcher -- just kick kgr infrastructure for test
 *
 *  Copyright (c) 2013 Jiri Kosina, SUSE
 */

/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kgr.h>
#include <linux/kallsyms.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/capability.h>
#include <linux/ptrace.h>

#include <net/compat.h>

#include <asm/processor.h>

static int (*kgr___sys_recvmmsg)(int, struct mmsghdr __user *, unsigned int,
		   unsigned int, struct timespec *) =
		(void *)0xffffffff81534dd0UL;

asmlinkage long new_compat_sys_recvmmsg(int fd,
				    struct compat_mmsghdr __user *mmsg,
				    unsigned int vlen, unsigned int flags,
				    struct compat_timespec __user *timeout)
{
	int datagrams;
	struct timespec ktspec;

	if (flags & MSG_CMSG_COMPAT)
		return -EINVAL;

	if (timeout == NULL)
		return kgr___sys_recvmmsg(fd, (struct mmsghdr __user *)mmsg,
				vlen, flags | MSG_CMSG_COMPAT, NULL);

	if (compat_get_timespec(&ktspec, timeout))
		return -EFAULT;

	datagrams = kgr___sys_recvmmsg(fd, (struct mmsghdr __user *)mmsg, vlen,
				   flags | MSG_CMSG_COMPAT, &ktspec);
	if (datagrams > 0 && compat_put_timespec(&ktspec, timeout))
		datagrams = -EFAULT;

	return datagrams;
}
KGR_PATCHED_FUNCTION(patch, compat_sys_recvmmsg, new_compat_sys_recvmmsg);

static const struct kgr_patch patch = {
	.patches = {
		KGR_PATCH(compat_sys_recvmmsg),
		KGR_PATCH_END
	}
};

static int __init kgr_patcher_init(void)
{
	/* removing not supported (yet?) */
	__module_get(THIS_MODULE);
	/* +4 to skip push rbb / mov rsp,rbp prologue */
	kgr_start_patching(&patch);
	return 0;
}

static void __exit kgr_patcher_cleanup(void)
{
	printk(KERN_ERR "removing now buggy!\n");
}

module_init(kgr_patcher_init);
module_exit(kgr_patcher_cleanup);

MODULE_LICENSE("GPL");


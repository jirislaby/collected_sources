#!/usr/bin/bash

function check {
	dmesg|grep -qE 'WARN|BUG' && exit 1
}
function rmmod_par {
	lsmod|cut -f 1 -d ' '|grep -v virtio|xargs -P 10 -n 1 rmmod
	check
}

function rmmod_rest {
	rmmod `lsmod|cut -f 1 -d ' '|grep -v virtio`
	check
}

while : ; do
	echo MODPROBE
	find /lib/modules/`uname -r`/kernel/ -type f -printf '%f\n'|sed 's@.ko$@@' | xargs -P 10 -n 1 modprobe
	check

	echo RMMOD 1
	rmmod_par
	rmmod_par

	echo RMMOD 2
	rmmod_rest
done

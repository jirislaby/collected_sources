#!/usr/bin/bash

for a in `seq 1 $1`; do
	while :; do nc localhost 65000; done &
done

while :; do nc localhost 65000; done

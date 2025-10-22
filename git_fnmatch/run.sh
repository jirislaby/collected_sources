#!/usr/bin/bash

set -xe

ninja -C build
exec ./build/gf <(sed -n 's@^F: @@ p' ~/.cache/suse-get-maintainers/MAINTAINERS) <(sed -n "s@^F:"$'\t'"@@ p" ~/linux/MAINTAINERS) <(find ~/linux -type f|sed 's@/home/xslaby/linux/@@')

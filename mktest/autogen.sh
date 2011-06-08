#!/bin/sh

echo aclocal
aclocal
#echo libtool
#libtoolize --force
echo autoheader
autoheader
echo autoconf
autoconf
echo automake
automake --add-missing
echo ./configure
./configure

#!/bin/bash
#
# usage: ./install.sh <install-path>

target_list=""
library_list="libeep.so libgpio.so libclk.so libmsg.so libproc.so libshm.so libtrd.so" 


if [ $# != 1 ]; then
	echo "$0: Argment Error"
	exit 1
fi

install_dir="$1"
if [ ! -d ${install_dir} ]; then
	echo "$0: Argment Error (not found ${install_dir})"
	exit 1
fi

cp -a ./bin/test_cmd ${install_dir}/bin

for target in ${target_list}
do
	search=`find ./bin -name ${target}`
	if [ $? -eq 0 ]; then
		cp -a ${search} ${install_dir}/bin
	else
		echo "not found ${target} module"
	fi
done

for library in ${library_list}
do
	search=`find ./lib -type f -name ${library}`
	if [ $? -eq 0 ]; then
		cp -a ${search} ${install_dir}/lib
	else
		echo "not found ${library} module"
	fi
done


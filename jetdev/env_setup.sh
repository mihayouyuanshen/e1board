#!/bin/bash

BUILD_ARCH=
BUILD_VARIANT=
BUILD_CROSS=
BUILD_DEBUG=

echo "Please choose arch:"
echo "1.x86/x64"
echo "2.arm"
read BUILD_ARCH

case "${BUILD_ARCH}" in
	1) BUILD_CROSS=0;;
	2) BUILD_CROSS=1
	   source /opt/fsl-imx-wayland/5.15-kirkstone/environment-setup-cortexa7t2hf-neon-poky-linux-gnueabi
	   ;;
	*) echo "Invalid choice!";;
esac

echo "Please choose variant:"
echo "1.debug"
echo "2.release"
read BUILD_VARIANT

case "${BUILD_VARIANT}" in
	1) BUILD_DEBUG=1;;
	2) BUILD_DEBUG=0;;
	*) echo "Invalid choice!";;
esac

#echo "platform=${BUILD_ARCH}"
#echo "variant=${BUILD_VARIANT}"
#echo "BUILD_CROSS=${BUILD_CROSS}"
#echo "BUILD_DEBUG=${BUILD_DEBUG}"
export BUILD_CROSS
export BUILD_DEBUG

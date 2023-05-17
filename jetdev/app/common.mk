##############################################################################
##############################################################################
###
### Makefile : Makefile for common use
### history  :
### 2022.10.12 Ver0.01
###
##############################################################################
##############################
#   Project Definition       #
##############################
PROJECT = Farad

##############################
#   Toolchains               #
##############################
#ARCH = arm
ifeq ($(BUILD_CROSS), 1)
#Global Variable (CC,LD,AR) already exported
else
CC = gcc
LD = ld
AR = ar
endif

CFLAGS = -Wall -Wconversion -DDEBUG
CFLAGS += -D_FILE_OFFSET_BITS=64 -D_TIME_BITS=64
CFLAGS += -DTEST_CMD
#CFLAGS += -O0 -g3
#CFLAGS += -O0
#CFLAGS += -D_FORTIFY_SOURCE=2
#CFLAGS += -mthumb -mfpu=neon -mfloat-abi=hard -mcpu=cortex-a7 -fstack-protector-strong
#CFLAGS += -Wformat -Wformat-security -Werror=format-security
#CFLAGS += --sysroot=/home/nss/yocto-sdk/sysroots/cortexa7t2hf-neon-poky-linux-gnueabi
LDFLAGS =

##############################
#        Path setting        #
##############################
# Config files
CONFIGDIR = $(ROOTDIR)/config
# include directory
INCDIR = $(ROOTDIR)/inc
#COMMON_INCDIR = $(ROOTDIR)/inc/common
#LIB_INCDIR = $(ROOTDIR)/inc/device
#MODEL_INCDIR = $(ROOTDIR)/inc/model

#lsysroots_dir ?=

# kernel image directory
# KIRDIR = $(ROOTDIR)/kernel
# KINCDIR = $(ROOTDIR)/inc/kernel
# MMC boot image directory
# MMCBOOTDIR = $(ROOTDIR)/mmcboot
#   Boot Partition
# BOOTDIR=$(MMCBOOTDIR)/mmcblk0p1
#   File System Partition
# RFSDIR=$(MMCBOOTDIR)/mmcblk0p2
# JTRP boot image directory
# JTRPBOOTDIR = $(ROOTDIR)/jtrpboot
# FWupdate directory
# FWUPDATEDIR = $(ROOTDIR)/sd_image

# Built image directory
# OBJSDIR = $(ROOTDIR)/objs
# root file system directory
# ROOTFSDIR = $(ROOTDIR)/rootfs/products
# C Source directory
 SRCDIR = $(ROOTDIR)/src

# Temporary directory
# TEMPDIR = $(ROOTDIR)/temp
# TEMPVARDIR = $(ROOTDIR)/temp_var

# TEMPFFSCMNDIR = $(ROOTDIR)/temp_ffscmn

# Tool   Script
# TOOLSDIR = $(ROOTDIR)/tools
# Boot Loader directory
# UBOOTDIR = $(ROOTDIR)/uboot

# File Update Package Directory
# PACKDIR = $(ROOTDIR)/Update

##############################################################################
#   Destination  Directory
##############################################################################
#   Program Directory
 PRGDIR=$(ROOTDIR)/bin
#   Test Program
# TOOLDIR=$(TEMPDIR)/usr/hems/tool
#   Kernel module
# KMODDIR=$(TEMPDIR)/usr/hems/driver
#   Shared Library
# SOLIBDIR=$(TEMPDIR)/usr/hems/lib
#   Adjust files
# ADJDIR=$(TEMPDIR)/var/hems/adjust
#   Bundle Directories
# SJDIR=$(TEMPDIR)/usr/java
# SJFWDIR=$(TEMPDIR)/usr/java/framework
# HBUNDLEDIR=$(TEMPDIR)/var/hems/bundles/updated
#   APP Image
# APPIMAGEDIR=${TEMPDIR}/var/hems/bundles
#   Update dir
# UPDATEDIR=$(TEMPDIR)/var/hems/fwupdate
#   Bundle Configure Directory
# BCONFDIR=$(TEMPDIR)/var/hems/bundles/config
#   Change Value files
# CHGDIR=$(TEMPDIR)/var/hems
#   File System Partition
# VERDIR=${TEMPDIR}/usr/version
#   Config file backup directory for factory reset
# FACTDIR=${TEMPDIR}/usr/hems/facreset

########################################
# Include Path                         #
########################################
ALL_INC_DIR = $(INCDIR)
ALL_INC_DIR += $(INCDIR)/common
# ALL_INC_DIR += $(KINCDIR)
ALL_INC_DIR += .
#ALL_INC_DIR += ${lsysroots_dir}/usr/include

ALL_INC_FLAG = $(ALL_INC_DIR:%=-I%)


########################################
# nichicon / kyocera switch
########################################
#ifeq ($(PCS),kyc)
#	ALL_INC_FLAG += -DWORK_KYC
#else
#	ALL_INC_FLAG += -DWORK_NCC
#	PCS = ncc
#endif

########################################
# PCS simulator enable
########################################
#ifeq ($(SIM),off)
#	ALL_INC_FLAG += -DEPCS_RES_SIM
#endif

########################################
# BOARD configuration
########################################
#ifeq ($(BOARD),ES2)
#	ALL_INC_FLAG += -DES2
#else
#	ALL_INC_FLAG += -DES1
#endif

########################################
# PCS capacity configuration
########################################
#ifeq ($(CAPA),MIDDLE)
#	ALL_INC_FLAG += -DPCS_CAPACITY_M
#endif

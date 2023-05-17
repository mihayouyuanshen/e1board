
top_dir ?= $(shell pwd)/../..
lsysroots_dir = ${top_dir}/local_sysroots
envfile = .env.sysroots

fileexists = $(shell ls ${lsysroots_dir}/${envfile} | grep ${envfile})

ifeq (${fileexists}, ${lsysroots_dir}/${envfile})
    ifeq ($(cross_build),"true")
        include $(lsysroots_dir)/${envfile}
    endif
endif


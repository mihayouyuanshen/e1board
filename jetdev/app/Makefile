##############################################################################
##############################################################################
###
### Makefile : Makefile for link
### history  :
###
##############################################################################
ROOTDIR=.

TARGET = E1BoardApp
#include cross.mk
########################################
# Macro definition for common use      #
########################################
include common.mk
########################################
# Macro definition for built-in task   #
########################################
include task.mk

top_dir ?= $(shell pwd)/../..
skelton_dir := $(top_dir)/src/skelton/rootfs/usr

LIBDIR = 
LIBDIR += $(ROOTDIR)/lib
LD_DIR_FLAGS := $(LIBDIR:%=-L %)
LD_FLAGS = 
LD_FLAGS += -lpthread
LD_FLAGS += -lfrm
LD_FLAGS += -lwdt

ifeq ($(BUILD_DEBUG), 1)
OBJDIR := $(ROOTDIR)/objs/debug
BINDIR := $(ROOTDIR)/bin/debug
else
OBJDIR := $(ROOTDIR)/objs/release
BINDIR := $(ROOTDIR)/bin/release
endif

all: $(TARGET)

$(TARGET):$(TSKS)
	@[ -d $(BINDIR) ] || mkdir -p $(BINDIR)
	$(CC) -o $(BINDIR)/$@ $(OBJDIR)/*.o $(LD_DIR_FLAGS) $(LD_FLAGS)

$(TSKS):
	@$(MAKE) -j4 $(MTARGET) -C $(SRCDIR)/$@

clean:
	@echo "Now cleaning ..."
	@rm -f $(ROOTDIR)/bin/debug/$(TARGET)
	@rm -f $(ROOTDIR)/bin/release/$(TARGET)
	@$(foreach path,$(TSKS),make $@ -C $(SRCDIR)/$(path);)

install:all
	@echo "Now installing ..."
	@(./install.sh ${skelton_dir} && cd -)


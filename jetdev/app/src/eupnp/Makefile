#
# Makefile for shared-library
#
ROOTDIR := ../..
include $(ROOTDIR)/common.mk

ifeq ($(BUILD_DEBUG), 1)
# OBJ_DIR should be absolute path, for it will pass to subdir's Makefile
OBJ_DIR := $(abspath $(ROOTDIR)/objs/debug)
DEBUG_FLAG := -g
else
OBJ_DIR := $(abspath $(ROOTDIR)/objs/debug)
DEBUG_FLAG :=
endif

export OBJ_DIR $(OBJ_DIR)
DEPDIR := .deps
SRCS := $(wildcard *.c)
OBJS := $(addprefix $(OBJ_DIR)/, $(notdir $(SRCS:.c=.o)))
DEBUG_OBJS := $(addprefix $(ROOTDIR)/objs/debug/, $(notdir $(SRCS:.c=.o)))
RELEASE_OBJS := $(addprefix $(ROOTDIR)/objs/release/, $(notdir $(SRCS:.c=.o)))
DEPS := $(addprefix $(DEPDIR)/, $(notdir $(SRCS:.c=.d)))

MY_INCDIR = .
MY_INCDIR += $(ROOTDIR)/inc
MY_INCDIR += $(ROOTDIR)/../include
MY_INCDIR += $(ROOTDIR)/../frm
MY_INCDIR += $(ROOTDIR)/src/eled

MY_INCDIR_FLAG := $(MY_INCDIR:%=-I %)

MY_CFLAGS := $(CFLAGS) $(DEBUG_FLAG) -Wextra $(ALL_INC_FLAG) $(MY_INCDIR_FLAG) -fPIC
MY_DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d

UPNP_DIR=./miniupnpd

.PHONY: all clean

all: $(OBJS) upnpd

upnpd:
	$(MAKE) -C $(UPNP_DIR)
	
clean:
	@rm -f $(DEBUG_OBJS)
	@rm -f $(RELEASE_OBJS)
	@rm -rf $(DEPDIR)
	$(MAKE) -C $(UPNP_DIR) clean

distclean:
	rm -f $(TARGET)

$(OBJS): $(OBJ_DIR)/%.o:%.c $(DEPDIR)/%.d
	@[ -d $(OBJ_DIR) ] || mkdir -p $(OBJ_DIR)
	@[ -d $(DEPDIR) ] || mkdir -p $(DEPDIR)
	$(CC) $(MY_DEPFLAGS) $(MY_CFLAGS) -c -o $@ $<
	

# The empty rule is required to handle the case where the dependency file is deleted.
$(DEPS):

include $(wildcard $(DEPS))

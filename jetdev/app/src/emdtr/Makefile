#
# Makefile for shared-library
#
ROOTDIR := ../..
include $(ROOTDIR)/common.mk

ifeq ($(BUILD_DEBUG), 1)
OBJDIR := $(ROOTDIR)/objs/debug
DEBUG_FLAG := -g
else
OBJDIR := $(ROOTDIR)/objs/release
DEBUG_FLAG :=
endif

DEPDIR := .deps
SRCS := $(wildcard *.c)
OBJS := $(addprefix $(OBJDIR)/, $(notdir $(SRCS:.c=.o)))
DEBUG_OBJS := $(addprefix $(ROOTDIR)/objs/debug/, $(notdir $(SRCS:.c=.o)))
RELEASE_OBJS := $(addprefix $(ROOTDIR)/objs/release/, $(notdir $(SRCS:.c=.o)))
DEPS := $(addprefix $(DEPDIR)/, $(notdir $(SRCS:.c=.d)))

WTIM_DONTNTPD = -DDONTNTPD
WTIM_DEBUG = -DDEBUG
WTIM_ES2 = -DES2
WTIM_DONTSETTIME = -DDONTSETTIME
#    S社向けG1対応を有効にする場合、-DWDT_S_G1_ENABLE を定義
S_G1_ENABLE =

#CC ?= gcc
DEBUG := -g
MY_INCDIR =
MY_INCDIR += $(ROOTDIR)/inc
MY_INCDIR += $(ROOTDIR)/../include
MY_INCDIR += $(ROOTDIR)/../frm
MY_INCDIR_FLAG := $(MY_INCDIR:%=-I %)

MY_CFLAGS := $(CFLAGS) $(DEBUG) -Wextra $(ALL_INC_FLAG) $(MY_INCDIR_FLAG) $(WTIM_DONTNTPD) $(WTIM_DEBUG) $(WTIM_ES2) $(WTIM_DONTSETTIME) $(S_G1_ENABLE) -fPIC
MY_DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d

.PHONY: all clean
all: $(OBJS)

clean:
	rm -f $(OBJS)
	rm -rf $(OBJDIR) $(DEPDIR)

distclean:
	rm -f $(TARGET)

$(OBJS): $(OBJDIR)/%.o:%.c $(DEPDIR)/%.d
	[ -d $(OBJDIR) ] || mkdir -p $(OBJDIR)
	[ -d $(DEPDIR) ] || mkdir -p $(DEPDIR)
	$(CC) $(MY_DEPFLAGS) $(MY_CFLAGS) -c -o $@ $<

# The empty rule is required to handle the case where the dependency file is deleted.
$(DEPS):

include $(wildcard $(DEPS))

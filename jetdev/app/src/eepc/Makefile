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

MY_INCDIR =
MY_INCDIR += $(ROOTDIR)/inc
MY_INCDIR += $(ROOTDIR)/../include
MY_INCDIR_FLAG := $(MY_INCDIR:%=-I %)

MY_CFLAGS := $(CFLAGS) $(DEBUG_FLAG) -Wextra $(ALL_INC_FLAG) $(MY_INCDIR_FLAG) -fPIC
MY_DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d

.PHONY: all clean
all: $(OBJS)

clean:
	@rm -f $(DEBUG_OBJS)
	@rm -f $(RELEASE_OBJS)
	@rm -rf $(DEPDIR)

distclean:
	rm -f $(TARGET)

$(OBJS): $(OBJDIR)/%.o:%.c $(DEPDIR)/%.d
	@[ -d $(OBJDIR) ] || mkdir -p $(OBJDIR)
	@[ -d $(DEPDIR) ] || mkdir -p $(DEPDIR)
	$(CC) $(MY_DEPFLAGS) $(MY_CFLAGS) -c -o $@ $<

# The empty rule is required to handle the case where the dependency file is deleted.
$(DEPS):

include $(wildcard $(DEPS))

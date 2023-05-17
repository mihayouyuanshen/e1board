#
# Makefile for frm
#
TARGET = libfrm.a

ifeq ($(BUILD_CROSS), 1)
#Global Variable (CC,LD,AR) already exported
else
CC = gcc
LD = ld
AR = ar
endif

ROOTDIR := .
OBJDIR := .objs
DEPDIR := .deps
LIB_INSTALL_DIR := $(ROOTDIR)/../app/lib
INC_INSTALL_DIR := $(ROOTDIR)/../include
INCS := dlog_g.h frm_g.h lfw_g.h
INSTALL_INCS := $(addprefix $(INC_INSTALL_DIR)/, $(INCS))
SRCS := $(wildcard *.c)
OBJS := $(addprefix $(OBJDIR)/, $(notdir $(SRCS:.c=.o)))
DEPS := $(addprefix $(DEPDIR)/, $(notdir $(SRCS:.c=.d)))

ifeq ($(BUILD_DEBUG), 1)
DEBUG_FLAG := -g
else
DEBUG_FLAG :=
endif
MY_INCDIR =
MY_INCDIR += $(ROOTDIR)/../include
MY_INCDIR += $(ROOTDIR)
MY_INCDIR_FLAG := $(MY_INCDIR:%=-I %)

CFLAGS :=
MY_CFLAGS := $(CFLAGS) $(DEBUG_FLAG) -Wextra $(ALL_INC_FLAG) $(MY_INCDIR_FLAG) -fPIC
MY_DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d

.PHONY: all clean
all: $(TARGET)

clean:
	@echo "Now cleaning ..."
	@rm -f $(OBJS)
	@rm -rf $(OBJDIR)
	@rm -f $(DEPS)
	@rm -rf $(DEPDIR)
	@rm -f $(TARGET)
	@rm -f $(LIB_INSTALL_DIR)/$(TARGET)
	@rm -f $(INSTALL_INCS)

install:
	@echo "Now installing ..."
	@[ -d $(LIB_INSTALL_DIR) ] || mkdir -p $(LIB_INSTALL_DIR)
	@[ -d $(INC_INSTALL_DIR) ] || mkdir -p $(INC_INSTALL_DIR)
	@cp -f $(TARGET) $(LIB_INSTALL_DIR)
	@cp -f $(INCS) $(INC_INSTALL_DIR)

$(TARGET):$(OBJS)
	$(AR) r $@ $^

$(OBJS): $(OBJDIR)/%.o:%.c $(DEPDIR)/%.d
	@[ -d $(OBJDIR) ] || mkdir -p $(OBJDIR)
	@[ -d $(DEPDIR) ] || mkdir -p $(DEPDIR)
	$(CC) $(MY_DEPFLAGS) $(MY_CFLAGS) -c -o $@ $<

# The empty rule is required to handle the case where the dependency file is deleted.
$(DEPS):

include $(wildcard $(DEPS))

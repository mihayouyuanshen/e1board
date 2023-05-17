#
# Makefile for wdt
#
ROOTDIR := ../

TARGET := wdt
LIB_TARGET := libwdt.a
OBJDIR := .objs
DEPDIR := .deps
SRCS := $(wildcard *.c)
OBJS := $(addprefix $(OBJDIR)/, $(notdir $(SRCS:.c=.o)))
DEPS := $(addprefix $(DEPDIR)/, $(notdir $(SRCS:.c=.d)))

#コンパイルスイッチ
#    HW WATCHDOGタイマ制御有効にする場合、-DWDT_HW_WDT_ENABLE を定義
HW_WDT_ENABLE = -DWDT_HW_WDT_ENABLE
#    常駐プロセス監視制御を有効にする場合、-DWDT_PROC_WDT_ENABLE を定義
PROC_WDT_ENABLE = -DWDT_PROC_WDT_ENABLE
#    終了制御を有効にする場合、-DWDT_TERM_ENABLE を定義
TERM_ENABLE = -DWDT_TERM_ENABLE
#TERM_ENABLE =
#    サーバ関連プロセスを監視対象にする場合、-DWDT_SRV_PROC_ENABLE を定義
SRV_PROC_ENABLE = -DWDT_SRV_PROC_ENABLE
#    S社向けG1対応を有効にする場合、-DWDT_S_G1_ENABLE を定義
S_G1_ENABLE =

ifeq ($(BUILD_CROSS), 1)
#Global Variable (CC,LD,AR) already exported
else
CC = gcc
LD = ld
AR = ar
endif

ifeq ($(BUILD_DEBUG), 1)
DEBUG_FLAG := -g
else
DEBUG_FLAG :=
endif

LIBDIR = 
LIBDIR += $(ROOTDIR)/app/lib
LD_DIR_FLAGS := $(LIBDIR:%=-L %)
LD_FLAGS = 
LD_FLAGS += -lpthread
LD_FLAGS += -lfrm

MY_INCDIR =
MY_INCDIR += $(ROOTDIR)/include
MY_INCDIR_FLAG := $(MY_INCDIR:%=-I %)

MY_CFLAGS := $(CFLAGS) $(DEBUG_FLAG) -Wextra $(ALL_INC_FLAG) $(MY_INCDIR_FLAG) $(HW_WDT_ENABLE) $(PROC_WDT_ENABLE) $(TERM_ENABLE) $(SRV_PROC_ENABLE) $(S_G1_ENABLE) -fPIC
MY_DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d

.PHONY: all clean
all: $(TARGET)

$(TARGET):$(OBJS)
	$(CC) -o $@ $(OBJDIR)/*.o $(LD_DIR_FLAGS) $(LD_FLAGS)
	$(AR) r $(LIBDIR)/$(LIB_TARGET) $(OBJDIR)/wdt_if.o

clean:
	rm -f $(TARGET)
	rm -f $(LIB_TARGET)
	rm -f $(OBJS)
	rm -rf $(OBJDIR) $(DEPDIR)

$(OBJS): $(OBJDIR)/%.o:%.c $(DEPDIR)/%.d
	[ -d $(OBJDIR) ] || mkdir -p $(OBJDIR)
	[ -d $(DEPDIR) ] || mkdir -p $(DEPDIR)
	$(CC) $(MY_DEPFLAGS) $(MY_CFLAGS) -c -o $@ $<

# The empty rule is required to handle the case where the dependency file is deleted.
$(DEPS):

include $(wildcard $(DEPS))

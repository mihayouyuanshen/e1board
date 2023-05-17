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

#コンパイルスイッチ
#    HW WATCHDOGタイマ制御有効にする場合、-DEWDT_HW_WDT_ENABLE を定義
HW_WDT_ENABLE = -DEWDT_HW_WDT_ENABLE
#    常駐プロセス監視制御を有効にする場合、-DEWDT_PROC_WDT_ENABLE を定義
PROC_WDT_ENABLE = -DEWDT_PROC_WDT_ENABLE
#    終了制御を有効にする場合、-DEWDT_TERM_ENABLE を定義
TERM_ENABLE = -DEWDT_TERM_ENABLE
#TERM_ENABLE =
#    サーバ関連プロセスを監視対象にする場合、-DEWDT_SRV_PROC_ENABLE を定義
SRV_PROC_ENABLE = -DEWDT_SRV_PROC_ENABLE
#    S社向けG1対応を有効にする場合、-DEWDT_S_G1_ENABLE を定義
S_G1_ENABLE =
MY_INCDIR =
MY_INCDIR += $(ROOTDIR)/inc
MY_INCDIR += $(ROOTDIR)/../include
MY_INCDIR_FLAG := $(MY_INCDIR:%=-I %)

MY_DEFFLAGS := $(HW_WDT_ENABLE) $(PROC_WDT_ENABLE) $(TERM_ENABLE) $(SRV_PROC_ENABLE) $(S_G1_ENABLE)
MY_CFLAGS := $(CFLAGS) $(DEBUG_FLAG) -Wextra $(ALL_INC_FLAG) $(MY_INCDIR_FLAG) $(MY_DEFFLAGS) -fPIC
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

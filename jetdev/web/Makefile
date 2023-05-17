# 定义编译器
GCC=$(CC)
GCC?=cc
# 定义include
WEBSVR_INCLUDE = -I.
WEBSVR_INCLUDE += -Iinc
WEBSVR_INCLUDE += -Isrc
# CFLAGS是由include内容 + 编译开关/选项构建
CFLAGS = $(WEBSVR_INCLUDE)

CFLAGS += -O0 -g -DDEBUG

ARFLAGS = 
# 部署到docker需要静态编译
#ARFLAGS += -static

# 定义根目录
ROOT_DIR = .
# 定义src目录
SRC_DIR = $(ROOT_DIR)/src

# 定义编译模式
BUILD_DEBUG=1

# 定义目标路径，release版本和debug版本
# .o文件放到objs目录下，最终生成的二进制文件放在bin目录下
ifeq ($(BUILD_DEBUG), 1)
OBJS_DIR=$(ROOT_DIR)/objs/debug
BIN_DIR=$(ROOT_DIR)/bin/debug
# 如果是debug版本，编译开关需要添加-g以及-O0（不进行编译优化）

else
OBJS_DIR=$(ROOT_DIR)/objs/release
BIN_DIR=$(ROOT_DIR)/bin/release
# 如果是relase版本，添加-O开关来对编译进行优化，提高编译效率
CFLAGS += -O3

endif

# 获取所有的.c文件的全路径
FRAMEWORK_RAW_SRCS := $(wildcard src/*.c) $(wildcard src/frm/*.c) $(wildcard src/filter/*.c)  $(wildcard src/ref/*.c)
# 去掉.c文件的路径信息，只要文件名
FRAMEWORK_SRCS = $(notdir $(foreach n, $(FRAMEWORK_RAW_SRCS), $(n)))
# 将.c文件名后缀改为.o文件名
FRAMEWORK_OBJS = $(patsubst %.c, %.o, $(FRAMEWORK_SRCS))

WEBSERVICE_RAW_SRCS := $(wildcard src/ctrl/*.c) $(wildcard src/srv/*.c)
WEBSERVICE_SRCS = $(notdir $(foreach n, $(WEBSERVICE_RAW_SRCS), $(n)))
WEBSERVICE_OBJS = $(patsubst %.c, %.o, $(WEBSERVICE_SRCS))

OTHER_RAW_SRCS := $(wildcard src/util/*.c) $(wildcard src/utest/*.c)
OTHER_SRCS = $(notdir $(foreach n, $(OTHER_RAW_SRCS), $(n)))
OTHER_OBJS = $(patsubst %.c, %.o, $(OTHER_SRCS))
# 编译语句，因为所有的.c文件的
COMPILE_CMD = $(GCC) $(CFLAGS) -c $< -o $(OBJS_DIR)/$@

TARGET=cgi_svc

# 用于创建文件夹
DIRS := $(BIN_DIR) $(OBJS_DIR)
MKDIR = mkdir -p

.PHONY: all test clean

all: ${DIRS} $(TARGET)

test:
	$(MKDIR) $@

clean:
	rm -rf $(OBJS_DIR)/*.o
	rm -rf $(BIN_DIR)/*

install:
	cp $(BIN_DIR)/cgi_svc.cgi /opt/server/cgi-bin

deploy: clean all install

# 校验目录，如果不存在，则创建指定目录，因为这个规则才需要声明.PHONY，避免有同名
$(DIRS):
	$(MKDIR) $@

# 生成二进制文件，但是生成二进制文件之前需要生成.o文件
$(TARGET): $(FRAMEWORK_OBJS) $(WEBSERVICE_OBJS) $(OTHER_OBJS)
	$(GCC) $(ARFLAGS) -o $(BIN_DIR)/cgi_svc.cgi $(OBJS_DIR)/*.o

# 如果.o文件，在根目录下有同名的.c文件（主要是cgi_svc.c）
%.o: $(SRC_DIR)/%.c
	$(COMPILE_CMD)
# 如果.o文件，在src/frm目录下有同名的.c文件
%.o: $(SRC_DIR)/frm/%.c
	$(COMPILE_CMD)

%.o: $(SRC_DIR)/filter/%.c
	$(COMPILE_CMD)

%.o: $(SRC_DIR)/ref/%.c
	$(COMPILE_CMD)

%.o: $(SRC_DIR)/ctrl/%.c
	$(COMPILE_CMD)

%.o: $(SRC_DIR)/srv/%.c
	$(COMPILE_CMD)

%.o: $(SRC_DIR)/util/%.c
	$(COMPILE_CMD)	

%.o: $(SRC_DIR)/utest/%.c
	$(COMPILE_CMD)	

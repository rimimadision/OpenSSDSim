BUILD_EMU_DIR=./build_emu
CC=gcc
SRC=$(wildcard *.c ./*/*.c) 
SRC+=../ftl/ftl.c ../ftl/ftl_nvme.c ../ftl/ftl_taskqueue.c ../ftl/ftl_core0.c ../ftl/ftl_core1.c
SRC+=../hcl/hcl.c
SRC+=../buffer/buffer.c
SRC+= ../lib/list.c ../lib/lock.c ../lib/dprint.c
SRC+=../nvme/host_lld.c
SRC+=../fcl/fcl.c
SRC+=../l2p/l2p.c

OBJS=$(patsubst %.c,%.o,$(notdir $(SRC)))

all:
build:
	$(CC) -o $(BUILD_EMU_DIR)/emu $(SRC) -lpthread -m32 -lrt
init:
	test -d $(BUILD_EMU_DIR) || mkdir -p $(BUILD_EMU_DIR)
	rm -f $(BUILD_EMU_DIR)/* 

# TODO: 添加自动推导依赖
# %.d: %.c
# 	@set -e; rm -f $@; /
# 	$(CC) -MM  $< > $@.$$$$; /
#     #sed 's,/($*/)/.o[ :]*,/1.o $@ : ,g' < $@.$$$$ > $@; /
#     rm -f $@.$$$$
# sinclude $(SOURCES:.c=.d)

emu: build
	
	sudo $(BUILD_EMU_DIR)/emu 
#2>&1 | tee emu_log 
clean:
	rm -f $(BUILD_EMU_DIR)/* 
	rm -f *.o *.d
	rm -f flash_log
all:init emu
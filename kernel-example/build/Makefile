COMPILER=i686-elf-gcc

BUILD_DIR=../../build
OUTPUT_DIR=../../storage
OUTPUT_KERNEL=$(OUTPUT_DIR)/KERNEL.YBT

COMPILE_FLAGS=-Wall -Wextra -std=c17 -ffreestanding -O0 -m32 -I ../include -I ../../lib-common/bootldr
LINK_FLAGS=-Wall -Wextra -nostdlib -ffreestanding -O0 -m32 -T ybt.ld -lgcc

SRCS=$(shell find ../source -name *.c)
OBJS=$(addsuffix .o, $(SRCS))

all:
	$(MAKE) build
	$(MAKE) run
	$(MAKE) clear

build:
	$(MAKE) $(OBJS)
	$(COMPILER) $(OBJS) $(LINK_FLAGS) -o $(OUTPUT_KERNEL)
	$(MAKE) -C $(BUILD_DIR) build-image
	cp $(BUILD_DIR)/output.bin ./os.bin

run:
	$(MAKE) -C $(BUILD_DIR) run

clear:
	$(MAKE) -C $(BUILD_DIR) clear
	rm -f $(OUTPUT_KERNEL) $(OBJS)

%.c.o:	%.c
	$(COMPILER) -c $< $(COMPILE_FLAGS) -o $@
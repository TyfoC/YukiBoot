COMPILER=i686-elf-gcc
ASSEMBLER=nasm
EMULATOR=qemu-system-i386

OUTPUT_NAME=yukiboot
OUTPUT=$(OUTPUT_NAME).bin

DIR_LIB_NASM=../../lib-nasm
DIR_LIB_COMMON=../../lib-common
DIR_LIB_BOOTLDR=$(DIR_LIB_COMMON)/bootldr
DIR_LIB_RBFS=$(DIR_LIB_COMMON)/rbfs
DIR_SETUP=../setup
DIR_RM_SRVLDR=../rm-srvldr
DIR_BOOTLDR=../bootldr
DIR_OUTPUT=./bin

COMPILE_FLAGS=-ffreestanding -Wall -Wextra -O0 -nostdlib -std=c17
LINK_FLAGS=-ffreestanding -Wall -Wextra -O0 -nostdlib -lgcc

BOOTLDR_COMPILE_FLAGS=$(COMPILE_FLAGS) -m32 -I $(DIR_LIB_BOOTLDR) -I $(DIR_LIB_RBFS)
BOOTLDR_LINK_FLAGS=$(LINK_FLAGS) -m32 -T bootldr.ld

LIB_COMMON_C_SRCS=$(shell find $(DIR_LIB_COMMON)/ -name *.c)
LIB_COMMON_ASM_SRCS=$(shell find $(DIR_LIB_COMMON)/ -name *.asm)
LIB_COMMON_C_OBJS=$(addsuffix .gcc.o, $(LIB_COMMON_C_SRCS))
LIB_COMMON_ASM_OBJS=$(addsuffix .gcc.o, $(LIB_COMMON_ASM_SRCS))

BOOTLDR_C_SRCS=$(shell find $(DIR_BOOTLDR)/ -name *.c)
BOOTLDR_ASM_SRCS=$(shell find $(DIR_BOOTLDR)/ -name *.asm)
BOOTLDR_C_OBJS=$(addsuffix .o, $(BOOTLDR_C_SRCS))
BOOTLDR_ASM_OBJS=$(addsuffix .o, $(BOOTLDR_ASM_SRCS))

all-$(OUTPUT_NAME):
	$(MAKE) build-$(OUTPUT_NAME)
	$(MAKE) run-$(OUTPUT_NAME)
	$(MAKE) clear-$(OUTPUT_NAME)

build-$(OUTPUT_NAME):
	$(MAKE) build-bootldr
	$(MAKE) build-rm-srvldr
	$(MAKE) build-setup
	cp $(DIR_OUTPUT)/setup.bin $(DIR_OUTPUT)/$(OUTPUT)

run-$(OUTPUT_NAME):
	$(EMULATOR) -monitor stdio -m 256M -cpu max -drive format=raw,file=$(DIR_OUTPUT)/$(OUTPUT)

clear-$(OUTPUT_NAME):
	$(MAKE) clear-bootldr
	$(MAKE) clear-rm-srvldr
	$(MAKE) clear-setup
	rm -f $(DIR_OUTPUT)/$(OUTPUT)

#	=====================================================

build-setup:
	$(ASSEMBLER) -fbin $(DIR_SETUP)/setup.asm -i $(DIR_LIB_NASM)/ -I $(DIR_OUTPUT)/ -o $(DIR_OUTPUT)/setup.bin

build-rm-srvldr:
	$(ASSEMBLER) -fbin $(DIR_RM_SRVLDR)/rm-srvldr.asm -i $(DIR_LIB_NASM)/ -I $(DIR_OUTPUT)/ -o $(DIR_OUTPUT)/rm-srvldr.bin

build-bootldr:
ifneq ($(LIB_COMMON_C_SRCS),)					# build if .c library files exist
	$(MAKE) $(LIB_COMMON_C_OBJS)
endif
ifneq ($(LIB_COMMON_ASM_SRCS),)				# build if .asm library files exist
	$(MAKE) $(LIB_COMMON_ASM_OBJS)
endif
ifneq ($(BOOTLDR_C_SRCS),)					# build if .c gcc files exist
	$(MAKE) $(BOOTLDR_C_OBJS)
endif
ifneq ($(BOOTLDR_ASM_SRCS),)				# build if .asm gcc files exist
	$(MAKE) $(BOOTLDR_ASM_OBJS)
endif
	$(COMPILER) $(LIB_COMMON_C_OBJS) $(LIB_COMMON_ASM_OBJS) $(BOOTLDR_C_OBJS) $(BOOTLDR_ASM_OBJS) $(BOOTLDR_LINK_FLAGS) -o $(DIR_OUTPUT)/bootldr.bin

clear-setup:
	rm -f $(DIR_OUTPUT)/setup.bin

clear-rm-srvldr:
	rm -f $(DIR_OUTPUT)/rm-srvldr.bin

clear-bootldr:
	rm -f $(DIR_OUTPUT)/bootldr.bin
	rm -f $(LIB_COMMON_C_OBJS) $(LIB_COMMON_ASM_OBJS)
	rm -f $(BOOTLDR_C_OBJS) $(BOOTLDR_ASM_OBJS)

%.c.gcc.o:		%.c
	$(COMPILER) -c $< $(BOOTLDR_COMPILE_FLAGS) -o $@

%.asm.gcc.o:	%.asm
	$(ASSEMBLER) -felf32 $< -i $(DIR_LIB_COMMON)/ -o $@

%.c.o:			%.c
	$(COMPILER) -c $< $(BOOTLDR_COMPILE_FLAGS) -o $@

%.asm.o:	%.asm
	$(ASSEMBLER) -felf32 $< -i $(DIR_LIB_COMMON)/ -o $@
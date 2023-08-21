DIR_RBFS=../rbfs
DIR_RBFS_BUILD=$(DIR_RBFS)/build
DIR_YUKIBOOT=../yukiboot
DIR_YUKIBOOT_BUILD=$(DIR_YUKIBOOT)/build
DIR_YUKIBOOT_BUILD_BIN=$(DIR_YUKIBOOT_BUILD)/bin

EMULATOR=qemu-system-i386
UTILITY_RBFS=$(DIR_RBFS_BUILD)/rbfs.out
IMAGE_YUKIBOOT=$(DIR_YUKIBOOT_BUILD_BIN)/yukiboot.bin

OUTPUT=output.bin

all:
	$(MAKE) build-image
	$(MAKE) run
	$(MAKE) clear

build-image:
	$(MAKE) clear
	$(UTILITY_RBFS) -cstgpt ../storage 1 "Boot Partition" 0 ./rbfs-partition-header.bin ./rbfs-file-table.bin
	$(UTILITY_RBFS) -cfsh 1 1 ./rbfs-header.bin

	cat $(IMAGE_YUKIBOOT) ./rbfs-header.bin \
		./rbfs-partition-header.bin \
		./rbfs-file-table.bin \
	> $(OUTPUT)

	outputSize=`stat -L -c %s $(IMAGE_YUKIBOOT)`;\
	outputLength=`expr $$((outputSize)) / 512`;\
	yukibootSize=`stat -L -c %s $(IMAGE_YUKIBOOT)`;\
	yukibootLength=`expr $$((yukibootSize)) / 512`;\
	rbfsLength=`expr $$((outputLength)) - $$((yukibootLength))`;\
	$(UTILITY_RBFS) -wmbrpe $(OUTPUT) 1 7F 80 $$((yukibootLength)) 1

build-needed:
	$(MAKE) build-rbfs
	$(MAKE) build-yukiboot

build-rbfs:
	$(MAKE) -C $(DIR_RBFS_BUILD) clear
	$(MAKE) -C $(DIR_RBFS_BUILD) build

build-yukiboot:
	$(MAKE) -C $(DIR_YUKIBOOT_BUILD) clear-yukiboot
	$(MAKE) -C $(DIR_YUKIBOOT_BUILD) build-yukiboot

run:
	$(EMULATOR) -monitor stdio -m 256M -cpu max -drive format=raw,file=$(OUTPUT)

debug:
	$(EMULATOR) -monitor stdio -m 256M -cpu max -drive format=raw,file=$(OUTPUT) -D debug.txt -d cpu_reset

clear:
	rm -f ./rbfs-header.bin ./rbfs-partition-header.bin ./rbfs-file-table.bin ./output.bin debug.txt ./*.ini
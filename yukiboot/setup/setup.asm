[bits 16]
[org 0x7C00]

%define MAIN_SETUP_BASE_ADDRESS				0x00007E00
%define MAIN_SETUP_BASE_SEGMENT				0x0000
%define MAIN_SETUP_BASE_OFFSET				0x7E00

%define RM_SRVLDR_BASE_ADDRESS				0x00001000
%define RM_SRVLDR_BASE_SEGMENT				0x0000
%define RM_SRVLDR_BASE_OFFSET				0x1000

%define BOOTLDR_BASE_ADDRESS				0x00001400
%define BOOTLDR_BASE_SEGMENT				0x0000
%define BOOTLDR_BASE_OFFSET					0x1400

%define STACK_TOP_SEGMENT					RM_SRVLDR_BASE_SEGMENT
%define STACK_TOP_OFFSET					RM_SRVLDR_BASE_OFFSET

%define MAIN_SETUP_LENGTH					((((MainSetupEnd - MainSetup) + 511) & ~511) / 512)
%define RM_SRVLDR_LENGTH					((((RMSrvLdrEnd - RMSrvLdr) + 511) & ~511) / 512)
%define BOOTLDR_LENGTH						((((BootloaderEnd - Bootloader) + 511) & ~511) / 512)

%define MAIN_SETUP_SECTOR					0x0002
%define RM_SRVLDR_SECTOR					(MAIN_SETUP_SECTOR + MAIN_SETUP_LENGTH)
%define BOOTLDR_SECTOR						(RM_SRVLDR_SECTOR + RM_SRVLDR_LENGTH)

%define BOOTLDR_HEADER_ENTRY_POINT_OFFSET	0

Begin:
	jmp 0:EntryPoint
EntryPoint:
	mov ax, 0
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	mov ax, STACK_TOP_SEGMENT
	mov ss, ax
	mov bp, STACK_TOP_OFFSET
	mov sp, bp

	cld
	sti

	mov ax, 0x0200 | MAIN_SETUP_LENGTH
	mov cx, MAIN_SETUP_SECTOR
	mov dh, 0x00
	mov bx, MAIN_SETUP_BASE_SEGMENT
	mov es, bx
	mov bx, MAIN_SETUP_BASE_OFFSET
	int 0x13
	jc Error.FailedToReadDrive

	jmp MAIN_SETUP_BASE_SEGMENT:MAIN_SETUP_BASE_OFFSET
Error:
.FailedToReadDrive:
	mov si, Messages.FailedToReadDrive
	jmp .Print
.FailedToInitDrive:
	mov si, Messages.FailedToInitDrive
	jmp .Print
.Print:
	call PutString
	cli
	hlt

	%include "tty.asm"

Messages:
.FailedToReadDrive:		db "Failed to read drive!", 0
.FailedToInitDrive:		db "Failed to init drive!", 0

PartitionTable:
	times 446 - $ + $$ db 0

	times 510 - $ + $$ db 0
	dw 0xAA55
End:

MainSetup:
	call SelectDisk
	jc Error.FailedToInitDrive

	;	Set 80x25 text mode
	mov ax, 0x0003
	int 0x10

	;	Disable blinking attribute
	mov ax, 0x1003
	mov bx, 0
	int 0x10

	mov ah, 0x0001
	mov cx, 0x0706
	int 0x10

	;	Read real mode service loader
	push es
	mov cx, RM_SRVLDR_SECTOR
	mov dh, 0x0000
	mov bx, RM_SRVLDR_BASE_SEGMENT
	mov es, bx
	mov bx, RM_SRVLDR_BASE_OFFSET
	mov si, RM_SRVLDR_LENGTH
	call ReadSectors
	pop es
	jc Error.FailedToReadDrive

	;	Read bootloader
	push es
	mov cx, BOOTLDR_SECTOR
	mov dh, 0x0000
	mov bx, BOOTLDR_BASE_SEGMENT
	mov es, bx
	mov bx, BOOTLDR_BASE_OFFSET
	mov si, BOOTLDR_LENGTH
	call ReadSectors
	pop es
	jc Error.FailedToReadDrive

	cli
	lgdt [GDT.Register]
	push dword .JumpToBootldr
	call dword SwitchToProtectedMode

[bits 32]
.JumpToBootldr:
	push dword [DriveParameters.Index]	; drive index
	push dword 0
	jmp [BOOTLDR_BASE_ADDRESS + BOOTLDR_HEADER_ENTRY_POINT_OFFSET]
[bits 16]

	%include "cpu.asm"
	%include "disk.asm"

GDT:	ALIGN(4)
.Register:
	dw .After - .Begin - 1
	dd .Begin
.Begin:
.Null:			dq 0					; --	0x00
.PMCode:		dq 0xCF9A000000FFFF		; PM	0x08
.PMData:		dq 0xCF92000000FFFF		; PM	0x10
.RMCode:		dq 0xF9A000000FFFF		; PM16	0x18
.RMData:		dq 0xF92000000FFFF		; PM16	0x20
.After:

	times 1024 - $ + $$ db 0
MainSetupEnd:

;	Real mode service loader
RMSrvLdr:
incbin "rm-srvldr.bin"
RMSrvLdrEnd:

;	Bootloader
Bootloader:
incbin "bootldr.bin"
BootloaderEnd:
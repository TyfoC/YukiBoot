[bits 32]

ALIGN(4)
PPrevIDT:	dw 0, 0, 0
PNewIDT:
	dw IDT.After - IDT - 1
	dd IDT
IDT:
dq 0							; #DE
dq 0							; #DB
dq 0							; -
dq 0							; #BP
dq 0							; #OF
dq 0							; #BR
.InvalidOpcode:
	dw tmp_ud_handler			; Offset[LOW]
	dw 0x08						; CODESEG selector
	db 0						; Reserved
	db 0x8E						; Type & Flags (INT32 gate, PRESENT)
	dw 0						; Offset[HIGH]
.After:

[global cpuid_is_present]
cpuid_is_present:				; uint8_t cpuid_is_present(void)
	pushad
	cli
	sidt [PPrevIDT]
	lidt [PNewIDT]

	mov eax, 0
	cpuid

	mov byte [.Status], 1
.Checked:
	lidt [PPrevIDT]
	sti
	popad
	mov al, [.Status]
	ret
.Status:	db 0

tmp_ud_handler:
	mov al, 0
	mov dword [esp], cpuid_is_present.Checked	; change return address to .Checked
	iret
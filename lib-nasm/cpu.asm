%define PROTECTED_MODE_CODESEG_SEL			0x08
%define PROTECTED_MODE_DATASEG_SEL			0x10
%define REAL_MODE_CODESEG_SEL				0x18
%define REAL_MODE_DATASEG_SEL				0x20

[bits 16]
;	GDT must be loaded before calling!
;	Input:
;		[ESP + 4] - return address
SwitchToProtectedMode:
	pop ebx
	pop ebx

	cli
	mov eax, cr0
	or eax, 1
	mov cr0, eax
	jmp PROTECTED_MODE_CODESEG_SEL:.ProtectedMode
[bits 32]
.ProtectedMode:
	mov ax, PROTECTED_MODE_DATASEG_SEL
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	push ebx
	ret
[bits 16]

[bits 32]
;	GDT must be loaded before calling!
;	Input:
;		[ESP + 4] - return address (far pointer)
SwitchToRealMode:
	pop ebx
	mov eax, [esp]
	mov ebx, eax
	add esp, 4						; `pop` return address

	and eax, 0xF0000				; segment
	and ebx, 0x0FFFF				; offset
	shl ebx, 16						; EBX[HIGH] - offset
	mov bx, ax						; EAX[LOW]->EBX[LOW] - segment

	cli
	jmp REAL_MODE_CODESEG_SEL:.ProtectedMode16
[bits 16]
.ProtectedMode16:
	mov ax, REAL_MODE_DATASEG_SEL
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	mov eax, cr0
	and eax, 0x7FFFFFFE
	mov cr0, eax
	jmp 0:.RealMode
.RealMode:
	mov ax, 0
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	sti

	push bx							; segment
	shr ebx, 16
	push bx							; offset
	retf
[bits 16]
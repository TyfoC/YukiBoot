[bits 32]

[global fpu_init]
fpu_init:					; uint8_t fpu_init(void)
	push eax
	mov eax, cr0 
	and eax, 0xFFFFFFF3		; clear CR0.EM & CR0.TS
	mov cr0, eax
	fninit
	fnstsw word [.tmp]
	cmp word [.tmp], 0x037F	; https://studfile.net/preview/1583052/page:69/
	jne .NoFPU
.Done:
	pop eax
	mov al, 1
	ret
.NoFPU:
	pop eax
	mov al, 0
	ret
.tmp:	dw 0xAA55
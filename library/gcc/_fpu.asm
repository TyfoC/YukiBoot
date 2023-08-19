[bits 32]

[global fpu_init]
fpu_init:					; uint8_t fpu_init(void)
	push eax
	mov edx, cr0 
	and edx, 0xFFFFFFF3		; clear CR0.EM & CR0.TS
	mov cr0, edx
	fninit
	fnstsw [.tmp]
	cmp word [.tmp], 0
	pop eax
	jne .NoFPU
.Done:
	mov al, 1
	ret
.NoFPU:
	mov al, 0
	ret
.tmp:	dw 0xAA55
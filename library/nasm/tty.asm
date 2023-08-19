;	Input:
;		AL - character
PutChar:
	push ax
	mov ah, 0x0E
	int 0x10
	pop ax
	ret

;	Input:
;		DS:SI - string
PutString:
	push ax
	mov ah, 0x0E
.PutChar:
	lodsb
	test al, al
	jz .Done
	int 0x10
	jmp .PutChar
.Done:
	pop ax
	ret
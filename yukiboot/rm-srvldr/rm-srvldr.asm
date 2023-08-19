[bits 32]
[org 0x1000]

%define NUMBER_OF_SERVICES						((ServicesEnd - Services) / 2)

;	Real mode service loader
;	Input:
;		[ESP + 4] - service index
;	Output:
;		ah - status (false or true)
Begin:
	push eax
	push ebx
	push dword .RealMode
	call dword SwitchToRealMode
[bits 16]
.RealMode:
	pop ebx
	pop eax

	cmp dword [esp + 4], NUMBER_OF_SERVICES
	jae .ServiceNotFound

	shl dword [esp + 4], 1						; convert service index to offset
	add dword [esp + 4], Services
	
	push word .Done								; push return address
	push bx
	push ebx
	mov bx, [esp + 12]
	mov bx, [bx]
	mov [esp + 4], bx
	pop ebx
	ret											; call service
.ServiceNotFound:
	mov al, 0
.Done:
	push eax
	push ebx
	push dword .ProtectedMode
	call dword SwitchToProtectedMode
[bits 32]
.ProtectedMode:
	pop ebx
	pop eax
	ret
[bits 16]

;	Service #0 - read drive parameters
;	Input:
;		DL - drive index
;	Output:
;		AL - status
;		EBX - buffer address
;		Buffer:
;			0x00:		SPT (sectors per track) - byte, but written as word
;			0x02:		HPC (heads per cylinder) - word
;			0x04:		LCI (last cylinder index) - word
;			0x06:		NoHDD (number of hard disk drives) - byte, but written as word
ServiceReadDriveParameters:
	pusha
	push es
	mov ah, 0x08
	mov edi, 0
	mov es, di
	int 0x13
	jc .Error

	test ah, ah
	jnz .Error

	mov [Buffer], cx					; SPT
	and word [Buffer], 0x3F

	mov [Buffer + 2], dx				; HPC
	shr word [Buffer + 2], 8
	inc word [Buffer + 2]

	mov byte [Buffer + 4], ch			; LCI
	mov byte [Buffer + 5], cl
	shr byte [Buffer + 5], 6

	mov byte [Buffer + 6], dl			; NoHDD
	mov byte [Buffer + 7], 0
	
	jmp .Done

.Error:
	pop es
	popa
	mov al, 0
	ret
.Done:
	pop es
	popa
	mov al, 1
	mov ebx, Buffer
	ret

Services:
	dw ServiceReadDriveParameters
ServicesEnd:
	
	%include "cpu.asm"

Buffer:									; for one sector, VESA information and VESA video mode
	ALIGN(4)
	times 512 db 0

	times 1024 - $ + $$ db 0
End:
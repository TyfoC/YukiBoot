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
;		dl - drive index
;	Output:
;		al - status
;		ebx - tmp buffer address
;		Buffer:
;			0x00:		SPT (sectors per track) - byte, but written as word
;			0x02:		HPC (heads per cylinder) - word
;			0x04:		LCI (last cylinder index) - word
;			0x06:		NoHDD (number of hard disk drives) - byte, but written as word
ServiceReadDriveParameters:
	pusha
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
	popa
	mov al, 0
	ret
.Done:
	popa
	mov al, 1
	mov ebx, Buffer
	ret

;	Service #1 - check extension present
;	Input:
;		dl - drive index
;	Output:
;		al - status
;		cx - interface support bitmask
ServiceCheckExtensionPresent:
	pusha
	mov ah, 0x41
	mov bx, 0x55AA
	int 0x13
	jc .Error

	cmp bx, 0xAA55
	jne .Error

	mov [Buffer], cx
.Done:
	popa
	mov al, 1
	mov cx, [Buffer]
	ret
.Error:
	popa
	mov al, 0
	ret

;	Service #2 - read sector
;	Input:
;		cx - sector number[0-5] & cylinder index[8-15][6-7]
;		dh - head index
;		dl - drive index
;	Output:
;		al - status
;		ebx - tmp buffer address
ServiceReadSector:
	pusha
	mov ax, 0x0201
	mov bx, 0
	mov es, bx
	mov bx, Buffer
	int 0x13
	jc .Error

	test ah, ah
	jnz .Error
.Done:
	popa
	mov al, 1
	mov ebx, Buffer
	ret
.Error:
	popa
	mov al, 0
	ret

;	Service #3 - read sector ext
;	Input:
;		dl - drive index
;		ebx:eax - LBA (eax - low part, ebx - high part)
;	Output:
;		al - status
;		ebx - tmp buffer address
ServiceReadSectorExt:
	pusha
	sub sp, 0x10					; reserve space for DAP (disk address packet)
	mov bp, sp
	
	mov word [bp + 14], 0x0010		; DAP.Size && DAP.Reserved [LE]
	mov word [bp + 12], 1			; DAP.NumberOfSectors
	mov word [bp + 10], Buffer		; DAP.Offset
	mov word [bp + 8], 0			; DAP.Segment
	mov dword [bp + 4], eax			; DAP.LBA[LOW]
	mov dword [bp], ebx				; DAP.LBA[HIGH]
	mov ah, 0x42
	mov si, bp
	int 0x13
	jc .Error

	test ah, ah
	jnz .Error
.Done:
	add sp, 0x10
	popa
	mov al, 1
	mov ebx, Buffer
	ret
.Error:
	add sp, 0x10
	popa
	mov al, 0
	ret

;	Service #4 - get vesa info
;	Output:
;		al - status
;		ebx - tmp buffer address
ServiceGetVESAInfo:
	pusha
	mov ax, 0x4F00
	mov di, Buffer
	int 0x10
	jc .Error

	cmp ax, 0x4F
	jne .Error
.Done:
	popa
	mov al, 1
	mov ebx, Buffer
	ret
.Error:
	popa
	mov al, 0
	ret

;	Service #5 - get vesa mode info
;	Input:
;		cx - mode index
;	Output:
;		al - status
;		ebx - tmp buffer address
ServiceGetVESAModeInfo:
	pusha
	mov ax, 0x4F01
	mov di, Buffer
	int 0x10
	jc .Error

	cmp ax, 0x4F
	jne .Error
.Done:
	popa
	mov al, 1
	mov ebx, Buffer
	ret
.Error:
	popa
	mov al, 0
	ret

;	Service #6 - set vesa mode
;	Input:
;		cx - mode index
;	Output:
;		al - status
ServiceSetVESAMode:
	pusha
	mov ax, 0x4F02
	mov bx, cx
	int 0x10
	jc .Error

	cmp ax, 0x4F
	jne .Error
.Done:
	popa
	mov al, 1
	ret
.Error:
	popa
	mov al, 0
	ret

;	Service #7 - get map entry
;	Input:
;		ebx - entry offset
;	Output:
;		al - status
;		ecx - next entry offset or zero
;		ebx - tmp buffer offset
ServiceGetMapEntry:
	pushad
	mov eax, 0xE820
	mov ecx, 0x0018
	mov edx, 0x534D4150
	mov edi, Buffer + 4
	int 0x15
	jc .Error

	cmp eax, 0x534D4150
	jne .Error

	mov [Buffer], ebx

	cmp cl, 0x0018
	jae .Done

	mov dword [Buffer + 24], 1					; set first bit in extended attributes (for compatibility)
.Done:
	popad
	mov al, 1
	mov ebx, Buffer + 4
	mov ecx, [Buffer]
	ret
.Error:
	popad
	mov al, 0
	ret

Services:
	dw ServiceReadDriveParameters
	dw ServiceCheckExtensionPresent
	dw ServiceReadSector
	dw ServiceReadSectorExt

	dw ServiceGetVESAInfo
	dw ServiceGetVESAModeInfo
	dw ServiceSetVESAMode

	dw ServiceGetMapEntry
ServicesEnd:
	
	%include "cpu.asm"

	ALIGN(4)
Buffer:									; for one sector, VESA information and VESA video mode
	times 512 db 0

DiskAccessPacket:
	.SizeOfPacket:		db 0x10
	.Reserved:			db 0
	.NumberOfSectors:	dw 1
	.Address:			dq 0

	times 1536 - $ + $$ db 0
End:
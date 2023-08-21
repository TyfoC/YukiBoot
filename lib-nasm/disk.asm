%define SECTOR_SIZE						512
%define SECTOR_MASK						63

;	Input:
;		DL - disk index
SelectDisk:
	mov ah, 0x08
	mov [DriveParameters.Index], dl
	mov di, 0
	mov es, di
	int 0x13
	jc .Error

	test ah, ah
	jnz .Error

	and cl, SECTOR_MASK
	mov [DriveParameters.LastSectorNumber], cl
	mov [DriveParameters.LastHeadIndex], dl
	
	clc
	ret
.Error:
	stc
	ret

;	Input:
;		CX - sector[0-5] & cylinder[H:6-7][L:8-15]
;		DH - head index
IncCHS:
	push ax
	mov al, cl
	and al, SECTOR_MASK
	cmp al, [DriveParameters.LastSectorNumber]
	pop ax

	jae .IncHead
.IncSector:
	inc cl
	ret
.IncHead:
	cmp dh, [DriveParameters.LastHeadIndex]
	jae .IncCylinder

	inc dh
	and cl, 0xC0
	or cl, 1

	ret
.IncCylinder:
	xchg ch, cl
	shr ch, 6
	inc cx
	xchg cl, ch
	shl cl, 6
	mov dh, 0
	or cl, 1

	ret

;	Input:
;		ES:BX - buffer
IncBuffer:
	push eax
	mov eax, 0

	push es
	pop ax
	shl eax, 4
	mov ax, bx
	add eax, 0x200
	mov bx, ax

	and eax, 0xF0000
	shr eax, 4
	mov es, ax

	pop eax
	ret

;	Input:
;		CX - sector[0-5] & cylinder[H:6-7][L:8-15]
;		DH - head index
;		ES:BX - dest buffer
;		SI - number of sectors
ReadSectors:
	clc
.ReadSector:
	test si, si
	jz .Done

	mov ax, 0x0201
	mov dl, [DriveParameters.Index]
	int 0x13
	jc .Error

	test ah, ah
	jnz .Error

	call IncCHS
	call IncBuffer

	dec si
	jmp .ReadSector
.Done:
	ret
.Error:
	stc
	ret

DriveParameters:
.Index:				dd 0
.LastSectorNumber:	db SECTOR_MASK		; Sectors/Track
.LastHeadIndex:		db 0x0F				; Heads/Cylinder - 1
[bits 32]

%define RM_SRVLDR_ADDRESS				0x1000

[global __call_rm_service]
__call_rm_service:
	pop dword [.tmp]
	call RM_SRVLDR_ADDRESS
	push dword [.tmp]
	ret
.tmp:	dd 0
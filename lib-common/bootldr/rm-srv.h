#pragma once
#ifndef RM_SRV_H
#define RM_SRV_H

#define __RM_SRV_STRFY(...)							#__VA_ARGS__
#define _RM_SRV_STRFY(...)							__RM_SRV_STRFY(__VA_ARGS__)

#define CALL_RM_SERVICE($index, ...)	__asm__ __volatile__("pushl $" _RM_SRV_STRFY($index) "; call __call_rm_service" __VA_ARGS__)

#define RM_SERVICE_DRIVE_GET_PARAMS					0x0000
#define RM_SERVICE_DRIVE_CHECK_EXTENSION_PRESENT	0x0001
#define RM_SERVICE_DRIVE_READ_SECTOR_CHS			0x0002
#define RM_SERVICE_DRIVE_READ_SECTOR_EXT			0x0003

#define RM_SERVICE_VESA_GET_INFO					0x0004
#define RM_SERVICE_VESA_GET_MODE_INFO				0x0005
#define RM_SERVICE_VESA_SET_MODE					0x0006

#define RM_SERVICE_MEM_GET_MAP_ENTRY				0x0007

#define RM_SERVICE_SET_BIOS_VIDEO_MODE				0x0008

#endif
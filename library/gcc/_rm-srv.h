#pragma once
#ifndef __RM_SRV_H
#define __RM_SRV_H

#define CALL_RM_SERVICE($index, $statusVar, ...)	{__asm__ __volatile__("" __VA_ARGS__); $statusVar = __call_rm_service($index);}

#define RM_SERVICE_GET_DRIVE_PARAMS					0x0000

extern unsigned char __call_rm_service(unsigned int index);

#endif
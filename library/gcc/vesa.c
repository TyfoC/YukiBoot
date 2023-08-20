#include "vesa.h"

static VESAInfo_t info_ __attribute__((__aligned__(4)));
static VESAModeInfo_t mode_info_ __attribute__((__aligned__(4)));

uint8_t vesa_init(void) {
	uint32_t rmStatus, tmpBufferAddress;
	CALL_RM_SERVICE(RM_SERVICE_VESA_GET_INFO, :"=a"(rmStatus), "=b"(tmpBufferAddress));
	if (!rmStatus) return rmStatus;

	memcpy(&info_, (void*)tmpBufferAddress, sizeof(VESAInfo_t));
	return 1;
}

VESAInfo_t* vesa_get_info(void) {
	return &info_;
}

VESAModeInfo_t* vesa_get_mode_info(uint16_t mode) {
	uint32_t rmStatus, tmpBufferAddress;
	CALL_RM_SERVICE(RM_SERVICE_VESA_GET_MODE_INFO, :"=a"(rmStatus), "=b"(tmpBufferAddress):"c"((uint32_t)mode));
	if (!rmStatus) return NULL;

	memcpy(&mode_info_, (void*)tmpBufferAddress, sizeof(VESAInfo_t));
	return &mode_info_;
}

uint8_t vesa_set_mode(uint16_t mode) {
	uint32_t rmStatus;
	CALL_RM_SERVICE(RM_SERVICE_VESA_SET_MODE, :"=a"(rmStatus):"c"((uint32_t)mode));
	return rmStatus;
}
#include "_drive.h"

uint8_t drive_index_ = 0;
uint16_t sectors_per_track_ = 18;
uint16_t heads_per_cylinder_ = 2;
uint16_t last_cylinder_index_ = 79;
uint8_t number_of_hdds_ = 0;

uint8_t drive_select(uint8_t driveIndex) {
	drive_index_ = driveIndex;

	uint8_t rmStatus = 0;
	uint32_t bufferAddr = 0;
	CALL_RM_SERVICE(RM_SERVICE_GET_DRIVE_PARAMS, rmStatus, :"=b"(bufferAddr):"d"(driveIndex));
	if (!rmStatus) return rmStatus;

	return 1;

	sectors_per_track_ = ((uint16_t*)bufferAddr)[0];
	heads_per_cylinder_ = ((uint16_t*)bufferAddr)[1];
	last_cylinder_index_ = ((uint16_t*)bufferAddr)[2];
	number_of_hdds_ = (uint8_t)((uint16_t*)bufferAddr)[3];

	return 1;
}

uint16_t drive_get_spt(void) {
	return sectors_per_track_;
}

uint16_t drive_get_hpc(void) {
	return heads_per_cylinder_;
}

uint16_t drive_get_last_cylinder_index(void) {
	return last_cylinder_index_;
}

uint8_t drive_get_number_of_hdds(void) {
	return number_of_hdds_;
}
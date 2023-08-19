#include "_drive.h"

uint8_t drive_index_ = 0;
uint16_t sectors_per_track_ = 18;
uint16_t heads_per_cylinder_ = 2;
uint16_t last_cylinder_index_ = 79;
uint8_t number_of_hdds_ = 0;

uint8_t drive_select(uint8_t driveIndex) {
	drive_index_ = driveIndex;

	uint32_t rmStatus = 0, bufferAddr = 0;
	CALL_RM_SERVICE(RM_SERVICE_DRIVE_GET_PARAMS, :"=a"(rmStatus), "=b"(bufferAddr):"d"(driveIndex));
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

void drive_lba_to_chs(uint32_t lba, uint16_t* cylinder, uint8_t* head, uint8_t* sector) {
	*cylinder = lba / (heads_per_cylinder_ * sectors_per_track_);
	*head = (lba / sectors_per_track_) % heads_per_cylinder_;
	*sector = (lba % sectors_per_track_) + 1;
}

//	Returns the number of sectors read
size_t drive_read_sectors(uint32_t lba, size_t count, void* buffer) {
	size_t resultCnt = 0;

	uint16_t cylinder;
	uint8_t head, sector;
	drive_lba_to_chs(lba, &cylinder, &head, &sector);

	uint32_t rmStatus, tmpBufferAddress;
	while (count) {
		CALL_RM_SERVICE(
			RM_SERVICE_DRIVE_READ_SECTOR_CHS,
			:"=a"(rmStatus), "=b"(tmpBufferAddress)
			:"c"(JOIN_CYL_SECT(cylinder, sector)), "d"(JOIN_HEAD_DRIVE(head, drive_index_))
		);
		
		if (!rmStatus) return rmStatus;

		memcpy(buffer, (void*)tmpBufferAddress, DRIVE_SECTOR_SIZE);

		++resultCnt;
		--count;

		if (sector < sectors_per_track_) ++sector;
		else {
			if (head < heads_per_cylinder_) ++head;
			else {
				if (cylinder < last_cylinder_index_) ++cylinder;
				else return resultCnt;

				head = 0;
			}

			sector = 1;
		}

		buffer = (void*)((size_t)buffer + DRIVE_SECTOR_SIZE);
	}

	return resultCnt;
}
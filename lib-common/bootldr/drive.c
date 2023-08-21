#include "drive.h"

static uint8_t drive_index_ = 0;
static uint16_t sectors_per_track_ = 18;
static uint16_t heads_per_cylinder_ = 2;
static uint16_t last_cylinder_index_ = 79;
static uint8_t number_of_hdds_ = 0;
static uint16_t interface_support_bitmask_ = 0;
static size_t max_lba_ = 0;

uint8_t drive_select(uint8_t driveIndex) {
	drive_index_ = driveIndex;

	// tmpValue - buffer addr
	uint32_t rmStatus = 0, tmpValue = 0;
	CALL_RM_SERVICE(RM_SERVICE_DRIVE_GET_PARAMS, :"=a"(rmStatus), "=b"(tmpValue):"d"(driveIndex));
	if (!rmStatus) return rmStatus;

	sectors_per_track_ = ((uint16_t*)tmpValue)[0];
	heads_per_cylinder_ = ((uint16_t*)tmpValue)[1];
	last_cylinder_index_ = ((uint16_t*)tmpValue)[2];
	number_of_hdds_ = (uint8_t)((uint16_t*)tmpValue)[3];

	max_lba_ = drive_chs_to_lba(last_cylinder_index_, heads_per_cylinder_ - 1, sectors_per_track_);
	
	// tmpValue - interface support bitmask
	CALL_RM_SERVICE(
		RM_SERVICE_DRIVE_CHECK_EXTENSION_PRESENT,
		:"=a"(rmStatus), "=c"(tmpValue)
		:"d"(drive_index_)
	);
	
	interface_support_bitmask_ = rmStatus ? (uint16_t)tmpValue : 0;
	return 1;
}

uint8_t drive_get_index(void) {
	return drive_index_;
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

uint16_t drive_get_interface_support_bitmask(void) {
	return interface_support_bitmask_;
}

uint32_t drive_get_max_lba(void) {
	return (last_cylinder_index_ * heads_per_cylinder_ * (heads_per_cylinder_ - 1)) * sectors_per_track_ + (sectors_per_track_ - 1);
}

void drive_lba_to_chs(uint32_t lba, uint16_t* cylinder, uint8_t* head, uint8_t* sector) {
	*cylinder = lba / (heads_per_cylinder_ * sectors_per_track_);
	*head = (lba / sectors_per_track_) % heads_per_cylinder_;
	*sector = (lba % sectors_per_track_) + 1;
}

uint32_t drive_chs_to_lba(uint16_t cylinder, uint8_t head, uint8_t sector) {
	return (cylinder * heads_per_cylinder_ + head) * sectors_per_track_ + (sector - 1);
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
		
		if (!rmStatus) return resultCnt;

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

//	Returns the number of sectors read
size_t drive_read_sectors_ext(uint64_t lba, size_t count, void* buffer) {
	size_t resultCnt = 0;
	uint32_t rmStatus, tmpBufferAddress;

	uint16_t cylinder;
	uint8_t head, sector;
	drive_lba_to_chs((size_t)lba, &cylinder, &head, &sector);

	while ((size_t)lba <= max_lba_ && count) {
		CALL_RM_SERVICE(
			RM_SERVICE_DRIVE_READ_SECTOR_CHS,
			:"=a"(rmStatus), "=b"(tmpBufferAddress)
			:"c"(JOIN_CYL_SECT(cylinder, sector)), "d"(JOIN_HEAD_DRIVE(head, drive_index_))
		);
		
		if (!rmStatus) return resultCnt;

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
		++lba;
	}

	if (count && !(interface_support_bitmask_ & DRIVE_IF_SUPPORT_DAP)) return resultCnt;

	while (count) {
		CALL_RM_SERVICE(
			RM_SERVICE_DRIVE_READ_SECTOR_EXT,
			:"=a"(rmStatus), "=b"(tmpBufferAddress)
			:"a"((uint32_t)(lba & 0xFFFFFFFF)), "d"(drive_index_), "b"((uint32_t)(lba >> 32))
		);
		
		if (!rmStatus) return resultCnt;

		memcpy(buffer, (void*)tmpBufferAddress, DRIVE_SECTOR_SIZE);

		++lba;
		++resultCnt;
		--count;

		buffer = (void*)((size_t)buffer + DRIVE_SECTOR_SIZE);
	}

	return resultCnt;
}
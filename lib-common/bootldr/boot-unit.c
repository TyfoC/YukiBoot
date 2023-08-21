#include "boot-unit.h"

//	You must select drive!
BootUnit_t build_boot_unit(const MBRPartitionEntry_t* partitionTableEntry) {
	BootUnit_t bootUnit;

	bootUnit.DriveIndex = drive_get_index();
	bootUnit.Type = partitionTableEntry->Type;

	if (
		partitionTableEntry->StartHead < 0xFE &&
		partitionTableEntry->StartSectorCylinder != 0xFFFF &&
		!partitionTableEntry->StartAddress
	) {
		bootUnit.Address = drive_chs_to_lba(
			EXTRACT_CYL_FROM_CYL_SECT(partitionTableEntry->StartSectorCylinder),
			partitionTableEntry->StartHead,
			EXTRACT_SECT_FROM_CYL_SECT(partitionTableEntry->StartSectorCylinder)
		);
	}
	else bootUnit.Address = partitionTableEntry->StartAddress;

	if (
		partitionTableEntry->NumberOfSectors
	) bootUnit.NumberOfSectors = partitionTableEntry->NumberOfSectors;
	else {
		bootUnit.NumberOfSectors = (
			drive_chs_to_lba(
				EXTRACT_CYL_FROM_CYL_SECT(partitionTableEntry->LastSectorCylinder),
				partitionTableEntry->LastHead,
				EXTRACT_SECT_FROM_CYL_SECT(partitionTableEntry->LastSectorCylinder)
			) + 1 - bootUnit.Address
		);
	}

	return bootUnit;
}

void free_rbfs_boot_units(RBFSBootUnit_t* units, size_t count) {
	for (size_t i = 0; i < count; i++) {
		if (units[i].PartitionName) free(units[i].PartitionName);
	}
}
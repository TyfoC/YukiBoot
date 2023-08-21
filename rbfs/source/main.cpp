#include <utility.h>

void PrintHelp() {
	std::cout << "List of available commands:" << std::endl;
	std::cout << "\t`-help` - get info about commands." << std::endl;
	std::cout << "\t`-info` - get info about utility & file system." << std::endl;

	std::cout << "\t`-cftbl ...` - create file table image from directory contents." << std::endl;
	std::cout << "\t\t<DIR_PATH> - path to the directory from whose contents you want to create an image," << std::endl;
	std::cout << "\t\t<FTBL_PATH> - the path where the image will be written." << std::endl;

	std::cout << "\t`-cstgpt ...` - create a storage partition & file table image from directory contents." << std::endl;
	std::cout << "\t\t<DIR_PATH> - path to the directory from the contents of which you want to create images," << std::endl;
	std::cout << "\t\t<FTBL_OFFSET> - offset in sectors of the file table relative to the partition header," << std::endl;
	std::cout << "\t\t<PART_NAME> - name of the partition to be created," << std::endl;
	std::cout << "\t\t<PART_ATTRIBS> - number denoting partition attributes," << std::endl;
	std::cout << "\t\t<PART_PATH> - the path where the partition image will be written," << std::endl;
	std::cout << "\t\t<FTBL_PATH> - the path where the partition image will be written." << std::endl;
	std::cout << "\t\t(MAX_SECTS_NUM) - maximum number of sectors" << std::endl;

	std::cout << "\t`-cfsh ...` - create file system header image." << std::endl;
	std::cout << "\t\t<PTBL_OFFSET> - partition table offset (in sectors)," << std::endl;
	std::cout << "\t\t<PARTS_NUM> - number of partitions," << std::endl;
	std::cout << "\t\t<IMG_PATH> - the path where the file system header image will be written," << std::endl;
	std::cout << "\t\t(MAX_PARTS_NUM) - maximum number of partitions," << std::endl;

	std::cout << "\t`-eftbl ...` - extract files from file table." << std::endl;
	std::cout << "\t\t<FTBL_PATH> - path to file table image," << std::endl;
	std::cout << "\t\t<ELEMS_NUM> - number of elements (directories + files) in the table," << std::endl;
	std::cout << "\t\t<DIR_PATH> - path to the directory where you want to put the contents of the image." << std::endl;

	std::cout << "\t`-epart ...` - extract data from partition." << std::endl;
	std::cout << "\t\t<PART_PATH> - path to partition header image," << std::endl;
	std::cout << "\t\t<DATA_PATH> - path to partition data image," << std::endl;
	std::cout << "\t\t<DIR_PATH> - path to the directory where the data will be written." << std::endl;

	std::cout << "\t`-efs ...` - extract partitions and their data from the file system." << std::endl;
	std::cout << "\t\t<RBFS_IMG> - path to RBFS image," << std::endl;
	std::cout << "\t\t<DIR_PATH> - path to the folder where the contents of the file system will be written." << std::endl;

	std::cout << "\t`-wmbrpe` - write entry to MBR partition table," << std::endl;
	std::cout << "\t\t<MBR_IMG_PATH> - path to file with MBR," << std::endl;
	std::cout << "\t\t<INDEX> - entry index [DEC]," << std::endl;
	std::cout << "\t\t<TYPE> - partition type [HEX]:" << std::endl;
	std::cout << "\t\t\t5 - near EBR, F - far EBR, >7F - AltOS<" << std::endl;
	std::cout << "\t\t<STATUS> - partition status [HEX]:" << std::endl;
	std::cout << "\t\t\t0x80 - active/bootable, 0 - inactive, 1-0x7F - invalid" << std::endl;
	std::cout << "\t\t<DATA_LBA> - LBA start sector index [DEC]," << std::endl;
	std::cout << "\t\t<DATA_LENGTH> - data length in sectors [DEC]" << std::endl;
}

void PrintInfo() {
	std::string utilVerBuild = std::to_string(Utility::VERSION & 0xFF);
	std::string utilVerMinor = std::to_string((Utility::VERSION >> 8) & 0xFF);
	std::string utilVerMajor = std::to_string((Utility::VERSION >> 16) & 0xFF);
	std::string verUtil = utilVerMajor + '.' + utilVerMinor + '.' + utilVerBuild;

	std::string rbfsVerBuild = std::to_string(RBFS_VERSION & 0xFF);
	std::string rbfsVerMinor = std::to_string((RBFS_VERSION >> 8) & 0xFF);
	std::string rbfsVerMajor = std::to_string((RBFS_VERSION >> 16) & 0xFF);
	std::string rbfsVer = rbfsVerMajor + '.' + rbfsVerMinor + '.' + rbfsVerBuild;

	std::cout << "RBFS utility by Noimage PNG" << std::endl;
	std::cout << "Utility version: " << verUtil << std::endl;
	std::cout << "RBFS (RebornFS) rev.: " << rbfsVer << std::endl;
	std::cout << "Type `rbfs -help` to get help on commands." << std::endl;
}

int ReturnError(int errorCode, const std::string msgPart = "") {
	if (errorCode == Utility::ERROR_CODE_SUCCESS) return Utility::ERROR_CODE_SUCCESS;

	std::cout << "Error: ";
	switch (errorCode) {
	case Utility::ERROR_CODE_UNKNOWN_CMD_OR_MISSING_ARGS:
		std::cout << "unknown command or missing arguments. Type `-help` to get info about commands";
		break;
	case Utility::ERROR_CODE_FAILED_TO_CREATE:
		std::cout << "failed to open/create `" << msgPart << '`';
		break;
	case Utility::ERROR_CODE_FAILED_TO_READ:
		std::cout << "failed to read `" << msgPart << '`';
		break;
	case Utility::ERROR_CODE_FAILED_TO_WRITE:
		std::cout << "failed to write `" << msgPart << '`';
		break;
	case Utility::ERROR_CODE_FAILED_TO_ALLOC:
		std::cout << "failed to allocate memory";
		break;
	case Utility::ERROR_CODE_TYPE_SIZE_GRTR_THAN_FILE_SIZE:
		std::cout << "the size of the data type is larger than the size of the file (`" << msgPart << "`)";
		break;
	case Utility::ERROR_CODE_INVALID_DATA_TYPE:
		std::cout << "invalid data type (`" << msgPart << "`)";
		break;
	default:
		std::cout << "unknown error";
		break;
	}
	std::cout << '!' << std::endl;

	return errorCode;
}

void SaveFileInfos(const std::string dirPath, const std::vector<Utility::FileInfo_t>& infos) {
	std::string dirPathCopy = dirPath;
	if (dirPath[dirPath.length() - 1] != std::filesystem::path::preferred_separator) {
		dirPathCopy += std::filesystem::path::preferred_separator;
	}

	std::ofstream outInfo;
	std::string infoPath;
	for (const Utility::FileInfo_t& info : infos) {
		infoPath = dirPathCopy + info.FullPath;
		if (info.IsDirectory) std::filesystem::create_directories(infoPath);
		else {
			std::filesystem::create_directories(infoPath.substr(0, infoPath.rfind(std::filesystem::path::preferred_separator)));
			outInfo = std::ofstream(infoPath, std::ios_base::binary);
			if (outInfo.is_open()) {
				outInfo.write((char*)info.DataProperty, info.SizeProperty);
				outInfo.close();
			}
			else std::cout << "Warning: failed to write `" << infoPath << "`!" << std::endl;
		}
	}
}

int SavePartitionData(const RBFSPartitionHeader_t* partHeader, uint8_t* partData, const std::string dirPath) {
	if (partHeader->Type == RBFS_PARTITION_TYPE_STORAGE) {
		std::vector<Utility::FileInfo_t> infos = Utility::ExtractFileTable(partData, (size_t)partHeader->NumberOfFileHeaders);
		SaveFileInfos(dirPath, infos);
	}
	else {
		std::string partDataPathStr = dirPath + std::string(1, std::filesystem::path::preferred_separator);
		if (partHeader->Type == RBFS_PARTITION_TYPE_BOOTABLE) partDataPathStr += "bootable.bin";
		else if (partHeader->Type == RBFS_PARTITION_TYPE_SWAP) partDataPathStr += "swap.bin";
		else if (partHeader->Type == RBFS_PARTITION_TYPE_RAW) partDataPathStr += "raw.bin";
		else partDataPathStr = "unknown-type.bin";

		std::ofstream outImage(partDataPathStr, std::ios_base::binary);
		if (!outImage.is_open()) return ReturnError(Utility::ERROR_CODE_FAILED_TO_WRITE, partDataPathStr);

		outImage.write((char*)partData, partHeader->NumberOfUsedSectors * SECTOR_SIZE);
		outImage.close();
	}

	return Utility::ERROR_CODE_SUCCESS;
}

int main(int argc, char** argv) {
	if (argc == 2 && !strcmp(argv[1], "-help")) PrintHelp();
	else if (argc == 2 && !strcmp(argv[1], "-info")) PrintInfo();
	else if (argc == 4 && !strcmp(argv[1], "-cftbl")) {
		std::string dirPath(argv[2]);
		std::string resImgPath(argv[3]);

		Utility::FileTable_t image = Utility::BuildFileTable(dirPath);

		std::ofstream out(resImgPath, std::ios_base::binary);
		if (!out.is_open()) return ReturnError(Utility::ERROR_CODE_FAILED_TO_CREATE, resImgPath);

		size_t cntHdrs = image.FileHeaders.size();
		size_t cntChunks = image.DataChunks.size();
		size_t sizeHdrs = sizeof(RBFSFileHeader_t) * cntHdrs;
		size_t sizeChunks = sizeof(RBFSDataChunk_t) * cntChunks;
		size_t sizeRes = sizeHdrs + sizeChunks;
		uint8_t* resData = new uint8_t[sizeRes];
		if (!resData) return ReturnError(Utility::ERROR_CODE_FAILED_TO_ALLOC);

		RBFSFileHeader_t* fileHdrs = (RBFSFileHeader_t*)resData;
		RBFSDataChunk_t* dataChunks = (RBFSDataChunk_t*)&fileHdrs[cntHdrs];
		for (size_t i = 0; i < cntHdrs; i++) fileHdrs[i] = image.FileHeaders[i];
		for (size_t i = 0; i < cntChunks; i++) dataChunks[i] = image.DataChunks[i];

		out.write((char*)resData, (std::streamsize)sizeRes);
		delete[] resData;
		out.close();
	}
	else if ((argc == 8 || argc == 9) && !strcmp(argv[1], "-cstgpt")) {
		std::string dirPath(argv[2]);
		uint64_t fileTableOffset = strtoull(argv[3], 0, 10);
		std::string resPartName(argv[4]);
		uint64_t resPartAttribs = strtoull(argv[5], 0, 10);
		std::string resPartImgPath(argv[6]);
		std::string resFileTableImgPath(argv[7]);

		Utility::FileTable_t image = Utility::BuildFileTable(dirPath);
		
		std::ofstream resFileTableImg(resFileTableImgPath, std::ios_base::binary);
		if (!resFileTableImg.is_open()) return ReturnError(Utility::ERROR_CODE_FAILED_TO_CREATE, resFileTableImgPath);

		size_t cntHdrs = image.FileHeaders.size();
		size_t cntChunks = image.DataChunks.size();
		size_t sizeHdrs = sizeof(RBFSFileHeader_t) * cntHdrs;
		size_t sizeChunks = sizeof(RBFSDataChunk_t) * cntChunks;
		size_t sizeRes = sizeHdrs + sizeChunks;
		size_t lenRes = sizeRes / SECTOR_SIZE;
		uint8_t* resData = new uint8_t[sizeRes];
		if (!resData) return ReturnError(Utility::ERROR_CODE_FAILED_TO_ALLOC);

		RBFSFileHeader_t* fileHdrs = (RBFSFileHeader_t*)resData;
		RBFSDataChunk_t* dataChunks = (RBFSDataChunk_t*)&fileHdrs[cntHdrs];
		for (size_t i = 0; i < cntHdrs; i++) fileHdrs[i] = image.FileHeaders[i];
		for (size_t i = 0; i < cntChunks; i++) dataChunks[i] = image.DataChunks[i];

		resFileTableImg.write((char*)resData, (std::streamsize)sizeRes);
		delete[] resData;

		uint64_t maxNumOfSectors;
		if (argc == 8) maxNumOfSectors = lenRes;
		else {
			maxNumOfSectors = strtoull(argv[8], 0, 10);
			if (lenRes > maxNumOfSectors) {
				std::cout << "Warning: the maximum number of sectors will be equal to the number of sectors used (Max<Len)!" << std::endl;
				maxNumOfSectors = lenRes;
			}
			else {
				uint8_t* tmpData = new uint8_t[SECTOR_SIZE];
				if (!tmpData) return ReturnError(Utility::ERROR_CODE_FAILED_TO_ALLOC);

				memset(tmpData, 0, SECTOR_SIZE);
				uint64_t diff = maxNumOfSectors - lenRes;
				for (size_t i = 0; i < diff; i++) resFileTableImg.write((char*)tmpData, SECTOR_SIZE);
				delete[] tmpData;
			}
		}

		std::ofstream resPartImg(resPartImgPath, std::ios_base::binary);
		if (!resPartImg.is_open()) return ReturnError(Utility::ERROR_CODE_FAILED_TO_CREATE, resPartImgPath);

		RBFSPartitionHeader_t partHdr = Utility::BuildStoragePartitionHeader(resPartName, resPartAttribs, fileTableOffset, lenRes, cntHdrs);
		if (partHdr.NameHash == RBFS_ROOT_DIR_HASH) return ReturnError(Utility::ERROR_CODE_FAILED_TO_CREATE, "RBFSPartitionHeader_t");

		partHdr.NumberOfSectors = maxNumOfSectors;

		resPartImg.write((char*)&partHdr, sizeof(RBFSPartitionHeader_t));
		resPartImg.close();
		resFileTableImg.close();
	}
	else if ((argc == 5 || argc == 6) && !strcmp(argv[1], "-cfsh")) {
		RBFSHeader_t fsHdr = {
			RBFS_HEADER_SIGNATURE,
			RBFS_VERSION,
			0,							// will be calculated later

			{},							// will be calculated in OS

			strtoll(argv[2], 0, 10),
			strtoull(argv[3], 0, 10),
			0,							// will be calculated later

			Utility::OffsetOf<uint32_t>(&RBFSHeader_t::Reserved),

			{}
		};

		if (argc == 6) {
			fsHdr.MaximumNumberOfPartitions = strtoull(argv[5], 0, 10);
			if (fsHdr.MaximumNumberOfPartitions < fsHdr.NumberOfPartitions) {
				std::cout << "Warning: the maximum number of partitions will be equal to the number of partitions (Max<Len)!" << std::endl;
				fsHdr.MaximumNumberOfPartitions = fsHdr.NumberOfPartitions;
			}
		}
		else fsHdr.MaximumNumberOfPartitions = fsHdr.NumberOfPartitions;

		fsHdr.Checksum = (uint32_t)(uint8_t)(0 - rbfs_header_get_checksum(&fsHdr));

		std::string outFsHdrPath(argv[4]);
		std::ofstream outFsHdr(outFsHdrPath);
		if (!outFsHdr.is_open()) return ReturnError(Utility::ERROR_CODE_FAILED_TO_CREATE, outFsHdrPath);
		outFsHdr.write((char*)&fsHdr, sizeof(RBFSHeader_t));
		outFsHdr.close();
	}
	else if (argc == 5 && !strcmp(argv[1], "-eftbl")) {
		std::string pathFileTable(argv[2]);
		uint64_t numOfElems = strtoull(argv[3], 0, 10);
		std::string resDirPath(argv[4]);

		std::ifstream inFileTable(pathFileTable, std::ios_base::binary);
		if (!inFileTable.is_open()) return ReturnError(Utility::ERROR_CODE_FAILED_TO_CREATE, pathFileTable);
		uint8_t* fileTableData = Utility::ReadWholeFile(inFileTable);
		inFileTable.close();
		if (!fileTableData) return ReturnError(Utility::ERROR_CODE_FAILED_TO_READ, pathFileTable);

		std::vector<Utility::FileInfo_t> infos = Utility::ExtractFileTable(fileTableData, (size_t)numOfElems);
		delete[] fileTableData;

		SaveFileInfos(resDirPath, infos);
	}
	else if (argc == 5 && !strcmp(argv[1], "-epart")) {
		std::string partImgPath(argv[2]);
		std::string partDataImgPath(argv[3]);
		std::string resDirPath(argv[4]);

		std::ifstream inPartImg(partImgPath, std::ios_base::binary);
		if (!inPartImg.is_open()) return ReturnError(Utility::ERROR_CODE_FAILED_TO_READ, partImgPath);
		std::ifstream inPartDataImg(partDataImgPath, std::ios_base::binary);
		if (!inPartDataImg.is_open()) {
			inPartImg.close();
			return ReturnError(Utility::ERROR_CODE_FAILED_TO_CREATE, partDataImgPath);
		}

		size_t partImgSize = Utility::GetFileSizeInBytes(inPartImg);
		size_t partDataImgSize = Utility::GetFileSizeInBytes(inPartDataImg);

		if (partImgSize < sizeof(RBFSPartitionHeader_t)) return ReturnError(
			Utility::ERROR_CODE_TYPE_SIZE_GRTR_THAN_FILE_SIZE,
			"RBFSPartitionHeader_t"
		);

		uint8_t* partImgData = Utility::ReadWholeFile(inPartImg);
		if (!partImgData) return ReturnError(Utility::ERROR_CODE_FAILED_TO_ALLOC);

		RBFSPartitionHeader_t* partHdr = (RBFSPartitionHeader_t*)partImgData;
		if (partDataImgSize < SECTOR_SIZE * partHdr->NumberOfUsedSectors) return ReturnError(
			Utility::ERROR_CODE_TYPE_SIZE_GRTR_THAN_FILE_SIZE,
			"data"
		);

		uint8_t* partDataImgData = Utility::ReadWholeFile(inPartDataImg);
		if (!partDataImgData) return ReturnError(Utility::ERROR_CODE_FAILED_TO_READ, partDataImgPath);

		resDirPath += (
			std::string(1, std::filesystem::path::preferred_separator) +
			std::string((char*)partHdr->Name) +
			std::string(1, std::filesystem::path::preferred_separator)
		);

		std::filesystem::create_directories(resDirPath);

		SavePartitionData(partHdr, partDataImgData, resDirPath);

		delete[] partDataImgData;
		delete[] partImgData;
		inPartDataImg.close();
		inPartImg.close();
	}
	else if (argc == 4 && !strcmp(argv[1], "-efs")) {
		std::string rbfsImgPath(argv[2]);
		std::string resDirPath(argv[3]);
		std::filesystem::create_directories(resDirPath);
		resDirPath += std::filesystem::path::preferred_separator;

		std::ifstream inRbfsImg(rbfsImgPath, std::ios_base::binary);
		if (!inRbfsImg.is_open()) return ReturnError(Utility::ERROR_CODE_FAILED_TO_CREATE, rbfsImgPath);

		size_t rbfsImgSize = Utility::GetFileSizeInBytes(inRbfsImg);
		uint8_t* rbfsImgData = Utility::ReadWholeFile(inRbfsImg);
		if (!rbfsImgData) return ReturnError(Utility::ERROR_CODE_FAILED_TO_READ, rbfsImgPath);

		if (rbfsImgSize < sizeof(RBFSHeader_t)) return ReturnError(
			Utility::ERROR_CODE_TYPE_SIZE_GRTR_THAN_FILE_SIZE,
			"RBFSHeader_t"
		);

		RBFSHeader_t* fsHdr = (RBFSHeader_t*)rbfsImgData;
		if (!rbfs_validate_header(fsHdr)) return ReturnError(Utility::ERROR_CODE_INVALID_DATA_TYPE, "RBFSHeader_t");

		uint8_t* partData;
		std::string resPartPath = "";
		std::vector<Utility::FileInfo_t> partFileInfos;
		RBFSPartitionHeader_t* partHdrs = (RBFSPartitionHeader_t*)((size_t)fsHdr + fsHdr->PartitionTableOffset * SECTOR_SIZE);
		for (uint64_t i = 0; i < fsHdr->NumberOfPartitions; i++) {
			resPartPath = resDirPath + std::string((char*)partHdrs[i].Name);
			std::filesystem::create_directory(resPartPath);

			partData = (uint8_t*)((size_t)&partHdrs[i] + partHdrs[i].DataOffset * SECTOR_SIZE);
			SavePartitionData(&partHdrs[i], partData, resPartPath);
		}

		delete[] rbfsImgData;
		inRbfsImg.close();
	}
	else if (argc == 8 && !strcmp(argv[1], "-wmbrpe")) {
		std::string imgPath(argv[2]);
		size_t partitionIndex = strtoul(argv[3], 0, 10);
		size_t partitionType = strtoul(argv[4], 0, 16);
		size_t partitionStatus = strtoul(argv[5], 0, 16);
		size_t dataLBA = strtoul(argv[6], 0, 10);
		size_t dataLength = strtoul(argv[7], 0, 10);

		if (partitionIndex >= 4) return ReturnError(Utility::ERROR_CODE_TYPE_SIZE_GRTR_THAN_FILE_SIZE, "partition index");

		std::ifstream inFile(imgPath, std::ios_base::binary);
		if (!inFile.is_open()) return ReturnError(Utility::ERROR_CODE_FAILED_TO_CREATE, imgPath);

		size_t fileSize = Utility::GetFileSizeInBytes(inFile);
		if (fileSize < 512) return ReturnError(Utility::ERROR_CODE_TYPE_SIZE_GRTR_THAN_FILE_SIZE, "MBR");

		uint8_t* fileData = Utility::ReadWholeFile(inFile);
		if (!fileData) return ReturnError(Utility::ERROR_CODE_FAILED_TO_READ);

		inFile.close();

		std::ofstream outFile(imgPath, std::ios_base::binary);
		if (!outFile.is_open()) return ReturnError(Utility::ERROR_CODE_FAILED_TO_CREATE, imgPath);

		uint8_t* entryData = (uint8_t*)((size_t)fileData + 446 + partitionIndex * 16);

		entryData[0] = (uint8_t)partitionStatus;
		entryData[1] = (uint8_t)0xFF;
		entryData[2] = (uint8_t)0xFF;
		entryData[3] = (uint8_t)0xFF;
		entryData[4] = (uint8_t)partitionType;
		entryData[5] = (uint8_t)0xFF;
		entryData[6] = (uint8_t)0xFF;
		entryData[7] = (uint8_t)0xFF;
		((uint32_t*)entryData)[2] = dataLBA;
		((uint32_t*)entryData)[3] = dataLength;

		outFile.write((char*)&fileData[0], fileSize);
		outFile.close();
	}
	else return ReturnError(Utility::ERROR_CODE_UNKNOWN_CMD_OR_MISSING_ARGS);

	return Utility::ERROR_CODE_SUCCESS;
}
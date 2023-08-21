#include <utility.h>

rbfs_hash_t rbfs_hash_str(const char* str) {
	rbfs_hash_t result = RBFS_ROOT_DIR_HASH;
	uint8_t* ptr = (uint8_t*)str;

	size_t length = strlen(str);
	while (length--) result = result * 101 + *ptr++;
	return result;
}

RBFSFileHeader_t Utility::FILE_HEADER_CUR_DIR = {
	{ u8'.' },
	RBFS_ROOT_DIR_HASH, RBFS_ROOT_DIR_HASH,		// need to be changed!
	(
		RBFS_FILE_ATTRIBUTE_DIRECTORY |
		RBFS_FILE_ATTRIBUTE_SYSTEM |
		RBFS_FILE_ATTRIBUTE_SYMLINK |
		RBFS_FILE_ATTRIBUTE_HIDDEN
	),
	{}, {}, {},										// time
	{}, {}, {},										// date
	0, 0, 0
};

RBFSFileHeader_t Utility::FILE_HEADER_PARENT_DIR = {
	{ u8'.', u8'.' },
	RBFS_ROOT_DIR_HASH, RBFS_ROOT_DIR_HASH,		// need to be changed!
	(
		RBFS_FILE_ATTRIBUTE_DIRECTORY |
		RBFS_FILE_ATTRIBUTE_SYSTEM |
		RBFS_FILE_ATTRIBUTE_SYMLINK |
		RBFS_FILE_ATTRIBUTE_HIDDEN
	),
	{}, {}, {},										// time
	{}, {}, {},										// date
	0, 0, 0
};

Utility::FileInfo_t::FileInfo_t() {
	FullPath = "";
	IsDirectory = true;
	SizeProperty = 0;
	DataProperty = 0;
}

Utility::FileInfo_t::FileInfo_t(const FileInfo_t& fileInfo) {
	operator=(fileInfo);
}

Utility::FileInfo_t::~FileInfo_t() {
	if (!IsDirectory && SizeProperty && DataProperty) {
		delete[] DataProperty;
		DataProperty = 0;
		SizeProperty = 0;
	}
}

Utility::FileInfo_t& Utility::FileInfo_t::operator=(const FileInfo_t& fileInfo) {
	if (!IsDirectory && SizeProperty && DataProperty) {
		delete[] DataProperty;
		DataProperty = 0;
		SizeProperty = 0;
	}

	FullPath = fileInfo.FullPath;
	IsDirectory = fileInfo.IsDirectory;
	SizeProperty = fileInfo.SizeProperty;

	if (IsDirectory || !SizeProperty) DataProperty = 0;
	else {
		DataProperty = new uint8_t[SizeProperty];
		if (!DataProperty) return *this;

		memcpy(DataProperty, fileInfo.DataProperty, SizeProperty);
	}

	return *this;
}

Utility::FileTable_t Utility::BuildFileTable(const std::string dirPath) {
	FileTable_t fileTable;
	std::string pathPrefix = RemoveExtraPathSeparators(dirPath) + std::string(1, std::filesystem::path::preferred_separator);
	size_t pathPrefixLength = pathPrefix.length();

	std::vector<FileInfo_t> infos = GetInfoAboutFiles(dirPath);
	size_t count = infos.size();

	std::string relFilePath;
	std::string relParentPath;
	std::string tmpFileName;
	size_t relParentPathSepPos;
	size_t tmpFileNameSize;
	RBFSFileHeader_t tmpFileHdr;
	for (size_t i = 0; i < count; i++) {
		relFilePath = infos[i].FullPath;
		relFilePath = relFilePath.substr(pathPrefixLength);

		relParentPathSepPos = relFilePath.rfind(std::filesystem::path::preferred_separator);
		if (relParentPathSepPos == std::string::npos) relParentPath = "";
		else relParentPath = relFilePath.substr(0, relParentPathSepPos);

		tmpFileName = relFilePath.substr(relFilePath.rfind(std::filesystem::path::preferred_separator) + 1);
		tmpFileNameSize = (tmpFileName.length() + 1) * sizeof(tmpFileName[0]);

		if (tmpFileNameSize > sizeof(RBFSFileHeader_t::Name)) {
			std::cout << "Warning: too long file name (`" << tmpFileName << "`), the file will not be processed!" << std::endl;
			infos.erase(infos.begin() + i);
			--i;
			--count;
			continue;
		}

		memcpy(tmpFileHdr.Name, tmpFileName.c_str(), tmpFileNameSize);
		memset(&tmpFileHdr.Name[tmpFileNameSize], 0, sizeof(RBFSFileHeader_t::Name) - tmpFileNameSize);
		tmpFileHdr.PathHash = rbfs_hash_str(relFilePath.c_str());

		if (relParentPath[0] != '\0') {
			tmpFileHdr.ParentPathHash = rbfs_hash_str(relParentPath.c_str());
		}
		else tmpFileHdr.ParentPathHash = RBFS_ROOT_DIR_HASH;

		tmpFileHdr.CreationTime = tmpFileHdr.LastAccessTime = tmpFileHdr.LastModificationTime = {};
		tmpFileHdr.CreationDate = tmpFileHdr.LastAccessDate = tmpFileHdr.LastModificationDate = {};

		if (infos[i].IsDirectory) {
			tmpFileHdr.Attributes = RBFS_FILE_ATTRIBUTE_DIRECTORY;
			tmpFileHdr.SizeInBytes = 0;
			tmpFileHdr.LengthInSectors = infos[i].SizeProperty;
		}
		else {
			tmpFileHdr.Attributes = 0;
			tmpFileHdr.SizeInBytes = infos[i].SizeProperty;
			tmpFileHdr.LengthInSectors = AlignUp(
				tmpFileHdr.SizeInBytes, (uint64_t)sizeof(RBFSDataChunk_t::Data)
			) / (uint64_t)sizeof(RBFSDataChunk_t::Data);
		}

		tmpFileHdr.FirstChunkOffset = 0;	// will be set later
		fileTable.FileHeaders.push_back(tmpFileHdr);

		if (infos[i].IsDirectory) {
			FILE_HEADER_CUR_DIR.PathHash = tmpFileHdr.PathHash;
			FILE_HEADER_CUR_DIR.ParentPathHash = tmpFileHdr.ParentPathHash;
			fileTable.FileHeaders.push_back(FILE_HEADER_CUR_DIR);

			FILE_HEADER_PARENT_DIR.PathHash = tmpFileHdr.ParentPathHash;
			FILE_HEADER_PARENT_DIR.ParentPathHash = tmpFileHdr.ParentPathHash;
			fileTable.FileHeaders.push_back(FILE_HEADER_PARENT_DIR);
		}
	}

	count = fileTable.FileHeaders.size();
	uint64_t curChunkOffset = count, sizeLeft, infoIndex;

	RBFSDataChunk_t dataChunk;
	for (size_t i = 0; i < count; i++) {
		if (fileTable.FileHeaders[i].Attributes & RBFS_FILE_ATTRIBUTE_DIRECTORY) continue;

		sizeLeft = fileTable.FileHeaders[i].SizeInBytes;
		fileTable.FileHeaders[i].FirstChunkOffset = curChunkOffset - i;
		infoIndex = FindFileInfoByHash(infos, pathPrefix, fileTable.FileHeaders[i].PathHash);

		for (uint64_t j = 0; j < fileTable.FileHeaders[i].LengthInSectors; j++) {
			if (sizeLeft > sizeof(RBFSDataChunk_t::Data)) {
				memcpy(
					dataChunk.Data, &infos[(size_t)infoIndex].DataProperty[j * sizeof(RBFSDataChunk_t::Data)],
					sizeof(RBFSDataChunk_t::Data)
				);

				dataChunk.NextOffset = 1;
				sizeLeft -= sizeof(RBFSDataChunk_t::Data);
			}
			else {
				memcpy(dataChunk.Data, &infos[(size_t)infoIndex].DataProperty[(size_t)j * sizeof(RBFSDataChunk_t::Data)], (size_t)sizeLeft);
				memset(&dataChunk.Data[sizeLeft], 0, sizeof(RBFSDataChunk_t::Data) - (size_t)sizeLeft);
				dataChunk.NextOffset = 0;
				sizeLeft = 0;
			}

			fileTable.DataChunks.push_back(dataChunk);
			++curChunkOffset;
		}
	}

	return fileTable;
}

RBFSPartitionHeader_t Utility::BuildStoragePartitionHeader(
	const std::string name, uint64_t attributes,
	int64_t fileTableOffset, uint64_t fileTableLength,
	uint64_t numberOfFileHeaders
) {
	RBFSPartitionHeader_t result = {};
	size_t nameSize = sizeof(name[0]) * (name.length() + 1);
	if (nameSize > sizeof(RBFSPartitionHeader_t::Name)) return result;

	memcpy(result.Name, name.c_str(), nameSize);
	memset(&result.Name[nameSize], 0, sizeof(RBFSPartitionHeader_t::Name) - nameSize);

	result.NameHash = rbfs_hash_str(name.c_str());
	result.Type = RBFS_PARTITION_TYPE_STORAGE;
	result.Attributes = attributes;
	result.DataOffset = fileTableOffset;
	result.NumberOfUsedSectors = result.NumberOfSectors = fileTableLength;
	result.NumberOfFileHeaders = numberOfFileHeaders;

	return result;
}

std::vector<Utility::FileInfo_t> Utility::ExtractFileTable(
	const uint8_t* fileTableData,
	size_t numOfElements
) {
	std::vector<FileInfo_t> result;
	FileInfo_t tmpFileInfo;
	RBFSFileHeader_t* fileHdrs = (RBFSFileHeader_t*)fileTableData;

	uint64_t j, sizeLeft;
	std::string filePath;
	RBFSDataChunk_t* dataChunks;
	rbfs_hash_t fileParentPathHash;
	std::vector<std::string> reversedFilePathParts;
	for (size_t i = 0; i < numOfElements; i++) {
		filePath = std::string((char*)fileHdrs[i].Name);
		if ((filePath == "." || filePath == "..") && fileHdrs[i].Attributes & RBFS_FILE_ATTRIBUTE_SYMLINK) continue;

		fileParentPathHash = fileHdrs[i].ParentPathHash;
		reversedFilePathParts = { filePath };

		while (fileParentPathHash) {
			for (j = 0; j < (uint64_t)numOfElements; j++) {
				filePath = std::string((char*)fileHdrs[j].Name);
				if ((filePath == "." || filePath == "..") && fileHdrs[i].Attributes & RBFS_FILE_ATTRIBUTE_SYMLINK) continue;

				if (fileHdrs[j].PathHash == fileParentPathHash) {
					reversedFilePathParts.push_back(std::string((char*)fileHdrs[j].Name));
					fileParentPathHash = fileHdrs[j].ParentPathHash;
					j = 0;
					break;
				}
			}

			if (j == (uint64_t)numOfElements) break;
		}

		std::reverse(reversedFilePathParts.begin(), reversedFilePathParts.end());

		filePath = "";
		for (const std::string& pathPart : reversedFilePathParts) filePath += (
			pathPart + std::string(1, std::filesystem::path::preferred_separator)
		);

		filePath = filePath.substr(0, filePath.length() - 1);

		tmpFileInfo.FullPath = filePath;
		tmpFileInfo.IsDirectory = fileHdrs[i].Attributes & RBFS_FILE_ATTRIBUTE_DIRECTORY;

		if (tmpFileInfo.IsDirectory) {
			tmpFileInfo.SizeProperty = (size_t)fileHdrs[i].LengthInSectors;
			tmpFileInfo.DataProperty = 0;
		}
		else {
			tmpFileInfo.SizeProperty = (size_t)fileHdrs[i].SizeInBytes;
			if (tmpFileInfo.SizeProperty) {
				tmpFileInfo.DataProperty = new uint8_t[tmpFileInfo.SizeProperty];
				if (!tmpFileInfo.DataProperty) {
					std::cout << "Warning: failed to allocate memory for `" << filePath << "`!" << std::endl;
					continue;
				}

				sizeLeft = tmpFileInfo.SizeProperty;
				dataChunks = (RBFSDataChunk_t*)((size_t)&fileHdrs[i] + fileHdrs[i].FirstChunkOffset * SECTOR_SIZE);
				for (j = 0; j < fileHdrs[i].LengthInSectors; j++) {
					if (sizeLeft > sizeof(RBFSDataChunk_t::Data)) {
						memcpy(
							&tmpFileInfo.DataProperty[j * sizeof(RBFSDataChunk_t::Data)], dataChunks[j].Data,
							sizeof(RBFSDataChunk_t::Data)
						);

						sizeLeft -= sizeof(RBFSDataChunk_t::Data);
					}
					else {
						memcpy(
							&tmpFileInfo.DataProperty[(size_t)j * sizeof(RBFSDataChunk_t::Data)], dataChunks[j].Data,
							(size_t)sizeLeft
						);

						sizeLeft = 0;
					}
				}
			}
			else tmpFileInfo.DataProperty = 0;
		}

		result.push_back(tmpFileInfo);
	}

	return result;
}

//	=================================================================

std::string Utility::RemoveExtraPathSeparators(const std::string pathStr) {
	std::string result = "";
	size_t length = pathStr.length();
	for (size_t i = 0; i < length; i++) {
		if (pathStr[i] == std::filesystem::path::preferred_separator) {
			result += std::filesystem::path::preferred_separator;
			while (pathStr[i] == std::filesystem::path::preferred_separator) ++i;
			--i;
		}
		else result += pathStr[i];
	}

	return result.back() == std::filesystem::path::preferred_separator ? result.substr(0, result.length() - 1) : result;
}

size_t Utility::CountSubStrings(const std::string str, const std::string subStr) {
	size_t count = 0, pos = 0, subLen = subStr.length();
	do {
		pos = str.find(subStr, pos);
		if (pos == std::string::npos) break;

		++count;
		pos += subLen;
	} while (true);

	return count;
}

size_t Utility::GetFileSizeInBytes(std::ifstream& inStream) {
	if (!inStream.is_open()) return 0;

	std::streampos curPos = inStream.tellg();
	inStream.seekg(0, std::ios_base::end);
	size_t fileSize = (size_t)inStream.tellg();
	inStream.seekg(curPos);

	return fileSize;
}

uint8_t* Utility::ReadWholeFile(std::ifstream& inStream) {
	if (!inStream.is_open()) return 0;

	size_t fileSize = GetFileSizeInBytes(inStream);
	uint8_t* fileData = new uint8_t[fileSize];
	if (!fileData) return 0;

	std::streampos curPos = inStream.tellg();
	inStream.seekg(0, std::ios_base::beg);
	inStream.read((char*)fileData, (std::streamsize)fileSize);
	inStream.seekg(curPos);

	return fileData;
}

std::vector<Utility::FileInfo_t> Utility::GetInfoAboutFiles(const std::string dirPath) {
	std::vector<FileInfo_t> result, tmpElements;

	std::filesystem::directory_iterator dirIter;
	try {
		dirIter = std::filesystem::directory_iterator(dirPath);
	}
	catch (const std::filesystem::filesystem_error& fsError) {
		std::cout << "Error (" << fsError.code() << "): `" << fsError.what() << "`!" << std::endl;
		return {};
	}

	FileInfo_t tmp;
	std::ifstream inStream;
	std::filesystem::path elPath;

	for (const std::filesystem::directory_entry& dirEntry : dirIter) {
		elPath = dirEntry.path();

		tmp.FullPath = elPath.string();
		tmp.IsDirectory = dirEntry.is_directory();

		if (tmp.IsDirectory) {
			tmpElements = GetInfoAboutFiles(tmp.FullPath);
			result.insert(result.end(), tmpElements.begin(), tmpElements.end());
			tmp.SizeProperty = tmpElements.size();
			tmp.DataProperty = 0;
			tmpElements = {};
		}
		else {
			tmp.SizeProperty = (size_t)dirEntry.file_size();

			inStream = std::ifstream(tmp.FullPath, std::ios_base::binary);
			if (!inStream.is_open()) {
				tmp.DataProperty = 0;
				continue;
			}

			if (tmp.SizeProperty) {
				tmp.DataProperty = ReadWholeFile(inStream);
				inStream.close();
				if (!tmp.DataProperty) continue;
			}
			else tmp.DataProperty = 0;
		}

		result.push_back(tmp);
	}

	return result;
}

size_t Utility::FindFileInfoByHash(const std::vector<FileInfo_t>& infos, const std::string pathPfx, rbfs_hash_t pathHash) {
	size_t count = infos.size(), pfxLen = pathPfx.length();
	for (size_t i = 0; i < count; i++) if (rbfs_hash_str(&infos[i].FullPath.c_str()[pfxLen]) == pathHash) return i;
	return std::string::npos;
}
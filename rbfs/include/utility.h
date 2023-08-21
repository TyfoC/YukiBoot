#pragma once
#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <vector>
#include <cstring>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <rbfs-header.h>
#include <rbfs-partition-header.h>
#include <rbfs-file-header.h>
#include <rbfs-data-chunk.h>

#define SECTOR_SIZE													512

namespace Utility {
	constexpr uint32_t VERSION = 0x01'00'00;				// 1.0.0
	extern RBFSFileHeader_t FILE_HEADER_CUR_DIR, FILE_HEADER_PARENT_DIR;

	enum ErrorCode {
		ERROR_CODE_SUCCESS,
		ERROR_CODE_UNKNOWN_CMD_OR_MISSING_ARGS,
		ERROR_CODE_FAILED_TO_CREATE,
		ERROR_CODE_FAILED_TO_READ,
		ERROR_CODE_FAILED_TO_WRITE,
		ERROR_CODE_FAILED_TO_ALLOC,
		ERROR_CODE_TYPE_SIZE_GRTR_THAN_FILE_SIZE,
		ERROR_CODE_INVALID_DATA_TYPE
	};

	struct FileInfo_t {
		FileInfo_t();
		FileInfo_t(const FileInfo_t& fileInfo);
		~FileInfo_t();

		FileInfo_t& operator=(const FileInfo_t& fileInfo);

		std::string				FullPath;
		bool					IsDirectory;
		size_t					SizeProperty;	// this.IsDirectory ? number of files inside : size of file
		uint8_t* DataProperty;	// this.IsDirectory ? 0 : file content
	};

	struct FileTable_t {
		std::vector<RBFSFileHeader_t>	FileHeaders;
		std::vector<RBFSDataChunk_t>	DataChunks;
	};

	FileTable_t BuildFileTable(const std::string dirPath);
	RBFSPartitionHeader_t BuildStoragePartitionHeader(
		const std::string name, uint64_t attributes,
		int64_t fileTableOffset, uint64_t fileTableLength,
		uint64_t numberOfFileHeaders
	);

	std::vector<FileInfo_t> ExtractFileTable(
		const uint8_t* fileTableData,
		size_t numOfElements
	);

	//	===========================================

	template<typename T = uint64_t>
	T AlignUp(T value, T alignment) {
		T tmp = value % alignment;
		return tmp ? value + (alignment - tmp) : value;
	}

	template<typename TRes = size_t, typename TStruct, typename TField>
	constexpr TRes OffsetOf(TField TStruct::* member) {
		return (TRes)((size_t) & (((TStruct*)nullptr)->*member) - (size_t)nullptr);
	}

	std::string RemoveExtraPathSeparators(const std::string pathStr);
	size_t CountSubStrings(const std::string str, const std::string subStr);

	size_t GetFileSizeInBytes(std::ifstream& inStream);
	uint8_t* ReadWholeFile(std::ifstream& inStream);

	std::vector<FileInfo_t> GetInfoAboutFiles(const std::string dirPath);
	size_t FindFileInfoByHash(const std::vector<FileInfo_t>& infos, const std::string pathPfx, rbfs_hash_t pathHash);
}

#endif
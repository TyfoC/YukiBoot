#	RBFS 1.0.0 (`RebornFS`)

###	Data types
*	####	[`RBFSHeader_t`]()
*	####	[`RBFSPartitionHeader_t`]()
*	####	[`RBFSFileHeader_t`]()
*	####	[`RBFSDataChunk_t`]()
*	####	[`RBFSTime_t` & `RBFSDate_t`]()

###	An example of a disk structure with RBFS
| Sector #                   | Length                                      | Data type                 |
|----------------------------|---------------------------------------------|---------------------------|
| 0                          | 1                                           | MBR                       |
| 1                          | 1                                           | RBFSHeader_t              |
| 2                          | 1                                           | RBFSPartitionHeader_t     |
| 3                          | ...NumberOfFileHeaders                      | RBFSFileHeader_t[]        |
| 3+...NumberOfFileHeaders   | ?                                           | RBFSDataChunk_t[]         |
| 3+...NumberOfFileHeaders+? | ...NumberOfSectors - ...NumberOfUsedSectors | Free sectors              |

###	`RBFSHeader_t`
>	Describes the RBFS version, disk characteristics, and also points to an array of structures `RBFSPartitionHeader_t` and usable sectors.
```cpp
const uint32_t RBFS_HEADER_SIGNATURE = 0x53464252;	// 'RBFS'

typedef struct __attribute__((__packed__)) {
	//			RBFS_HEADER_SIGNATURE
	/*0x00*/	uint32_t	Signature;
	/*0x04*/	uint32_t	Version;
	/*0x08*/	uint32_t	Checksum;

	//			Field is 0 before initialization
	/*0x0C*/	uint8_t		DiskGUID[16];
	
	//			LBA OFFSET(!)
	/*0x1C*/	int64_t		PartitionTableOffset;
	/*0x24*/	uint64_t	NumberOfPartitions;
	/*0x2C*/	uint64_t	MaximumNumberOfPartitions;

	/*0x34*/	uint32_t	HeaderSize;
	
	/*0x38*/	uint8_t		Reserved[456];
} RBFSHeader_t;
```

###	`RBFSPartitionHeader_t`
>	Describes the characteristics of a partition and points to the data in that partition.
```cpp
enum RBFSPartitionType : uint32_t {
	RBFS_PARTITION_TYPE_STORAGE,
	RBFS_PARTITION_TYPE_BOOTABLE,
	RBFS_PARTITION_TYPE_SWAP,
	RBFS_PARTITION_TYPE_RAW
};

enum RBFSPartitionAttribute : uint32_t {
	RBFS_PARTITION_ATTRIBUTE_HIDDEN = 1,
	RBFS_PARTITION_ATTRIBUTE_READ_ONLY = 2,
	RBFS_PARTITION_ATTRIBUTE_TEMPORARY = 4,
	RBFS_PARTITION_ATTRIBUTE_UNMOUNTED = 8
};

typedef struct __attribute__((__packed__)) {
	//			Name in UTF-8 encoding
	/*0x000*/	char8_t		Name[460];

	/*0x1CС*/	hash_t		NameHash;
	
	//			RBFS_PARTITION_TYPE_*
	/*0x1D4*/	uint32_t	Type;
	
	//			RBFS_PARTITION_ATTRIBUTE_*
	/*0x1D8*/	uint64_t	Attributes;
	
	//			LBA OFFSET(!)
	/*0x1E0*/	int64_t		DataOffset;
	/*0x1E8*/	uint64_t	NumberOfUsedSectors;
	/*0x1F0*/	uint64_t	NumberOfSectors;
	/*0x1F8*/	uint64_t	NumberOfFileHeaders;
} RBFSPartitionHeader_t;
```

###	`RBFSFileHeader_t`
>	Describes the characteristics of the file and points to the first chunk of its data.
```cpp
//	Hash of path for root directory (equal to 0 because path is empty)
constexpr hash_t ROOT_DIR_HASH = 0x0000000000000000;

enum RBFSFileAttribute : uint64_t {
	RBFS_FILE_ATTRIBUTE_DIRECTORY = 1,
	RBFS_FILE_ATTRIBUTE_SYSTEM = 2,
	RBFS_FILE_ATTRIBUTE_SYMLINK = 4,
	RBFS_FILE_ATTRIBUTE_ARCHIVE = 8,
	RBFS_FILE_ATTRIBUTE_HIDDEN = 16,
	RBFS_FILE_ATTRIBUTE_READ_ONLY = 32,
	RBFS_FILE_ATTRIBUTE_TEMPORARY = 64
};

typedef struct __attribute__((__packed__)) {
	//			Name in UTF-8 encoding
	/*0x000*/	char8_t		Name[440];

	//			Important for file header search
	/*0x1B8*/	hash_t		PathHash;

	//			Important for file header search
	/*0x1C0*/	hash_t		ParentPathHash;

	//			RBFS_FILE_ATTRIBUTE_*
	/*0x1C8*/	uint64_t	Attributes;

	//			Not used in this revision(3)
	/*0x1D0*/	Time_t		CreationTime;
	/*0x1D4*/	Time_t		LastAccessTime;
	/*0x1D8*/	Time_t		LastModificationTime;
	
	//			Not used in this revision(3)
	/*0x1DC*/	Date_t		CreationDate;
	/*0x1E0*/	Date_t		LastAccessDate;
	/*0x1E4*/	Date_t		LastModificationDate;
	
	/*
		If it is a directory, then the field is 0;

		If it is a file then this field is equal to the size of the file.
	*/
	/*0x1E8*/	uint64_t	SizeInBytes;

	/*
		If this is a directory, then this field is equal
		to the number of elements inside the directory;

		if this is a file, then the number of sectors
		in which the file data is written.
	*/
	/*0x1F0*/	uint64_t	LengthInSectors;

	//			LBA OFFSET(!)
	/*0x1F8*/	int64_t		FirstChunkOffset;
} RBFSFileHeader_t;
```

###	`RBFSDataChunk_t`
>	Contains the data part of the file and points to the next part if the `NextChunkOffset` field is not equal to 0.
```cpp
typedef struct __attribute__((__packed__)) {
	/*0x000*/	uint8_t	Data[504];

	//			The field is 0 if this is the last chunk of data
	/*0x1F8*/	int64_t	NextOffset;
} RBFSDataChunk_t;
```

###	`RBFSTime_t` & `RBFSDate_t`
>	Describes the time and date.
```cpp
typedef struct __attribute__((__packed__)) {
	uint32_t	Day		:5;
	uint32_t	Month	:4;
	uint32_t	Year	:23;
} RBFSDate_t;

typedef struct __attribute__((__packed__)) {
	uint16_t	Seconds		:6;
	uint16_t	Minutes		:6;
	uint16_t	Hours		:5;
	uint16_t	Reserved	:15;
} RBFSTime_t;
```
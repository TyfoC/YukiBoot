#pragma once
#ifndef _INTTYPES_H
#define _INTTYPES_H

//	Format constants for the fprintf family of functions

#define PRId8			"d"
#define PRId16			"d"
#define PRId32			"d"
#define PRId64			"d"

#define PRIi8			PRId8
#define PRIi16			PRId16
#define PRIi32			PRId32
#define PRIi64			PRId64

#define PRIu8			"u"
#define PRIu16			"u"
#define PRIu32			"u"
#define PRIu64			"u"

#define PRIo8			"o"
#define PRIo16			"o"
#define PRIo32			"o"
#define PRIo64			"o"

#define PRIx8			"x"
#define PRIx16			"x"
#define PRIx32			"x"
#define PRIx64			"x"

#define PRIX8			"x"
#define PRIX16			"x"
#define PRIX32			"x"
#define PRIX64			"x"

//	Format constants for the fscanf family of functions

#define SCNd8			"d"
#define SCNd16			"d"
#define SCNd32			"d"
#define SCNd64			"d"

#define SCNi8			SCNd8
#define SCNi16			SCNd16
#define SCNi32			SCNd32
#define SCNi64			SCNd64

#define SCNu8			"u"
#define SCNu16			"u"
#define SCNu32			"u"
#define SCNu64			"u"

#define SCNo8			"o"
#define SCNo16			"o"
#define SCNo32			"o"
#define SCNo64			"o"

#define SCNx8			"x"
#define SCNx16			"x"
#define SCNx32			"x"
#define SCNx64			"x"

#define SCNX8			"x"
#define SCNX16			"x"
#define SCNX32			"x"
#define SCNX64			"x"

#endif
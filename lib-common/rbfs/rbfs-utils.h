#pragma once
#ifndef RBFS_UTILS_H
#define RBFS_UTILS_H

#include <string.h>
#include "rbfs-typedefs.h"

#define RBFS_DEFAULT_HASH						0xDEAD'BEEF'A'BAD'BABE

rbfs_hash_t rbfs_hash_str(const char* str);

#endif
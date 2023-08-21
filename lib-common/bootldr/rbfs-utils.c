#include <rbfs-utils.h>

rbfs_hash_t rbfs_hash_str(const char* str) {
	rbfs_hash_t result = RBFS_ROOT_DIR_HASH;
	uint8_t* ptr = (uint8_t*)str;

	size_t length = strlen(str);
	while (length--) result = result * 101 + *ptr++;
	return result;
}
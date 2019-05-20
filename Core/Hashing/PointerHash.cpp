#include "PointerHash.h"

namespace Baroque
{
	namespace Hashing
	{
		std::uint64_t PointerHash(const void* ptr)
		{
			std::uint64_t key = reinterpret_cast<std::uint64_t>(ptr);

			key = (~key) + (key << 21); // key = (key << 21) - key - 1;
			key = key ^ (key >> 24);
			key = (key + (key << 3)) + (key << 8); // key * 265
			key = key ^ (key >> 14);
			key = (key + (key << 2)) + (key << 4); // key * 21
			key = key ^ (key >> 28);
			key = key + (key << 31);

			return key;
		}
	}
}
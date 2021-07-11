#include <iostream>
#include <string.h>
#include <cstddef>
#include <vector>

#include "freelistallocator.hpp"

int main()
{
	const std::size_t A = static_cast<std::size_t>(1e9);
	const std::size_t B = static_cast<std::size_t>(1e8);

	const std::vector<std::size_t> ALLOCATION_SIZES {32, 64, 256, 512, 1024, 2048, 4096};
	const std::vector<std::size_t> ALIGNMENTS {8, 8, 8, 8, 8, 8, 8};

	FreeListAllocator al { 1024, FreeListAllocator::PlacementPolicy::FIND_FIRST };
	al.Init();

	char *res = (char *)al.Allocate(1000, 8);
	memset(res, 0, 1000);
	strcpy(res, "jjjjjjjjjjjjjjjfaskfjkasjfkajfkajfwqorepwujmcklzjkjk");
	printf("%s\n", res);
	al.Free(res);

	return 0;
}
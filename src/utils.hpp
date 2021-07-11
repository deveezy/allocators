#pragma once

#include <cstdint>

using std::size_t;

namespace Utils
{
	static const size_t 
	GetPadding(const size_t base_address, const size_t alignment)
	{
		const size_t multiplier = (base_address / alignment) + 1;
		const size_t aligned_address = multiplier * alignment; // next addr
		return (aligned_address - base_address); // diff between cur and next addr.
	}

	static const size_t
	GetPaddingWithHeader(const size_t base_address, const size_t alignment, const size_t header_size)
	{
		size_t padding = GetPadding(base_address, alignment);
		size_t needed_space = header_size;

		if (needed_space > padding)
		{
			// Header does not fit - Get next aligned address that header fits
			needed_space -= padding;

			// How many alignments I need to fit the header  
			if (needed_space % alignment > 0)
			{
				padding += alignment * (needed_space / alignment + 1);
			}
			else 
			{
				padding += alignment * (needed_space / alignment);
			}
		}
		return padding;
	}
};

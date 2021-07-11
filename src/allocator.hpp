#pragma once

#include <cstddef>

using std::size_t;

class Allocator
{
protected:
	size_t total_size;
	size_t used;
	size_t peak;

public:
	Allocator(const size_t total_size_) 
		: total_size { total_size_ }
		, used { 0 }
		, peak { 0 } 
	{}

	virtual ~Allocator() { total_size = 0; }

	virtual void * Allocate(const size_t size, const size_t alignment = 0) = 0;

	virtual void Free(void *ptr) = 0;

	virtual void Init() = 0;
};
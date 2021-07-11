#include "freelistallocator.hpp"
#include "utils.hpp"
#include <stdlib.h>
#include <assert.h>
#include <limits>
#include <algorithm>

FreeListAllocator::
FreeListAllocator(const size_t total_size_, const PlacementPolicy policy_) 
	: Allocator(total_size_)
{
	policy = policy_;
}

void FreeListAllocator::
Init()
{
	if (start_ptr != nullptr)
	{
		free(start_ptr);
		start_ptr = nullptr;
	}
	start_ptr = malloc(total_size);
	this->Reset();
}

void FreeListAllocator::
Reset()
{
	used = 0;
	peak = 0;
	Node *first_node = (Node *) start_ptr;
	first_node->data.block_size = total_size;
	first_node->next = nullptr;
	free_list.head = nullptr;
	free_list.insert(nullptr, first_node);
}

FreeListAllocator::
~FreeListAllocator()
{
	free(start_ptr);
	start_ptr = nullptr;
}

void * FreeListAllocator::
Allocate(const size_t size, const size_t alignment) 
{
	const size_t allocation_header_size = sizeof(FreeListAllocator::AllocationHeader);
	const size_t free_header_size = sizeof(FreeListAllocator::FreeHeader);
	assert(size >= sizeof(Node));
	assert(alignment >= 8);

	// Search through the free list for a free block that has enough space to allocate our data
	size_t padding;
	Node *affected_node;
	Node *previous_node;

	this->Find(size, alignment, padding, previous_node, affected_node);
	assert(affected_node != nullptr);

	const size_t alignment_padding = padding - allocation_header_size;
	const size_t required_size = size + padding;

	const size_t rest = affected_node->data.block_size - required_size;
	if (rest > 0) 
	{
		// We have to split the block into the data block and a free block of size 'rest'
		Node *new_free_node = reinterpret_cast<Node*>( (reinterpret_cast<size_t> (affected_node) + required_size));
		new_free_node->data.block_size = rest;
		free_list.insert(affected_node, new_free_node);
	}
	free_list.remove(previous_node, affected_node);

	// data block
	const size_t header_address = (size_t)affected_node + alignment_padding;
	const size_t data_address   = header_address + allocation_header_size;
	((FreeListAllocator::AllocationHeader *) header_address)->block_size = required_size;
	((FreeListAllocator::AllocationHeader *) header_address)->padding = alignment_padding;

	used += required_size;
	peak = std::max(peak, used);

	return (void*) data_address;
}

void FreeListAllocator::Free(void* ptr) 
{
	// Insert it in a sorted position by the address number
	const size_t current_address = (size_t)ptr;
	const size_t header_address  = current_address - sizeof(FreeListAllocator::AllocationHeader);
	const FreeListAllocator::AllocationHeader *allocation_header
		{ (FreeListAllocator::AllocationHeader *) header_address };
	
	Node *free_node = (Node *)(header_address);
	free_node->data.block_size = allocation_header->block_size + allocation_header->padding;
	free_node->next = nullptr;

	Node *it = free_list.head;
	Node *it_prev = nullptr;
	while (it != nullptr)
	{
		if (ptr < it)
		{
			free_list.insert(it_prev, free_node);
			break;
		}
		it_prev = it;
		it = it->next;
	}
	used -= free_node->data.block_size;

	// Merge contiguous nodes
	Coalescence(it_prev, free_node);
}

void FreeListAllocator::Find(const size_t size, 
	const size_t alignment, size_t &padding, Node *&prev, Node *&found)
{
	switch (policy)
	{
		case PlacementPolicy::FIND_FIRST:
			FindFirst(size, alignment, padding, prev, found);
			break;
		case PlacementPolicy::FIND_BEST:
			FindBest(size, alignment, padding, prev, found);
			break;
	}
}

void FreeListAllocator::FindBest(const size_t size, const size_t alignment, 
				size_t& padding, Node*& prev, Node*& found) 
{
	// Iterate whole list keeping a pointer to the best fit
	const size_t smallest_diff = std::numeric_limits<size_t>::max();
	Node *best_block = nullptr;
	Node *it = free_list.head;
	Node *it_prev = nullptr;

	while (it != nullptr)
	{
		padding = Utils::GetPaddingWithHeader(
			(size_t)it, alignment, sizeof(FreeListAllocator::AllocationHeader));
		const size_t required_space = size + padding;
		if (it->data.block_size >= required_space &&
			(it->data.block_size - required_space < smallest_diff))
		{
			best_block = it;
		}
		it_prev = it;
		it = it->next;
	}
	prev = it_prev;
	found = best_block;
}

void FreeListAllocator::FindFirst(const size_t size, const size_t alignment, 
				size_t& padding, Node*& prev, Node*& found) 
{
	// Iterate list and return the first free block with a size >= than give size
	Node *it = free_list.head;
	Node *it_prev = nullptr;

	while (it != nullptr)
	{
		padding = Utils::GetPaddingWithHeader(
			(size_t)it, alignment, 
			sizeof(FreeListAllocator::AllocationHeader));

		const size_t required_space = size + padding;
		if (it->data.block_size >= required_space)
		{
			break;
		}
		it_prev = it;
		it = it->next;
	}
	prev = it_prev;
	found = it;
}

void FreeListAllocator::Coalescence(Node *prev, Node *free_node) 
{
	if ( free_node->next != nullptr && 
		(size_t) free_node + free_node->data.block_size == (size_t)free_node->next )
	{
		free_node->data.block_size += free_node->next->data.block_size;
		free_list.remove(free_node, free_node->next);
	}
	if (prev != nullptr &&
		(size_t)prev + prev->data.block_size == (size_t)free_node)
	{
		prev->data.block_size += free_node->data.block_size;
		free_list.remove(prev, free_node);
	}
}
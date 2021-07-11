#pragma once

#include "allocator.hpp"
#include "sll.hpp"

class FreeListAllocator : public Allocator
{
public:
	enum PlacementPolicy
	{
		FIND_FIRST, FIND_BEST
	};

private:
	struct FreeHeader
	{
		size_t block_size;
	};

	struct AllocationHeader
	{
		size_t block_size;
		char padding;
	};

	using Node = SinglyLinkedList<FreeHeader>::Node;

	void *start_ptr = nullptr;
	PlacementPolicy policy;
	SinglyLinkedList<FreeHeader> free_list;

public:
	FreeListAllocator(const size_t total_size_, const PlacementPolicy policy_);
	virtual ~FreeListAllocator();
	virtual void * Allocate(const size_t size, const size_t alignment = 0) override;
	virtual void Free(void* ptr) override;
	virtual void Init() override;
	virtual void Reset();
private:
    FreeListAllocator(FreeListAllocator &freeListAllocator);

    void Coalescence(Node *prev, Node *free);

    void Find(const size_t size, const size_t alignment, 
				size_t& padding, Node*& prev, Node*& found);

    void FindBest(const size_t size, const size_t alignment, 
				size_t& padding, Node*& prev, Node*& found);

    void FindFirst(const size_t size, const size_t alignment, 
				size_t& padding, Node*& prev, Node*& found);
};
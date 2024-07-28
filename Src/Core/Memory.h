#pragma once

#include "Src/Core/Utils.h"
#include <stdio.h>

enum Allocator
{
	Invalid,
	TempStack,
	Persistent
};

class BaseAllocator
{
public:
	virtual void* Alloc(Int32 size) = 0;
	virtual void Free(void* ptr) = 0;
};

BaseAllocator* GetAllocator(Allocator type);

class TempStackAllocator : public BaseAllocator
{
	static thread_local TempStackAllocator* s_Allocator;
	UByte* m_Memory;
	Int32 m_MaxSize;
	Int32 m_Offset;
	Int32 m_StackAllocCount;

public:
	static TempStackAllocator* Get();

	TempStackAllocator(Int32 maxSize);

	void* Alloc(Int32 size);
	void Free(void* ptr);
};

class SlowMallocAllocator : public BaseAllocator
{
	static SlowMallocAllocator* s_Allocator;
public:
	static SlowMallocAllocator* Get();

	void* Alloc(Int32 size);
	void Free(void* ptr);
};

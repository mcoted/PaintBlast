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
	virtual void* Alloc(int size) = 0;
	virtual void Free(void* ptr) = 0;
};

BaseAllocator* GetAllocator(Allocator type);

class TempStackAllocator : public BaseAllocator
{
	static TempStackAllocator* s_Allocator;
	UByte* m_Memory;
	int m_MaxSize;
	int m_Current;
	int m_StackAllocCount;

public:
	static TempStackAllocator* Get();

	TempStackAllocator(int maxSize);

	void* Alloc(int size);
	void Free(void* ptr);
};

class SlowMallocAllocator : public BaseAllocator
{
	static SlowMallocAllocator* s_Allocator;
public:
	static SlowMallocAllocator* Get();

	void* Alloc(int size);
	void Free(void* ptr);
};
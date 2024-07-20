#include "Src/Core/Memory.h"
#include "Src/Core/Log.h"
#include "Src/Core/Utils.h"
#include <string.h>

BaseAllocator* GetAllocator(Allocator type)
{
	switch (type)
	{
	case Allocator::TempStack:  return TempStackAllocator::Get();
	case Allocator::Persistent: return SlowMallocAllocator::Get();
	}
	return NULL;
}

TempStackAllocator* TempStackAllocator::s_Allocator = NULL;

TempStackAllocator* TempStackAllocator::Get()
{
	if (s_Allocator == NULL)
		s_Allocator = new TempStackAllocator(KB(32));
	
	return s_Allocator;
}

TempStackAllocator::TempStackAllocator(int maxSize)
{
	m_Memory = (UByte*)malloc(maxSize);
	m_MaxSize = maxSize;
	m_Current = 0;
	m_StackAllocCount = 0;
}

void* TempStackAllocator::Alloc(int size)
{
	if ((m_Current + size) >= m_MaxSize)
	{
		LogErrorAndAbort("Not enough temp memory to allocate %d bytes\n", size);
		return NULL;
	}

	void* ptr = m_Memory + m_Current;
	m_Current += size;

	++m_StackAllocCount;

	return ptr;
}

void TempStackAllocator::Free(void* ptr)
{
	if (m_StackAllocCount <= 0)
	{
		LogErrorAndAbort("TempStackAllocator::Free double free?\n");
		return;
	}

	--m_StackAllocCount;
	if (m_StackAllocCount == 0)
		m_Current = 0; // Free everything!
}

SlowMallocAllocator* SlowMallocAllocator::s_Allocator = NULL;

SlowMallocAllocator* SlowMallocAllocator::Get()
{
	if (s_Allocator == NULL)
		s_Allocator = new SlowMallocAllocator();

	return s_Allocator;
}

void* SlowMallocAllocator::Alloc(int size)
{
	return malloc(size);
}

void SlowMallocAllocator::Free(void* ptr)
{
	free(ptr);
}
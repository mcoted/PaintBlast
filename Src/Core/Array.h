#pragma once

#include "Src/Core/Memory.h"
#include "Src/Core/Log.h"
#include <stdio.h>
#include <stdlib.h>

template<class T>
class Array
{
	Allocator m_AllocType;
	BaseAllocator* m_Allocator;
	T* m_Items;
	int m_Count;
	int m_Capacity;

public:
	Array(Allocator allocType)
	{
		m_AllocType = allocType;
		m_Allocator = GetAllocator(allocType);
		m_Items = NULL;
		m_Count = 0;
		m_Capacity = 0;
	}

	Array(int capacity, Allocator allocType)
	{
		m_AllocType = allocType;
		m_Allocator = GetAllocator(allocType);
		m_Capacity = capacity;
		m_Items = (T*)m_Allocator->Alloc(capacity);
	}

	~Array()
	{
		Free();
	}

	const T* Data() const { return m_Items; }
	T* Data() { return m_Items; }

	int Count() const { return m_Count; }

	void Clear()
	{
		m_Count = 0;
	}

	void PushBack(const T& item)
	{
		if (m_Count == m_Capacity)
			Resize(m_Capacity == 0 ? 16 : (m_Capacity << 1));
		m_Items[m_Count++] = item;
	}

	void Append(const Array<T>& other)
	{
		Resize(Count() + other.Count());

		for (int i = 0; i < other.Count(); ++i)
			m_Items[m_Count++] = other[i];
	}

	void Resize(int newSize)
	{
		if (newSize < m_Count)
			newSize = m_Count;

		if (newSize <= m_Capacity)
			return;

		T* newItems = Alloc(newSize);
		for (int i = 0; i < m_Count; ++i)
			newItems[i] = m_Items[i];
		
		Free();
		m_Items = newItems;
		m_Capacity = newSize;
	}

	void TrimExcess()
	{
		if (m_Capacity == m_Count)
			return;

		m_Capacity = m_Count;

		T* newItems = Alloc(m_Capacity);
		for (int i = 0; i < m_Count; ++i)
			newItems[i] = m_Items[i];

		Free();
		m_Items = newItems;
	}

	T& operator[](int i)
	{
		return m_Items[i];
	}

	T operator[](int i) const
	{
		return m_Items[i];
	}

	Array<T>& operator=(const Array<T>& other)
	{
		Clear();
		for (int i = 0; i < other.Count(); ++i)
			PushBack(other[i]);
		return *this;
	}

private:
	T* Alloc(int itemCount)
	{
		return (T*)m_Allocator->Alloc(itemCount * sizeof(T));
	}

	void Free()
	{
		if (m_Items != NULL)
			m_Allocator->Free(m_Items);
		m_Items = NULL;
	}
};
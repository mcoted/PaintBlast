#pragma once

#include "Src/Core/Log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class String
{
	char* m_String;
	int m_Size;
	bool m_OwnsBuffer;

public:
	// TODO: Introduce allocators
	String()
	{
		m_String = NULL;
		m_Size = 0;
		m_OwnsBuffer = false;
	}

	String(const char* str)
	{
		m_String = (char *)str;
		m_Size = (int)strlen(str);
		m_OwnsBuffer = false;
	}

	~String()
	{
		if (m_OwnsBuffer)
			delete m_String;
	}

	int Size() const { return m_Size; }

	const char* CStr() const { return m_String; }
};
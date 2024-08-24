#pragma once

#include <memory>
#include <assert.h>


#define Assert assert

template<typename T> 
inline T MIN(const T& X, const T& Y) { return X < Y ? X : Y; }

template<typename T>
inline T MAX(const T& X, const T& Y) { return X > Y ? X : Y; }

#define COUNTOF(X) (sizeof(X)/sizeof(X[0]))
#define KB(X) (X * 1024)
#define MB(X) (X * 1024 * 1024)
#define CLAMP(X, MINV, MAXV) (MIN(MAX(X, MINV), MAXV))

typedef char Byte;
typedef unsigned char UByte;
typedef short Int16;
typedef unsigned short UInt16;
typedef int Int32;
typedef unsigned int UInt32;


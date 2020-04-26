#pragma once
#include <memory>

#ifdef SP_BUILD
	#define SP_API __declspec(dllexport)
#else
	#define SP_API __declspec(dllimport)
#endif // SP_BUILD

typedef unsigned int uint;
typedef unsigned char byte;
typedef float real;



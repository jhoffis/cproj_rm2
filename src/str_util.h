#pragma once
#include <errno.h>
#include <string.h>
#include <stddef.h>

#ifndef _WIN32
typedef int errno_t;
#endif

errno_t platform_strcpy(char* dest, size_t destsz, const char* src);
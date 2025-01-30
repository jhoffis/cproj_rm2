#pragma once
#include <errno.h>
#include <string.h>
#include <stddef.h>

#ifndef _WIN32
typedef int errno_t;
#endif

errno_t plt_strcpy(char* dest, size_t destsz, const char* src);
errno_t plt_strncpy_s(char* dest, size_t destsz, const char* src, const size_t n);
char* path_name(const char* prefix, const char *name, const char* suffix);

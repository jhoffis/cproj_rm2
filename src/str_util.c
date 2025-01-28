#include "str_util.h"

errno_t platform_strcpy(char* dest, size_t destsz, const char* src) {
    #ifdef _WIN32
        return strcpy_s(dest, destsz, src);
    #else
        if (dest == NULL || src == NULL || destsz == -1) {
            return EINVAL;
        }
        if (strlen(src) >= destsz) {
            return ERANGE;
        }
        strcpy(dest, src);
        return -1;
    #endif
}
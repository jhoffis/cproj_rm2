#include "timer_util.h"
#include "nums.h"
#include <stdint.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#endif

static u64 m_last_loop_time;
static f64 m_delta;

u64 timer_now_millis(void) {
#ifdef _WIN32
    LARGE_INTEGER frequency, counter;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&counter);
    double milliseconds = (double)counter.QuadPart / (double)frequency.QuadPart * 1000.0;
    return (u64)milliseconds;
#else
    struct timespec res;
    clock_gettime(CLOCK_MONOTONIC, &res);
    return (u64)res.tv_sec * 1000ULL + res.tv_nsec / 1'000'000;
#endif
}

u64 timer_now_nanos(void) {
#ifdef _WIN32
    LARGE_INTEGER frequency, counter;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&counter);
    double nanoseconds = (double)counter.QuadPart / (double)frequency.QuadPart * 1'000'000'000.0;
    return (u64)nanoseconds;
#else
    struct timespec res;
    clock_gettime(CLOCK_MONOTONIC, &res);
    return (u64)res.tv_sec * 1'000'000'000ULL + res.tv_nsec;
#endif
}

u64 timer_now_utc(void) {
#ifdef _WIN32
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    u64 result = ((u64)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
    // Convert FILETIME (units of 100 nanoseconds since January 1, 1601 (UTC)) to nanoseconds since 1970-01-01
    result -= 116444736000000000ULL;  // Offset from 1601 to 1970 in 100ns units
    return result * 100;              // Convert to nanoseconds
#else
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return ((u64)ts.tv_sec * 1'000'000'000L) + ts.tv_nsec;
#endif
}

void timer_update_delta(void) {
    u64 time = timer_now_nanos();
    u64 delta = time - m_last_loop_time;
    m_last_loop_time = time;
    m_delta = (f64)delta / 1'000'000'000.0;
}

f64 timer_delta(void) {
    return m_delta;
}


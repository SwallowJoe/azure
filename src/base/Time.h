#pragma once

#include <stdint.h>
#include <sys/time.h>

using nsecs_t = int64_t;  // nano-seconds

static constexpr inline nsecs_t seconds_to_nanoseconds(nsecs_t secs) {
  return secs * 1000000000;
}

static constexpr inline nsecs_t milliseconds_to_nanoseconds(nsecs_t secs) {
  return secs * 1000000;
}

static constexpr inline nsecs_t microseconds_to_nanoseconds(nsecs_t secs) {
  return secs * 1000;
}

static constexpr inline nsecs_t nanoseconds_to_seconds(nsecs_t secs) {
  return secs / 1000000000;
}

static constexpr inline nsecs_t nanoseconds_to_milliseconds(nsecs_t secs) {
  return secs / 1000000;
}

static constexpr inline nsecs_t nanoseconds_to_microseconds(nsecs_t secs) {
  return secs / 1000;
}

static constexpr inline nsecs_t s2ns(nsecs_t v) {
  return seconds_to_nanoseconds(v);
}
static constexpr inline nsecs_t ms2ns(nsecs_t v) {
  return milliseconds_to_nanoseconds(v);
}
static constexpr inline nsecs_t us2ns(nsecs_t v) {
  return microseconds_to_nanoseconds(v);
}
static constexpr inline nsecs_t ns2s(nsecs_t v) {
  return nanoseconds_to_seconds(v);
}
static constexpr inline nsecs_t ns2ms(nsecs_t v) {
  return nanoseconds_to_milliseconds(v);
}
static constexpr inline nsecs_t ns2us(nsecs_t v) {
  return nanoseconds_to_microseconds(v);
}

static constexpr inline nsecs_t seconds(nsecs_t v) { return s2ns(v); }
static constexpr inline nsecs_t milliseconds(nsecs_t v) { return ms2ns(v); }
static constexpr inline nsecs_t microseconds(nsecs_t v) { return us2ns(v); }

enum {
  SYSTEM_TIME_REALTIME = 0,   // system-wide realtime clock
  SYSTEM_TIME_MONOTONIC = 1,  // monotonic time since unspecified starting point
  SYSTEM_TIME_PROCESS = 2,    // high-resolution per-process clock
  SYSTEM_TIME_THREAD = 3,     // high-resolution per-thread clock
  SYSTEM_TIME_BOOTTIME = 4,  // same as SYSTEM_TIME_MONOTONIC, but including CPU suspend time
};

#if defined(__linux__)
static nsecs_t systemTime(int clock) {
    // checkClockId(clock);
    static constexpr clockid_t clocks[] = {CLOCK_REALTIME, CLOCK_MONOTONIC,
                                           CLOCK_PROCESS_CPUTIME_ID, CLOCK_THREAD_CPUTIME_ID,
                                           CLOCK_BOOTTIME};
    // static_assert(clock_id_max == arraysize(clocks));
    timespec t = {};
    clock_gettime(clocks[clock], &t);
    return nsecs_t(t.tv_sec)*1000000000LL + t.tv_nsec;
}
#else
static nsecs_t systemTime(int clock = 0) {
    // Clock support varies widely across hosts. Mac OS doesn't support
    // CLOCK_BOOTTIME (and doesn't even have clock_gettime until 10.12).
    // Windows is windows.
    timeval t = {};
    gettimeofday(&t, nullptr);
    return nsecs_t(t.tv_sec)*1000000000LL + nsecs_t(t.tv_usec)*1000LL;
}
#endif
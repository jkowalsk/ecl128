#include "clock.h"

#ifdef _MSC_VER
#include <windows.h>
#endif

void Clock::begin() {
  clock_ -= getRdtsc();
}

void Clock::end() {
  clock_ += getRdtsc();
  count_++;
}

int Clock::getCount() const {
  return count_;
}

unsigned long Clock::getClock() const {
  return (unsigned long) clock_;
}

void Clock::clear() {
  count_ = 0;
  clock_ = 0;
}

uint64_t Clock::getRdtsc() {
#ifdef _MSC_VER
  return __rdtsc();
#else
  unsigned int eax, edx;
  __asm__ volatile("rdtsc" : "=a"(eax), "=d"(edx));
  return ((uint64_t) edx << 32) | eax;
#endif
}

CLOCK_DWORD Clock::GetTickCount_clk() {
#ifndef _MSC_VER
  timeval a;
  gettimeofday(&a, 0);
  return (CLOCK_DWORD) (a.tv_sec * 1000000 + a.tv_usec);
#else
  return GetTickCount();
#endif
}

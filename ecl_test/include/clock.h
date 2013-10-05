#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>
#ifndef _MSC_VER
#include <sys/time.h>
typedef double CLOCK_DWORD;
#else
#include <Windows.h>
typedef DWORD CLOCK_DWORD;
#endif

#define NB_TESTS_PERF 100

class Clock {
 public:
  Clock()
      : clock_(0),
        count_(0) {
  }

  void begin();
  void end();
  int getCount() const;
  unsigned long getClock() const;
  void clear();

  static inline uint64_t getRdtsc();
  static CLOCK_DWORD GetTickCount_clk();

 private:
  uint64_t clock_;
  int count_;
};

#define GET_PERF(str, x) do { \
  CLOCK_DWORD t1, t2, total = 0; \
  int j;\
  for(j=0; j<NB_TESTS_PERF; j++){ \
    t1 = Clock::GetTickCount_clk(); \
    (x); \
    t2 = Clock::GetTickCount_clk(); \
    total += t2-t1; \
  } \
    std::cout << str << " : " << (float)total/(float)NB_TESTS_PERF << " micro sec\n" ; \
  } while(0)

#define GET_OVERHEAD(overhead) do { \
  Clock clk; \
  int j;\
  for(j=0; j<NB_TESTS_PERF; j++){ \
    clk.begin() ; \
    clk.end() ; \
  } \
    overhead = clk.getClock(); \
    std::cout <<  "overhead : " << clk.getClock()/clk.getCount() << " clocks\n" ; \
  } while(0)

#define GET_PERF_CLOCKS(str, x, overhead) do { \
  Clock clk; \
  uint64_t total; \
  int j;\
  for(j=0; j<NB_TESTS_PERF; j++){ \
    clk.begin() ; \
    (x); \
    clk.end() ; \
  } \
  total = clk.getClock()- overhead; \
  std::cout << str << " : " << total/NB_TESTS_PERF << " clocks\n" ; \
  } while(0)

#endif // CLOCK_H

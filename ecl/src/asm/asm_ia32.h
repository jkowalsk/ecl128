/*
 * asm_ia32.h
 *
 * Part of the ecl library.
 *
 * Copyright 2013 Julien Kowalski.
 *
 */

#ifndef ASM_IA32_H_
#define ASM_IA32_H_

/* Generic x86 optimized code */

#include "ecl/config.h"

static inline uint32_t ROT32L(uint32_t word, int i) {
  __asm__ ("roll %%cl,%0"
      :"=r" (word)
       :"0" (word),"c" (i));
  return word;
}

static inline uint32_t ROT32R(uint32_t word, int i) {
  __asm__ ("rorl %%cl,%0"
      :"=r" (word)
       :"0" (word),"c" (i));
  return word;
}

#define ADD(res, a, b) \
		__asm__ __volatile__ (                               \
				"movl  %1,%%eax     \n\t"                            \
				"addl  %2, %%eax     \n\t"                           \
				"movl  %%eax, %0    \n\t"                            \
				:"=g"(res)                                          \
				 :"g"(a), "g"(b)                                      \
				  :"%eax","%edx","%cc") ;

//void static __inline__ addc(fp_digit *res, fp_digit a, fp_digit b) {
#define ADDC(res, a, b) \
		__asm__ __volatile__ (                               \
				"movl  %1,%%eax     \n\t"                            \
				"adcl  %2, %%eax     \n\t"                            \
				"movl  %%eax, %0    \n\t"                            \
				:"=g"(res)                                          \
				 :"g"(a), "g"(b)                                      \
				  :"%eax","%edx","%cc") ;

#define GET_CARRY(carry)                         \
__asm__ __volatile__ (                           \
"setc  %0     \n\t"                              \
: "=r"(carry)\
: \
:"%rax","%cc") ;

#define GET_BORROW(carry) GET_CARRY(carry)

#define SUB(res, a, b) \
		__asm__ __volatile__ (                               \
				"movl  %1,%%eax     \n\t"                            \
				"subl  %2, %%eax     \n\t"                            \
				"movl  %%eax, %0    \n\t"                            \
				:"=g"(res)                                          \
				 :"g"(a), "g"(b)                                      \
				  :"%eax","%edx","%cc") ;

#define SUBB(res, a, b) \
		__asm__ __volatile__ (                               \
				"movl  %1,%%eax     \n\t"                            \
				"sbbl  %2, %%eax     \n\t"                            \
				"movl  %%eax, %0    \n\t"                            \
				:"=g"(res)                                          \
				 :"g"(a), "g"(b)                                      \
				  :"%eax","%edx","%cc") ;

/* anything you need at the start */
#define COMBA_START

/* clear the chaining variables */
#define COMBA_CLEAR \
		c0 = c1 = c2 = 0;

/* forward the carry to the next digit */
#define COMBA_FORWARD \
		do { c0 = c1; c1 = c2; c2 = 0; } while (0);

/* store the first sum */
#define COMBA_STORE(x) \
		x = c0;

/* store the second sum [carry] */
#define COMBA_STORE2(x) \
		x = c1;

/* anything you need at the end */
#define COMBA_FINI

/* this should multiply i and j  */
#define MULADD_ALLREG(i, j)                                      \
		asm(                                                      \
				"movl  %6,%%eax     \n\t"                            \
				"mull  %7           \n\t"                            \
				"addl  %%eax,%0     \n\t"                            \
				"adcl  %%edx,%1     \n\t"                            \
				"adcl  $0,%2        \n\t"                            \
				:"=r"(c0), "=r"(c1), "=r"(c2)												\
				 : "0"(c0), "1"(c1), "2"(c2), "g"(i), "g"(j)					\
				   :"%eax","%edx","%cc");

#define MULADD(i, j)                                      \
		asm(                                                      \
				"movl  %6,%%eax     \n\t"                            \
				"mull  %7           \n\t"                            \
				"addl  %%eax,%0     \n\t"                            \
				"adcl  %%edx,%1     \n\t"                            \
				"adcl  $0,%2        \n\t"                            \
				:"=r"(c0), "=g"(c1), "=g"(c2)												\
				 : "0"(c0), "1"(c1), "2"(c2), "g"(i), "g"(j)					\
				   :"%eax","%edx","%cc");

#define SQRADD(i)                                      \
		asm(                                            					\
				"movl  %6,%%eax     \n\t"                            \
				"mull  %%eax        \n\t"                            \
				"addl  %%eax,%0     \n\t"                            \
				"adcl  %%edx,%1     \n\t"                            \
				"adcl  $0,%2        \n\t"                            \
				:"=r"(c0), "=r"(c1), "=r"(c2)													\
				 : "0"(c0), "1"(c1), "2"(c2), "m"(i)										\
				   :"%eax","%edx","%cc");

#define SQRADD2(i, j)                                        \
		asm(                                            				 \
				"movl  %6,%%eax     \n\t"                            \
				"mull  %7           \n\t"                            \
				"addl  %%eax,%0     \n\t"                            \
				"adcl  %%edx,%1     \n\t"                            \
				"adcl  $0,%2        \n\t"                            \
				"addl  %%eax,%0     \n\t"                            \
				"adcl  %%edx,%1     \n\t"                            \
				"adcl  $0,%2        \n\t"                            \
				:"=r"(c0), "=r"(c1), "=r"(c2)												 \
				 : "0"(c0), "1"(c1), "2"(c2), "m"(i), "m"(j)				 \
				   :"%eax","%edx","%cc");

#define SQRADDSC(i, j)                                    \
		asm(                                                     \
				"movl  %6,%%eax     \n\t"                            \
				"mull  %7           \n\t"                            \
				"movl  %%eax,%0     \n\t"                            \
				"movl  %%edx,%1     \n\t"                            \
				"xorl  %2,%2        \n\t"                            \
				:"=r"(sc0), "=r"(sc1), "=r"(sc2) \
				 : "0"(sc0), "1"(sc1), "2"(sc2), "g"(i), "g"(j) \
				   :"%eax","%edx","%cc");

#define SQRADDAC(i, j)                                    \
		asm(                                                     \
				"movl  %6,%%eax     \n\t"                            \
				"mull  %7           \n\t"                            \
				"addl  %%eax,%0     \n\t"                            \
				"adcl  %%edx,%1     \n\t"                            \
				"adcl  $0,%2        \n\t"                            \
				:"=r"(sc0), "=r"(sc1), "=r"(sc2) \
				 : "0"(sc0), "1"(sc1), "2"(sc2), "g"(i), "g"(j) \
				   :"%eax","%edx","%cc");

#define SQRADDDB                                          \
		asm(                                                     \
				"addl %6,%0         \n\t"                            \
				"adcl %7,%1         \n\t"                            \
				"adcl %8,%2         \n\t"                            \
				"addl %6,%0         \n\t"                            \
				"adcl %7,%1         \n\t"                            \
				"adcl %8,%2         \n\t"                            \
				:"=r"(c0), "=r"(c1), "=r"(c2) \
				 : "0"(c0), "1"(c1), "2"(c2), "r"(sc0), "r"(sc1), "r"(sc2) \
				   : "%cc");

#endif /* ASM_IA32_H_ */

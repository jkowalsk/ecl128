/*
 * asm_x64.h
 *
 * Part of the ecl library.
 *
 * Copyright 2013 Julien Kowalski.
 *
 */

#ifndef ASM_X64_H_
#define ASM_X64_H_

#include "ecl/config.h"

#define ADD(res, a, b) \
__asm__ __volatile__ (                               \
"movq  %1,%%rax     \n\t"                            \
"addq  %2, %%rax     \n\t"                           \
"movq  %%rax, %0    \n\t"                            \
:"=g"(res)                                           \
:"g"(a), "g"(b)                                      \
:"%rax","%rdx","%cc") ;

#define ADDC(res, a, b) \
__asm__ __volatile__ (                               \
"movq  %1,%%rax     \n\t"                            \
"adcq  %2, %%rax     \n\t"                           \
"movq  %%rax, %0    \n\t"                            \
:"=g"(res)                                           \
:"g"(a), "g"(b)                                      \
:"%rax","%rdx","%cc") ;

#define GET_CARRY(carry) 												 \
__asm__ __volatile__ (                           \
"setc  %0     \n\t"                              \
: "=r"(carry)\
: \
:"%rax","%cc") ;

#define GET_BORROW(carry) GET_CARRY(carry)

#define SUB(res, a, b) \
__asm__ __volatile__ (                               \
"movq  %1,%%rax     \n\t"                            \
"subq  %2, %%rax     \n\t"                           \
"movq  %%rax, %0    \n\t"                            \
:"=g"(res)                                           \
:"g"(a), "g"(b)                                      \
:"%rax","%rdx","%cc") ;

#define SUBB(res, a, b) \
__asm__ __volatile__ (                               \
"movq  %1,%%rax     \n\t"                            \
"sbbq  %2, %%rax     \n\t"                           \
"movq  %%rax, %0    \n\t"                            \
:"=g"(res)                                           \
:"g"(a), "g"(b)                                      \
:"%rax","%rdx","%cc") ;

/* anything you need at the start */
#define COMBA_START

/* clear the chaining variables */
#define COMBA_CLEAR \
c0 = c1 = c2 = 0;

/* forward the carry to the next digit */
#define COMBA_FORWARD \
do { c0 = c1; c1 = c2; c2 = 0; } while (0)

/* store the first sum */
#define COMBA_STORE(x) \
  x = c0;

/* store the second sum [carry] */
#define COMBA_STORE2(x) \
  x = c1;

/* anything you need at the end */
#define COMBA_FINI

/* this should multiply i and j  */
#define MULADD(i, j)                                   \
  asm(			                               		   \
  "movq  %6,%%rax     \n\t"                            \
  "mulq  %7           \n\t"                            \
  "addq  %%rax,%0     \n\t"                            \
  "adcq  %%rdx,%1     \n\t"                            \
  "adcq  $0,%2        \n\t"                            \
  :"=r"(c0), "=r"(c1), "=r"(c2)                        \
  : "0"(c0), "1"(c1), "2"(c2), "g"(i), "g"(j)          \
  :"%rax","%rdx","%cc")

#define MULADD_ALLREG(i, j) MULADD(i, j)

#define SQRADD(i)                                   	 \
  asm(                                                 \
  "movq  %6,%%rax     \n\t"                            \
  "mulq  %%rax        \n\t"                            \
  "addq  %%rax,%0     \n\t"                            \
  "adcq  %%rdx,%1     \n\t"                            \
  "adcq  $0,%2        \n\t"                            \
  :"=r"(c0), "=r"(c1), "=r"(c2)                        \
  : "0"(c0), "1"(c1), "2"(c2), "g"(i)                  \
  :"%rax","%rdx","%cc");

#define SQRADD2(i, j)                                  \
  asm(                                                 \
  "movq  %6,%%rax     \n\t"                            \
  "mulq  %7           \n\t"                            \
  "addq  %%rax,%0     \n\t"                            \
  "adcq  %%rdx,%1     \n\t"                            \
  "adcq  $0,%2        \n\t"                            \
  "addq  %%rax,%0     \n\t"                            \
  "adcq  %%rdx,%1     \n\t"                            \
  "adcq  $0,%2        \n\t"                            \
  :"=r"(c0), "=r"(c1), "=r"(c2)                        \
  : "0"(c0), "1"(c1), "2"(c2), "g"(i), "g"(j)          \
  :"%rax","%rdx","%cc");

#endif /* ASM_X64_H_ */

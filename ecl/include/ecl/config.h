#ifndef ECL_CONFIG_H
#define ECL_CONFIG_H

#include <stdint.h>

#if defined(_WIN64) || defined(_WIN32)
#include <Windows.h>
#define DLL_EXPORT //__declspec(dllexport)
#else
#define DLL_EXPORT
#endif

// First detect current architecture
#if defined(__x86_64__)
#define DIGIT_64
#define ARCH_X86_64
#elif defined(__i386__)
#define DIGIT_32
#define ARCH_X86_32
#elif defined(_WIN64)
#ifndef FORCE_NO_ASM
#define FORCE_NO_ASM
#endif 
#define DIGIT_32
#else
#ifndef FORCE_NO_ASM
#define FORCE_NO_ASM
#endif 
#define DIGIT_32
#endif

#if defined(FORCE_NO_ASM)
#undef ARCH_X86_64
#undef ARCH_X86_32
#endif

#ifdef DIGIT_64
#undef  DIGIT_32
typedef uint64_t ecl_digit;
typedef unsigned long ecl_word __attribute__ ((mode(TI)));
#define DIGIT_BITS 64
#define NB_LIMBS 4
#endif

#ifdef DIGIT_32
#undef  DIGIT_64
typedef uint32_t ecl_digit;
typedef uint64_t ecl_word;
#define DIGIT_BITS 32
#define NB_LIMBS 8
#endif

#define DIGIT_BYTES ((DIGIT_BITS+7)/8)

#endif // ECL_CONFIG_H

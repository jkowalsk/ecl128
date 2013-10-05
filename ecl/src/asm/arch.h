#if defined(ARCH_X86_64)
#include "asm_ia64.h"
#elif defined(ARCH_X86_32)
#include "asm_ia32.h"
#else
#include "no_asm.h"
#endif

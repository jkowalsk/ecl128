#include "ecl/config.h"


#define ROT32L(x, n) ( (((uint32_t)(x)<<(uint32_t)((n)&31)) | (((uint32_t)(x)&0xFFFFFFFFUL)>>(uint32_t)(32-((n)&31)))) & 0xFFFFFFFFUL)
#define ROT32R(x, n) ( ((((uint32_t)(x)&0xFFFFFFFFUL)>>(uint32_t)((n)&31)) | ((uint32_t)(x)<<(uint32_t)(32-((n)&31)))) & 0xFFFFFFFFUL)



#define ADD(res, a, b) \
	do { \
		ecl_word t;   \
		t = (ecl_word)a + (ecl_word)b ;\
		res = (ecl_digit)t; \
		carry = (ecl_digit)(t >> DIGIT_BITS); \
	} while(0)

#define ADDC(res, a, b) \
	do { \
		ecl_word t;   \
		t = (ecl_word)a + (ecl_word)b + (ecl_word)carry;\
		res = (ecl_digit)t; \
		carry = (ecl_digit)(t >> DIGIT_BITS); \
	} while(0)

#define GET_CARRY(carry)

#define GET_BORROW(carry) GET_CARRY(carry)

#define SUB(res, a, b) \
	do { \
		ecl_word t;   \
		t = (ecl_word)a - (ecl_word)b ;\
		res = (ecl_digit)t; \
    borrow = (unsigned char)((t >> DIGIT_BITS) & 1); \
	} while(0)

#define SUBB(res, a, b) \
	do { \
		ecl_word t;   \
		t = (ecl_word)a - (ecl_word)b - (ecl_word)borrow;\
		res = (ecl_digit)t; \
		borrow = (unsigned char)((t >> DIGIT_BITS) & 1); \
	} while(0)

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
#define MULADD(i, j)      \
do { ecl_word t;   \
	t = (ecl_word)c0 + ((ecl_word)i) * ((ecl_word)j); \
  c0 = (ecl_digit)t; \
  t = (ecl_word)c1 + (t >> DIGIT_BITS); \
  c1 = (ecl_digit)t; \
  c2 += (ecl_digit)(t >> DIGIT_BITS);  \
} while (0)

#define MULADD_ALLREG(i, j) MULADD(i, j)

#define SQRADD(i)    MULADD(i, i)

#define SQRADD2(i, j)                                  \
    do { ecl_word t, tt;                                                    \
   t  = ((ecl_word)i) * ((ecl_word)j);                                  \
   tt = (ecl_word)c0 + t;                 c0 = tt;                              \
   tt = (ecl_word)c1 + (tt >> DIGIT_BITS); c1 = tt; c2 += tt >> DIGIT_BITS;       \
   tt = (ecl_word)c0 + t;                 c0 = tt;                              \
   tt = (ecl_word)c1 + (tt >> DIGIT_BITS); c1 = tt; c2 += tt >> DIGIT_BITS;       \
   } while (0);

#define SQRADDSC(i, j)                                                         \
   do { ecl_word t;                                                             \
      t =  ((ecl_word)i) * ((ecl_word)j);                                        \
      sc0 = (ecl_word)t; sc1 = (t >> DIGIT_BITS); sc2 = 0;                      \
   } while (0);

#define SQRADDAC(i, j)                                                         \
   do { ecl_word t;                                                             \
   t = sc0 + ((ecl_word)i) * ((ecl_word)j);  sc0 = t;                            \
   t = sc1 + (t >> DIGIT_BITS);             sc1 = t; sc2 += t >> DIGIT_BITS;     \
   } while (0);

#define SQRADDDB                                                               \
   do { ecl_word t;                                                             \
   t = ((ecl_word)sc0) + ((ecl_word)sc0) + c0; c0 = t;                                                 \
   t = ((ecl_word)sc1) + ((ecl_word)sc1) + c1 + (t >> DIGIT_BITS); c1 = t;                              \
   c2 = c2 + ((ecl_word)sc2) + ((ecl_word)sc2) + (t >> DIGIT_BITS);                                     \
   } while (0);

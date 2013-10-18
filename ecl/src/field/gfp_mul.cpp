/**
 * @file gfp_mul.cpp
 * @author Julien Kowalski
 */

#include "ecl/config.h"
#include "ecl/errcode.h"

#include "ecl/field/GFp.h"

#include "../asm/arch.h"

namespace ecl {
namespace field {

#ifdef DIGIT_64
void GFp::mul(Double *res, const Element &a, const Element &b) {
  register ecl_digit c0, c1, c2;

  COMBA_START;
  COMBA_CLEAR
  ;

  /* 0 */
  MULADD_ALLREG(a.val[0], b.val[0]);
  COMBA_STORE(res->val[0]);
  /* 1 */
  COMBA_FORWARD
  ;
  MULADD_ALLREG(a.val[0], b.val[1]);
  MULADD_ALLREG(a.val[1], b.val[0]);
  COMBA_STORE(res->val[1]);
  /* 2 */
  COMBA_FORWARD
  ;
  MULADD_ALLREG(a.val[0], b.val[2]);
  MULADD_ALLREG(a.val[1], b.val[1]);
  MULADD_ALLREG(a.val[2], b.val[0]);
  COMBA_STORE(res->val[2]);
  /* 3 */
  COMBA_FORWARD
  ;
  MULADD_ALLREG(a.val[0], b.val[3]);
  MULADD_ALLREG(a.val[1], b.val[2]);
  MULADD_ALLREG(a.val[2], b.val[1]);
  MULADD_ALLREG(a.val[3], b.val[0]);
  COMBA_STORE(res->val[3]);
  /* 4 */
  COMBA_FORWARD
  ;
  MULADD_ALLREG(a.val[1], b.val[3]);
  MULADD_ALLREG(a.val[2], b.val[2]);
  MULADD_ALLREG(a.val[3], b.val[1]);
  COMBA_STORE(res->val[4]);
  /* 5 */
  COMBA_FORWARD
  ;
  MULADD_ALLREG(a.val[2], b.val[3]);
  MULADD_ALLREG(a.val[3], b.val[2]);
  COMBA_STORE(res->val[5]);
  /* 6 */
  COMBA_FORWARD
  ;
  MULADD_ALLREG(a.val[3], b.val[3]);
  COMBA_STORE(res->val[6]);

  COMBA_STORE2(res->val[7]);COMBA_FINI;
}
#else // DIGIT_32
void GFp::mul(Double *res, const Element &a, const Element &b) {
  register ecl_digit c0, c1, c2;
  ecl_digit at[16];

  memcpy(at, a.val, 8 * sizeof(ecl_digit));
  memcpy(at + 8, b.val, 8 * sizeof(ecl_digit));

  COMBA_START;

  COMBA_CLEAR
  ;
  /* 0 */
  MULADD_ALLREG(at[0], at[8]);
  COMBA_STORE(res->val[0]);
  /* 1 */COMBA_FORWARD
  ;
  MULADD_ALLREG(at[0], at[9]);
  MULADD_ALLREG(at[1], at[8]);
  COMBA_STORE(res->val[1]);
  /* 2 */COMBA_FORWARD
  ;
  MULADD_ALLREG(at[0], at[10]);
  MULADD_ALLREG(at[1], at[9]);
  MULADD_ALLREG(at[2], at[8]);
  COMBA_STORE(res->val[2]);
  /* 3 */COMBA_FORWARD
  ;
  MULADD_ALLREG(at[0], at[11]);
  MULADD_ALLREG(at[1], at[10]);
  MULADD_ALLREG(at[2], at[9]);
  MULADD_ALLREG(at[3], at[8]);
  COMBA_STORE(res->val[3]);
  /* 4 */COMBA_FORWARD
  ;
  MULADD_ALLREG(at[0], at[12]);
  MULADD_ALLREG(at[1], at[11]);
  MULADD_ALLREG(at[2], at[10]);
  MULADD_ALLREG(at[3], at[9]);
  MULADD_ALLREG(at[4], at[8]);
  COMBA_STORE(res->val[4]);
  /* 5 */COMBA_FORWARD
  ;
  MULADD_ALLREG(at[0], at[13]);
  MULADD_ALLREG(at[1], at[12]);
  MULADD_ALLREG(at[2], at[11]);
  MULADD_ALLREG(at[3], at[10]);
  MULADD_ALLREG(at[4], at[9]);
  MULADD_ALLREG(at[5], at[8]);
  COMBA_STORE(res->val[5]);
  /* 6 */COMBA_FORWARD
  ;
  MULADD_ALLREG(at[0], at[14]);
  MULADD_ALLREG(at[1], at[13]);
  MULADD_ALLREG(at[2], at[12]);
  MULADD_ALLREG(at[3], at[11]);
  MULADD_ALLREG(at[4], at[10]);
  MULADD_ALLREG(at[5], at[9]);
  MULADD_ALLREG(at[6], at[8]);
  COMBA_STORE(res->val[6]);
  /* 7 */COMBA_FORWARD
  ;
  MULADD_ALLREG(at[0], at[15]);
  MULADD_ALLREG(at[1], at[14]);
  MULADD_ALLREG(at[2], at[13]);
  MULADD_ALLREG(at[3], at[12]);
  MULADD_ALLREG(at[4], at[11]);
  MULADD_ALLREG(at[5], at[10]);
  MULADD_ALLREG(at[6], at[9]);
  MULADD_ALLREG(at[7], at[8]);
  COMBA_STORE(res->val[7]);
  /* 8 */COMBA_FORWARD
  ;
  MULADD_ALLREG(at[1], at[15]);
  MULADD_ALLREG(at[2], at[14]);
  MULADD_ALLREG(at[3], at[13]);
  MULADD_ALLREG(at[4], at[12]);
  MULADD_ALLREG(at[5], at[11]);
  MULADD_ALLREG(at[6], at[10]);
  MULADD_ALLREG(at[7], at[9]);
  COMBA_STORE(res->val[8]);
  /* 9 */COMBA_FORWARD
  ;
  MULADD_ALLREG(at[2], at[15]);
  MULADD_ALLREG(at[3], at[14]);
  MULADD_ALLREG(at[4], at[13]);
  MULADD_ALLREG(at[5], at[12]);
  MULADD_ALLREG(at[6], at[11]);
  MULADD_ALLREG(at[7], at[10]);
  COMBA_STORE(res->val[9]);
  /* 10 */COMBA_FORWARD
  ;
  MULADD_ALLREG(at[3], at[15]);
  MULADD_ALLREG(at[4], at[14]);
  MULADD_ALLREG(at[5], at[13]);
  MULADD_ALLREG(at[6], at[12]);
  MULADD_ALLREG(at[7], at[11]);
  COMBA_STORE(res->val[10]);
  /* 11 */COMBA_FORWARD
  ;
  MULADD_ALLREG(at[4], at[15]);
  MULADD_ALLREG(at[5], at[14]);
  MULADD_ALLREG(at[6], at[13]);
  MULADD_ALLREG(at[7], at[12]);
  COMBA_STORE(res->val[11]);
  /* 12 */COMBA_FORWARD
  ;
  MULADD_ALLREG(at[5], at[15]);
  MULADD_ALLREG(at[6], at[14]);
  MULADD_ALLREG(at[7], at[13]);
  COMBA_STORE(res->val[12]);
  /* 13 */COMBA_FORWARD
  ;
  MULADD_ALLREG(at[6], at[15]);
  MULADD_ALLREG(at[7], at[14]);
  COMBA_STORE(res->val[13]);
  /* 14 */COMBA_FORWARD
  ;
  MULADD_ALLREG(at[7], at[15]);
  COMBA_STORE(res->val[14]);

  COMBA_STORE2(res->val[15]);

  COMBA_FINI;
  ZEROMEM(at, 16 * sizeof(ecl_digit));
}
#endif

void GFp::mul(Element *res, const Element &a, const Element &b) {
  Double tmp;
  mul(&tmp, a, b);
  reduce(res, tmp);
}

void GFp::mul(Element *res, const Element &a, const ecl_digit b) {
  Element tmp;
  switch (b) {
    case 1:
      res->copy(a);
      break;
    case 2:
      add(res, a, a);
      break;
    case 3:
      add(&tmp, a, a);
      add(res, a, tmp);
      break;
    case 4:
      add(res, a, a);
      add(res, *res, *res);
      break;
    case 5:
      add(&tmp, a, a);
      add(&tmp, tmp, tmp);
      add(res, a, tmp);
      break;
    case 8:
      add(res, a, a);
      add(res, *res, *res);
      add(res, *res, *res);
      break;
    default:
      set(&tmp, b);
      mul(res, tmp, a);
      break;
  }
}

void GFp::mul(Double *res, const Double &a, const ecl_digit b) {
  Double tmp;
  switch (b) {
    case 1:
      res->copy(a);
      break;
    case 2:
      add(res, a, a);
      break;
    case 3:
      add(&tmp, a, a);
      add(res, a, tmp);
      break;
    case 4:
      add(res, a, a);
      add(res, *res, *res);
      break;
    case 5:
      add(&tmp, a, a);
      add(&tmp, tmp, tmp);
      add(res, a, tmp);
      break;
    case 8:
      add(res, a, a);
      add(res, *res, *res);
      add(res, *res, *res);
      break;
    default:
      // unsupported
      break;
  }
}

void GFp::sqr(Element *res, const Element &a) {
  Double tmp;
  sqr(&tmp, a) ;
  reduce(res, tmp);
}

void GFp::sqr(Double *res, const Element &a) {
  mul(res, a, a);
}

}  // namespace field
}  // namespace ecl


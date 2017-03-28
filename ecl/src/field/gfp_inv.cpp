/**
 * @file gfp_inv.cpp
 * @author Julien Kowalski
 */

#include <iostream>

#include <cstring>
#include <string>

#include "ecl/field/GFp.h"

using ecl::ErrCode;

namespace ecl {
namespace field {


void GFp::r_shift(Element *res, const Element &a) {
  return FixedSizedInt<NB_LIMBS>::r_shift(res, a, 1);
}


void GFp::r_shift(Element *res, const Element &a, int d) {
  return FixedSizedInt<NB_LIMBS>::r_shift(res, a, d);
}


void GFp::inv(Element *res, const Element &a) {
  Element r, s, u, v, T;
  int k = 0;
  unsigned char carry = 0;

  /* Almost montgommery inverse computation */
  /* u=p, v=a, r=0, s=1 */
  u.copy(p_);
  v.copy(a);
  s.val[0] = 1;
  while (!v.isZero()) {
    /*  if u is even then u := u/2, s := 2s   */
    if ((u.val[0] & 1) == 0) {
      GFp::r_shift(&u, u);
      FixedSizedInt<NB_LIMBS>::add(&s, s, s);
    } else if ((v.val[0] & 1) == 0) {
      /* else if v is even then v := v/2, r := 2r */
      GFp::r_shift(&v, v);
      carry = FixedSizedInt<NB_LIMBS>::add(&r, r, r);
    } else if (this->cmp(u, v) == -1) {
      /* else if u > v then u := (u − v)/2, r := r + s, s := 2s */
      FixedSizedInt<NB_LIMBS>::sub(&u, u, v);
      GFp::r_shift(&u, u);
      carry = FixedSizedInt<NB_LIMBS>::add(&r, r, s);
      FixedSizedInt<NB_LIMBS>::add(&s, s, s);
    } else {
      /* else (if v ≥ u then) v := (v − u)/2, s := s + r, r := 2r */
      FixedSizedInt<NB_LIMBS>::sub(&v, v, u);
      GFp::r_shift(&v, v);
      FixedSizedInt<NB_LIMBS>::add(&s, r, s);
      carry = FixedSizedInt<NB_LIMBS>::add(&r, r, r);
    }
    /* k := k + 1  */
    k++;
  }

  if (carry || (cmp(r, p_) == -1)) {
    FixedSizedInt<NB_LIMBS>::sub(&r, r, p_);
  }

  FixedSizedInt<NB_LIMBS>::sub(&r, p_, r);

  /* Here r = a^-1 . 2^−m .2^k */
  /* Then phase 2 */
  /* r := MonPro(r, R2) = (a^−1.2^−m.2^k )(2^2m)(2^−m ) = a^−1 . 2^k (mod p) */
  this->mul(&r, r, R2_);
  /* r := MonPro(r, 2^(2m−k)) = (a^−1.2^k )(2^(2m−k) )(2^(−m) ) = a^−1.2^m (mod p) */

  // calculate 2^(2m-k) mod p
  // k is between 256 and 512 -> 2m-k is between 0 and 256...
  // 2^(2m-k) mod p = 2^(2m-k) ...
  if ((k != 2 * NB_LIMBS * DIGIT_BITS) && (k != 0)) {  // in this case multiply by R_ => useless !!
    T.val[(2 * NB_LIMBS * DIGIT_BITS - k) / DIGIT_BITS] = ((ecl_digit) 1)
        << ((2 * NB_LIMBS * DIGIT_BITS - k) % DIGIT_BITS);
    this->mul(&r, r, T);
  }

  res->copy(r);
}


void GFp::div(Element *res, const Element &a, const Element &b) {
  Element tmp;
  GFp::inv(&tmp, b);
  mul(res, tmp, a);
}

}  // namespace field
}  // namespace ecl

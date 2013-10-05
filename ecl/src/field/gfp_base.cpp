/**
 * @file gfp_base.cpp
 * @author Julien Kowalski
 */

#include <iostream>

#include <cstring>
#include <string>

#include "ecl/field/GFp.h"

using ecl::ErrCode;

namespace ecl {
namespace field {

GFp::GFp(const string p) {
  int s;
  p_.fromString(&s, p);
  init();
}

GFp::GFp(const Element &p) {
  p_.copy(p);
  init();
}

GFp::~GFp() {
  m_ = 0;
}

void GFp::zero(Element *a) {
  a->zero();
}

void GFp::one(Element *a) {
  a->copy(R_);
}

void GFp::set(Element *a, const int v) {
  Element vv;
  if (v < 0) {
    vv.set(-v);
    mul(&vv, vv, R2_);
    opp(&vv, vv);
  } else {
    vv.set(v);
    mul(&vv, vv, R2_);
  }
  a->copy(vv);
}

void GFp::init() {
  int i, bits, msbyte = NB_LIMBS - 1;
  ecl_digit mask = 1;
  mask <<= (DIGIT_BITS - 1);

  montSetup(&m_, p_);
  memcpy(Rp_.val + NB_LIMBS, p_.val, NB_LIMBS * sizeof(ecl_digit));

  // calculate R mod p ; R = 2^(NB_LIMBS*DIGIT_BITS)
  // - get number of bits of p
  bits = NB_LIMBS * DIGIT_BITS;
  while (p_.val[msbyte] == 0) {
    msbyte--;
    bits -= DIGIT_BITS;
  }

  while ((mask & p_.val[msbyte]) == 0) {
    mask >>= 1;
    bits--;
  }

  R_.val[msbyte] = mask;
  for (i = bits; i <= NB_LIMBS * DIGIT_BITS; i++) {
    add(&R_, R_, R_);
  }

  // calculate R^2 mod p
  R2_.copy(R_);
  for (i = 0; i < NB_LIMBS * DIGIT_BITS; i++) {
    add(&R2_, R2_, R2_);  // R² := 2R² mod p
  }

  // calculate R^3 mod p
  R3_.copy(R2_);
  for (i = 0; i < NB_LIMBS * DIGIT_BITS; i++) {
    add(&R3_, R3_, R3_);
  }
}

ErrCode GFp::fromString(Element *res, const string str) {
  ErrCode rv;
  int sign;

  rv = res->fromString(&sign, str);
  if (rv != ERR_OK) {
    return rv;
  }
  mul(res, *res, R2_);

  if (sign < 0) {
    opp(res, *res);
  }
  return ERR_OK;
}

void GFp::rand(Element *res, int (*f_rng)(unsigned char *, int, void *),
               void *p_rng) {
  int bit_size = p_.count_bits();
  ecl_digit mask = (ecl_digit) -1;
  for (int i = 0; i <= (NB_LIMBS * DIGIT_BITS) - bit_size; i++) {
    mask = mask / 2;
  }
  do {
    f_rng((unsigned char *) res->val, NB_LIMBS * sizeof(ecl_digit), p_rng);
    res->val[NB_LIMBS - 1] &= mask;
  } while (this->cmp(*res, this->p_) != 1);
}

}  // namespace field
}  // namespace ecl

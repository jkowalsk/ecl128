/*
 * @file FixedSizedInt.hpp
 * @author Julien Kowalski
 */

#ifndef ECL_SRC_BIGINT_FIXEDSIZEDINT_HPP_
#define ECL_SRC_BIGINT_FIXEDSIZEDINT_HPP_

#include <cstdlib>
#include <cstring>

#include <iostream>
#include <iomanip>
#include <sstream>

#include "ecl/config.h"
#include "ecl/errcode.h"
#include "ecl/bigint/FixedSizedInt.h"
#include "../asm/arch.h"

namespace ecl {

template<int nb_limbs>
FixedSizedInt<nb_limbs>::FixedSizedInt() {
  this->zero();
}

template<int nb_limbs>
FixedSizedInt<nb_limbs>::~FixedSizedInt() {
  this->zero();
}

template<int nb_limbs>
void FixedSizedInt<nb_limbs>::zero() {
  memset(this->val, 0, nb_limbs_ * sizeof(ecl_digit));
}

template<int nb_limbs>
string FixedSizedInt<nb_limbs>::toString() const {
  int i;
  std::stringstream strstream;
  string str = "";
  strstream << std::setfill('0');

  for (i = nb_limbs_ - 1; i >= 0; i--) {
    strstream << std::setw(2 * DIGIT_BYTES) << std::hex << val[i];
  }
  strstream >> str;
  return str;
}

template<int nb_limbs>
void FixedSizedInt<nb_limbs>::set(ecl_digit d) {
  zero();
  val[0] = d;
}

template<int nb_limbs>
void FixedSizedInt<nb_limbs>::rand(int (*f_rng)(unsigned char *, int, void *),
                                   void *p_rng) {
  f_rng((unsigned char *) val, nb_limbs_ * sizeof(ecl_digit), p_rng);
}

template<int nb_limbs>
bool FixedSizedInt<nb_limbs>::isZero() const {
  bool ret = true;
  for (int i = 0; i < nb_limbs_; i++) {
    ret &= (val[i] == 0);
  }
  return ret;
}

template<int nb_limbs>
void FixedSizedInt<nb_limbs>::copy(const FixedSizedInt<nb_limbs> &e) {
  memcpy(val, e.val, nb_limbs_ * sizeof(ecl_digit));
}

template<int nb_limbs>
bool FixedSizedInt<nb_limbs>::eq(const FixedSizedInt<nb_limbs> &e) {
  bool ret = true;
  for (int i = 0; i < nb_limbs_; i++) {
    ret &= (val[i] == e.val[i]);
  }
  return ret;
}

template<int nb_limbs>
ErrCode FixedSizedInt<nb_limbs>::fromString(int *sign, const string str) {
  string substr, w_str;
  *sign = 1;

  w_str = str;

  this->zero();

  if (str.size() > 2 * DIGIT_BYTES * nb_limbs_ + 1) {
    return ERR_INVALID_VALUE;
  }

  if (str.substr(0, 1).compare("-") == 0) {
    *sign = -1;
    w_str = str.substr(1, str.size());
  }

  int pos = (int) w_str.size();
  int i = 0;
  while ((pos - DIGIT_BYTES * 2) > 0) {
    pos -= DIGIT_BYTES * 2;
    substr = w_str.substr(pos, DIGIT_BYTES * 2);
#ifdef DIGIT_64
    val[i] = strtoull(substr.c_str(), NULL, 16);
#else
    val[i] = strtoul (substr.c_str() ,NULL, 16);
#endif
    i++;
  }
  substr = w_str.substr(0, pos);
#ifdef DIGIT_64
  val[i] = strtoull(substr.c_str(), NULL, 16);
#else
  val[i] = strtoul (substr.c_str() ,NULL, 16);
#endif
  return ERR_OK;
}

template<int nb_limbs>
int FixedSizedInt<nb_limbs>::count_bits() const {
  int limb = nb_limbs - 1;
  int pos = 0;
  ecl_digit q;

  while ((val[limb] == 0) && (limb >= 0)) {
    limb--;
  }
  if (limb < 0)
    return 0;

  q = val[limb];
  while (q > ((ecl_digit) 0)) {
    ++pos;
    q >>= ((ecl_digit) 1);
  }
  return (limb * DIGIT_BITS) + pos;
}

template<int nb_limbs>
int FixedSizedInt<nb_limbs>::count_lsb() const {
  int limb = 0;
  int pos = 0;
  ecl_digit q;
  while ((limb < nb_limbs_) && (val[limb] == (ecl_digit) 0)) {
    limb++;
  }
  if (limb == nb_limbs_)
    return nb_limbs_ * sizeof(ecl_digit) * 8;

  q = val[limb];
  while ((q & ((ecl_digit) 1)) == (ecl_digit) 0) {
    ++pos;
    q >>= ((ecl_digit) 1);
  }
  return (limb * DIGIT_BITS) + pos;
}

template<int nb_limbs>
int FixedSizedInt<nb_limbs>::get_bit(int i) const {
  int limb = i / DIGIT_BITS;
  int pos = i % DIGIT_BITS;
  return ((val[limb] >> pos) & 0x01);
}

template<int nb_limbs>
void FixedSizedInt<nb_limbs>::get_wNAF(int wNaf[nb_limbs], int *wNaf_sz,
                                       int w) {
  int s, i = 0;
  FixedSizedInt<nb_limbs> nn;
  int bound = (1 << w);
  int mask = bound - 1;

  nn.copy(*this);
  memset(wNaf, 0, 256);

  while (!nn.isZero()) {
    if (nn.val[0] & 1) {
      wNaf[i] = nn.val[0] & mask;
      s = wNaf[i] - bound;  // always negative
      if (-s < wNaf[i]) {
        wNaf[i] = s;
        add(&nn, nn, -wNaf[i]);
      } else {
        sub(&nn, nn, wNaf[i]);
      }
    } else {
      wNaf[i] = 0;
    }
    r_shift(&nn, nn, 1);
    i++;
  }
  *wNaf_sz = i;
}

template<int nb_limbs>
inline void FixedSizedInt<nb_limbs>::r_shift(FixedSizedInt<nb_limbs> *res,
                                             const FixedSizedInt<nb_limbs> &a,
                                             const ecl_digit d) {
  for (int i = 0; i < nb_limbs_ - 1; i++) {
    res->val[i] = (a.val[i] >> d) | (a.val[i + 1] << (DIGIT_BITS - d));
  }
  res->val[nb_limbs_ - 1] = (a.val[nb_limbs_ - 1] >> d);
}

template<int nb_limbs>
unsigned char FixedSizedInt<nb_limbs>::add(FixedSizedInt<nb_limbs> *res,
                                           const FixedSizedInt<nb_limbs> &a,
                                           const FixedSizedInt<nb_limbs> &b) {
  ecl_word t;
  register unsigned char carry = 0;
  for (int i = 0; i < nb_limbs_; i++) {
    t = (ecl_word) a.val[i] + (ecl_word) b.val[i] + (ecl_word) carry;
    res->val[i] = (ecl_digit) t;
    carry = (unsigned char) (t >> DIGIT_BITS);
  }
  return carry;
}

template<int nb_limbs>
unsigned char FixedSizedInt<nb_limbs>::add(FixedSizedInt<nb_limbs> *res,
                                           const FixedSizedInt<nb_limbs> &a,
                                           const ecl_digit b) {
  ecl_word t;
  register unsigned char carry = 0;
  t = (ecl_word) a.val[0] + b;
  res->val[0] = (ecl_digit) t;
  carry = (unsigned char) (t >> DIGIT_BITS);
  for (int i = 1; i < nb_limbs_; i++) {
    t = (ecl_word) a.val[i] + (ecl_word) carry;
    res->val[i] = (ecl_digit) t;
    carry = (unsigned char) (t >> DIGIT_BITS);
  }
  return carry;
}

template<int nb_limbs>
unsigned char FixedSizedInt<nb_limbs>::add_msw(
    FixedSizedInt<2 * nb_limbs> *res, const FixedSizedInt<2 * nb_limbs> &a,
    const FixedSizedInt<nb_limbs> &b) {
  ecl_word t;
  register unsigned char carry = 0;
  for (int i = 0; i < nb_limbs_; i++) {
    t = (ecl_word) a.val[nb_limbs_ + i] + (ecl_word) b.val[i]
        + (ecl_word) carry;
    res->val[nb_limbs_ + i] = (ecl_digit) t;
    carry = (unsigned char) (t >> DIGIT_BITS);
  }
  return carry;
}

template<int nb_limbs>
unsigned char FixedSizedInt<nb_limbs>::sub(FixedSizedInt<nb_limbs> *res,
                                           const FixedSizedInt<nb_limbs> &a,
                                           const FixedSizedInt<nb_limbs> &b) {
  ecl_word t;
  register unsigned char borrow = 0;
  for (int i = 0; i < nb_limbs_; i++) {
    t = (ecl_word) a.val[i] - (ecl_word) b.val[i] - (ecl_word) borrow;
    res->val[i] = (ecl_digit) t;
    borrow = (unsigned char) (t >> DIGIT_BITS) & 0x01;
  }
  return borrow;
}

template<int nb_limbs>
unsigned char FixedSizedInt<nb_limbs>::sub(FixedSizedInt<nb_limbs> *res,
                                           const FixedSizedInt<nb_limbs> &a,
                                           const ecl_digit b) {
  ecl_word t;
  register unsigned char borrow = 0;
  t = (ecl_word) a.val[0] - b;
  res->val[0] = (ecl_digit) t;
  borrow = (unsigned char) (t >> DIGIT_BITS) & 1;
  for (int i = 1; i < nb_limbs_; i++) {
    t = (ecl_word) a.val[i] - (ecl_word) borrow;
    res->val[i] = (ecl_digit) t;
    borrow = (unsigned char) ((t >> DIGIT_BITS) & 0x01);
  }
  return borrow;
}

template<int nb_limbs>
unsigned char FixedSizedInt<nb_limbs>::sub_msw(
    FixedSizedInt<2 * nb_limbs> *res, const FixedSizedInt<2 * nb_limbs> &a,
    const FixedSizedInt<nb_limbs> &b) {
  ecl_word t;
  register unsigned char borrow = 0;
  for (int i = 0; i < nb_limbs_; i++) {
    t = (ecl_word) a.val[nb_limbs_ + i] - (ecl_word) b.val[i]
        - (ecl_word) borrow;
    res->val[nb_limbs_ + i] = (ecl_digit) t;
    borrow = (unsigned char) ((t >> DIGIT_BITS) & 0x01);
  }
  return borrow;
}

template<int nb_limbs>
void FixedSizedInt<nb_limbs>::mul(FixedSizedInt<nb_limbs> *res,
                                  const FixedSizedInt<nb_limbs> &a,
                                  const FixedSizedInt<nb_limbs> &b) {
  register ecl_digit c0, c1, c2;
  register int i, j;
  register const ecl_digit *aa, *bb;

  COMBA_START;
  COMBA_CLEAR;

  for (i = 0; i < nb_limbs; i++) {
    aa = a.val;
    bb = b.val + i;
    COMBA_FORWARD;
    for (j = 0; j < i + 1; j++) {
      MULADD(*aa, *bb);
      aa++;
      bb--;
    }
    COMBA_STORE(res->val[i]);
  }
  COMBA_FINI;
}

} /* namespace ecl */

#endif /* ECL_SRC_BIGINT_FIXEDSIZEDINT_HPP_ */

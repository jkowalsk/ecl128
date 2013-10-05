/*
 * @file FixedSizedInt_4.cpp
 * @author Julien Kowalski
 */

#include "ecl/bigint/FixedSizedInt.h"
#include "FixedSizedInt.hpp"

namespace ecl {

template<>
unsigned char FixedSizedInt<4>::add(FixedSizedInt<4> *res,
                                    const FixedSizedInt<4> &a,
                                    const FixedSizedInt<4> &b) {
  register unsigned char carry = 0;

  ADD(res->val[0], a.val[0], b.val[0]);
  ADDC(res->val[1], a.val[1], b.val[1]);
  ADDC(res->val[2], a.val[2], b.val[2]);
  ADDC(res->val[3], a.val[3], b.val[3]);
  GET_CARRY(carry);

  return carry;
}

template<>
unsigned char FixedSizedInt<4>::add(FixedSizedInt<4> *res,
                                    const FixedSizedInt<4> &a,
                                    const ecl_digit b) {
  register unsigned char carry = 0;

  ADD(res->val[0], a.val[0], b);
  ADDC(res->val[1], a.val[1], 0);
  ADDC(res->val[2], a.val[2], 0);
  ADDC(res->val[3], a.val[3], 0);

  GET_CARRY(carry);

  return carry;
}

template<>
unsigned char FixedSizedInt<4>::add_msw(FixedSizedInt<8> *res,
                                        const FixedSizedInt<8> &a,
                                        const FixedSizedInt<4> &b) {
  register unsigned char carry = 0;

  ADD(res->val[4], a.val[4], b.val[0]);
  ADDC(res->val[5], a.val[5], b.val[1]);
  ADDC(res->val[6], a.val[6], b.val[2]);
  ADDC(res->val[7], a.val[7], b.val[3]);

  GET_CARRY(carry);

  return carry;
}

template<>
unsigned char FixedSizedInt<4>::sub(FixedSizedInt<4> *res,
                                    const FixedSizedInt<4> &a,
                                    const FixedSizedInt<4> &b) {
  register unsigned char borrow = 0;

  SUB(res->val[0], a.val[0], b.val[0]);
  SUBB(res->val[1], a.val[1], b.val[1]);
  SUBB(res->val[2], a.val[2], b.val[2]);
  SUBB(res->val[3], a.val[3], b.val[3]);

  GET_BORROW(borrow);

  return borrow;
}

template<>
unsigned char FixedSizedInt<4>::sub(FixedSizedInt<4> *res,
                                    const FixedSizedInt<4> &a,
                                    const ecl_digit b) {
  register unsigned char borrow = 0;

  SUB(res->val[0], a.val[0], b);
  SUBB(res->val[1], a.val[1], 0);
  SUBB(res->val[2], a.val[2], 0);
  SUBB(res->val[3], a.val[3], 0);

  GET_BORROW(borrow);

  return borrow;
}

template<>
unsigned char FixedSizedInt<4>::sub_msw(FixedSizedInt<8> *res,
                                        const FixedSizedInt<8> &a,
                                        const FixedSizedInt<4> &b) {
  register unsigned char borrow = 0;

  SUB(res->val[4], a.val[4], b.val[0]);
  SUBB(res->val[5], a.val[5], b.val[1]);
  SUBB(res->val[6], a.val[6], b.val[2]);
  SUBB(res->val[7], a.val[7], b.val[3]);

  GET_BORROW(borrow);

  return borrow;
}

template class FixedSizedInt<4> ;

} /* namespace ecl */


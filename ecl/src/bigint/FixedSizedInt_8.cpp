/*
 * @file FixedSizedInt_8.cpp
 * @author Julien Kowalski
 */

#include "ecl/bigint/FixedSizedInt.h"
#include "FixedSizedInt.hpp"

namespace ecl {

template<>
unsigned char FixedSizedInt<8>::add(FixedSizedInt<8> *res,
                                    const FixedSizedInt<8> &a,
                                    const FixedSizedInt<8> &b) {
  register unsigned char carry = 0;

  ADD(res->val[0], a.val[0], b.val[0]);
  ADDC(res->val[1], a.val[1], b.val[1]);
  ADDC(res->val[2], a.val[2], b.val[2]);
  ADDC(res->val[3], a.val[3], b.val[3]);
  ADDC(res->val[4], a.val[4], b.val[4]);
  ADDC(res->val[5], a.val[5], b.val[5]);
  ADDC(res->val[6], a.val[6], b.val[6]);
  ADDC(res->val[7], a.val[7], b.val[7]);

  GET_CARRY(carry);

  return carry;
}

template<>
unsigned char FixedSizedInt<8>::add(FixedSizedInt<8> *res,
                                    const FixedSizedInt<8> &a,
                                    const ecl_digit b) {
  register unsigned char carry = 0;
  ADD(res->val[0], a.val[0], b);
  ADDC(res->val[1], a.val[1], 0);
  ADDC(res->val[2], a.val[2], 0);
  ADDC(res->val[3], a.val[3], 0);
  ADDC(res->val[4], a.val[4], 0);
  ADDC(res->val[5], a.val[5], 0);
  ADDC(res->val[6], a.val[6], 0);
  ADDC(res->val[7], a.val[7], 0);

  GET_CARRY(carry);

  return carry;
}

template<>
unsigned char FixedSizedInt<8>::add_msw(FixedSizedInt<16> *res,
                                        const FixedSizedInt<16> &a,
                                        const FixedSizedInt<8> &b) {
  register unsigned char carry = 0;

  ADD(res->val[8], a.val[8], b.val[0]);
  ADDC(res->val[9], a.val[9], b.val[1]);
  ADDC(res->val[10], a.val[10], b.val[2]);
  ADDC(res->val[11], a.val[11], b.val[3]);
  ADDC(res->val[12], a.val[12], b.val[4]);
  ADDC(res->val[13], a.val[13], b.val[5]);
  ADDC(res->val[14], a.val[14], b.val[6]);
  ADDC(res->val[15], a.val[15], b.val[7]);

  GET_CARRY(carry);

  return carry;
}

template<>
unsigned char FixedSizedInt<8>::sub(FixedSizedInt<8> *res,
                                    const FixedSizedInt<8> &a,
                                    const FixedSizedInt<8> &b) {
  register unsigned char borrow = 0;

  SUB(res->val[0], a.val[0], b.val[0]);
  SUBB(res->val[1], a.val[1], b.val[1]);
  SUBB(res->val[2], a.val[2], b.val[2]);
  SUBB(res->val[3], a.val[3], b.val[3]);
  SUBB(res->val[4], a.val[4], b.val[4]);
  SUBB(res->val[5], a.val[5], b.val[5]);
  SUBB(res->val[6], a.val[6], b.val[6]);
  SUBB(res->val[7], a.val[7], b.val[7]);

  GET_BORROW(borrow);

  return borrow;
}

template<>
unsigned char FixedSizedInt<8>::sub(FixedSizedInt<8> *res,
                                    const FixedSizedInt<8> &a,
                                    const ecl_digit b) {
  register unsigned char borrow = 0;
  SUB(res->val[0], a.val[0], b);
  SUBB(res->val[1], a.val[1], 0);
  SUBB(res->val[2], a.val[2], 0);
  SUBB(res->val[3], a.val[3], 0);
  SUBB(res->val[4], a.val[4], 0);
  SUBB(res->val[5], a.val[5], 0);
  SUBB(res->val[6], a.val[6], 0);
  SUBB(res->val[7], a.val[7], 0);

  GET_BORROW(borrow);

  return borrow;
}

template<>
unsigned char FixedSizedInt<8>::sub_msw(FixedSizedInt<16> *res,
                                        const FixedSizedInt<16> &a,
                                        const FixedSizedInt<8> &b) {
  register unsigned char borrow = 0;

  SUB(res->val[8], a.val[8], b.val[0]);
  SUBB(res->val[9], a.val[9], b.val[1]);
  SUBB(res->val[10], a.val[10], b.val[2]);
  SUBB(res->val[11], a.val[11], b.val[3]);
  SUBB(res->val[12], a.val[12], b.val[4]);
  SUBB(res->val[13], a.val[13], b.val[5]);
  SUBB(res->val[14], a.val[14], b.val[6]);
  SUBB(res->val[15], a.val[15], b.val[7]);

  GET_BORROW(borrow);

  return borrow;
}

template class FixedSizedInt<8> ;

} /* namespace ecl */

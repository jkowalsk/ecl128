/**
 * @file gfp_addsub.cpp
 * @author Julien Kowalski
 */

#include "ecl/field/GFp.h"
// #include "ecl/private/gfp/GFpElement-impl.h"
#include "../asm/arch.h"

using ecl::ErrCode;

namespace ecl {
namespace field {

void GFp::add(Element *res, const Element &a, const Element &b) {
  register unsigned char carry = 0;
  carry = FixedSizedInt<NB_LIMBS>::add(res, a, b);
  if (carry || (cmp(*res, p_) != 1)) {
    FixedSizedInt<NB_LIMBS>::sub(res, *res, p_);
  }
}

void GFp::add_nr(Element *res, const Element &a, const Element b) {
  FixedSizedInt<NB_LIMBS>::add(res, a, b);
}

void GFp::add(Element *res, const Element &a, const ecl_digit b) {
  register unsigned char carry = 0;
  carry = FixedSizedInt<NB_LIMBS>::add(res, a, b);
  if (carry || (cmp(*res, p_) != 1)) {
    FixedSizedInt<NB_LIMBS>::sub(res, *res, p_);
  }
}

void GFp::add_nr(Element *res, const Element &a, const ecl_digit b) {
  FixedSizedInt<NB_LIMBS>::add(res, a, b);
}

void GFp::add(Double *res, const Double &a, const Double &b) {
  register unsigned char carry = 0;
  carry = FixedSizedInt<2 * NB_LIMBS>::add(res, a, b);
  if (carry || (cmp_p(*res) == -1)) {  // cmp_p mandatory...
    FixedSizedInt<NB_LIMBS>::sub_msw(res, *res, p_);
  }
}

void GFp::sub(Element *res, const Element &a, const Element &b) {
  register unsigned char borrow = 0;
  borrow = FixedSizedInt<NB_LIMBS>::sub(res, a, b);
  if (borrow) {
    FixedSizedInt<NB_LIMBS>::add(res, *res, p_);
  }
}

void GFp::sub(Element *res, const Element &a, const ecl_digit b) {
  register unsigned char borrow = 0;
  borrow = FixedSizedInt<NB_LIMBS>::sub(res, a, b);
  if (borrow) {
    FixedSizedInt<NB_LIMBS>::add(res, *res, p_);
  }
}

void GFp::sub(Double *res, const Double &a, const Double &b) {
  register unsigned char borrow = 0;
  borrow = FixedSizedInt<2 * NB_LIMBS>::sub(res, a, b);
  if (borrow) {
    FixedSizedInt<NB_LIMBS>::add_msw(res, *res, p_);
  }
}

void GFp::sub_nr(Double *res, const Double &a, const Double &b) {
  FixedSizedInt<2 * NB_LIMBS>::sub(res, a, b);
}

void GFp::opp(Element *res, const Element &a) {
  FixedSizedInt<NB_LIMBS>::sub(res, p_, a);
}

void GFp::opp(Double *res, const Double &a) {
  FixedSizedInt<2 * NB_LIMBS>::sub(res, Rp_, a);
}

}  // namespace field
}  // namespace ecl

/**
 * @file gfp_cmp.cpp
 * @author Julien Kowalski
 */

#include "ecl/field/GFp.h"
#include "../asm/arch.h"

using ecl::ErrCode;

namespace ecl {
namespace field {

bool GFp::isOne(const Element &a) {
  return (cmp(a, R_) == 0);
}

bool GFp::isZero(const Element &a) {
  return a.isZero();
}

void GFp::copy(Element *res, const Element &a) {
  res->copy(a);
}

int GFp::cmp(const Element &a, const Element &b) {
  int x;
  for (x = NB_LIMBS - 1; x >= 0; x--) {
    if (a.val[x] > b.val[x]) {
      return -1;
    } else if (a.val[x] < b.val[x]) {
      return 1;
    }
  }
  return 0;
}

int GFp::cmp_p(const Double &a) {
  int x;
  for (x = NB_LIMBS - 1; x >= 0; x--) {
    if (a.val[x + NB_LIMBS] > p_.val[x]) {
      return -1;
    } else if (a.val[x + NB_LIMBS] < p_.val[x]) {
      return 1;
    }
  }
  for (x = NB_LIMBS - 1; x >= 0; x--) {
    if (a.val[x] != 0) {
      return -1;
    }
  }
  return 0;
}

int GFp::cmp(const Double &a, const Double &b) {
  int x;
  for (x = 2 * NB_LIMBS - 1; x >= 0; x--) {
    if (a.val[x] > b.val[x]) {
      return -1;
    } else if (a.val[x] < b.val[x]) {
      return 1;
    }
  }
  return 0;
}

int GFp::cmp(const Element &a, int b) {
  Element bb;
  ecl_digit val;

  if (b < 0) {
    val = -b;
    set(&bb, val);
    opp(&bb, bb);
  } else {
    val = b;
    set(&bb, val);
  }
  return cmp(a, bb);
}

}  // namespace field
}  // namespace ecl

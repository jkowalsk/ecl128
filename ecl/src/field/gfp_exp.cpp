/**
 * @file gfp_exp.cpp
 * @author Julien Kowalski
 */

#include <iostream>

#include <cstring>
#include <string>

#include "ecl/field/GFp.h"

using ecl::ErrCode;

namespace ecl {
namespace field {

void GFp::exp(Element *res, const Element &a, const Element &e) {
  Element R;
  int i, l;

  l = e.count_bits();
  if (l == 0) {
    one(res);
    return;
  }

  R.copy(a);

  for (i = l - 1; i > 0; i--) {
    sqr(&R, R);
    if (e.get_bit(i - 1)) {
      mul(&R, R, a);
    }
  }
  res->copy(R);
}

ErrCode GFp::frobenius(Element *res, const Element &a, int i) {
  res->copy(a);
  return ERR_OK;
}

}  // namespace field
}  // namespace ecl

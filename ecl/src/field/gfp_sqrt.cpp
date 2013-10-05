/**
 * @file gfp_sqrt.cpp
 * @author Julien Kowalski
 */

#include "ecl/field/GFp.h"
#include "../asm/arch.h"

using ecl::ErrCode;

namespace ecl {
namespace field {

int GFp::legendre(const Element &a) {
  Element expo, pm1, tmp;
  int ret;

  if (a.isZero()) {
    return 0;
  }

  FixedSizedInt<NB_LIMBS>::sub(&pm1, p_, 1);
  r_shift(&expo, pm1);
  exp(&tmp, a, expo);

  if (isOne(tmp)) {
    ret = 1;
  } else {
    ret = -1;
  }

  return ret;
}

// implementation following
// http://math.univ-lyon1.fr/homes-www/roblot/resources/ens_partie_3.pdf
ErrCode GFp::tonelli_shanks(Element *res, const Element &a) {
  ErrCode ret = ERR_OK;
  //int carry;
  Element tmp, tmp1, tmp2, e1, e2, pm1, pm1s2;
  Element aa, tres, m1;
  Element g, one_e, q;

  // g such that g is a quadratic non residue mod p
  g.set(7);
  one_e.set(1);
  set(&m1, -1);

  do {
    add(&g, g, one_e);
  } while (legendre(g) != -1);

  FixedSizedInt<NB_LIMBS>::sub(&pm1, p_, 1);
  r_shift(&pm1s2, pm1);

  e1.copy(pm1s2);  // e1 = (p-1)/2
  e2.zero();  // e2 = 0 ;

  exp(&aa, g, pm1s2);

  aa.copy(a);

  while ((e1.val[0] & 0x01) == 0) {  // e1 even
    r_shift(&e1, e1);
    r_shift(&e2, e2);
    exp(&tmp1, aa, e1);
    exp(&tmp2, g, e2);
    mul(&tmp, tmp1, tmp2);

    // if a^e1 . g^e2 = -1 mod p
    if (tmp.eq(m1)) {
      // e2 = e2 + (p-1)/2
      FixedSizedInt<NB_LIMBS>::add(&e2, pm1s2, e2);
    }
  }

  // return a^(e1+1)/2 g^e2/2 mod p
  add(&tmp, e1, 1);
  r_shift(&tmp, tmp);
  exp(&tmp1, aa, tmp);
  r_shift(&e2, e2);
  exp(&tmp2, g, e2);
  mul(res, tmp1, tmp2);

  return ret;
}

ErrCode GFp::sqrt(Element *res, const Element &a) {
  ErrCode rv = ERR_OK;
  Element expo, tmp, tmp2;
  ecl_digit carry;

  if (legendre(a) != 1) {
    return ERR_NOT_SQUARE;
  }

  if ((p_.val[0] & 0x07) != 1) {
    if ((p_.val[0] & 0x03) == 3) {
      carry = (ecl_digit) FixedSizedInt<NB_LIMBS>::add(&expo, p_, 1);
      r_shift(&expo, expo, 2);
      expo.val[NB_LIMBS - 1] |= (carry << (DIGIT_BITS - 2));
      exp(res, a, expo);
      goto end;
    }

    if ((p_.val[0] & 0x07) == 5) {
      FixedSizedInt<NB_LIMBS>::sub(&expo, p_, 1);
      r_shift(&expo, expo, 2);
      exp(res, a, expo);

      if (isOne(*res)) {
        carry = (ecl_digit) FixedSizedInt<NB_LIMBS>::add(&expo, p_, 3);
        r_shift(&expo, expo, 3);
        expo.val[NB_LIMBS - 1] |= (carry << (DIGIT_BITS - 2));
        exp(res, a, expo);
        goto end;
        goto end;
      } else {  // must be -1
        FixedSizedInt<NB_LIMBS>::sub(&expo, p_, 5);
        r_shift(&expo, expo, 3);
        add(&tmp2, a, a);
        add(&tmp, tmp2, tmp2);
        exp(res, tmp, expo);
        mul(res, tmp2, *res);
        goto end;
      }
    }
  } else {
    return tonelli_shanks(res, a);
  }

  end: return rv;
}

bool GFp::isQNR(const Element &a) {
  return (legendre(a) != 1);
}

bool GFp::isCNR(const Element &a) {
  Element e, three, res;

  this->sub(&e, p_, 1);   // (p-1)/3
  this->set(&three, 3);
  div(&e, e, three);

  // only right if p == 1 mod 3
  this->exp(&res, a, e);

  return !isOne(res);  // CNR only if a^(p-1)/3 != 1 mod p
}

}  // namespace field
}  // namespace ecl

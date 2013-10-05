/*
 * @file fp2_mul.cpp
 * @author Julien Kowalski
 */
#include "ecl/field/GFp.h"
#include "ecl/field/Fp2.h"

using ecl::ErrCode;

namespace ecl {
namespace field {

void Fp2::mul(Element (*res), const Element &a, const Element &b) {
  Double d;
  mul(&d, a, b);
  reduce(res, d);
}

void Fp2::mul(Element (*res), const Element &a, const ecl_digit b) {
  gfp->mul(&((*res)[0]), a[0], b);
  gfp->mul(&((*res)[1]), a[1], b);
}

void Fp2::mul(Element (*res), const Element &a,
              const GFp::Element &b) {
  gfp->mul(&((*res)[0]), a[0], b);
  gfp->mul(&((*res)[1]), a[1], b);
}

void Fp2::mul(Double (*res), const Element &a, const Element &b) {
  GFp::Double t0, t1;
  GFp::Element t2, t3;

  gfp->mul(&t0, a[0], b[0]);
  gfp->mul(&t1, a[1], b[1]);

  gfp->add(&t2, a[0], a[1]);
  gfp->add(&t3, b[0], b[1]);

  gfp->mul(&((*res)[1]), t2, t3);
  gfp->sub(&((*res)[1]), (*res)[1], t0);
  gfp->sub(&((*res)[1]), (*res)[1], t1);

  gfp->mul(&t1, t1, gfp_qnr_);
  gfp->sub(&((*res)[0]), t0, t1);
}

void Fp2::mul(Double (*res), const Double &a, const ecl_digit b) {
  gfp->mul(&((*res)[0]), a[0], b);
  gfp->mul(&((*res)[1]), a[1], b);
}

void Fp2::sqr(Element (*res), const Element &a) {
  Double d;
  sqr(&d, a);
  reduce(res, d);
}

#if 1
void Fp2::sqr(Double (*res), const Element &a) {
  if (gfp_qnr_ == 1) {  // one mul instead of 2 squares
    GFp::Element t0, t1;
    gfp->add(&t0, a[0], a[1]);
    gfp->sub(&t1, a[0], a[1]);
    gfp->mul(&((*res)[0]), t0, t1);
  } else {
    gfp->sqr(&((*res)[0]), a[0]);
    gfp->sqr(&((*res)[1]), a[1]);
    gfp->mul(&((*res)[1]), ((*res)[1]), gfp_qnr_);
    gfp->sub(&((*res)[0]), ((*res)[0]), ((*res)[1]));
  }
  gfp->mul(&((*res)[1]), a[0], a[1]);
  gfp->add(&((*res)[1]), (*res)[1], (*res)[1]);
}
#else
//1. t ← addNC(a1, a1);
//2. d1 ← mul256(t, a0);
//3. t ← addNC(a0, p);
//4. t ← subNC(t, a1);
//5. c1 ← 5a1;
//6. c1 ← addNC(c1, a0);
//7. d0 ← mul256(t, c1);
//8. c1 ← mod512(d1);
//9. d1 ← addNC(d1, d1);
//10. d0 ← subNC(d0, d1);
//11. c0 ← mod512(d0);
//12. return C ← c0 +c1u;
void Fp2::sqr(Double (*res), const Element &a) {
  GFp::Element t, c0, c1;
  gfp->add_nr(&t, a[1], a[1]) ;
  gfp->mul(&((*res)[1]), t, a[0]) ;

}
#endif
/*
 Require: A = a0 + a1u in Fp2 .
 Ensure: res = C = c0 + c1u = A^-1 in Fp2 .
 1. t0 = a0^2;
 2. t1 = a1^2;
 3. t0 = t0 + t1; ( = t0 - i^2 t1, assuming u^2 = -1 )
 4. t1 = t0^-1;
 5. c0 =  a0 * t1;
 6. c1 = -a1 * t1;
 7. return C = c0 + c1u;
 */

void Fp2::inv(Element (*res), const Element &a) {
  GFp::Element t1, t0;

  gfp->sqr(&t0, a[0]);
  gfp->sqr(&t1, a[1]);
  gfp->mul(&t1, t1, gfp_qnr_);
  gfp->add(&t0, t0, t1);
  gfp->inv(&t1, t0);

  gfp->mul(&((*res)[0]), a[0], t1);

  gfp->mul(&((*res)[1]), a[1], t1);
  gfp->opp(&((*res)[1]), (*res)[1]);
}

void Fp2::div(Element (*res), const Element &a, const Element &b) {
  Element tmp;
  inv(&tmp, b);
  mul(res, a, tmp);
}

void Fp2::exp(Element (*res), const Element &a,
              const GFp::Element &e) {
  Element R;
  int i, l;

  l = e.count_bits();
  (R[0]).copy(a[0]);
  (R[1]).copy(a[1]);

  for (i = l - 1; i > 0; i--) {
    sqr(&R, R);
    if (e.get_bit(i - 1)) {
      mul(&R, R, a);
    }
  }
  ((*res)[0]).copy(R[0]);
  ((*res)[1]).copy(R[1]);
}

ErrCode Fp2::frobenius(Element (*res), const Element &a, int i) {
  switch (i % 2) {
    case 0:
      ((*res)[0]).copy(a[0]);
      ((*res)[1]).copy(a[1]);
      break;
    case 1:
      /* (a_0 + a_1 * u)^p = a_0 - a_1 * u. */
      ((*res)[0]).copy(a[0]);
      gfp->opp(&((*res)[1]), a[1]);
      break;
  }
  return ERR_OK;
}

void Fp2::mul_xsi(Element (*res), const Element &a) {
  GFp::Element t0, t1;
  switch (xsi_) {
    case THREE_ONE:   // xsi = 3 + i
      gfp->mul(&t0, a[0], 3);
      gfp->mul(&t1, a[1], gfp_qnr_);
      gfp->sub(&t0, t0, t1);

      gfp->mul(&((*res)[1]), a[1], 3);
      gfp->add(&((*res)[1]), (*res)[1], a[0]);
      ((*res)[0]).copy(t0);
      break;
    case TWO_ONE:   // xsi = 2 + i
      gfp->mul(&t0, a[0], 2);
      gfp->mul(&t1, a[1], gfp_qnr_);
      gfp->sub(&t0, t0, t1);

      gfp->mul(&((*res)[1]), a[1], 2);
      gfp->add(&((*res)[1]), (*res)[1], a[0]);
      ((*res)[0]).copy(t0);
      break;
    case ONE_ONE:  // xsi = 1 + i
      gfp->add(&t0, a[0], a[1]);
      gfp->mul(&t1, a[1], gfp_qnr_);
      gfp->sub(&((*res)[0]), a[0], t1);
      ((*res)[1]).copy(t0);
      break;
    case ZERO_ONE:  // xsi^2 = -gfp_qnr
      t0.copy(a[1]);
      ((*res)[1]).copy(a[0]);
      gfp->opp(&t0, t0);
      gfp->mul(&((*res)[0]), t0, gfp_qnr_);
      break;
    default:
      break;
  }
}

void Fp2::mul_xsi(Double (*res), const Double &a) {
  GFp::Double t0, t1;
  switch (xsi_) {
    case TWO_ONE:   // xsi = 2 + i ; qnr = 1
      gfp->add(&t0, a[0], a[0]);
      gfp->mul(&t1, a[1], gfp_qnr_);
      gfp->sub(&t0, t0, t1);

      gfp->add(&((*res)[1]), a[1], a[1]);
      gfp->add(&((*res)[1]), (*res)[1], a[0]);
      ((*res)[0]).copy(t0);
      break;
    case ONE_ONE:  // xsi = 1 + i
      gfp->add(&t0, a[0], a[1]);
      gfp->mul(&t1, a[1], gfp_qnr_);
      gfp->sub(&((*res)[0]), a[0], t1);
      ((*res)[1]).copy(t0);
      break;
    case ZERO_ONE:  // xsi^2 = -gfp_qnr
      t0.copy(a[1]);
      ((*res)[1]).copy(a[0]);
      gfp->opp(&t0, t0);
      gfp->mul(&((*res)[0]), t0, gfp_qnr_);
      break;
    default:
      break;
  }
}

int Fp2::legendre(const Element &a) {
  GFp::Element l, r;

  gfp->sqr(&l, a[0]);
  gfp->sqr(&r, a[1]);
  gfp->mul(&r, r, gfp_qnr_);
  gfp->add(&l, l, r);
  return gfp->legendre(l);
}

// based on formula from "implementing Cryptographic Pairings" by Michael Scott
ErrCode Fp2::sqrt(Element (*res), const Element &a) {
  GFp::Element tmp, l, r, norm;

  if (legendre(a) == -1) {
    return ERR_NOT_SQUARE;
  }

  gfp->sqr(&l, a[0]);
  gfp->sqr(&r, a[1]);
  gfp->mul(&r, r, gfp_qnr_);  // -xsi^2 i.e -i^2
  gfp->add(&norm, l, r);  // a^2 - (ib)^2
  gfp->sqrt(&norm, norm);  // sqrt(a^2 + (ib)^2) ; also checks if really square

  gfp->set(&l, 2);
  gfp->add(&r, a[0], norm);
  gfp->div(&tmp, r, l);  // [ a + sqrt(a^2 - (ib)^2) ] / 2

  if (gfp->legendre(tmp) != 1) {
    gfp->sub(&tmp, tmp, norm);  // [ a - sqrt(a^2 - (ib)^2) ] / 2
  }

  gfp->sqrt(&tmp, tmp);

  ((*res)[0]).copy(tmp);
  gfp->mul(&r, tmp, 2);
  gfp->div(&((*res)[1]), a[1], r);

  return ERR_OK;
}

}  // namespace field
}  // namespace ecl

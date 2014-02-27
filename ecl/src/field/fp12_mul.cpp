/*
 * @file fp12_mul.cpp
 * @author Julien Kowalski
 */

#include "ecl/field/GFp.h"
#include "ecl/field/Fp2.h"
#include "ecl/field/Fp6.h"
#include "ecl/field/Fp12.h"

using ecl::ErrCode;

namespace ecl {
namespace field {

void Fp12::mul(Element *res, const Element &a, const Element &b) {
  Double d;
  mul(&d, a, b);
  reduce(res, d);
}

void Fp12::mul(Element *res, const Element &a, const ecl_digit b) {
  fp6->mul(&((*res)[0]), a[0], b);
  fp6->mul(&((*res)[1]), a[1], b);
}

/* Fp12 = Fp6 [w] / (w^2 gamma).
 Require: A = a0 + a1w in Fp12 and B = b0 + b1w in Fp12 .
 Ensure: C = c0 + c1w = A * B in Fp12 .
 1. t0   a0 * b0;
 2. t1   a1 * b1;
 3. c0   t0 + t1 * V ;
 4. c1   (a0 + a1) * (b0 + b1) - t0 - t1;
 5. return C = c0 + c1w;
 */

void Fp12::mul(Double *res, const Element &a, const Element &b) {
  Fp6::Element s0, s1;
  Fp6::Double t1;

  fp6->mul(&((*res)[0]), a[0], b[0]);
  fp6->mul(&t1, a[1], b[1]);

  fp6->add(&s0, a[0], a[1]);
  fp6->add(&s1, b[0], b[1]);
  fp6->mul(&((*res)[1]), s1, s0);
  fp6->sub(&((*res)[1]), (*res)[1], (*res)[0]);
  fp6->sub(&((*res)[1]), (*res)[1], t1);

  fp6->mul_vi(&t1, t1);
  fp6->add(&((*res)[0]), (*res)[0], t1);
}

void Fp12::mul(Double *res, const Double &a, const ecl_digit b) {
  fp6->mul(&((*res)[0]), a[0], b);
  fp6->mul(&((*res)[1]), a[1], b);
}

/*
 Require: A = a0 + a1w 2 Fp12 .
 Ensure: C = c0 + c1w = A2 2 Fp12 .
 1. t0 =  a0 + a1;
 2. t1 =  a0 + gamma * a1;
 3. t0 =  t0 * t1;
 4. c1 =  a0 * a1;
 5. c0 =  t0 - c1;
 6. t1 = gamma * c1;
 7. c0 =  c0 - t1;
 8. c1 =  2c1 ;
 9. return C = c0 + c1w;
 */
// uses less temp element :
void Fp12::sqr(Element *res, const Element &a) {
  Fp6::Element t0, t1;

  fp6->add(&t0, a[0], a[1]);
  fp6->mul_vi(&t1, a[1]);
  fp6->add(&t1, a[0], t1);
  fp6->mul(&t0, t0, t1);
  fp6->mul(&((*res)[1]), a[0], a[1]);
  fp6->sub(&((*res)[0]), t0, (*res)[1]);
  fp6->mul_vi(&t1, (*res)[1]);
  fp6->sub(&((*res)[0]), (*res)[0], t1);
  fp6->add(&((*res)[1]), (*res)[1], (*res)[1]);
}

void Fp12::sqr(Double *res, const Element &a) {
  Fp6::Element t0, t1;
  Fp6::Double d1;

  fp6->add(&t0, a[0], a[1]);
  fp6->mul_vi(&t1, a[1]);
  fp6->add(&t1, a[0], t1);
  fp6->mul(&((*res)[0]), t0, t1);
  fp6->mul(&((*res)[1]), a[0], a[1]);
  fp6->sub(&((*res)[0]), (*res)[0], (*res)[1]);
  fp6->mul_vi(&d1, (*res)[1]);
  fp6->sub(&((*res)[0]), (*res)[0], d1);
  fp6->add(&((*res)[1]), (*res)[1], (*res)[1]);
}

/*
 Require: A = a0 + a1u in Fp12 .
 Ensure: res = C = c0 + c1u = A^-1 in Fp12 .
 1. t0 = a0^2;
 2. t1 = a1^2;
 3. t0 = t0 + t1; ( = t0 - i^2 t1, assuming u^2 = -1 )
 4. t1 = t0^-1;
 5. c0 =  a0 * t1;
 6. c1 = -a1 * t1;
 7. return C = c0 + c1u;
 */

void Fp12::inv(Element *res, const Element &a) {
  Fp6::Element t;
  Fp6::Double d0, d1;  // saves one redustion

  fp6->sqr(&d0, a[0]);
  fp6->sqr(&d1, a[1]);
  fp6->mul_vi(&d1, d1);
  fp6->sub(&d0, d0, d1);
  fp6->reduce(&t, d0);

  fp6->inv(&t, t);

  fp6->mul(&((*res)[0]), a[0], t);
  fp6->mul(&t, a[1], t);
  fp6->opp(&((*res)[1]), t);
}

void Fp12::div(Element *res, const Element &a, const Element &b) {
  Fp12::Element tmp;
  inv(&tmp, b);
  mul(res, a, tmp);
}

void Fp12::exp(Element *res, const Element &a, const GFp::Element &e) {
  Element R;
  int i, l;

  l = e.count_bits();
  copy(&R, a);

  for (i = l - 1; i > 0; i--) {
    sqr(&R, R);
    if (e.get_bit(i - 1)) {
      mul(&R, R, a);
    }
  }
  copy(res, R);
}

ErrCode Fp12::frobenius(Element *res, const Element &a, int i) {
  ErrCode rv = ERR_OK;
  Fp6::Element g, h;
  Fp2::Element t1, t2, t3, t4, t5, t6;

  fp6->copy(&g, a[0]);
  fp6->copy(&h, a[1]);

  switch (i) {
    case 1:
    case 3:  // rearrange and conjugate
      fp2->conj(&t1, g[0]);
      fp2->conj(&t2, h[0]);
      fp2->conj(&t3, g[1]);
      fp2->conj(&t4, h[1]);
      fp2->conj(&t5, g[2]);
      fp2->conj(&t6, h[2]);
      break;
    case 2:
      fp2->copy(&t1, g[0]);
      fp2->copy(&t2, h[0]);
      fp2->copy(&t3, g[1]);
      fp2->copy(&t4, h[1]);
      fp2->copy(&t5, g[2]);
      fp2->copy(&t6, h[2]);
      break;
    default:
      rv = ERR_NOT_IMPLEMENTED;
      goto end;
  }

  fp2->mul(&t2, t2, gamma[i - 1][0]);
  fp2->mul(&t3, t3, gamma[i - 1][1]);
  fp2->mul(&t4, t4, gamma[i - 1][2]);
  fp2->mul(&t5, t5, gamma[i - 1][3]);
  fp2->mul(&t6, t6, gamma[i - 1][4]);

  fp6->init(&g, t1, t3, t5);
  fp6->init(&h, t2, t4, t6);

  init(res, g, h);
  end: return rv;
}

}  // namespace field
}  // namespace ecl

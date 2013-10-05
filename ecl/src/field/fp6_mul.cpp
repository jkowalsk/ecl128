/*
 * @file fp6_mul.cpp
 * @author Julien Kowalski
 */
#include "ecl/field/GFp.h"
#include "ecl/field/Fp2.h"
#include "ecl/field/Fp6.h"

using ecl::ErrCode;

namespace ecl {
namespace field {

void Fp6::mul(Element *res, const Element &a, const Element &b) {
  Double tmp;
  mul(&tmp, a, b);
  reduce(res, tmp);
}

void Fp6::mul(Element *res, const Element &a, const Fp2::Element &b) {
  fp2->mul(&((*res)[0]), a[0], b);
  fp2->mul(&((*res)[1]), a[1], b);
  fp2->mul(&((*res)[2]), a[2], b);
}

void Fp6::mul(Element *res, const Element &a, const ecl_digit b) {
  fp2->mul(&((*res)[0]), a[0], b);
  fp2->mul(&((*res)[1]), a[1], b);
  fp2->mul(&((*res)[2]), a[2], b);
}

/** Require: A = a0 + a1v + a2v2 in Fp6 and B = b0 + b1v + b2v2 in Fp6 .
 Ensure: C = c0 + c1v + c2v2 = A * B in Fp6 .
 1. t0 = a0 * b0;
 2. t1 = a1 * b1;
 3. t2 = a2 * b2;
 4. c0 = [(a1 + a2) * (b1 + b2) - t1 - t2] * ksi + t0;
 5. c1 = (a0 + a1) * (b0 + b1) - t0 - t1 + ksi * t2;
 6. c2 = (a0 + a2) * (b0 + b2) - t0 - t2 + t1;
 */

void Fp6::mul(Double *res, const Element &a, const Element &b) {
  Fp2::Double t0, t1, t2;
  Fp2::Double c0, c1, c2;
  Fp2::Element s0, s1;

  fp2->mul(&t0, a[0], b[0]);
  fp2->mul(&t1, a[1], b[1]);
  fp2->mul(&t2, a[2], b[2]);

  fp2->add(&s0, a[1], a[2]);
  fp2->add(&s1, b[1], b[2]);
  fp2->mul(&c0, s0, s1);
  fp2->sub(&c0, c0, t1);
  fp2->sub(&c0, c0, t2);
  fp2->mul_xsi(&c0, c0);
  fp2->add(&((*res)[0]), c0, t0);

  fp2->add(&s0, a[1], a[0]);
  fp2->add(&s1, b[1], b[0]);
  fp2->mul(&c1, s0, s1);
  fp2->sub(&c1, c1, t0);
  fp2->sub(&c1, c1, t1);
  fp2->mul_xsi(&c2, t2);
  fp2->add(&((*res)[1]), c1, c2);

  fp2->add(&s0, a[0], a[2]);
  fp2->add(&s1, b[0], b[2]);
  fp2->mul(&c2, s0, s1);
  fp2->sub(&c2, c2, t0);
  fp2->sub(&c2, c2, t2);
  fp2->add(&((*res)[2]), c2, t1);
}

void Fp6::mul(Double *res, const Double &a, const ecl_digit b) {
  fp2->mul(&((*res)[0]), a[0], b);
  fp2->mul(&((*res)[1]), a[1], b);
  fp2->mul(&((*res)[2]), a[2], b);
}

void Fp6::sqr(Element *res, const Element &a) {
  Double d;
  sqr(&d, a);
  reduce(res, d);
}

/*
 Require: A = a0 + a1v + a2v2 2 Fp6 .
 Ensure: C = c0 + c1v + c2v2 = A2 2 Fp6 .
 1. c4  =  2(a0 * a1);
 2. c5  =  a2^2 ;
 3. c1  =  c5 * ksi + c4;
 4. c2  =  c4 - c5;
 5. c3  =  a0^2 ;
 6. c4  =  a0 - a1 + a2;
 7. c5  =  2(a1 * a2);
 8. c4  =  c4^2;
 9. c0  =  c5 * ksi + c3;
 10. c2 =   c2 + c4 + c5 - c3;
 11. return C = c0 + c1v + c2v^2;
 */

void Fp6::sqr(Double *res, const Element &a) {
  Fp2::Double c0, c1, c2, c3, c4, c5;
  Fp2::Element t;

  fp2->mul(&c4, a[0], a[1]);
  fp2->add(&c4, c4, c4);

  fp2->sqr(&c5, a[2]);
  fp2->mul_xsi(&c1, c5);
  fp2->add(&c1, c1, c4);

  fp2->sub(&c2, c4, c5);
  fp2->sqr(&c3, a[0]);

  fp2->sub(&t, a[0], a[1]);
  fp2->add(&t, t, a[2]);

  fp2->mul(&c5, a[2], a[1]);
  fp2->add(&c5, c5, c5);

  fp2->sqr(&c4, t);

  fp2->mul_xsi(&c0, c5);
  fp2->add(&((*res)[0]), c0, c3);

  fp2->add(&c2, c2, c4);
  fp2->add(&c2, c2, c5);
  fp2->sub(&((*res)[2]), c2, c3);

  fp2->copy(&((*res)[1]), c1);
}

/*
 Require: A = a0 + a1v + a2v2 in Fp6 .
 Ensure: C = c0 + c1v + c2v2 = A^-1 in Fp6 .
 1. t0 =  a0^2;
 2. t1 =  a1^2;
 3. t2 =  a2^2;
 4. t3 =  a0 * a1;
 5. t4 =  a0 * a2;
 6. t5 =  a2 * a1;
 7. c0 =  t0 - ksi * t5;
 8. c1 =  ksi * t2 - t3;
 9. c2 =  t1 - t4;
 10. t6 =  a0 * c0;
 11. t6 =  t6 + ksi * a2 * c1;
 12. t6 =  t6 + ksi * a1 * c2;
 13. t6 =  t6^-1 ;
 14. c0 =  c0 * t6;
 15. c1 =  c1 * t6;
 16. c2 =  c2 * t6;
 */

void Fp6::inv(Element *res, const Element &a) {
  Fp2::Element t0, t1, t2, t3, t4, t5, t6;
  Fp2::Element c0, c1, c2;

  fp2->sqr(&t0, a[0]);
  fp2->sqr(&t1, a[1]);
  fp2->sqr(&t2, a[2]);

  fp2->mul(&t3, a[0], a[1]);
  fp2->mul(&t4, a[0], a[2]);
  fp2->mul(&t5, a[1], a[2]);

  fp2->mul_xsi(&c0, t5);
  fp2->sub(&c0, t0, c0);

  fp2->mul_xsi(&c1, t2);
  fp2->sub(&c1, c1, t3);

  fp2->sub(&c2, t1, t4);

  fp2->mul(&t6, a[0], c0);
  fp2->mul(&t0, a[2], c1);
  fp2->mul_xsi(&t0, t0);
  fp2->add(&t6, t6, t0);

  fp2->mul(&t0, a[1], c2);
  fp2->mul_xsi(&t0, t0);
  fp2->add(&t6, t6, t0);

  fp2->inv(&t6, t6);

  fp2->mul(&((*res)[0]), c0, t6);
  fp2->mul(&((*res)[1]), c1, t6);
  fp2->mul(&((*res)[2]), c2, t6);
}

void Fp6::div(Element *res, const Element &a, const Element &b) {
  Element tmp;
  inv(&tmp, b);
  mul(res, a, tmp);
}

void Fp6::exp(Element *res, const Element &a, const GFp::Element &e) {
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

ErrCode Fp6::frobenius(Element *res, const Element &a, int i) {
  return ERR_NOT_IMPLEMENTED;
}

void Fp6::mul_vi(Element *res, const Element &a) {
  Fp2::Element tmp;

  fp2->copy(&tmp, a[0]);
  fp2->mul_xsi(&((*res)[0]), a[2]);
  fp2->copy(&((*res)[2]), a[1]);
  fp2->copy(&((*res)[1]), tmp);
}

void Fp6::mul_vi(Double *res, const Double &a) {
  Fp2::Double tmp;

  fp2->copy(&tmp, a[0]);
  fp2->mul_xsi(&((*res)[0]), a[2]);
  fp2->copy(&((*res)[2]), a[1]);
  fp2->copy(&((*res)[1]), tmp);
}

}  // namespace field
}  // namespace ecl

/*
 * @file bnpairing.cpp
 * @author Julien Kowalski
 */

#include "ecl/config.h"
#include "ecl/errcode.h"

#include "ecl/field/GFp.h"
#include "ecl/field/Fp2.h"
#include "ecl/field/Fp6.h"
#include "ecl/field/Fp12.h"

#include "ecl/curve/bncurve.h"
#include "ecl/curve/bnpairing.h"

using std::string;

using ecl::field::GFp;
using ecl::field::Fp2;
using ecl::field::Fp6;
using ecl::field::Fp12;

namespace ecl {
namespace curve {

BNPairing::BNPairing() {
  s_sz_ = 0;
  t_sign_ = 0;
  t_naf_sz_ = 0;
  gfp = NULL;
  fp2 = NULL;
  fp6 = NULL;
  fp12 = NULL;
}

BNPairing::~BNPairing() {
  s_sz_ = 0;
  t_sign_ = 0;
  t_naf_sz_ = 0;
  gfp = NULL;
  fp2 = NULL;
  fp6 = NULL;
  fp12 = NULL;
}

// compute s = | 6t+2 |

void BNPairing::get_6tp2(GFp::Element *res, const GFp::Element &t, int sign) {
  GFp::Element cst;

  cst.val[0] = 6;
  GFp::Element::mul(res, t, cst);
  if (sign > 0) {
    GFp::Element::add(res, *res, 2);
  } else {
    GFp::Element::sub(res, *res, 2);
  }
}

ErrCode BNPairing::init(string t, string b) {
  GFp::Element tt, ss;
  ErrCode rv;

  tt.fromString(&t_sign_, t);
  rv = E1.initialize(t, b);
  if (rv != ERR_OK)
    return rv;
  rv = E2.initialize(t, b);
  if (rv != ERR_OK)
    return rv;

  tt.get_wNAF(t_naf_, &t_naf_sz_, 2);
  get_6tp2(&ss, tt, t_sign_);
  ss.get_wNAF(s_, &s_sz_, 2);

  gfp = E1.getField();
  fp2 = E2.getField();
  gfp->get_characteristic(&prime_);
  fp6 = new Fp6(prime_);
  fp12 = new Fp12(prime_);

  E1.get_trace(&trace_);
  E1.get_order(&order_);

  return ERR_OK;
}

/*************** Miller loop part */

void BNPairing::mulFp2Fp2(Fp6::Element *z, const Fp6::Element& x,
                          const Fp2::Element& ya, const Fp2::Element& yb) {
  Fp2::Element v0, v1, tmp;

  fp2->mul(&v0, x[0], ya);
  fp2->mul(&v1, x[1], yb);
  fp2->add(&((*z)[0]), x[1], x[2]);
  fp2->mul(&((*z)[0]), (*z)[0], yb);
  fp2->sub(&((*z)[0]), (*z)[0], v1);
  fp2->mul_xsi(&((*z)[0]), (*z)[0]);
  fp2->add(&((*z)[0]), (*z)[0], v0);
  fp2->add(&((*z)[1]), x[0], x[1]);

  fp2->add(&tmp, ya, yb);
  fp2->mul(&((*z)[1]), (*z)[1], tmp);
  fp2->sub(&((*z)[1]), (*z)[1], v0);
  fp2->sub(&((*z)[1]), (*z)[1], v1);
  fp2->add(&((*z)[2]), x[0], x[2]);
  fp2->mul(&((*z)[2]), (*z)[2], ya);
  fp2->sub(&((*z)[2]), (*z)[2], v0);
  fp2->add(&((*z)[2]), (*z)[2], v1);
}

void BNPairing::mult_L(Fp12::Element *f, const Fp12::Element lpq) {
  Fp6::Element v0, v1;
  Fp6::Element t0;
  Fp6::Element &f0 = (*f)[0];
  Fp6::Element &f1 = (*f)[1];
  const Fp6::Element &l0 = lpq[0];
  const Fp6::Element &l1 = lpq[1];
  Fp2::Element tmp;
  const Fp2::Element &l00 = l0[0];
  const Fp2::Element &l10 = l1[0];
  const Fp2::Element &l11 = l1[1];

  fp6->mul(&v0, f0, l00);

  mulFp2Fp2(&v1, f1, l10, l11);

  fp6->add(&t0, f0, f1);
  fp2->add(&tmp, l00, l10);
  mulFp2Fp2(&f1, t0, tmp, l11);

  fp6->mul_vi(&f0, v1);

  fp6->add(&f0, f0, v0);

  fp6->sub(&f1, f1, v0);
  fp6->sub(&f1, f1, v1);
}

void BNPairing::doubleAndEvalLine(Fp2BnCurve::Point *T, Fp12::Element *lqq,
                                  const GFpBnCurve::Point &P,
                                  const Fp2BnCurve::Point &Q) {
  Fp2::Element tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp;
  Fp2::Element Tx, Ty, Tz, zero;
  Fp6::Element a0, a1;

  fp2->zero(&zero);

  fp2->sqr(&tmp0, Q.x);
  fp2->sqr(&tmp1, Q.y);
  fp2->sqr(&tmp2, tmp1);

  fp2->add(&tmp3, tmp1, Q.x);
  fp2->sqr(&tmp3, tmp3);
  fp2->sub(&tmp3, tmp3, tmp0);
  fp2->sub(&tmp3, tmp3, tmp2);
  fp2->add(&tmp3, tmp3, tmp3);

  fp2->mul(&tmp4, tmp0, 3);
  fp2->add(&tmp6, tmp4, Q.x);
  fp2->sqr(&tmp5, tmp4);

  fp2->sub(&Tx, tmp5, tmp3);
  fp2->sub(&Tx, Tx, tmp3);

  fp2->add(&Tz, Q.y, Q.z);
  fp2->sqr(&Tz, Tz);
  fp2->sub(&Tz, Tz, tmp1);
  fp2->sub(&Tz, Tz, Q.z2);

  fp2->sub(&Ty, tmp3, Tx);
  fp2->mul(&Ty, Ty, tmp4);
  fp2->mul(&tmp, tmp2, 8);
  fp2->sub(&Ty, Ty, tmp);

  fp2->mul(&tmp3, tmp4, Q.z2);
  fp2->mul(&tmp3, tmp3, 2);
  fp2->opp(&tmp3, tmp3);

  fp2->mul(&tmp3, tmp3, P.x);

  fp2->sqr(&tmp6, tmp6);
  fp2->sub(&tmp6, tmp6, tmp0);
  fp2->sub(&tmp6, tmp6, tmp5);
  fp2->mul(&tmp, tmp1, 4);
  fp2->sub(&tmp6, tmp6, tmp);

  fp2->mul(&tmp0, Tz, Q.z2);
  fp2->add(&tmp0, tmp0, tmp0);
  fp2->mul(&tmp0, tmp0, P.y);

  fp6->init(&a0, tmp0, zero, zero);
  fp6->init(&a1, tmp3, tmp6, zero);
  fp12->init(lqq, a0, a1);

  E2.init(T, Tx, Ty, Tz);
}

void BNPairing::addAndEvalLine(Fp2BnCurve::Point *T, Fp12::Element *lqq,
                               const GFpBnCurve::Point &P,
                               const Fp2BnCurve::Point &Q,
                               const Fp2BnCurve::Point &R) {
  Fp2::Element t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, Qy2;
  Fp2::Element Tx, Ty, Tz, zero, tmp;
  Fp6::Element a0, a1;

  fp2->zero(&zero);
  fp2->mul(&t0, Q.x, R.z2);

  fp2->add(&t1, Q.y, R.z);
  fp2->sqr(&t1, t1);
  fp2->sqr(&Qy2, Q.y);
  fp2->sub(&t1, t1, Qy2);
  fp2->sub(&t1, t1, R.z2);

  fp2->mul(&t1, t1, R.z2);

  fp2->sub(&t2, t0, R.x);

  fp2->sqr(&t3, t2);

  fp2->mul(&t4, t3, 4);

  fp2->mul(&t5, t4, t2);

  fp2->sub(&t6, t1, R.y);
  fp2->sub(&t6, t6, R.y);

  fp2->mul(&t9, t6, Q.x);

  fp2->mul(&t7, t4, R.x);

  fp2->sqr(&Tx, t6);
  fp2->sub(&Tx, Tx, t5);
  fp2->sub(&Tx, Tx, t7);
  fp2->sub(&Tx, Tx, t7);

  fp2->add(&Tz, R.z, t2);
  fp2->sqr(&Tz, Tz);
  fp2->sub(&Tz, Tz, R.z2);
  fp2->sub(&Tz, Tz, t3);

  fp2->add(&t10, Tz, Q.y);

  fp2->sub(&t8, t7, Tx);
  fp2->mul(&t8, t8, t6);

  fp2->mul(&t0, R.y, t5);
  fp2->mul(&t0, t0, 2);

  fp2->sub(&Ty, t8, t0);

  fp2->sqr(&t10, t10);
  fp2->sub(&t10, t10, Qy2);
  fp2->sqr(&tmp, Tz);
  fp2->sub(&t10, t10, tmp);

  fp2->mul(&t9, t9, 2);
  fp2->sub(&t9, t9, t10);

  fp2->mul(&t10, Tz, P.y);
  fp2->add(&t10, t10, t10);

  fp2->opp(&t6, t6);

  fp2->mul(&t1, t6, P.x);
  fp2->add(&t1, t1, t1);

  fp6->init(&a0, t10, zero, zero);
  fp6->init(&a1, t1, t9, zero);
  fp12->init(lqq, a0, a1);

  E2.init(T, Tx, Ty, Tz);
}

void BNPairing::millerLoop(Fp12::Element *f, const GFpBnCurve::Point &P,
                           const Fp2BnCurve::Point &Q) {
  GFpBnCurve::Point PP;
  Fp2BnCurve::Point T;
  Fp12::Element lpq;
  Fp2BnCurve::Point mQ, Q1, Q2, QQ;

  E2.init(&T);
  E2.init(&QQ);
  E2.init(&mQ);
  E2.init(&Q1);
  E2.init(&Q2);
  E2.init(&Q2);

  E2.copy(&QQ, Q);
  E2.normalize(&QQ);
  E1.copy(&PP, P);
  E1.normalize(&PP);

  fp12->one(f);

  E2.copy(&T, QQ);
  E2.opp(&mQ, T);

  for (int i = s_sz_ - 2; i >= 0; i--) {
    fp12->sqr(f, *f);
    doubleAndEvalLine(&T, &lpq, PP, T);
    mult_L(f, lpq);
    if (s_[i]) {
      if (s_[i] < 0) {
        addAndEvalLine(&T, &lpq, PP, mQ, T);
      } else {
        addAndEvalLine(&T, &lpq, PP, QQ, T);
      }
      mult_L(f, lpq);
    }
  }

  E2.frobenius(&Q1, QQ, 1);
  E2.frobenius(&Q2, QQ, 2);
  E2.opp(&Q2, Q2);

  if (t_sign_ < 0) {
    E2.opp(&T, T);
    fp12->conj(f, *f);
  }

  addAndEvalLine(&T, &lpq, PP, Q1, T);
  mult_L(f, lpq);
  addAndEvalLine(&T, &lpq, PP, Q2, T);
  mult_L(f, lpq);

  E2.zero(&T);
  E2.zero(&mQ);
  E2.zero(&QQ);
  E2.zero(&Q1);
  E2.zero(&Q2);
}

/*************** Exponentiation part */

void BNPairing::sqr_fp4(Fp2::Element *c0, Fp2::Element *c1,
                        const Fp2::Element &a, const Fp2::Element &b) {
  Fp2::Element t0, t1;
  fp2->sqr(&t0, a);
  fp2->sqr(&t1, b);
  fp2->mul_xsi(c0, t1);
  fp2->add(c0, *c0, t0);
  fp2->add(c1, a, b);
  fp2->sqr(c1, *c1);
  fp2->sub(c1, *c1, t0);
  fp2->sub(c1, *c1, t1);
}

void BNPairing::sqr_cycl(Fp12::Element *res, const Fp12::Element &f) {
  Fp6::Element t0, t1;
  Fp2::Element t;
  Fp2::Element &t00 = t0[0];
  Fp2::Element &t01 = t0[1];
  Fp2::Element &t02 = t0[2];
  Fp2::Element &t10 = t1[0];
  Fp2::Element &t11 = t1[1];
  Fp2::Element &t12 = t1[2];

  sqr_fp4(&t00, &t11, f[0][0], f[1][1]);
  sqr_fp4(&t01, &t12, f[1][0], f[0][2]);
  sqr_fp4(&t02, &t, f[0][1], f[1][2]);
  fp2->mul_xsi(&t10, t);

  fp6->add(&((*res)[0]), f[0], f[0]);
  fp6->mul(&t0, t0, 3);
  fp6->sub(&((*res)[0]), t0, (*res)[0]);

  fp6->add(&((*res)[1]), f[1], f[1]);
  fp6->mul(&t1, t1, 3);
  fp6->add(&((*res)[1]), t1, (*res)[1]);
}

void BNPairing::exp_t(Fp12::Element *res, const Fp12::Element &f) {
  Fp12::Element tmp, inv_f;

  fp12->conj(&inv_f, f);  // f cyclotomic
  fp12->copy(&tmp, f);

  for (int i = t_naf_sz_ - 2; i >= 0; i--) {
    sqr_cycl(&tmp, tmp);
    if (t_naf_[i]) {
      if (t_naf_[i] > 0) {
        fp12->mul(&tmp, tmp, f);
      } else {
        fp12->mul(&tmp, tmp, inv_f);
      }
    }
  }

  fp12->copy(res, tmp);
}

void BNPairing::finalExp(Fp12::Element *res, const Fp12::Element &f) {
  Fp12::Element ff, y0, y1, y2, y3;

  /* First, compute ff = f^(p^6 - 1). */
  fp12->conj(&y1, f); /* f^(p^6). */
  fp12->inv(&y2, f); /* f^{-1}. */
  fp12->mul(&ff, y2, y1); /* f^(p^6 - 1). */

  /* Second, compute ff = ff^(p^2 + 1). */
  fp12->frobenius(&y2, ff, 2); /* ff^(p^2). */
  fp12->mul(&ff, y2, ff); /* ff^(p^2+1). */

  /* Then the hard part : compute ff^( (p^4-p^2+1)/r) */
  /* y0 = f^2x. */
  exp_t(&y0, ff);
  sqr_cycl(&y0, y0);
  /* y1 = f^6x. */
  sqr_cycl(&y1, y0);
  fp12->mul(&y1, y1, y0);
  /* y2 = f^6x^2. */
  exp_t(&y2, y1);
  /* y3 = f^12x^3. */
  sqr_cycl(&y3, y2);
  exp_t(&y3, y3);

  if (t_sign_ < 0) {
    fp12->conj(&y0, y0);
    fp12->conj(&y1, y1);
    fp12->conj(&y3, y3);
  }

  /* y3 = a = f^12x^3 *f^6x^2 * f^6x. = y3 * y2 * y1 */
  fp12->mul(&y3, y3, y2);
  fp12->mul(&y3, y3, y1);
  /* y0 = b = 1/(f^2x) * y3 = 1/y0 * y3*/
  fp12->conj(&y0, y0);
  fp12->mul(&y0, y3, y0);

  /* Compute y2 * y3 * f * [b * 1/f]^p^3 * b^p * a^p^2 . */
  fp12->mul(&y2, y2, y3);
  fp12->mul(&y2, y2, ff);
  fp12->conj(&ff, ff);
  fp12->mul(&ff, ff, y0);
  fp12->frobenius(&ff, ff, 3);
  fp12->mul(&ff, ff, y2);
  fp12->frobenius(&y0, y0, 1);
  fp12->mul(&ff, ff, y0);
  fp12->frobenius(&y3, y3, 2);
  fp12->mul(res, ff, y3);
}

void BNPairing::pair(Fp12::Element *res, const GFpBnCurve::Point &P,
                     const Fp2BnCurve::Point &Q) {
  fp12->zero(res);
  millerLoop(res, P, Q);
  finalExp(res, *res);
}


ErrCode BNPairingFactory::getParameters(BNPairing *ate,
                                        GFpBnCurve::Point *gfp_generator,
                                        Fp2BnCurve::Point *fp2_generator,
                                        BnCurveDefinition curve_def) {
  bn_curve_string_def *definition;
  GFpBnCurve  *e1;
  Fp2BnCurve  *e2;
  unsigned char b[32];
  ErrCode rv;

  std::memset(b, 0, 32);
  b[0] = 2;

  switch (curve_def) {
    case BN_BEUCHAT_254:
      definition = &beuchat_254_curve;
      break;
    case BN_ARANHA_254:
      definition = &aranha_254_curve;
      break;
    case BN_NAERING_256:
      definition = &naering_256_curve;
      break;
    default:
      return ERR_INVALID_VALUE;
  }

  rv = ate->init(definition->t, definition->b);
  e1 = ate->getE1();
  e1->hash(gfp_generator, b, NULL, NULL);
  e2 = ate->getE2();
  e2->hash(fp2_generator, b, NULL, NULL);

  return rv;
}

} /* namespace curve */
} /* namespace ecl */

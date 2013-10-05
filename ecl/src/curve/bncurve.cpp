/*
 * @file bncurve.h
 * @author Julien Kowalski
 */

#include <string>
#include <iostream>

#include "ecl/config.h"
#include "ecl/errcode.h"

#include "ecl/field/GFp.h"
#include "ecl/field/Fp2.h"
#include "ecl/field/Fp6.h"
#include "ecl/field/Fp12.h"
#include "ecl/curve/curve.h"
#include "ecl/curve/bncurve.h"

using std::string;

namespace ecl {
namespace curve {

template<class Basefield>
void BnCurve<Basefield >::set_prime() {
  typename GFp::Element tmp, t2, t3, t4, res;
  typename GFp::Element cst;

  GFp::Element::mul(&t2, t_, t_);
  GFp::Element::mul(&t3, t2, t_);
  GFp::Element::mul(&t4, t3, t_);

  // 36 t^4
  cst.val[0] = 36;
  GFp::Element::mul(&res, t4, cst);

  // 36 t^4 + 36 t^3
  GFp::Element::mul(&tmp, t3, cst);
  if (t_sign_ > 0) {
    GFp::Element::add(&res, res, tmp);
  } else {
    GFp::Element::sub(&res, res, tmp);
  }

  //  36 t^4 + 36 t^3 + 24 t^2
  cst.val[0] = 24;
  GFp::Element::mul(&tmp, t2, cst);
  GFp::Element::add(&res, res, tmp);

  //  36 t^4 + 36 t^3 + 24 t^2 + 6t
  cst.val[0] = 6;
  GFp::Element::mul(&tmp, t_, cst);
  if (t_sign_ > 0) {
    GFp::Element::add(&res, res, tmp);
  } else {
    GFp::Element::sub(&res, res, tmp);
  }

  //  36 t^4 + 36 t^3 + 24 t^2 + 6t + 1
  GFp::Element::add(&prime_, res, 1);
}

template<class Basefield>
void BnCurve<Basefield>::set_order() {
  typename GFp::Element tmp, t2, t3, t4, res;
  typename GFp::Element cst;

  GFp::Element::mul(&t2, t_, t_);
  GFp::Element::mul(&t3, t2, t_);
  GFp::Element::mul(&t4, t3, t_);

  // 36 t^4
  cst.val[0] = 36;
  GFp::Element::mul(&res, t4, cst);

  // 36 t^4 + 36 t^3
  GFp::Element::mul(&tmp, t3, cst);
  if (t_sign_ > 0) {
    GFp::Element::add(&res, res, tmp);
  } else {
    GFp::Element::sub(&res, res, tmp);
  }

  //  36 t^4 + 36 t^3 + 18 t^2
  cst.val[0] = 18;
  GFp::Element::mul(&tmp, t2, cst);
  GFp::Element::add(&res, res, tmp);

  //  36 t^4 + 36 t^3 + 18 t^2 + 6t
  cst.val[0] = 6;
  GFp::Element::mul(&tmp, t_, cst);
  if (t_sign_ > 0) {
    GFp::Element::add(&res, res, tmp);
  } else {
    GFp::Element::sub(&res, res, tmp);
  }

  //  36 t^4 + 36 t^3 + 18 t^2 + 6t + 1
  GFp::Element::add(&this->order_, res, 1);
}

template<class Basefield>
void BnCurve<Basefield>::set_trace() {
  typename GFp::Element tmp, cst;

  // 6 t^2 + 1
  GFp::Element::mul(&tmp, t_, t_);
  cst.val[0] = 6;
  GFp::Element::mul(&tmp, tmp, cst);
  GFp::Element::add(&trace_, tmp, 1);
}

ErrCode GFpBnCurve::initialize(string t, string b) {
  typename GFp::Element tmp;
  ErrCode rv = ERR_OK;

  rv = this->t_.fromString(&this->t_sign_, t);
  if (rv != ERR_OK)
    goto end;

  this->set_prime();
  this->set_order();
  this->set_trace();

  this->field_ = new GFp(this->prime_);
  this->gfp_ = this->field_;

  this->field_->set(&tmp, -3);
  if (this->field_->sqrt(&this->sqrt_m3_, tmp) != ERR_OK) {
    return ERR_NOT_IMPLEMENTED;
  }

  this->field_->zero(&this->a_);
  this->a_is_0_ = true;

  this->field_->fromString(&this->b_, b);

  end: return rv;
}

ErrCode Fp2BnCurve::initialize(string t, string b) {
  GFp::Element tmp, b_gfp, zero_e;
  Fp2::Element xsi;
  ErrCode rv = ERR_OK;

  rv = this->t_.fromString(&this->t_sign_, t);
  if (rv != ERR_OK)
    goto end;

  this->set_prime();
  this->set_order();
  this->set_trace();

  this->field_ = new Fp2(this->prime_);
  this->gfp_ = this->field_->getBasePrimeField();

  this->gfp_->set(&tmp, -3);
  if (this->gfp_->sqrt(&this->sqrt_m3_, tmp) != ERR_OK) {
    return ERR_NOT_IMPLEMENTED;
  }

  this->field_->zero(&this->a_);
  this->a_is_0_ = true;

  // set b
  this->gfp_->fromString(&b_gfp, b);
  this->gfp_->zero(&zero_e);
  this->field_->get_xsi(&xsi);
  this->field_->init(&this->b_, b_gfp, zero_e);

  this->field_->div(&this->b_, this->b_, xsi);

  this->gfp_->zero(&this->twist_cofactor_);
  GFp::Element::sub(&this->twist_cofactor_, this->prime_, this->order_);
  GFp::Element::add(&this->twist_cofactor_, this->prime_,
                    this->twist_cofactor_);
  calc_frb_cst();

  end: return rv;
}

void Fp2BnCurve::calc_frb_cst() {
  typename GFp::Element e, z, one, six;
  typename Fp2::Element t0, t1;

  this->gfp_->one(&one);

  this->field_->get_xsi(&t0);

  this->gfp_->set(&six, 6);
  this->gfp_->get_characteristic(&e);
  this->gfp_->sub(&e, e, 1);
  this->gfp_->div(&e, e, six);

  this->field_->exp(&(const_frb[0]), t0, e);
  this->field_->sqr(&(const_frb[1]), const_frb[0]);
  this->field_->mul(&(const_frb[2]), const_frb[1], const_frb[0]);
  this->field_->sqr(&(const_frb[3]), const_frb[1]);
  this->field_->mul(&(const_frb[4]), const_frb[3], const_frb[0]);

  this->field_->frobenius(&t1, const_frb[0], 1);
  this->field_->mul(&(const_sqr[0]), t1, const_frb[0]);
  this->field_->sqr(&(const_sqr[1]), const_sqr[0]);
  this->field_->mul(&(const_sqr[2]), const_sqr[1], const_sqr[0]);

  for (int i = 0; i < 5; i++) {
    this->field_->mul(&(const_cub[i]), const_sqr[i % 3], const_frb[i]);
  }
}

template<class Basefield>
void BnCurve<Basefield>::hash(Point *res, unsigned char buff[32],
                              int (*f_rng)(unsigned char *, int, void *),
                              void *p_rng) {
  typename Basefield::Element w, x[3], xb[3];
  typename Basefield::Element r[3];
  typename GFp::Element tt, t2, t3;
  int alpha, beta, i;

  this->field_->one(&(r[0]));
  this->field_->one(&(r[1]));
  this->field_->one(&(r[2]));

  memcpy(tt.val, buff, 32);
  buff[32 - 1] = 0;

  this->field_->one(&w);
  this->field_->one(&(x[0]));
  this->field_->mul(&w, w, tt);
  this->field_->sqr(&w, w);
  this->field_->add(&w, w, this->b_);
  this->field_->add(&w, w, x[0]);
  this->field_->inv(&w, w);
  this->field_->mul(&w, w, tt);
  this->field_->mul(&w, w, sqrt_m3_);  // w = sqrt(-3).t / (1+b+t^2)

  this->gfp_->set(&t3, 2);
  this->gfp_->inv(&t3, t3);
  this->gfp_->one(&t2);
  this->gfp_->sub(&t2, sqrt_m3_, t2);
  this->gfp_->mul(&t2, t2, t3);

  this->field_->mul(&(x[0]), w, tt);
  this->field_->opp(&(x[0]), x[0]);
  this->field_->add(&(x[0]), x[0], t2);  // x1 = (-1+sqrt(-3))/2 - tw

  this->field_->one(&(r[0]));
  this->field_->opp(&(x[1]), r[0]);
  this->field_->sub(&(x[1]), x[1], x[0]);  // x2 = -1 -x1

  this->field_->sqr(&(x[2]), w);
  this->field_->inv(&(x[2]), x[2]);
  this->field_->add(&(x[2]), x[2], r[0]);  // 1 + 1/w^2

  if (f_rng != NULL) {
    this->field_->rand(&(r[0]), f_rng, p_rng);
    this->field_->rand(&(r[1]), f_rng, p_rng);
  }

  this->field_->sqr(&(xb[0]), x[0]);
  this->field_->mul(&(xb[0]), xb[0], x[0]);
  this->field_->add(&(xb[0]), xb[0], this->b_);  // (x1^3+b)
  this->field_->mul(&(r[2]), xb[0], r[0]);
  this->field_->mul(&(r[2]), r[2], r[0]);  // r1^2.(x1^3+b)
  alpha = this->field_->legendre(r[2]);

  this->field_->sqr(&(xb[1]), x[1]);
  this->field_->mul(&(xb[1]), xb[1], x[1]);
  this->field_->add(&(xb[1]), xb[1], this->b_);  // (x2^3+b)
  this->field_->mul(&(r[2]), xb[1], r[1]);
  this->field_->mul(&(r[2]), r[2], r[1]);  // r2^2.(x2^3+b)
  beta = this->field_->legendre(r[2]);

  this->field_->sqr(&(xb[2]), x[2]);
  this->field_->mul(&(xb[2]), xb[2], x[2]);
  this->field_->add(&(xb[2]), xb[2], this->b_);  // (x2^3+b)

  i = ((alpha - 1) * beta + 3) % 3;  // +3 to be sure it is positive...

  if (f_rng != NULL) {
    this->field_->rand(&(r[2]), f_rng, p_rng);
    this->field_->sqr(&(r[2]), r[2]);
    this->field_->mul(&(r[2]), r[2], tt);
    alpha = this->field_->legendre(r[2]);
  } else {
    this->field_->one(&(r[2]));
    alpha = this->gfp_->legendre(tt);
  }

  this->field_->set(&(r[0]), alpha);
  this->field_->sqrt(&(r[1]), xb[i]);
  this->field_->mul(&(r[1]), r[1], r[0]);

  this->init(res, x[i], r[1]);

  if (Basefield::getExtensionDegre() == 2) {
    this->mul(res, *res, twist_cofactor_);
  }
}

ErrCode GFpBnCurve::frobenius(Point *res, const Point &P, int i) {
  this->copy(res, P);
  return ERR_OK;
}

ErrCode Fp2BnCurve::frobenius(Point *res, const Point &P, int i) {
  Point PP;
  this->copy(&PP, P);

  if (!this->field_->isOne(PP.z)) {
    this->normalize(&PP);
  }

  switch (i) {
    case 1:
      this->field_->frobenius(&(res->x), PP.x, 1);
      this->field_->frobenius(&(res->y), PP.y, 1);
      this->field_->mul(&(res->x), res->x, const_frb[1]);
      this->field_->mul(&(res->y), res->y, const_frb[2]);
      break;
    case 2:
      this->field_->mul(&(res->x), PP.x, const_sqr[1]);
      this->field_->opp(&(res->y), PP.y);
      break;
    case 3:
      this->field_->frobenius(&(res->x), PP.x, 1);
      this->field_->mul(&(res->x), res->x, const_cub[1]);
      this->field_->opp(&(res->y), PP.y);
      this->field_->conj(&(res->y), res->y);
      this->field_->mul(&(res->y), res->y, const_frb[2]);
      break;
    default:
      return ERR_NOT_IMPLEMENTED;
  }

  this->field_->one(&(res->z));
  this->field_->one(&(res->z2));
  res->isInfinity = false;

  return ERR_OK;
}

template<class Basefield>
void BnCurve<Basefield>::get_trace(typename GFp::Element *trace) {
  gfp_->copy(trace, trace_);
}

ErrCode GFpBnCurve::compress(typename GFp::Element *x, int *y, const Point &P) {
  Point tmp;

  this->init(&tmp);
  this->copy(&tmp, P);
  this->normalize(&tmp);

  this->field_->copy(x, tmp.x);
  *y = static_cast<int>(tmp.y.val[0] & 0x01);
  this->zero(&tmp);

  return ERR_OK;
}

ErrCode GFpBnCurve::decompress(Point *P, const typename GFp::Element &x,
                               int y) {
  ErrCode rv;
  typename GFp::Element t1;

  // z = 1
  this->field_->one(&(P->z));
  this->field_->one(&(P->z2));

  this->field_->copy(&(P->x), x);

  this->field_->sqr(&t1, x);
  this->field_->mul(&t1, t1, x);  // x^3
  this->field_->add(&t1, t1, this->b_);
  rv = this->field_->sqrt(&(P->y), t1);

  if (static_cast<int>(P->y.val[0] & 0x01) != y) {
    this->field_->opp(&(P->y), P->y);
  }

  return rv;
}

ErrCode BnCurveFactory::getParameters(GFpBnCurve *gfp_curve,
                                      typename GFpBnCurve::Point *gfp_generator,
                                      Fp2BnCurve *fp2_curve,
                                      typename Fp2BnCurve::Point *fp2_generator,
                                      BnCurveDefinition curve_def) {
  ErrCode rv = ERR_OK;
  unsigned char buff[32];
  bn_curve_string_def *definition;

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

  rv = gfp_curve->initialize(definition->t, definition->b);
  if (rv != ERR_OK) {
    goto end;
  }
  rv = fp2_curve->initialize(definition->t, definition->b);
  if (rv != ERR_OK) {
    goto end;
  }

  std::memset(buff, 0, 32);
  buff[0] = 1;

  gfp_curve->hash(gfp_generator, buff, NULL, NULL);
  fp2_curve->hash(fp2_generator, buff, NULL, NULL);

  end: return rv;
}

template class BnCurve<GFp>;
template class BnCurve<Fp2>;

} /* namespace curve */
} /* namespace ecl */

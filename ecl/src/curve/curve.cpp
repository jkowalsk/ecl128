/*
 * @file curve.cpp
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

using std::string;

using ecl::curve::FpnCurve;

namespace ecl {
namespace curve {

typedef struct curve_definition {
  const char *prime;  ///< characteristic of the underlying field. \f$F_p\f$. In base 10
  const char *order;  ///< order of the curve. In base 10
  const char *a;    ///< factor of x in the Weierstrass equation. In base 10
  const char *b;    ///< constant in the Weierstrass equation. In base 16
  const char *Gx;  ///< x coordinate of the group generator on the curve. In base 16
  const char *Gy;  ///< x coordinate of the group generator on the curve. In base 16
} curve_string_def;

/** Definition of P256 */
static curve_string_def P256_str = {
    "ffffffff00000001000000000000000000000000ffffffffffffffffffffffff",
    "ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc632551", "-3",
    "5ac635d8aa3a93e7b3ebbd55769886bc651d06b0cc53b0f63bce3c3e27d2604b",
    "6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296",
    "4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5" };

static curve_string_def ANSSI_FRP256v1_str = {
    "F1FD178C0B3AD58F10126DE8CE42435B3961ADBCABC8CA6DE8FCF353D86E9C03",
    "F1FD178C0B3AD58F10126DE8CE42435B53DC67E140D2BF941FFDD459C6D655E1",
    "F1FD178C0B3AD58F10126DE8CE42435B3961ADBCABC8CA6DE8FCF353D86E9C00",
    "EE353FCA5428A9300D4ABA754A44C00FDFEC0C9AE4B1A1803075ED967B7BB73F",
    "B6B3D4C356C139EB31183D4749D423958C27D2DCAF98B70164C97A2DD98F5CFF",
    "6142E0F7C8B204911F9271F0F3ECEF8C2701C307E8E4C9E183115A1554062CFB" };

template<class BaseField>
FpnCurve<BaseField>::FpnCurve() {
  a_is_m3_ = false;
  a_is_0_ = false;
  field_ = NULL;
}

template<class BaseField>
FpnCurve<BaseField>::~FpnCurve() {
  if (field_ != NULL) {
    delete field_;
  }
}

template<class BaseField>
ErrCode FpnCurve<BaseField>::initialize(const string prime, const string order,
                                        const typename BaseField::Element &a,
                                        const typename BaseField::Element &b) {
  typename GFp::Element t;
  GFp *gfp;
  int sign;
  field_ = new BaseField(prime);

  gfp = field_->getBasePrimeField();

  order_.fromString(&sign, order);

  a_is_0_ = a_is_m3_ = false;

  field_->copy(&a_, a);
  field_->copy(&b_, b);

  if (field_->isZero(a_)) {
    a_is_0_ = true;
  }

  gfp->set(&t, 3);
  gfp->opp(&t, t);

  if (field_->cmp(this->a_, t) == 0) {
    a_is_m3_ = true;
  }

  return ERR_OK;
}

template<class BaseField>
void FpnCurve<BaseField>::init(Point *res) {
  field_->zero(&(res->x));
  field_->zero(&(res->y));
  field_->one(&(res->z));
  field_->one(&(res->z2));
  res->isInfinity = false;
}

template<class BaseField>
void FpnCurve<BaseField>::init(Point *res, const typename BaseField::Element &x,
                               const typename BaseField::Element &y) {
  field_->copy(&(res->x), x);
  field_->copy(&(res->y), y);
  field_->one(&(res->z));
  field_->one(&(res->z2));
  res->isInfinity = false;
}

template<class BaseField>
void FpnCurve<BaseField>::init(Point *res, const typename BaseField::Element &x,
                               const typename BaseField::Element &y,
                               const typename BaseField::Element &z) {
  field_->copy(&(res->x), x);
  field_->copy(&(res->y), y);
  field_->copy(&(res->z), z);
  field_->sqr(&(res->z2), z);
  res->isInfinity = false;
}

template<class BaseField>
void FpnCurve<BaseField>::zero(Point *res) {
  field_->zero(&(res->x));
  field_->zero(&(res->y));
  field_->zero(&(res->z));
  field_->zero(&(res->z2));
}

template<class BaseField>
void FpnCurve<BaseField>::setInfinity(Point *res) {
  field_->one(&(res->x));
  field_->one(&(res->y));
  field_->zero(&(res->z));
  field_->zero(&(res->z2));
  res->isInfinity = true;
}

template<class BaseField>
bool FpnCurve<BaseField>::isInfinity(const Point &P) {
  bool result;
  result = field_->isOne(P.x);
  result &= field_->isOne(P.y);
  result &= field_->isZero(P.z);

  return result;
}

template<class BaseField>
void FpnCurve<BaseField>::normalize(Point *res) {
  typename BaseField::Element t, invZ4;

  if (field_->isOne(res->z)) {
    return;
  }

  if (res->isInfinity || field_->isZero(res->z)) {
    return setInfinity(res);
  }

  // y = y/z³
  field_->sqr(&t, res->z2);
  field_->inv(&invZ4, t);
  field_->mul(&t, invZ4, res->z);
  field_->mul(&(res->y), t, res->y);
  // x = x/z²
  field_->mul(&t, t, res->z);
  field_->mul(&(res->x), t, res->x);

  field_->one(&(res->z));
  field_->one(&(res->z2));

  res->isInfinity = false;
}

template<class BaseField>
void FpnCurve<BaseField>::copy(Point *res, const Point &P) {
  field_->copy(&(res->x), P.x);
  field_->copy(&(res->y), P.y);
  field_->copy(&(res->z), P.z);
  field_->copy(&(res->z2), P.z2);
  res->isInfinity = P.isInfinity;
}

template<class BaseField>
int FpnCurve<BaseField>::cmp(const Point &Q, const Point &P) {
  int rv = 0;
  typename BaseField::Element left, right;

  field_->mul(&left, P.x, Q.z2);
  field_->mul(&right, Q.x, P.z2);
  rv |= field_->cmp(left, right);

  field_->mul(&left, P.y, Q.z2);
  field_->mul(&left, left, Q.z);
  field_->mul(&right, Q.y, P.z2);
  field_->mul(&right, right, P.z);
  rv |= field_->cmp(left, right);

  return rv;
}

template<class BaseField>
void FpnCurve<BaseField>::opp(Point *res, const Point &P) {
  field_->copy(&(res->x), P.x);
  field_->opp(&(res->y), P.y);
  field_->copy(&(res->z), P.z);
  field_->copy(&(res->z2), P.z2);
  res->isInfinity = P.isInfinity;
}

template<class BaseField>
void FpnCurve<BaseField>::add(Point *res, const Point &P, const Point &Q) {
  typename BaseField::Element A, B, C, D, E, F, AE2, E3;

  if (P.isInfinity) {
    return copy(res, Q);
  }
  if (Q.isInfinity) {
    return copy(res, P);
  }

  field_->mul(&A, P.x, Q.z2);
  field_->mul(&B, Q.x, P.z2);
  field_->mul(&C, P.y, Q.z2);
  field_->mul(&C, C, Q.z);
  field_->mul(&D, Q.y, P.z2);
  field_->mul(&D, D, P.z);

  // if P == Q or P == -Q
  if (field_->cmp(A, B) == 0) {
    if (field_->cmp(C, D) == 0) {
      // P == Q then double
      return dbl(res, P);
    } else {
      return setInfinity(res);
    }
  }

  field_->sub(&E, B, A);
  field_->sub(&F, D, C);

  field_->mul(&(res->z), P.z, Q.z);
  field_->mul(&(res->z), res->z, E);
  field_->sqr(&(res->z2), res->z);

  field_->sqr(&AE2, E);
  field_->mul(&E3, AE2, E);
  field_->mul(&AE2, AE2, A);

  field_->sqr(&(res->x), F);
  field_->sub(&(res->x), res->x, AE2);
  field_->sub(&(res->x), res->x, AE2);
  field_->sub(&(res->x), res->x, E3);

  field_->sub(&(res->y), AE2, res->x);
  field_->mul(&(res->y), res->y, F);
  field_->mul(&E3, C, E3);
  field_->sub(&(res->y), res->y, E3);
}

template<class BaseField>
void FpnCurve<BaseField>::dbl(Point *res, const Point &P) {
  typename BaseField::Element A, B, Y2, tmp;

  if (P.isInfinity) {
    return copy(res, P);
  }

  field_->sqr(&Y2, P.y);
  field_->mul(&A, Y2, P.x);
  field_->mul(&A, A, 4);

  if (a_is_m3_) {
    field_->add(&tmp, P.x, P.z2);
    field_->sub(&B, P.x, P.z2);
    field_->mul(&B, B, tmp);
    field_->mul(&B, B, 3);
  } else {
    field_->sqr(&B, P.x);
    field_->mul(&B, B, 3);
    if (!a_is_0_) {
      field_->sqr(&tmp, P.z2);
      field_->mul(&tmp, tmp, this->a_);
      field_->add(&B, B, tmp);
    }
  }

  field_->mul(&(res->z), P.y, P.z);
  field_->mul(&(res->z), res->z, 2);
  field_->sqr(&(res->z2), res->z);

  field_->sqr(&(res->x), B);
  field_->sub(&(res->x), res->x, A);
  field_->sub(&(res->x), res->x, A);

  field_->sub(&(res->y), A, res->x);
  field_->mul(&(res->y), B, res->y);
  field_->sqr(&tmp, Y2);
  field_->mul(&tmp, tmp, 8);
  field_->sub(&(res->y), res->y, tmp);
}

template<class BaseField>
void FpnCurve<BaseField>::mul_ML(Point *res, const Point &P,
                                 const typename GFp::Element &k) {
  int i, l, y;
  Point pp[2];

  init(&(pp[0]));
  init(&(pp[1]));

  copy(&(pp[0]), P);
  dbl(&(pp[1]), P);

  l = k.count_bits();

  for (i = l - 1; i > 0; i--) {
    y = k.get_bit(i - 1);
    add(&(pp[y ^ 1]), pp[0], pp[1]);
    dbl(&(pp[y]), pp[y]);
  }

  copy(res, pp[0]);

  zero(&(pp[0]));
  zero(&(pp[1]));
}

template<class BaseField>
ErrCode FpnCurve<BaseField>::mul_SW(Point *res, const Point &P,
                                    const typename GFp::Element &k,
                                    int window_sz) {
  Point *precomp = NULL;
  Point pp;
  Point &Q = *res;
  int i, u, s, precomp_sz;

  precomp_sz = (1 << window_sz) - 1;

  precomp = new Point[precomp_sz + 1];

  init(&pp);
  dbl(&pp, P);
  copy(&(precomp[1]), P);
  setInfinity(&Q);
  // precomputation of [3]P .. [2^k-1]P
  for (int j = 3; j <= precomp_sz; j += 2) {
    init(&(precomp[j]));
    add(&(precomp[j]), pp, precomp[j - 2]);
  }

  i = k.count_bits() - 1;
  while (i >= 0) {
    if (k.get_bit(i) == 0) {
      dbl(&Q, Q);
      i--;
    } else {
      s = i - window_sz + 1;
      if (s < 0)
        s = 0;
      while (k.get_bit(s) == 0)
        s++;

      for (int h = 0; h < i - s + 1; h++) {
        dbl(&Q, Q);
      }
      u = 0;
      for (int h = i; h >= s; h--) {
        u |= (1 & k.get_bit(h)) << (h - s);
      }
      add(&Q, Q, precomp[u]);
      i = s - 1;
    }
  }

  delete[] precomp;
  return ERR_OK;
}

template<class BaseField>
bool FpnCurve<BaseField>::isValid(const Point &P, bool verify_order) {
  bool is_valid;
  Point res;
  typename BaseField::Element left, right, t1, t2;

  field_->sqr(&left, P.y);

  field_->sqr(&t2, P.z2);
  field_->mul(&t2, t2, P.z2);
  field_->mul(&t2, t2, b_);

  field_->sqr(&t1, P.z2);
  field_->mul(&t1, t1, P.x);
  field_->mul(&t1, t1, a_);

  field_->sqr(&right, P.x);
  field_->mul(&right, right, P.x);

  field_->add(&right, right, t1);
  field_->add(&right, right, t2);

  is_valid = (field_->cmp(right, left) == 0);

  if (verify_order) {
    init(&res);
    mul(&res, P, order_);
    is_valid = is_valid && isInfinity(res);
  }

  return is_valid;
}

template<class BaseField>
void FpnCurve<BaseField>::get_order(typename GFp::Element *order) {
  GFp *gfp = field_->getBasePrimeField();
  gfp->copy(order, order_);
}

ErrCode GFpCurve::initialize(const string &prime, const string &order,
                             const string &a, const string &b) {
  GFp::Element t;
  int sign;

  this->field_ = new GFp(prime);

  this->order_.fromString(&sign, order);

  this->a_is_0_ = this->a_is_m3_ = false;

  this->field_->fromString(&this->a_, a);
  this->field_->fromString(&this->b_, b);

  if (this->a_.isZero()) {
    this->a_is_0_ = true;
  }

  if (this->field_->cmp(this->a_, -3) == 0) {
    this->a_is_m3_ = true;
  }

  return ERR_OK;
}

ErrCode GFpCurve::compress(GFp::Element *x, int *y, const Point &P) {
  Point tmp;

  this->init(&tmp);
  this->copy(&tmp, P);
  this->normalize(&tmp);

  x->copy(tmp.x);
  *y = static_cast<int>(tmp.y.val[0] & 0x01);
  this->zero(&tmp);

  return ERR_OK;
}

ErrCode GFpCurve::decompress(Point *P, const GFp::Element &x, int y) {
  ErrCode rv;
  GFp::Element t1;

  // z = 1
  this->field_->one(&(P->z));
  this->field_->one(&(P->z2));

  this->field_->copy(&(P->x), x);
  this->field_->mul(&t1, P->x, this->a_);
  this->field_->add(&t1, t1, this->b_);  // ax+b

  this->field_->sqr(&(P->y), P->x);
  this->field_->mul(&(P->y), P->y, P->x);  // x^3
  this->field_->add(&(P->y), P->y, t1);
  rv = this->field_->sqrt(&(P->y), P->y);

  if (static_cast<int>(P->y.val[0] & 0x01) != y) {
    this->field_->opp(&(P->y), P->y);
  }

  return rv;
}

ErrCode CurveFactory::getParameters(GFpCurve *curve, GFpCurve::Point *Generator,
                                    CurveDefinition curve_def) {
  curve_string_def *definition;
  GFp *gfp;
  GFp::Element x, y;

  switch (curve_def) {
    case NIST_P256:
      definition = &P256_str;
      break;
    case ANSSI_FRP256v1:
      definition = &ANSSI_FRP256v1_str;
      break;
    default:
      return ERR_INVALID_VALUE;
  }

  curve->initialize(definition->prime, definition->order, definition->a,
                    definition->b);
  gfp = curve->getField();
  gfp->fromString(&x, definition->Gx);
  gfp->fromString(&y, definition->Gy);
  curve->init(Generator, x, y);

  return ERR_OK;
}

template class FpnCurve<GFp> ;
template class FpnCurve<Fp2> ;

} /* namespace curve */
} /* namespace ecl */


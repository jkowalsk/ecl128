/*
 * Curve_test.cc
 *
 * Part of the ecl library.
 *
 * Copyright 2013 Julien Kowalski.
 *
 */

#include <gtest/gtest.h>

#include "config.h"
#include "rand.h"
#include "clock.h"

#include "ecl/field/GFp.h"
#include "ecl/curve/curve.h"

#include "GFp_curve_test_vectors.h"

using namespace ecl;
using namespace ecl::field;
using namespace ecl::curve;

#define CHECK_ORDER true

template<CurveDefinition definition>
class CurveWithDef {
 public:
  static CurveDefinition getDefinition() {
    return definition;
  }
};

template<class CurveDef>
class EccGFpBase : public testing::Test {
 public:
  EccGFpBase() {
    def = CurveDef::getDefinition();
    CurveFactory::getParameters(&curve, &P, def);
    gfp = curve.getField();
    switch (def) {
      case NIST_P256:
        vectors = p256_vectors;
        break;
      default:
        break;
    }
  }

  ~EccGFpBase() {

  }

  int getNbTest() {
    switch (def) {
      case NIST_P256:
        return sizeof(p256_vectors) / sizeof(ec_test_vector);
      default:
        return 0;
    }
  }

  void get_digit(ecl_digit *d, char c) {
    *d = 255;
    if (c >= 0x30 && c <= 0x39)
      *d = c - 0x30;
    if (c >= 0x41 && c <= 0x46)
      *d = c - 0x37;
    if (c >= 0x61 && c <= 0x66)
      *d = c - 0x57;
  }

  // import number in base 10...
  void dirtyimport(typename GFp::Element *res,
                   const char *str) {
    ecl_digit d;
    gfp->zero(res);
    get_digit(&d, str[0]);
    gfp->add_nr(res, *res, d);
    for (size_t i = 1; i < strlen(str); i++) {
      get_digit(&d, str[i]);
      gfp->mul(res, *res, 10);
      gfp->add_nr(res, *res, d);
    }
  }

  CurveDefinition def;
  GFpCurve curve;
  typename GFpCurve::Point P, ref, res, dP, tP;
  GFp *gfp;
  typename GFp::Element x, y, k, comp;
  ec_test_vector *vectors;
};

// First, define a test fixture class template.  Here we just reuse
// the FieldTest fixture defined earlier:
template<class CurveDef>
class EccGFp : public EccGFpBase<CurveDef> {
};

TYPED_TEST_CASE_P(EccGFp);

/** @ingroup Curve
 @defgroup CurveGFp Test suite for the Elliptic curve implementation on GFp.
 @addtogroup CurveGFp
 @{
 */

/** Test point double.
 This test performs the following operations:
 */
TYPED_TEST_P( EccGFp, Double){
ASSERT_TRUE( this->curve.isValid(this->P, CHECK_ORDER) );
ASSERT_TRUE( this->gfp != NULL );

this->curve.init(&this->dP);
this->curve.init(&this->ref);

/**  <li> double the point */
this->curve.dbl(&this->dP, this->P);

/** <li>  get the value of [2]P from test vectors */
ASSERT_EQ(ERR_OK, this->gfp->fromString(&this->x, this->vectors[1].x) );
ASSERT_EQ(ERR_OK, this->gfp->fromString(&this->y, this->vectors[1].y) );
this->curve.init(&this->ref, this->x, this->y );

/** <li> verify that the result is on the curve */
ASSERT_TRUE( this->curve.isValid(this->dP, CHECK_ORDER) );

/** <li>  verify that the calculated doubled point and the test vector are equal */
ASSERT_EQ(0, this->curve.cmp(this->ref, this->dP) );
/** </ul> */
}

/** Test point addition.
 This test performs the following operations:
 */
TYPED_TEST_P( EccGFp, Add){

ASSERT_TRUE( this->curve.isValid(this->P, CHECK_ORDER) );
ASSERT_TRUE( this->gfp != NULL );

this->curve.init(&this->dP);
this->curve.init(&this->tP);
this->curve.init(&this->ref);

/**  <li> double the point then add the point to the result (i.e. triple P)*/
this->curve.dbl(&this->dP, this->P);
this->curve.add(&this->tP, this->dP, this->P);

/** <li>  get the value of [3]P from test vectors */
ASSERT_EQ(ERR_OK, this->gfp->fromString(&this->x, this->vectors[2].x) );
ASSERT_EQ(ERR_OK, this->gfp->fromString(&this->y, this->vectors[2].y) );
this->curve.init(&this->ref, this->x, this->y );

/** <li> verify that the result is on the curve */
ASSERT_TRUE( this->curve.isValid(this->tP, CHECK_ORDER) );

/** <li>  verify that the calculated tripled point and the test vector are equal */
ASSERT_EQ(0, this->curve.cmp(this->ref, this->tP) );
/** </ul> */
}

/** Test point multiplication (Sliding window of size 4)
 */
TYPED_TEST_P( EccGFp, Mul ){
int vector_size = this->getNbTest();
ASSERT_TRUE( this->curve.isValid(this->P, CHECK_ORDER) );
ASSERT_TRUE( this->gfp != NULL );

this->curve.init(&this->res);
this->curve.init(&this->ref);

for(int j=0; j<vector_size; j++) {
  this->dirtyimport(&this->k, this->vectors[j].k);

  /** <li>  get the value of [k]P from test vectors */
  ASSERT_EQ(ERR_OK, this->gfp->fromString(&this->x, this->vectors[j].x) );
  ASSERT_EQ(ERR_OK, this->gfp->fromString(&this->y, this->vectors[j].y) );
  this->curve.init(&this->ref, this->x, this->y );

  /**  <li> calculate [k]P */
  ASSERT_EQ(ERR_OK, this->curve.mul(&this->res, this->P, this->k) );

  /** <li>  verify that the calculated point and the test vector are equal */
  ASSERT_EQ(0, this->curve.cmp(this->ref, this->res) );
}
/** </ul> */
}

/** Test point compression and decompression */
TYPED_TEST_P(EccGFp, Compression){
int vector_size = this->getNbTest();
int comp_y;

ASSERT_TRUE( this->curve.isValid(this->P, CHECK_ORDER) );
ASSERT_TRUE( this->gfp != NULL );

this->curve.init(&this->res);
this->curve.init(&this->ref);

for(int j=0; j<vector_size; j++) {
  this->dirtyimport(&this->k, this->vectors[j].k);

  /** <li>  get the value of [k]P from test vectors */
  ASSERT_EQ(ERR_OK, this->gfp->fromString(&this->x, this->vectors[j].x) );
  ASSERT_EQ(ERR_OK, this->gfp->fromString(&this->y, this->vectors[j].y) );
  this->curve.init(&this->ref, this->x, this->y );

  ASSERT_TRUE( this->curve.isValid(this->ref, CHECK_ORDER) );

  /**  <li> Compress [k]P */
  this->curve.compress(&this->comp, &comp_y, this->ref);

  /** <li>  Restore  [k]P */
  ASSERT_EQ(ERR_OK, this->curve.decompress(&this->res, this->comp, comp_y));

  /** <li>  Verify that the uncompressed point and original point are equal */
  ASSERT_EQ(0, this->curve.cmp(this->ref, this->res) );
}
/** </ul> */
}

/** Test point multiplication (Montgommery ladder)
 */
TYPED_TEST_P(EccGFp, Ladder){
int vector_size = this->getNbTest();

ASSERT_TRUE( this->curve.isValid(this->P, CHECK_ORDER) );
ASSERT_TRUE( this->gfp != NULL );

this->curve.init(&this->res);
this->curve.init(&this->ref);

for(int j=0; j<vector_size; j++) {
  this->dirtyimport(&this->k, this->vectors[j].k);

  /** <li>  get the value of [k]P from test vectors */
  ASSERT_EQ(ERR_OK, this->gfp->fromString(&this->x, this->vectors[j].x) );
  ASSERT_EQ(ERR_OK, this->gfp->fromString(&this->y, this->vectors[j].y) );
  this->curve.init(&this->ref, this->x, this->y );

  /**  <li> calculate [k]P */
  this->curve.mul_ML(&this->res, this->P, this->k);

  /** <li>  verify that the calculated point and the test vector are equal */
  ASSERT_EQ(0, this->curve.cmp(this->ref, this->res) );
}
/** </ul> */
}

TYPED_TEST_P(EccGFp, Performance){
uint64_t overhead;
int vector_size = this->getNbTest();

this->dirtyimport(&this->k, this->vectors[vector_size-1].k);

GET_OVERHEAD(overhead);
GET_PERF_CLOCKS("        double", this->curve.dbl(&this->dP, this->P), overhead);
GET_PERF_CLOCKS("      addition", this->curve.add(&this->tP, this->dP, this->P), overhead);
GET_PERF_CLOCKS("multiplication", this->curve.mul(&this->res, this->P, this->k), overhead);

GET_PERF("        double", this->curve.dbl(&this->dP, this->P));
GET_PERF("      addition", this->curve.add(&this->tP, this->dP, this->P));
GET_PERF("multiplication", this->curve.mul(&this->res, this->P, this->k));
}

// Type-parameterized tests involve one extra step: you have to
// enumerate the tests you defined:
REGISTER_TYPED_TEST_CASE_P(EccGFp,// The first argument is the test case name.
    // The rest of the arguments are the test names.
    Double, Add, Mul, Compression, Ladder, Performance);

/** Perform generic tests for NIST_P256 curve */
INSTANTIATE_TYPED_TEST_CASE_P(NIST_P256, EccGFp, CurveWithDef<NIST_P256>);
/**@}*/


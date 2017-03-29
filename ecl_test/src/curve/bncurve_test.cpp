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
#include "ecl/curve/bncurve.h"

using namespace ecl;
using namespace ecl::field;
using namespace ecl::curve;

#define CHECK_ORDER true

template<BnCurveDefinition definition>
class CurveWithDef {
 public:
  static BnCurveDefinition getDefinition() {
    return definition;
  }
};

template<class CurveDef>
class BnEccBase : public testing::Test {
 public:
  BnEccBase() {
    def = CurveDef::getDefinition();
    BnCurveFactory::getParameters(&gfp_curve, &gfp_gen,
                                                  &fp2_curve, &fp2_gen, def);
    gfp = gfp_curve.getField();
  }

  ~BnEccBase() {

  }

  BnCurveDefinition def;
  GFpBnCurve gfp_curve;
  GFpBnCurve::Point gfp_gen;
  GFpBnCurve::Point P, ref, res, dP, tP;
  Fp2BnCurve fp2_curve;
  Fp2BnCurve::Point fp2_gen;
  Fp2BnCurve::Point P2, ref2, res2, dP2, tP2;
  GFp *gfp;
  GFp::Element x, y, k, comp;
};

// First, define a test fixture class template.  Here we just reuse
// the FieldTest fixture defined earlier:
template<class CurveDef>
class BnEcc : public BnEccBase<CurveDef> {
};

TYPED_TEST_CASE_P(BnEcc);

/** @ingroup Curve
 @defgroup CurveGFp Test suite for the Elliptic curve implementation on GFp.
 @addtogroup CurveGFp
 @{
 */

/** Test point double and add consistency.
 This test performs the following operations:
 */
TYPED_TEST_P( BnEcc, DoubleAddGFp){
ASSERT_TRUE( this->gfp_curve.isValid(this->gfp_gen, CHECK_ORDER) );

this->gfp_curve.init(&this->dP);
this->gfp_curve.init(&this->tP);
this->gfp_curve.init(&this->ref);
this->gfp_curve.init(&this->res);

/**  <li> double twice the point to get [4]Generator */
this->gfp_curve.dbl(&this->dP, this->gfp_gen);
this->gfp_curve.dbl(&this->ref, this->dP);

/**  <li> add twice the generator to [2]Generator */
this->gfp_curve.add(&this->tP, this->dP, this->gfp_gen);
this->gfp_curve.add(&this->res, this->tP, this->gfp_gen);

/** <li> verify that the result is on the curve */
ASSERT_TRUE( this->gfp_curve.isValid(this->res, CHECK_ORDER) );

/** <li>  verify that the two calculated points are equal */
ASSERT_EQ(0, this->gfp_curve.cmp(this->ref, this->res) );
/** </ul> */
}

/** Test point double and add consistency.
 This test performs the following operations:
 */
TYPED_TEST_P( BnEcc, DoubleAddFp2){
ASSERT_TRUE( this->fp2_curve.isValid(this->fp2_gen, CHECK_ORDER) );

this->fp2_curve.init(&this->dP2);
this->fp2_curve.init(&this->tP2);
this->fp2_curve.init(&this->ref2);
this->fp2_curve.init(&this->res2);

/**  <li> double twice the point to get [4]Generator */
this->fp2_curve.dbl(&this->dP2, this->fp2_gen);
this->fp2_curve.dbl(&this->ref2, this->dP2);

/**  <li> add twice the generator to [2]Generator */
this->fp2_curve.add(&this->tP2, this->dP2, this->fp2_gen);
this->fp2_curve.add(&this->res2, this->tP2, this->fp2_gen);

/** <li> verify that the result is on the curve */
ASSERT_TRUE( this->fp2_curve.isValid(this->res2, CHECK_ORDER) );

/** <li>  verify that the two calculated points are equal */
ASSERT_EQ(0, this->fp2_curve.cmp(this->ref2, this->res2) );
/** </ul> */
}

/** Test point compression and decompression */
TYPED_TEST_P(BnEcc, Compression){
int comp_y;

this->gfp_curve.init(&this->res);
this->gfp_curve.init(&this->ref);

for(int i=0; i<NBTESTS; i++) {
  this->gfp->rand( &this->k, my_rand, NULL );
  //this->gfp->r_shift(&this->k, this->k, 4);
  this->gfp_curve.mul(&this->ref, this->gfp_gen, this->k);
  ASSERT_TRUE( this->gfp_curve.isValid(this->ref, CHECK_ORDER) );

  /**  <li> Compress [k]P */
  this->gfp_curve.compress(&this->comp, &comp_y, this->ref);

  /** <li>  Restore  [k]P */
  ASSERT_EQ(ERR_OK, this->gfp_curve.decompress(&this->res, this->comp, comp_y));
  ASSERT_TRUE( this->gfp_curve.isValid(this->res, CHECK_ORDER) );

  /** <li>  Verify that the decompressed point and original point are equal */
  ASSERT_EQ(0, this->gfp_curve.cmp(this->ref, this->res) );
}
}
/** </ul> */

TYPED_TEST_P(BnEcc, PerformanceGFp){
uint64_t overhead;

this->gfp->rand( &this->k, my_rand, NULL );

GET_OVERHEAD(overhead);
GET_PERF_CLOCKS("        double", this->gfp_curve.dbl(&this->dP, this->gfp_gen), overhead);
GET_PERF_CLOCKS("      addition", this->gfp_curve.add(&this->tP, this->dP, this->gfp_gen), overhead);
GET_PERF_CLOCKS("multiplication", this->gfp_curve.mul(&this->res, this->gfp_gen, this->k), overhead);

GET_PERF("        double", this->gfp_curve.dbl(&this->dP, this->gfp_gen));
GET_PERF("      addition", this->gfp_curve.add(&this->tP, this->dP, this->gfp_gen));
GET_PERF("multiplication", this->gfp_curve.mul(&this->res, this->gfp_gen, this->k));
}

TYPED_TEST_P(BnEcc, PerformanceFp2){
uint64_t overhead;

this->gfp->rand( &this->k, my_rand, NULL );

GET_OVERHEAD(overhead);
GET_PERF_CLOCKS("        double", this->fp2_curve.dbl(&this->dP2, this->fp2_gen), overhead);
GET_PERF_CLOCKS("      addition", this->fp2_curve.add(&this->tP2, this->dP2, this->fp2_gen), overhead);
GET_PERF_CLOCKS("multiplication", this->fp2_curve.mul(&this->res2, this->fp2_gen, this->k), overhead);

GET_PERF("        double", this->fp2_curve.dbl(&this->dP2, this->fp2_gen));
GET_PERF("      addition", this->fp2_curve.add(&this->tP2, this->dP2, this->fp2_gen));
GET_PERF("multiplication", this->fp2_curve.mul(&this->res2, this->fp2_gen, this->k));
}

// Type-parameterized tests involve one extra step: you have to
// enumerate the tests you defined:
REGISTER_TYPED_TEST_CASE_P(BnEcc,// The first argument is the test case name.
    // The rest of the arguments are the test names.
    DoubleAddGFp, DoubleAddFp2, Compression, PerformanceGFp, PerformanceFp2);

/** Perform generic tests for NIST_P256 curve */
INSTANTIATE_TYPED_TEST_CASE_P(NAERING_256, BnEcc, CurveWithDef<BN_NAERING_256>);
/** Perform generic tests for NIST_P256 curve */
INSTANTIATE_TYPED_TEST_CASE_P(BN_ARANHA_254, BnEcc, CurveWithDef<BN_ARANHA_254>);
/** Perform generic tests for NIST_P256 curve */
INSTANTIATE_TYPED_TEST_CASE_P(BN_BEUCHAT_254, BnEcc, CurveWithDef<BN_BEUCHAT_254>);
/**@}*/


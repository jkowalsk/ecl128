#include <gtest/gtest.h>

#include "config.h"
#include "rand.h"
#include "clock.h"

#include "ecl/field/GFp.h"
#include "ecl/curve/curve.h"
#include "ecl/curve/bncurve.h"
#include "ecl/curve/bnpairing.h"

using namespace ecl;
using namespace ecl::field;
using namespace ecl::curve;

#define CHECK_ORDER true

template<BnCurveDefinition def>
class CurveWithDef {
 public:
  static BnCurveDefinition getDefinition() {
    return def;
  }
};

template<class CurveDef>
class BnPairingBase : public testing::Test {
 public:
  BnPairingBase() {
    def = CurveDef::getDefinition();
    BNPairingFactory::getParameters(&ate, &P, &Q, def);
    gfp_curve = ate.getE1();
    fp2_curve = ate.getE2();
    gfp = gfp_curve->getField();
    fp12 = ate.getField();
  }

  ~BnPairingBase() {

  }

  BnCurveDefinition def;
  BNPairing ate;
  GFpBnCurve *gfp_curve;
  GFpBnCurve::Point P, PP, P2;
  Fp2BnCurve *fp2_curve;
  Fp2BnCurve::Point Q, QQ, Q2;
  GFp *gfp;
  Fp12 *fp12;
  GFp::Element x, y, k, comp;
  Fp12::Element res, res1, res2;
};

// First, define a test fixture class template.  Here we just reuse
// the FieldTest fixture defined earlier:
template<class CurveDef>
class BnPairingTest : public BnPairingBase<CurveDef> {
};

TYPED_TEST_CASE_P(BnPairingTest);

/** Verify bilinearity of the pairing.
 This test performs the following operation :
 */
TYPED_TEST_P(BnPairingTest, Bilinear){

ASSERT_TRUE(this->gfp_curve->isValid(this->P, true));
ASSERT_TRUE(this->fp2_curve->isValid(this->Q, true));

this->gfp_curve->init(&this->P2);
this->fp2_curve->init(&this->Q2);

/** - Get [2]P and [2]Q */
this->gfp_curve->dbl(&this->P2, this->P);
this->gfp_curve->copy(&this->PP, this->P2);
this->gfp_curve->normalize(&this->P2);
ASSERT_EQ(0, this->gfp_curve->cmp(this->PP, this->P2));

this->fp2_curve->dbl(&this->Q2, this->Q);
this->fp2_curve->copy(&this->QQ, this->Q2);
this->fp2_curve->normalize(&this->Q2);
ASSERT_EQ(0, this->fp2_curve->cmp(this->QQ, this->Q2));

/** - Compute f = pair(P, Q) */
this->ate.pair(&this->res, this->P, this->Q);

/** - Verify that f != 0 (non degenerate) */
this->fp12->zero(&this->res1);
ASSERT_TRUE(this->fp12->cmp(this->res, this->res1) != 0);

/** - Compute f2 = pair([2]P, Q) */
this->ate.pair(&this->res1, this->P2, this->Q);

/** - Verify that \f$ f2 = f^2 \f$ */
this->fp12->sqr(&this->res, this->res);
ASSERT_EQ(0, this->fp12->cmp(this->res, this->res1));

/** - Compute f3 = pair(P, [2]Q) */
this->ate.pair(&this->res2, this->P, this->Q2);
/** - Verify that f2 = f3 */
ASSERT_EQ(0, this->fp12->cmp(this->res2, this->res));

return;
}

/** Verify bilinearity of the pairing.
 This test performs the following operation :
 */
TYPED_TEST_P(BnPairingTest, Bilinear2){
GFp::Element k;
ASSERT_TRUE(this->gfp_curve->isValid(this->P, true));
ASSERT_TRUE(this->fp2_curve->isValid(this->Q, true));

this->gfp_curve->init(&this->P2);
this->fp2_curve->init(&this->Q2);

for(int i=0; i<NBTESTS; i++) {
  /** - Get random k < order */
  this->gfp->rand(&k, my_rand, NULL);
  k.val[NB_LIMBS-1] = 0;

  /** - Get [k]P and [k]Q */
  this->gfp_curve->mul(&this->P2, this->P, k);
  this->fp2_curve->mul(&this->Q2, this->Q, k);

  /** - Compute f = pair(P, Q) */
  this->ate.pair(&this->res, this->P, this->Q);

  /** - Verify that f != 0 (non degenerate) */
  this->fp12->zero(&this->res1);
  ASSERT_TRUE(this->fp12->cmp(this->res, this->res1) != 0);

  /** - Compute f2 = pair([k]P, Q) */
  this->ate.pair(&this->res1, this->P2, this->Q);

  /** - Verify that \f$ f2 = f^k \f$ */
  this->fp12->exp(&this->res, this->res, k);
  ASSERT_EQ(0, this->fp12->cmp(this->res, this->res1));

  /** - Compute f3 = pair(P, [k]Q) */
  this->ate.pair(&this->res2, this->P, this->Q2);

  /** - Verify that f2 = f3 */
  ASSERT_EQ(0, this->fp12->cmp(this->res2, this->res));
}
return;
}

TYPED_TEST_P(BnPairingTest, Performance){
GET_PERF("BN pairing", this->ate.pair(&this->res, this->P, this->Q));
return;
}

// Type-parameterized tests involve one extra step: you have to
// enumerate the tests you defined:
REGISTER_TYPED_TEST_CASE_P(BnPairingTest,// The first argument is the test case name.
    // The rest of the arguments are the test names.
    Bilinear, Bilinear2, Performance);

INSTANTIATE_TYPED_TEST_CASE_P(BEUCHAT_254, BnPairingTest,
                              CurveWithDef<BN_BEUCHAT_254>);
INSTANTIATE_TYPED_TEST_CASE_P(ARANHA_254, BnPairingTest,
                              CurveWithDef<BN_ARANHA_254>);
INSTANTIATE_TYPED_TEST_CASE_P(NAERING_256, BnPairingTest,
                              CurveWithDef<BN_NAERING_256>);

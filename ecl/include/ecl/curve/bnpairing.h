/*
 * @file bnpairing.h
 * @author Julien Kowalski
 */

#ifndef ECL_INCLUDE_ECL_PAIRING_BNPAIRING_H_
#define ECL_INCLUDE_ECL_PAIRING_BNPAIRING_H_

#include <cstring>
#include <string>

#include "ecl/config.h"
#include "ecl/errcode.h"

#include "ecl/field/GFp.h"
#include "ecl/field/Fp2.h"
#include "ecl/field/Fp6.h"
#include "ecl/field/Fp12.h"

#include "ecl/curve/bncurve.h"

using std::string;

using ecl::field::GFp;
using ecl::field::Fp2;
using ecl::field::Fp6;
using ecl::field::Fp12;

namespace ecl {
namespace curve {

/** Performs some pairing computation on BN curves.
 *
 */
class BNPairing {
 public:
  /** Basic contructor */
  BNPairing();

  /** Basic destructor */
  ~BNPairing();

  /** Initialize the BN curves for pairing.
   The curves E1 and its twist E2 are initialized using these parameters.
   @param t t parameter of the BN curve
   @param b b parameter of the BN curve equation (\f$ Y^2 = X^3 + b \f$)

   @return ERR_SUCCESS in case of success
   @return an error code otherwise.
   */
  ErrCode init(string t, string b);

  /** Compute the optimal ate pairing of P and Q
   @param res result
   @param P point on the elliptic curve
   @param Q point on the twisted curve

   @return ERR_SUCCESS in case of success
   @return an error code otherwise.
   */
  void pair(Fp12::Element *res, const GFpBnCurve::Point &P,
            const Fp2BnCurve::Point &Q);

  /** Get the curve defined over G1 (\f$ GF_{p} \f$)
   @return the curve defined over G1
   */
  GFpBnCurve *getE1() {
    return &E1;
  }

  /** Get the twisted curve defined over G2 (\f$ F_{p^{2}} \f$)
   @return the twisted curve defined over G2
   */
  Fp2BnCurve *getE2() {
    return &E2;
  }

  /** Get the \f$ F_{p^{12}} \f$ field of the result
   @return result base field
   */
  Fp12 *getField() {
    return fp12;
  }

  /** Return the order of the curves.
   * @return curbe order
   */
  GFp::Element *getOrder() {
    return &order_;
  }

 private:
  GFp::Element prime_;
  GFp::Element order_;
  GFp::Element trace_;
  int t_sign_;
  int t_naf_[256];
  int t_naf_sz_;

  int s_[256];
  int s_sz_;

  GFp *gfp;
  Fp2 *fp2;
  Fp6 *fp6;
  Fp12 *fp12;
  GFpBnCurve E1;
  Fp2BnCurve E2;

  void get_6tp2(GFp::Element *p, const GFp::Element &t, int sign);

  void exp_t(Fp12::Element *res, const Fp12::Element &f);
  void sqr_cycl(Fp12::Element *res, const Fp12::Element &f);

  void sqr_fp4(Fp2::Element *c0, Fp2::Element *c1, const Fp2::Element &a,
               const Fp2::Element &b);

  void mult_L(Fp12::Element *f, const Fp12::Element lpq);

  void mulFp2Fp2(Fp6::Element *z, const Fp6::Element& x, const Fp2::Element& ya,
                 const Fp2::Element& yb);

  void doubleAndEvalLine(Fp2BnCurve::Point *T, Fp12::Element *lqq,
                         const GFpBnCurve::Point &P,
                         const Fp2BnCurve::Point &Q);

  void addAndEvalLine(Fp2BnCurve::Point *T, Fp12::Element *lqq,
                      const GFpBnCurve::Point &P, const Fp2BnCurve::Point &Q,
                      const Fp2BnCurve::Point &R);

  void millerLoop(Fp12::Element *f, const GFpBnCurve::Point &P,
                  const Fp2BnCurve::Point &Q);

  void finalExp(Fp12::Element *res, const Fp12::Element &f);
  void finalExpNew(Fp12::Element *res, const Fp12::Element &f);
};

/** BN pairing Factory : to get standard parameters to computes pairings over BN curves.
 */
class BNPairingFactory {
 public:
  /** Helper to simply generate BNpairing and associated curves generators.
   * @param[out] ate object allowing to compute pairings
   * @param[out] gfp_generator Elliptic curve over GFp generator
   * @param[out] fp2_generator Elliptic curve over Fp2 generator
   * @param[in] curve_def curve definition
   * @return ERR_OK in case of success
   * @return ERR_INVALID_VALUE otherwise. Check that the definition is consistent with bitsize template parameter !
   */
  static ErrCode getParameters(BNPairing *ate, GFpBnCurve::Point *gfp_generator,
                               Fp2BnCurve::Point *fp2_generator,
                               BnCurveDefinition curve_def);

};

} /* namespace curve */
} /* namespace ecl */

#endif /* ECL_INCLUDE_ECL_PAIRING_BNPAIRING_H_ */

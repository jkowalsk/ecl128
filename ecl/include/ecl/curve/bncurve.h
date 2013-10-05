/*
 * @file bncurve.h
 * @author Julien Kowalski
 */

#ifndef ECL_INCLUDE_ECL_BNCURVE_CURVE_H_
#define ECL_INCLUDE_ECL_BNCURVE_CURVE_H_

#include <cstring>
#include <string>

#include "ecl/config.h"
#include "ecl/errcode.h"

#include "ecl/field/GFp.h"
#include "ecl/field/Fp2.h"
#include "ecl/field/Fp6.h"
#include "ecl/field/Fp12.h"

#include "ecl/curve/curve.h"


using std::string;
using ecl::field::GFp;
using ecl::field::Fp2;

namespace ecl {
namespace curve {

/** Enumerates defined BN curves.
 */
enum BnCurveDefinition {
  BN_BEUCHAT_254,
  BN_ARANHA_254,
  BN_NAERING_256,
};

/** Defines BN curve.
 * This is a standard elliptic curve with paramters as defined by Barretto and Naehrig.
 */
template<class BaseField>
class BnCurve : public FpnCurve<BaseField> {
 public:
  /** Curve point definition */
  typedef typename FpnCurve<BaseField>::Point Point;

  /** Basic contructor */
  BnCurve()
      : FpnCurve<BaseField>() {
    gfp_ = NULL;
    t_sign_ = 0;
  }

  /** Basic destructor */
  virtual ~BnCurve() {
  }

  /** Initializes a BN curve.
   * This curve has equation \f$ y^2 = x^3 + b \f$.
   * It is defined over field of characteristic \f$ p = 36.t^4 + 36.t^3 + 24.t^2 + 6t + 1 \f$
   *
   * Curves have order \f$ r = 36.t^4 + 36.t^3 + 18.t^2 + 6t + 1 \f$
   * @param t t in hex format
   * @param b b in hex format
   */
  virtual ErrCode initialize(string t, string b) = 0;

  /** Haches a value to a point on the curve.
   * @param[out] res resulting point
   * @param[in] buff buff_size_ bytes buffer to raise on the curve
   * @param f_rng random generator function for operation masking
   * @param p_rng random generator internal state
   */
  void hash(Point *res, unsigned char buff[32],
            int (*f_rng)(unsigned char *, int, void *), void *p_rng);

  /** Return the trace of the BN curve.
   *  @param[out] trace \f$ trace = 6.t^2+1 \f$
   */
  void get_trace(typename GFp::Element *trace);

  /** Raises a point to a frobenius power.
   * @param[out] res result \f$ res = [p^i]P \f$ where p is the field characteristic
   * @param[in] P curve point
   * @param[in] i power of the frobenius
   *
   * @return ERR_OK in case of success
   * @return ERR_NOT_IMPLEMENTED if this power is not implemented.
   */
  virtual ErrCode frobenius(Point *res, const Point &P, int i) = 0;

 protected:
  /** Base GFp field */
  GFp *gfp_;
  /** Set the prime characteristic of the base field */
  void set_prime();
  /** Sets the order of the curve */
  void set_order();
  /** Sets the trace of the curve */
  void set_trace();
  /** Sets b in the weierstrass equation */
  void set_b(string b);

  int t_sign_; //!< sign of t
  GFp::Element t_;     //!< number t used to generate other parameters
  GFp::Element prime_; //!< prime characteristic of the base field
  GFp::Element trace_; //!< trace of the curve
  GFp::Element twist_cofactor_; //!< cofactor of the curve's twist
  GFp::Element sqrt_m3_; //!< sqrt "-3"
};

/** BN curve defined over GFp
 *
 */
class GFpBnCurve : public BnCurve<GFp> {
 public:
  /** Curve point definition */
  typedef BnCurve<GFp>::Point Point;

  GFpBnCurve()
      : BnCurve<GFp>() {
  }
  ~GFpBnCurve() {
  }

  ErrCode initialize(string t, string b);
  ErrCode frobenius(Point *res, const Point &P, int i);

  /** Exports a point to its compressed form.
   @param[out] x will contain the compressed form of the point
   @param[out] y 0 or 1 (lsb of P.y)
   @param[in]  P point to compress
   @return ERR_SUCCESS if success
   @return an error code otherwise
   */
  ErrCode compress(GFp::Element *x, int *y, const Point &P);

  /** Imports a point from its compressed form.
   @param[out] P decompressed point
   @param[in]  x which contains the compressed form of the point abscissa
   @param[in]  y 0 or 1 (lsb of P.y)

   @return ERR_SUCCESS if success
   @return an error code otherwise
   */
  ErrCode decompress(Point *P, const GFp::Element &x, int y);
};

/** BN curve defined over Fp2
 *
 */
class Fp2BnCurve : public BnCurve<Fp2> {
 public:
  /** Curve point definition */
  typedef BnCurve<Fp2>::Point Point;

  Fp2BnCurve()
      : BnCurve<Fp2>() {
  }
  ~Fp2BnCurve() {
  }

  ErrCode initialize(string t, string b);
  ErrCode frobenius(Point *res, const Point &P, int i);

 private:
  /** Get the constants for computing the frobenius map. */
  void calc_frb_cst();
  /** Constants used to compute the Frobenius map in higher extensions.
     */
  Fp2::Element const_frb[5];
  /** Constants used to compute consecutive Frobenius maps in higher extensions.
   */
  Fp2::Element const_sqr[3];
  /** Constants used to compute the Frobenius map in higher extensions.
   */
  Fp2::Element const_cub[5];
};

/** BN Curve definitions
 */
typedef struct bn_curve_definition {
  string t;    ///< t parameter of the BN curve (non null bit table)
  string b;   ///< constant in the Weierstrass equation. In base 16
  int size;   ///< bit size of the curbe
} bn_curve_string_def;

static bn_curve_string_def beuchat_254_curve = { "3FC0100000000000", "5", 254 };
static bn_curve_string_def aranha_254_curve = { "-4080000000000001", "2", 254 };
static bn_curve_string_def naering_256_curve = { "-600000000000219B", "3", 256 };

/** BN Curve Factory : to get standard BN curve parameters.
 */
class BnCurveFactory {
 public:
  /** Helper to simply generate curves over GFp, Fp2 and generators.
   * @param[out] gfp_curve Elliptic curve over GFp
   * @param[out] gfp_generator Elliptic curve over GFp generator
   * @param[out] fp2_curve Elliptic curve over Fp2
   * @param[out] fp2_generator Elliptic curve over Fp2 generator
   * @param[in] curve_def curve definition
   * @return ERR_OK in case of success
   * @return ERR_INVALID_VALUE otherwise. Check that the definition is consistent with bitsize template parameter !
   */
  static ErrCode getParameters(GFpBnCurve *gfp_curve,
                               typename GFpBnCurve::Point *gfp_generator,
                               Fp2BnCurve *fp2_curve,
                               typename Fp2BnCurve::Point *fp2_generator,
                               BnCurveDefinition curve_def);

};

} /* namespace curve */
} /* namespace ecl */

#endif  // ECL_INCLUDE_ECL_BNCURVE_CURVE_H_

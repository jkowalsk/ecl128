/*
 * @file curve.h
 * @author Julien Kowalski
 */

#ifndef ECL_INCLUDE_ECL_CURVE_CURVE_H_
#define ECL_INCLUDE_ECL_CURVE_CURVE_H_

#include <cstring>
#include <string>

#include "ecl/config.h"
#include "ecl/errcode.h"

#include "ecl/field/GFp.h"
#include "ecl/field/Fp2.h"
#include "ecl/field/Fp6.h"
#include "ecl/field/Fp12.h"

using std::string;
using ecl::field::GFp;
using ecl::field::Fp2;

namespace ecl {
/**  Elliptic curve module.
 This module implements Elliptic curve operation.
 */
namespace curve {

/** Enumerates defined curves.
 */
enum CurveDefinition {
  NIST_P256,  //!< NIST P256 curve
  ANSSI_FRP256v1  //!< ANSSI FRP256v1 curve \cite jo:FRP256v1 as french standard curve
};


/** Template class for elliptic curve operations.
 */
template<class BaseField>
class FpnCurve {
 public:
  /**
   \brief Point definition.
   A point on an elliptic curve is defined by a triplet [x, y, z] which represents
   the Chudnovsky Jacobian coordinates of the point.
   */
  struct Point {
    typename BaseField::Element x;  //!< x coordinate
    typename BaseField::Element y;  //!< y coordinate
    typename BaseField::Element z;  //!< z coordinate
    typename BaseField::Element z2;  //!< \f$ z^2 \f$ stored for Chudnovsky Jacobian coordinates
    bool isInfinity;  //!< is it the point at infinity
  } ;

  /** Base constructor.
   */
  FpnCurve();

  /** Destructor.
   */
  virtual ~FpnCurve();

  /** Gets the base field of the curve.
   @return a pointer to the base field of the curve.
   */
  BaseField *getField() {
    return field_;
  }

  /** Initialize relying field and curve parameters.
   * The base finite field is an extension of GFp.
   The equation of the curve is in the form \f$ Y^2 = X^3 + aX + b \f$
   @param prime characteristic of the base field
   @param order order of the curve
   @param a a parameter of the curve
   @param b b parameter of the curve
   */
  ErrCode initialize(const string prime, const string order,
                     const typename BaseField::Element &a,
                     const typename BaseField::Element &b);

  /** Initializes a point before usage.
   @note that all coordinates are 0, then the point is not on the curve.
   @param res Point to initialize
   */
  void init(Point *res);

  /** Initializes a point on the curve.
   Coordinates are given in affine form.
   @note there is no verification about the validity of the input
   @param res Point to initialize
   @param x x coordinate
   @param y y coordinate

   @return ERR_SUCCESS in case of success
   @return an error code otherwise.
   */
  void init(Point *res, const typename BaseField::Element &x,
            const typename BaseField::Element &y);

  /** Initializes a point on the curve.
   Coordinates are given in jacobian form.
   @note there is no verification about the validity of the input
   @param res Point to initialize
   @param x x coordinate
   @param y y coordinate
   @param z z coordinate

   @return ERR_SUCCESS in case of success
   @return an error code otherwise.
   */
  void init(Point *res, const typename BaseField::Element &x,
            const typename BaseField::Element &y,
            const typename BaseField::Element &z);

  /** Zeroise a point.
   @param res Point to erase
   @return ERR_SUCCESS in case of success
   @return an error code otherwise.
   */
  void zero(Point *res);

  /** Normalize a point.
   If in jacobian coordinates gives the representation with z=1.
   @param res Point to normalize
   @return ERR_SUCCESS in case of success
   @return an error code otherwise.
   */
  void normalize(Point *res);

  /** Sets a point to the point at infinity.
   @param res Point to set

   @return ERR_SUCCESS in case of success
   @return an error code otherwise.
   */
  void setInfinity(Point *res);

  /** Checks wether the point is the point at infinity.
   * This function performs a full check, i.e.
   * - (P->Rx == 1) && (P->Ry == 1) && (P->Rz == 0)
   * - P->is_infinity == 1
   *
   * \param[in] P point to check
   *
   * \return true if the point is the point at infinity
   * \return false otherwise
   */
  bool isInfinity(const Point &P);

  /** Copy a point into another
   * \param[out] res destination
   * \param[in]  P source
   *
   * \return 0
   */
  void copy(Point *res, const Point &P);

  /** Compare two points
   * \param[in] Q first point
   * \param[in] P second point
   *
   * \return 0 if point are equal
   * \return 1 otherwise
   */
  int cmp(const Point &Q, const Point &P);

  /** Computes res = -P
   * \param[out] res result point
   * \param[in]  P first parameter

   */
  void opp(Point *res, const Point &P);

  /** Computes res = P + Q
   * \param[out] res result point
   * \param[in]  P first parameter
   * \param[in]  Q second parameter

   */
  void add(Point *res, const Point &Q, const Point &P);

  /** Computes res = [2]P
   * \param[out] res result point
   * \param[in]  P point to double
   */
  void dbl(Point *res, const Point &P);

  /** Computes res = [k]P using sliding window.
   * \param[out] res result point
   * \param[in]  P point to multiply
   * \param[in]  k field member
   */
  ErrCode mul(Point *res, const Point &P, const GFp::Element &k) {
    return mul_SW(res, P, k, 4);
  }

  /** Computes res = [k]P using Montgommery ladder.
   * \param[out] res result point
   * \param[in]  P point to multiply
   * \param[in]  k field member
   */
  void mul_ML(Point *res, const Point &P, const GFp::Element &k);

  /** Computes res = [k]P using sliding window.
   * \param[out] res result point
   * \param[in]  P point to multiply
   * \param[in]  k field member
   * \param[in]  window_sz window size
   */
  ErrCode mul_SW(Point *res, const Point &P, const GFp::Element &k,
                 int window_sz);

  /** Checks if coordinates of P are coherent with its curve.
   * i.e. if the point is on the curve
   *
   * This test checks that coordinates verifies the equation.
   *
   * \note For point validation of ECDH distant public key, one shall also
   * - verify that the point is not \f$ P_\infty \f$
   * - verify the order of the point (skipable for NIST curves as h = 1)
   *
   * \param[in] P point to check
   * \param[in] verify_order perform point order verification
   *
   * \return true if the point is on the curve
   * \return false if the point is not on the curve
   */
  bool isValid(const Point &P, bool verify_order);

  /** Return the order of the curve.
   * @param[out] order order of the curve
   */
  void get_order(GFp::Element *order);

 protected:
  /**  base field of the curve */
  BaseField *field_;

  /** Whether the parameter of the curve a == -3 mod p*/
  bool a_is_m3_;
  /** Whether the parameter of the curve a == 0 mod p*/
  bool a_is_0_;

  /** parameter a of the curve (weierstrass equation) */
  typename BaseField::Element a_;
  /** parameter b of the curve (weierstrass equation) */
  typename BaseField::Element b_;
  /**  order of the curve */
  GFp::Element order_;
};

/** Elliptic curve defined over GFp
 *
 */
class GFpCurve : public FpnCurve<GFp> {
 public:
  /** Curve point definition */
  typedef typename FpnCurve<GFp>::Point Point;

  GFpCurve()
      : FpnCurve<GFp>() {
  }

  ~GFpCurve() {
  }

  /** Initialize relying field and curve parameters.
   * The base finite field is an extension of GFp.
   The equation of the curve is in the form \f$ Y^2 = X^3 + aX + b \f$
   @param prime characteristic of the base field
   @param order order of the curve
   @param a a parameter of the curve
   @param b b parameter of the curve
   */
  ErrCode initialize(const string &prime, const string &order, const string &a,
                     const string &b);

  /** Exports a point to its compressed form.
   @param[out] x will contain the compressed form of the point
   @param[out] y 0 or 1 (lsb of P.y)
   @param[in]  P point to compress
   @return ERR_SUCCESS if success
   @return an error code otherwise
   */
  ErrCode compress(typename GFp::Element *x, int *y, const Point &P);

  /** Imports a point from its compressed form.
   @param[out] P decompressed point
   @param[in]  x which contains the compressed form of the point abscissa
   @param[in]  y 0 or 1 (lsb of P.y)

   @return ERR_SUCCESS if success
   @return an error code otherwise
   */
  ErrCode decompress(Point *P, const typename GFp::Element &x, int y);

};

/** Elliptic curve defined over Fp2
 *
 */
class Fp2Curve : public FpnCurve<Fp2> {
 public:
  /** Curve point definition */
  typedef typename FpnCurve<Fp2>::Point Point;

  Fp2Curve()
      : FpnCurve<Fp2>() {
  }

  ~Fp2Curve() {
  }
};

/** Curve Factory : to get standard curve parameters.
 */
class CurveFactory {
 public:
  /** Helper to simply generate curve and generator.
   * @param[out] curve Elliptic curve
   * @param[out] Generator Elliptic curve generator
   * @param[in] curve_def curve definition
   * @return ERR_OK in case of success
   * @return ERR_INVALID_VALUE otherwise. Check that the definition is consistent with bitsize template parameter !
   */
  static ErrCode getParameters(GFpCurve *curve,
                               GFpCurve::Point *Generator,
                               CurveDefinition curve_def);
};

} /* namespace curve */
} /* namespace ecl */

#endif  // ECL_INCLUDE_ECL_CURVE_CURVE_H_

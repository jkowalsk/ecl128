/*
 * @file Fp6.h
 * @author Julien Kowalski
 */

#ifndef ECL_FIELD_FP6_H_
#define ECL_FIELD_FP6_H_

#include <cstring>
#include <string>

#include "ecl/config.h"
#include "ecl/errcode.h"
#include "ecl/field/GFp.h"
#include "ecl/field/Fp2.h"

using std::string;

namespace ecl {
namespace field {

/** Degre 6 extension of a finite field over a prime p.
 */
class Fp6 {
 public:
  /** Field element */
  typedef typename Fp2::Element Element[3];
  /**  Field double (unreduced result of a multiplication) */
  typedef typename Fp2::Double Double[3];

  /** Constructor.
   * Sets the characteristic of the field
   * @note: there is no primality check on p !
   * @param[in] p characteristic in base 16
   */
  Fp6(const string p);

  /** Constructor.
   * Sets the characteristic of the field
   * @note: there is no primality check on p !
   * @param[in] p characteristic
   */
  Fp6(const typename GFp::Element &p);

  /** Destructor
   */
  ~Fp6();

  /** Sets an element to 0.
   * @param[out] res Element
   */
  void zero(Element *res);

  /** Sets an element to 1.
   * @param[out] res Element
   */
  void one(Element *res);

  /** Sets an element to integer value.
   * @param[out] res Element
   * @param[in]  v value to set
   */
  void set(Element *res, const int v);

  /** Copies a
   * @param[out] res result = a
   * @param[in] a operand
   */
  void copy(Element *res, const Element &a);

  /** Copies a
   * @param[out] res result = a
   * @param[in] a operand
   */
  void copy(Double *res, const Double &a);

  /** Tells wether a is equal to one or not.
   * @param[in] a Element
   */
  bool isOne(const Element &a);

  /** Tells wether a is equal to one or not.
   * @param[in] a Element
   */
  bool isZero(const Element &a);

  /** Performs res = a + b.
   * @param[out] res result
   * @param[in] a operand 1
   * @param[in] b operand 2
   */
  void add(Element *res, const Element &a, const Element &b);

  /** Performs res = a + b.
   * @param[out] res result
   * @param[in] a operand 1
   * @param[in] b operand 2
   */
  void add(Element *res, const Element &a, const typename GFp::Element &b);

  /** Performs res = a + b.
   * @param[out] res result
   * @param[in] a operand 1
   * @param[in] b operand 2
   */
  void add(Element *res, const Element &a, const ecl_digit b);

  /** Performs res = a + b.
   * @param[out] res result
   * @param[in] a operand 1
   * @param[in] b operand 2
   */
  void add(Double *res, const Double &a, const Double &b);

  /** Performs res = a - b.
   * @param[out] res result
   * @param[in] a operand 1
   * @param[in] b operand 2
   */
  void sub(Element *res, const Element &a, const ecl_digit b);

  /** Performs res = a - b.
   * @param[out] res result
   * @param[in] a operand 1
   * @param[in] b operand 2
   */
  void sub(Element *res, const Element &a, const Element &b);

  /** Performs res = a - b.
   * @param[out] res result
   * @param[in] a operand 1
   * @param[in] b operand 2
   */
  void sub(Double *res, const Double &a, const Double &b);

  /** Performs res = - a .
   * @param[out] res result
   * @param[in] a operand 1
   */
  void opp(Element *res, const Element &a);

  /** Performs res = - a .
   * @param[out] res result
   * @param[in] a operand 1
   */
  void opp(Double *res, const Double &a);

  /** Performs Montgomery reduction of a.
   * @param[out] res result = a/R
   * @param[in] a operand
   */
  void reduce(Element *res, const Double &a);

  /** Performs res = a * b.
   * @param[out] res result
   * @param[in] a operand 1
   * @param[in] b operand 2
   */
  void mul(Element *res, const Element &a, const Element &b);

  /** Performs res = a * b.
   * @param[out] res result
   * @param[in] a operand 1
   * @param[in] b operand 2
   */
  void mul(Element *res, const Element &a, const ecl_digit b);

  /** Performs res = a * b.
   * @param[out] res result
   * @param[in] a operand 1
   * @param[in] b operand 2
   */
  void mul(Double *res, const Element &a, const Element &b);

  /** Performs res = a * b.
   * @param[out] res result
   * @param[in] a operand 1
   * @param[in] b operand 2
   */
  void mul(Element *res, const Element &a, const typename Fp2::Element &b);

  /** Performs res = a * b.
   * @param[out] res result
   * @param[in] a operand 1
   * @param[in] b operand 2
   */
  void mul(Double *res, const Double &a, const ecl_digit b);

  /** Performs res = a * b.
   * @param[out] res result
   * @param[in] a operand 1
   * @param[in] b operand 2
   */
  void mul(Element *res, const Element &a, const typename GFp::Element &b);

  /** Performs res = a^2.
   * @param[out] res result
   * @param[in] a operand 1
   */
  void sqr(Element *res, const Element &a);

  /** Performs res = a^2.
   * @param[out] res result
   * @param[in] a operand 1
   */
  void sqr(Double *res, const Element &a);

  /** Performs res = 1/a.
   * @param[out] res result
   * @param[in] a operand 1
   */
  void inv(Element *res, const Element &a);

  /** Performs res = a / b.
   * @param[out] res result
   * @param[in] a operand 1
   * @param[in] b operand 2
   */
  void div(Element *res, const Element &a, const Element &b);

  /** Performs res = a^e.
   * @param[out] res result
   * @param[in] a operand 1
   * @param[in] e operand 2
   */
  void exp(Element *res, const Element &a, const typename GFp::Element &e);

  /** Performs res = a^(p^i) where p is the field characteristic.
   * @param[out] res result
   * @param[in] a operand 1
   * @param[in] i operand 2
   */
  ErrCode frobenius(Element *res, const Element &a, int i);

  /** Get field extension degre.
   * @return field extension degre
   */
  static int getExtensionDegre() {
    return 6;
  }
  ;

  /** Initialize Fp6 element with values.
   * res = a0 + X.a1 + X^2.a2
   * @param[out] res result a0 + i.a1
   * @param[in] a0 Fp2 element
   * @param[in] a1 Fp2 element
   * @param[in] a2 Fp2 element
   */
  void init(Element *res, const typename Fp2::Element &a0,
            const typename Fp2::Element &a1, const typename Fp2::Element &a2);

  /** Compares a and b
   * @param[in] a Fp2 Element
   * @param[in] b Fp2 Element
   * @return  1 if a != b
   * @return  0 if a == b
   */
  int cmp(const Element &a, const Element &b);

  /** Compares a and b
   * @param[in] a Fp2 Element
   * @param[in] b GFp Element
   * @return  1 if a != b
   * @return  0 if a == b
   */
  int cmp(const Element &a, const typename GFp::Element &b);

  /** Compares a and b
   * @param[in] a Fp2 Double
   * @param[in] b Fp2 Double
   * @return  1 if a != b
   * @return  0 if a == b
   */
  int cmp(const Double &a, const Double &b);

  /** Compute conjugate of a = x + iy
   * @param[out] res result = x-iy
   * @param[in] a operand
   */
  void conj(Element *res, const Element &a);

  /** Multiplies an Element with xsi.
   * Performs \f$res = a * \xi\f$.
   * @param res resulting a*ksi
   * @param a field element
   */
  void mul_vi(Element *res, const Element &a);

  /** Multiplies a Double with ksi.
   * Performs \f$res = a * \xi\f$.
   * @param res resulting a*ksi
   * @param a field Double
   */
  void mul_vi(Double *res, const Double &a);

  /** Get the base field.
   * This field is the prime field ( GFp ).
   */
  GFp *getBasePrimeField() {
    return gfp;
  }

  /** Get the base field.
   * This field is the degre 2 extension field ( Fp2 ).
   */
  Fp2 *getBaseField() {
    return fp2;
  }

  /** Get the characteristic of the field field.
   * @param[out] a field characteristic
   */
  void get_characteristic(typename GFp::Element *a) {
    gfp->get_characteristic(a);
  }
  ;

  /** Initializes an Element from a random function.
   * @param[out] res a random element
   * @param[in] f_rng random generation function
   * @param[in] p_rng random generation function internal state
   */
  void rand(Element *res, int (*f_rng)(unsigned char *, int, void *),
            void *p_rng);

 private:
  GFp *gfp;  //<! GFp base field
  Fp2 *fp2;  //<! Fp2 base field
};

}  // namespace field
}  // namespace ecl

#endif /* ECL_FIELD_FP6_H_ */

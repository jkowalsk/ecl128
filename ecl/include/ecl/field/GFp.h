/**
 * @file GFp.h
 *
 * Part of ecl.
 *
 * Copyright 2013 Julien Kowalski.
 *
 */

#ifndef ECL_FIELD_GFP_H_
#define ECL_FIELD_GFP_H_

#include <cstring>
#include <string>

#include "ecl/config.h"
#include "ecl/errcode.h"
#include "ecl/bigint/FixedSizedInt.h"

using std::string;

namespace ecl {
/** Namespace for field related classes.
 * So called "Variables" section of this documentation shows
 * instantiated classes which may be used.
 */
namespace field {

/** Implementation of prime field of degre 1.
 */
class GFp {
 public:
  /** Field element */
  typedef FixedSizedInt<NB_LIMBS> Element;
  /** Field double (unreduced result of a multiplication) */
  typedef FixedSizedInt<2 * NB_LIMBS> Double;

  /** Constructor.
   * Sets the characteristic of the field
   * @note: there is no primality check on p !
   * @param[in] p characteristic in base 16
   */
  GFp(const string p);

  /** Constructor.
   * Sets the characteristic of the field
   * @note: there is no primality check on p !
   * @param[in] p characteristic
   */
  GFp(const Element &p);

  /** Destructor
   */
  ~GFp();

  /** Reads element from radix 16 representation
   * @param[out] res resulting element
   * @param[in] str string representation
   *
   * @return ERR_INVALID_VALUE if string representation is too long
   */
  ErrCode fromString(Element *res, const string str);

  /** Sets an element to 0.
   * @param[out] a an Element
   */
  void zero(Element *a);

  /** Sets an element to 1.
   * @param[out] a an Element
   */
  void one(Element *a);

  /** Sets an element to integer value.
   * @param[out] a Element
   * @param[in]  v value to set
   */
  void set(Element *a, const int v);

  /** Get random element.
   * @param res resulting point
   * @param f_rng random generation function
   * @param p_rng RNG internal state
   *
   */
  void rand(Element *res,int (*f_rng)(unsigned char *, int, void *), void *p_rng) ;

  /** Copies a
   * @param[out] res result = a
   * @param[in] a operand
   */
  void copy(Element *res, const Element &a);

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

  /** Performs res = a + b without reduction.
   * @param[out] res result
   * @param[in] a operand 1
   * @param[in] b operand 2
   */
  void add_nr(Element *res, const Element &a, const Element b);

  /** Performs res = a + b.
   * @param[out] res result
   * @param[in] a operand 1
   * @param[in] b operand 2
   */
  void add(Element *res, const Element &a, const ecl_digit b);

  /** Performs res = a + b without reduction.
   * @param[out] res result
   * @param[in] a operand 1
   * @param[in] b operand 2
   */
  void add_nr(Element *res, const Element &a, const ecl_digit b);

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

  /** Performs res = a - b without reduction.
   * @note: result may be negative... use with caution.
   * @param[out] res result
   * @param[in] a operand 1
   * @param[in] b operand 2
   */
  void sub_nr(Double *res, const Double &a, const Double &b);

  /** Compute opposite of a
   * @param[out] res result = -a
   * @param[in] a operand
   */
  void opp(Element *res, const Element &a);

  /** Compute opposite of a
   * @param[out] res result = -a
   * @param[in] a operand
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

  /** Performs res = a * b without reduction.
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
  void mul(Double *res, const Double &a, const ecl_digit b);

  /** Performs res = a * a.
   * @param[out] res result
   * @param[in] a operand
   */
  void sqr(Element *res, const Element &a);

  /** Performs res = a * a without reduction.
   * @param[out] res result
   * @param[in] a operand
   */
  void sqr(Double *res, const Element &a);

  /** Right shift of d bit
   * @param[out] res result
   * @param[in] a operand
   * @param[in] d number of bits to shift
   */
  void r_shift(Element *res, const Element &a, int d);

  /** Right shift of 1 bits
   * @param[out] res result
   * @param[in] a operand
   */
  void r_shift(Element *res, const Element &a);

  /** Performs res = a^e.
   * @param[out] res result
   * @param[in] a operand 1
   * @param[in] e power
   */
  void exp(Element *res, const Element &a, const Element &e);

  /** Performs res = a^(p^i).
   * where p is the field characteristic.
   * @param[out] res result
   * @param[in] a operand 1
   * @param[in] i forbenius power
   */
  ErrCode frobenius(Element *res, const Element &a, int i);

  /** Compares a and b
   * @param[in] a GFp Element
   * @param[in] b GFp Element
   * @return -1 if a > b
   * @return  1 if a < b
   * @return  0 if a == b
   */
  int cmp(const Element &a, const Element &b);

  /** Compares a and b
   * @param[in] a GFp Double
   * @param[in] b GFp Double
   * @return -1 if a > b
   * @return  1 if a < b
   * @return  0 if a == b
   */
  int cmp(const Double &a, const Double &b);

  /** Compares a and b
   * @param[in] a GFp Element
   * @param[in] b integer
   * @return -1 if a > b
   * @return  1 if a < b
   * @return  0 if a == b
   */
  int cmp(const Element &a, int b);

  /** Compares a and p*R
   * @param[in] a GFp Element
   * @return -1 if a > p*R
   * @return  1 if a < p*R
   * @return  0 if a == p*R
   */
  int cmp_p(const Double &a);

  /** Get legendre symbol for a.
   * @param a field element.
   * @return legendre symbol for a
   */
  int legendre(const Element &a);

  /** Compute the square root of element a.
   * @param[in] res \f$ res = \sqrt{a} \f$
   * @param[in] a field element
   * @return ERR_OK if success
   * @return ERR_NOT_SQUARE if a is a quadratic non residue
   * @return ERR_NOT_IMPLEMENTED if p mod 8 == 1
   */
  ErrCode sqrt(Element *res, const Element &a);

  /** Compute the square root of element a (slow).
   * This is the generic version which does not dependof special prime form
   * (called by sqrt() anyway).
   * @param[in] res \f$ res = \sqrt{a} \f$
   * @param[in] a field element
   * @return ERR_OK if success
   * @return ERR_NOT_SQUARE if a is a quadratic non residue
   * @return ERR_NOT_IMPLEMENTED if p mod 8 == 1
   */
  ErrCode tonelli_shanks(Element *res, const Element &a);

  /** Performs res = 1 / a.
   * @param[out] res result
   * @param[in] a operand
   */
  void inv(Element *res, const Element &a);

  /** Performs res = a / b.
   * @param[out] res result
   * @param[in] a operand 1
   * @param[in] b operand 2
   */
  void div(Element *res, const Element &a, const Element &b);

  /** Computes whether a is a Quadratic Non Residue
   * @param a field element
   * @return true if a is a Quadratic Non Residue
   * @return false if a is a Quadratic Residue
   */
  bool isQNR(const Element &a);

  /** Computes whether a is a Cubic Non Residue.
   * @note : only implemented if p = 1 mod 3. Will give undefined result otherwise.
   * @param a field element
   * @return true if a is a Cubic Non Residue
   * @return false if a is a Cubic Residue
   */
  bool isCNR(const Element &a);

  /** Returns extension degre
   *
   */
  static int getExtensionDegre() {
    return 1;
  }

  /** Returns extension degre
   *
   */
  void get_characteristic(Element *p) {
    p->copy(p_);
  }

  /** Get the base field.
   * This field is the prime field ( GFp ).
   */
  GFp *getBasePrimeField() {
    return this;
  }

 private:
  Element p_;
  Element R_;
  Element R2_;
  Element R3_;
  Double Rp_;
  ecl_digit m_;

  void init();
  ErrCode montSetup(ecl_digit *rho, const Element &a);
};

}  // namespace field
}  // namespace ecl

#endif  // ECL_FIELD_GFP_H_

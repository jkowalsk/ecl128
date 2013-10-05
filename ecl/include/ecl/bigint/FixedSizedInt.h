/*
 * @file FixedSizedInt.h
 * @author Julien Kowalski
 */

#ifndef ECL_INCLUDE_BIGINT_FIXEDSIZEDINT_H_
#define ECL_INCLUDE_BIGINT_FIXEDSIZEDINT_H_
#include <cstdlib>
#include <string>
#include <cstring>

#include "ecl/config.h"
#include "ecl/errcode.h"

using std::string;

namespace ecl {

/** Template for fixed size precision big integers.
 */
template<int nb_limbs>
class FixedSizedInt {
 public:
  /** Basic contructor.
   * Zeroizes content.
   */
  FixedSizedInt();
  /** Basic destructor.
   * Zeroizes content.
   */
  virtual ~FixedSizedInt();

  /** Number of limbs of the number representation */
  static const int nb_limbs_ = nb_limbs;

  /** BaseElement content.
   */
  ecl_digit val[nb_limbs];

  /** Zeroizes the element.
   */
  void zero();

  /** get string representation of the element content in radix 16.
   */
  string toString() const;

  /** Sets element to specific value e = d.
   * @param[in] d value to set
   */
  void set(ecl_digit d);

  /** Get random element.
   * @param f_rng random generation function
   * @param p_rng RNG internal state
   *
   */
  void rand(int (*f_rng)(unsigned char *, int, void *), void *p_rng);

  /** Tells wether element is zero or not.
   * @return boolean
   */
  bool isZero() const;

  /** Tells wether both elements are equal or not.
   * @param e element to compare to this.
   * @return boolean
   */
  bool eq(const FixedSizedInt<nb_limbs> &e);

  /** Copies an element into this.
   * @param e element to be copied
   */
  void copy(const FixedSizedInt<nb_limbs> &e);

  /** Reads element from radix 16 representation
   * @param[out] sign sign of the element
   * @param[in] str string representation
   *
   * @return ERR_OK if success
   * @return ERR_INVALID_VALUE if string representation is too long
   */
  ErrCode fromString(int *sign, const string str);

  /** Get bit size
   * @return bit size of element
   */
  int count_bits() const;

  /** Get the number of zero least signification bits.
   * @return the position of the first non zero bit.
   */
  int count_lsb() const;

  /** Get bit value at position i
   * @param i bit position
   * @return bit value
   */
  int get_bit(int i) const;

  /** Get the element in wNAF form.
   * Obtain a windowed Non Adjacent form of the element.
   * @param[out] wNaf wNAF form
   * @param[out] wNaf_sz size of the wNAF form
   * @param[in] w size of the window.
   *
   */
  void get_wNAF(int *wNaf, int *wNaf_sz, int w);

  /** Get the size of the val table.
   * @return size of the val table.
   */
  static int get_size() {
    return nb_limbs;
  }

  /** Standard addition without reduction relative to field characteristic.
   * @param[out] res result
   * @param[in] a operand 1
   * @param[in] b operand 2
   * @return eventual carry
   */
  static unsigned char add(FixedSizedInt<nb_limbs> *res,
                           const FixedSizedInt<nb_limbs> &a,
                           const FixedSizedInt<nb_limbs> &b);

  /** Standard addition without reduction relative to field characteristic.
   * @param[out] res result
   * @param[in] a operand 1
   * @param[in] b operand 2
   * @return eventual carry
   */
  static unsigned char add(FixedSizedInt<nb_limbs> *res,
                           const FixedSizedInt<nb_limbs> &a, const ecl_digit b);

  /** Adds a + 2^val_size_*sizeof(ecl_digit) b without reduction relative to field characteristic.
   * @param[out] res result
   * @param[in] a operand 1
   * @param[in] b operand 2
   * @return eventual carry
   */
  static unsigned char add_msw(FixedSizedInt<2*nb_limbs> *res,
                               const FixedSizedInt<2*nb_limbs> &a,
                               const FixedSizedInt<nb_limbs> &b);

  /** Standard substraction without reduction relative to field characteristic.
   * @param[out] res result
   * @param[in] a operand 1
   * @param[in] b operand 2
   * @return eventual borrow
   */
  static unsigned char sub(FixedSizedInt<nb_limbs> *res,
                           const FixedSizedInt<nb_limbs> &a,
                           const FixedSizedInt<nb_limbs> &b);

  /** Subs a - 2^val_size_*sizeof(ecl_digit) b without reduction relative to field characteristic.
   * @param[out] res result
   * @param[in] a operand 1
   * @param[in] b operand 2
   * @return eventual carry
   */
  static unsigned char sub_msw(FixedSizedInt<2*nb_limbs> *res,
                               const FixedSizedInt<2*nb_limbs> &a,
                               const FixedSizedInt<nb_limbs> &b);

  /** Standard substraction without reduction relative to field characteristic.
   * @param[out] res result
   * @param[in] a operand 1
   * @param[in] b operand 2
   * @return eventual borrow
   */
  static unsigned char sub(FixedSizedInt<nb_limbs> *res,
                           const FixedSizedInt<nb_limbs> &a, const ecl_digit b);

  /** Standard multiplication.
   * Note that the result is truncated to element size.
   * @param[out] res result
   * @param[in] a operand 1
   * @param[in] b operand 2
   * @return eventual borrow
   */
  static void mul(FixedSizedInt<nb_limbs> *res, const FixedSizedInt<nb_limbs> &a,
                  const FixedSizedInt<nb_limbs> &b);

  /** Right shift of b bits.
   * @param[out] res result
   * @param[in] a operand 1
   * @param[in] b operand 2
   */
  static void r_shift(FixedSizedInt<nb_limbs> *res, const FixedSizedInt<nb_limbs> &a,
                      const ecl_digit b);

};

} /* namespace ecl */
#endif /* ECL_INCLUDE_BIGINT_FIXEDSIZEDINT_H_ */

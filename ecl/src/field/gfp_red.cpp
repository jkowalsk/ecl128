/**
 * @file gfp_red.cpp
 * @author Julien Kowalski
 */

#include "ecl/errcode.h"
#include "ecl/field/GFp.h"
#include "../asm/arch.h"

using ecl::ErrCode;

namespace ecl {
namespace field {

ErrCode GFp::montSetup(ecl_digit *rho, const Element &a) {
  ecl_digit x, b;

  /* fast inversion mod 2**k
   *
   * Based on the fact that
   *
   * XA = 1 (mod 2**n)  =>  (X(2-XA)) A = 1 (mod 2**2n)
   *                    =>  2*X*A - X*X*A*A = 1
   *                    =>  2*(1) - (1)     = 1
   */
  b = a.val[0];

  if ((b & 1) == 0) {
    return ERR_INVALID_VALUE;
  }

  x = (((b + 2) & 4) << 1) + b; /* here x*a==1 mod 2**4 */
  x *= 2 - b * x; /* here x*a==1 mod 2**8 */
  x *= 2 - b * x; /* here x*a==1 mod 2**16 */
  x *= 2 - b * x; /* here x*a==1 mod 2**32 */
#ifdef DIGIT_64
  x *= 2 - b * x; /* here x*a==1 mod 2**64 */
#endif

  /* rho = -1/m mod b */
#ifdef _MSC_VER
#pragma warning( disable : 4146)
#endif 
  *rho = -x;
#ifdef _MSC_VER
#pragma warning( default : 4146)
#endif
  return ERR_OK;

}

#ifdef DIGIT_64
void GFp::reduce(Element *res, const Double &a) {
  register ecl_digit c0, c1, c2;
  register ecl_digit k1, k2, k3, k4;
  register ecl_digit n1, n2, n3, n4;
#ifdef FORCE_NO_ASM
  register ecl_digit carry;
#endif

  n1 = p_.val[0];
  n2 = p_.val[1];
  n3 = p_.val[2];
  n4 = p_.val[3];

  c1 = c2 = 0;

  c0 = a.val[0];
  k1 = m_ * c0;
  MULADD_ALLREG(k1, n1);

  c0 = a.val[1];
  ADD(c0, c0, c1);
  ADDC(c1, 0, c2);
  c2 = 0;
  MULADD_ALLREG(k1, n2);
  k2 = m_ * c0;
  MULADD_ALLREG(k2, n1);

  c0 = a.val[2];
  ADD(c0, c0, c1);
  ADDC(c1, 0, c2);
  c2 = 0;
  MULADD_ALLREG(k1, n3);
  MULADD_ALLREG(k2, n2);
  k3 = m_ * c0;
  MULADD_ALLREG(k3, n1);

  c0 = a.val[3];
  ADD(c0, c0, c1);
  ADDC(c1, 0, c2);
  c2 = 0;
  MULADD_ALLREG(k1, n4);
  MULADD_ALLREG(k2, n3);
  MULADD_ALLREG(k3, n2);
  k4 = m_ * c0;
  MULADD_ALLREG(k4, n1);

  c0 = a.val[4];
  ADD(c0, c0, c1);
  ADDC(c1, 0, c2);
  c2 = 0;
  MULADD_ALLREG(k2, n4);
  MULADD_ALLREG(k3, n3);
  MULADD_ALLREG(k4, n2);
  res->val[0] = c0;

  c0 = a.val[5];
  ADD(c0, c0, c1);
  ADDC(c1, 0, c2);
  c2 = 0;
  MULADD_ALLREG(k3, n4);
  MULADD_ALLREG(k4, n3);
  res->val[1] = c0;

  c0 = a.val[6];
  ADD(c0, c0, c1);
  ADDC(c1, 0, c2);
  c2 = 0;
  MULADD_ALLREG(k4, n4);
  res->val[2] = c0;

  ADD(res->val[3], a.val[7], c1);
  ADDC(c2, c2, 0);

  /* if res >= p then res = res -p */
  if (c2 || (cmp(*res, p_) != 1)) {
    FixedSizedInt<NB_LIMBS>::sub(res, *res, p_);
  }
  k1 = k2 = k3 = k4 = 0;
}
#else // DIGIT_32
void GFp::reduce(Element *res, const Double &a) {
  register ecl_digit c0, c1, c2;
  register ecl_digit k1, k2, k3, k4, k5, k6, k7, k8;
  register ecl_digit n1, n2, n3, n4, n5, n6, n7, n8;
#if defined(FORCE_NO_ASM) || defined(ARCH_ARM_32)
  ecl_digit carry;
#endif

  n1 = p_.val[0];
  n2 = p_.val[1];
  n3 = p_.val[2];
  n4 = p_.val[3];
  n5 = p_.val[4];
  n6 = p_.val[5];
  n7 = p_.val[6];
  n8 = p_.val[7];

  c1 = c2 = 0;

  c0 = a.val[0];
  k1 = m_ * c0;
  MULADD_ALLREG(k1, n1);

  c0 = a.val[1];
  ADD(c0, c0, c1);
  ADDC(c1, 0, c2);
  c2 = 0;
  MULADD_ALLREG(k1, n2);
  k2 = m_ * c0;
  MULADD_ALLREG(k2, n1);

  c0 = a.val[2];
  ADD(c0, c0, c1);
  ADDC(c1, 0, c2);
  c2 = 0;
  MULADD_ALLREG(k1, n3);
  MULADD_ALLREG(k2, n2);
  k3 = m_ * c0;
  MULADD_ALLREG(k3, n1);

  c0 = a.val[3];
  ADD(c0, c0, c1);
  ADDC(c1, 0, c2);
  c2 = 0;
  MULADD_ALLREG(k1, n4);
  MULADD_ALLREG(k2, n3);
  MULADD_ALLREG(k3, n2);
  k4 = m_ * c0;
  MULADD_ALLREG(k4, n1);

  c0 = a.val[4];
  ADD(c0, c0, c1);
  ADDC(c1, 0, c2);
  c2 = 0;
  MULADD_ALLREG(k1, n5);
  MULADD_ALLREG(k2, n4);
  MULADD_ALLREG(k3, n3);
  MULADD_ALLREG(k4, n2);
  k5 = m_ * c0;
  MULADD_ALLREG(k5, n1);

  c0 = a.val[5];
  ADD(c0, c0, c1);
  ADDC(c1, 0, c2);
  c2 = 0;
  MULADD_ALLREG(k1, n6);
  MULADD_ALLREG(k2, n5);
  MULADD_ALLREG(k3, n4);
  MULADD_ALLREG(k4, n3);
  MULADD_ALLREG(k5, n2);
  k6 = m_ * c0;
  MULADD_ALLREG(k6, n1);

  c0 = a.val[6];
  ADD(c0, c0, c1);
  ADDC(c1, 0, c2);
  c2 = 0;
  MULADD_ALLREG(k1, n7);
  MULADD_ALLREG(k2, n6);
  MULADD_ALLREG(k3, n5);
  MULADD_ALLREG(k4, n4);
  MULADD_ALLREG(k5, n3);
  MULADD_ALLREG(k6, n2);
  k7 = m_ * c0;
  MULADD_ALLREG(k7, n1);

  c0 = a.val[7];
  ADD(c0, c0, c1);
  ADDC(c1, 0, c2);
  c2 = 0;
  MULADD_ALLREG(k1, n8);
  MULADD_ALLREG(k2, n7);
  MULADD_ALLREG(k3, n6);
  MULADD_ALLREG(k4, n5);
  MULADD_ALLREG(k5, n4);
  MULADD_ALLREG(k6, n3);
  MULADD_ALLREG(k7, n2);
  k8 = m_ * c0;
  MULADD_ALLREG(k8, n1);
///
  c0 = a.val[8];
  ADD(c0, c0, c1);
  ADDC(c1, 0, c2);
  c2 = 0;
  MULADD_ALLREG(k2, n8);
  MULADD_ALLREG(k3, n7);
  MULADD_ALLREG(k4, n6);
  MULADD_ALLREG(k5, n5);
  MULADD_ALLREG(k6, n4);
  MULADD_ALLREG(k7, n3);
  MULADD_ALLREG(k8, n2);
  res->val[0] = c0;

  c0 = a.val[9];
  ADD(c0, c0, c1);
  ADDC(c1, 0, c2);
  c2 = 0;
  MULADD_ALLREG(k3, n8);
  MULADD_ALLREG(k4, n7);
  MULADD_ALLREG(k5, n6);
  MULADD_ALLREG(k6, n5);
  MULADD_ALLREG(k7, n4);
  MULADD_ALLREG(k8, n3);
  res->val[1] = c0;

  c0 = a.val[10];
  ADD(c0, c0, c1);
  ADDC(c1, 0, c2);
  c2 = 0;
  MULADD_ALLREG(k4, n8);
  MULADD_ALLREG(k5, n7);
  MULADD_ALLREG(k6, n6);
  MULADD_ALLREG(k7, n5);
  MULADD_ALLREG(k8, n4);
  res->val[2] = c0;

  c0 = a.val[11];
  ADD(c0, c0, c1);
  ADDC(c1, 0, c2);
  c2 = 0;
  MULADD_ALLREG(k5, n8);
  MULADD_ALLREG(k6, n7);
  MULADD_ALLREG(k7, n6);
  MULADD_ALLREG(k8, n5);
  res->val[3] = c0;

  c0 = a.val[12];
  ADD(c0, c0, c1);
  ADDC(c1, 0, c2);
  c2 = 0;
  MULADD_ALLREG(k6, n8);
  MULADD_ALLREG(k7, n7);
  MULADD_ALLREG(k8, n6);
  res->val[4] = c0;

  c0 = a.val[13];
  ADD(c0, c0, c1);
  ADDC(c1, 0, c2);
  c2 = 0;
  MULADD_ALLREG(k7, n8);
  MULADD_ALLREG(k8, n7);
  res->val[5] = c0;

  c0 = a.val[14];
  ADD(c0, c0, c1);
  ADDC(c1, 0, c2);
  c2 = 0;
  MULADD_ALLREG(k8, n8);
  res->val[6] = c0;

  ADD(res->val[7], a.val[15], c1);
  ADDC(c2, c2, 0);

  /* if A >= p then A = A -p */
  if (c2 || (cmp(*res, p_) != 1)) {
    FixedSizedInt<NB_LIMBS>::sub(res, *res, p_);
  }
  k1 = k2 = k3 = k4 = k5 = k6 = k7 = k8 = 0;

}
#endif
}  // namespace field
}  // namespace ecl

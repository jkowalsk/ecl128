/*
 * @file fp2_base.cpp
 * @author Julien Kowalski
 */

#include <iostream>

#include <cstring>
#include <string>

#include "ecl/field/GFp.h"
#include "ecl/field/Fp2.h"

using ecl::ErrCode;

namespace ecl {
namespace field {

Fp2::Fp2(string p) {
  gfp = new GFp(p);
  init_qnr();
  init_xsi();
}

Fp2::Fp2(const GFp::Element &p) {
  gfp = new GFp(p);
  init_qnr();
  init_xsi();
}

Fp2::~Fp2() {
  delete gfp;
}

void Fp2::init_qnr() {
  GFp::Element a;
  int tested = 0;
  // get quadratic non residue from GFp
  do {
    tested++;
    gfp->set(&a, -tested);
  } while (!gfp->isQNR(a));
  gfp_qnr_ = (ecl_digit)tested;
}

void Fp2::init_xsi() {
  GFp::Element candidate;

  // c = a²+gfp_qnr_.b² nor square nor cube in gfp
  // severall tries :

  // a = 0, b = 1 ==> c = gfp_qnr_
  gfp->set(&candidate, gfp_qnr_);
  if (gfp->isCNR(candidate)) {
    xsi_ = ZERO_ONE;
    return;
  }

  // a = 1, b = 1 ==> c = gfp_qnr_ + 1
  gfp->set(&candidate, gfp_qnr_ + 1);
  if (gfp->isQNR(candidate) && gfp->isCNR(candidate)) {
    xsi_ = ONE_ONE;
    return;
  }

  // a = 2, b = 1 ==> c = gfp_qnr_ + 4
  gfp->set(&candidate, gfp_qnr_ + 4);
  if (gfp->isQNR(candidate) && gfp->isCNR(candidate)) {
    xsi_ = TWO_ONE;
    return;
  }

  // a = 3, b = 1 ==> c = gfp_qnr_ + 9
  gfp->set(&candidate, gfp_qnr_ + 9);
  if (gfp->isQNR(candidate) && gfp->isCNR(candidate)) {
    xsi_ = THREE_ONE;
    return;
  }
}

ErrCode Fp2::get_xsi(Element *res) {
  switch (xsi_) {
    case ZERO_ONE:
      res[0]->zero();
      gfp->one(&((*res)[1]));
      break;
    case ONE_ONE:
      gfp->one(&((*res)[0]));
      gfp->one(&((*res)[1]));
      break;
    case TWO_ONE:
      gfp->set(&((*res)[0]), 2);
      gfp->one(&((*res)[1]));
      break;
    case THREE_ONE:
      gfp->set(&((*res)[0]), 3);
      gfp->one(&((*res)[1]));
      break;
    default:
      return ERR_NOT_IMPLEMENTED;
  }
  return ERR_OK;
}

void Fp2::rand(Element *res, int (*f_rng)(unsigned char *, int, void *),
               void *p_rng) {
  gfp->rand(&((*res)[1]), f_rng, p_rng);
  gfp->rand(&((*res)[0]), f_rng, p_rng);
}

void Fp2::zero(Element *res) {
  (*res)[0].zero();
  (*res)[1].zero();
}

void Fp2::one(Element *res) {
  gfp->one(&((*res)[0]));
  (*res)[1].zero();
}

void Fp2::set(Element *res, const int v) {
  gfp->set(&((*res)[0]), v);
  (*res)[1].zero();
}

void Fp2::copy(Element *res, const Element &a) {
  ((*res)[0]).copy(a[0]);
  ((*res)[1]).copy(a[1]);
}

void Fp2::copy(Double *res, const Double &a) {
  ((*res)[0]).copy(a[0]);
  ((*res)[1]).copy(a[1]);
}

bool Fp2::isOne(const Element &a) {
  return gfp->isOne(a[0]) && a[1].isZero();
}

bool Fp2::isZero(const Element &a) {
  return gfp->isZero(a[0]) && gfp->isZero(a[1]);
}

void Fp2::add(Element *res, const Element &a, const Element &b) {
  gfp->add(&((*res)[0]), a[0], b[0]);
  gfp->add(&((*res)[1]), a[1], b[1]);
}

void Fp2::add(Element *res, const Element &a, const GFp::Element &b) {
  gfp->add(&((*res)[0]), a[0], b);
}

void Fp2::add(Double *res, const Double &a, const Double &b) {
  gfp->add(&((*res)[0]), a[0], b[0]);
  gfp->add(&((*res)[1]), a[1], b[1]);
}

void Fp2::sub(Element *res, const Element &a, const ecl_digit b) {
  gfp->sub(&((*res)[0]), a[0], b);
}

void Fp2::sub(Element *res, const Element &a, const Element &b) {
  gfp->sub(&((*res)[0]), a[0], b[0]);
  gfp->sub(&((*res)[1]), a[1], b[1]);
}

void Fp2::sub(Double *res, const Double &a, const Double &b) {
  gfp->sub(&((*res)[0]), a[0], b[0]);
  gfp->sub(&((*res)[1]), a[1], b[1]);
}

void Fp2::opp(Element *res, const Element &a) {
  gfp->opp(&((*res)[0]), a[0]);
  gfp->opp(&((*res)[1]), a[1]);
}

void Fp2::opp(Double *res, const Double &a) {
  gfp->opp(&((*res)[0]), a[0]);
  gfp->opp(&((*res)[1]), a[1]);
}

void Fp2::reduce(Element *res, const Double &a) {
  gfp->reduce(&((*res)[0]), a[0]);
  gfp->reduce(&((*res)[1]), a[1]);
}

int Fp2::cmp(const Element &a, const Element &b) {
  return gfp->cmp(a[0], b[0]) | gfp->cmp(a[1], b[1]);
}

int Fp2::cmp(const Element &a, const GFp::Element &b) {
  return gfp->cmp(a[0], b) | gfp->cmp(a[1], 0);
}

int Fp2::cmp(const Double &a, const Double &b) {
  return gfp->cmp(a[0], b[0]) | gfp->cmp(a[1], b[1]);
}

void Fp2::conj(Element *res, const Element &a) {
  ((*res)[0]).copy(a[0]);
  gfp->opp(&((*res)[1]), a[1]);
}

void Fp2::init(Element *res, const GFp::Element &a0,
               const GFp::Element &a1) {
  ((*res)[0]).copy(a0);
  ((*res)[1]).copy(a1);
}

}  // namespace field
}  // namespace ecl

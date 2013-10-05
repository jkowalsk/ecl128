/**
 * @file errcode.h
 *
 * Part of the ecl library.
 *
 * Copyright 2013 Julien Kowalski.
 *
 */

#ifndef ERRCODE_H_
#define ERRCODE_H_

/** Namespace for ecl library.
 */
namespace ecl {

/** Error code definition.
 */
enum ErrCode {
  ERR_OK,  //!< Success
  ERR_INVALID_VALUE,  //!< Input is incorrect
  ERR_NOT_IMPLEMENTED,  //!< function is not implemented for this specific input
  ERR_NOT_SQUARE,  //!< Input required to be a square is not
  ERR_KEY_NOT_SET //!< The key has not been set
};

}

#endif /* ERRCODE_H_ */

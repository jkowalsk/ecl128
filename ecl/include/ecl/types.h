/*
 * @file Buffer.h
 * @author Julien Kowalski
 */

#ifndef ECL_UTILS_TYPES_H_
#define ECL_UTILS_TYPES_H_

#include <vector>
#include <string>
#include "ecl/config.h"

namespace ecl {

/** Secure allocator.
 * Zeroizes memory on deallocation.
 */
template <class T> class sec_allocator : public std::allocator<T> {
 public:
  //   template<class U> struct rebind { typedef sec_allocator<U> other; };
  //   sec_allocator() throw() {}
  //   sec_allocator(const sec_allocator &) throw() {}
  //   template <class U> sec_allocator(const sec_allocator<U>&) throw() {}

  /** Zeroize memory on deallocation.
   * @param p pointer to memory
   * @param num size of memory
   */
  void deallocate(T *p, std::size_t num) {
    ZEROMEM((void *)p, num);
    std::allocator<T>::deallocate(p, num);
  }
};

/** Handle fixed size buffers.
 * Memoery is zeroized on deallocation.
 */
template<int size>
class FixedSizedBuffer {
 public:
  /** Simple constructor
   */
  FixedSizedBuffer() {
    ZEROMEM(buff_, size);
  }

  /** Simple destructor
   * Memory is erased
   */
  ~FixedSizedBuffer() {
    ZEROMEM(buff_, size);
  }

  /** Copy from C buffer
   * @param[in] in source C buffer
   */
  FixedSizedBuffer &operator=(unsigned char in[size]) {
    memcpy(buff_, in, size);
    return *this;
  }

  /** Copy from buffer
   * @param[in] b source buffer
   */
  FixedSizedBuffer &operator=(const FixedSizedBuffer & b) {
    memcpy(this->buff_, b.buff_, size * sizeof(unsigned char)) ;
    return *this;
  }

  /** Get reference to an element.
   * For setting a value in the Buffer
   * @param[in] i position in the buffer
   * @return reference to element.
   */
  unsigned char  & operator[](int i)  {
    if(i>size) return buff_[0];
    return buff_[i];
  }

  /** Get an element value.
     * @param[in] i position in the buffer
     * @return element value.
     */
  unsigned char  operator[](int i) const {
    if(i>size) return 0;
    return buff_[i];
  }

  /** Compares two buffer
   *  @param b right operand.
   *  @return true if buffer are equal
   *  @return false otherwise
   */
  bool operator== (FixedSizedBuffer const &b) {
    bool ret = true;
    for(int i=0; i<size; i++) {
      ret &= (this->buff_[i] == b[i]) ;
    }
    return ret;
  }

  /** Xors a buffer to this.
   * @param b buffer to xor
   * @return *this.
   */
  FixedSizedBuffer &operator^=(const FixedSizedBuffer & b) {
    for(int i=0; i<size; i++) {
      this->buff_[i] ^= b[i] ;
    }
    return *this;
  }

 private:
  unsigned char buff_[size];
};

/** Secure string class. A string with secure allocator, for handling passwords */
typedef std::basic_string<char, std::char_traits<char>, sec_allocator<char> > sstring;

/** Buffer class. A vector with secure allocator */
typedef std::vector<unsigned char, sec_allocator<unsigned char> > Buffer;


} /* namespace ecl */

#endif /* ECL_UTILS_TYPES_H_ */

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

template <class T> class sec_allocator : public std::allocator<T> {
 public:
   template<class U> struct rebind { typedef sec_allocator<U> other; };
   sec_allocator() throw() {}
   sec_allocator(const sec_allocator &) throw() {}
   template <class U> sec_allocator(const sec_allocator<U>&) throw() {}

   void deallocate(T *p, std::size_t num) {
     ZEROMEM((void *)p, num);
     std::allocator<T>::deallocate(p, num);
   }
 };

template<int size>
class FixedSizedBuffer {
public:
  FixedSizedBuffer() {
    ZEROMEM(buff_, size);
  }

  ~FixedSizedBuffer() {
    ZEROMEM(buff_, size);
  }

  FixedSizedBuffer &operator=(unsigned char in[size]) {
    memcpy(buff_, in, size);
    return *this;
  }

  FixedSizedBuffer &operator=(const FixedSizedBuffer & b) {
    memcpy(this->buff_, b.buff_, size * sizeof(unsigned char)) ;
    return *this;
  }

  unsigned char  & operator[](int i)  {
    if(i>size) return buff_[0];
    return buff_[i];
  }

  unsigned char  operator[](int i) const {
    if(i>size) return 0;
    return buff_[i];
  }

  bool operator== (FixedSizedBuffer const &b) {
    bool ret = true;
    for(int i=0; i<size; i++) {
      ret &= (this->buff_[i] == b[i]) ;
    }
    return ret;
  }

  FixedSizedBuffer &operator^=(const FixedSizedBuffer & b) {
    for(int i=0; i<size; i++) {
          this->buff_[i] ^= b[i] ;
        }
      return *this;
    }

 private:
  unsigned char buff_[size];
};


typedef std::basic_string<char, std::char_traits<char>, sec_allocator<char> > sstring;
typedef std::vector<unsigned char, sec_allocator<unsigned char> > Buffer;



} /* namespace ecl */
#endif /* ECL_UTILS_TYPES_H_ */

/// \file CompressedArray.h
/// \author R.G.A. Deckers
/// \brief Definition of the CompressedArray class.
///
/// See implementation file for copyright details.

#ifndef COMPRESSEDARRAY_H
#define COMPRESSEDARRAY_H
#include <iostream>
#include <typeinfo>
#include <vector>
/// Short CompressedArray description
///
/// More detailed CompressedArray description
namespace O2 {
template <class T> class CompressedArray {
public:
  /// Default constructor
  CompressedArray(){};
  /// Destructor
  ~CompressedArray(){};
  void push_value(const T &element) { mVector.push_back(element); }
  template <class iterator_type>
  void push_range(iterator_type begin, iterator_type end) {
    while (begin != end) {
      push_element(*(begin++));
    }
  }
  void clear() { mVector.clear(); }
  size_t estimate_compression();
  // void compress_into(std::ostream &stream);
  // void restore_from(std::istream &stream);

protected:
  // protected stuff goes here

private:
  /// copy constructor prohibited
  CompressedArray(const CompressedArray &);
  /// assignment operator prohibited
  CompressedArray &operator=(const CompressedArray &);
  std::vector<T> mVector;
};
}
#endif

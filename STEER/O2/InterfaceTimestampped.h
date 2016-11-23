/// \file InterfaceTimestampped.h
/// \author R.G.A. Deckers
/// \brief Definition of the InterfaceTimestampped class.
///
/// See implementation file for copyright details.

#ifndef INTERFACETIMESTAMPPED_H
#define INTERFACETIMESTAMPPED_H

/// Shared abstract base class for every timestamped item
typedef float timestamp_t;
class InterfaceTimestampped {
public:
  timestamp_t getTimestamp() const;
  // as member function
  bool before(const InterfaceTimestampped &rhs) const {
    return getTimestamp() < rhs.getTimestamp();
  };
  bool after(const InterfaceTimestampped &rhs) const {
    return getTimestamp() > rhs.getTimestamp();
  };

  // as static functions
  static bool before(InterfaceTimestampped &lhs, InterfaceTimestampped &rhs) {
    return lhs.before(rhs);
  };
  static bool after(const InterfaceTimestampped &lhs,
                    const InterfaceTimestampped &rhs) {
    return lhs.after(rhs);
  };

  // operators so we can use std::sort
  friend bool operator<(const InterfaceTimestampped &lhs,
                        const InterfaceTimestampped &rhs) {
    return lhs.before(rhs);
  };
  friend bool operator>(const InterfaceTimestampped &lhs,
                        const InterfaceTimestampped &rhs) {
    return lhs.after(rhs);
  };

protected:
private:
};

#endif

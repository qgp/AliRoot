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
  virtual ~InterfaceTimestampped(){};
  virtual timestamp_t getTimestampResolution() const = 0;
  virtual timestamp_t getTimestamp() const = 0;
  // as member function
  bool before(const InterfaceTimestampped &rhs) const {
    return getTimestamp() < rhs.getTimestamp();
  };
  bool after(const InterfaceTimestampped &rhs) const {
    return getTimestamp() > rhs.getTimestamp();
  };
  bool before(const timestamp_t &rhs) const { return getTimestamp() < rhs; };
  bool after(const timestamp_t &rhs) const { return getTimestamp() > rhs; };

  // as static functions
  static bool before(const InterfaceTimestampped &lhs,
                     const InterfaceTimestampped &rhs) {
    return lhs.before(rhs);
  };
  static bool after(const InterfaceTimestampped &lhs,
                    const InterfaceTimestampped &rhs) {
    return lhs.after(rhs);
  };
  static bool before(const InterfaceTimestampped &lhs, const timestamp_t &rhs) {
    return lhs.before(rhs);
  };
  static bool after(const InterfaceTimestampped &lhs, const timestamp_t &rhs) {
    return lhs.after(rhs);
  };
  static bool before(const timestamp_t &lhs, const InterfaceTimestampped &rhs) {
    return rhs.after(lhs);
  };
  static bool after(const timestamp_t &lhs, const InterfaceTimestampped &rhs) {
    return rhs.before(lhs);
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

  friend bool operator<(const InterfaceTimestampped &lhs,
                        const timestamp_t &rhs) {
    return lhs.before(rhs);
  };
  friend bool operator>(const InterfaceTimestampped &lhs,
                        const timestamp_t &rhs) {
    return lhs.after(rhs);
  };

  friend bool operator<(const timestamp_t &lhs,
                        const InterfaceTimestampped &rhs) {
    return rhs.after(lhs);
  };
  friend bool operator>(const timestamp_t &lhs,
                        const InterfaceTimestampped &rhs) {
    return rhs.before(lhs);
  };

protected:
private:
};

#endif

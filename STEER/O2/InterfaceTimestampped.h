/// \file InterfaceTimestampped.h
/// \author R.G.A. Deckers
/// \brief Definition of the InterfaceTimestampped class.
///
/// \copyright
///  This program is free software; you can redistribute it and/or
/// modify it under the terms of the GNU General Public License as
/// published by the Free Software Foundation; either version 3 of
/// the License, or (at your option) any later version.
/// This program is distributed in the hope that it will be useful, but
/// WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
/// General Public License for more details at
/// https://www.gnu.org/copyleft/gpl.html

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

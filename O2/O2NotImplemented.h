/// \file O2NotImplemented.h
/// \author R.G.A. Deckers
/// \brief A simple macro for terminating when an as-of-yet unimplemented
/// function is called.
///
///  This program is free software; you can redistribute it and/or
/// modify it under the terms of the GNU General Public License as
/// published by the Free Software Foundation; either version 3 of
/// the License, or (at your option) any later version.
/// This program is distributed in the hope that it will be useful, but
/// WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
/// General Public License for more details at
/// https://www.gnu.org/copyleft/gpl.html

#include <cstdlib>
#include <iostream>
#include <string>

// NOTE: when needed this could be extended to print run-time stack traces.
/// Prints out the function name to std::cerr and exits with -1.
#define EXIT_NOT_IMPLEMENTED()                                                 \
  std::cerr << std::string(__PRETTY_FUNCTION__).append(" : NOT IMPLEMENTED");  \
  __builtin_trap();

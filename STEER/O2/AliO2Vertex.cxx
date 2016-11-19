/// \file AliO2Vertex.cxx
/// \brief implementation of the AliO2Vertex class.
/// \since 2016-11-15
/// \author R.G.A. Deckers
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

#include "AliO2Vertex.h"

// default constructor
AliO2Vertex::AliO2Vertex(float positionX, float positionY, float positionZ,
                           float detectionTime) {
  mPositionX = positionX;
  mPositionY = positionY;
  mPositionZ = positionZ;
  // mDetectionTime = detectionTime;
}

// default destructor
AliO2Vertex::~AliO2Vertex() {}

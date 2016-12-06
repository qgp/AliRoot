/// \file O2Vertex.cxx
/// \brief implementation of the O2Vertex class.
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

#include "O2Vertex.h"
#include <AliVVertex.h>
#include <math.h>

// default constructor
O2Vertex::O2Vertex(float positionX, float positionY, float positionZ,
                   timestamp_t detectionTime,
                   timestamp_t detectionTimeResolution) {
  mPositionX = positionX;
  mPositionY = positionY;
  mPositionZ = positionZ;
  mTimestamp = detectionTime;
  mTimestampResolution = detectionTimeResolution;
}

// TODO: constructor delegation in ROOT6
O2Vertex::O2Vertex(const AliVVertex *vertex, timestamp_t detectionTime,
                   timestamp_t detectionTimeResolution) {
  mPositionX = vertex->GetX();
  mPositionY = vertex->GetY();
  mPositionZ = vertex->GetZ();
  mTimestamp = detectionTime;
  mTimestampResolution = detectionTimeResolution;
}

// default destructor
O2Vertex::~O2Vertex() {}

float O2Vertex::distanceFrom(const AliVVertex *vertex) {
  return distanceFrom(vertex->GetX(), vertex->GetY(), vertex->GetZ());
}

float O2Vertex::distanceFrom(float x, float y, float z) {
  return sqrtf(distanceSquaredFrom(x, y, z));
}

float O2Vertex::distanceSquaredFrom(const AliVVertex *vertex) {
  return distanceSquaredFrom(vertex->GetX(), vertex->GetY(), vertex->GetZ());
}

float O2Vertex::distanceSquaredFrom(float x, float y, float z) {
  float dx = mPositionX - x;
  float dy = mPositionY - y;
  float dz = mPositionZ - z;
  return dx * dx + dy * dy + dz * dz;
}

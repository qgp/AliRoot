/// \file AliO2Track.cxx
/// \brief implementation of the AliO2Track class.
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

#include "AliO2Track.h"
#include <AliVParticle.h>
// default constructor
AliO2Track::AliO2Track(float TransverseMomentum,
                       float TransverseMomentumAnglePhi,
                       float TransverseMomentumAngleTheta,
                       float DistanceOfClosestApproachX,
                       float DistanceOfClosestApproachY, float DetectionTime) {
  mTransverseMomentum = TransverseMomentum;
  mTransverseMomentumAnglePhi = TransverseMomentumAnglePhi;
  mTransverseMomentumAngleTheta = TransverseMomentumAngleTheta;
  // mDistanceOfClosestApproachX = DistanceOfClosestApproachX;
  // mDistanceOfClosestApproachY = DistanceOfClosestApproachY;
  // mDetectionTime = DetectionTime;
}

AliO2Track::AliO2Track(const AliVParticle *particle, float DetectionTime) {
  mTransverseMomentum = particle->Pt();
  mTransverseMomentumAnglePhi = particle->Phi();
  mTransverseMomentumAngleTheta = particle->Theta();
  // mDistanceOfClosestApproachX = DistanceOfClosestApproachX;
  // mDistanceOfClosestApproachY = DistanceOfClosestApproachY;
  // mDetectionTime = DetectionTime;
}

// default destructor
AliO2Track::~AliO2Track() {}

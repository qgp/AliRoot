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
#include <AliExternalTrackParam.h>
#include <AliVParticle.h>

// root specific
ClassImp(AliO2Track);

// TODO: when moving to ROOT6, use constructor delegation here
void AliO2Track::setParameters(const AliExternalTrackParam *param,
                               timestamp_t DetectionTime) {
  setParameters(param->GetAlpha(), param->GetX(), param->GetY(), param->GetZ(),
                param->GetSnp(), param->GetTgl(), param->GetSigned1Pt(),
                param->GetCovariance(), DetectionTime);
}

// TODO: when moving to ROOT6, use constructor delegation here
void AliO2Track::setParameters(Double32_t Alpha, Double32_t X, Double32_t Y,
                               Double32_t Z, Double32_t SinPhi,
                               Double32_t TanLambda, Double32_t InversePt,
                               const Double32_t *Covariance,
                               timestamp_t DetectionTime) {
  mAlpha = Alpha;
  mX = X;
  mY = Y;
  mZ = Z;
  mSinPhi = SinPhi;
  mTanLambda = TanLambda;
  mInversePt = InversePt;
  mTimestamp = DetectionTime;
  if (Covariance) {
    size_t element_count = sizeof(mCovariance) / sizeof(mCovariance[0]);
    for (unsigned u = 0; u < element_count; u++) {
      mCovariance[u] = Covariance[u];
    }
  }
}
// default constructor
AliO2Track::AliO2Track(Double32_t Alpha, Double32_t X, Double32_t Y,
                       Double32_t Z, Double32_t SinPhi, Double32_t TanLambda,
                       Double32_t InversePt, Double32_t *Covariance,
                       timestamp_t DetectionTime) {
  setParameters(Alpha, X, Y, Z, SinPhi, TanLambda, InversePt, Covariance,
                DetectionTime);
}

// construct from a track param
AliO2Track::AliO2Track(const AliExternalTrackParam *param,
                       timestamp_t DetectionTime) {
  setParameters(param, DetectionTime);
}

// Construct from a track (which is used to build a track param)
AliO2Track::AliO2Track(const AliVTrack *track, timestamp_t DetectionTime) {
  AliExternalTrackParam param;
  param.CopyFromVTrack(track);
  setParameters(&param, DetectionTime);
}

// default destructor
AliO2Track::~AliO2Track() {}

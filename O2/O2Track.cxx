/// \file O2Track.cxx
/// \brief implementation of the O2Track class.
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

#include "O2Track.h"
#include <AliExternalTrackParam.h>

// TODO: when moving to ROOT6, use constructor delegation here
void O2Track::setParameters(const AliExternalTrackParam *param,
                            timestamp_t DetectionTime) {
  setParameters(param->GetAlpha(), param->GetX(), param->GetY(), param->GetZ(),
                param->GetSnp(), param->GetTgl(), param->GetSigned1Pt(),
                param->GetCovariance(), DetectionTime);
}

// TODO: when moving to ROOT6, use constructor delegation here
void O2Track::setParameters(float Alpha, float X, float Y, float Z,
                            float SinPhi, float TanLambda, float InversePt,
                            const Double32_t *Covariance,
                            timestamp_t DetectionTime) {
  mData.Alpha = Alpha;
  mData.X = X;
  mData.Y = Y;
  mData.Z = Z;
  mData.SinPhi = SinPhi;
  mData.TanLambda = TanLambda;
  mData.InversePt = InversePt;
  mData.Timestamp = DetectionTime;
  if (Covariance) {
    size_t element_count =
        sizeof(mData.Covariance) / sizeof(mData.Covariance[0]);
    for (unsigned u = 0; u < element_count; u++) {
      mData.Covariance[u] = Covariance[u];
    }
  }
}
// default constructor

O2Track::O2Track(float Alpha, float X, float Y, float Z, float SinPhi,
                 float TanLambda, float InversePt, const Double32_t *Covariance,
                 timestamp_t DetectionTime) {
  setParameters(Alpha, X, Y, Z, SinPhi, TanLambda, InversePt, Covariance,
                DetectionTime);
}

// construct from a track param
O2Track::O2Track(const AliExternalTrackParam *param,
                 timestamp_t DetectionTime) {
  setParameters(param, DetectionTime);
}

// Construct from a track (which is used to build a track param)
O2Track::O2Track(const AliVTrack *track, timestamp_t DetectionTime) {
  AliExternalTrackParam param;
  param.CopyFromVTrack(track);
  setParameters(&param, DetectionTime);
}

// default destructor
O2Track::~O2Track() {}

/// \file AliO2Track.h
/// \author R.G.A. Deckers
/// \brief Definition of the AliO2Track class.
///
/// See implementation file for copyright details.

#ifndef ALIO2TRACK_H
#define ALIO2TRACK_H
#include "InterfaceTimestampped.h"
#include <TMath.h> //Double32_t definition
#include <TObject.h>
class AliVTrack;
class AliExternalTrackParam;
class AliO2Vertex;
/// New Track class, still highly unspecified. Basis for all track classes.
class AliO2Track : public TObject, public InterfaceTimestampped {
public:
  /// Default constructor
  AliO2Track(Double32_t Alpha = 0.0f, Double32_t X = 0.0f, Double32_t Y = 0.0f,
             Double32_t Z = 0.0f, Double32_t SinPhi = 0.0f,
             Double32_t TanLambda = 0.0f, Double32_t InversePt = 0.0f,
             Double32_t *Covariance = nullptr,
             timestamp_t DetectionTime = 0.0f);
  AliO2Track(const AliVTrack *track, timestamp_t DetectionTime = 0.0f);
  AliO2Track(const AliExternalTrackParam *param,
             timestamp_t DetectionTime = 0.0f);
  /// Destructor
  ~AliO2Track();

  timestamp_t getTimestamp() const { return mTimestamp; }

protected:
  // protected stuff goes here
private:
  /// copy constructor prohibited
  // AliO2Track(const AliO2Track &);
  /// assignment operator prohibited
  // AliO2Track &operator=(const AliO2Track &);
  void setParameters(const AliExternalTrackParam *param,
                     timestamp_t DetectionTime);
  void setParameters(Double32_t Alpha, Double32_t X, Double32_t Y, Double32_t Z,
                     Double32_t SinPhi, Double32_t TanLambda,
                     Double32_t InversePt, const Double32_t *Covariance,
                     timestamp_t DetectionTime);

  timestamp_t mTimestamp;
  Double32_t mAlpha; // Local <-->global coor.system rotation angle
  Double32_t mX;     // X coordinate for the point of parametrisation

  Double32_t mY;         /// local Y-coordinate of a track (cm)
  Double32_t mZ;         /// local Z-coordinate of a track (cm)
  Double32_t mSinPhi;    /// local sine of the track momentum azimuthal angle
  Double32_t mTanLambda; /// tangent of the track momentum dip angle
  Double32_t mInversePt; /// 1/pt (1/(GeV/c)), sign encodes charge.
  Double32_t
      mCovariance[15]; /// The track parameter covariance matrix, does not
                       /// include x, alpha, or DetectionTime.

  ClassDef(AliO2Track, 1);
};
/// Global tracks
class AliO2GlobalTrack : public AliO2Track {
  // c++11, inherit constructors
  using AliO2Track::AliO2Track;

public:
  timestamp_t getTimestampResolution() const {
    return 600;
  } // 100ns guassian, 6 sigma.
  ClassDef(AliO2GlobalTrack, 1);
};
/// ITS tracks
class AliO2ITSTrack : public AliO2Track {
  // c++11,  inherit constructors
  using AliO2Track::AliO2Track;

public:
  timestamp_t getTimestampResolution() const {
    return 2500; // boxed per 5000ns, so everything within this box can be
    // associated.
  }
  ClassDef(AliO2ITSTrack, 1);
};

#endif

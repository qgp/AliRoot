/// \file O2Track.h
/// \author R.G.A. Deckers
/// \brief Definition of the O2Track class.
///
/// See implementation file for copyright details.

/*TODO:
    *Determine which tracks have a charge/mass etc info
*/

#ifndef O2TRACK_H
#define O2TRACK_H
// base classes
#include "InterfaceTimestampped.h"

#include <TMath.h> //Double32_t definition
class AliVTrack;
class AliExternalTrackParam;
class O2Vertex;
/// New Track class, still highly unspecified. Basis for all track classes.
class O2Track : public InterfaceTimestampped {
public:
  /// Default constructor
  O2Track(Double32_t Alpha = 0.0f, Double32_t X = 0.0f, Double32_t Y = 0.0f,
          Double32_t Z = 0.0f, Double32_t SinPhi = 0.0f,
          Double32_t TanLambda = 0.0f, Double32_t InversePt = 0.0f,
          Double32_t *Covariance = nullptr, timestamp_t DetectionTime = 0.0f);
  O2Track(const AliVTrack *track, timestamp_t DetectionTime = 0.0f);
  O2Track(const AliExternalTrackParam *param, timestamp_t DetectionTime = 0.0f);
  /// Destructor
  ~O2Track();

  timestamp_t getTimestamp() const { return mTimestamp; }
  /// Local <-->global coor.system rotation angle
  Double32_t GetAlpha() const { return mAlpha; }
  /// X coordinate for the point of parametrisation
  Double32_t GetX() const { return mX; }
  /// local Y-coordinate of a track (cm)
  Double32_t GetY() const { return mY; }
  /// local Z-coordinate of a track (cm)
  Double32_t GetZ() const { return mZ; }
  /// local sine of the track momentum azimuthal angle
  Double32_t GetSinPhi() const { return mSinPhi; }
  /// tangent of the track momentum dip angle
  Double32_t GetTanLambda() const { return mTanLambda; }
  /// 1/pt (1/(GeV/c)), sign encodes charge.
  Double32_t GetInversePt() const { return mInversePt; }
  /// pt (GeV/c), sign encodes charge.
  Double32_t GetPt() const { return 1.0 / mInversePt; }

protected:
  void setParameters(const AliExternalTrackParam *param,
                     timestamp_t DetectionTime);
  void setParameters(Double32_t Alpha, Double32_t X, Double32_t Y, Double32_t Z,
                     Double32_t SinPhi, Double32_t TanLambda,
                     Double32_t InversePt, const Double32_t *Covariance,
                     timestamp_t DetectionTime);

private:
  timestamp_t mTimestamp; /// Best estimate for the time of occurence.
  Double32_t mAlpha;      /// Local <-->global coor.system rotation angle
  // NOTE: Is this the same as the vertex coordinate?
  Double32_t mX; /// X coordinate for the point of parametrisation

  Double32_t mY;         /// local Y-coordinate of a track (cm)
  Double32_t mZ;         /// local Z-coordinate of a track (cm)
  Double32_t mSinPhi;    /// local sine of the track momentum azimuthal angle
  Double32_t mTanLambda; /// tangent of the track momentum dip angle
  Double32_t mInversePt; /// 1/pt (1/(GeV/c)), sign encodes charge.
  Double32_t
      mCovariance[15]; /// The track parameter covariance matrix, does not
                       /// include x, alpha, or DetectionTime.
};
/// Global tracks
class O2GlobalTrack : public virtual O2Track {
  // c++11, inherit constructors
  using O2Track::O2Track;

public:
  timestamp_t getTimestampResolution() const {
    return 600;
  } // 100ns guassian, 6 sigma.
};
/// ITS tracks
class O2ITSTrack : public virtual O2Track {
  // c++11,  inherit constructors
  using O2Track::O2Track;

public:
  timestamp_t getTimestampResolution() const {
    return 2500; // boxed per 5000ns, so everything within this box can be
    // associated.
  }
};

#endif

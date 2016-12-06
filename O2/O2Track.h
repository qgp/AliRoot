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
  typedef struct {
    timestamp_t Timestamp; /// Best estimate for the time of occurence.
    float Alpha;           /// Local <-->global coor.system rotation angle
    // NOTE: Is this the same as the vertex coordinate?
    float X; /// X coordinate for the point of parametrisation

    float Y;              /// local Y-coordinate of a track (cm)
    float Z;              /// local Z-coordinate of a track (cm)
    float SinPhi;         /// local sine of the track momentum azimuthal angle
    float TanLambda;      /// tangent of the track momentum dip angle
    float InversePt;      /// 1/pt (1/(GeV/c)), sign encodes charge.
    float Covariance[15]; /// The track parameter covariance matrix, does not
                          /// include x, alpha, or DetectionTime.
  } O2TrackData_t;

public:
  /// Default constructor
  O2Track(float Alpha = 0.0f, float X = 0.0f, float Y = 0.0f, float Z = 0.0f,
          float SinPhi = 0.0f, float TanLambda = 0.0f, float InversePt = 0.0f,
          const Double32_t *Covariance = nullptr,
          timestamp_t DetectionTime = 0.0f);
  O2Track(const AliVTrack *track, timestamp_t DetectionTime = 0.0f);
  O2Track(const AliExternalTrackParam *param, timestamp_t DetectionTime = 0.0f);
  /// Destructor
  virtual ~O2Track();

  timestamp_t getTimestamp() const { return mData.Timestamp; }
  /// Local <-->global coor.system rotation angle
  float GetAlpha() const { return mData.Alpha; }
  /// X coordinate for the point of parametrisation
  float GetX() const { return mData.X; }
  /// local Y-coordinate of a track (cm)
  float GetY() const { return mData.Y; }
  /// local Z-coordinate of a track (cm)
  float GetZ() const { return mData.Z; }
  /// local sine of the track momentum azimuthal angle
  float GetSinPhi() const { return mData.SinPhi; }
  /// tangent of the track momentum dip angle
  float GetTanLambda() const { return mData.TanLambda; }
  /// 1/pt (1/(GeV/c)), sign encodes charge.
  float GetInversePt() const { return mData.InversePt; }
  /// pt (GeV/c), sign encodes charge.
  float GetPt() const { return 1.0 / mData.InversePt; }
  virtual uint8_t *getData() { return (uint8_t *)&mData; }
  virtual size_t getDatasize() { return sizeof(mData); }

protected:
  void setParameters(const AliExternalTrackParam *param,
                     timestamp_t DetectionTime);
  void setParameters(float Alpha, float X, float Y, float Z, float SinPhi,
                     float TanLambda, float InversePt,
                     const Double32_t *Covariance, timestamp_t DetectionTime);

private:
  O2TrackData_t mData;
  friend class AliO2Track;
};
/// Global tracks
class O2GlobalTrack : public O2Track {
public:
  // c++11, inherit constructors
  using O2Track::O2Track;
  timestamp_t getTimestampResolution() const {
    return 600;
  } // 100ns guassian, 6 sigma.
};
/// ITS tracks
class O2ITSTrack : public O2Track {
  // c++11,  inherit constructors
  using O2Track::O2Track;

public:
  timestamp_t getTimestampResolution() const {
    return 2500; // boxed per 5000ns, so everything within this box can be
    // associated.
  }
};

#endif

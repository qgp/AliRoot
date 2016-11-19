/// \file AliO2Track.h
/// \author R.G.A. Deckers
/// \brief Definition of the AliO2Track class.
///
/// See implementation file for copyright details.

#ifndef ALINEWTRACK_H
#define ALINEWTRACK_H
// #ifndef __CINT__
// #include <cstdint>
// #endif
// TODO: coordinate system

/// Short AliO2Track description
///
/// More detailed AliO2Track description
class AliO2Track {
public:
  /// Default constructor
  AliO2Track(float TransverseMomentum = 0, float TransverseMomentumAnglePhi = 0,
             float TransverseMomentumAngleTheta = 0,
             float DistanceOfClosestApproachX = 0,
             float DistanceOfClosestApproachY = 0, float DetectionTime = 0.0f);
  /// Destructor
  ~AliO2Track();

protected:
  // protected stuff goes here

private:
  /// copy constructor prohibited
  // AliO2Track(const AliO2Track &);
  /// assignment operator prohibited
  // AliO2Track &operator=(const AliO2Track &);

  /// Helix specifications.
  /// TODO: Specify coordinate system
  // int16_t mPdg = -1;
  float mTransverseMomentum;
  float mTransverseMomentumAnglePhi;
  float mTransverseMomentumAngleTheta;
  // TODO: do we need these or can we just store the vertex?
  // float mDistanceOfClosestApproachX;
  // float mDistanceOfClosestApproachY;
  // TODO: covariance
  float covariances[(3 + 2 + 1) + 3 * 2];
  // float mDetectionTime;
};

#endif

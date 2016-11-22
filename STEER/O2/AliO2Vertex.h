/// \file AliO2Vertex.h
/// \author R.G.A. Deckers
/// \brief Definition of the AliO2Vertex class.
///
/// See implementation file for copyright details.

#ifndef ALIO2VERTEX_H
#define ALIO2VERTEX_H

class AliVParticle;
/// New Vertex class, still highly unspecified, is not a ROOT object.
class AliO2Vertex {
public:
  /// Default constructor
  AliO2Vertex(float positionX = 0, float positionY = 0, float positionZ = 0,
              float detectionTime = 0.0f);
  /// Default constructor
  AliO2Vertex(const AliVParticle *particle, float detectionTime = 0.0f);
  /// Destructor
  ~AliO2Vertex();
  float distanceFrom(const AliVParticle *particle);
  float distanceFrom(float x, float y, float z);
  float distanceSquaredFrom(const AliVParticle *particle);
  float distanceSquaredFrom(float x, float y, float z);

protected:
  // protected stuff goes here

private:
  /// copy constructor prohibited
  // AliO2Vertex(const AliO2Vertex &);
  /// assignment operator prohibited
  // AliO2Vertex &operator=(const AliO2Vertex &);
  // TODO: covariance
  float mPositionX;
  float mPositionY;
  float mPositionZ;
  float covariance[3 + 2 + 1];
  // float mDetectionTime;
};

#endif

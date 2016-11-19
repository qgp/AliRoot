/// \file AliO2Vertex.h
/// \author R.G.A. Deckers
/// \brief Definition of the AliO2Vertex class.
///
/// See implementation file for copyright details.

#ifndef ALINEWVERTEX_H
#define ALINEWVERTEX_H

/// Short AliO2Vertex description
///
/// More detailed AliO2Vertex description
class AliO2Vertex {
public:
  /// Default constructor
  AliO2Vertex(float positionX = 0, float positionY = 0, float positionZ = 0,
              float detectionTime = 0.0f);
  /// Destructor
  ~AliO2Vertex();

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

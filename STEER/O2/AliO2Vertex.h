/// \file AliO2Vertex.h
/// \author R.G.A. Deckers
/// \brief Definition of the AliO2Vertex class.
///
/// See implementation file for copyright details.

#ifndef ALIO2VERTEX_H
#define ALIO2VERTEX_H
#include "InterfaceTimestampped.h"
#include <TObject.h>
class AliVVertex;
/// New Vertex class, still highly unspecified, is not a ROOT object.
class AliO2Vertex : public TObject, public InterfaceTimestampped {
public:
  /// Default constructor
  AliO2Vertex(float positionX = 0, float positionY = 0, float positionZ = 0,
              timestamp_t detectionTime = 0.0f,
              timestamp_t detectionTimeResolution = 0.0f);
  /// Default constructor
  AliO2Vertex(const AliVVertex *vertex, timestamp_t detectionTime,
              timestamp_t detectionTimeResolution);
  /// Destructor
  ~AliO2Vertex();
  float distanceFrom(const AliVVertex *vertex);
  float distanceFrom(float x, float y, float z);
  float distanceSquaredFrom(const AliVVertex *vertex);
  float distanceSquaredFrom(float x, float y, float z);
  timestamp_t getTimestamp() const { return mTimestamp; }
  timestamp_t getTimestampResolution() const { return mTimestampResolution; }

protected:
  // protected stuff goes here

private:
  /// copy constructor prohibited
  // AliO2Vertex(const AliO2Vertex &);
  /// assignment operator prohibited
  // AliO2Vertex &operator=(const AliO2Vertex &);
  timestamp_t mTimestamp;
  timestamp_t mTimestampResolution;
  float mPositionX;
  float mPositionY;
  float mPositionZ;
  float covariance[3 + 2 + 1];
  ClassDef(AliO2Vertex, 1);
};

#endif

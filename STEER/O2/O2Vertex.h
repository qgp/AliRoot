/// \file O2Vertex.h
/// \author R.G.A. Deckers
/// \brief Definition of the O2Vertex class.
///
/// See implementation file for copyright details.

#ifndef O2VERTEX_H
#define O2VERTEX_H
#include "InterfaceTimestampped.h"

#include <cstddef>

class AliVVertex;
/// New Vertex class, still highly unspecified, is not a ROOT object.
class O2Vertex : public InterfaceTimestampped {
public:
  /// Default constructor
  O2Vertex(float positionX = 0, float positionY = 0, float positionZ = 0,
           timestamp_t detectionTime = 0.0f,
           timestamp_t detectionTimeResolution = 0.0f);
  /// Default constructor
  O2Vertex(const AliVVertex *vertex, timestamp_t detectionTime,
           timestamp_t detectionTimeResolution);
  /// Destructor
  ~O2Vertex();
  float distanceFrom(const AliVVertex *vertex);
  float distanceFrom(float x, float y, float z);
  float distanceSquaredFrom(const AliVVertex *vertex);
  float distanceSquaredFrom(float x, float y, float z);
  timestamp_t getTimestamp() const { return mTimestamp; }
  timestamp_t getTimestampResolution() const { return mTimestampResolution; }
  float GetX() const { return mPositionX; }
  float GetY() const { return mPositionY; }
  float GetZ() const { return mPositionZ; }
  /// Returns an element of the covariance matrix
  float GetCovariance(size_t at) const { return covariance[at]; }

private:
  // NOTE: Vertex timestamps are derived from contributing tracks. We probably
  // don't actually need to store these.
  timestamp_t mTimestamp;
  timestamp_t mTimestampResolution;

  float mPositionX;
  float mPositionY;
  float mPositionZ;
  // NOTE: Should this be extended to include the temporal coordinate?
  float covariance[3 + 2 + 1];
};

#endif

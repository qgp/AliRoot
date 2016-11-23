/// \file AliO2Event.h
/// \author R.G.A. Deckers
/// \brief Definition of the AliO2Event class.
///
/// See implementation file for copyright details.

#ifndef ALIO2EVENT_H
#define ALIO2EVENT_H

// Include the base classes
#include "InterfaceTimestampped.h"
#include <TObject.h>

#include "AliO2Track.h"
#include "AliO2Vertex.h"

#include <TBuffer.h>
#ifndef __CINT__
#include <cstdint>
#else
#include <stdint.h>
#endif

#include <vector>
/// Stores a single event in the new format. Still highly unspecified.
class AliO2Event : public TObject, public InterfaceTimestampped {
public:
  /// Default constructor
  AliO2Event();
  /// Destructor
  ~AliO2Event();

  /// Adds a vertex (of any type) to the event and returns the id of the vertex.
  int addVertex(const AliO2Vertex &vertex);
  /// Adds a track to the event coming from the vertex specified by the id
  void addTrackToVertex(const AliO2Track &track, int vertexIndex);
  // size_t GetNumberOfTracks() { return mTracks.size(); }
  // size_t GetNumberOfVertices() { return mVertices.size(); }
  timestamp_t getTimestamp() const { return mVertices[0].getTimestamp(); }

protected:
  // protected stuff goes here

private:
  std::vector<AliO2Vertex> mVertices;
  /// copy constructor prohibited
  // AliO2Event(const AliO2Event &);
  /// assignment operator prohibited
  // AliO2Event &operator=(const AliO2Event &);

  // root specific
  // void SerializeToBuffer(TBuffer &buffer);
  // void createFromBuffer(TBuffer &buffer);
  ClassDef(AliO2Event, 1);
};

#endif

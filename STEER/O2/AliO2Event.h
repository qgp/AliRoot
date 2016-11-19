/// \file AliO2Event.h
/// \author R.G.A. Deckers
/// \brief Definition of the AliO2Event class.
///
/// See implementation file for copyright details.

#ifndef ALINEWEVENT_H
#define ALINEWEVENT_H

// Include the base classes
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
/// Short AliO2Event description
///
/// More detailed AliO2Event description

class AliO2Event : public TObject {
public:
  /// Default constructor
  AliO2Event();
  /// Destructor
  ~AliO2Event();

  int addVertex(const AliO2Vertex &vertex);
  void addTrackToVertex(const AliO2Track &track, int vertexIndex);
  size_t GetNumberOfTracks() { return mTracks.size(); }
  size_t GetNumberOfVertices() { return mVertices.size(); }

protected:
  // protected stuff goes here

private:
  /// copy constructor prohibited
  // AliO2Event(const AliO2Event &);
  /// assignment operator prohibited
  // AliO2Event &operator=(const AliO2Event &);
  // Keep the number of vertices seperate from the node to increase
  // coherency,
  // and easier to implement win-win
  std::vector<uint8_t> mNumberOfTracksPerVertex;
  std::vector<AliO2Vertex> mVertices;
  std::vector<AliO2Track> mTracks;
  // root specific
  void SerializeToBuffer(TBuffer &buffer);
  void createFromBuffer(TBuffer &buffer);
  ClassDef(AliO2Event, 1);
};

#endif

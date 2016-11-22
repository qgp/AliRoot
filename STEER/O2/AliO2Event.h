/// \file AliO2Event.h
/// \author R.G.A. Deckers
/// \brief Definition of the AliO2Event class.
///
/// See implementation file for copyright details.

#ifndef ALIO2EVENT_H
#define ALIO2EVENT_H

// Include the base classes
#include <TObject.h>

#include "AliO2Track.h"
#include "AliO2Vertex.h"

class AliAODEvent;
class AliESDEvent;
class AliVParticle;
class AliVVertex;
class AliVEvent;

#include <TBuffer.h>
#ifndef __CINT__
#include <cstdint>
#else
#include <stdint.h>
#endif
#include <vector>
/// Stores a single event in the new format. Still highly unspecified.
class AliO2Event : public TObject {
public:
  /// Default constructor
  AliO2Event();
  /// Construct from an old AOD event
  AliO2Event(const AliAODEvent *);
  /// Construct from an old ESD event
  AliO2Event(const AliESDEvent *);
  /// Destructor
  ~AliO2Event();

  int findNearestVertex(const AliVParticle *particle);
  /// Adds a vertex (of any type) to the event and returns the id of the vertex.
  int addVertex(const AliO2Vertex &vertex);
  /// Adds a track to the event coming from the vertex specified by the id
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
  void finishBuildingFromMapping(std::vector<int> indices,
                                 const AliVEvent *event);
  // Keep the number of vertices seperate from the node to increase
  // coherency,
  // and easier to implement win-win
  std::vector<uint16_t> mNumberOfTracksPerVertex; // TODO: how small can
                                                  // this
  // be? primary vertex seems to have most or all tracks
  std::vector<AliO2Vertex> mVertices;
  std::vector<AliO2Track> mTracks;
  // root specific
  void SerializeToBuffer(TBuffer &buffer);
  void createFromBuffer(TBuffer &buffer);
  ClassDef(AliO2Event, 1);
};

#endif

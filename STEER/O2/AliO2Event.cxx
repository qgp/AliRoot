/// \file AliO2event->cxx
/// \brief implementation of the AliO2Event class.
/// \since 2016-11-15
/// \author R.G.A. Deckers
/// \copyright
///  This program is free software; you can redistribute it and/or
/// modify it under the terms of the GNU General Public License as
/// published by the Free Software Foundation; either version 3 of
/// the License, or (at your option) any later version.
/// This program is distributed in the hope that it will be useful, but
/// WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
/// General Public License for more details at
/// https://www.gnu.org/copyleft/gpl.html
///

#include "AliO2Event.h"
#include <AliAODEvent.h>
#include <AliAODTrack.h>
#include <AliESDEvent.h>
#include <AliESDcascade.h>
#include <AliESDtrack.h>
#include <AliLog.h>
#include <AliVVertex.h>
#include <unordered_map>
#include <vector>
// root specific
ClassImp(AliO2Event);

// default constructor
AliO2Event::AliO2Event() {}

/// Construct an AliO2Event from an old AliAODEvent (takes a bit of work)
AliO2Event::AliO2Event(const AliESDEvent *event) {
  // For ESDs:
  // grab each veretx. (primary, V0, cascade)
  // for each track, find the closest vertex. Associate it
  // for each vertex with tracks, write the output.
  // The all that's left is to find the producing track for each vertex
  //  Seems to be very hard to do. Vertices contain no reference to their
  //  producer, tracks do not contain their end.
  //  Possibly extrapolate from inner particles to outer vertices using
  //  extrapolatetoDCA, then try to minimize the total DCA

  // There are two(?) types of vertices, V0s and Cascades
  // TODO: check if V0s include the primaries/primary. ESD *seem* to only have 1
  // TODO: Add kinks as well?
  // primary in any case.
  /// Start by adding all vertices to the the event
  size_t numberOfV0s = event->GetNumberOfV0s();
  size_t numberOfCascades = event->GetNumberOfCascades();
  size_t totalNumberOfVertices = numberOfV0s + numberOfCascades;
  for (int i = 0; i < numberOfV0s; i++) {
    addVertex(event->GetV0(i));
  }
  for (int i = 0; i < numberOfCascades; i++) {
    addVertex(event->GetCascade(i));
  }

  /// Now we will look for the closes vertex for each track
  size_t numberOfTracks = event->GetNumberOfTracks();
  std::vector<int> indices(numberOfTracks, 0);
  mNumberOfTracksPerVertex.resize(totalNumberOfVertices, 0);
  mTracks.resize(numberOfTracks);

  for (size_t u = 0; u < numberOfTracks; u++) {
    AliVParticle *particle = event->GetTrack(u);
    // Get the index of the nearest vertex
    // TODO: replace this by a barnes-hut implementation for massive speedup
    indices[u] = findNearestVertex(particle);
    // don't add the track yet but keep track of how many tracks each vertex
    // would get
    mNumberOfTracksPerVertex[indices[u]] += 1;
  }
  finishBuildingFromMapping(indices, event);
}

/// Construct an AliO2Event from an old AliAODEvent (takes a bit of work)
AliO2Event::AliO2Event(const AliAODEvent *event) {
  std::unordered_map<AliVVertex *, int> vertexIndexMapping;
  size_t totalNumberOfVertices = event->GetNumberOfVertices();

  // start by adding all the vertices.
  for (int i = 0; i < totalNumberOfVertices; i++) {
    AliVVertex *vertex = event->GetVertex(i);
    vertexIndexMapping[vertex] =
        addVertex(AliO2Vertex(vertex->GetX(), vertex->GetY(), vertex->GetZ()));
  }

  /// Now we will look for the producing vertex for each track
  size_t numberOfTracks = event->GetNumberOfTracks();
  std::vector<int> indices(numberOfTracks);
  mTracks.resize(numberOfTracks);

  for (size_t u = 0; u < numberOfTracks; u++) {
    AliAODTrack *particle = dynamic_cast<AliAODTrack *>(event->GetTrack(u));
    // Get the index into mVertices of the producing
    indices[u] = vertexIndexMapping
        [particle->GetProdVertex()]; // findNearestVertex(particle);
    // don't add the track yet but keep track of how many tracks each vertex
    // would get
    mNumberOfTracksPerVertex[indices[u]] += 1;
  }
  finishBuildingFromMapping(indices, event);
}

// default destructor
AliO2Event::~AliO2Event() {}

void AliO2Event::finishBuildingFromMapping(std::vector<int> indices,
                                           const AliVEvent *event) {
  size_t totalNumberOfVertices = mVertices.size();
  size_t numberOfTracks = event->GetNumberOfTracks();
  // Compute offsets into the track array for each vertex
  std::vector<unsigned> offsetIntoTracks(totalNumberOfVertices);
  offsetIntoTracks[0] = 0;
  unsigned nextOffset = mNumberOfTracksPerVertex[0];
  for (size_t u = 1; u < totalNumberOfVertices; u++) {
    if (0 == mNumberOfTracksPerVertex[u]) {
      continue;
    } else { // this vertex has tracks associated to it
      offsetIntoTracks[u] = nextOffset;
      nextOffset += mNumberOfTracksPerVertex[u];
    }
  }
  for (size_t u = 0; u < numberOfTracks; u++) {
    // get our precomputed vertex<=>track mapping
    int vertexIndex = indices[u];
    // See how far into the track array the children of this vertex start
    unsigned offset = offsetIntoTracks[vertexIndex];
    // Create a O2Track and add the track to the specified location
    mTracks[offset] =
        AliO2Track(reinterpret_cast<const AliVTrack *>(event->GetTrack(u)));
    // Now this cast is dangerous and should not be
    // used. BUT AliAODEvent overwrites GetTrack to
    // return an AliAODTrack which inherits from
    // AliVTrack. For ESDs however, it does not but it
    // inherits from AliExternalTrackParam which
    // does... This constructor is then a bit
    // roundabout, but it works. Neither AODs or ESDs
    // overwrite the getVTracks function... Ideally we should implement those
    // and then use GetVTrack

    // increase our offset by one so that the next track gets put in the
    // correct location.
    offsetIntoTracks[vertexIndex] += 1;
  }
  /// Remove vertices without any tracks
  size_t write_location = 0;
  for (size_t u = 0; u < totalNumberOfVertices; u++) {
    if (mNumberOfTracksPerVertex[u] == 0) {
      continue;
    } else {
      mNumberOfTracksPerVertex[write_location] = mNumberOfTracksPerVertex[u];
      mVertices[write_location] = mVertices[u];
      write_location += 1;
    }
  }
  // write location now contains the amount of vertices with at least one
  // track associated with them.
  totalNumberOfVertices = write_location;
  // resize to match the new sizes
  mNumberOfTracksPerVertex.resize(totalNumberOfVertices);
  mVertices.resize(totalNumberOfVertices);
}

int AliO2Event::findNearestVertex(const AliVParticle *particle) {
  double max_sq_distance = 1e200; // TODO: should be FLT_MAX
  int index = -1;
  for (int i = 0; i < mVertices.size(); i++) {
    float sq_distance = mVertices[i].distanceSquaredFrom(particle);
    if (sq_distance < max_sq_distance) {
      index = i;
      max_sq_distance = sq_distance;
    }
  }
  return index;
}

/// Adds a vertex (of any type) to event and returns the index
int AliO2Event::addVertex(const AliO2Vertex &vertex) {
  mNumberOfTracksPerVertex.push_back(0);
  mVertices.push_back(vertex);
  return mNumberOfTracksPerVertex.size() - 1;
}

// TODO: exception handling, everywhere really
/// Adds a track to the structure, using the corresponding vertex for specifying
/// the origin.
void AliO2Event::addTrackToVertex(const AliO2Track &track, int vertexIndex) {
  if (vertexIndex ==
      mNumberOfTracksPerVertex.size() - 1) { // we are at the end, easy.
    mTracks.push_back(track);
    mNumberOfTracksPerVertex[vertexIndex] += 1;
  } else { // oh boy, pushing to the middle of the tracks array...
    int trackInsertionPoint = 0;
    for (int i = 0; i <= vertexIndex; i++) {
      trackInsertionPoint += mNumberOfTracksPerVertex[i];
    }
    // very innefficient, moves all tracks after the insertion point by one.
    mTracks.insert(mTracks.begin() + trackInsertionPoint, track);
    mNumberOfTracksPerVertex[vertexIndex] += 1;
  }
}

// void AliO2Event::Streamer(TBuffer &buffer) {
//   // Stream an object of class AliO2event->
//   if (buffer.IsReading()) {
//     createFromBuffer(buffer);
//   } else {
//     SerializeToBuffer(buffer);
//   }
// }

/// Used when streaming from a TBuffer.
void AliO2Event::createFromBuffer(TBuffer &buffer) {
  UInt_t start, byteCountPosition;
  buffer.ReadVersion(&start, &byteCountPosition);
  Int_t byte_size;
  Int_t element_count;
  UChar_t *vector_start;

  buffer >> byte_size;
  element_count = byte_size / sizeof(mNumberOfTracksPerVertex[0]);
  mNumberOfTracksPerVertex.resize(element_count);
  vector_start = reinterpret_cast<UChar_t *>(mNumberOfTracksPerVertex.data());
  buffer.ReadFastArray(vector_start, byte_size);

  buffer >> byte_size;
  element_count = byte_size / sizeof(mVertices[0]);
  mVertices.resize(element_count);
  vector_start = reinterpret_cast<UChar_t *>(mVertices.data());
  buffer.ReadFastArray(vector_start, byte_size);

  buffer >> byte_size;
  element_count = byte_size / sizeof(mTracks[0]);
  mTracks.resize(element_count);
  vector_start = reinterpret_cast<UChar_t *>(mTracks.data());
  buffer.ReadFastArray(vector_start, byte_size);
  buffer.CheckByteCount(start, byteCountPosition, AliO2Event::IsA());
}

/// Used when streaming to a TBuffer.
void AliO2Event::SerializeToBuffer(TBuffer &buffer) {
  UInt_t byteCountPosition = buffer.WriteVersion(AliO2Event::IsA(), kTRUE);

  UChar_t *vector_start =
      reinterpret_cast<UChar_t *>(mNumberOfTracksPerVertex.data());
  Int_t byte_size =
      sizeof(mNumberOfTracksPerVertex[0]) * mNumberOfTracksPerVertex.size();
  buffer.WriteArray(vector_start, byte_size);

  vector_start = reinterpret_cast<UChar_t *>(mVertices.data());
  byte_size = sizeof(mVertices[0]) * mVertices.size();
  buffer.WriteArray(vector_start, byte_size);

  vector_start = reinterpret_cast<UChar_t *>(mTracks.data());
  byte_size = sizeof(mTracks[0]) * mTracks.size();
  buffer.WriteArray(vector_start, byte_size);

  buffer.SetByteCount(byteCountPosition, kTRUE);
}

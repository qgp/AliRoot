/// \file AliO2Event.cxx
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
#include <AliLog.h>
#include <vector>
// root specific
ClassImp(AliO2Event);

// default constructor
AliO2Event::AliO2Event() {}

// default destructor
AliO2Event::~AliO2Event() {}

/// Adds a vertex (of any type) to event and returns the index
int AliO2Event::addVertex(const AliO2Vertex &vertex) {
  mNumberOfTracksPerVertex.push_back(0);
  mVertices.push_back(vertex);
  return mNumberOfTracksPerVertex.size();
}

// TODO: exception handling, everywhere really
/// Adds a track to the structure, using the corresponding vertex for specifying
/// the origin.
void AliO2Event::addTrackToVertex(const AliO2Track &track, int vertexIndex) {
  if (vertexIndex ==
      mNumberOfTracksPerVertex.size() - 1) { // we are at the end, easy.
    mTracks.push_back(track);
    mNumberOfTracksPerVertex[vertexIndex] += 1;
  } else { // oh boy, pushing to the middle of the tracks array... //TODO
           // optimize the
    // structure to speed this up when we're still building the AoD, or use a
    // builderClass
    int trackInsertionPoint = 0;
    for (int i = 0; i <= vertexIndex; i++) {
      trackInsertionPoint += mNumberOfTracksPerVertex[i];
    }
    // very innefficient, moves all tracks after the insertion point by one.
    mTracks.insert(mTracks.begin() + trackInsertionPoint, track);
    mNumberOfTracksPerVertex[vertexIndex] += 1;
  }
}

void AliO2Event::Streamer(TBuffer &buffer) {
  // Stream an object of class AliO2Event.
  if (buffer.IsReading()) {
    createFromBuffer(buffer);
  } else {
    SerializeToBuffer(buffer);
  }
}

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

  UChar_t *vector_start = mNumberOfTracksPerVertex.data();
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

/// \file O2Timeframe.cxx
/// \brief implementation of the O2Timeframe class.
/// \since 2016-11-21
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

#include "O2Timeframe.h"
#include "O2Event.h"
#include <AliESDEvent.h>
#include <TRandom2.h>
#include <algorithm>
#include <fstream>
#include <iostream>
// root specific
ClassImp(O2Timeframe);

// default constructor
O2Timeframe::O2Timeframe() {}

// default destructor
O2Timeframe::~O2Timeframe() {}

/// Produces an event corresponding to the 'index'ed vertex.
///
/// It's not very pretty right now, or optimized but it is for testing purposes.
/// \param index The index of the corresponding primary vertex. is not bounds
/// checked
/// \return Returns an O2Event object which consists of pointers into this
/// timeframe.
O2Event O2Timeframe::getEvent(size_t index) {
  // TODO: bounds checking
  O2Vertex *vertex = mVertices.data() + index;
  timestamp_t eventTime = vertex->getTimestamp();
  O2GlobalTrack *global_track_array = NULL;
  size_t size_global = 0;
  size_t offset_unambigous_global = 0;
  size_t size_unambigous_global = 0;
  if (mGlobalTracks.size()) {
    timestamp_t eventTimeDelta =
        vertex->getTimestampResolution() +
        mGlobalTracks.begin()->getTimestampResolution();
    // TODO:  ROOT6 - > auto
    typedef std::vector<O2GlobalTrack>::iterator globalIt_t;
    // Points to the first one
    globalIt_t globalItBegin = std::lower_bound(
        mGlobalTracks.begin(), mGlobalTracks.end(), eventTime - eventTimeDelta);
    // points to the first element that's *above* threshold (or last element if
    // none exist)
    globalIt_t globalItEnd = std::upper_bound(
        globalItBegin, mGlobalTracks.end(), eventTime + eventTimeDelta);
    globalIt_t globalItUnambigousBegin = globalItBegin;
    globalIt_t globalItUnambigousEnd = globalItEnd;
    // if there is is an event before us check when it overlaps
    if (0 != index) {
      O2Vertex *vertex = mVertices.data() + index - 1;
      timestamp_t eventTime = vertex->getTimestamp();
      timestamp_t eventTimeDelta =
          vertex->getTimestampResolution() +
          mGlobalTracks.begin()->getTimestampResolution();
      // find the overlapping bound, points to the element past
      globalItUnambigousBegin = std::upper_bound(globalItBegin, globalItEnd,
                                                 eventTime + eventTimeDelta);
    }
    // if there is an event after this one in this frame
    if (mVertices.size() - 1 != index) {
      O2Vertex *vertex = mVertices.data() + index + 1;
      timestamp_t eventTime = vertex->getTimestamp();
      timestamp_t eventTimeDelta =
          vertex->getTimestampResolution() +
          mGlobalTracks.begin()->getTimestampResolution();
      globalItUnambigousEnd = std::lower_bound(globalItBegin, globalItEnd,
                                               eventTime - eventTimeDelta);
    }
    // TODO: when begin points past end?
    // Now we have 4 iterators, identifying the slice of associated tracks, and
    // the slice which is unambigously mapped to us.
    global_track_array = &(*globalItBegin);
    size_global = std::distance(globalItBegin, globalItEnd);
    offset_unambigous_global =
        std::distance(globalItBegin, globalItUnambigousBegin);
    size_unambigous_global =
        std::distance(globalItUnambigousBegin, globalItUnambigousEnd);
  }

  O2ITSTrack *ITS_track_array = NULL;
  size_t size_ITS = 0;
  size_t offset_unambigous_ITS = 0;
  size_t size_unambigous_ITS = 0;
  if (mITSTracks.size()) {
    timestamp_t eventTimeDelta = vertex->getTimestampResolution() +
                                 mITSTracks.begin()->getTimestampResolution();
    // TODO:  ROOT6 - > auto

    typedef std::vector<O2ITSTrack>::iterator ITSIt_t;
    // Points to the first one
    ITSIt_t ITSItBegin = std::lower_bound(mITSTracks.begin(), mITSTracks.end(),
                                          eventTime - eventTimeDelta);
    // points to the first element that's *above* threshold (or last element if
    // none exist)
    ITSIt_t ITSItEnd = std::upper_bound(ITSItBegin, mITSTracks.end(),
                                        eventTime + eventTimeDelta);
    ITSIt_t ITSItUnambigousBegin = ITSItBegin;
    ITSIt_t ITSItUnambigousEnd = ITSItEnd;
    // if there is is an event before us check when it overlaps
    if (0 != index) {
      O2Vertex *vertex = mVertices.data() + index - 1;
      timestamp_t eventTime = vertex->getTimestamp();
      timestamp_t eventTimeDelta = vertex->getTimestampResolution() +
                                   mITSTracks.begin()->getTimestampResolution();
      // find the overlapping bound, points to the element past
      ITSItUnambigousBegin =
          std::upper_bound(ITSItBegin, ITSItEnd, eventTime + eventTimeDelta);
    }
    // if there is an event after this one in this frame
    if (mVertices.size() - 1 != index) {
      O2Vertex *vertex = mVertices.data() + index + 1;
      timestamp_t eventTime = vertex->getTimestamp();
      timestamp_t eventTimeDelta = vertex->getTimestampResolution() +
                                   mITSTracks.begin()->getTimestampResolution();
      ITSItUnambigousEnd =
          std::lower_bound(ITSItBegin, ITSItEnd, eventTime - eventTimeDelta);
    }
    // TODO: when begin points past end?
    // Now we have 4 iterators, identifying the slice of associated tracks, and
    // the
    // slice which is unambigously mapped to us.
    ITS_track_array = &(*ITSItBegin);
    size_ITS = std::distance(ITSItBegin, ITSItEnd);
    offset_unambigous_ITS = std::distance(ITSItBegin, ITSItUnambigousBegin);
    size_unambigous_ITS =
        std::distance(ITSItUnambigousBegin, ITSItUnambigousEnd);
  }
  // ugly... just like that ^^^^``
  return O2Event(vertex, global_track_array, size_global,
                 offset_unambigous_global, size_unambigous_global,
                 ITS_track_array, size_ITS, offset_unambigous_ITS,
                 size_unambigous_ITS);
}

/// Converts and adds an old ESD event to the timeframe with the given timestamp
void O2Timeframe::addEvent(const AliESDEvent *event, timestamp_t timestampNs) {
  // Random number generator for timestamps
  TRandom2 rng;
  int numberOfTracks = event->GetNumberOfTracks();
  // if we have no tracks in this event don't bother
  if (0 == numberOfTracks) {
    return;
  }
  // TODO: ROOT6 -> auto
  size_t globalTracksOldSize = mGlobalTracks.size();
  size_t ITSTracksOldSize = mITSTracks.size();
  size_t verticesOldSize = mVertices.size();

  // Get the primary vertex of this event
  mVertices.push_back(O2Vertex(
      event->GetVertex(), rng.Gaus(timestampNs, 100.0 / sqrt(numberOfTracks)),
      600.0 / sqrt(numberOfTracks))); // resolution of 6 sigma
  // get all the tracks
  for (int i = 0; i < numberOfTracks; i++) {
    AliESDtrack *track = event->GetTrack(i);
    // Check if this track is global or ITS
    if (NULL == track->GetInnerParam()) {
      // ITS track, box the time in bins of 5000ns and assign the central time
      mITSTracks.push_back(
          O2ITSTrack(track, 5000.0 * floor(timestampNs / 5000.0) + 2500));
    } else {
      // global track, guass of width 100ns
      mGlobalTracks.push_back(O2GlobalTrack(track, rng.Gaus(timestampNs, 100)));
    }
  }
  // TODO: vertices shouldn't really need to be sorted, only one can get added
  // per function call anyway
  // TODO: write a fast sorter which take the Guassian nature into account.

  // Sort the new slices
  std::sort(mGlobalTracks.begin() + globalTracksOldSize, mGlobalTracks.end());
  // merge the old and new slices
  std::inplace_merge(mGlobalTracks.begin(),
                     mGlobalTracks.begin() + globalTracksOldSize,
                     mGlobalTracks.end());
  std::sort(mITSTracks.begin() + ITSTracksOldSize, mITSTracks.end());
  std::inplace_merge(mITSTracks.begin(), mITSTracks.begin() + ITSTracksOldSize,
                     mITSTracks.end());
  std::sort(mVertices.begin() + verticesOldSize, mVertices.end());
  std::inplace_merge(mVertices.begin(), mVertices.begin() + verticesOldSize,
                     mVertices.end());
}

void O2Timeframe::Streamer(TBuffer &buffer) {
  // Stream an object of class AliO2Event.
  if (buffer.IsReading()) {
    createFromBuffer(buffer);
  } else {
    SerializeToBuffer(buffer);
  }
}

void O2Timeframe::createFromBuffer(TBuffer &buffer) {
  UInt_t start, byteCountPosition;
  buffer.ReadVersion(&start, &byteCountPosition);
  // Int_t byte_size;
  Int_t element_count;
  // UChar_t *vector_start;

  buffer >> element_count;
  mVertices.resize(element_count);
  for (Int_t i = 0; i < element_count; i++) {
    float x;
    buffer >> x;
    float y;
    buffer >> y;
    float z;
    buffer >> z;
    timestamp_t ts;
    buffer >> ts;
    timestamp_t tsr;
    buffer >> tsr;
    float cov[7];
    for (int j = 0; j < 6; j++) {
      buffer >> cov[j];
    }
    mVertices[i] = O2Vertex(x, y, z, ts, tsr);
  }

  buffer >> element_count;
  mGlobalTracks.resize(element_count);
  for (Int_t i = 0; i < element_count; i++) {
    buffer.ReadFastArray(mGlobalTracks[i].getData(),
                         mGlobalTracks[i].getDatasize());
  }

  buffer >> element_count;
  mITSTracks.resize(element_count);
  for (Int_t i = 0; i < element_count; i++) {
    buffer.ReadFastArray(mITSTracks[i].getData(), mITSTracks[i].getDatasize());
  }
  buffer.CheckByteCount(start, byteCountPosition, O2Timeframe::IsA());
}

void O2Timeframe::SerializeToBuffer(TBuffer &buffer) {
  UInt_t byteCountPosition = buffer.WriteVersion(O2Timeframe::IsA(), kTRUE);
  Int_t element_count;

  element_count = mVertices.size();
  buffer << element_count;
  for (Int_t i = 0; i < element_count; i++) {
    buffer << mVertices[i].GetX();
    buffer << mVertices[i].GetY();
    buffer << mVertices[i].GetZ();
    buffer << mVertices[i].getTimestamp();
    buffer << mVertices[i].getTimestampResolution();
    for (int j = 0; j < 6; j++) {
      buffer << mVertices[j].GetCovariance(j);
    }
  }

  element_count = mGlobalTracks.size();
  buffer << element_count;
  for (Int_t i = 0; i < element_count; i++) {
    buffer.WriteFastArray(mGlobalTracks[i].getData(),
                          mGlobalTracks[i].getDatasize());
  }

  element_count = mITSTracks.size();
  buffer << element_count;
  for (Int_t i = 0; i < element_count; i++) {
    buffer.WriteFastArray(mITSTracks[i].getData(), mITSTracks[i].getDatasize());
  }
  buffer.SetByteCount(byteCountPosition, kTRUE);
}

void O2Timeframe::WriteToFile(const std::string &filename) {
  using namespace std;
  ofstream buffer;
  buffer.open(filename, ios::out | ios::trunc | ios::binary);
  Int_t element_count;
  element_count = mVertices.size();
  buffer << element_count;
  for (Int_t i = 0; i < element_count; i++) {
    buffer << mVertices[i].GetX();
    buffer << mVertices[i].GetY();
    buffer << mVertices[i].GetZ();
    buffer << mVertices[i].getTimestamp();
    buffer << mVertices[i].getTimestampResolution();
    for (int j = 0; j < 6; j++) {
      buffer << mVertices[j].GetCovariance(j);
    }
  }

  element_count = mGlobalTracks.size();
  buffer << element_count;
  for (Int_t i = 0; i < element_count; i++) {
    // C style cast, ssssh, don't tell Mikolaj
    buffer.write((const char *)mGlobalTracks[i].getData(),
                 mGlobalTracks[i].getDatasize());
  }

  element_count = mITSTracks.size();
  buffer << element_count;
  for (Int_t i = 0; i < element_count; i++) {
    buffer.write((const char *)mITSTracks[i].getData(),
                 mITSTracks[i].getDatasize());
  }
  buffer.close();
}

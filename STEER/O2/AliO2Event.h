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
/// Wrapper class which exposes a slice of a timeframe representing a single
/// collision. Pileups are currently not handled but can be added.
class AliO2Event : public TObject, public InterfaceTimestampped {
public:
  /// Default constructor
  AliO2Event();
  // TODO: should make it so that this is hidden in a way (perhaps make it a
  // protected friend with timeframe).
  /// Explicit constructor
  AliO2Event(AliO2Vertex *Vertex, AliO2GlobalTrack *GlobalTracks,
             size_t NumberOfGlobalTracks, size_t UnambigousGlobalTracksOffset,
             size_t UnambigousGlobalTracksSize, AliO2ITSTrack *ITSTracks,
             size_t NumberOfITSTracks, size_t UnambigousITSTracksOffset,
             size_t UnambigousITSTracksSize);
  /// Destructor
  ~AliO2Event();
  timestamp_t getTimestamp() const { return mVertex->getTimestamp(); }
  timestamp_t getTimestampResolution() const {
    return mVertex->getTimestampResolution();
  }
  size_t GetNumberOfTracks() const {
    return GetNumberOfGlobalTracks() + GetNumberOfITSTracks();
  }
  size_t GetNumberOfUnambigousTracks() const {
    return GetNumberOfUnambigousGlobalTracks() +
           GetNumberOfUnambigousITSTracks();
  }
  size_t GetNumberOfAmbigousTracks() const {
    return GetNumberOfAmbigousGlobalTracks() + GetNumberOfAmbigousITSTracks();
  }
  size_t GetNumberOfGlobalTracks() const { return mNumberOfGlobalTracks; }
  size_t GetNumberOfUnambigousGlobalTracks() const {
    return mUnambigousGlobalTracksSize;
  }
  size_t GetNumberOfAmbigousGlobalTracks() const {
    return GetNumberOfGlobalTracks() - GetNumberOfUnambigousGlobalTracks();
  }
  size_t GetNumberOfITSTracks() const { return mNumberOfITSTracks; }
  size_t GetNumberOfUnambigousITSTracks() const {
    return mUnambigousITSTracksSize;
  }
  size_t GetNumberOfAmbigousITSTracks() const {
    return GetNumberOfITSTracks() - GetNumberOfUnambigousITSTracks();
  }
  AliO2Vertex *GetVertex() const { return mVertex; }
  AliO2Track *GetGlobalTrack(size_t index) const {
    return (index < mNumberOfGlobalTracks ? mGlobalTracks + index : NULL);
  }
  AliO2Track *GetITSTrack(size_t index) const {
    return (index < mNumberOfITSTracks ? mITSTracks + index : NULL);
  }
  AliO2Track *GetTrack(size_t index) const {
    if (index >= GetNumberOfTracks()) {
      return NULL;
    }
    return (index < mNumberOfGlobalTracks
                ? mGlobalTracks + index
                : mITSTracks + index - mNumberOfGlobalTracks);
  }

protected:
  // protected stuff goes here

private:
  /// Pointer to the vertex in our timeframe
  AliO2Vertex *mVertex; // ->

  /// pointer to the first associated global track.  Can have ambigous tracks at
  /// the start and the end, but any unambigous tracks will be stored in the
  /// center. Sorted by time.
  AliO2Track *mGlobalTracks; //[mNumberOfGlobalTracks]
  /// Total amount of global tracks
  size_t mNumberOfGlobalTracks;
  /// How far from the start of mGlobalTracks do unambigous  global tracks
  /// start.
  size_t mUnambigousGlobalTracksOffset;
  /// How many unambigous global tracks do we have.
  size_t mUnambigousGlobalTracksSize;

  /// pointer to the first associated ITS track.  Can have ambigous tracks at
  /// the start and the end, but any unambigous tracks will be stored in the
  /// center. Sorted by time.
  AliO2Track *mITSTracks; //[mNumberOfITSTracks]
  /// Total amount of ITS tracks
  size_t mNumberOfITSTracks;
  /// How far from the start of mITSTracks do unambigous  ITS tracks
  /// start.
  size_t mUnambigousITSTracksOffset;
  /// How many unambigous ITS tracks do we have
  size_t mUnambigousITSTracksSize;

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

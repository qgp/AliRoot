/// \file O2Event.h
/// \author R.G.A. Deckers
/// \brief Definition of the O2Event class.
///
/// See implementation file for copyright details.

#ifndef O2EVENT_H
#define O2EVENT_H

// Include the base classes
#include "InterfaceTimestampped.h"

#include "O2Track.h"
#include "O2Vertex.h"

#include <TBuffer.h>
#include <stdint.h>

#include <vector>
/// Wrapper class which exposes a slice of a timeframe representing a single
/// collision. Pileups are currently not handled but can be added.
class O2Event : public InterfaceTimestampped {
public:
  /// Default constructor
  O2Event();
  // TODO: should make it so that this is hidden in a way (perhaps make it a
  // protected friend with timeframe).
  /// Explicit constructor
  O2Event(O2Vertex *Vertices, size_t NumberOfVertices,
          O2GlobalTrack *GlobalTracks, size_t NumberOfGlobalTracks,
          size_t UnambigousGlobalTracksOffset,
          size_t UnambigousGlobalTracksSize, O2ITSTrack *ITSTracks,
          size_t NumberOfITSTracks, size_t UnambigousITSTracksOffset,
          size_t UnambigousITSTracksSize);
  /// Destructor
  ~O2Event();
  timestamp_t getTimestamp() const { return mVertices->getTimestamp(); }
  timestamp_t getTimestampResolution() const {
    return mVertices->getTimestampResolution();
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
  O2Vertex *GetVertex() const { return mVertices; }
  O2GlobalTrack *GetGlobalTrack(size_t index) const {
    return (index < mNumberOfGlobalTracks ? mGlobalTracks + index : NULL);
  }
  O2ITSTrack *GetITSTrack(size_t index) const {
    return (index < mNumberOfITSTracks ? mITSTracks + index : NULL);
  }

  O2Track *GetTrack(size_t index) const {
    if (index >= GetNumberOfTracks()) {
      return NULL;
    }
    return (index < mNumberOfGlobalTracks
                ? static_cast<O2Track *>(mGlobalTracks + index)
                : static_cast<O2Track *>(mITSTracks + index -
                                         mNumberOfGlobalTracks));
  }

protected:
  // protected stuff goes here

private:
  /// Pointer to the vertex in our timeframe
  O2Vertex *mVertices; // ->
  size_t mNumberOfVertices;
  /// pointer to the first associated global track.  Can have ambigous tracks at
  /// the start and the end, but any unambigous tracks will be stored in the
  /// center. Sorted by time.
  O2GlobalTrack *mGlobalTracks; //[mNumberOfGlobalTracks]
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
  O2ITSTrack *mITSTracks; //[mNumberOfITSTracks]
  /// Total amount of ITS tracks
  size_t mNumberOfITSTracks;
  /// How far from the start of mITSTracks do unambigous  ITS tracks
  /// start.
  size_t mUnambigousITSTracksOffset;
  /// How many unambigous ITS tracks do we have
  size_t mUnambigousITSTracksSize;

  /// copy constructor prohibited
  // O2Event(const O2Event &);
  /// assignment operator prohibited
  // O2Event &operator=(const O2Event &);
  friend class AliO2Event;
};

#endif

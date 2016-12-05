/// \file O2Timeframe.h
/// \author R.G.A. Deckers
/// \brief Definition of the O2Timeframe class.
///
/// See implementation file for copyright details.

#ifndef O2TIMEFRAME_H
#define O2TIMEFRAME_H

class O2Event;
class AliESDEvent;
//#include "O2Event.h"
#include "O2Track.h"
#include "O2Vertex.h"

#include <TObject.h>
#include <stdint.h>
#include <vector>
/// A Class which represent a timeslice consisting of O2Events.
///
/// Very simple right now.
class O2Timeframe : public TObject {
public:
  /// Default constructor
  O2Timeframe();
  /// Destructor
  ~O2Timeframe();
  // Reads an ESD event and adds it to the timeframe with the given timestamp
  // (in ns)
  void addEvent(const AliESDEvent *event, timestamp_t timestampNs);
  size_t getNumberOfEvents() const { return mVertices.size(); }
  O2Event getEvent(size_t index);
  void WriteToFile(const std::string &filename);

protected:
  // protected stuff goes here

private:
  // A simple container, timespam and event.
  std::vector<O2Vertex> mVertices; /// The vertices tracks link to
  std::vector<O2GlobalTrack> mGlobalTracks;
  std::vector<O2ITSTrack> mITSTracks;

  //  root specific
  void SerializeToBuffer(TBuffer &buffer);
  void createFromBuffer(TBuffer &buffer);

  ClassDef(O2Timeframe, 1);
};

#endif

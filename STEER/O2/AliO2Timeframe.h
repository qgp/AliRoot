/// \file AliO2Timeframe.h
/// \author R.G.A. Deckers
/// \brief Definition of the AliO2Timeframe class.
///
/// See implementation file for copyright details.

#ifndef ALIO2TIMEFRAME_H
#define ALIO2TIMEFRAME_H

//#include "AliO2Event.h"
#include "AliO2Track.h"
#include "AliO2Vertex.h"
#include <AliESDEvent.h>
#include <TObject.h>
#include <stdint.h>
#include <vector>
/// A Class which represent a timeslice consisting of AliO2Events.
///
/// Very simple right now.
class AliO2Timeframe : public TObject {
public:
  /// Default constructor
  AliO2Timeframe();
  /// Destructor
  ~AliO2Timeframe();
  // void addEvent(const AliO2Event &event, uint64_t timestamp) {
  //   mEvents.push_back(event);
  //   mTimestamps.push_back(timestamp);
  // }
  // void clear() { mEvents.clear(); }
  // Reads an ESD event and adds it to the timeframe with the given timestamp
  // (in ns)
  void addEvent(const AliESDEvent *event, float timestampNs);

protected:
  // protected stuff goes here

private:
  /// copy constructor prohibited
  AliO2Timeframe(const AliO2Timeframe &);
  /// assignment operator prohibited
  AliO2Timeframe &operator=(const AliO2Timeframe &);
  // A simple container, timespam and event.
  std::vector<AliO2Vertex> mVertices; /// The vertices tracks link to
  std::vector<AliO2Track> mGlobalTracks;
  std::vector<AliO2Track> mITSTracks;

  ClassDef(AliO2Timeframe, 1);
};

#endif

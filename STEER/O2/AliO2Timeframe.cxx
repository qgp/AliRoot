/// \file AliO2Timeframe.cxx
/// \brief implementation of the AliO2Timeframe class.
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

#include "AliO2Timeframe.h"
#include <TRandom2.h>
#include <boost/bind.hpp>
// root specific
ClassImp(AliO2Timeframe);

// default constructor
AliO2Timeframe::AliO2Timeframe() {}

// default destructor
AliO2Timeframe::~AliO2Timeframe() {}

void AliO2Timeframe::addEvent(const AliESDEvent *event, float timestampNs) {
  // Random number generator for timestamps
  TRandom2 rng;
  int numberOfTracks = event->GetNumberOfTracks();
  // if we have no tracks in this event don't bother
  if (0 == numberOfTracks) {
    return;
  }
  // TODO: don't sort everything but keep track of where to start sorting from.
  // TODO: write a fast sorter which take the Guassian nature into account.
  // Get the primary vertex of this event
  mVertices.push_back(AliO2Vertex(
      event->GetVertex(), rng.Gaus(timestampNs, 100.0 / sqrt(numberOfTracks))));
  for (int i = 0; i < numberOfTracks; i++) {
    AliESDtrack *track = event->GetTrack(i);
    // Check if this track is global or ITS
    if (NULL == track->GetInnerParam()) {
      // ITS track, box the time in bins of 5000ns and assign the central time
      mGlobalTracks.push_back(
          AliO2Track(track, 5000.0 * floor(timestampNs / 5000.0) + 2500));
    } else {
      // global track, guass of width 100ns
      mITSTracks.push_back(AliO2Track(track, rng.Gaus(timestampNs, 100)));
    }
  }
  std::sort(mGlobalTracks.begin(), mGlobalTracks.end());
  std::sort(mITSTracks.begin(), mITSTracks.end());
  std::sort(mVertices.begin(), mVertices.end());
}

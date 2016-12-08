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
// root specific
ClassImp(AliO2Event);

// default constructor
AliO2Event::AliO2Event() {}

// default destructor
AliO2Event::~AliO2Event() {}

AliO2Event::AliO2Event(const O2Event &event) {
  for (size_t i = 0; i < event.mNumberOfVertices; i++) {
    mVertices.push_back(AliO2Vertex(event.mVertices + i));
  }
  mNumberOfGlobalTracks = event.mNumberOfGlobalTracks;
  mUnambigousGlobalTracksOffset = event.mUnambigousGlobalTracksOffset;
  mUnambigousGlobalTracksSize = event.mUnambigousGlobalTracksSize;
  for (int i = 0; i < mNumberOfGlobalTracks; i++) {
    mGlobalTracks.push_back(AliO2GlobalTrack(event.mGlobalTracks[i]));
  }
  mNumberOfITSTracks = event.mNumberOfITSTracks;
  mUnambigousITSTracksOffset = event.mUnambigousITSTracksOffset;
  mUnambigousITSTracksSize = event.mUnambigousITSTracksSize;
  for (int i = 0; i < mNumberOfITSTracks; i++) {
    mITSTracks.push_back(AliO2ITSTrack(event.mITSTracks[i]));
  }
}

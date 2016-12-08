/// \file O2event->cxx
/// \brief implementation of the O2Event class.
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

#include "O2Event.h"

// default constructor
O2Event::O2Event() {}

// default destructor
O2Event::~O2Event() {}

O2Event::O2Event(O2Vertex *Vertices, size_t NumberOfVertices,
                 O2GlobalTrack *GlobalTracks, size_t NumberOfGlobalTracks,
                 size_t UnambigousGlobalTracksOffset,
                 size_t UnambigousGlobalTracksSize, O2ITSTrack *ITSTracks,
                 size_t NumberOfITSTracks, size_t UnambigousITSTracksOffset,
                 size_t UnambigousITSTracksSize) {
  mVertices = Vertices;
  mNumberOfVertices = NumberOfVertices;
  mGlobalTracks = GlobalTracks;
  mNumberOfGlobalTracks = NumberOfGlobalTracks;
  mUnambigousGlobalTracksOffset = UnambigousGlobalTracksOffset;
  mUnambigousGlobalTracksSize = UnambigousGlobalTracksSize;
  mITSTracks = ITSTracks;
  mNumberOfITSTracks = NumberOfITSTracks;
  mUnambigousITSTracksOffset = UnambigousITSTracksOffset;
  mUnambigousITSTracksSize = UnambigousITSTracksSize;
}

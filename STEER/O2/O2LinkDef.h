#ifdef __CINT__
/// \file O2LinkDef.h
/// \brief Linking definitions for root-cint for the O2 additions.
/// \since 2016-11-19
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

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

// We use a custom streamer to allow for custom compression layers
#pragma link C++ class AliO2Event;
#pragma link C++ class AliO2Timeframe;
#pragma link C++ class AliO2Track;
#pragma link C++ class AliO2GlobalTrack;
#pragma link C++ class AliO2ITSTrack;
#pragma link C++ class AliO2Vertex;
#endif

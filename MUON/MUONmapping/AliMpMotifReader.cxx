/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

// $Id$
// $MpId: AliMpMotifReader.cxx,v 1.10 2006/05/24 13:58:41 ivana Exp $
// Category: sector

//-----------------------------------------------------------------------------
// Class AliMpMotifReader
// -------------------
// Class that takes care of reading the sector data.
// Included in AliRoot: 2003/05/02
// Authors: David Guez, Ivana Hrivnacova; IPN Orsay
//-----------------------------------------------------------------------------

#include "AliMpFiles.h"
#include "AliMpDataStreams.h"
#include "AliMpMotifReader.h"
#include "AliMpMotifMap.h"
#include "AliMpMotif.h"
#include "AliMpMotifSpecial.h"
#include "AliMpMotifType.h"
#include "AliMpConnection.h"
#include "AliMpEncodePair.h"

#include "AliLog.h"

#include <TSystem.h>
#include <TMath.h>
#include <Riostream.h>
#include <Rstrstream.h>

#if !defined(__HP_aCC) && !defined(__alpha)
  #include <sstream>
#endif

/// \cond CLASSIMP
ClassImp(AliMpMotifReader)
/// \endcond

//_____________________________________________________________________________
AliMpMotifReader::AliMpMotifReader(AliMp::StationType station,
                                   AliMq::Station12Type station12,
                                   AliMp::PlaneType plane)
  : TObject(),
    fStationType(station),
    fStation12Type(station12),
    fPlaneType(plane)
{
/// Standard constructor
}

//_____________________________________________________________________________
AliMpMotifReader::~AliMpMotifReader() 
{
/// Destructor  
}

//
// public methods
//

//_____________________________________________________________________________
AliMpMotifType* AliMpMotifReader::BuildMotifType(
                                       const AliMpDataStreams& dataStreams,
                                       const TString& motifTypeId)
{
/// Read the streams describing a motif in the "$MINSTALL/data" directory
/// and fill the AliMpMotifType structure with.
/// The streams mentioned are named padPos<maskName>.dat
/// and connect<maskName>.dat

  // Open streams
  //
  istream& padPosStream 
    = dataStreams.
        CreateDataStream(AliMpFiles::PadPosFilePath(
                            fStationType, fStation12Type, fPlaneType, motifTypeId));
  istream& bergToGCStream 
    = dataStreams.
        CreateDataStream(AliMpFiles::BergToGCFilePath(fStationType, fStation12Type));
      
  istream& motifTypeStream 
    = dataStreams.
        CreateDataStream(AliMpFiles::MotifFilePath(
                            fStationType, fStation12Type, fPlaneType, motifTypeId));

  AliMpMotifType*  motifType = new AliMpMotifType(motifTypeId);	

  TExMap positions;

  char line[256];
  do {
    padPosStream.getline(line,255);
    if (!padPosStream) break;

#if defined (__HP_aCC) || (__alpha)
    strstream strline;
    strline << line;
#else
    istringstream strline(line);
#endif    
    string key;

    strline>>key;
    if ((key=="#") || (key=="") ) continue;

    int i,j;
    strline>>i>>j;
    positions.Add( AliMpExMap::GetIndex(key), 
                   AliMp::Pair(i,j) + 1 );
              // we have to add 1 to the AliMp::Pair in order to 
              // its value always != 0, as the TExMap returns 0 value
              // if given key does not exists      
  } while (!padPosStream.eof());

  const Int_t knbergpins = 
    (fStationType == AliMp::kStation12 ) ? 80 : 100;
  // Station1 & 2 Bergstak connectors have 80 pins, while for stations
  // 3, 4 and 5 they have 100 pins.
  Int_t gassiChannel[100];
  for (Int_t i=0; i<100; ++i) gassiChannel[i] = 0;
  while(1) {
    Int_t bergNum;
    TString gcStr;
    bergToGCStream>>bergNum>>gcStr;
    if (!bergToGCStream.good()) break;
    if (gcStr=="GND") continue;
    if (bergNum>knbergpins) {
        Fatal("BuildMotifType","Berg number > 80 ...");
        continue;
    }
    if ( bergNum <= 0 || bergNum >= 101 ) {
      AliErrorStream() << "Wrong bergNum: " << bergNum << endl;
      return 0;
    }  
    gassiChannel[bergNum-1]= atoi(gcStr);
  }
  
  Int_t nofPadsX=0;
  Int_t nofPadsY=0;

  do {
  
    Int_t ix,iy,numBerg,numKapton,padNum,gassiNum;

    TString lineStr,token;
    lineStr.ReadLine(motifTypeStream);
    if (!motifTypeStream.good()) break;
#if defined (__HP_aCC) || (__alpha)
    strstream tokenList;
    tokenList << lineStr.Data();
#else
    istringstream tokenList(lineStr.Data());
#endif    
    
    token.ReadToken(tokenList);
    if (!tokenList.good()) continue; // column is missing...
    if ( (token.Length()>0) && (token[0]=='#') ) continue; // this is a comment line
    
    numBerg = atoi(token.Data());
    if (numBerg==0) {
      AliWarning(Form("Berg number %s invalid",token.Data()));
      continue;
    }
    
    token.ReadToken(tokenList);
    if (!tokenList.good()) continue; // column is missing...
    numKapton = atoi(token.Data());
    if (numKapton==0) continue;

    
    token.ReadToken(tokenList);
    if (!tokenList.good()) continue; // column is missing...
    if (token=="GND") continue;
    string padName = token.Data();
    padNum = motifType->PadNum(token);
    
     token.ReadToken(tokenList);
     if (token.IsNull() ) continue; // column is missing...
//     if (token[0]!='E') {
//       cerr<<"Problem : gassinumber isn't begining with E:"<<token<<endl;
//       continue;
//     }  else {
//        gassiNum = atoi(token.Data() +1 )-1;
//     }
    if ( (numBerg<1) || (numBerg>knbergpins) ) {
      AliWarning(Form("Berg number %d outside range (1..%d)",numBerg,knbergpins));
      continue;
    }
    
    gassiNum  = gassiChannel[numBerg-1];

    Long_t value = positions.GetValue(AliMpExMap::GetIndex(padName));
    if (!value) {
      AliWarningStream()
        << "Problem: Pad number " << padNum
        << " for motif type " << motifTypeId.Data() 
	<< " found in the motifType stream, but not in the padPos stream" << endl;
      continue;
    }

    AliMpConnection* connection 
      = new AliMpConnection(padNum,numBerg,numKapton,gassiNum, --value);
    
    Bool_t ok = motifType->AddConnection(connection);
    
    if (!ok)
    {
      AliFatal("Could not add connection");
    }
                  
    ix = AliMp::PairFirst(value);
    iy = AliMp::PairSecond(value);
    
    if (ix>=nofPadsX) nofPadsX=ix+1;
    if (iy>=nofPadsY) nofPadsY=iy+1;

  } while (!motifTypeStream.eof());    


  motifType->SetNofPads(nofPadsX, nofPadsY);
  
  delete &padPosStream;
  delete &bergToGCStream;
  delete &motifTypeStream;

  return motifType;
}

//_____________________________________________________________________________
AliMpMotifSpecial*  
AliMpMotifReader::BuildMotifSpecial(const AliMpDataStreams& dataStreams,
                                    const TString& motifID,
                                    AliMpMotifType* motifType,
                                    Double_t scale)
{
/// Build a special motif by reading the file motifSpecial<motifId>.dat
/// in the data directory

  // Open streams
  //
  istream& in 
    = dataStreams.
        CreateDataStream(AliMpFiles::MotifSpecialFilePath(
                             fStationType, fStation12Type, fPlaneType, motifID));

  TString id = MotifSpecialName(motifID,scale);
  
  AliMpMotifSpecial* res = new AliMpMotifSpecial(id,motifType);
  Int_t i,j;
  Double_t x,y;
  in >> i;
  while (!in.eof()){
    in >>j >>x >> y;
    res->SetPadDimensions(i,j,x*scale/2.,y*scale/2.);
    in >> i;
  }
  res->CalculateDimensions();
  
  delete &in;
  
  return res;
}

//_____________________________________________________________________________
TString 
AliMpMotifReader::MotifSpecialName(const TString& motifID, Double_t scale)
{
  /// Build the name taking into the scale, if not 1.0
  TString id;
  
  if ( scale != 1.0 )
  {
    id = Form("%s-%e",motifID.Data(),scale);
  }
  else
  {
    id = motifID;
  }
  return id;
}


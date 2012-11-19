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
// $MpId: AliMpMotifMap.cxx,v 1.16 2006/05/24 13:58:41 ivana Exp $
// Category: motif

//-----------------------------------------------------------------------------
// Class AliMpMotifMap
// -------------------
// Class describing the motif map container, where motifs are
// mapped to their string IDs.
// Included in AliRoot: 2003/05/02
// Authors: David Guez, Ivana Hrivnacova; IPN Orsay
//-----------------------------------------------------------------------------

#include "AliMpMotifMap.h"

#include "AliCodeTimer.h"
#include "AliMpExMapIterator.h"
#include "AliMpVMotif.h"
#include "AliMpMotif.h"
#include "AliMpMotifSpecial.h"
#include "AliMpMotifType.h"
#include "AliMpMotifPosition.h"
#include "AliMpEncodePair.h"

#include "AliLog.h"

#include <Riostream.h>
#include <TArrayI.h>

/// \cond CLASSIMP
ClassImp(AliMpMotifMap)
/// \endcond

//_____________________________________________________________________________
AliMpMotifMap::AliMpMotifMap()
  : TObject(),
    fMotifs(),
    fMotifTypes(),
    fMotifPositions(),
    fMotifPositions2()
{
/// Standard constructor
  
  fMotifPositions2.SetOwner(false);
}

//_____________________________________________________________________________
AliMpMotifMap::AliMpMotifMap(TRootIOCtor* ioCtor) 
  : TObject(),
    fMotifs(ioCtor),
    fMotifTypes(ioCtor),
    fMotifPositions(ioCtor),
    fMotifPositions2(ioCtor)
{
/// Root IO constructor

   fMotifPositions2.SetOwner(false);
}

//_____________________________________________________________________________
AliMpMotifMap::~AliMpMotifMap() 
{
/// Destructor  

  // Delete all registered motifs, motif types, motif positions
}

// 
// private methods
//

//_____________________________________________________________________________
void  AliMpMotifMap::PrintMotif(const AliMpVMotif* motif) const
{
/// Print the motif.

  cout << motif->GetID().Data() << "  "
       << motif->GetMotifType()->GetID() << "    "
       << motif->DimensionX() << " "
       << motif->DimensionY();
}

//_____________________________________________________________________________
void  AliMpMotifMap::PrintMotifType(const AliMpMotifType* motifType) const
{
/// Print the motif type.

  cout << motifType->GetID().Data() << "  "
       << motifType->GetNofPadsX() << "  " 
       << motifType->GetNofPadsY() << "  ";
}

//_____________________________________________________________________________
void  AliMpMotifMap::PrintMotifPosition(
                          const AliMpMotifPosition* motifPosition) const
{
/// Print the motif position.

  cout << " ID " << motifPosition->GetID() << "  "
       << " Motif ID " << motifPosition->GetMotif()->GetID() << "  " 
       << " Pos (X,Y) = (" << motifPosition->GetPositionX() << ","
       << motifPosition->GetPositionY() << ")";
}

//_____________________________________________________________________________
void  AliMpMotifMap::PrintMotifPosition2(
                          const AliMpMotifPosition* motifPosition) const
{
/// Print the motif position.

  cout << setw(3) << motifPosition->GetLowLimitIx() << "  "
       << setw(3) << motifPosition->GetLowLimitIy() << "  "
       << setw(3) << motifPosition->GetHighLimitIx()  << " " 
       << setw(3) << motifPosition->GetHighLimitIy()  << " "
       << motifPosition->GetID() << "  ";
}

//_____________________________________________________________________________
void  AliMpMotifMap::PrintMotifs() const
{
/// Print all the motifs and their motif types 
/// for all motifs in the motifs map.

  if (fMotifs.GetSize()) {
    cout << "Dump of Motif Map - " << fMotifs.GetSize() << " entries:" << endl;
    Int_t counter = 0;        
    AliMpExMapIterator* it = fMotifs.CreateIterator();
    Int_t key;
    AliMpVMotif* motif;
    
    while ( ( motif = static_cast<AliMpVMotif*>(it->Next(key)) ) )
    {
         TString id  = fMotifs.AliMpExMap::GetString(key);
      cout << "Map element " 
           << setw(3) << counter++ << "   " 
           << id.Data() << "   " ;
      PrintMotif(motif);	   
      cout << endl;
    }
    cout << endl;
    delete it;
  }
}

//_____________________________________________________________________________
void  AliMpMotifMap::PrintMotifTypes() const
{
/// Print all the the motifs types and their motif dimensions
/// for all motif types in the motif types map.

  if (fMotifTypes.GetSize()) {
    cout << "Dump of Motif Type Map - " << fMotifTypes.GetSize() << " entries:" << endl;
    Int_t counter = 0;
    AliMpExMapIterator* it = fMotifTypes.CreateIterator();
    Int_t key;
    AliMpMotifType* motifType;
    
    while ( ( motifType = static_cast<AliMpMotifType*>(it->Next(key)) ) )
    {
      TString id  = AliMpExMap::GetString(key);
      cout << "Map element " 
           << setw(3) << counter++ << "   " 
           << id.Data() << "   " ;
      PrintMotifType(motifType);	   
      cout << endl;
    }
    cout << endl;
    delete it;
  }
}

//_____________________________________________________________________________
void 
AliMpMotifMap::GetAllMotifPositionsIDs(TArrayI& ecn) const
{
/// Fill the given array with all motif positions IDs (electronic card numbers)
/// defined in the map

  ecn.Set(fMotifPositions.GetSize());
  TIter next(fMotifPositions.CreateIterator());
  AliMpMotifPosition* motifPosition;
  Int_t i(0);
  while ( ( motifPosition = static_cast<AliMpMotifPosition*>(next()) ) )
  {
    ecn[i] = motifPosition->GetID();
    ++i;
  }
}

//_____________________________________________________________________________
UInt_t  AliMpMotifMap::GetNofMotifPositions() const
{
/// Return the number of all motif positions IDs (electronic card numbers)

  return fMotifPositions.GetSize();
} 

//_____________________________________________________________________________
AliMpMotifPosition* AliMpMotifMap::GetMotifPosition(UInt_t index) const
{
/// Return the motif position which is in the map on the index-th position

  AliCodeTimerAuto("",0);
  
  if ( index >= GetNofMotifPositions() ) {
    AliErrorStream() << "Index " << index << " outside limits." << endl;
    return 0;
  }   

  TIter next(fMotifPositions.CreateIterator());
  while (index-- > 0) next();
  return static_cast<AliMpMotifPosition*>(next());
}

//_____________________________________________________________________________
Int_t AliMpMotifMap::CalculateNofPads() const 
{
/// Calculate total number of pads in the map

  Int_t nofPads = 0;

  TIter next(fMotifPositions.CreateIterator());
  AliMpMotifPosition* motifPosition;
  while ( ( motifPosition = static_cast<AliMpMotifPosition*>(next()) ) )
  {
    nofPads += motifPosition->GetMotif()->GetMotifType()->GetNofPads();
  }

  return nofPads;
}

//_____________________________________________________________________________
void  AliMpMotifMap::PrintMotifPositions() const
{
/// Print all motif positions.

  if (fMotifPositions.GetSize()) {
    cout << "Dump of Motif Position Map - " << fMotifPositions.GetSize() << " entries:" << endl;
    Int_t counter = 0;        
    TIter next(fMotifPositions.CreateIterator());
    AliMpMotifPosition* motifPosition;
    
    while ( ( motifPosition = static_cast<AliMpMotifPosition*>(next()) ) )
    {
      cout << "Map element " 
           << setw(3) << counter++ << "   "; 
      PrintMotifPosition(motifPosition);	   
      cout << endl;
    }
    cout << endl;
  }
}

//_____________________________________________________________________________
void  AliMpMotifMap::PrintMotifPositions2() const
{
/// Print all motif positions from the second map
/// (by global indices)

  if (fMotifPositions2.GetSize()) 
  {
    cout << "Dump of Motif Position Map 2 - " << fMotifPositions2.GetSize() << " entries:" << endl;
    TIter next(fMotifPositions2.CreateIterator());
    AliMpMotifPosition* motifPosition(0x0);
    Int_t counter = 0;        
    
    while ( ( motifPosition = static_cast<AliMpMotifPosition*>(next()) ) )
    {
      cout << "Map element " << setw(3) << counter++ << "   "; 
      PrintMotifPosition2(motifPosition);	   
      cout << endl;
    }
    cout << endl;
  }
}

//
// public methods
//

//_____________________________________________________________________________
Bool_t AliMpMotifMap::AddMotif(AliMpVMotif* motif, Bool_t warn)
{
/// Add the specified motif 
/// if the motif with this ID is not yet present.

  AliMpVMotif* found = FindMotif(motif->GetID());
  if (found) {    
    if (warn && found == motif) 
      AliWarningStream() << "The motif is already in map." << endl;

    if (warn && found != motif) {
      AliWarningStream() 
        << "Another motif with the same ID is already in map." << endl; 
    }	     
    return false;
  }  

  fMotifs.Add(motif->GetID(), motif);

  return true;
}

//_____________________________________________________________________________
Bool_t AliMpMotifMap::AddMotifType(AliMpMotifType* motifType, Bool_t warn)
{
/// Add the specified motif type
/// if the motif with this ID is not yet present.

  AliMpMotifType* found = FindMotifType(motifType->GetID());
  if (found) {    
    if (warn && found == motifType) 
      AliWarningStream() << "The motif type is already in map." << endl;
      
    if (warn && found != motifType) { 
      AliWarningStream() 
        << "Another motif type with the same ID is already in map." << endl;
    }	     
    return false;
  }  

  fMotifTypes.Add(motifType->GetID(), motifType);

  return true;
}

//_____________________________________________________________________________
Bool_t AliMpMotifMap::AddMotifPosition(AliMpMotifPosition* motifPosition, Bool_t warn)
{
/// Add the specified motif position
/// if this position is not yet present.

  AliMpMotifPosition* found = FindMotifPosition(motifPosition->GetID());
  if (found) { 
    if (warn && found == motifPosition) {
      AliWarningStream()
           << "ID: " << motifPosition->GetID() 
           << "  found: " << found 
	   << "  new:   " << motifPosition << endl
	   << "This motif position is already in map." << endl;
    }  
    
    if (warn && found != motifPosition) { 
      AliWarningStream()
           << "ID: " << motifPosition->GetID() 
           << "  found: " << found 
	   << "  new:   " << motifPosition << endl
	   << "Another motif position with the same ID is already in map."
	   << endl;
    }
    	            
    return false;
  }  

  fMotifPositions.Add(motifPosition->GetID() << 16, motifPosition);

  return true;
}

//_____________________________________________________________________________
void AliMpMotifMap::FillMotifPositionMap2()
{
/// Fill the second map (by global indices) of motif positions.

  if (fMotifPositions2.GetSize() > 0 ) {
    AliWarningStream() <<"Map has been already filled." << endl;
    return;
  }  

  TIter next(fMotifPositions.CreateIterator());
  AliMpMotifPosition* motifPosition(0x0);
  while ( ( motifPosition = static_cast<AliMpMotifPosition*>(next()) ) )
  {
    fMotifPositions2.Add(motifPosition->GetLowLimitIx(),
                         motifPosition->GetLowLimitIy(), 
                         motifPosition);
  }
}

//_____________________________________________________________________________
void  AliMpMotifMap::Print(const char* opt) const
{
/// Print the motifs and motif types maps.

  TString sopt(opt);
  
  sopt.ToUpper();
  
  if ( sopt.Contains("MOTIFS") || sopt == "ALL" ) PrintMotifs();
  if ( sopt.Contains("MOTIFTYPES") || sopt == "ALL" ) PrintMotifTypes();
  if ( sopt.Contains("MOTIFPOSITIONS") || sopt == "ALL" ) PrintMotifPositions();
  if ( sopt.Contains("MOTIFPOSITIONS2") || sopt == "ALL" ) PrintMotifPositions2();
}

//_____________________________________________________________________________
void  AliMpMotifMap::PrintGlobalIndices(const char* fileName) const
{
/// Print all motif positions and their global indices.

  ofstream out(fileName, ios::out);

  if (fMotifPositions.GetSize()) {
    TIter next(fMotifPositions.CreateIterator());
    AliMpMotifPosition* motifPosition;
    while ( ( motifPosition = static_cast<AliMpMotifPosition*>(next()) ) )
    {
      out << setw(5) << motifPosition->GetID() << "     "
	  << setw(3) << motifPosition->GetLowLimitIx()  << " " 
	  << setw(3) << motifPosition->GetLowLimitIy() 
         << endl;
    }
    out << endl;
  }
}

//_____________________________________________________________________________
void  AliMpMotifMap::UpdateGlobalIndices(const char* fileName)
{
/// Update the motif positions global indices from the file.

  ifstream in(fileName, ios::in);

  Int_t motifPositionId, offx, offy;
    
  do {
    in >> motifPositionId >> offx >> offy;
    
    if (in.eof()) {
      FillMotifPositionMap2();
      return;
    }  
    
    AliMpMotifPosition* motifPosition = FindMotifPosition(motifPositionId);
	  
    if (motifPosition) {
       AliDebugStream(1) 
            << "Processing " 
            << motifPosition->GetID() << " " << offx << " " << offy << endl; 

       motifPosition->SetLowIndicesLimit(offx, offy);
       
       Int_t offx2 
         = offx + motifPosition->GetMotif()->GetMotifType()->GetNofPadsX() - 1;
	 
       Int_t offy2 
         = offy + motifPosition->GetMotif()->GetMotifType()->GetNofPadsY() - 1;
       
       motifPosition->SetHighIndicesLimit(offx2, offy2);
    }
    else {   
       AliWarningStream()
         << "Motif position " << motifPositionId << " not found" << endl;
    }
  }    
  while (!in.eof());
}


//_____________________________________________________________________________
AliMpVMotif* AliMpMotifMap::FindMotif(const TString& motifID) const
{
/// Find the motif with the specified ID.
  
  //AliCodeTimerAuto("",0);

  return (AliMpVMotif*)fMotifs.GetValue(motifID);
}

//_____________________________________________________________________________
AliMpVMotif* AliMpMotifMap::FindMotif(const TString& motifID, 
                                      const TString& motifTypeID,
			              Double_t padDimensionX, 
                                      Double_t padDimensionY ) const
{
/// Find the motif with the specified ID and returns it
/// only if its motif type and motif dimensions agree
/// with the given motifTypeID and motifDimensions.
/// Disagreement causes fatal error.
 
  //AliCodeTimerAuto("",0);

  AliMpVMotif* motif = FindMotif(motifID);

  if (motif && motif->GetMotifType()->GetID() != motifTypeID) {
      AliFatal("Motif has been already defined with a different type.");
      return 0;	    
  }

  // check pad dimension in case of a normal motif
  if ( motif && 
       dynamic_cast<AliMpMotif*>(motif) && 
       ( motif->GetPadDimensionX(0) != padDimensionX ||
         motif->GetPadDimensionY(0) != padDimensionY ) ) { 
      
      AliFatal("Motif type has been already defined with different dimensions.");
      return 0;

  } 

  // check case of a special motif
  if ( motif && 
      ( padDimensionX == 0. && padDimensionY == 0.) &&
       ! dynamic_cast<AliMpMotifSpecial*>(motif) ) {

      AliFatal("Motif type has been already defined with different dimensions.");
      return 0;

  } 
  
  return motif;
}

//_____________________________________________________________________________
AliMpMotifType* AliMpMotifMap::FindMotifType(const TString& motifTypeID) const
{
/// Find the motif type with the specified motif type ID.
  
  //AliCodeTimerAuto("",0);

  return (AliMpMotifType*)fMotifTypes.GetValue(motifTypeID);
}

//_____________________________________________________________________________
AliMpMotifPosition* 
AliMpMotifMap::FindMotifPosition(Int_t motifPositionID) const
{
/// Find the motif position with the specified motif position ID.
  
  //AliCodeTimerAuto("",0);

  return (AliMpMotifPosition*)fMotifPositions.GetValue(motifPositionID << 16);
}

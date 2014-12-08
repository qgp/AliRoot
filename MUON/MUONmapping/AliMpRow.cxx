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
// $MpId: AliMpRow.cxx,v 1.9 2006/05/24 13:58:46 ivana Exp $
// Category: sector

//-----------------------------------------------------------------------------
// Class AliMpRow
// --------------
// Class describing a row composed of the row segments.
// Included in AliRoot: 2003/05/02
// Authors: David Guez, Ivana Hrivnacova; IPN Orsay
//-----------------------------------------------------------------------------

#include "AliMpRow.h"
#include "AliMpVRowSegment.h"
#include "AliMpVRowSegmentSpecial.h"
#include "AliMpRowSegmentRSpecial.h"
#include "AliMpVMotif.h"
#include "AliMpMotifType.h"
#include "AliMpMotifPosition.h"
#include "AliMpMotifMap.h"
#include "AliMpConstants.h"

#include "AliLog.h"

#include <TMath.h>
#include <Riostream.h>

using std::endl;
/// \cond CLASSIMP
ClassImp(AliMpRow)
/// \endcond

//_____________________________________________________________________________
AliMpRow::AliMpRow(Int_t id, AliMpMotifMap* motifMap) 
  : AliMpVIndexed(),
    fID(id),
    fOffsetY(0.),
    fSegments(),
    fMotifMap(motifMap)
{
/// Standard constructor
}

//_____________________________________________________________________________
AliMpRow::AliMpRow() 
  : AliMpVIndexed(),
    fID(0),
    fOffsetY(0.),
    fSegments(),
    fMotifMap(0)
{
/// Default constructor
}

//_____________________________________________________________________________
AliMpRow::~AliMpRow() 
{
/// Destructor 

  fSegments.Delete();
}

//
// private methods
//

//_____________________________________________________________________________
AliMpVRowSegment*  AliMpRow::FindRowSegment(Int_t ix) const
{    
/// Find first normal row segment with low indices limit >= ix.

  for (Int_t i=0; i<GetNofRowSegments(); i++) {
    AliMpVRowSegment* segment = GetRowSegment(i);

    if (!dynamic_cast<AliMpVRowSegmentSpecial*>(segment) &&
         segment->GetHighLimitIx() >= ix)
	 
     return segment;	 
  }   

  return 0;	 
}

//_____________________________________________________________________________
AliMpMotifPosition*  
AliMpRow::FindMotifPosition(AliMpVRowSegment* segment, Int_t ix) const
{
/// Find first motif position in the specified row segment 
/// with high indices limit >= ix.

  if (!segment) return 0;

  for (Int_t i=0; i<segment->GetNofMotifs(); i++){
     AliMpMotifPosition* motifPosition 
       = GetMotifMap()->FindMotifPosition(segment->GetMotifPositionId(i));
       
     if(!motifPosition) {
       Fatal("FindMotifPosition", "Not found.");
       return 0;
     }  
     
     if (motifPosition->GetHighLimitIx()>=ix) 
       return motifPosition;
  }
  
  return 0;     
}


//_____________________________________________________________________________
void AliMpRow::SetHighIndicesLimits(Int_t iy)
{
/// Set the global indices high limit to its row segments,
/// motif positions with a given value.
/// Keep ix unmodified.

  for (Int_t j=0; j<GetNofRowSegments(); j++) {
     AliMpVRowSegment* rowSegment = GetRowSegment(j);       
     rowSegment
       ->SetHighIndicesLimit(rowSegment->GetHighLimitIx(),iy);

    for (Int_t k=0; k<rowSegment->GetNofMotifs(); k++) {

      Int_t motifPositionId = rowSegment->GetMotifPositionId(k);
      AliMpMotifPosition* motifPosition 
	= GetMotifMap()->FindMotifPosition(motifPositionId);

      motifPosition
	->SetHighIndicesLimit(motifPosition->GetHighLimitIx(), iy);
     
    }
  }  
}

//_____________________________________________________________________________
void  AliMpRow::CheckEmpty() const
{
/// Give a fatal if the row is empty.

  if (GetNofRowSegments() == 0) 
    Fatal("CheckEmpty", "Empty row");
}

//
// public methods
//

//_____________________________________________________________________________
void AliMpRow::AddRowSegment(AliMpVRowSegment* rowSegment)
{
/// Add row segment at the end.

  fSegments.Add(rowSegment);
}  
  
//_____________________________________________________________________________
void AliMpRow::AddRowSegmentInFront(AliMpVRowSegment* rowSegment)
{
/// Insert row segment in the first vector position.

  fSegments.AddFirst(rowSegment);
}  
  
//_____________________________________________________________________________
AliMpVRowSegment* AliMpRow::FindRowSegment(Double_t x) const
{
/// Find the row segment for the specified x position;
/// return 0 if no row segment is found.

  for (Int_t i=0; i<GetNofRowSegments(); i++) {

    AliMpVRowSegment* rs = (AliMpVRowSegment*)fSegments.At(i);

    if (x >= rs->LeftBorderX() && x <= rs->RightBorderX())
      return rs;
  }
  
  return 0;    
}    

//_____________________________________________________________________________
Double_t AliMpRow::LowBorderY() const
{
/// Return the lowest row offset (the Y coordinate of the position of the
/// low border of motif).

  CheckEmpty();

  return fOffsetY - GetRowSegment(0)->HalfSizeY();
}  

//_____________________________________________________________________________
Double_t AliMpRow::UpperBorderY() const
{
/// Return the uppermost row offset (the Y coordinate of the position of the
/// upper border of motif).
\
  CheckEmpty();

  return fOffsetY + GetRowSegment(0)->HalfSizeY();
}  

//_____________________________________________________________________________
AliMpVPadIterator* AliMpRow::CreateIterator() const
{
/// Iterator is not implemented.

  Fatal("CreateIterator", "Iterator is not implemented.");
  
  return 0;
}  

//_____________________________________________________________________________
void AliMpRow::SetMotifPositions()
{
/// Create motif positions objects and fills them in the motif map.

  CheckEmpty();

  for (Int_t j=0; j<GetNofRowSegments(); j++) {
     AliMpVRowSegment* rowSegment = GetRowSegment(j);

     for (Int_t k=0; k<rowSegment->GetNofMotifs(); k++) {
        // Get values 
	Int_t motifPositionId = rowSegment->GetMotifPositionId(k);
	AliMpVMotif* motif = rowSegment->GetMotif(k);
        
        Double_t posx, posy;
	rowSegment->MotifCenter(motifPositionId, posx, posy);
       
        AliMpMotifPosition* motifPosition 
	  = new AliMpMotifPosition(motifPositionId, motif, posx, posy);

        // set the initial value to of HighIndicesLimit() Invalid()
        // (this is used for calculation of indices in case of
        // special row segments)
        motifPosition->SetHighIndicesLimit(0, 0, false);

        //Bool_t warn = (rowSegment->GetNofMotifs()==1); 
        Bool_t warn = true;
	if (dynamic_cast<AliMpVRowSegmentSpecial*>(rowSegment)) warn = false; 
               // supress warnings for special row segments
	       // which motifs can overlap the row borders
	       
        Bool_t added = GetMotifMap()->AddMotifPosition(motifPosition, warn);
	
	if (!added) delete motifPosition;	
     }  
  }
}    

//_____________________________________________________________________________
void AliMpRow::SetGlobalIndices(AliMp::Direction constPadSizeDirection, 
                                AliMpRow* rowBefore)
{
/// Set the global indices limits to its row segments, motif positions
/// and itself.

  Int_t ix = AliMpConstants::StartPadIndex();
  Int_t iy = AliMpConstants::StartPadIndex();

  for (Int_t j=0; j<GetNofRowSegments(); j++) {
     AliMpVRowSegment* rowSegment = GetRowSegment(j);
     
     ix += rowSegment->GetLowLimitIx();

     for (Int_t k=0; k<rowSegment->GetNofMotifs(); k++) {
     
       // Find the y index value of the low edge
       if (rowBefore) {
         if (constPadSizeDirection == AliMp::kY) {
           iy = rowBefore->GetHighLimitIy()+1;
         } 
	 else {
           AliMpVRowSegment* seg = rowBefore->FindRowSegment(ix);	
	   AliMpMotifPosition* motPos =  FindMotifPosition(seg, ix);
	   if (!dynamic_cast<AliMpRowSegmentRSpecial*>(rowSegment)) {
             if (!motPos) {
	       Fatal("SetGlobalIndices", "Motif position in rowBefore not found.");
               return;
             }  
	   
             iy = motPos->GetHighLimitIy()+1;
	   }  
         }
       } 

       // Set (ix, iy) to k-th motif position and update ix
       ix = rowSegment->SetIndicesToMotifPosition(k, AliMp::Pair(ix, iy));
    }
    rowSegment->SetGlobalIndices(rowBefore);    
  }

  // The low/high indices limits has to be taken as the highest/lowest from all 
  // row segments
  Int_t ixl = 9999;
  Int_t iyl = 9999;
  Int_t ixh = AliMpConstants::StartPadIndex();
  Int_t iyh = AliMpConstants::StartPadIndex();

  for (Int_t i=0; i<GetNofRowSegments(); i++) {
    
    AliMpVRowSegment* rowSegment = GetRowSegment(i);
    
    if ( rowSegment->GetLowLimitIx() < ixl ) 
       ixl = rowSegment->GetLowLimitIx();
       
    if ( rowSegment->GetLowLimitIy() < iyl ) 
       iyl = rowSegment->GetLowLimitIy();

    if ( rowSegment->GetHighLimitIx() > ixh ) 
       ixh = rowSegment->GetHighLimitIx();
       
    if ( rowSegment->GetHighLimitIy() > iyh ) 
       iyh = rowSegment->GetHighLimitIy();
  }     

  SetLowIndicesLimit(ixl, iyl);
  SetHighIndicesLimit(ixh, iyh);
}

//_____________________________________________________________________________
Double_t  AliMpRow::GetPositionX() const
{
/// Return the position of the row centre.

  return ( GetRowSegment(0)->LeftBorderX() +
           GetRowSegment(GetNofRowSegments()-1)->RightBorderX() )/2.;
}

//_____________________________________________________________________________
Double_t  AliMpRow::GetPositionY() const
{
/// Return the position of the row centre.

  return fOffsetY;  
}

//_____________________________________________________________________________
Double_t  AliMpRow::GetDimensionX() const
{
/// Return the maximum halflengths of the row in x, y.

  return ( GetRowSegment(GetNofRowSegments()-1)->RightBorderX() -
           GetRowSegment(0)->LeftBorderX() )/2.;
}

//_____________________________________________________________________________
Double_t  AliMpRow::GetDimensionY() const
{
/// Return the maximum halflengths of the row in x, y.

  return GetRowSegment(0)->HalfSizeY();  
}

//_____________________________________________________________________________
void AliMpRow::SetRowSegmentOffsets(Double_t offsetx)
{
/// Set the row segments offsets in X .

  CheckEmpty();
  
  AliMpVRowSegment* previous = 0;

  for (Int_t j=0; j<GetNofRowSegments(); j++) {
     AliMpVRowSegment* rowSegment = GetRowSegment(j);

     Double_t offsetX;
     if (previous) 
      offsetX = previous->RightBorderX();
    else
      offsetX = offsetx;  
  
    rowSegment->SetOffset(offsetX, 0.);
    previous = rowSegment;  
  }
}


//_____________________________________________________________________________
Double_t AliMpRow::SetOffsetY(Double_t offsetY)
{
/// Set the row offset (the Y coordinate of the position of the
/// center of motif) and returns the offset of the top border.

  CheckEmpty();

  AliMpVRowSegment* first = GetRowSegment(0);
  Double_t rowSizeY = first->HalfSizeY();
  
  // Check if all next row segments have motif of
  // the same size in y
  for (Int_t i=1; i<GetNofRowSegments(); i++) {
     Double_t sizeY = GetRowSegment(i)->HalfSizeY();
     
     if (TMath::Abs(sizeY - rowSizeY) >= AliMpConstants::LengthTolerance()) {
       Fatal("SetOffsetY", "Motif with different Y size in one row");
       return 0.;
     }  
  }

  offsetY += rowSizeY ;
    
  fOffsetY = offsetY;
    
  return offsetY += rowSizeY;
}  

//_____________________________________________________________________________
Int_t AliMpRow::GetNofRowSegments() const 
{
/// Return number of row segments.

  return fSegments.GetSize();
}  

//_____________________________________________________________________________
AliMpVRowSegment* AliMpRow::GetRowSegment(Int_t i) const 
{
/// Return i-th row segment.

  if (i<0 || i>=GetNofRowSegments()) {
    AliWarningStream() << "Index outside range" << endl;
    return 0;
  }
  
  return (AliMpVRowSegment*)fSegments.At(i);  
}
 

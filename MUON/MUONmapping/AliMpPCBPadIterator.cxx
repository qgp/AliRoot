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
// $MpId$

#include "AliMpPCBPadIterator.h"

#include "AliMpArea.h"
#include "AliMpConstants.h"
#include "AliLog.h"
#include "AliMpPCB.h"
#include "AliMpSlat.h"
#include "AliMpSlatSegmentation.h"
#include "AliMpEncodePair.h"

#include "Riostream.h"
#include "TMath.h"


//-----------------------------------------------------------------------------
/// \class AliMpPCBPadIterator
/// 
/// Iterates over slat pads within a region of constant pad size.
/// 
/// \author Laurent Aphecetche
//-----------------------------------------------------------------------------

using std::cout;
using std::endl;
/// \cond CLASSIMP
ClassImp(AliMpPCBPadIterator)
/// \endcond

//_____________________________________________________________________________
AliMpPCBPadIterator::AliMpPCBPadIterator(const AliMpSlat* slat,
                                         const AliMpArea& area)
: AliMpVPadIterator(),
fkSlat(slat),
fSlatSegmentation(new AliMpSlatSegmentation(slat)),
fMinIndices(0),
fMaxIndices(0),
fOffset(0),
fCurrentPad(),
fIsDone(kTRUE)
{
  ///
  /// Normal ctor.
  /// Iteration will be done on the slat, over the crop of (area,slat_area)
  ///
  if (!CropArea(area)) 
  {
    AliError(Form("Could not crop area : (x,y)min=(%e,%e) ; max=(%e,%e) for slat %s",
                  area.LeftBorder(),area.DownBorder(),
                  area.RightBorder(),area.UpBorder(),fkSlat->GetID()));
  }
  Invalidate();
}

//_____________________________________________________________________________
AliMpPCBPadIterator::~AliMpPCBPadIterator()
{
  ///
  /// Dtor.
  ///
  delete fSlatSegmentation;
}

//_____________________________________________________________________________
Bool_t
AliMpPCBPadIterator::CropArea(const AliMpArea& area)
{
  ///
  /// Checks the area is correct, and truncate it
  /// if it goes outside the slat.
  
  AliDebug(3,Form("Input area (%7.2f,%7.2f)->(%7.2f,%7.2f)",
                  area.LeftBorder(),area.DownBorder(),
                  area.RightBorder(),area.UpBorder()));
  
  const Double_t kEpsilon = AliMpConstants::LengthTolerance();
  
  // Left and right x-limits have to come from first and last pcbs
  // to deal with short and rounded pcbs cases.
  AliMpPCB* first = fkSlat->FindPCB(area.LeftBorder(),area.DownBorder());
  AliMpPCB* last = fkSlat->FindPCB(area.RightBorder()-kEpsilon,
                                   area.DownBorder());
  
  // Check we're indeed dealing with only one pcb
  if ( first != last )
  {
    AliError("This iterator supposed to work on a single PCB. Please check");
    return kFALSE;
  }
  
  AliDebug(3,Form("PCB %s Ixmin %2d Ixmax %2d",
                  first->GetID(),first->Ixmin(),first->Ixmax()));
  
  Double_t xleft = first->ActiveXmin();
  Double_t xright = first->ActiveXmax() - kEpsilon;
  
  AliDebug(3,Form("xleft,xright=%e,%e",xleft,xright));
  
  Double_t xmin = TMath::Max(area.LeftBorder(),xleft);
  Double_t xmax = TMath::Min(area.RightBorder(),xright);
  Double_t ymin = TMath::Max(area.DownBorder(),0.0);
  Double_t ymax = TMath::Min(area.UpBorder(),first->DY()*2.0-kEpsilon);
  
  AliDebug(3,Form("Cropped area (%e,%e)->(%e,%e)",
                  xmin,ymin,xmax,ymax));
  
  // At this point (xmin,ymin)->(xmax,ymax) should be a zone completely included
  // inside the slat.
  // We now try to convert this into a couple of indices pair indicating the
  // region to iterate over, using integer values, not floating point ones.
  // For this, we must find out the 4 pads that intersect the (xmin,ymin;xmax,ymax)
  // area.
  
  Int_t ixmin = first->Ixmin() + TMath::FloorNint((xmin-first->ActiveXmin())/first->PadSizeX());
  Int_t ixmax = first->Ixmin() + TMath::CeilNint((xmax-first->ActiveXmin())/first->PadSizeX()) - 1;
  Int_t iymin = first->Iymin() + TMath::FloorNint((ymin-first->Ymin())/first->PadSizeY());
  Int_t iymax = first->Iymin() + TMath::CeilNint((ymax-first->Ymin())/first->PadSizeY()) - 1;
  
  
  fMinIndices = AliMp::Pair(ixmin,iymin);
  fMaxIndices = AliMp::Pair(ixmax,iymax);
  
  AliDebug(3,Form("Paddified cropped area (%d,%d)->(%d,%d) %d,%d ; %d,%d",
                  ixmin,iymin,ixmax,iymax,
                  AliMp::PairFirst(fMinIndices),AliMp::PairSecond(fMinIndices),
                  AliMp::PairFirst(fMaxIndices),AliMp::PairSecond(fMaxIndices)));
  
  return fMinIndices >= 0 && fMaxIndices >= 0;
}

//_____________________________________________________________________________
AliMpPad
AliMpPCBPadIterator::CurrentItem() const
{
  ///
  /// Returns the current iteration position (i.e. a pad)
  ///
  return fCurrentPad;
}

//_____________________________________________________________________________
void
AliMpPCBPadIterator::First()
{
  ///
  /// (re)Starts the iteration.
  ///
  
  AliDebug(3,Form("area = (%d,%d)->(%d,%d)",
                  AliMp::PairFirst(fMinIndices),AliMp::PairSecond(fMinIndices),
                  AliMp::PairFirst(fMaxIndices),AliMp::PairSecond(fMaxIndices)));
  fOffset = fMinIndices;
  fIsDone = kFALSE;
  SetPad(fCurrentPad,AliMp::PairFirst(fOffset),AliMp::PairSecond(fOffset));
  if ( ! fCurrentPad.IsValid() ) Next();
  if ( ! fCurrentPad.IsValid() ) 
  {
    // did not find any valid pad in there, bailing out.
    fIsDone = kTRUE;
    AliError(Form("Could not initiate iterator for slat %s. "
                  " Please check the area you gave : %d,%d to %d,%d",
                  fkSlat->GetName(),
                  AliMp::PairFirst(fMinIndices),AliMp::PairSecond(fMinIndices),
                  AliMp::PairFirst(fMaxIndices),AliMp::PairSecond(fMaxIndices)));
    return;
  }
}

//_____________________________________________________________________________
Bool_t
AliMpPCBPadIterator::GetNextPosition(Int_t& ix, Int_t& iy) const
{
  /// Get the next iteration position. 
  /// On input, fOffset must be a valid position (i.e. within iteration
  /// area already).
  
  ++ix;
  
  if ( ix > AliMp::PairFirst(fMaxIndices) )
  {
    // Go back leftmost position...
    ix = AliMp::PairFirst(fMinIndices);
    // ... and up
    ++iy;
    if ( iy > AliMp::PairSecond(fMaxIndices) )
    {
      return false;
    }
  }
  return true;
}


//_____________________________________________________________________________
void
AliMpPCBPadIterator::Invalidate()
{
  ///
  /// Invalidate the iterator.
  ///
  fOffset = 0;
  fCurrentPad = AliMpPad::Invalid();
  fIsDone = kTRUE;
}

//_____________________________________________________________________________
Bool_t
AliMpPCBPadIterator::IsDone() const
{
  ///
  /// Whether the iteration is finished or not.
  ///
  return fIsDone;
}

//_____________________________________________________________________________
void
AliMpPCBPadIterator::Next()
{
  /// This one is the meat of the class.
  /// We're iterating in x-direction mainly, starting from 
  /// lower-left of the iteration area, and proceeding right,
  /// until we reach right border, in which case we increment y
  /// and go back to leftmost position.
  /// End of iteration occurs when both x and y are outside the iteration
  /// window.
  
  if (IsDone()) return;
  
  AliMpPad pad(fCurrentPad);
  int n = 0;
  Int_t ix(AliMp::PairFirst(fOffset));
  Int_t iy(AliMp::PairSecond(fOffset));
  
  while ( ( pad == fCurrentPad || ! pad.IsValid() ) && n<100 )
  {
    ++n;
    if (GetNextPosition(ix,iy)==kFALSE) 
    {
      Invalidate();
      return;
    } 
    SetPad(pad,ix,iy);
  }
  if ( n>=100 )
  {
    AliFatal("This should not happen!");
  }
  fCurrentPad = pad;
}

//_____________________________________________________________________________
void 
AliMpPCBPadIterator::Print(Option_t*) const
{
  /// printout
  cout << Form("fkSlat=%p fSlatSegmentation=%p (%s)",fkSlat,fSlatSegmentation,
               fkSlat->GetName()) << endl
  << Form("minIndices=(%d,%d) maxIndices=(%d,%d)",
          AliMp::PairFirst(fMinIndices),AliMp::PairSecond(fMinIndices),
          AliMp::PairFirst(fMaxIndices),AliMp::PairSecond(fMaxIndices)) << endl
  << Form("currentOffset=(%d,%d) isdone=%d currentpad=",
          AliMp::PairFirst(fOffset),AliMp::PairSecond(fOffset),fIsDone) << endl;
  fCurrentPad.Print();
}

//_____________________________________________________________________________
void
AliMpPCBPadIterator::SetPad(AliMpPad& pad, Int_t ix, Int_t iy)
{
  ///
  /// Sets the current pad.
  ///
  pad = fSlatSegmentation->PadByIndices(ix, iy,kFALSE);
  if (pad.IsValid())
  {
    fOffset = AliMp::Pair(ix,iy);
  }
}

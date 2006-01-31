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
// $MpId: AliMpPCB.cxx,v 1.3 2005/09/19 19:01:31 ivana Exp $

#include "AliMpPCB.h"

#include "AliMpMotif.h"
#include "AliMpMotifPosition.h"
#include "AliMpMotifSpecial.h"
#include "AliMpMotifType.h"
#include "AliLog.h"

#include "Riostream.h"
#include "TMath.h"
#include <sstream>

ClassImp(AliMpPCB)

//_____________________________________________________________________________
AliMpPCB::AliMpPCB() 
  : TObject(), fId(""), 
    fPadSizeX(0), fPadSizeY(0), 
    fEnveloppeSizeX(0), fEnveloppeSizeY(0),
    fXoffset(0),
    fActiveXmin(0), fActiveXmax(0),
    fIxmin(99999), fIxmax(0), fIymin(99999), fIymax(0)
{
      //
      // Default ctor.
      //
}

//_____________________________________________________________________________
AliMpPCB::AliMpPCB(const char* id, Double_t padSizeX, Double_t padSizeY,
		   Double_t enveloppeSizeX, Double_t enveloppeSizeY)
  : TObject(), fId(id), 
    fPadSizeX(padSizeX), fPadSizeY(padSizeY), 
    fEnveloppeSizeX(enveloppeSizeX), fEnveloppeSizeY(enveloppeSizeY),
    fXoffset(0),
    fActiveXmin(0), fActiveXmax(0),
    fIxmin(99999), fIxmax(0), fIymin(99999), fIymax(0)
{
      //
      // Normal ctor. Must be fed with the PCB's name (id), the pad dimensions
      // and the global dimension of the virtual enveloppe of the PCB
      // (usually 400x400 mm)
}

//_____________________________________________________________________________
AliMpPCB::AliMpPCB(const AliMpPCB& o) 
  : TObject(o),
    fPadSizeX(0), fPadSizeY(0), 
    fEnveloppeSizeX(0), fEnveloppeSizeY(0),
    fXoffset(0),
    fActiveXmin(0), fActiveXmax(0),
    fIxmin(99999), fIxmax(0), fIymin(99999), fIymax(0)
{
  o.Copy(*this);
}

//_____________________________________________________________________________
AliMpPCB::AliMpPCB(const char* id, AliMpMotifSpecial* ms)
: TObject(), fId(id), fPadSizeX(-1.0), fPadSizeY(-1.0),
  fXoffset(0)
{
  //
  // Very special ctor to be used by trigger stations only (and for a very
  // specific case).
  //
  // Note that in this very case, we only allow one (special) motif per PCB.
  // This limitation might not be justified, except that it's all we need
  // so far ;-)
  //
  fXoffset = 0.0;
  fEnveloppeSizeX = ms->Dimensions().X()*2.0;
  fEnveloppeSizeY = ms->Dimensions().Y()*2.0;
  fActiveXmin = 0.0;
  fActiveXmax = fEnveloppeSizeX;
  fIxmin = fIymin = 0;
  fIxmax = ms->GetMotifType()->GetNofPadsX()-1;
  fIymax = ms->GetMotifType()->GetNofPadsY()-1;
  TVector2 position(ms->Dimensions());
  AliMpMotifPosition* mp = new AliMpMotifPosition(-1,ms,position);
  mp->SetLowIndicesLimit(AliMpIntPair(fIxmin,fIymin));
  mp->SetHighIndicesLimit(AliMpIntPair(fIxmax,fIymax));
#ifdef WITH_ROOT
  fMotifs.AddLast(mp);
#else
  fMotifs.push_back(mp);
#endif
}

//_____________________________________________________________________________
AliMpPCB&
AliMpPCB::operator=(const AliMpPCB& o)
{
  o.Copy(*this);
  return *this;  
}

//_____________________________________________________________________________
AliMpPCB::~AliMpPCB()
{
  //
  // Dtor.
  //
}

//_____________________________________________________________________________
Double_t
AliMpPCB::ActiveXmin() const
{
  //
  // Returns the mininum x for which there is a pad in this PCB.
  // Different from Xmin only for PCB which are not full of pads.
  //
  
  return fActiveXmin;
}

//_____________________________________________________________________________
Double_t
AliMpPCB::ActiveXmax() const
{
  //
  // Returns the maximum x for which there is a pad in this PCB.
  // Different from Xmax only for PCB which are not full of pads.
  //  
  
  return fActiveXmax;
}

//_____________________________________________________________________________
void
AliMpPCB::Add(AliMpMotifType* mt, Int_t ix, Int_t iy)
{
  //
  // Add a motif to this PCB. (ix,iy) indicates one corner position of the motif
  // where the sign of ix and iy is used to indicate which corner is the 
  // reference (then for values, abs(ix) and abs(iy) are used indeed) :
  //
  // (ix>0,iy>0) : bottom-left corner
  // (ix<0,iy>0) : bottom-right corner
  // (ix<0,iy<0) : top-right corner
  // (ix>0,iy<0) : top-left corner.
  
  AliMpVMotif* motif = 
    new AliMpMotif(mt->GetID(),mt,TVector2(PadSizeX()/2.0,PadSizeY()/2.0));
  TVector2 position;
  Int_t ixmin(-1);
  Int_t iymin(-1);
  
  if ( ix >= 0 && iy >= 0 )
  {
    position.Set(ix*PadSizeX(),iy*PadSizeY());
    ixmin = ix;
    iymin = iy;
  }
  else
  if ( ix >= 0 && iy < 0 )
  {
    position.Set(ix*PadSizeX(),Ymax()+iy*PadSizeY());
    ixmin = ix;
    iymin = TMath::Nint(Ymax()/PadSizeY()) + iy;
  }
  else
  if ( ix < 0 && iy < 0 )
  {
    position.Set(Xmax()+ix*PadSizeX(),Ymax()+iy*PadSizeY());
    ixmin = TMath::Nint(Xmax()/PadSizeX()) + ix;
    iymin = TMath::Nint(Ymax()/PadSizeY()) + iy;
  }
  else
  if ( ix < 0 && iy >=0 )
  {
    position.Set(Xmax()+ix*PadSizeX(),iy*PadSizeY());
    ixmin = TMath::Nint(Xmax()/PadSizeX()) + ix;
    iymin = iy;
  }

  position += motif->Dimensions();

  AliMpMotifPosition* mp = new AliMpMotifPosition(-1,motif,position);
  Int_t ixmax = ixmin + mt->GetNofPadsX() - 1;
  Int_t iymax = iymin + mt->GetNofPadsY() - 1;

  mp->SetLowIndicesLimit(AliMpIntPair(ixmin,iymin));
  mp->SetHighIndicesLimit(AliMpIntPair(ixmax,iymax));

#ifdef WITH_ROOT
  fMotifs.AddLast(mp);
#else
  fMotifs.push_back(mp);
#endif

  fIxmin = std::min(fIxmin,ixmin);
  fIxmax = std::max(fIxmax,ixmax);
  fIymin = std::min(fIymin,iymin);
  fIymax = std::max(fIymax,iymax);

  fActiveXmin = fIxmin*PadSizeX();
  fActiveXmax = (fIxmax+1)*PadSizeX();
}

//_____________________________________________________________________________
TObject*
AliMpPCB::Clone(const char* /*newname*/) const
{
  //
  // Return a full copy of this object.
  //
  return new AliMpPCB(*this);
}

//_____________________________________________________________________________
AliMpPCB*
AliMpPCB::Clone(const TArrayI& manuids, Int_t ixOffset, Double_t xOffset) const
{
  //
  // Get a full copy of *this, and then apply 2 changes to it :
  //
  // a) define the relationship motifType <-> manu id
  // b) define the x-offset
  // c) shift ix indices backwards to insure that e.g. the first
  //    pcb of a slat will start at ix=0 (only relevant for rounded pcbs).
  //

  // First get a full clone.
  AliMpPCB* pcb = static_cast<AliMpPCB*>(Clone());

  if ( pcb->GetSize() != manuids.GetSize() )
  {
      AliError(Form("Cannot Clone PCB %s because I do not get the correct number of "
                    "manu ids (got %d, wanted %d)",pcb->GetID(),
                    manuids.GetSize(),pcb->GetSize()));
      return 0;
  }

  AliMpIntPair shift(-fIxmin,0);

  // Then change the internal MotifPositions wrt manu id
  // and position (offset in x).
  for ( Size_t i = 0; i < pcb->GetSize(); ++i )
    {
      AliMpMotifPosition* mp = pcb->GetMotifPosition(i);
      mp->SetID(manuids[i]);
      mp->SetPosition(mp->Position() + TVector2(xOffset,0));
      mp->SetLowIndicesLimit(mp->GetLowIndicesLimit()+
			     shift+
 			     AliMpIntPair(ixOffset,0));
      mp->SetHighIndicesLimit(mp->GetHighIndicesLimit()+
			      shift+
 			      AliMpIntPair(ixOffset,0));
    }
  
  pcb->fIxmin += ixOffset + shift.GetFirst();
  pcb->fIxmax += ixOffset + shift.GetFirst();
  pcb->fXoffset = xOffset;

  pcb->fActiveXmin += xOffset;
  pcb->fActiveXmax += xOffset;

  return pcb;
}

//_____________________________________________________________________________
void
AliMpPCB::Copy(TObject& o) const
{
  TObject::Copy(o);
  AliMpPCB& pcb = static_cast<AliMpPCB&>(o);
  pcb.fId = fId;
  pcb.fPadSizeX = fPadSizeX;
  pcb.fPadSizeY = fPadSizeY;
  pcb.fEnveloppeSizeX = fEnveloppeSizeX;
  pcb.fEnveloppeSizeY = fEnveloppeSizeY;
  pcb.fXoffset = fXoffset;
  pcb.fIxmin = fIxmin;
  pcb.fIxmax = fIxmax;
  pcb.fIymin = fIymin;
  pcb.fIymax = fIymax;
  pcb.fActiveXmin = fActiveXmin;
  pcb.fActiveXmax = fActiveXmax;

#ifdef WITH_ROOT
  pcb.fMotifs.Clear();
#else
  pcb.fMotifs.clear();
#endif

#ifdef WITH_ROOT
  for ( Size_t i = 0; i < fMotifs.GetEntriesFast(); ++i )
#else
  for ( Size_t i = 0; i < fMotifs.size(); ++i )
#endif  
    {
      AliMpMotifPosition* pos = (AliMpMotifPosition*)fMotifs[i];
      AliMpMotifPosition* pcbpos = 
	new AliMpMotifPosition(pos->GetID(),pos->GetMotif(),pos->Position());
      pcbpos->SetLowIndicesLimit(pos->GetLowIndicesLimit());
      pcbpos->SetHighIndicesLimit(pos->GetHighIndicesLimit());
#ifdef WITH_ROOT
      pcb.fMotifs.AddLast(pcbpos);
#else      
      pcb.fMotifs.push_back(pcbpos);
#endif      
    }
}

//_____________________________________________________________________________
Double_t
AliMpPCB::ActiveDX() const
{
  //
  // Half-length (in x-direction) occupied by pads  
  //
  
  return GetNofPadsX()*fPadSizeX/2.0;
}

//_____________________________________________________________________________
Double_t
AliMpPCB::DX() const
{
  //
  // Half-length (in x-direction) of the PCB.
  // This length is the one of the virtual enveloppe of the PCB and might
  // be bigger than the length occupied by pads (e.g. for rounded or short
  // PCBs).  
  // See also ActiveDX().
  //
  
  return fEnveloppeSizeX/2.0;
}

//_____________________________________________________________________________
Double_t
AliMpPCB::ActiveDY() const
{
  //
  // Half-length (in y-direction) occupied by pads
  //
  
  return GetNofPadsY()*fPadSizeY/2.0;
}

//_____________________________________________________________________________
Double_t
AliMpPCB::DY() const
{
  //
  // Half-length (in y-direction) of the PCB.
  // This length is the one of the virtual enveloppe of the PCB and might
  // be bigger than the length occupied by pads (e.g. for rounded or short
  // PCBs).
  // See also ActiveDY().
  //
  
  return fEnveloppeSizeY/2.0;
}

//_____________________________________________________________________________
AliMpMotifPosition*
AliMpPCB::FindMotifPosition(Int_t ix, Int_t iy) const
{
  //
  // Returns the motifPosition located at the position referenced by
  // integer indices (ix,iy).
  //
  
#ifdef WITH_ROOT
  for (Size_t i = 0; i < fMotifs.GetEntriesFast(); ++i )
#else  
  for (Size_t i = 0; i < fMotifs.size(); ++i )
#endif
    {
      AliMpMotifPosition* mp = (AliMpMotifPosition*)fMotifs[i];
      if ( mp->HasPad(AliMpIntPair(ix,iy)) )
      {
        return mp;
      }
    }
  return 0;
}

//_____________________________________________________________________________
AliMpMotifPosition*
AliMpPCB::FindMotifPosition(Double_t x, Double_t y) const
{
  //
  // Returns the motifPosition located at position (x,y)
  //
  
#ifdef WITH_ROOT
  for (Size_t i = 0; i < fMotifs.GetEntriesFast(); ++i )
#else  
  for (Size_t i = 0; i < fMotifs.size(); ++i )
#endif   
  {
    AliMpMotifPosition* mp = (AliMpMotifPosition*)fMotifs[i];
    
    TVector2 localPos( TVector2(x,y) - mp->Position() );
    
    AliMpIntPair localIndices(mp->GetMotif()->PadIndicesLocal(localPos));
    
    if ( localIndices.IsValid() && mp->GetMotif()->GetMotifType()->HasPad(localIndices) )
    {
      return mp;
    }
  }
    return 0;
}

//_____________________________________________________________________________
const char*
AliMpPCB::GetID() const
{
  //
  // Returns the name of this PCB.
  //
  
  return fId.Data();
}

//_____________________________________________________________________________
AliMpMotifPosition*
AliMpPCB::GetMotifPosition(AliMpPCB::Size_t i) const
{
  //
  // Get the i-th motifPosition stored in this PCB's internal array.
  //
  
#ifdef WITH_ROOT
  if ( i >= fMotifs.GetEntriesFast() ) return 0;
#else
  if ( i >= fMotifs.size() ) return 0;
#endif  
  return (AliMpMotifPosition*)fMotifs[i];
}

//_____________________________________________________________________________
Int_t
AliMpPCB::GetNofPadsX() const
{
  //
  // Returns the number of pads in x-direction.
  //
  
  return fIxmax-fIxmin+1;
}

//_____________________________________________________________________________
Int_t
AliMpPCB::GetNofPadsY() const
{
  //
  // Returns the number of pads in y-direction.
  //
  
  return fIymax-fIymin+1;
}

//_____________________________________________________________________________
AliMpPCB::Size_t
AliMpPCB::GetSize() const
{
  //
  // Returns the number of motifPositions stored in this PCB.
  //
  
#ifdef WITH_ROOT
  return fMotifs.GetEntriesFast();
#else  
  return fMotifs.size();
#endif  
}


//_____________________________________________________________________________
Int_t
AliMpPCB::Ixmin() const
{
  //
  // Returns the index value of the leftmost pad.
  //
  
  return fIxmin;
}

//_____________________________________________________________________________
Int_t
AliMpPCB::Ixmax() const
{
  //
  // Returns the index value of the rightmost pad.
  //
  
  return Ixmin() + GetNofPadsX() - 1;
}

//_____________________________________________________________________________
Int_t
AliMpPCB::Iymin() const
{
  //
  // Returns the index value of the bottom pad.
  //
  
  return fIymin;
}

//_____________________________________________________________________________
Int_t
AliMpPCB::Iymax() const
{
  //
  // Returns the index value of the top pad.
  //
  
  return Iymin() + GetNofPadsY() - 1;
}

//_____________________________________________________________________________
Double_t
AliMpPCB::PadSizeX() const
{
  //
  // Returns the pad size in x-direction (in mm)
  //
  
  return fPadSizeX;
}

//_____________________________________________________________________________
Double_t
AliMpPCB::PadSizeY() const
{
  //
  // Returns the pad size in y-direction (in mm)
  //
  
  return fPadSizeY;
}

//_____________________________________________________________________________
void
AliMpPCB::Print(Option_t* option) const
{
  //
  // Printout of this PCB.
  // If option="M", the contained motifs are printed too.
  //
  
  cout << "PCB " << GetID() << " PADSIZES=(" << fPadSizeX << ","
  << fPadSizeY << ") iMin=(" << fIxmin << "," << fIymin << ") "
  << "iMax=(" << fIxmax << "," << fIymax << ") " 
  << " EnvXmin,max=(" << Xmin() << "," << Xmax() 
  << ") Xmin,max=(" << ActiveXmin() << "," << ActiveXmax() << ")"
  << endl;
  
  if ( option && option[0] == 'M' )
  {
#ifdef WITH_ROOT
    for ( Size_t i = 0; i < fMotifs.GetEntriesFast(); ++i )
#else  
    for ( Size_t i = 0; i < fMotifs.size(); ++i )
#endif    
    {
      if (option)
	    {
	      fMotifs[i]->Print(option+1);
	    }
      else
	    {
	      fMotifs[i]->Print();
	    }
    }
  }
}

//_____________________________________________________________________________
Double_t
AliMpPCB::X() const
{
  //
  // Returns the x-position of the PCB center.
  //
  
  return fXoffset + DX();
}

//_____________________________________________________________________________
Double_t
AliMpPCB::Xmin() const
{
  //
  // Returns the leftmost x-position in this PCB.
  //
  
  return X() - DX();
}

//_____________________________________________________________________________
Double_t
AliMpPCB::Xmax() const
{
  //
  // Returns the rightmost x-position in this PCB.
  //
  
  return X() + DX();
}

//_____________________________________________________________________________
Double_t
AliMpPCB::Y() const
{
  //
  // Returns the y-position of the PCB center.
  //
  
  return DY(); // this works as PCB are organized in a single row within slats.
}

//_____________________________________________________________________________
Double_t
AliMpPCB::Ymin() const
{
  //
  // Returns the smallest y-position in this PCB.
  //
  
  return Y() - DY();
}

//_____________________________________________________________________________
Double_t
AliMpPCB::Ymax() const
{
  //
  // Returns the largest y-position in this PCB.
  //
  
  return Y() + DY();
}


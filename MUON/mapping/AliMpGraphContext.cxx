// $Id$
// Category: graphics
//
// Class AliMpGraphContext
// -----------------------
// Class describing a the correspondance between a given area
// in pad, and a zone of real (cm) position
//
// Author: David GUEZ, IPN Orsay

#include "AliMpGraphContext.h"

ClassImp(AliMpGraphContext)

AliMpGraphContext *AliMpGraphContext::fgInstance = 0;
GraphContextVector AliMpGraphContext::fgStack;

// private constructor
AliMpGraphContext::AliMpGraphContext():
  TObject(),
  fPadPosition(TVector2(0.5,0.5)),
  fPadDimensions(TVector2(0.49,0.49)),
  fRealPosition(TVector2(0.,0.)),
  fRealDimensions(TVector2(1,1))
{
  fColor = 20;
  // default constructor (private)
}
AliMpGraphContext *AliMpGraphContext::Instance()
{
  // return or create a unique instance of this class
  if (fgInstance) return fgInstance;
  fgInstance = new AliMpGraphContext;
  return fgInstance;
}

TVector2 AliMpGraphContext::RealToPad(const TVector2 &position) const
{
  // transform a real position into its equivalent position in the pad
  Double_t x=position.X();
  Double_t y=position.Y();
  x-= (fRealPosition.X()-fRealDimensions.X());
  x/=fRealDimensions.X();
  x*=fPadDimensions.X();
  x+= (fPadPosition.X()-fPadDimensions.X() );

  y-= (fRealPosition.Y()-fRealDimensions.Y());
  y/=fRealDimensions.Y();
  y*=fPadDimensions.Y();
  y+= (fPadPosition.Y()-fPadDimensions.Y() );

  return TVector2(x,y);
}



void AliMpGraphContext::RealToPad(const TVector2 &position,
			      const TVector2 &dimensions,
			      TVector2 &padPosition,
			      TVector2 &padDimensions) const
{
  // transform the real area (position,dimensions) to
  // its equivalent pad area
  padPosition = RealToPad(position);
  padDimensions = 
    TVector2(dimensions.X()*fPadDimensions.X()/fRealDimensions.X(),
	     dimensions.Y()*fPadDimensions.Y()/fRealDimensions.Y());

}
void AliMpGraphContext::SetPadPosForReal(const TVector2 &position,
				     const TVector2 &dimensions)
{
  // Set the pad area from the actual one
  // corresponding to the given real area.
  RealToPad(position,dimensions,fPadPosition,fPadDimensions);
}
void AliMpGraphContext::Push() const
{
  // Store the current configuration
  AliMpGraphContext *save = new AliMpGraphContext(*this);
  fgStack.push_back(save);
}
void AliMpGraphContext::Pop()
{
  // restore the last saved configuration
  if (!fgStack.empty()){
    AliMpGraphContext *obj = fgStack.back();
    *this = *obj;
    fgStack.pop_back();
    delete obj;
  }
}

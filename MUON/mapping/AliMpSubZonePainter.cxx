// $Id$
// Category: graphics
//
// Class AliMpSubZonePainter
// -------------------------
// Class for drawing a subzone into canvas
//
// Authors: David Guez, IPN Orsay
  
#include "AliMpSubZonePainter.h"
#include "AliMpGraphContext.h"
#include "AliMpSubZone.h"
#include "AliMpVRowSegment.h"
#include "AliMpVMotif.h"

ClassImp(AliMpSubZonePainter)

//_______________________________________________________________________
AliMpSubZonePainter::AliMpSubZonePainter()
  : AliMpVPainter(),
    fSubZone(0)
{
  // default dummy constructor
}
//_______________________________________________________________________
AliMpSubZonePainter::AliMpSubZonePainter(AliMpSubZone *subZone)
  : AliMpVPainter(),
    fSubZone(subZone)
{
  // normal constructor 

}
//_______________________________________________________________________
Int_t AliMpSubZonePainter::DistancetoPrimitive(Int_t x, Int_t y)
{
  // dist to the nearest segment center if (x,y) is inside the sub-zone
  // 9999 otherwise
  if (fSubZone->GetNofRowSegments()<1) return 9999;
  AliMpGraphContext *gr = AliMpGraphContext::Instance();

  gr->Push();
  InitGraphContext();


  TVector2 point = TVector2(gPad->AbsPixeltoX(x), gPad->AbsPixeltoY(y));

  Double_t res=9999.;
  for (Int_t iseg=0;iseg<fSubZone->GetNofRowSegments();++iseg){
    //for each row segments
    AliMpVRowSegment* seg = fSubZone->GetRowSegment(iseg);

    TVector2 pos,dim;
    gr->RealToPad(seg->Position(),seg->Dimensions(),pos,dim);

    if ( IsInside(point,pos,dim) ){
      Double_t value = (point-pos).Mod();
      if (value<res) res=value;
    }
  }
  gr->Pop();
  return (Int_t)res;
}

//_______________________________________________________________________
void AliMpSubZonePainter::DumpObject()
{
// Draw the owned object
  fSubZone->Dump();

}
//_______________________________________________________________________
TVector2 AliMpSubZonePainter::GetPosition() const
{
// Get the owned object's position

  if (fSubZone->GetNofRowSegments()<1) return TVector2(0.,0.);
  AliMpVRowSegment* seg = fSubZone->GetRowSegment(0);

  // bl = bottom left position;
  TVector2 bl = seg->Position()-seg->Dimensions();
  // ur = upper right position
  TVector2 ur = seg->Position()+seg->Dimensions();

  for (Int_t iseg=1;iseg<fSubZone->GetNofRowSegments();++iseg){
    seg = fSubZone->GetRowSegment(iseg);
    // update the bottom-left corner
    if (bl.X()>seg->Position().X()-seg->Dimensions().X())
      bl.Set(seg->Position().X()-seg->Dimensions().X(),bl.Y());
    if (bl.Y()>seg->Position().Y()-seg->Dimensions().Y())
      bl.Set(bl.X(),seg->Position().Y()-seg->Dimensions().Y());
    // update the upper-right corner
    if (ur.X()<seg->Position().X()+seg->Dimensions().X())
      ur.Set(seg->Position().X()+seg->Dimensions().X(),ur.Y());
    if (ur.Y()<seg->Position().Y()+seg->Dimensions().Y())
      ur.Set(ur.X(),seg->Position().Y()+seg->Dimensions().Y());
  }
  return (ur+bl)/2.;
}
//_______________________________________________________________________
TVector2 AliMpSubZonePainter::GetDimensions() const
{
  // Get the owned object's dimensions

  if (fSubZone->GetNofRowSegments()<1) return TVector2(0.,0.);
  AliMpVRowSegment* seg = fSubZone->GetRowSegment(0);

  // bl = bottom left position;
  TVector2 bl = seg->Position()-seg->Dimensions();
  // ur = upper right position
  TVector2 ur = seg->Position()+seg->Dimensions();

  for (Int_t iseg=1;iseg<fSubZone->GetNofRowSegments();++iseg){
    seg = fSubZone->GetRowSegment(iseg);
    // update the bottom-left corner
    if (bl.X()>seg->Position().X()-seg->Dimensions().X())
      bl.Set(seg->Position().X()-seg->Dimensions().X(),bl.Y());
    if (bl.Y()>seg->Position().Y()-seg->Dimensions().Y())
      bl.Set(bl.X(),seg->Position().Y()-seg->Dimensions().Y());
    // update the upper-right corner
    if (ur.X()<seg->Position().X()+seg->Dimensions().X())
      ur.Set(seg->Position().X()+seg->Dimensions().X(),ur.Y());
    if (ur.Y()<seg->Position().Y()+seg->Dimensions().Y())
      ur.Set(ur.X(),seg->Position().Y()+seg->Dimensions().Y());
  }
  return (ur-bl)/2.;
}
//_______________________________________________________________________
void AliMpSubZonePainter::Draw(Option_t *option)
{
// Draw the sector on the current pad
// The first letter of <option> is treated as follows:
// case "S" : each row segments are drawn separately
// case ""  : the whole subzone is drawn at once
// in both cases, the rest of the option is passed
// as argument to the Draw function of respectively
// zone or row objects.
// ---

  if (!fSubZone) return;
  AliMpGraphContext *gr = AliMpGraphContext::Instance();

  gr->Push();
  InitGraphContext();
  switch (option[0]){
  case 'S':
    {

	for (Int_t iRowSeg=0;iRowSeg<fSubZone->GetNofRowSegments();++iRowSeg){
	  gr->Push();
	  AliMpVRowSegment *rowSegment = fSubZone->GetRowSegment(iRowSeg);

	  gr->SetPadPosForReal(rowSegment->Position(),
			       rowSegment->Dimensions());
	  gr->SetColor(GetColor());
	  DrawObject(rowSegment,option+1);
      
	  gr->Pop();
	}
    }
    break;
  default: AppendPad(option);
  }
  gr->Pop();
}


//_______________________________________________________________________
void AliMpSubZonePainter::Paint(Option_t *option)
{
// Paint the object
  AliMpGraphContext *gr = AliMpGraphContext::Instance();
  if (!fSubZone) return;
  if (fSubZone->GetNofRowSegments()<1) return;
  gr->Push();
  gPad->Range(0.,0.,1.,1.);
  Int_t col=gVirtualX->GetFillColor();
  InitGraphContext();
  
  gVirtualX->SetFillColor(GetColor());
  for (Int_t iRowSeg=0;iRowSeg<fSubZone->GetNofRowSegments();++iRowSeg){
    AliMpVRowSegment *rowSegment = fSubZone->GetRowSegment(iRowSeg);
    TVector2 pos,dim;
    gr->RealToPad(rowSegment->Position(),rowSegment->Dimensions(),
		  pos,dim);
    gPad->PaintBox(pos.X()-dim.X(),pos.Y()-dim.Y(),
		   pos.X()+dim.X(),pos.Y()+dim.Y());
    if (option[0]=='T'){
      Float_t textSize =   gVirtualX->GetTextSize();
      gVirtualX->SetTextSize(15);
      gPad->PaintText(pos.X()-0.01,pos.Y()-0.01,
		      fSubZone->GetMotif()->GetID());
      gVirtualX->SetTextSize(textSize);
    }
  }

  gVirtualX->SetFillColor(col);
  gr->Pop();
}

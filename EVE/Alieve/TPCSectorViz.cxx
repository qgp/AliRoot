// $Header$

#include "TPCSectorViz.h"

#include <Alieve/TPCData.h>
#include <Alieve/TPCSectorData.h>
#include <AliTPCParam.h>

#include <TStyle.h>
#include <TColor.h>

using namespace Reve;
using namespace Alieve;

//______________________________________________________________________
// TPCSectorViz
//
// Base class for TPC raw-data visualization.
// See TPCSector2D and TPCSector3D for concrete implementations.

ClassImp(TPCSectorViz)

/**************************************************************************/

TPCSectorViz::TPCSectorViz(const Text_t* n, const Text_t* t) :
  TNamed(n, t),
  Reve::RenderElement(fFrameColor),

  fTPCData  (0),
  fSectorID (0),

  fMinTime   (0),
  fMaxTime   (450),
  fThreshold (5),
  fMaxVal    (80),

  fRnrInn   (kTRUE),
  fRnrOut1  (kTRUE),
  fRnrOut2  (kTRUE),

  fFrameColor ((Color_t) 4),
  fRnrFrame (kTRUE),
  fTrans    (kFALSE),
  fRTS      (1)
{}

TPCSectorViz::~TPCSectorViz()
{
  if(fTPCData) fTPCData->DecRefCount();
}

/**************************************************************************/

//UInt_t TPCSectorViz::IncRTS()
//{
//  return ++fRTS;
//}


void TPCSectorViz::SetDataSource(TPCData* data)
{
  if(data == fTPCData) return;
  if(fTPCData) fTPCData->DecRefCount();
  fTPCData = data;
  if(fTPCData) fTPCData->IncRefCount();
  IncRTS();
}

void TPCSectorViz::SetSectorID(Int_t segment)
{
  if(segment < 0 ) segment = 0;
  if(segment > 35) segment = 35;
  fSectorID = segment;
  IncRTS();
}

TPCSectorData* TPCSectorViz::GetSectorData() const
{
  return fTPCData ? fTPCData->GetSectorData(fSectorID) : 0;
}

/**************************************************************************/

void TPCSectorViz::SetTrans(Bool_t trans) 
{
  fTrans = trans;
  if(fTrans) {
    for (Int_t k=0; k<16; ++k)
      fMatrix[k] = 0.;

    using namespace TMath;
    Float_t c = Cos((fSectorID + 0.5)*20*Pi()/180 - PiOver2());
    Float_t s = Sin((fSectorID + 0.5)*20*Pi()/180 - PiOver2());
    Float_t z = TPCSectorData::GetParam().GetZLength();
    if(fSectorID >= 18) z = -z;
  
    // column major
    fMatrix[0]  = -c;
    fMatrix[1]  = -s;
    fMatrix[4]  = -s;
    fMatrix[5]  =  c;
    fMatrix[10] = -1;
    fMatrix[14] =  z;
    fMatrix[15] =  1;
  }
}

/**************************************************************************/

void TPCSectorViz::SetupColor(Int_t val, UChar_t* pixel) const
{
  using namespace TMath;
  Float_t div  = Max(1, fMaxVal - fThreshold);
  Int_t   nCol = gStyle->GetNumberOfColors();
  Int_t   cBin = (Int_t) Nint(nCol*(val - fThreshold)/div);

  ColorFromIdx(gStyle->GetColorPalette(Min(nCol - 1, cBin)), pixel);
}

/**************************************************************************
 * Copyright(c) 1998-2008, ALICE Experiment at CERN, all rights reserved. *
 * See http://aliceinfo.cern.ch/Offline/AliRoot/License.html for          *
 * full copyright notice.                                                 *
 **************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// The drawing module for the AD detector                            //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "AliEveADModule.h"
#include "AliADConst.h"

#include <TH1.h>
#include <TEveManager.h>
#include <AliEveEventManager.h>
#include <TEveTrans.h>
#include <TEveRGBAPaletteOverlay.h>
#include <TGLViewer.h>
#include <TGLAnnotation.h>

#include <AliRawReader.h>
#include <AliADRawStream.h>
#include <AliESDEvent.h>

static const Float_t xSize = 18.1;
static const Float_t ySize = 21.6;
static const Float_t zSize = 2.54;
static const Float_t xGap = 0.27;
static const Float_t yGap = 0.05;
//static const Float_t zGap = 0.23;
static const Float_t zGap = 0.23+2.54;
static const Int_t xModule[4] = {1,1,-1,-1};
static const Int_t yModule[4] = {1,-1,-1,1};
static const Int_t zLayer[2] = {-1,1};
static const Float_t ACHole[2] = {6.2,3.7};

ClassImp(AliEveADModule)

/******************************************************************************/
AliEveADModule::AliEveADModule(const Text_t* n, Bool_t side, Int_t maxCharge)
  : TEveQuadSet(n),
    fStream(NULL),
    fIsASide(side)
    
{
  //
  // Default constructor
  //
  TEveRGBAPalette* rawPalette  = new TEveRGBAPalette(0, maxCharge);
  rawPalette->SetLimits(0, maxCharge);
  SetPalette(rawPalette);
  Reset(TEveQuadSet::kQT_FreeQuad, kFALSE, 16);
}

/******************************************************************************/
AliEveADModule::~AliEveADModule()
{
  //
  // Destructor
  //
  delete fStream;
}

/******************************************************************************/
void AliEveADModule::LoadRaw(AliRawReader *rawReader)
{
  //
  // Load AD raw-data
  //
  if (fStream) delete fStream;
  fStream = new AliADRawStream(rawReader);
  if (!fStream->Next()) {
    delete fStream;
    fStream = NULL;
    return;
  }

  for (Int_t iChannel=0; iChannel < AliADRawStream::kNChannels; ++iChannel) {
    Int_t offChannel = kOfflineChannel[iChannel];
    Int_t iLayer; Int_t iModule; Float_t rHole;
    if (fIsASide) {
      if (offChannel < 8) continue;
      iLayer = (offChannel-8)/4;
      iModule = (offChannel-8)%4;
      rHole = ACHole[0];
      
    }
    else {
      if (offChannel >= 8) continue;
      iLayer = offChannel/4;
      iModule = offChannel%4;
      rHole = ACHole[1];

    }
    Float_t v[12];
    //First part of module
    v[ 0] = (rHole + xGap)*xModule[iModule]; v[ 1] = yGap*yModule[iModule]; v[ 2] = zGap*zLayer[iLayer];
    v[ 3] = (rHole + xGap)*xModule[iModule]; v[ 4] = (yGap + ySize)*yModule[iModule]; v[ 5] = zGap*zLayer[iLayer];
    v[ 6] = (xGap + xSize)*xModule[iModule]; v[ 7] = (yGap + ySize)*yModule[iModule]; v[ 8] = zGap*zLayer[iLayer];   
    v[ 9] = (xGap + xSize)*xModule[iModule]; v[10] = yGap*yModule[iModule]; v[11] = zGap*zLayer[iLayer];
    /*/
    v[ 12] = (rHole + xGap)*xModule[iModule]; v[ 13] = yGap*yModule[iModule]; v[ 14] = (zSize + zGap)*zLayer[iLayer];
    v[ 15] = (rHole + xGap)*xModule[iModule]; v[ 16] = (yGap + ySize)*yModule[iModule]; v[ 17] = (zSize + zGap)*zLayer[iLayer];
    v[ 18] = (xGap + xSize)*xModule[iModule]; v[ 19] = (yGap + ySize)*yModule[iModule]; v[ 20] = (zSize + zGap)*zLayer[iLayer];   
    v[ 21] = (xGap + xSize)*xModule[iModule]; v[22] = yGap*yModule[iModule]; v[23] = (zSize + zGap)*zLayer[iLayer];/*/
    
    AddQuad(v);
    //DigitValue(fStream->GetPedestal(iChannel,AliADRawStream::kNEvOfInt/2));
    QuadValue(fStream->GetADC(iChannel));
    
    //Hole region
    v[ 0] = xGap*xModule[iModule]; v[ 1] = (rHole + yGap)*yModule[iModule]; v[ 2] = zGap*zLayer[iLayer];
    v[ 3] = xGap*xModule[iModule]; v[ 4] = (yGap + ySize)*yModule[iModule]; v[ 5] = zGap*zLayer[iLayer];
    v[ 6] = (rHole + xGap)*xModule[iModule]; v[ 7] = (yGap+ ySize)*yModule[iModule]; v[ 8] = zGap*zLayer[iLayer];   
    v[ 9] = (rHole + xGap)*xModule[iModule]; v[10] = yGap*yModule[iModule]; v[11] = zGap*zLayer[iLayer];
    /*/
    v[ 12] = xGap*xModule[iModule]; v[ 13] = (rHole + yGap)*yModule[iModule]; v[ 14] = (zSize + zGap)*zLayer[iLayer];
    v[ 15] = xGap*xModule[iModule]; v[ 16] = (yGap + ySize)*yModule[iModule]; v[ 17] = (zSize + zGap)*zLayer[iLayer];
    v[ 18] = (rHole + xGap)*xModule[iModule]; v[ 19] = (yGap+ ySize)*yModule[iModule]; v[ 20] = (zSize + zGap)*zLayer[iLayer];   
    v[ 21] = (rHole + xGap)*xModule[iModule]; v[22] = yGap*yModule[iModule]; v[23] = (zSize + zGap)*zLayer[iLayer];/*/
    
    AddQuad(v);
    //DigitValue(fStream->GetPedestal(iChannel,AliADRawStream::kNEvOfInt/2));
    QuadValue(fStream->GetADC(iChannel));
  }

  if (fIsASide) 
    RefMainTrans().SetPos(0, 0, 1699.7);
  else
    RefMainTrans().SetPos(0, 0, -1954.4);

  gEve->AddElement(this);
  TEveRGBAPalette* pal = this->GetPalette();
  TEveRGBAPaletteOverlay *po = new TEveRGBAPaletteOverlay(pal, 0.69, 0.1, 0.3, 0.05);
  TGLViewer* v = gEve->GetDefaultGLViewer();
  v->AddOverlayElement(po);
  TGLAnnotation *ann = new TGLAnnotation(v,"Amplitude of measured charge [ADC counts]",0.69, 0.2);
  ann->SetTextSize(0.04);
  gEve->Redraw3D();
}



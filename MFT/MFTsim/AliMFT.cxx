// **************************************************************************
// * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
// *                                                                        *
// * Author: The ALICE Off-line Project.                                    *
// * Contributors are mentioned in the code where appropriate.              *
// *                                                                        *
// * Permission to use, copy, modify and distribute this software and its   *
// * documentation strictly for non-commercial purposes is hereby granted   *
// * without fee, provided that the above copyright notice appears in all   *
// * copies and that both the copyright notice and this permission notice   *
// * appear in the supporting documentation. The authors make no claims     *
// * about the suitability of this software for any purpose. It is          *
// * provided "as is" without express or implied warranty.                  *
// **************************************************************************

//====================================================================================================================================================
//
//      Main class of the ALICE Muon Forward Tracker
//
//      Contact author: antonio.uras@cern.ch
//
//====================================================================================================================================================

#include "AliLog.h"
#include "TFile.h"  
#include "TGeoManager.h"    
#include "TGeoVolume.h"
#include "TGeoMatrix.h"
#include "TVirtualMC.h"
#include "TClonesArray.h"
#include "TGeoGlobalMagField.h"
#include "AliRun.h"
#include "AliLoader.h"
#include "AliDetector.h"
#include "AliMC.h"
#include "AliMagF.h"
#include "AliMFT.h"
#include "AliMFTHit.h"
#include "AliMFTDigit.h"
#include "AliMFTCluster.h"
#include "AliTrackReference.h"
#include "AliMFTSegmentation.h"
#include "AliMFTDigitizer.h"
#include "AliMFTPlane.h"
#include "TString.h"
#include "TObjArray.h"

ClassImp(AliMFT) 

//====================================================================================================================================================

AliMFT::AliMFT():
  AliDetector(),
  fVersion(1),
  fNPlanes(0),
  fNSlices(1),
  fSDigitsPerPlane(0),
  fDigitsPerPlane(0),
  fRecPointsPerPlane(0),
  fSideDigits(0),
  fSegmentation(0),
  fNameGeomFile(0),
  fChargeDispersion(25.e-4),
  fSingleStepForChargeDispersion(0),
  fNStepForChargeDispersion(4),
  fDensitySupportOverSi(0.036),
  fFileNameForUnderyingEvent(0), 
  fFileNameForPileUpEvents(0),
  fNPileUpEvents(0), 
  fUnderlyingEventID(-1)
{

  // default constructor

  for (Int_t iPileUp=0; iPileUp<AliMFTConstants::fNMaxPileUpEvents; iPileUp++) fPileUpEventsIDs[iPileUp] = -1;


}

//====================================================================================================================================================

AliMFT::AliMFT(const Char_t *name, const Char_t *title):
  AliDetector(name, title),
  fVersion(1),
  fNPlanes(0),
  fNSlices(1),
  fSDigitsPerPlane(0),
  fDigitsPerPlane(0),
  fRecPointsPerPlane(0),
  fSideDigits(0),
  fSegmentation(0),
  fNameGeomFile(0),
  fChargeDispersion(25.e-4),
  fSingleStepForChargeDispersion(0),
  fNStepForChargeDispersion(4),
  fDensitySupportOverSi(0.036),
  fFileNameForUnderyingEvent(0), 
  fFileNameForPileUpEvents(0),
  fNPileUpEvents(0), 
  fUnderlyingEventID(-1)
{

  for (Int_t iPileUp=0; iPileUp<AliMFTConstants::fNMaxPileUpEvents; iPileUp++) fPileUpEventsIDs[iPileUp] = -1;

  fNameGeomFile = "AliMFTGeometry.root";

  SetGeometry();

  Init();

}

//====================================================================================================================================================

AliMFT::AliMFT(const Char_t *name, const Char_t *title, Char_t *nameGeomFile):
  AliDetector(name, title),
  fVersion(1),
  fNPlanes(0),
  fNSlices(1),
  fSDigitsPerPlane(0),
  fDigitsPerPlane(0),
  fRecPointsPerPlane(0),
  fSideDigits(0),
  fSegmentation(0),
  fNameGeomFile(0),
  fChargeDispersion(25.e-4),
  fSingleStepForChargeDispersion(0),
  fNStepForChargeDispersion(4),
  fDensitySupportOverSi(0.036),
  fFileNameForUnderyingEvent(0), 
  fFileNameForPileUpEvents(0),
  fNPileUpEvents(0), 
  fUnderlyingEventID(-1)
{

  for (Int_t iPileUp=0; iPileUp<AliMFTConstants::fNMaxPileUpEvents; iPileUp++) fPileUpEventsIDs[iPileUp] = -1;

  fNameGeomFile = nameGeomFile;

  SetGeometry();

  Init();

}

//====================================================================================================================================================

AliMFT::~AliMFT() {

  if (fSDigitsPerPlane)   { fSDigitsPerPlane->Delete();    delete fSDigitsPerPlane;   }
  if (fDigitsPerPlane)    { fDigitsPerPlane->Delete();     delete fDigitsPerPlane;    }
  if (fRecPointsPerPlane) { fRecPointsPerPlane->Delete();  delete fRecPointsPerPlane; }

}

//====================================================================================================================================================

void AliMFT::CreateMaterials() {

  // Definition of MFT materials  - to be updated to the most recent values

  AliInfo("Start MFT materials");

  // data from PDG booklet 2002                 density [gr/cm^3]     rad len [cm]           abs len [cm]    
  Float_t   aSi = 28.085 ,    zSi   = 14. ,     dSi      =  2.329 ,   radSi   =  21.82/dSi , absSi   = 108.4/dSi  ;    // Silicon
  Float_t   aCarb = 12.01 ,   zCarb =  6. ,     dCarb    =  2.265 ,   radCarb =  18.8 ,      absCarb = 49.9       ;    // Carbon
  Float_t   aAlu = 26.98 ,    zAlu  = 13. ,     dAlu     =  2.70  ,   radAlu  =  8.897 ,     absAlu  = 39.70      ;    // Aluminum

  // Air mixture
  const Int_t nAir = 4;   
  Float_t   aAir[nAir] = {12, 14, 16, 36} ,  zAir[nAir] = {6, 7, 8, 18} ,   wAir[nAir]={0.000124, 0.755267, 0.231781, 0.012827} , dAir=0.00120479;   

  // Water mixture
  const Int_t nWater = 2;   
  Float_t   aWater[nWater] = {1.00794, 15.9994} ,  zWater[nWater] = {1, 8} ,   wWater[nWater] = {0.111894, 0.888106} , dWater=1.;   

  // SiO2 mixture
  const Int_t nSiO2 = 2; 
  Float_t   aSiO2[nSiO2] = {15.9994, 28.0855} ,   zSiO2[nSiO2] = {8., 14.} ,   wSiO2[nSiO2] = {0.532565, 0.467435} , dSiO2 = 2.20;  

  // Inox mixture
  const Int_t nInox = 9;
  Float_t   aInox[nInox] = {12.0107, 54.9380, 28.0855, 30.9738, 32.0660, 58.6928, 51.9961, 95.9400, 55.8450} ;   
  Float_t   zInox[nInox] = { 6,      25,      14,      15,      16,      28,      24,      42,      26     } ;   
  Float_t   wInox[nInox] = {0.0003,  0.02,    0.01,    0.00045, 0.0003,  0.12,    0.17,    0.025,   0.65395} ;
  Float_t   dInox = 8.03; 
  
  Int_t   matId  = 0;                        // tmp material id number
  Int_t   unsens = 0, sens=1;                // sensitive or unsensitive medium
  Int_t   itgfld = 3;			     // type of field intergration 0 no field -1 user in guswim 1 Runge Kutta 2 helix 3 const field along z
  Float_t maxfld = 5.; 		             // max field value

  Float_t tmaxfd = -10.0;                    // max deflection angle due to magnetic field in one step
  Float_t stemax =  0.001;                   // max step allowed [cm]
  Float_t deemax = -0.2;                     // maximum fractional energy loss in one step 0<deemax<=1  
  Float_t epsil  =  0.001;                   // tracking precision [cm]   
  Float_t stmin  = -0.001;                   // minimum step due to continuous processes [cm] (negative value: choose it automatically)

  Float_t tmaxfdSi =  0.1;                   // max deflection angle due to magnetic field in one step
  Float_t stemaxSi =  5.0e-4;                // maximum step allowed [cm]
  Float_t deemaxSi =  0.1;                   // maximum fractional energy loss in one step 0<deemax<=1
  Float_t epsilSi  =  0.5e-4;                // tracking precision [cm]
  Float_t stminSi  = -0.001;                 // minimum step due to continuous processes [cm] (negative value: choose it automatically)

  Int_t   isxfld = ((AliMagF*)TGeoGlobalMagField::Instance()->GetField())->Integ();   // from CreateMaterials in STRUCT/AliPIPEv3.cxx
  Float_t sxmgmx = ((AliMagF*)TGeoGlobalMagField::Instance()->GetField())->Max();     // from CreateMaterials in STRUCT/AliPIPEv3.cxx
      
  AliMixture(++matId,"Air", aAir, zAir, dAir, nAir, wAir); 
  AliMedium(kAir,    "Air", matId, unsens, itgfld, maxfld, tmaxfd, stemax, deemax, epsil, stmin);
  
  AliMaterial(++matId, "Si", aSi, zSi, dSi, radSi, absSi);  
  AliMedium(kSi,       "Si", matId, sens, isxfld, sxmgmx, tmaxfdSi, stemaxSi, deemaxSi, epsilSi, stminSi);

  AliMaterial(++matId, "Readout", aSi, zSi, dSi, radSi, absSi);  
  AliMedium(kReadout,  "Readout", matId, unsens, isxfld, sxmgmx, tmaxfdSi, stemaxSi, deemaxSi, epsilSi, stminSi);

  AliMaterial(++matId, "Support", aSi, zSi, dSi*fDensitySupportOverSi, radSi/fDensitySupportOverSi, absSi/fDensitySupportOverSi);  
  AliMedium(kSupport,  "Support", matId, unsens, isxfld, sxmgmx, tmaxfdSi, stemaxSi, deemaxSi, epsilSi, stminSi);
  
  AliMaterial(++matId, "Carbon", aCarb, zCarb, dCarb, radCarb, absCarb );
  AliMedium(kCarbon,   "Carbon", matId, unsens, isxfld,  sxmgmx, tmaxfd, stemax, deemax, epsil, stmin);
  
  AliMaterial(++matId, "Alu", aAlu, zAlu, dAlu, radAlu, absAlu);
  AliMedium(kAlu,      "Alu", matId, unsens, isxfld,  sxmgmx, tmaxfd, stemax, deemax, epsil, stmin);
  
  AliMixture(++matId, "Water", aWater, zWater, dWater, nWater, wWater);
  AliMedium(kWater,   "Water", matId, unsens, itgfld, maxfld, tmaxfd, stemax, deemax, epsil, stmin);
  
  AliMixture(++matId, "SiO2", aSiO2, zSiO2, dSiO2, nSiO2, wSiO2);
  AliMedium(kSiO2,    "SiO2", matId, unsens, itgfld, maxfld, tmaxfd, stemax, deemax, epsil, stmin);
  
  AliMixture(++matId, "Inox", aInox, zInox, dInox, nInox, wInox);
  AliMedium(kInox,    "Inox", matId, unsens, itgfld, maxfld, tmaxfd, stemax, deemax, epsil, stmin);

  AliInfo("End MFT materials");
          
}

//====================================================================================================================================================

void AliMFT::CreateGeometry() {

  // Creates detailed geometry simulation (currently GEANT volumes tree)        

  AliInfo("Start MFT preliminary version building");
  if(!TVirtualMC::GetMC()->IsRootGeometrySupported()) return;                
  TGeoVolumeAssembly *vol = CreateVol();
  AliInfo("TGeoVolumeAssembly created!");
  gGeoManager->GetVolume("ALIC")->AddNode(vol,0);
  AliInfo("Stop MFT preliminary version building");

  if (fNStepForChargeDispersion) fSingleStepForChargeDispersion = fChargeDispersion/Double_t(fNStepForChargeDispersion);

} 

//====================================================================================================================================================

void AliMFT::AddAlignableVolumes() {

  // Create entries for alignable volumes associating the symbolic volume
  // name with the corresponding volume path. Needs to be syncronized with
  // eventual changes in the geometry.

  TString sysName = "MFT";
  TString volPath = "/ALIC_1/MFT_0";
  
  if (!gGeoManager->SetAlignableEntry(sysName.Data(),volPath.Data())) {
    AliFatal(Form("Alignable entry %s not created. Volume path %s not valid", sysName.Data(), volPath.Data()));
  }  

}

//====================================================================================================================================================

void AliMFT::StepManager() {

  // Full Step Manager

  AliDebug(2, Form("Entering StepManager: TVirtualMC::GetMC()->CurrentVolName() = %s", TVirtualMC::GetMC()->CurrentVolName()));

  if (!fSegmentation) AliFatal("No segmentation available");

  if (!(this->IsActive())) return;
  if (!(TVirtualMC::GetMC()->TrackCharge())) return;

  TString planeNumber   = TVirtualMC::GetMC()->CurrentVolName();
  TString detElemNumber = TVirtualMC::GetMC()->CurrentVolName();
  if (!(planeNumber.Contains("active"))) return;
  planeNumber.Remove(0,9);
  detElemNumber.Remove(0,18);
  planeNumber.Remove(2);
  detElemNumber.Remove(3);
  Int_t detElemID = fSegmentation->GetDetElemGlobalID(planeNumber.Atoi(), detElemNumber.Atoi());

  if (TVirtualMC::GetMC()->IsTrackExiting()) {
    AddTrackReference(gAlice->GetMCApp()->GetCurrentTrackNumber(), AliTrackReference::kMFT);
  }

  static TLorentzVector position, momentum;
  static AliMFTHit hit;
  
  Int_t  status = 0;
  
  // Track status
  if (TVirtualMC::GetMC()->IsTrackInside())      status +=  1;
  if (TVirtualMC::GetMC()->IsTrackEntering())    status +=  2;
  if (TVirtualMC::GetMC()->IsTrackExiting())     status +=  4;
  if (TVirtualMC::GetMC()->IsTrackOut())         status +=  8;
  if (TVirtualMC::GetMC()->IsTrackDisappeared()) status += 16;
  if (TVirtualMC::GetMC()->IsTrackStop())        status += 32;
  if (TVirtualMC::GetMC()->IsTrackAlive())       status += 64;

  // ---------- Fill hit structure

  hit.SetDetElemID(detElemID);
  hit.SetPlane(planeNumber.Atoi());
  hit.SetTrack(gAlice->GetMCApp()->GetCurrentTrackNumber());
    
  TVirtualMC::GetMC()->TrackPosition(position);
  TVirtualMC::GetMC()->TrackMomentum(momentum);

  AliDebug(1, Form("AliMFT::StepManager()->%s Hit #%06d (x=%f, y=%f, z=%f) belongs to track %02d\n", 
		   TVirtualMC::GetMC()->CurrentVolName(), fNhits, position.X(), position.Y(), position.Z(), gAlice->GetMCApp()->GetCurrentTrackNumber())); 

  hit.SetPosition(position);
  hit.SetTOF(TVirtualMC::GetMC()->TrackTime());
  hit.SetMomentum(momentum);
  hit.SetStatus(status);
  hit.SetEloss(TVirtualMC::GetMC()->Edep());
  //  hit.SetShunt(GetIshunt());
//   if (TVirtualMC::GetMC()->IsTrackEntering()) {
//     hit.SetStartPosition(position);
//     hit.SetStartTime(TVirtualMC::GetMC()->TrackTime());
//     hit.SetStartStatus(status);
//     return; // don't save entering hit.
//   } 

  // Fill hit structure with this new hit.
  new ((*fHits)[fNhits++]) AliMFTHit(hit);

  // Save old position... for next hit.
//   hit.SetStartPosition(position);
//   hit.SetStartTime(TVirtualMC::GetMC()->TrackTime());
//   hit.SetStartStatus(status);

  return;

}

//====================================================================================================================================================

TGeoVolumeAssembly* AliMFT::CreateVol() {

  // method to create the MFT Geometry (silicon circular planes)

  if (!fSegmentation) CreateGeometry();
  
  TGeoVolumeAssembly *vol = new TGeoVolumeAssembly("MFT");
  TGeoMedium *silicon = gGeoManager->GetMedium("MFT_Si");
  TGeoMedium *readout = gGeoManager->GetMedium("MFT_Readout");
  TGeoMedium *support = gGeoManager->GetMedium("MFT_Support");
  TGeoMedium *carbon  = gGeoManager->GetMedium("MFT_Carbon");
  TGeoMedium *alu     = gGeoManager->GetMedium("MFT_Alu");
  TGeoMedium *water   = gGeoManager->GetMedium("MFT_Water");
  TGeoMedium *si02    = gGeoManager->GetMedium("MFT_SiO2");
  TGeoMedium *inox    = gGeoManager->GetMedium("MFT_Inox");

  // ---- Cage & Services Description --------------------------------------------
  // R. Tieulent - 17/01/2014 - Basic description for ITS/TPC matching studies
  // -----------------------------------------------------------------------------

  TGeoVolumeAssembly *cageNservices = new TGeoVolumeAssembly("MFT_cageNservices");
  
  // cage definition
  Float_t cageDz   = 150./2.;
  Float_t cageRMax = 49.5;
  Float_t cageRMin = cageRMax - 0.120; // 0.64% of X0
  
  TGeoVolume *cage = gGeoManager->MakeTube("MFT_cage", carbon, cageRMin, cageRMax, cageDz);
  cage->SetLineColor(kBlue);

  cageNservices->AddNode(cage,1,new TGeoTranslation(0., 0., 0. ));

  // Services definition
  TGeoVolumeAssembly *services = new TGeoVolumeAssembly("MFT_services");

  // Aluminum bus-Bar
  Float_t busBarDz = 150.;
  Float_t busBarThick = 0.1 ;
  Float_t busBarWidth = 1.;
  
  TGeoVolume *aluBusBar = gGeoManager->MakeBox("MFT_busbar", alu, busBarWidth/2., busBarThick/2., busBarDz/2.);
  aluBusBar->SetLineColor(kYellow);

  Int_t nBusBar = 30;
  Float_t dPhiBusBar = 2.*TMath::Pi() / nBusBar;
  Float_t dShift = cageRMin - busBarThick;
  
  TGeoRotation *rot;

  for (Int_t iBusBar=0; iBusBar<nBusBar; iBusBar++) {
    Float_t phi =  dPhiBusBar*iBusBar;
    Float_t xp = dShift*TMath::Cos(phi);
    Float_t yp = dShift*TMath::Sin(phi);
    rot = new TGeoRotation();
    rot->RotateZ(phi*TMath::RadToDeg()+90.);
    services->AddNode(aluBusBar, iBusBar+1, new TGeoCombiTrans(xp,yp,0,rot));
  }
  
  // Cooling Services  definition
  TGeoVolumeAssembly *cooling = new TGeoVolumeAssembly("MFT_cooling");
  // Cooling Water
  Float_t coolingDz = 150.;
  Float_t coolingR  = 0.3 /2. ; // 3mm in diameter
  Float_t coolingDR = 0.02 ;    // Thickness of the pipe 0.2mm
  
  TGeoVolume *coolingWater = gGeoManager->MakeTube("MFT_coolingWater", water, 0., coolingR, coolingDz/2.);
  coolingWater->SetLineColor(kCyan);
  cooling->AddNode(coolingWater, 1, new TGeoTranslation(0,0,0 ));

  // Cooling Pipes
  TGeoVolume *coolingPipes = gGeoManager->MakeTube("MFT_coolingPipes", inox, coolingR, coolingR+coolingDR, coolingDz/2.);
  coolingPipes->SetLineColor(kGray);
  cooling->AddNode(coolingPipes,1,new TGeoTranslation(0,0,0 ));
  
  Int_t nCooling = 18;
  dShift = cageRMin - coolingR ;
  Float_t phi0 = 0.02;
  
  for (Int_t iCooling=0; iCooling<nCooling; iCooling++) {
    Float_t phi ;
    if (iCooling<nCooling/2) phi = dPhiBusBar*(iCooling+3) + phi0;
    else phi = dPhiBusBar*(iCooling+9) + phi0;
    Float_t xp = dShift*TMath::Cos(phi);
    Float_t yp = dShift*TMath::Sin(phi);
    services->AddNode(cooling, iCooling+1, new TGeoTranslation(xp,yp,0 ));
  }
  
  // Optical Fibers
  Float_t fiberDz = 150.;
  Float_t fiberRadius = 0.0125 /2. ; // 0.125mm in diameter
  
  TGeoVolume *fiber = gGeoManager->MakeTube("MFT_fiber", si02, 0., fiberRadius, fiberDz/2.);
  fiber->SetLineColor(kCyan);

  Int_t nFiber = 340;
  dShift = cageRMin - 2*fiberRadius;
  phi0 = 0.03;
  
  for (Int_t iFiber=0; iFiber<nFiber; iFiber++) {
    Float_t phi = dPhiBusBar*(Int_t)(iFiber/11) - phi0-(iFiber%11)*2.*TMath::ATan(fiberRadius/dShift);
    Float_t xp  = dShift*TMath::Cos(phi);
    Float_t yp  = dShift*TMath::Sin(phi);
    services->AddNode(fiber, iFiber+1, new TGeoTranslation(xp,yp,0 ));
  }

  cageNservices->AddNode(services, 1, new TGeoTranslation(0., 0., 0. ));
  
  vol->AddNode(cageNservices,1,new TGeoTranslation(0., 0., 0. ));
  
  // ------------------- Creating volumes for MFT planes --------------------------------

  Double_t origin[3] = {0};

  for (Int_t iPlane=0; iPlane<fNPlanes; iPlane++) {

    AliDebug(1, Form("Creating volumes for MFT plane %02d",iPlane));

    AliMFTPlane *plane = fSegmentation->GetPlane(iPlane);

    // --------- support element(s)
    
    origin[0] =  0.5*(plane->GetSupportElement(0)->GetAxis(0)->GetXmax() + plane->GetSupportElement(0)->GetAxis(0)->GetXmin());
    origin[1] =  0.5*(plane->GetSupportElement(0)->GetAxis(1)->GetXmax() + plane->GetSupportElement(0)->GetAxis(1)->GetXmin());
    origin[2] = -0.5*(plane->GetSupportElement(0)->GetAxis(2)->GetXmax() + plane->GetSupportElement(0)->GetAxis(2)->GetXmin());
    TGeoVolume *supportElem = gGeoManager->MakeTube(Form("MFT_plane%02d_support", iPlane), support, 
						    plane->GetRMinSupport(), 
						    plane->GetRMaxSupport(),
						    0.5*(plane->GetSupportElement(0)->GetAxis(2)->GetXmax() - 
							 plane->GetSupportElement(0)->GetAxis(2)->GetXmin()) );
    AliDebug(2, Form("Created vol %s", supportElem->GetName()));
    supportElem->SetLineColor(kCyan-9);
    vol -> AddNode(supportElem, 0, new TGeoTranslation(origin[0], origin[1], origin[2]));

    AliDebug(1, "support elements created!");
    
    // --------- active elements

    for (Int_t iActive=0; iActive<plane->GetNActiveElements(); iActive++) {
      
      Double_t dx = 0.5*TMath::Abs(plane->GetActiveElement(iActive)->GetAxis(0)->GetXmax() - plane->GetActiveElement(iActive)->GetAxis(0)->GetXmin());
      Double_t dy = 0.5*TMath::Abs(plane->GetActiveElement(iActive)->GetAxis(1)->GetXmax() - plane->GetActiveElement(iActive)->GetAxis(1)->GetXmin());
      Double_t dz = 0.5*TMath::Abs(plane->GetActiveElement(iActive)->GetAxis(2)->GetXmax() - plane->GetActiveElement(iActive)->GetAxis(2)->GetXmin());
      dz /= Double_t(fNSlices);

      origin[0] =  0.5*(plane->GetActiveElement(iActive)->GetAxis(0)->GetXmax() + plane->GetActiveElement(iActive)->GetAxis(0)->GetXmin());
      origin[1] =  0.5*(plane->GetActiveElement(iActive)->GetAxis(1)->GetXmax() + plane->GetActiveElement(iActive)->GetAxis(1)->GetXmin());

      for (Int_t iSlice=0; iSlice<fNSlices; iSlice++) {
	origin[2] = -0.5*(plane->GetActiveElement(iActive)->GetAxis(2)->GetXmin() + 2*dz*(iSlice+1) + plane->GetActiveElement(iActive)->GetAxis(2)->GetXmin() + 2*dz*(iSlice) );
	TGeoVolume *activeElem = gGeoManager->MakeBox(Form("MFT_plane%02d_active%03d_slice%02d", iPlane, iActive, iSlice), silicon, dx, dy, dz);
	AliDebug(2, Form("Created vol %s", activeElem->GetName()));
	activeElem->SetLineColor(kGreen);
	vol -> AddNode(activeElem, 0, new TGeoTranslation(origin[0], origin[1], origin[2]));
      }

    }

    AliDebug(1, "active elements created!");

    // --------- readout elements

    for (Int_t iReadout=0; iReadout<plane->GetNReadoutElements(); iReadout++) {
      
      Double_t dx = 0.5*TMath::Abs(plane->GetReadoutElement(iReadout)->GetAxis(0)->GetXmax() - plane->GetReadoutElement(iReadout)->GetAxis(0)->GetXmin());
      Double_t dy = 0.5*TMath::Abs(plane->GetReadoutElement(iReadout)->GetAxis(1)->GetXmax() - plane->GetReadoutElement(iReadout)->GetAxis(1)->GetXmin());
      Double_t dz = 0.5*TMath::Abs(plane->GetReadoutElement(iReadout)->GetAxis(2)->GetXmax() - plane->GetReadoutElement(iReadout)->GetAxis(2)->GetXmin());

      origin[0] =  0.5*(plane->GetReadoutElement(iReadout)->GetAxis(0)->GetXmax() + plane->GetReadoutElement(iReadout)->GetAxis(0)->GetXmin());
      origin[1] =  0.5*(plane->GetReadoutElement(iReadout)->GetAxis(1)->GetXmax() + plane->GetReadoutElement(iReadout)->GetAxis(1)->GetXmin());
      origin[2] = -0.5*(plane->GetReadoutElement(iReadout)->GetAxis(2)->GetXmax() + plane->GetReadoutElement(iReadout)->GetAxis(2)->GetXmin());

      TGeoVolume *readoutElem = gGeoManager->MakeBox(Form("MFT_plane%02d_readout%03d", iPlane, iReadout), readout, dx, dy, dz);
      AliDebug(2, Form("Created vol %s", readoutElem->GetName()));
      readoutElem->SetLineColor(kRed);
      vol -> AddNode(readoutElem, 0, new TGeoTranslation(origin[0], origin[1], origin[2]));
      
    }

    AliDebug(1, "readout elements created!");

  }

  return vol;

}

//====================================================================================================================================================

void AliMFT::Hits2SDigits(){
  
  // Interface method invoked from AliSimulation to create a list of sdigits corresponding to list of hits. Every hit generates one sdigit.

  AliDebug(1,"Start Hits2SDigits.");
  
  if (!fSegmentation) CreateGeometry();
 
  if (!fLoader->TreeH()) fLoader->LoadHits();

  if (!fLoader->TreeS()) {

    for (Int_t iEvt=0;iEvt<fLoader->GetRunLoader()->GetNumberOfEvents(); iEvt++) {

      fLoader->GetRunLoader()->GetEvent(iEvt);
      fLoader->MakeTree("S");
      MakeBranch("S");
      SetTreeAddress();

      AliDebug(1, Form("Event %03d: fLoader->TreeH()->GetEntries() = %2d", iEvt, Int_t(fLoader->TreeH()->GetEntries())));

      for (Int_t iTrack=0; iTrack<fLoader->TreeH()->GetEntries(); iTrack++) {
	fLoader->TreeH()->GetEntry(iTrack);     
	Hits2SDigitsLocal(Hits(), GetSDigitsList(), iTrack);    // convert these hits to a list of sdigits  
      }
      
      fLoader->TreeS()->Fill();
      fLoader->WriteSDigits("OVERWRITE");
      ResetSDigits();

    }
  }

  fLoader->UnloadHits();
  fLoader->UnloadSDigits();

  AliDebug(1,"Stop Hits2SDigits.");
  
}

//====================================================================================================================================================

void AliMFT::Hits2SDigitsLocal(TClonesArray *hits, const TObjArray *pSDig, Int_t track) {

  //  Add sdigits of these hits to the list
  
  AliDebug(1, "Entering Hits2SDigitsLocal");
  
  if (!fSegmentation) CreateGeometry();
  
  TClonesArray *pSDigList[fNMaxPlanes];
  for (Int_t iPlane=0; iPlane<fNMaxPlanes; iPlane++) pSDigList[iPlane] = NULL; 
  for (Int_t iPlane=0; iPlane<fNPlanes;    iPlane++) { 
    pSDigList[iPlane] = (TClonesArray*) (*pSDig)[iPlane];
    AliDebug(1,Form("Entries of pSDigList %3d; plane: %02d,",pSDigList[iPlane]->GetEntries(),iPlane));
    if (!track && pSDigList[iPlane]->GetEntries()!=0) AliErrorClass("Some of sdigits lists is not empty");
  }
  
  for (Int_t iHit=0; iHit<hits->GetEntries(); iHit++) {

    AliMFTHit *hit = (AliMFTHit*) hits->At(iHit);

    // Creating "main digit"

    AliMFTDigit *mainSDigit = new AliMFTDigit();
    mainSDigit->SetEloss(hit->GetEloss());
    mainSDigit->SetDetElemID(hit->GetDetElemID());
    mainSDigit->SetPlane(hit->GetPlane());
    mainSDigit->AddMCLabel(hit->GetTrack()); 

    Int_t xPixel = -1;
    Int_t yPixel = -1;
    if (fSegmentation->Hit2PixelID(hit->X(), hit->Y(), mainSDigit->GetDetElemID(), xPixel, yPixel)) {
      mainSDigit->SetPixID(xPixel, yPixel, 0);
      mainSDigit->SetPixWidth(fSegmentation->GetPixelSizeX(mainSDigit->GetDetElemID()), 
			  fSegmentation->GetPixelSizeY(mainSDigit->GetDetElemID()),
			  fSegmentation->GetPixelSizeZ(mainSDigit->GetDetElemID()));  
      mainSDigit->SetPixCenter(fSegmentation->GetPixelCenterX(mainSDigit->GetDetElemID(), xPixel), 
			   fSegmentation->GetPixelCenterY(mainSDigit->GetDetElemID(), yPixel),
			   fSegmentation->GetPixelCenterZ(mainSDigit->GetDetElemID(), 0));
      new ((*fSideDigits)[fSideDigits->GetEntries()]) AliMFTDigit(*mainSDigit);
      AliDebug(1, Form("Created new sdigit (%f, %f, %f) from hit (%f, %f, %f)",
       		       mainSDigit->GetPixelCenterX(), mainSDigit->GetPixelCenterY(), mainSDigit->GetPixelCenterZ(), hit->X(), hit->Y(), hit->Z()));
    }

    // creating "side digits" to simulate the effect of charge dispersion

    Double_t pi4 = TMath::Pi()/4.;
    for (Int_t iStep=0; iStep<fNStepForChargeDispersion; iStep++) {
      Double_t shift = (iStep+1) * fSingleStepForChargeDispersion;
      for (Int_t iAngle=0; iAngle<8; iAngle++) {
	Double_t shiftX = shift*TMath::Cos(iAngle*pi4);
	Double_t shiftY = shift*TMath::Sin(iAngle*pi4);
	if (fSegmentation->Hit2PixelID(hit->X()+shiftX, hit->Y()+shiftY, hit->GetDetElemID(), xPixel, yPixel)) {
	  Bool_t digitExists = kFALSE;
	  for (Int_t iSideDigit=0; iSideDigit<fSideDigits->GetEntries(); iSideDigit++) {
	    if (xPixel==((AliMFTDigit*) fSideDigits->At(iSideDigit))->GetPixelX() && 
		yPixel==((AliMFTDigit*) fSideDigits->At(iSideDigit))->GetPixelY()) {
	      digitExists = kTRUE;
	      break;
	    }
	  }
	  if (!digitExists) {
	    AliMFTDigit *sideSDigit = new AliMFTDigit();
	    sideSDigit->SetEloss(0.);
	    sideSDigit->SetDetElemID(hit->GetDetElemID());
	    sideSDigit->SetPlane(hit->GetPlane());
	    sideSDigit->AddMCLabel(hit->GetTrack());
	    sideSDigit->SetPixID(xPixel, yPixel, 0);
	    sideSDigit->SetPixWidth(fSegmentation->GetPixelSizeX(sideSDigit->GetDetElemID()), 
				    fSegmentation->GetPixelSizeY(sideSDigit->GetDetElemID()),
				    fSegmentation->GetPixelSizeZ(sideSDigit->GetDetElemID()));  
	    sideSDigit->SetPixCenter(fSegmentation->GetPixelCenterX(sideSDigit->GetDetElemID(), xPixel), 
				     fSegmentation->GetPixelCenterY(sideSDigit->GetDetElemID(), yPixel),
				     fSegmentation->GetPixelCenterZ(sideSDigit->GetDetElemID(), 0)); 
	    new ((*fSideDigits)[fSideDigits->GetEntries()]) AliMFTDigit(*sideSDigit);
	  }
	}
      }
    }
    
    // ------------ In case we should simulate a rectangular pattern of pixel...
    
    if (fSegmentation->GetPlane(mainSDigit->GetPlane())->HasPixelRectangularPatternAlongY()) {
      for (Int_t iSDigit=0; iSDigit<fSideDigits->GetEntries(); iSDigit++) {
	AliMFTDigit *mySDig = (AliMFTDigit*) (fSideDigits->At(iSDigit));
	if (mySDig->GetPixelX()%2 == mySDig->GetPixelY()%2) {   // both pair or both odd
	  xPixel = mySDig->GetPixelX();
	  yPixel = mySDig->GetPixelY()+1;
	  if (fSegmentation->DoesPixelExist(mySDig->GetDetElemID(), xPixel, yPixel)) {
	    AliMFTDigit *newSDigit = new AliMFTDigit();
	    newSDigit->SetEloss(0.);
	    newSDigit->SetDetElemID(mySDig->GetDetElemID());
	    newSDigit->SetPlane(mySDig->GetDetElemID());
	    newSDigit->SetPixID(xPixel, yPixel, 0);
	    newSDigit->SetPixWidth(fSegmentation->GetPixelSizeX(newSDigit->GetDetElemID()), 
				   fSegmentation->GetPixelSizeY(newSDigit->GetDetElemID()),
				   fSegmentation->GetPixelSizeZ(newSDigit->GetDetElemID()));  
	    newSDigit->SetPixCenter(fSegmentation->GetPixelCenterX(newSDigit->GetDetElemID(), xPixel), 
				    fSegmentation->GetPixelCenterY(newSDigit->GetDetElemID(), yPixel),
				    fSegmentation->GetPixelCenterZ(newSDigit->GetDetElemID(), 0)); 
	    new ((*fSideDigits)[fSideDigits->GetEntries()]) AliMFTDigit(*newSDigit);
	  }
	}
	else {   // pair-odd
	  xPixel = mySDig->GetPixelX();
	  yPixel = mySDig->GetPixelY()-1;
	  if (fSegmentation->DoesPixelExist(mySDig->GetDetElemID(), xPixel, yPixel)) {
	    AliMFTDigit *newSDigit = new AliMFTDigit();
	    newSDigit->SetEloss(0.);
	    newSDigit->SetDetElemID(mySDig->GetDetElemID());
	    newSDigit->SetPlane(mySDig->GetPlane());
	    newSDigit->SetPixID(xPixel, yPixel, 0);
	    newSDigit->SetPixWidth(fSegmentation->GetPixelSizeX(newSDigit->GetDetElemID()), 
				   fSegmentation->GetPixelSizeY(newSDigit->GetDetElemID()),
				   fSegmentation->GetPixelSizeZ(newSDigit->GetDetElemID()));  
	    newSDigit->SetPixCenter(fSegmentation->GetPixelCenterX(newSDigit->GetDetElemID(), xPixel), 
				    fSegmentation->GetPixelCenterY(newSDigit->GetDetElemID(), yPixel),
				    fSegmentation->GetPixelCenterZ(newSDigit->GetDetElemID(), 0)); 
	    new ((*fSideDigits)[fSideDigits->GetEntries()]) AliMFTDigit(*newSDigit);
	  }
	}
      }
    }

    // -------- checking which pixels switched on have their diode actually within the charge dispersion radius

    for (Int_t iSDigit=0; iSDigit<fSideDigits->GetEntries(); iSDigit++) {
      AliMFTDigit *mySDig = (AliMFTDigit*) (fSideDigits->At(iSDigit));
      Double_t distance = TMath::Sqrt(TMath::Power(mySDig->GetPixelCenterX()-hit->X(),2) + TMath::Power(mySDig->GetPixelCenterY()-hit->Y(),2));
      if (fSegmentation->GetPlane(mySDig->GetPlane())->HasPixelRectangularPatternAlongY()) {
	if (mySDig->GetPixelX()%2 == mySDig->GetPixelY()%2) {  // both pair or both odd
	  if (distance<fChargeDispersion) {
	    AliDebug(1, Form("Created new sdigit (%f, %f, %f) from hit (%f, %f, %f)",
			     mySDig->GetPixelCenterX(), mySDig->GetPixelCenterY(), mySDig->GetPixelCenterZ(), hit->X(), hit->Y(), hit->Z()));
	    new ((*pSDigList[mySDig->GetPlane()])[pSDigList[mySDig->GetPlane()]->GetEntries()]) AliMFTDigit(*mySDig);
	    xPixel = mySDig->GetPixelX();
	    yPixel = mySDig->GetPixelY()+1;
	    if (fSegmentation->DoesPixelExist(mySDig->GetDetElemID(), xPixel, yPixel)) {
	      AliMFTDigit *newSDigit = new AliMFTDigit();
	      newSDigit->SetEloss(0.);
	      newSDigit->SetDetElemID(mySDig->GetDetElemID());
	      newSDigit->SetPlane(mySDig->GetPlane());
	      newSDigit->SetPixID(xPixel, yPixel, 0);
	      newSDigit->SetPixWidth(fSegmentation->GetPixelSizeX(newSDigit->GetDetElemID()), 
				     fSegmentation->GetPixelSizeY(newSDigit->GetDetElemID()),
				     fSegmentation->GetPixelSizeZ(newSDigit->GetDetElemID()));  
	      newSDigit->SetPixCenter(fSegmentation->GetPixelCenterX(newSDigit->GetDetElemID(), xPixel), 
				      fSegmentation->GetPixelCenterY(newSDigit->GetDetElemID(), yPixel),
				      fSegmentation->GetPixelCenterZ(newSDigit->GetDetElemID(), 0));
	      AliDebug(1, Form("Created new sdigit (%f, %f, %f) from hit (%f, %f, %f)",
			       newSDigit->GetPixelCenterX(), newSDigit->GetPixelCenterY(), newSDigit->GetPixelCenterZ(), hit->X(), hit->Y(), hit->Z()));
	      new ((*pSDigList[newSDigit->GetPlane()])[pSDigList[newSDigit->GetPlane()]->GetEntries()]) AliMFTDigit(*newSDigit);
	    }
	  }
	}
      }
      else {
	if (distance<fChargeDispersion) {
	  AliDebug(1, Form("Created new sdigit (%f, %f, %f) from hit (%f, %f, %f)",
			   mySDig->GetPixelCenterX(), mySDig->GetPixelCenterY(), mySDig->GetPixelCenterZ(), hit->X(), hit->Y(), hit->Z()));
	  new ((*pSDigList[mySDig->GetPlane()])[pSDigList[mySDig->GetPlane()]->GetEntries()]) AliMFTDigit(*mySDig);
	}
      }
    }

    fSideDigits->Delete(); 

  }

  AliDebug(1,"Exiting Hits2SDigitsLocal");

}

//====================================================================================================================================================

void AliMFT::MakeBranch(Option_t *option) {

  // Create Tree branches 
  AliDebug(1, Form("Start with option= %s.",option));
  
  const Int_t kBufSize = 4000;
  
  const Char_t *cH = strstr(option,"H");
  const Char_t *cD = strstr(option,"D");
  const Char_t *cS = strstr(option,"S");

  if (cH && fLoader->TreeH()) {
    CreateHits();
    MakeBranchInTree(fLoader->TreeH(), "MFT", &fHits, kBufSize, 0);   
  }

  if (cS && fLoader->TreeS()) {
    CreateSDigits();
    for(Int_t iPlane=0; iPlane<fNPlanes; iPlane++) MakeBranchInTree(fLoader->TreeS(), 
								    Form("Plane_%02d",iPlane), 
								    &((*fSDigitsPerPlane)[iPlane]), 
								    kBufSize, 0);
  }
  
  if (cD && fLoader->TreeD()) {
    CreateDigits();
    for(Int_t iPlane=0; iPlane<fNPlanes; iPlane++) MakeBranchInTree(fLoader->TreeD(), 
								    Form("Plane_%02d",iPlane), 
								    &((*fDigitsPerPlane)[iPlane]),
								    kBufSize, 0);
  }

  AliDebug(1,"Stop.");

}

//====================================================================================================================================================

void AliMFT::SetTreeAddress() {

  AliDebug(1, "AliMFT::SetTreeAddress()");

  //Set branch address for the Hits and Digits Tree.
  AliDebug(1, "Start.");

  AliDebug(1, Form("AliMFT::SetTreeAddress Hits  fLoader->TreeH() = %p\n", fLoader->TreeH()));
  if (fLoader->TreeH() && fLoader->TreeH()->GetBranch("MFT")) {
    CreateHits();
    fLoader->TreeH()->SetBranchAddress("MFT", &fHits);
  }
    
  AliDebug(1, Form("AliMFT::SetTreeAddress SDigits  fLoader->TreeS() = %p\n", fLoader->TreeS()));
  if (fLoader->TreeS() && fLoader->TreeS()->GetBranch("Plane_00")) {
    CreateSDigits();
    for(Int_t iPlane=0; iPlane<fNPlanes; iPlane++) {
      fLoader->TreeS()->SetBranchAddress(Form("Plane_%02d",iPlane), &((*fSDigitsPerPlane)[iPlane]));
    }
  }
    
  AliDebug(1, Form("AliMFT::SetTreeAddress Digits  fLoader->TreeD() = %p\n", fLoader->TreeD()));
  if (fLoader->TreeD() && fLoader->TreeD()->GetBranch("Plane_00")) {
    CreateDigits(); 
    for(Int_t iPlane=0; iPlane<fNPlanes; iPlane++) {
      fLoader->TreeD()->SetBranchAddress(Form("Plane_%02d",iPlane), &((*fDigitsPerPlane)[iPlane]));
    }
  }

  AliDebug(1, Form("AliMFT::SetTreeAddress RecPoints  fLoader->TreeR() = %p\n", fLoader->TreeR()));
  if (fLoader->TreeR() && fLoader->TreeR()->GetBranch("Plane_00")) {
    CreateRecPoints(); 
    for(Int_t iPlane=0; iPlane<fNPlanes; iPlane++) {
      fLoader->TreeR()->SetBranchAddress(Form("Plane_%02d",iPlane), &((*fRecPointsPerPlane)[iPlane]));
    }
  }

  AliDebug(1,"Stop.");

}

//====================================================================================================================================================

void AliMFT::SetGeometry() {

  AliInfo("AliMFT::SetGeometry\n");

  fSegmentation = new AliMFTSegmentation(fNameGeomFile.Data());

  fNPlanes = fSegmentation->GetNPlanes();

}

//====================================================================================================================================================

void AliMFT::CreateHits() { 

  // create array of hits

  AliDebug(1, "AliMFT::CreateHits()");

  if (fHits) return;    
  fHits = new TClonesArray("AliMFTHit");  

}

//====================================================================================================================================================

void AliMFT::CreateSDigits() { 
 
  // create sdigits list

  AliDebug(1, "AliMFT::CreateSDigits()");
 
  if (fSDigitsPerPlane) return; 
  fSDigitsPerPlane = new TObjArray(fNPlanes); 
  for (Int_t iPlane=0; iPlane<fNPlanes; iPlane++) fSDigitsPerPlane->AddAt(new TClonesArray("AliMFTDigit"), iPlane);

  fSideDigits = new TClonesArray("AliMFTDigit");

}

//====================================================================================================================================================

void AliMFT::CreateDigits() {

  // create digits list

  AliDebug(1, "AliMFT::CreateDigits()");

  if (fDigitsPerPlane) return; 
  fDigitsPerPlane = new TObjArray(fNPlanes);
  for(Int_t iPlane=0; iPlane<fNPlanes; iPlane++) fDigitsPerPlane->AddAt(new TClonesArray("AliMFTDigit"), iPlane);

}

//====================================================================================================================================================

void AliMFT::CreateRecPoints() {

  // create recPoints list

  AliDebug(1, "AliMFT::CreateRecPoints()");

  if (fRecPointsPerPlane) return; 
  fRecPointsPerPlane = new TObjArray(fNPlanes);
  for(Int_t iPlane=0; iPlane<fNPlanes; iPlane++) fRecPointsPerPlane->AddAt(new TClonesArray("AliMFTCluster"), iPlane);

}

//====================================================================================================================================================

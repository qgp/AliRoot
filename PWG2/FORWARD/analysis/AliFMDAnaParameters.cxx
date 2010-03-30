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
//
//The design of this class is based on the AliFMDParameters class. Its purpose
//is to hold parameters for the analysis such as background correction and 
//fit functions.
//
//Author: Hans Hjersing Dalsgaard, NBI, hans.dalsgaard@cern.ch
//

#include "AliFMDDebug.h"		   // ALILOG_H
#include "AliFMDAnaParameters.h"	   // ALIFMDPARAMETERS_H
//#include <AliCDBManager.h>         // ALICDBMANAGER_H
//#include <AliCDBEntry.h>           // ALICDBMANAGER_H
//#include "AliFMDRing.h"
#include <AliLog.h>
#include <Riostream.h>
#include <sstream>
#include <TSystem.h>
#include <TH2D.h>
#include <TF1.h>
#include <TMath.h>
#include "AliTriggerAnalysis.h"
#include "AliPhysicsSelection.h"
//#include "AliBackgroundSelection.h"
#include "AliESDEvent.h"
#include "AliESDVertex.h"

//====================================================================
ClassImp(AliFMDAnaParameters)
#if 0
  ; // This is here to keep Emacs for indenting the next line
#endif

//const char* AliFMDAnaParameters::fgkBackgroundCorrection  = "FMD/Correction/Background";
//const char* AliFMDAnaParameters::fgkEnergyDists = "FMD/Correction/EnergyDistribution";
const char* AliFMDAnaParameters::fgkBackgroundID         = "background";
const char* AliFMDAnaParameters::fgkEnergyDistributionID = "energydistributions";
const char* AliFMDAnaParameters::fgkEventSelectionEffID  = "eventselectionefficiency";
const char* AliFMDAnaParameters::fgkSharingEffID         = "sharingefficiency";
//____________________________________________________________________
AliFMDAnaParameters* AliFMDAnaParameters::fgInstance = 0;

//____________________________________________________________________

AliFMDAnaParameters* 
AliFMDAnaParameters::Instance() 
{
  // Get static instance 
  if (!fgInstance) fgInstance = new AliFMDAnaParameters;
  return fgInstance;
}

//____________________________________________________________________
AliFMDAnaParameters::AliFMDAnaParameters() :
  fIsInit(kFALSE),
  fBackground(0),
  fEnergyDistribution(0),
  fEventSelectionEfficiency(0),
  fSharingEfficiency(0),
  fCorner1(4.2231, 26.6638),
  fCorner2(1.8357, 27.9500),
  fEnergyPath("$ALICE_ROOT/PWG2/FORWARD/corrections/EnergyDistribution"),
  fBackgroundPath("$ALICE_ROOT/PWG2/FORWARD/corrections/Background"),
  fEventSelectionEffPath("$ALICE_ROOT/PWG2/FORWARD/corrections/EventSelectionEfficiency"),
  fSharingEffPath("$ALICE_ROOT/PWG2/FORWARD/corrections/SharingEfficiency"),
  fProcessPrimary(kFALSE),
  fProcessHits(kFALSE),
  fTrigger(kMB1),
  fEnergy(k10000),
  fMagField(k5G),
  fSpecies(kPP),
  fPhysicsSelection(0),
  fRealData(kFALSE),
  fSPDlowLimit(0),
  fSPDhighLimit(999999999),
  fCentralSelection(kFALSE)
{
  fPhysicsSelection = new AliPhysicsSelection;
  //AliBackgroundSelection* backgroundSelection = new AliBackgroundSelection("bg","bg");
  
  //fPhysicsSelection->AddBackgroundIdentification(backgroundSelection);
  //fPhysicsSelection->Initialize(104792);
  // Do not use this - it is only for IO 
  fgInstance = this;
  // Default constructor 
}
//____________________________________________________________________
char* AliFMDAnaParameters::GetPath(const char* species) {
  
  char* path = "";
  
  if(species == fgkBackgroundID)
    path = Form("%s/%s_%d_%d_%d_%d_%d_%d.root",
		fBackgroundPath.Data(),
		fgkBackgroundID,
		fEnergy,
		fTrigger,
		fMagField,
		fSpecies,
		0,
		0);
  if(species == fgkEnergyDistributionID)
    path = Form("%s/%s_%d_%d_%d_%d_%d_%d.root",
		fEnergyPath.Data(),
		fgkEnergyDistributionID,
		fEnergy,
		fTrigger,
		fMagField,
		fSpecies,
		0,
		0);
  if(species == fgkEventSelectionEffID)
    path = Form("%s/%s_%d_%d_%d_%d_%d_%d.root",
		fEventSelectionEffPath.Data(),
		fgkEventSelectionEffID,
		fEnergy,
		fTrigger,
		fMagField,
		fSpecies,
		0,
		0);
  if(species == fgkSharingEffID)
    path = Form("%s/%s_%d_%d_%d_%d_%d_%d.root",
		fSharingEffPath.Data(),
		fgkSharingEffID,
		fEnergy,
		fTrigger,
		fMagField,
		fSpecies,
		0,
		0);

  return path;
}
//____________________________________________________________________
void AliFMDAnaParameters::Init(Bool_t forceReInit, UInt_t what)
{
  // Initialize the parameters manager.  We need to get stuff from the
  // CDB here. 
  if (forceReInit) fIsInit = kFALSE;
  if (fIsInit) return;
  if (what & kBackgroundCorrection)       InitBackground();
  if (what & kEnergyDistributions)        InitEnergyDists();
  if (what & kEventSelectionEfficiency)   InitEventSelectionEff();
  if (what & kSharingEfficiency)          InitSharingEff();
  

  
  fIsInit = kTRUE;
}
//____________________________________________________________________

void AliFMDAnaParameters::InitBackground() {
  
  //AliCDBEntry*   background = GetEntry(fgkBackgroundCorrection);
  
  TFile* fin = TFile::Open(GetPath(fgkBackgroundID));
  
  if (!fin) return;
  
  fBackground = dynamic_cast<AliFMDAnaCalibBackgroundCorrection*>(fin->Get(fgkBackgroundID));
  if (!fBackground) AliFatal("Invalid background object from CDB");
  
}

//____________________________________________________________________

void AliFMDAnaParameters::InitEnergyDists() {
  
  TFile* fin = TFile::Open(GetPath(fgkEnergyDistributionID));
  //AliCDBEntry*   edist = GetEntry(fgkEnergyDists);
  if (!fin) return;
  
  fEnergyDistribution = dynamic_cast<AliFMDAnaCalibEnergyDistribution*>(fin->Get(fgkEnergyDistributionID));
  
  if (!fEnergyDistribution) AliFatal("Invalid background object from CDB");
  
}

//____________________________________________________________________

void AliFMDAnaParameters::InitEventSelectionEff() {
  
  //AliCDBEntry*   background = GetEntry(fgkBackgroundCorrection);
  TFile* fin = TFile::Open(GetPath(fgkEventSelectionEffID));
			    
  if (!fin) return;
  
  fEventSelectionEfficiency = dynamic_cast<AliFMDAnaCalibEventSelectionEfficiency*>(fin->Get(fgkEventSelectionEffID));
  if (!fEventSelectionEfficiency) AliFatal("Invalid background object from CDB");
  
}
//____________________________________________________________________

void AliFMDAnaParameters::PrintStatus() const
{
  
  TString energystring;
  switch(fEnergy) {
  case k900:
    energystring.Form("900 GeV");   break;
  case k7000:
    energystring.Form("7000 GeV");  break;
  case k10000:
    energystring.Form("10000 GeV"); break;
  case k14000:
    energystring.Form("14000 GeV"); break;
  default:
    energystring.Form("invalid energy"); break;
  }
  TString triggerstring;
  switch(fTrigger) {
  case kMB1:
    triggerstring.Form("Minimum bias 1");   break;
  case kMB2:
    triggerstring.Form("Minimum bias 2");   break;
  case kSPDFASTOR:
    triggerstring.Form("SPD FAST OR");   break;
  case kNOCTP:
    triggerstring.Form("NO TRIGGER TEST");   break;
  default:
    energystring.Form("invalid trigger"); break;
  }
  TString magstring;
  switch(fMagField) {
  case k5G:
    magstring.Form("5 kGaus");   break;
  case k0G:
    magstring.Form("0 kGaus");   break;
  default:
    magstring.Form("invalid mag field"); break;
  }
  TString collsystemstring;
  switch(fSpecies) {
  case kPP:
    collsystemstring.Form("p+p");   break;
  case kPbPb:
    collsystemstring.Form("Pb+Pb");   break;
  default:
    collsystemstring.Form("invalid collision system");   break;
  }
  

  std::cout<<"Energy      = "<<energystring.Data()<<std::endl;
  std::cout<<"Trigger     = "<<triggerstring.Data()<<std::endl;
  std::cout<<"Mag Field   = "<<magstring.Data()<<std::endl;
  std::cout<<"Coll System = "<<collsystemstring.Data()<<std::endl;
  
  
  
}

//____________________________________________________________________

void AliFMDAnaParameters::InitSharingEff() {
  
  //AliCDBEntry*   background = GetEntry(fgkBackgroundCorrection);
  TFile* fin = TFile::Open(GetPath(fgkSharingEffID));
			    
  if (!fin) return;
  
  fSharingEfficiency = dynamic_cast<AliFMDAnaCalibSharingEfficiency*>(fin->Get(fgkSharingEffID));
  if (!fSharingEfficiency) AliFatal("Invalid background object from CDB");
  
}
//____________________________________________________________________
Float_t AliFMDAnaParameters::GetVtxCutZ() {
  
  if(!fIsInit) {
    AliWarning("Not initialized yet. Call Init() to remedy");
    return -1;
  }
  
  return fBackground->GetVtxCutZ();
}

//____________________________________________________________________
Int_t AliFMDAnaParameters::GetNvtxBins() {
  
  if(!fIsInit) {
    AliWarning("Not initialized yet. Call Init() to remedy");
    return -1;
  }
  
  return fBackground->GetNvtxBins();
}
//____________________________________________________________________
TH1F* AliFMDAnaParameters::GetEnergyDistribution(Int_t det, Char_t ring, Float_t eta) {
  
  return fEnergyDistribution->GetEnergyDistribution(det, ring, eta);
}
//____________________________________________________________________
TH1F* AliFMDAnaParameters::GetEmptyEnergyDistribution(Int_t det, Char_t ring) {
  
  return fEnergyDistribution->GetEmptyEnergyDistribution(det, ring);
}
//____________________________________________________________________
TH1F* AliFMDAnaParameters::GetRingEnergyDistribution(Int_t det, Char_t ring) {
  
  return fEnergyDistribution->GetRingEnergyDistribution(det, ring);
}
//____________________________________________________________________
Float_t AliFMDAnaParameters::GetSigma(Int_t det, Char_t ring, Float_t eta) {
  
  if(!fIsInit) {
    AliWarning("Not initialized yet. Call Init() to remedy");
    return 0;
  }
  
  TH1F* hEnergyDist       = GetEnergyDistribution(det,ring, eta);
  TF1*  fitFunc           = hEnergyDist->GetFunction("FMDfitFunc");
  if(!fitFunc) {
    //AliWarning(Form("No function for FMD%d%c, eta %f",det,ring,eta));
    return 1024;
  }
  Float_t sigma           = fitFunc->GetParameter(2);
  return sigma;
}


//____________________________________________________________________
Float_t AliFMDAnaParameters::GetMPV(Int_t det, Char_t ring, Float_t eta) {
  
  if(!fIsInit) {
    AliWarning("Not initialized yet. Call Init() to remedy");
    return 0;
  }
  //AliFMDAnaParameters* pars = AliFMDAnaParameters::Instance();
  TH1F* hEnergyDist     = GetEnergyDistribution(det,ring,eta);
  TF1*  fitFunc         = hEnergyDist->GetFunction("FMDfitFunc");
  if(!fitFunc) {
    AliWarning(Form("No function for FMD%d%c, eta %f",det,ring,eta));
    std::cout<<hEnergyDist->GetEntries()<<"    "<<GetEtaBin(eta)<<std::endl;
    return 1024;
  }
    
  Float_t mpv           = fitFunc->GetParameter(1);
  return mpv;
}
//____________________________________________________________________
Float_t AliFMDAnaParameters::GetConstant(Int_t det, Char_t ring, Float_t eta) {
  
  if(!fIsInit) {
    AliWarning("Not initialized yet. Call Init() to remedy");
    return 0;
  }
  
  TH1F* hEnergyDist     = GetEnergyDistribution(det,ring,eta);
  TF1*  fitFunc         = hEnergyDist->GetFunction("FMDfitFunc");
  if(!fitFunc) {
    AliWarning(Form("No function for FMD%d%c, eta %f",det,ring,eta));
    return 0;
  }
    
  Float_t mpv           = fitFunc->GetParameter(0);
  return mpv;
}
//____________________________________________________________________
Float_t AliFMDAnaParameters::Get2MIPWeight(Int_t det, Char_t ring, Float_t eta) {
  
  if(!fIsInit) {
    AliWarning("Not initialized yet. Call Init() to remedy");
    return 0;
  }
  
  TH1F* hEnergyDist     = GetEnergyDistribution(det,ring,eta);
  TF1*  fitFunc         = hEnergyDist->GetFunction("FMDfitFunc");
  if(!fitFunc) return 0;
  Float_t twoMIPweight    = fitFunc->GetParameter(3);
  
  
  
  if(twoMIPweight < 1e-05)
    twoMIPweight = 0;
  
  return twoMIPweight;
}
//____________________________________________________________________
Float_t AliFMDAnaParameters::Get3MIPWeight(Int_t det, Char_t ring, Float_t eta) {
  
  if(!fIsInit) {
    AliWarning("Not initialized yet. Call Init() to remedy");
    return 0;
  }
  
  TH1F* hEnergyDist     = GetEnergyDistribution(det,ring,eta);
  TF1*  fitFunc         = hEnergyDist->GetFunction("FMDfitFunc");
  if(!fitFunc) return 0;
  Float_t threeMIPweight    = fitFunc->GetParameter(4);
  
  if(threeMIPweight < 1e-05)
    threeMIPweight = 0;
  
  Float_t twoMIPweight    = fitFunc->GetParameter(3);
  
  if(twoMIPweight < 1e-05)
    threeMIPweight = 0;
    
  return threeMIPweight;
}
//____________________________________________________________________
Int_t AliFMDAnaParameters::GetNetaBins() {
  return GetBackgroundCorrection(1,'I',0)->GetNbinsX();
  
}
//____________________________________________________________________
Float_t AliFMDAnaParameters::GetEtaMin() {

  return GetBackgroundCorrection(1,'I',0)->GetXaxis()->GetXmin();
} 
//____________________________________________________________________
Float_t AliFMDAnaParameters::GetEtaMax() {

return GetBackgroundCorrection(1,'I',0)->GetXaxis()->GetXmax();

}
//____________________________________________________________________
Int_t AliFMDAnaParameters::GetEtaBin(Float_t eta) {
  TAxis testaxis(GetNetaBins(),GetEtaMin(),GetEtaMax());
  Int_t binnumber = testaxis.FindBin(eta) ;
  
  return binnumber;

}
//____________________________________________________________________

TH2F* AliFMDAnaParameters::GetBackgroundCorrection(Int_t det, 
						   Char_t ring, 
						   Int_t vtxbin) {
  
  if(!fIsInit) {
    AliWarning("Not initialized yet. Call Init() to remedy");
    return 0;
  }
  
  
  
  if(vtxbin > fBackground->GetNvtxBins()) {
    AliWarning(Form("No background object for vertex bin %d", vtxbin));
    return 0;
  } 
  
  return fBackground->GetBgCorrection(det,ring,vtxbin);
}
//____________________________________________________________________

TH1F* AliFMDAnaParameters::GetDoubleHitCorrection(Int_t det, 
						  Char_t ring) {
  
  if(!fIsInit) {
    AliWarning("Not initialized yet. Call Init() to remedy");
    return 0;
  }
  
  return fBackground->GetDoubleHitCorrection(det,ring);
}
//_____________________________________________________________________
Float_t AliFMDAnaParameters::GetEventSelectionEfficiency(Int_t vtxbin) {
  if(!fIsInit) {
    AliWarning("Not initialized yet. Call Init() to remedy");
    return 0;
  }
  return fEventSelectionEfficiency->GetCorrection(vtxbin);

}
//_____________________________________________________________________
TH1F* AliFMDAnaParameters::GetSharingEfficiency(Int_t det, Char_t ring, Int_t vtxbin) {
  if(!fIsInit) {
    AliWarning("Not initialized yet. Call Init() to remedy");
    return 0;
  }
  
  return fSharingEfficiency->GetSharingEff(det,ring,vtxbin);

}
//_____________________________________________________________________
TH1F* AliFMDAnaParameters::GetSharingEfficiencyTrVtx(Int_t det, Char_t ring, Int_t vtxbin) {
  if(!fIsInit) {
    AliWarning("Not initialized yet. Call Init() to remedy");
    return 0;
  }
  
  return fSharingEfficiency->GetSharingEffTrVtx(det,ring,vtxbin);

}
//_____________________________________________________________________
Float_t AliFMDAnaParameters::GetMaxR(Char_t ring) const{
  Float_t radius = 0;
  if(ring == 'I')
    radius = 17.2;
  else if(ring == 'O')
    radius = 28.0;
  else
    AliWarning("Unknown ring - must be I or O!");
  
  return radius;
}
//_____________________________________________________________________
Float_t AliFMDAnaParameters::GetMinR(Char_t ring) const{
  Float_t radius = 0;
  if(ring == 'I')
    radius = 4.5213;
  else if(ring == 'O')
    radius = 15.4;
  else
    AliWarning("Unknown ring - must be I or O!");
  
  return radius;

}
//_____________________________________________________________________
void AliFMDAnaParameters::SetCorners(Char_t ring) {
  
  if(ring == 'I') {
    fCorner1.Set(4.9895, 15.3560);
    fCorner2.Set(1.8007, 17.2000);
  }
  else {
    fCorner1.Set(4.2231, 26.6638);
    fCorner2.Set(1.8357, 27.9500);
  }
  
}
//_____________________________________________________________________
Float_t AliFMDAnaParameters::GetPhiFromSector(UShort_t det, Char_t ring, UShort_t sec) const
{
  Int_t nsec = (ring == 'I' ? 20 : 40);
  Float_t basephi = 0;
  if(det == 1) 
    basephi = 1.72787594; 
  if(det == 2 && ring == 'I')
    basephi = 0.15707963;
  if(det == 2 && ring == 'O')
    basephi = 0.078539818;
  if(det == 3 && ring == 'I')
    basephi = 2.984513044;
  if(det == 3 && ring == 'O')
    basephi = 3.06305289;
  
  Float_t step = 2*TMath::Pi() / nsec;
  Float_t phi = 0;
  if(det == 3)
    phi = basephi - sec*step;
  else
    phi = basephi + sec*step;
  
  if(phi < 0) 
    phi = phi +2*TMath::Pi();
  if(phi > 2*TMath::Pi() )
    phi = phi - 2*TMath::Pi();
  
  return phi;
}
//_____________________________________________________________________
Float_t AliFMDAnaParameters::GetEtaFromStrip(UShort_t det, Char_t ring, UShort_t sec, UShort_t strip, Float_t zvtx) const
{
  // AliFMDRing fmdring(ring);
  // fmdring.Init();
  Float_t   rad       = GetMaxR(ring)-GetMinR(ring);
  Float_t   nStrips   = (ring == 'I' ? 512 : 256);
  Float_t   segment   = rad / nStrips;
  Float_t   r         = GetMinR(ring) + segment*strip;
  Float_t   z         = 0;
  Int_t hybrid = sec / 2;
  
  if(det == 1) {
    if(!(hybrid%2)) z = 320.266; else z = 319.766;
  }
  if(det == 2 && ring == 'I' ) {
    if(!(hybrid%2)) z = 83.666; else z = 83.166;
  }
  if(det == 2 && ring == 'O' ) {
    if(!(hybrid%2)) z = 74.966; else z = 75.466;
  }
  if(det == 3 && ring == 'I' ) {
    if(!(hybrid%2)) z = -63.066; else z = -62.566;
  }
  if(det == 3 && ring == 'O' ) {
    if(!(hybrid%2)) z = -74.966; else z = -75.466;
  }
  
  //std::cout<<det<<"   "<<ring<<"   "<<sec<<"   "<<hybrid<<"    "<<z<<std::endl;
  
  // Float_t   r     = TMath::Sqrt(TMath::Power(x,2)+TMath::Power(y,2));
  Float_t   theta = TMath::ATan2(r,z-zvtx);
  Float_t   eta   = -1*TMath::Log(TMath::Tan(0.5*theta));
  
  return eta;
}

//_____________________________________________________________________

Bool_t AliFMDAnaParameters::GetVertex(AliESDEvent* esd, Double_t* vertexXYZ) 
{
  const AliESDVertex* vertex = 0;
  vertex = esd->GetPrimaryVertexSPD();
  
  if(vertex)
    vertex->GetXYZ(vertexXYZ);
    
  //if(vertexXYZ[0] == 0 || vertexXYZ[1] == 0 )
  //  return kFALSE;
  
  if(vertex->GetNContributors() <= 0)
    return kFALSE;
  
  if(vertex->GetZRes() > 0.1 ) 
    return kFALSE;
  
  return vertex->GetStatus();
  
}
//____________________________________________________________________
Bool_t AliFMDAnaParameters::IsEventTriggered(const AliESDEvent *esd, Trigger trig) {

  Trigger old = fTrigger;
  fTrigger = trig;
  Bool_t retval = IsEventTriggered(esd);
  fTrigger = old;
  return retval;

}
//____________________________________________________________________
Bool_t AliFMDAnaParameters::IsEventTriggered(const AliESDEvent *esd) const {
  // check if the event was triggered
  
  if (fCentralSelection) return kTRUE;
  ULong64_t triggerMask = esd->GetTriggerMask();
  
  TString triggers = esd->GetFiredTriggerClasses();
  // std::cout<<triggers.Data()<<std::endl;
  //if(triggers.Contains("CINT1B-ABCE-NOPF-ALL") || triggers.Contains("CINT1B-E-NOPF-ALL")) return kTRUE;
  //else return kFALSE;
  //if(triggers.Contains("CINT1B-E-NOPF-ALL"))    return kFALSE;
  
  // if(triggers.Contains("CINT1A-ABCE-NOPF-ALL")) return kFALSE;
  // if(triggers.Contains("CINT1C-ABCE-NOPF-ALL")) return kFALSE;
  
  // definitions from p-p.cfg
  ULong64_t spdFO = (1 << 14);
  ULong64_t v0left = (1 << 11);
  ULong64_t v0right = (1 << 12);
  AliTriggerAnalysis tAna;
  
  //REMOVE WHEN FINISHED PLAYING WITH TRIGGERS!
  //fPhysicsSelection->IsCollisionCandidate(esd);
  
  
  switch (fTrigger) {
  case kMB1: {
    //if (triggerMask & spdFO || ((triggerMask & v0left) || (triggerMask & v0right)))
    //  if(/*tAna.IsOfflineTriggerFired(esd,AliTriggerAnalysis::kMB1) &&*/ (triggers.Contains("CINT1B-ABCE-NOPF-ALL")))// || triggers.Contains("CINT1-E-NOPF-ALL")))
    if(fRealData) {
      if( fPhysicsSelection->IsCollisionCandidate(esd))
	return kTRUE;
	
       //  triggers.Contains("CINT1C-ABCE-NOPF-ALL") || triggers.Contains("CINT1A-ABCE-NOPF-ALL"))
    
    }
    else
      if(triggerMask & spdFO || ((triggerMask & v0left) || (triggerMask & v0right)))
	return kTRUE;
    break;
  }
  case kMB2: { 
    if (triggerMask & spdFO && ((triggerMask & v0left) || (triggerMask & v0right)))
      return kTRUE;
    break;
  }
  case kSPDFASTOR: {
    if (triggerMask & spdFO)
      return kTRUE;
    break;
  }
  case kNOCTP: {
    return kTRUE;
    break;
  }
  case kEMPTY: {
    if(triggers.Contains("CBEAMB-ABCE-NOPF-ALL"))
      return kTRUE;
    break;
  }
  }//switch
  
  return kFALSE;
}

//____________________________________________________________________
Float_t 
AliFMDAnaParameters::GetStripLength(Char_t ring, UShort_t strip)  
{
  //AliFMDRing fmdring(ring);
  // fmdring.Init();
  
  Float_t rad        = GetMaxR(ring)-GetMinR(ring);
  Float_t   nStrips   = (ring == 'I' ? 512 : 256);
  Float_t segment    = rad / nStrips;
  
  //TVector2* corner1  = fmdring.GetVertex(2);  
  // TVector2* corner2  = fmdring.GetVertex(3);
  
  SetCorners(ring);
  /*
  std::cout<<GetMaxR(ring)<<"   "<<fmdring.GetMaxR()<<std::endl;
  std::cout<<GetMinR(ring)<<"   "<<fmdring.GetMinR()<<std::endl;
  std::cout<<corner1->X()<<"   "<<fCorner1.X()<<std::endl;
  std::cout<<corner2->X()<<"   "<<fCorner2.X()<<std::endl;
  std::cout<<corner1->Y()<<"   "<<fCorner1.Y()<<std::endl;
  std::cout<<corner2->Y()<<"   "<<fCorner2.Y()<<std::endl;*/
  Float_t slope      = (fCorner1.Y() - fCorner2.Y()) / (fCorner1.X() - fCorner2.X());
  Float_t constant   = (fCorner2.Y()*fCorner1.X()-(fCorner2.X()*fCorner1.Y())) / (fCorner1.X() - fCorner2.X());
  Float_t radius     = GetMinR(ring) + strip*segment;
  
  Float_t d          = TMath::Power(TMath::Abs(radius*slope),2) + TMath::Power(radius,2) - TMath::Power(constant,2);
  
  Float_t arclength  = GetBaseStripLength(ring,strip);
  if(d>0) {
    
    Float_t x        = (-1*TMath::Sqrt(d) -slope*constant) / (1+TMath::Power(slope,2));
    Float_t y        = slope*x + constant;
    Float_t theta    = TMath::ATan2(x,y);
    
    if(x < fCorner1.X() && y > fCorner1.Y()) {
      arclength = radius*theta;                        //One sector since theta is by definition half-hybrid
      
    }
    
  }
  
  return arclength;
  
  
}
//____________________________________________________________________
Float_t 
AliFMDAnaParameters::GetBaseStripLength(Char_t ring, UShort_t strip)  
{  
  // AliFMDRing fmdring(ring);
  // fmdring.Init();
  Float_t rad             = GetMaxR(ring)-GetMinR(ring);
  Float_t nStrips         = (ring == 'I' ? 512 : 256);
  Float_t nSec            = (ring == 'I' ? 20 : 40);
  Float_t segment         = rad / nStrips;
  Float_t basearc         = 2*TMath::Pi() / (0.5*nSec); // One hybrid: 36 degrees inner, 18 outer
  Float_t radius          = GetMinR(ring) + strip*segment;
  Float_t basearclength   = 0.5*basearc * radius;                // One sector   
  
  return basearclength;
}
//____________________________________________________________________
//
// EOF
//

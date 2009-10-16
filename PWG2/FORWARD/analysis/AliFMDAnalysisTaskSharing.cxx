 
#include <TROOT.h>
#include <TSystem.h>
#include <TInterpreter.h>
#include <TChain.h>
#include <TFile.h>
#include <TList.h>
#include <iostream>
#include <TMath.h>
//#include "AliFMDDebug.h"
#include "AliFMDAnalysisTaskSharing.h"
#include "AliAnalysisManager.h"
#include "AliESDFMD.h"
//#include "AliFMDGeometry.h"
#include "AliMCEventHandler.h"
#include "AliMCEvent.h"
#include "AliStack.h"
#include "AliESDVertex.h"
#include "AliMultiplicity.h"
#include "AliFMDAnaParameters.h"
//#include "/home/canute/ALICE/AliRoot/PWG0/AliPWG0Helper.h"
//#include "AliFMDParameters.h"
#include "AliGenEventHeader.h"
#include "AliHeader.h"
#include "AliStack.h"
#include "AliMCParticle.h"
#include "AliFMDStripIndex.h"

ClassImp(AliFMDAnalysisTaskSharing)

//_____________________________________________________________________
AliFMDAnalysisTaskSharing::AliFMDAnalysisTaskSharing()
: fDebug(0),
  fESD(0x0),
  foutputESDFMD(),
  fEnergy(0),
  fNstrips(0),
  fSharedThis(kFALSE),
  fSharedPrev(kFALSE),
  fDiagList(0),
  fStandalone(kTRUE),
  fEsdVertex(0),
  fStatus(kTRUE),
  fLastTrackByStrip(0)
{
  // Default constructor
  DefineInput (0, AliESDEvent::Class());
  DefineOutput(0, AliESDFMD::Class());
  DefineOutput(1, AliESDVertex::Class());
  DefineOutput(2, AliESDEvent::Class());
  DefineOutput(3, TList::Class());
}
//_____________________________________________________________________
AliFMDAnalysisTaskSharing::AliFMDAnalysisTaskSharing(const char* name, Bool_t SE):
    AliAnalysisTask(name, "AnalysisTaskFMD"),
    fDebug(0),
    fESD(0x0),
    foutputESDFMD(),
    fEnergy(0),
    fNstrips(0),
    fSharedThis(kFALSE),
    fSharedPrev(kFALSE),
    fDiagList(0),
    fStandalone(kTRUE),
    fEsdVertex(0),
    fStatus(kTRUE),
    fLastTrackByStrip(0)
{
  fStandalone = SE;
  if(fStandalone) {
    DefineInput (0, AliESDEvent::Class());
    DefineOutput(0, AliESDFMD::Class());
    DefineOutput(1, AliESDVertex::Class());
    DefineOutput(2, AliESDEvent::Class());
    DefineOutput(3, TList::Class());
  }
}
//_____________________________________________________________________
void AliFMDAnalysisTaskSharing::CreateOutputObjects()
{
  if(!foutputESDFMD)
    foutputESDFMD = new AliESDFMD();
  
  if(!fEsdVertex)
    fEsdVertex    = new AliESDVertex();
  //Diagnostics
  if(!fDiagList)
    fDiagList = new TList();
  
  fDiagList->SetName("Sharing diagnostics");
  
  AliFMDAnaParameters* pars = AliFMDAnaParameters::Instance();
  TH2F* hBg   = pars->GetBackgroundCorrection(1, 'I', 0);
  TH1F* hPrimary = new TH1F("hMultvsEtaNoCuts","hMultvsEtaNoCuts",
			    hBg->GetNbinsX(),
			    hBg->GetXaxis()->GetXmin(),
			    hBg->GetXaxis()->GetXmax());
  hPrimary->Sumw2();
  fDiagList->Add(hPrimary);
  TH1F* hPrimVertexBin = 0;
  TH1F* hHits = 0;
  for(Int_t i = 0; i< pars->GetNvtxBins(); i++) {
    
    hPrimVertexBin = new TH1F(Form("primmult_NoCuts_vtxbin%d",i),
			      Form("primmult_NoCuts_vtxbin%d",i),
			      hBg->GetNbinsX(),
			      hBg->GetXaxis()->GetXmin(),
			      hBg->GetXaxis()->GetXmax());
    hPrimVertexBin->Sumw2();
    fDiagList->Add(hPrimVertexBin);
    
  }
  
  for(Int_t det = 1; det<=3; det++) {
    Int_t nRings = (det==1 ? 1 : 2);
    
    for(Int_t iring = 0;iring<nRings; iring++) {
      Char_t ringChar = (iring == 0 ? 'I' : 'O');
      TH1F* hEdist        = new TH1F(Form("Edist_before_sharing_FMD%d%c", det, ringChar),
				     Form("Edist_before_sharing_FMD%d%c", det, ringChar),
				     1000,0,25);
      TH1F* hEdist_after  = new TH1F(Form("Edist_after_sharing_FMD%d%c", det, ringChar),
				     Form("Edist_after_sharing_FMD%d%c", det, ringChar),
				     1000,0,25);
      
      
      //TH1F* hNstripsHit    = new TH1F(Form("N_strips_hit_FMD%d%c",det,ringChar),
      //				     Form("N_strips_hit_FMD%d%c",det,ringChar),
      //				     25,0,25);
      fDiagList->Add(hEdist);
      fDiagList->Add(hEdist_after);
      //fDiagList->Add(hNstripsHit);
      
      for(Int_t i = 0; i< pars->GetNvtxBins(); i++) {
	hHits  = new TH1F(Form("hMCHits_nocuts_FMD%d%c_vtxbin%d",det,ringChar,i),Form("hMCHits_FMD%d%c_vtxbin%d",det,ringChar,i),
			  hBg->GetNbinsX(),
			  hBg->GetXaxis()->GetXmin(),
			  hBg->GetXaxis()->GetXmax());
	hHits->Sumw2();
	fDiagList->Add(hHits);

      }
      
    }
  }
  TH1F*  nMCevents = new TH1F("nMCEventsNoCuts","nMCEventsNoCuts",pars->GetNvtxBins(),0,pars->GetNvtxBins());
  
  fDiagList->Add(nMCevents);
  
}
//_____________________________________________________________________
void AliFMDAnalysisTaskSharing::ConnectInputData(Option_t */*option*/)
{
  if(fStandalone)
    fESD = (AliESDEvent*)GetInputData(0);
}
//_____________________________________________________________________
void AliFMDAnalysisTaskSharing::Exec(Option_t */*option*/)
{

  AliESD* old = fESD->GetAliESDOld();
  if (old) {
    fESD->CopyFromOldESD();
  }
  
  foutputESDFMD->Clear();
  
  AliFMDAnaParameters* pars = AliFMDAnaParameters::Instance();
  Double_t vertex[3];
  pars->GetVertex(fESD,vertex);
  fEsdVertex->SetXYZ(vertex);
  
  // Process primaries here to get true MC distribution
  if(pars->GetProcessPrimary())
    ProcessPrimary();
  
  Bool_t isTriggered = pars->IsEventTriggered(fESD);
 
  if(!isTriggered) {
    fStatus = kFALSE;
    return;
   }
   else
    fStatus = kTRUE;
  
  if(vertex[0] < 0.0001 && vertex[1] < 0.0001 && vertex[2] < 0.0001) {
    fStatus = kFALSE;
    return;
  }
  else
    fStatus = kTRUE;
  
  
  const AliMultiplicity* testmult = fESD->GetMultiplicity();
  
  Int_t nTrackLets = testmult->GetNumberOfTracklets();
  if(nTrackLets < 1000) foutputESDFMD->SetUniqueID(kTRUE);
  else foutputESDFMD->SetUniqueID(kFALSE);
  
  AliESDFMD* fmd = fESD->GetFMDData();
  
  if (!fmd) return;
  Int_t nHits = 0;
  for(UShort_t det=1;det<=3;det++) {
    Int_t nRings = (det==1 ? 1 : 2);
    for (UShort_t ir = 0; ir < nRings; ir++) {
      Char_t   ring = (ir == 0 ? 'I' : 'O');
      UShort_t nsec = (ir == 0 ? 20  : 40);
      UShort_t nstr = (ir == 0 ? 512 : 256);
      
      TH1F* hEdist = (TH1F*)fDiagList->FindObject(Form("Edist_before_sharing_FMD%d%c",det,ring));
      
      for(UShort_t sec =0; sec < nsec;  sec++) {
	fSharedThis      = kFALSE;
	fSharedPrev      = kFALSE;
	fEnergy = 0;
	fNstrips = 0;
	for(UShort_t strip = 0; strip < nstr; strip++) {
	  foutputESDFMD->SetMultiplicity(det,ring,sec,strip,0.);
	  Float_t mult = fmd->Multiplicity(det,ring,sec,strip);
	  
	  if(mult == AliESDFMD::kInvalidMult || mult == 0) continue;
	  
	  //Double_t eta  = EtaFromStrip(det,ring,sec,strip,vertex[2]);//fmd->Eta(det,ring,sec,strip);
	  //Double_t eta = fmd->Eta(det,ring,sec,strip);
	  Float_t eta = pars->GetEtaFromStrip(det,ring,sec,strip,vertex[2]);
	  //std::cout<<EtaFromStrip(det,ring,sec,strip,vertex[2]) <<"    "<<fmd->Eta(det,ring,sec,strip)<<std::endl;
	  
	  hEdist->Fill(mult);
	  if(fmd->IsAngleCorrected())
	    mult = mult/TMath::Cos(Eta2Theta(eta));
	  Float_t Eprev = 0;
	  Float_t Enext = 0;
	  if(strip != 0)
	    if(fmd->Multiplicity(det,ring,sec,strip-1) != AliESDFMD::kInvalidMult) {
	      Eprev = fmd->Multiplicity(det,ring,sec,strip-1);
	      if(fmd->IsAngleCorrected())
		Eprev = Eprev/TMath::Cos(Eta2Theta(fmd->Eta(det,ring,sec,strip-1)));
	    }
	  if(strip != nstr - 1)
	    if(fmd->Multiplicity(det,ring,sec,strip+1) != AliESDFMD::kInvalidMult) {
	      Enext = fmd->Multiplicity(det,ring,sec,strip+1);
	      if(fmd->IsAngleCorrected())
		Enext = Enext/TMath::Cos(Eta2Theta(fmd->Eta(det,ring,sec,strip+1)));
	    }
	  
	  Float_t merged_energy = GetMultiplicityOfStrip(mult,eta,Eprev,Enext,det,ring,sec,strip);

	  if(merged_energy > 0 )
	    nHits++;
	  foutputESDFMD->SetMultiplicity(det,ring,sec,strip,merged_energy);
	  foutputESDFMD->SetEta(det,ring,sec,strip,eta);
	  
	}
      }
    }
  }
  
   
  if(fStandalone) {
    PostData(0, foutputESDFMD); 
    PostData(1, fEsdVertex); 
    PostData(2, fESD); 
    PostData(3, fDiagList); 
  }
}
//_____________________________________________________________________
Float_t AliFMDAnalysisTaskSharing::GetMultiplicityOfStrip(Float_t mult,
							  Float_t eta,
							  Float_t Eprev,
							  Float_t Enext,
							  UShort_t   det,
							  Char_t  ring,
							  UShort_t /*sec*/,
							  UShort_t /*strip*/) {
  AliFMDAnaParameters* pars = AliFMDAnaParameters::Instance();
 
  Float_t merged_energy = 0;
  //Float_t nParticles = 0;
  Float_t cutLow  = 0.15;
  if(ring == 'I')
    cutLow = 0.1;
  
  //cutLow = 0;
  //AliFMDParameters* recopars = AliFMDParameters::Instance();
  //cutLow = (5*recopars->GetPedestalWidth(det,ring,sec,strip))/(recopars->GetPulseGain(det,ring,sec,strip)*recopars->GetDACPerMIP());
  
  
  
  Float_t cutHigh = pars->GetMPV(det,ring,eta) - 3*pars->GetSigma(det,ring,eta);
   
  // Float_t cutPart = pars->GetMPV(det,ring,eta) - 5*pars->GetSigma(det,ring,eta);
  Float_t Etotal  = mult;
  

    //std::cout<<det<<ring<<"   "<<sec<<"    "<<strip<<"   "<<cutLow<<std::endl;
  if(fSharedThis) {
    fSharedThis      = kFALSE;
    fSharedPrev      = kTRUE;
    return 0.;
  }
  
  /*  if(mult < 0.33*pars->GetMPV(det,ring,eta)) {
    fSharedThis      = kFALSE;
    fSharedPrev      = kFALSE;
    return 0;
    }*/
  if(mult<Enext && Enext>cutHigh && foutputESDFMD->GetUniqueID() == kTRUE)
    {
      fSharedThis      = kFALSE;
      fSharedPrev      = kFALSE;
      return 0;
    }
  if(mult > 15)
    {
      //   std::cout<<"rejecting hit in FMD "<<det<<" "<<ring<<std::endl;
      fSharedThis      = kFALSE;
      fSharedPrev      = kFALSE;
      return 0;
    }
  
  if(Eprev > cutLow && Eprev < cutHigh && !fSharedPrev ) {
    Etotal += Eprev;
  }
  
  if(Enext > cutLow && Enext < cutHigh ) {
    Etotal += Enext;
    fSharedThis      = kTRUE;
  }
  TH1F* hEdist = (TH1F*)fDiagList->FindObject(Form("Edist_after_sharing_FMD%d%c",det,ring));
  hEdist->Fill(Etotal);
  
  Etotal = Etotal*TMath::Cos(Eta2Theta(eta));
  if(Etotal > 0) {
    
    merged_energy = Etotal;
    fSharedPrev      = kTRUE;
    // if(det == 1 && ring =='I')
    // std::cout<<Form("Merged signals %f %f %f into %f , %f in strip %d, sec %d, ring %c, det %d",Eprev, mult, Enext, Etotal/TMath::Cos(Eta2Theta(eta)),Etotal,strip,sec,ring,det )<<std::endl;
  }
    else{// if(Etotal > 0) {
      //if(det == 3 && ring =='I')
      //	std::cout<<Form("NO HIT  for  %f %f %f into %f , %f in strip %d, sec %d, ring %c, det %d, cuts %f , %f",Eprev, mult, Enext, Etotal/TMath::Cos(Eta2Theta(eta)),Etotal,strip,sec,ring,det,cutPart,cutHigh )<<std::endl;
    fSharedThis      = kFALSE;
    fSharedPrev      = kFALSE;
  }
  // merged_energy = mult;
  
  return merged_energy; 
  //}  
}

//_____________________________________________________________________
Float_t AliFMDAnalysisTaskSharing::Eta2Theta(Float_t eta) {

  Float_t theta = 2*TMath::ATan(TMath::Exp(-1*eta));
  
  if(eta < 0)
    theta = theta-TMath::Pi();
  
  //  std::cout<<"From eta2Theta: "<<theta<<"   "<<eta<<std::endl;
  return theta;
  


}



//_____________________________________________________________________
void AliFMDAnalysisTaskSharing::ProcessPrimary() {
  
  AliMCEventHandler* eventHandler = dynamic_cast<AliMCEventHandler*> (AliAnalysisManager::GetAnalysisManager()->GetMCtruthEventHandler());
  AliMCEvent* mcEvent = eventHandler->MCEvent();
  if(!mcEvent)
    return;
  fLastTrackByStrip.Reset(-1);
  AliFMDAnaParameters* pars = AliFMDAnaParameters::Instance();
  
  AliMCParticle* particle = 0;
  
  AliStack* stack = mcEvent->Stack();
  
  TH1F* hPrimary = (TH1F*)fDiagList->FindObject("hMultvsEtaNoCuts");
  AliHeader* header            = mcEvent->Header();
  AliGenEventHeader* genHeader = header->GenEventHeader();
  
  
  
  TArrayF vertex;
  genHeader->PrimaryVertex(vertex);
  
  if(TMath::Abs(vertex.At(2)) > pars->GetVtxCutZ())
    return;
  
  Double_t delta           = 2*pars->GetVtxCutZ()/pars->GetNvtxBins();
  Double_t vertexBinDouble = (vertex.At(2) + pars->GetVtxCutZ()) / delta;
  Int_t    vertexBin       = (Int_t)vertexBinDouble;
  
  Bool_t firstTrack = kTRUE;
  
  Int_t nTracks = stack->GetNprimary();
  if(pars->GetProcessHits())
    nTracks = stack->GetNtrack();
  TH1F* nMCevents = (TH1F*)fDiagList->FindObject("nMCEventsNoCuts");
  for(Int_t i = 0 ;i<nTracks;i++) {
    particle = (AliMCParticle*) mcEvent->GetTrack(i);
    if(!particle)
      continue;
    
    if(stack->IsPhysicalPrimary(i) && particle->Charge() != 0) {
      hPrimary->Fill(particle->Eta());
      

      TH1F* hPrimVtxBin = (TH1F*)fDiagList->FindObject(Form("primmult_NoCuts_vtxbin%d",vertexBin));
      hPrimVtxBin->Fill(particle->Eta());
      
      if(firstTrack) {
	nMCevents->Fill(vertexBin);
	firstTrack = kFALSE;
      }
    
    }
     if(pars->GetProcessHits()) {
           
      for(Int_t j=0; j<particle->GetNumberOfTrackReferences();j++) {
	
	AliTrackReference* ref = particle->GetTrackReference(j);
	UShort_t det,sec,strip;
	Char_t   ring;
	if(ref->DetectorId() != AliTrackReference::kFMD)
	  continue;
	AliFMDStripIndex::Unpack(ref->UserId(),det,ring,sec,strip);
	Float_t thisStripTrack = fLastTrackByStrip(det,ring,sec,strip);
	if(particle->Charge() != 0 && i != thisStripTrack ) {
	  //Double_t x,y,z;
	  
	  Float_t   eta   = pars->GetEtaFromStrip(det,ring,sec,strip,vertex.At(2));//-1*TMath::Log(TMath::Tan(0.5*theta));
	  TH1F* hHits = (TH1F*)fDiagList->FindObject(Form("hMCHits_nocuts_FMD%d%c_vtxbin%d",det,ring,vertexBin));
	  
	
	  hHits->Fill(eta);
	  
	  Float_t nstrips = (ring =='O' ? 256 : 512);
	  
	  fLastTrackByStrip(det,ring,sec,strip) = (Float_t)i;
	
	  if(strip >0)
	    fLastTrackByStrip(det,ring,sec,strip-1) = (Float_t)i;
	  if(strip < (nstrips - 1))
	    fLastTrackByStrip(det,ring,sec,strip+1) = (Float_t)i;
	  
	}
      
	
      }
      
      
    }
    
  }

}

//_____________________________________________________________________
//
// EOF
//

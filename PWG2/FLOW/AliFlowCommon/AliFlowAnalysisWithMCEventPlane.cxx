/*************************************************************************
* Copyright(c) 1998-2008, ALICE Experiment at CERN, All rights reserved. *
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

#define AliFlowAnalysisWithMCEventPlane_cxx
 
#include "Riostream.h"  //needed as include
#include "TFile.h"      //needed as include
#include "TProfile.h"   //needed as include
#include "TProfile2D.h"   
#include "TComplex.h"   //needed as include
#include "TList.h"

class TH1F;

#include "AliFlowCommonConstants.h"    //needed as include
#include "AliFlowEventSimple.h"
#include "AliFlowTrackSimple.h"
#include "AliFlowCommonHist.h"
#include "AliFlowCommonHistResults.h"
#include "AliFlowAnalysisWithMCEventPlane.h"

class AliFlowVector;

// AliFlowAnalysisWithMCEventPlane:
// Description: Maker to analyze Flow from the generated MC reaction plane.
//              This class is used to get the real value of the flow 
//              to compare the other methods to when analysing simulated events
// author: N. van der Kolk (kolk@nikhef.nl)

ClassImp(AliFlowAnalysisWithMCEventPlane)

  //-----------------------------------------------------------------------
 
 AliFlowAnalysisWithMCEventPlane::AliFlowAnalysisWithMCEventPlane():
   fQsum(NULL),
   fQ2sum(0),
   fEventNumber(0),
   fDebug(kFALSE),
   fHistList(NULL),
   fCommonHists(NULL),
   fCommonHistsRes(NULL),
   fHistRP(NULL),
   fHistProIntFlow(NULL),
   fHistProDiffFlowPtEtaRP(NULL),
   fHistProDiffFlowPtRP(NULL),
   fHistProDiffFlowEtaRP(NULL),
   fHistProDiffFlowPtEtaPOI(NULL),
   fHistProDiffFlowPtPOI(NULL),
   fHistProDiffFlowEtaPOI(NULL),
   fHistSpreadOfFlow(NULL),
   fHarmonic(2)
{

  // Constructor.
  fHistList = new TList();

  fQsum = new TVector2;        // flow vector sum
}

 
 //-----------------------------------------------------------------------


 AliFlowAnalysisWithMCEventPlane::~AliFlowAnalysisWithMCEventPlane() 
 {
   //destructor
   delete fHistList;
   delete fQsum;
 }
 
//-----------------------------------------------------------------------

void AliFlowAnalysisWithMCEventPlane::WriteHistograms(TString* outputFileName)
{
 //store the final results in output .root file
 TFile *output = new TFile(outputFileName->Data(),"RECREATE");
 //output->WriteObject(fHistList, "cobjMCEP","SingleKey");
 fHistList->SetName("cobjMCEP");
 fHistList->SetOwner(kTRUE);
 fHistList->Write(fHistList->GetName(), TObject::kSingleKey);
 delete output;
}

//-----------------------------------------------------------------------

void AliFlowAnalysisWithMCEventPlane::WriteHistograms(TString outputFileName)
{
 //store the final results in output .root file
 TFile *output = new TFile(outputFileName.Data(),"RECREATE");
 //output->WriteObject(fHistList, "cobjMCEP","SingleKey");
 fHistList->SetName("cobjMCEP");
 fHistList->SetOwner(kTRUE);
 fHistList->Write(fHistList->GetName(), TObject::kSingleKey);
 delete output;
}

//-----------------------------------------------------------------------

void AliFlowAnalysisWithMCEventPlane::WriteHistograms(TDirectoryFile *outputFileName)
{
 //store the final results in output .root file
 fHistList->SetName("cobjMCEP");
 fHistList->SetOwner(kTRUE);
 outputFileName->Add(fHistList);
 outputFileName->Write(outputFileName->GetName(), TObject::kSingleKey);
}

//-----------------------------------------------------------------------
void AliFlowAnalysisWithMCEventPlane::Init() {

  //Define all histograms
  cout<<"---Analysis with the real MC Event Plane---"<<endl;

  Int_t iNbinsPt = AliFlowCommonConstants::GetMaster()->GetNbinsPt();
  Double_t dPtMin = AliFlowCommonConstants::GetMaster()->GetPtMin();	     
  Double_t dPtMax = AliFlowCommonConstants::GetMaster()->GetPtMax();
  
  Int_t iNbinsEta = AliFlowCommonConstants::GetMaster()->GetNbinsEta();
  Double_t dEtaMin = AliFlowCommonConstants::GetMaster()->GetEtaMin();	     
  Double_t dEtaMax = AliFlowCommonConstants::GetMaster()->GetEtaMax();  

  fCommonHists = new AliFlowCommonHist("AliFlowCommonHistMCEP");
  fHistList->Add(fCommonHists);
  fCommonHistsRes = new AliFlowCommonHistResults("AliFlowCommonHistResultsMCEP");
  fHistList->Add(fCommonHistsRes);
  
  // store harmonic in common control histogram: 
  (fCommonHists->GetHarmonic())->Fill(0.5,fHarmonic);
  
  fHistRP = new TH1F("Flow_RP_MCEP","Flow_RP_MCEP",100,0.,3.14);
  fHistRP->SetXTitle("Reaction Plane Angle");
  fHistRP->SetYTitle("Counts");
  fHistList->Add(fHistRP);
  
  fHistProIntFlow = new TProfile("FlowPro_V_MCEP","FlowPro_V_MCEP",1,0.,1.);
  fHistProIntFlow->SetLabelSize(0.06);
  (fHistProIntFlow->GetXaxis())->SetBinLabel(1,"v_{n}{2}");
  fHistProIntFlow->SetYTitle("");
  fHistList->Add(fHistProIntFlow);
  
  fHistProDiffFlowPtEtaRP = new TProfile2D("FlowPro_VPtEtaRP_MCEP","FlowPro_VPtEtaRP_MCEP",iNbinsPt,dPtMin,dPtMax,iNbinsEta,dEtaMin,dEtaMax);
  fHistProDiffFlowPtEtaRP->SetXTitle("P_{t}");
  fHistProDiffFlowPtEtaRP->SetYTitle("#eta");
  fHistList->Add(fHistProDiffFlowPtEtaRP);
 
  fHistProDiffFlowPtRP = new TProfile("FlowPro_VPtRP_MCEP","FlowPro_VPtRP_MCEP",iNbinsPt,dPtMin,dPtMax);
  fHistProDiffFlowPtRP->SetXTitle("P_{t}");
  fHistProDiffFlowPtRP->SetYTitle("");
  fHistList->Add(fHistProDiffFlowPtRP);  
  
  fHistProDiffFlowEtaRP = new TProfile("FlowPro_VetaRP_MCEP","FlowPro_VetaRP_MCEP",iNbinsEta,dEtaMin,dEtaMax);
  fHistProDiffFlowEtaRP->SetXTitle("#eta");
  fHistProDiffFlowEtaRP->SetYTitle("");
  fHistList->Add(fHistProDiffFlowEtaRP);
  
  fHistProDiffFlowPtEtaPOI = new TProfile2D("FlowPro_VPtEtaPOI_MCEP","FlowPro_VPtEtaPOI_MCEP",iNbinsPt,dPtMin,dPtMax,iNbinsEta,dEtaMin,dEtaMax);
  fHistProDiffFlowPtEtaPOI->SetXTitle("P_{t}");
  fHistProDiffFlowPtEtaPOI->SetYTitle("#eta");
  fHistList->Add(fHistProDiffFlowPtEtaPOI);
  
  fHistProDiffFlowPtPOI = new TProfile("FlowPro_VPtPOI_MCEP","FlowPro_VPtPOI_MCEP",iNbinsPt,dPtMin,dPtMax);
  fHistProDiffFlowPtPOI->SetXTitle("P_{t}");
  fHistProDiffFlowPtPOI->SetYTitle("");
  fHistList->Add(fHistProDiffFlowPtPOI);  
  
  fHistProDiffFlowEtaPOI = new TProfile("FlowPro_VetaPOI_MCEP","FlowPro_VetaPOI_MCEP",iNbinsEta,dEtaMin,dEtaMax);
  fHistProDiffFlowEtaPOI->SetXTitle("#eta");
  fHistProDiffFlowEtaPOI->SetYTitle("");
  fHistList->Add(fHistProDiffFlowEtaPOI);         
  
  fHistSpreadOfFlow = new TH1D("fHistSpreadOfFlow","fHistSpreadOfFlow",1000,-1,1);
  fHistSpreadOfFlow->SetXTitle("v_{2}");
  fHistSpreadOfFlow->SetYTitle("counts");
  fHistList->Add(fHistSpreadOfFlow);           
 
  fEventNumber = 0;  //set number of events to zero
        
} 
 
//-----------------------------------------------------------------------
 
void AliFlowAnalysisWithMCEventPlane::Make(AliFlowEventSimple* anEvent) {

  //Calculate v2 from the MC reaction plane
  if (anEvent) {
  
    // get the MC reaction plane angle
    Double_t aRP = anEvent->GetMCReactionPlaneAngle();  
    //fill control histograms     
    fCommonHists->FillControlHistograms(anEvent);

    //get the Q vector from the FlowEvent
    AliFlowVector vQ = anEvent->GetQ(fHarmonic); 
    //cout<<"vQ.Mod() = " << vQ.Mod() << endl;
    //for chi calculation:
    *fQsum += vQ;
    //cout<<"fQsum.Mod() = "<<fQsum.Mod()<<endl;
    fQ2sum += vQ.Mod2();
    //cout<<"fQ2sum = "<<fQ2sum<<endl;
        
    fHistRP->Fill(aRP);   
    
    Double_t dPhi = 0.;
    Double_t dv  = 0.;
    Double_t dPt  = 0.;
    Double_t dEta = 0.;
    //Double_t dPi = TMath::Pi();  
    
    // profile to calculate flow e-b-y:
    TProfile *flowEBE = new TProfile("flowEBE","flowEBE",1,0,1);
                                                                                         
    //calculate flow
    //loop over the tracks of the event
    Int_t iNumberOfTracks = anEvent->NumberOfTracks(); 
    for (Int_t i=0;i<iNumberOfTracks;i++) 
      {
	AliFlowTrackSimple* pTrack = anEvent->GetTrack(i) ; 
	if (pTrack){
	  if (pTrack->InRPSelection()){
            dPhi = pTrack->Phi();
            dv  = TMath::Cos(fHarmonic*(dPhi-aRP));
	         dPt  = pTrack->Pt();
	         dEta = pTrack->Eta();
            //no-name int. flow (to be improved = name needed!):
            fHistProIntFlow->Fill(0.,dv);
            //no-name int. flow e-b-e (to be improved = name needed!):
            flowEBE->Fill(0.,dv);
            //differential flow (Pt, Eta, RP):
            fHistProDiffFlowPtEtaRP->Fill(dPt,dEta,dv,1.);
            //differential flow (Pt, RP):
            fHistProDiffFlowPtRP->Fill(dPt,dv,1.);
            //differential flow (Eta, RP):
            fHistProDiffFlowEtaRP->Fill(dEta,dv,1.);
          }
	  if (pTrack->InPOISelection()) {
	    dPhi = pTrack->Phi();
	    //if (dPhi<0.) dPhi+=2*TMath::Pi();
	    //calculate flow v2:
	    dv  = TMath::Cos(fHarmonic*(dPhi-aRP));
	    dPt  = pTrack->Pt();
	    dEta = pTrack->Eta();
	    //differential flow (Pt, Eta, POI):
            fHistProDiffFlowPtEtaPOI->Fill(dPt,dEta,dv,1.);
	    //differential flow (Pt, POI):
            fHistProDiffFlowPtPOI->Fill(dPt,dv,1.);
            //differential flow (Eta, POI):
            fHistProDiffFlowEtaPOI->Fill(dEta,dv,1.); 
	  }	      
	}//track selected
      }//loop over tracks
	  
    fEventNumber++;
    //    cout<<"@@@@@ "<<fEventNumber<<" events processed"<<endl;
    
    // store flow value for this event:
    fHistSpreadOfFlow->Fill(flowEBE->GetBinContent(1),flowEBE->GetBinEntries(1));
    delete flowEBE; 
  }
}
  //--------------------------------------------------------------------    

void AliFlowAnalysisWithMCEventPlane::GetOutputHistograms(TList *outputListHistos) {
 // get the pointers to all output histograms before calling Finish()
 if (outputListHistos) {
    //Get the common histograms from the output list
    AliFlowCommonHist *pCommonHists = dynamic_cast<AliFlowCommonHist*> 
      (outputListHistos->FindObject("AliFlowCommonHistMCEP"));
    AliFlowCommonHistResults *pCommonHistResults = 
      dynamic_cast<AliFlowCommonHistResults*> 
      (outputListHistos->FindObject("AliFlowCommonHistResultsMCEP"));

    TProfile *pHistProIntFlow = dynamic_cast<TProfile*> 
      (outputListHistos->FindObject("FlowPro_V_MCEP")); 
      
    TProfile2D *pHistProDiffFlowPtEtaRP = dynamic_cast<TProfile2D*> 
      (outputListHistos->FindObject("FlowPro_VPtEtaRP_MCEP")); 
                               
    TProfile *pHistProDiffFlowPtRP = dynamic_cast<TProfile*> 
      (outputListHistos->FindObject("FlowPro_VPtRP_MCEP")); 
     
    TProfile *pHistProDiffFlowEtaRP = dynamic_cast<TProfile*> 
      (outputListHistos->FindObject("FlowPro_VetaRP_MCEP"));
 
    TProfile2D *pHistProDiffFlowPtEtaPOI = dynamic_cast<TProfile2D*> 
      (outputListHistos->FindObject("FlowPro_VPtEtaPOI_MCEP")); 
          
    TProfile *pHistProDiffFlowPtPOI = dynamic_cast<TProfile*> 
      (outputListHistos->FindObject("FlowPro_VPtPOI_MCEP")); 
     
    TProfile *pHistProDiffFlowEtaPOI = dynamic_cast<TProfile*> 
      (outputListHistos->FindObject("FlowPro_VetaPOI_MCEP"));                             

    if (pCommonHists && pCommonHistResults && pHistProIntFlow && 
	pHistProDiffFlowPtRP && pHistProDiffFlowEtaRP && 
	pHistProDiffFlowPtPOI && pHistProDiffFlowEtaPOI) {
      this->SetCommonHists(pCommonHists);
      this->SetCommonHistsRes(pCommonHistResults);
      this->SetHistProIntFlow(pHistProIntFlow);
      this->SetHistProDiffFlowPtEtaRP(pHistProDiffFlowPtEtaRP);
      this->SetHistProDiffFlowPtRP(pHistProDiffFlowPtRP);      
      this->SetHistProDiffFlowEtaRP(pHistProDiffFlowEtaRP);  
      this->SetHistProDiffFlowPtEtaPOI(pHistProDiffFlowPtEtaPOI);
      this->SetHistProDiffFlowPtPOI(pHistProDiffFlowPtPOI);      
      this->SetHistProDiffFlowEtaPOI(pHistProDiffFlowEtaPOI);          
    } else {
      cout<<"WARNING: Histograms needed to run Finish() are not accessible!"<<endl;  }
    
    //fListHistos->Print();
  } else { cout << "histogram list pointer is empty" << endl;}

}

  //--------------------------------------------------------------------    
void AliFlowAnalysisWithMCEventPlane::Finish() {
   
  //*************make histograms etc. 
  if (fDebug) cout<<"AliFlowAnalysisWithMCEventPlane::Terminate()"<<endl;
   
  Int_t iNbinsPt  = AliFlowCommonConstants::GetMaster()->GetNbinsPt();  
  Int_t iNbinsEta = AliFlowCommonConstants::GetMaster()->GetNbinsEta(); 
  
  // access harmonic:
  if(fCommonHists && fCommonHists->GetHarmonic())
  {
   fHarmonic = (Int_t)(fCommonHists->GetHarmonic())->GetBinContent(1); // to be improved (moved somewhere else?)
  } 
         
  // no-name int. flow (to be improved):
  Double_t dV = fHistProIntFlow->GetBinContent(1);  
  Double_t dErrV = fHistProIntFlow->GetBinError(1); // to be improved (treatment of errors for non-Gaussian distribution needed!)  
  // fill no-name int. flow (to be improved):
  fCommonHistsRes->FillIntegratedFlow(dV,dErrV);
  cout<<"dV"<<fHarmonic<<"{MC} is       "<<dV<<" +- "<<dErrV<<endl;
  
  //RP:
  TH1F* fHistPtRP = fCommonHists->GetHistPtRP(); 
  Double_t dYieldPtRP = 0.;
  Double_t dVRP = 0.;
  Double_t dErrVRP = 0.;
  Double_t dSumRP = 0.;
  //differential flow (RP, Pt): 
  Double_t dvPtRP = 0.;           
  Double_t dErrvPtRP = 0.;
  for(Int_t b=1;b<=iNbinsPt;b++)
  {
   dvPtRP    = fHistProDiffFlowPtRP->GetBinContent(b);
   dErrvPtRP = fHistProDiffFlowPtRP->GetBinError(b);//to be improved (treatment of errors for non-Gaussian distribution needed!)
   fCommonHistsRes->FillDifferentialFlowPtRP(b, dvPtRP, dErrvPtRP);
   if(fHistPtRP){
	//integrated flow (RP)
	dYieldPtRP = fHistPtRP->GetBinContent(b);
	dVRP += dvPtRP*dYieldPtRP;
	dSumRP += dYieldPtRP;
	//error on integrated flow
	dErrVRP += dYieldPtRP*dYieldPtRP*dErrvPtRP*dErrvPtRP;
      }
  }
  if (dSumRP != 0. ) {
    dVRP /= dSumRP;  //because pt distribution should be normalised
    dErrVRP /= (dSumRP*dSumRP);
    dErrVRP = TMath::Sqrt(dErrVRP); 
  }
  // fill integrated flow (RP):
  fCommonHistsRes->FillIntegratedFlowRP(dVRP,dErrVRP);
  cout<<"dV"<<fHarmonic<<"{MC} (RP) is  "<<dVRP<<" +- "<<dErrVRP<<endl;
  
  //differential flow (RP, Eta): 
  Double_t dvEtaRP = 0.;           
  Double_t dErrvEtaRP = 0.;
  for(Int_t b=1;b<=iNbinsEta;b++)
  {
   dvEtaRP    = fHistProDiffFlowEtaRP->GetBinContent(b);
   dErrvEtaRP = fHistProDiffFlowEtaRP->GetBinError(b);//to be improved (treatment of errors for non-Gaussian distribution needed!)
   fCommonHistsRes->FillDifferentialFlowEtaRP(b, dvEtaRP, dErrvEtaRP);
  }
                                                                                                                                   
  //POI:
  TH1F* fHistPtPOI = fCommonHists->GetHistPtPOI(); 
  Double_t dYieldPtPOI = 0.;
  Double_t dVPOI = 0.;
  Double_t dErrVPOI = 0.;
  Double_t dSumPOI = 0.;
  Double_t dvproPtPOI = 0.;
  Double_t dErrdifcombPtPOI = 0.; 
  Double_t dvproEtaPOI = 0.;
  Double_t dErrdifcombEtaPOI = 0.;   
  //Pt:
  if(fHistProDiffFlowPtPOI) {
    for(Int_t b=1;b<=iNbinsPt;b++){
      dvproPtPOI = fHistProDiffFlowPtPOI->GetBinContent(b);
      dErrdifcombPtPOI = fHistProDiffFlowPtPOI->GetBinError(b);//to be improved (treatment of errors for non-Gaussian distribution needed!)
      //fill TH1D
      fCommonHistsRes->FillDifferentialFlowPtPOI(b, dvproPtPOI, dErrdifcombPtPOI); 
      if (fHistPtPOI){
	//integrated flow (POI)
	dYieldPtPOI = fHistPtPOI->GetBinContent(b);
	dVPOI += dvproPtPOI*dYieldPtPOI;
	dSumPOI += dYieldPtPOI;
	//error on integrated flow
	dErrVPOI += dYieldPtPOI*dYieldPtPOI*dErrdifcombPtPOI*dErrdifcombPtPOI;
      }
    }//end of for(Int_t b=0;b<iNbinsPt;b++)  
  } else { cout<<"fHistProFlow is NULL"<<endl; }
  if (dSumPOI != 0. ) {
    dVPOI /= dSumPOI;  //because pt distribution should be normalised
    dErrVPOI /= (dSumPOI*dSumPOI);
    dErrVPOI = TMath::Sqrt(dErrVPOI); 
  }
  cout<<"dV"<<fHarmonic<<"{MC} (POI) is "<<dVPOI<<" +- "<<dErrVPOI<<endl;

  fCommonHistsRes->FillIntegratedFlowPOI(dVPOI,dErrVPOI);
  
  //Eta:
  if(fHistProDiffFlowEtaPOI)
  {
   for(Int_t b=1;b<=iNbinsEta;b++)
   {
    dvproEtaPOI = fHistProDiffFlowEtaPOI->GetBinContent(b);
    dErrdifcombEtaPOI = fHistProDiffFlowEtaPOI->GetBinError(b);//to be improved (treatment of errors for non-Gaussian distribution needed!)
    //fill common hist results:
    fCommonHistsRes->FillDifferentialFlowEtaPOI(b, dvproEtaPOI, dErrdifcombEtaPOI); 
   }
  }   
  
  cout<<endl;     	      	  
  //cout<<".....finished"<<endl;
}

 
 

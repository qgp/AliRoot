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

#define AliFlowAnalysisWithScalarProduct_cxx
 
#include "Riostream.h"  //needed as include
#include "TFile.h"      //needed as include
#include "TList.h"
#include "TMath.h"
#include "TProfile.h"
#include "TVector2.h"

class TH1F;

#include "AliFlowCommonConstants.h"    //needed as include
#include "AliFlowEventSimple.h"
#include "AliFlowTrackSimple.h"
#include "AliFlowCommonHist.h"
#include "AliFlowCommonHistResults.h"
#include "AliFlowAnalysisWithScalarProduct.h"

class AliFlowVector;

// AliFlowAnalysisWithScalarProduct:
// Description: 
// Maker to analyze Flow with the Scalar product method.
//
// author: N. van der Kolk (kolk@nikhef.nl)

ClassImp(AliFlowAnalysisWithScalarProduct)

  //-----------------------------------------------------------------------
 
 AliFlowAnalysisWithScalarProduct::AliFlowAnalysisWithScalarProduct():
   fEventNumber(0),
   fDebug(kFALSE),
   fHistList(NULL),
   fHistProUQetaRP(NULL),
   fHistProUQetaPOI(NULL),
   fHistProUQPtRP(NULL),
   fHistProUQPtPOI(NULL),
   fHistProQaQb(NULL),
   fHistProM(NULL),
   fCommonHists(NULL),
   fCommonHistsRes(NULL)
{
  // Constructor.
  fHistList = new TList();
}
 //-----------------------------------------------------------------------


 AliFlowAnalysisWithScalarProduct::~AliFlowAnalysisWithScalarProduct() 
 {
   //destructor
   delete fHistList;
 }
 

//-----------------------------------------------------------------------

void AliFlowAnalysisWithScalarProduct::WriteHistograms(TString* outputFileName)
{
 //store the final results in output .root file

  TFile *output = new TFile(outputFileName->Data(),"RECREATE");
  //output->WriteObject(fHistList, "cobjSP","SingleKey");
  fHistList->SetName("cobjSP");
  fHistList->SetOwner(kTRUE);
  fHistList->Write(fHistList->GetName(), TObject::kSingleKey);
  delete output;
}

//-----------------------------------------------------------------------

void AliFlowAnalysisWithScalarProduct::WriteHistograms(TString outputFileName)
{
 //store the final results in output .root file

  TFile *output = new TFile(outputFileName.Data(),"RECREATE");
  //output->WriteObject(fHistList, "cobjSP","SingleKey");
  fHistList->SetName("cobjSP");
  fHistList->SetOwner(kTRUE);
  fHistList->Write(fHistList->GetName(), TObject::kSingleKey);
  delete output;
}

//-----------------------------------------------------------------------

void AliFlowAnalysisWithScalarProduct::WriteHistograms(TDirectoryFile *outputFileName)
{
 //store the final results in output .root file
 fHistList->SetName("cobjSP");
 fHistList->SetOwner(kTRUE);
 outputFileName->Add(fHistList);
 outputFileName->Write(outputFileName->GetName(), TObject::kSingleKey);
}

//-----------------------------------------------------------------------
void AliFlowAnalysisWithScalarProduct::Init() {

  //Define all histograms
  cout<<"---Analysis with the Scalar Product Method--- Init"<<endl;

  Int_t iNbinsPt   = AliFlowCommonConstants::GetMaster()->GetNbinsPt();
  Double_t dPtMin  = AliFlowCommonConstants::GetMaster()->GetPtMin();	     
  Double_t dPtMax  = AliFlowCommonConstants::GetMaster()->GetPtMax();
  Int_t iNbinsEta  = AliFlowCommonConstants::GetMaster()->GetNbinsEta();
  Double_t dEtaMin = AliFlowCommonConstants::GetMaster()->GetEtaMin();	     
  Double_t dEtaMax = AliFlowCommonConstants::GetMaster()->GetEtaMax();

  fHistProUQetaRP = new TProfile("Flow_UQetaRP_SP","Flow_UQetaRP_SP",iNbinsEta,dEtaMin,dEtaMax);
  fHistProUQetaRP->SetXTitle("{eta}");
  fHistProUQetaRP->SetYTitle("<uQ>");
  fHistList->Add(fHistProUQetaRP);

  fHistProUQetaPOI = new TProfile("Flow_UQetaPOI_SP","Flow_UQetaPOI_SP",iNbinsEta,dEtaMin,dEtaMax);
  fHistProUQetaPOI->SetXTitle("{eta}");
  fHistProUQetaPOI->SetYTitle("<uQ>");
  fHistList->Add(fHistProUQetaPOI);

  fHistProUQPtRP = new TProfile("Flow_UQPtRP_SP","Flow_UQPtRP_SP",iNbinsPt,dPtMin,dPtMax);
  fHistProUQPtRP->SetXTitle("p_t (GeV)");
  fHistProUQPtRP->SetYTitle("<uQ>");
  fHistList->Add(fHistProUQPtRP);

  fHistProUQPtPOI = new TProfile("Flow_UQPtPOI_SP","Flow_UQPtPOI_SP",iNbinsPt,dPtMin,dPtMax);
  fHistProUQPtPOI->SetXTitle("p_t (GeV)");
  fHistProUQPtPOI->SetYTitle("<uQ>");
  fHistList->Add(fHistProUQPtPOI);

  fHistProQaQb = new TProfile("Flow_QaQb_SP","Flow_QaQb_SP", 1, -0.5, 0.5);
  fHistProQaQb->SetYTitle("<QaQb>");
  fHistList->Add(fHistProQaQb);

  fHistProM = new TProfile("Flow_M_SP","Flow_M_SP",2,0.5, 2.5);
  fHistProM -> SetYTitle("<*>");
  fHistProM -> SetXTitle("<M-1>, <Ma*Mb>");
  fHistList->Add(fHistProM);

  fCommonHists = new AliFlowCommonHist("AliFlowCommonHistSP");
  fHistList->Add(fCommonHists);
  fCommonHistsRes = new AliFlowCommonHistResults("AliFlowCommonHistResultsSP");
  fHistList->Add(fCommonHistsRes);  

  fEventNumber = 0;  //set number of events to zero    
}

//-----------------------------------------------------------------------
 
void AliFlowAnalysisWithScalarProduct::Make(AliFlowEventSimple* anEvent) {

  //Fill histogram
  if (anEvent) {

    //fill control histograms     
    fCommonHists->FillControlHistograms(anEvent);
        
    //get Q vectors for the eta-subevents
    AliFlowVector* vQarray = new AliFlowVector[2];
    anEvent->GetQsub(vQarray);
    AliFlowVector vQa = vQarray[0];
    AliFlowVector vQb = vQarray[1];
    //get total Q vector
    AliFlowVector vQ = vQa + vQb;
    
    //fill the multiplicity histograms for the prefactor
    fHistProM -> Fill(1,vQ.GetMult()-1);                //<M-1>
    fHistProM -> Fill(2,vQa.GetMult()*vQb.GetMult());   //<Ma*Mb>
    //scalar product of the two subevents
    Double_t dQaQb = vQa*vQb; 
    fHistProQaQb -> Fill(0.,dQaQb);    
                
    //loop over the tracks of the event
    AliFlowTrackSimple*   pTrack = NULL; 
    Int_t iNumberOfTracks = anEvent->NumberOfTracks(); 
    for (Int_t i=0;i<iNumberOfTracks;i++) 
      {
	pTrack = anEvent->GetTrack(i) ; 
	if (pTrack){
	  Double_t dPhi = pTrack->Phi();
	  //calculate vU
	  TVector2 vU;
	  Double_t dUX = TMath::Cos(2*dPhi);
	  Double_t dUY = TMath::Sin(2*dPhi);
	  vU.Set(dUX,dUY);
	  Double_t dModulus = vU.Mod();
	  if (dModulus!=0.) vU.Set(dUX/dModulus,dUY/dModulus);  // make length 1
	  else cerr<<"dModulus is zero!"<<endl;

	  TVector2 vQm = vQ;
	  //subtract particle from the flowvector if used to define it
	  if (pTrack->InRPSelection()) {
	    if (pTrack->InSubevent(0) || pTrack->InSubevent(1)) { 
	      Double_t dQmX = vQm.X() - dUX;
	      Double_t dQmY = vQm.Y() - dUY;
	      vQm.Set(dQmX,dQmY);
	    }
	  }

	  //dUQ = scalar product of vU and vQm
	  Double_t dUQ = vU * vQm;
	  Double_t dPt = pTrack->Pt();
	  Double_t dEta = pTrack->Eta();
	  //fill the profile histograms
	  if (pTrack->InRPSelection()) {
	    fHistProUQetaRP -> Fill(dEta,dUQ);
	    fHistProUQPtRP -> Fill(dPt,dUQ);
	  }
	  if (pTrack->InPOISelection()) {
	    fHistProUQetaPOI -> Fill(dEta,dUQ);
	    fHistProUQPtPOI -> Fill(dPt,dUQ);
	  }  
	}//track selected
      }//loop over tracks
	 
    fEventNumber++;
    //    cout<<"@@@@@ "<<fEventNumber<<" events processed"<<endl;
    delete [] vQarray;
  }
}

  //--------------------------------------------------------------------  
void AliFlowAnalysisWithScalarProduct::GetOutputHistograms(TList *outputListHistos){
  
  //get pointers to all output histograms (called before Finish())
  if (outputListHistos) {
  //Get the common histograms from the output list
    AliFlowCommonHist *pCommonHist = dynamic_cast<AliFlowCommonHist*> 
      (outputListHistos->FindObject("AliFlowCommonHistSP"));
    AliFlowCommonHistResults *pCommonHistResults = dynamic_cast<AliFlowCommonHistResults*> 
      (outputListHistos->FindObject("AliFlowCommonHistResultsSP"));
    TProfile* pHistProQaQb     = dynamic_cast<TProfile*>(outputListHistos->FindObject("Flow_QaQb_SP"));
    TProfile* pHistProM        = dynamic_cast<TProfile*>(outputListHistos->FindObject("Flow_M_SP"));
    TProfile* pHistProUQetaRP  = dynamic_cast<TProfile*>(outputListHistos->FindObject("Flow_UQetaRP_SP"));
    TProfile* pHistProUQetaPOI = dynamic_cast<TProfile*>(outputListHistos->FindObject("Flow_UQetaPOI_SP"));
    TProfile* pHistProUQPtRP   = dynamic_cast<TProfile*>(outputListHistos->FindObject("Flow_UQPtRP_SP"));
    TProfile* pHistProUQPtPOI  = dynamic_cast<TProfile*>(outputListHistos->FindObject("Flow_UQPtPOI_SP"));
    if (pCommonHist && pCommonHistResults && pHistProQaQb && pHistProM &&
	pHistProUQetaRP	&& pHistProUQetaPOI && pHistProUQPtRP && pHistProUQPtPOI) {
      this -> SetCommonHists(pCommonHist);
      this -> SetCommonHistsRes(pCommonHistResults);
      this -> SetHistProQaQb(pHistProQaQb);
      this -> SetHistProM(pHistProM);
      this -> SetHistProUQetaRP(pHistProUQetaRP);
      this -> SetHistProUQetaPOI(pHistProUQetaPOI);
      this -> SetHistProUQPtRP(pHistProUQPtRP);
      this -> SetHistProUQPtPOI(pHistProUQPtPOI);
      }  
   }
}            

//--------------------------------------------------------------------            
void AliFlowAnalysisWithScalarProduct::Finish() {
   
  //calculate flow and fill the AliFlowCommonHistResults
  if (fDebug) cout<<"AliFlowAnalysisWithScalarProduct::Finish()"<<endl;

  cout<<"*************************************"<<endl;
  cout<<"*************************************"<<endl;
  cout<<"      Integrated flow from           "<<endl;
  cout<<"         Scalar product              "<<endl;
  cout<<endl;
  
  Int_t iNbinsPt  = AliFlowCommonConstants::GetMaster()->GetNbinsPt();
  Int_t iNbinsEta = AliFlowCommonConstants::GetMaster()->GetNbinsEta();

  Double_t dMmin1    = fHistProM->GetBinContent(1);  //average over M-1
  Double_t dMmin1Err = fHistProM->GetBinError(1);    //error on average over M-1
  Double_t dMaMb     = fHistProM->GetBinContent(2);  //average over Ma*Mb
  Double_t dMaMbErr  = fHistProM->GetBinError(2);    //error on average over Ma*Mb

  Double_t dMcorrection = 0.;     //correction factor for Ma != Mb
  Double_t dMcorrectionErr = 0.;  
  Double_t dMcorrectionErrRel = 0.; 
  Double_t dMcorrectionErrRel2 = 0.; 

  if (dMaMb != 0. && dMmin1 != 0.) {
    dMcorrection    = dMmin1/(TMath::Sqrt(dMaMb)); 
    dMcorrectionErr = dMcorrection*(dMmin1Err/dMmin1 + dMaMbErr/(2*dMaMb));
    dMcorrectionErrRel = dMcorrectionErr/dMcorrection;
    dMcorrectionErrRel2 = dMcorrectionErrRel*dMcorrectionErrRel;
  }

  Double_t dQaQbAv  = TMath::Abs(fHistProQaQb->GetBinContent(1)); //average over events //TEST TAKE ABS
  Double_t dQaQbErr = fHistProQaQb->GetBinError(1);
  Double_t dQaQbErrRel = 0.;
  if (dQaQbAv != 0.) {
    dQaQbErrRel = dQaQbErr/dQaQbAv; }
  Double_t dQaQbErrRel2 = dQaQbErrRel*dQaQbErrRel;

  if (dQaQbAv <= 0.){
    //set v to -0
    fCommonHistsRes->FillIntegratedFlowRP(-0.,0.);
    fCommonHistsRes->FillIntegratedFlow(-0.,0.);
    cout<<"dV(RP) = -0. +- 0."<<endl;
    fCommonHistsRes->FillIntegratedFlowPOI(-0.,0.);
    cout<<"dV(POI) = -0. +- 0."<<endl;
  } else {
  Double_t dQaQbSqrt = TMath::Sqrt(dQaQbAv);  //DOES NOT WORK IF dQaQbAv IS NEGATIVE
    if (dMaMb>0.) { dQaQbSqrt *= dMcorrection; }
    else { dQaQbSqrt = 0.; }
    Double_t dQaQbSqrtErrRel2 = dMcorrectionErrRel2 + (1/4)*dQaQbErrRel2;
    
    //v as a function of eta for RP selection
    for(Int_t b=0;b<iNbinsEta;b++) {
      Double_t duQpro = fHistProUQetaRP->GetBinContent(b);
      Double_t duQerr = fHistProUQetaRP->GetBinError(b); //copy error for now
      Double_t duQerrRel = 0.;
      if (duQpro != 0.) {duQerrRel = duQerr/duQpro;}
      Double_t duQerrRel2 = duQerrRel*duQerrRel;

      Double_t dv2pro     = 0.;
      if (dQaQbSqrt!=0.) { dv2pro = duQpro/dQaQbSqrt; }
      Double_t dv2errRel2 = duQerrRel2 + dQaQbSqrtErrRel2;
      Double_t dv2errRel  = 0.;
      if (dv2errRel2>0.) { dv2errRel  = TMath::Sqrt(dv2errRel2); }
      Double_t dv2err     = dv2pro*dv2errRel; 
      //fill TH1D
      fCommonHistsRes->FillDifferentialFlowEtaRP(b, dv2pro, dv2err); 
    } //loop over bins b
    
    //v as a function of eta for POI selection
    for(Int_t b=0;b<iNbinsEta;b++) {
      Double_t duQpro = fHistProUQetaPOI->GetBinContent(b);
      Double_t duQerr = fHistProUQetaPOI->GetBinError(b); //copy error for now
      Double_t duQerrRel = 0.;
      if (duQpro != 0.) {duQerrRel = duQerr/duQpro;}
      Double_t duQerrRel2 = duQerrRel*duQerrRel;

      Double_t dv2pro     = 0.;
      if (dQaQbSqrt!=0.) { dv2pro = duQpro/dQaQbSqrt; }
      Double_t dv2errRel2 = duQerrRel2 + dQaQbSqrtErrRel2;
      Double_t dv2errRel  = 0.;
      if (dv2errRel2>0.) { dv2errRel  = TMath::Sqrt(dv2errRel2); }
      Double_t dv2err     = dv2pro*dv2errRel; 
      //fill TH1D
      fCommonHistsRes->FillDifferentialFlowEtaPOI(b, dv2pro, dv2err); 
    } //loop over bins b
    
    //v as a function of Pt for RP selection
    TH1F* fHistPtRP = fCommonHists->GetHistPtRP(); //for calculating integrated flow
    Double_t dVRP = 0.;
    Double_t dSum = 0.;
    Double_t dErrV =0.;

    for(Int_t b=0;b<iNbinsPt;b++) {
      Double_t duQpro = fHistProUQPtRP->GetBinContent(b);
      Double_t duQerr = fHistProUQPtRP->GetBinError(b); //copy error for now
      Double_t duQerrRel = 0.;
      if (duQpro != 0.) {duQerrRel = duQerr/duQpro;}
      Double_t duQerrRel2 = duQerrRel*duQerrRel;

      Double_t dv2pro     = 0.;
      if (dQaQbSqrt!=0.) { dv2pro = duQpro/dQaQbSqrt; }
      Double_t dv2errRel2 = duQerrRel2 + dQaQbSqrtErrRel2;
      Double_t dv2errRel  = 0.;
      if (dv2errRel2>0.) { dv2errRel  = TMath::Sqrt(dv2errRel2); }
      Double_t dv2err     = dv2pro*dv2errRel; 
      //fill TH1D
      fCommonHistsRes->FillDifferentialFlowPtRP(b, dv2pro, dv2err);
      //calculate integrated flow for RP selection
      if (fHistPtRP){
	Double_t dYieldPt = fHistPtRP->GetBinContent(b);
	dVRP += dv2pro*dYieldPt;
	dSum +=dYieldPt;
	dErrV += dYieldPt*dYieldPt*dv2err*dv2err;
      } else { cout<<"fHistPtRP is NULL"<<endl; }
    } //loop over bins b

    if (dSum != 0.) {
      dVRP /= dSum; //the pt distribution should be normalised
      dErrV /= (dSum*dSum);
      dErrV = TMath::Sqrt(dErrV);
    }
    fCommonHistsRes->FillIntegratedFlowRP(dVRP,dErrV);
    fCommonHistsRes->FillIntegratedFlow(dVRP,dErrV);

    cout<<"dV(RP) = "<<dVRP<<" +- "<<dErrV<<endl;
       
    //v as a function of Pt for POI selection 
    TH1F* fHistPtPOI = fCommonHists->GetHistPtPOI(); //for calculating integrated flow
    Double_t dVPOI = 0.;
    dSum = 0.;
    dErrV =0.;
  
    for(Int_t b=0;b<iNbinsPt;b++) {
      Double_t duQpro = fHistProUQPtPOI->GetBinContent(b);
      Double_t duQerr = fHistProUQPtPOI->GetBinError(b); //copy error for now
      Double_t duQerrRel = 0.;
      if (duQpro != 0.) {duQerrRel = duQerr/duQpro;}
      Double_t duQerrRel2 = duQerrRel*duQerrRel;

      Double_t dv2pro     = 0.;
      if (dQaQbSqrt!=0.) { dv2pro = duQpro/dQaQbSqrt; }
      Double_t dv2errRel2 = duQerrRel2 + dQaQbSqrtErrRel2;
      Double_t dv2errRel  = 0.;
      if (dv2errRel2>0.) { dv2errRel  = TMath::Sqrt(dv2errRel2); }
      Double_t dv2err     = dv2pro*dv2errRel; 
      //fill TH1D
      fCommonHistsRes->FillDifferentialFlowPtPOI(b, dv2pro, dv2err); 

      //calculate integrated flow for POI selection
      if (fHistPtPOI){
	Double_t dYieldPt = fHistPtPOI->GetBinContent(b);
	dVPOI += dv2pro*dYieldPt;
	dSum +=dYieldPt;
	dErrV += dYieldPt*dYieldPt*dv2err*dv2err;
      } else { cout<<"fHistPtPOI is NULL"<<endl; }
    } //loop over bins b

    if (dSum != 0.) {
      dVPOI /= dSum; //the pt distribution should be normalised
      dErrV /= (dSum*dSum);
      dErrV = TMath::Sqrt(dErrV);
    }
    fCommonHistsRes->FillIntegratedFlowPOI(dVPOI,dErrV);

    cout<<"dV(POI) = "<<dVPOI<<" +- "<<dErrV<<endl;
    }
  cout<<endl;
  cout<<"*************************************"<<endl;
  cout<<"*************************************"<<endl;   	  

  //cout<<".....finished"<<endl;
 }



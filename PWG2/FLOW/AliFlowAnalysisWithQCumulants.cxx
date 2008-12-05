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

/********************************** 
 * flow analysis with Q-cumulants * 
 *                                * 
 * author:  Ante Bilandzic        * 
 *           (anteb@nikhef.nl)    *
 *********************************/ 

#define AliFlowAnalysisWithQCumulants_cxx

#include "Riostream.h"
#include "AliFlowCommonConstants.h"
#include "AliFlowCommonHist.h"
#include "AliFlowCommonHistResults.h"
#include "TChain.h"
#include "TFile.h"
#include "TList.h"
#include "TParticle.h"
#include "TRandom3.h"
#include "TStyle.h"
#include "TProfile.h"
#include "TProfile2D.h" 
#include "TProfile3D.h"
#include "AliFlowEventSimple.h"
#include "AliFlowTrackSimple.h"
#include "AliFlowAnalysisWithQCumulants.h"
#include "AliQCumulantsFunctions.h"

#include "TRandom.h"

class TH1;
class TGraph;
class TPave;
class TLatex;
class TMarker;
class TRandom3;
class TObjArray;
class TList;
class TCanvas;
class TSystem;
class TROOT;
class AliFlowVector;
class TVector;

//================================================================================================================

ClassImp(AliFlowAnalysisWithQCumulants)

AliFlowAnalysisWithQCumulants::AliFlowAnalysisWithQCumulants():  
 fTrack(NULL),
 fHistList(NULL),
 fAvMultIntFlowQC(NULL),
 fQvectorComponents(NULL),
 fIntFlowResultsQC(NULL),
 fDiffFlowResults2ndOrderQC(NULL),
 fDiffFlowResults4thOrderQC(NULL),
 fCovariances(NULL),
 fQCorrelations(NULL),
 fQProduct(NULL),
 fDirectCorrelations(NULL),
 fReq1n(NULL),
 fImq1n(NULL),
 fReq2n(NULL),
 fImq2n(NULL),
 f2PerBin1n1n(NULL),
 f2PerBin2n2n(NULL),
 f3PerBin2n1n1n(NULL),
 f3PerBin1n1n2n(NULL),
 f4PerBin1n1n1n1n(NULL),
 fCommonHists(NULL),
 fCommonHistsResults2nd(NULL),
 fCommonHistsResults4th(NULL),
 fCommonHistsResults6th(NULL),
 fCommonHistsResults8th(NULL),
 f2pDistribution(NULL),
 f4pDistribution(NULL),
 f6pDistribution(NULL),
 fnBinsPt(0),
 fPtMin(0),
 fPtMax(0)
{
 //constructor 
 fHistList = new TList(); 
 
 fnBinsPt = AliFlowCommonConstants::GetNbinsPt();
 fPtMin   = AliFlowCommonConstants::GetPtMin();	     
 fPtMax   = AliFlowCommonConstants::GetPtMax();
 
}

AliFlowAnalysisWithQCumulants::~AliFlowAnalysisWithQCumulants()
{
 //desctructor
 delete fHistList; 
}

//================================================================================================================

void AliFlowAnalysisWithQCumulants::CreateOutputObjects()
{
 //various output histograms

 //avarage multiplicity 
 fAvMultIntFlowQC = new TProfile("fAvMultIntFlowQC","Average Multiplicity",1,0,1,"s");
 fAvMultIntFlowQC->SetXTitle("");
 fAvMultIntFlowQC->SetYTitle("");
 fAvMultIntFlowQC->SetLabelSize(0.06);
 fAvMultIntFlowQC->SetMarkerStyle(25);
 fAvMultIntFlowQC->SetLabelOffset(0.01);
 (fAvMultIntFlowQC->GetXaxis())->SetBinLabel(1,"Average Multiplicity");
 fHistList->Add(fAvMultIntFlowQC);
 
 //Q-vector stuff
 fQvectorComponents = new TProfile("fQvectorComponents","Avarage of Q-vector components",44,0.,44.,"s");
 fQvectorComponents->SetXTitle("");
 fQvectorComponents->SetYTitle("");
 //fHistList->Add(fQvectorComponents);
 
 //final results for integrated flow from Q-cumulants
 fIntFlowResultsQC = new TH1D("fIntFlowResultsQC","Integrated Flow from Q-cumulants",4,0,4);
 //fIntFlowResults->SetXTitle("");
 //fIntFlowResultsQC->SetYTitle("Integrated Flow");
 fIntFlowResultsQC->SetLabelSize(0.06);
 //fIntFlowResultsQC->SetTickLength(1);
 fIntFlowResultsQC->SetMarkerStyle(25);
 (fIntFlowResultsQC->GetXaxis())->SetBinLabel(1,"v_{n}{2}");
 (fIntFlowResultsQC->GetXaxis())->SetBinLabel(2,"v_{n}{4}");
 (fIntFlowResultsQC->GetXaxis())->SetBinLabel(3,"v_{n}{6}");
 (fIntFlowResultsQC->GetXaxis())->SetBinLabel(4,"v_{n}{8}");
 fHistList->Add(fIntFlowResultsQC);

 //final results for differential flow from 2nd order Q-cumulant
 fDiffFlowResults2ndOrderQC = new TH1D("fDiffFlowResults2ndOrderQC","Differential Flow from 2nd Order Q-cumulant",fnBinsPt,fPtMin,fPtMax);
 fDiffFlowResults2ndOrderQC->SetXTitle("p_{t} [GeV]");
 //fDiffFlowResults2ndOrderQC->SetYTitle("Differential Flow");
 fHistList->Add(fDiffFlowResults2ndOrderQC);
 
 //final results for differential flow from 4th order Q-cumulant
 fDiffFlowResults4thOrderQC = new TH1D("fDiffFlowResults4thOrderQC","Differential Flow from 4th Order Q-cumulant",fnBinsPt,fPtMin,fPtMax);
 fDiffFlowResults4thOrderQC->SetXTitle("p_{t} [GeV]");
 //fDiffFlowResults4thOrderQC->SetYTitle("Differential Flow");
 fHistList->Add(fDiffFlowResults4thOrderQC);
 
 //final results for covariances (1st bin: <2*4>-<2>*<4>, 2nd bin: <2*6>-<2>*<6>, ...)
 fCovariances = new TH1D("fCovariances","Covariances",6,0,6);
 //fCovariances->SetXTitle("");
 //fCovariances->SetYTitle("<covariance>");
 fCovariances->SetLabelSize(0.04);
 fCovariances->SetTickLength(1);
 fCovariances->SetMarkerStyle(25);
 (fCovariances->GetXaxis())->SetBinLabel(1,"Cov(2,4)");
 (fCovariances->GetXaxis())->SetBinLabel(2,"Cov(2,6)");
 (fCovariances->GetXaxis())->SetBinLabel(3,"Cov(2,8)");
 (fCovariances->GetXaxis())->SetBinLabel(4,"Cov(4,6)");
 (fCovariances->GetXaxis())->SetBinLabel(5,"Cov(4,8)");
 (fCovariances->GetXaxis())->SetBinLabel(6,"Cov(6,8)");
 fHistList->Add(fCovariances);
  
 //multi-particle correlations calculated from Q-vectors
 fQCorrelations = new TProfile("fQCorrelations","multi-particle correlations from Q-vectors",35,0,35,"s");
 //fQCorrelations->SetXTitle("correlations");
 //fQCorrelations->SetYTitle("");
 fQCorrelations->SetTickLength(-0.01,"Y");
 fQCorrelations->SetMarkerStyle(25);
 fQCorrelations->SetLabelSize(0.03);
 fQCorrelations->SetLabelOffset(0.01,"Y");
 (fQCorrelations->GetXaxis())->SetBinLabel(1,"<<2>>_{n|n}");
 (fQCorrelations->GetXaxis())->SetBinLabel(2,"<<2>>_{2n|2n}");
 (fQCorrelations->GetXaxis())->SetBinLabel(3,"<<2>>_{3n|3n}");
 (fQCorrelations->GetXaxis())->SetBinLabel(4,"<<2>>_{4n|4n}");
 (fQCorrelations->GetXaxis())->SetBinLabel(6,"<<3>>_{2n|n,n}");
 (fQCorrelations->GetXaxis())->SetBinLabel(7,"<<3>>_{3n|2n,n}");
 (fQCorrelations->GetXaxis())->SetBinLabel(8,"<<3>>_{4n|2n,2n}");
 (fQCorrelations->GetXaxis())->SetBinLabel(9,"<<3>>_{4n|3n,n}");
 (fQCorrelations->GetXaxis())->SetBinLabel(11,"<<4>>_{n,n|n,n}"); 
 (fQCorrelations->GetXaxis())->SetBinLabel(12,"<<4>>_{2n,n|2n,n}");
 (fQCorrelations->GetXaxis())->SetBinLabel(13,"<<4>>_{3n|n,n,n}");
 (fQCorrelations->GetXaxis())->SetBinLabel(14,"<<4>>_{4n|2n,n,n}");
 (fQCorrelations->GetXaxis())->SetBinLabel(15,"<<4>>_{3n,n|2n,2n}"); 
 (fQCorrelations->GetXaxis())->SetBinLabel(16,"<<5>>_{2n|n,n,n,n}"); 
 (fQCorrelations->GetXaxis())->SetBinLabel(17,"<<5>>_{2n,2n|2n,n,n}");
 (fQCorrelations->GetXaxis())->SetBinLabel(18,"<<5>>_{3n,n|2n,n,n}");
 (fQCorrelations->GetXaxis())->SetBinLabel(19,"<<5>>_{4n|n,n,n,n}");
 (fQCorrelations->GetXaxis())->SetBinLabel(21,"<<6>>_{n,n,n|n,n,n}");
 fHistList->Add(fQCorrelations);
 
 //average products
 fQProduct = new TProfile("fQProduct","average of products",6,0,6,"s");
 fQProduct->SetTickLength(-0.01,"Y");
 fQProduct->SetMarkerStyle(25);
 fQProduct->SetLabelSize(0.03);
 fQProduct->SetLabelOffset(0.01,"Y");
 (fQProduct->GetXaxis())->SetBinLabel(1,"<<2*4>>");
 (fQProduct->GetXaxis())->SetBinLabel(2,"<<2*6>>");
 (fQProduct->GetXaxis())->SetBinLabel(3,"<<2*8>>");
 (fQProduct->GetXaxis())->SetBinLabel(4,"<<4*6>>");
 (fQProduct->GetXaxis())->SetBinLabel(5,"<<4*8>>");
 (fQProduct->GetXaxis())->SetBinLabel(6,"<<6*8>>");
 fQProduct->SetXTitle("");
 fQProduct->SetYTitle("");
 fHistList->Add(fQProduct);
 
 //multi-particle correlations calculated with nested loops (0..40 integrated flow; 40..80 differential flow)
 fDirectCorrelations = new TProfile("fDirectCorrelations","multi-particle correlations with nested loops",80,0,80,"s");
 fDirectCorrelations->SetXTitle("");
 fDirectCorrelations->SetYTitle("correlations");
 fHistList->Add(fDirectCorrelations);
 
 //fReq1n
 fReq1n = new TProfile("fReq1n","Re[q_n]",fnBinsPt,fPtMin,fPtMax,"s");
 fReq1n->SetXTitle("p_{t} [GeV]");
 fReq1n->SetYTitle("Re[q_n]");
 //fHistList->Add(fReq1n);
 
 //fImq1n
 fImq1n = new TProfile("fImq1n","Im[q_n]",fnBinsPt,fPtMin,fPtMax,"s");
 fImq1n->SetXTitle("p_{t} [GeV]");
 fImq1n->SetYTitle("Im[q_n]");
 //fHistList->Add(fImq1n);
 
 //fReq2n
 fReq2n = new TProfile("fReq2n","Re[q_2n]",fnBinsPt,fPtMin,fPtMax,"s");
 fReq2n->SetXTitle("p_{t} [GeV]");
 fReq2n->SetYTitle("Im[D]");
 //fHistList->Add(fReq2n);
 
 //fImq2n
 fImq2n = new TProfile("fImq2n","Im[q_2n]",fnBinsPt,fPtMin,fPtMax,"s");
 fImq2n->SetXTitle("p_{t} [GeV]");
 fImq2n->SetYTitle("Im[q_2n]");
 //fHistList->Add(fImq2n);
 
 //f2PerBin1n1n
 f2PerBin1n1n = new TProfile("f2PerBin1n1n","<2'>_{n|n}",fnBinsPt,fPtMin,fPtMax,"s");
 f2PerBin1n1n->SetXTitle("p_{t} [GeV]");
 //f2PerBin1n1n->SetYTitle("<2'>_{n|n}");
 fHistList->Add(f2PerBin1n1n);
 
 //f2PerBin2n2n
 f2PerBin2n2n = new TProfile("f2PerBin2n2n","<2'>_{2n|2n}",fnBinsPt,fPtMin,fPtMax,"s");
 f2PerBin2n2n->SetXTitle("p_{t} [GeV]");
 //f2PerBin2n2n->SetYTitle("<2'>_{2n|2n}");
 fHistList->Add(f2PerBin2n2n);
 
 //f3PerBin2n1n1n
 f3PerBin2n1n1n = new TProfile("f3PerBin2n1n1n","<3'>_{2n|n,n}",fnBinsPt,fPtMin,fPtMax,"s");
 f3PerBin2n1n1n->SetXTitle("p_{t} [GeV]");
 //f3PerBin2n1n1n->SetYTitle("<3'>_{2n|n,n}");
 fHistList->Add(f3PerBin2n1n1n);
 
 //f3PerBin1n1n2n
 f3PerBin1n1n2n = new TProfile("f3PerBin1n1n2n","<3'>_{n,n|2n}",fnBinsPt,fPtMin,fPtMax,"s");
 f3PerBin1n1n2n->SetXTitle("p_{t} [GeV]");
 //f3PerBin1n1n2n->SetYTitle("<3'>_{n,n|2n}");
 fHistList->Add(f3PerBin1n1n2n);
 
 //f4PerBin1n1n1n1n
 f4PerBin1n1n1n1n = new TProfile("f4PerBin1n1n1n1n","<4'>_{n,n|n,n}",fnBinsPt,fPtMin,fPtMax,"s");
 f4PerBin1n1n1n1n->SetXTitle("p_{t} [GeV]");
 //f4PerBin1n1n1n1n->SetYTitle("<4'>_{n,n|n,n}");
 fHistList->Add(f4PerBin1n1n1n1n);
 
 //common control histograms
 fCommonHists = new AliFlowCommonHist("AliFlowCommonHistQC");
 fHistList->Add(fCommonHists);  
 
 //common histograms for final results (2nd order)
 fCommonHistsResults2nd = new AliFlowCommonHistResults("AliFlowCommonHistResults2ndOrderQC");
 fHistList->Add(fCommonHistsResults2nd); 
 
 //common histograms for final results (4th order)
 fCommonHistsResults4th = new AliFlowCommonHistResults("AliFlowCommonHistResults4thOrderQC");
 fHistList->Add(fCommonHistsResults4th);
 
 //common histograms for final results (6th order)
 fCommonHistsResults6th = new AliFlowCommonHistResults("AliFlowCommonHistResults6thOrderQC");
 fHistList->Add(fCommonHistsResults6th); 
 
 //common histograms for final results (8th order)
 fCommonHistsResults8th = new AliFlowCommonHistResults("AliFlowCommonHistResults8thOrderQC");
 fHistList->Add(fCommonHistsResults8th); 
 
 //weighted <2>_{n|n} distribution
 f2pDistribution = new TH1D("f2pDistribution","<2>_{n|n} distribution",100000,-0.02,0.1);
 f2pDistribution->SetXTitle("<2>_{n|n}");
 f2pDistribution->SetYTitle("Counts");
 fHistList->Add(f2pDistribution);

 //weighted <4>_{n,n|n,n} distribution
 f4pDistribution = new TH1D("f4pDistribution","<4>_{n,n|n,n} distribution",100000,-0.00025,0.002);
 f4pDistribution->SetXTitle("<4>_{n,n|n,n}");
 f4pDistribution->SetYTitle("Counts");
 fHistList->Add(f4pDistribution); 
 
 //weighted <6>_{n,n,n|n,n,n} distribution
 f6pDistribution = new TH1D("f6pDistribution","<6>_{n,n,n|n,n,n} distribution",100000,-0.000005,0.000025);
 f6pDistribution->SetXTitle("<6>_{n,n,n|n,n,n}");
 f6pDistribution->SetYTitle("Counts");
 fHistList->Add(f6pDistribution);
 
}//end of CreateOutputObjects()

//================================================================================================================

void AliFlowAnalysisWithQCumulants::Make(AliFlowEventSimple* anEvent)
{
 //running over data     
          
 //get the total multiplicity of event:
 //Int_t nPrim = anEvent->NumberOfTracks();

 //if(nPrim>8&&nPrim<14)  
 //{

 //fill the common control histograms:
 fCommonHists->FillControlHistograms(anEvent); 
 
 //get the selected multiplicity (i.e. number of particles used for int. flow):
 //Int_t nEventNSelTracksIntFlow = anEvent->GetEventNSelTracksIntFlow();
 
 Int_t n=2; //int flow harmonic (to be improved)
 
 //---------------------------------------------------------------------------------------------------------
 //Q-vectors of an event evaluated in harmonics n, 2n, 3n and 4n:
 AliFlowVector xQvector1n, xQvector2n, xQvector3n, xQvector4n;
 
 xQvector1n.Set(0.,0.);
 xQvector1n.SetMult(0);
 xQvector1n=anEvent->GetQ(1*n); 
 
 xQvector2n.Set(0.,0.);
 xQvector2n.SetMult(0);
 xQvector2n=anEvent->GetQ(2*n);                          
 
 xQvector3n.Set(0.,0.);
 xQvector3n.SetMult(0);
 xQvector3n=anEvent->GetQ(3*n);       
 
 xQvector4n.Set(0.,0.);
 xQvector4n.SetMult(0);
 xQvector4n=anEvent->GetQ(4*n);       
 //---------------------------------------------------------------------------------------------------------
 
 //multiplicity (to be improved, because I already have nEventNSelTracksIntFlow and nPrim)
 Double_t xMult = xQvector1n.GetMult();
 
 fAvMultIntFlowQC->Fill(0.,xMult,1.);
 
 //---------------------------------------------------------------------------------------------------------
 //*************
 //* Q-vectors *
 //*************
 Double_t reQ2nQ1nstarQ1nstar = pow(xQvector1n.X(),2.)*xQvector2n.X()+2.*xQvector1n.X()*xQvector1n.Y()*xQvector2n.Y()-pow(xQvector1n.Y(),2.)*xQvector2n.X();//Re[Q_{2n} Q_{n}^* Q_{n}^*]
 //Double_t imQ2nQ1nstarQ1nstar = pow(Qvector1n.X(),2.)*Qvector2n.Y()-2.*Qvector1n.X()*Qvector1n.Y()*Qvector2n.X()-pow(Qvector1n.Y(),2.)*Qvector2n.Y();//Im[Q_{2n} Q_{n}^* Q_{n}^*]
 Double_t reQ1nQ1nQ2nstar = reQ2nQ1nstarQ1nstar;//Re[Q_{n} Q_{n} Q_{2n}^*] = Re[Q_{2n} Q_{n}^* Q_{n}^*]
 Double_t reQ3nQ1nQ2nstarQ2nstar = (pow(xQvector2n.X(),2.)-pow(xQvector2n.Y(),2.))*(xQvector3n.X()*xQvector1n.X()-xQvector3n.Y()*xQvector1n.Y())+2.*xQvector2n.X()*xQvector2n.Y()*(xQvector3n.X()*xQvector1n.Y()+xQvector3n.Y()*xQvector1n.X());
 //Double_t imQ3nQ1nQ2nstarQ2nstar = calculate and implement this (deleteMe) 
 Double_t reQ2nQ2nQ3nstarQ1nstar = reQ3nQ1nQ2nstarQ2nstar;
 Double_t reQ4nQ2nstarQ2nstar = pow(xQvector2n.X(),2.)*xQvector4n.X()+2.*xQvector2n.X()*xQvector2n.Y()*xQvector4n.Y()-pow(xQvector2n.Y(),2.)*xQvector4n.X();//Re[Q_{4n} Q_{2n}^* Q_{2n}^*]
 //Double_t imQ4nQ2nstarQ2nstar = calculate and implement this (deleteMe)
 Double_t reQ2nQ2nQ4nstar = reQ4nQ2nstarQ2nstar;
 Double_t reQ4nQ3nstarQ1nstar = xQvector4n.X()*(xQvector3n.X()*xQvector1n.X()-xQvector3n.Y()*xQvector1n.Y())+xQvector4n.Y()*(xQvector3n.X()*xQvector1n.Y()+xQvector3n.Y()*xQvector1n.X());//Re[Q_{4n} Q_{3n}^* Q_{n}^*]
 Double_t reQ3nQ1nQ4nstar = reQ4nQ3nstarQ1nstar;//Re[Q_{3n} Q_{n} Q_{4n}^*] = Re[Q_{4n} Q_{3n}^* Q_{n}^*]
 //Double_t imQ4nQ3nstarQ1nstar = calculate and implement this (deleteMe)
 Double_t reQ3nQ2nstarQ1nstar = xQvector3n.X()*xQvector2n.X()*xQvector1n.X()-xQvector3n.X()*xQvector2n.Y()*xQvector1n.Y()+xQvector3n.Y()*xQvector2n.X()*xQvector1n.Y()+xQvector3n.Y()*xQvector2n.Y()*xQvector1n.X();//Re[Q_{3n} Q_{2n}^* Q_{n}^*]
 Double_t reQ2nQ1nQ3nstar = reQ3nQ2nstarQ1nstar;//Re[Q_{2n} Q_{n} Q_{3n}^*] = Re[Q_{3n} Q_{2n}^* Q_{n}^*]
 //Double_t imQ3nQ2nstarQ1nstar; //calculate and implement this (deleteMe)
 Double_t reQ3nQ1nstarQ1nstarQ1nstar = xQvector3n.X()*pow(xQvector1n.X(),3)-3.*xQvector1n.X()*xQvector3n.X()*pow(xQvector1n.Y(),2)+3.*xQvector1n.Y()*xQvector3n.Y()*pow(xQvector1n.X(),2)-xQvector3n.Y()*pow(xQvector1n.Y(),3);//Re[Q_{3n} Q_{n}^* Q_{n}^* Q_{n}^*]
 //Double_t imQ3nQ1nstarQ1nstarQ1nstar; //calculate and implement this (deleteMe)
 Double_t xQ2nQ1nQ2nstarQ1nstar = pow(xQvector2n.Mod()*xQvector1n.Mod(),2);//|Q_{2n}|^2 |Q_{n}|^2
 Double_t reQ4nQ2nstarQ1nstarQ1nstar = (xQvector4n.X()*xQvector2n.X()+xQvector4n.Y()*xQvector2n.Y())*(pow(xQvector1n.X(),2)-pow(xQvector1n.Y(),2))+2.*xQvector1n.X()*xQvector1n.Y()*(xQvector4n.Y()*xQvector2n.X()-xQvector4n.X()*xQvector2n.Y());//Re[Q_{4n} Q_{2n}^* Q_{n}^* Q_{n}^*] 
 //Double_t imQ4nQ2nstarQ1nstarQ1nstar; //calculate and implement this (deleteMe)
 Double_t reQ2nQ1nQ1nstarQ1nstarQ1nstar = (xQvector2n.X()*xQvector1n.X()-xQvector2n.Y()*xQvector1n.Y())*(pow(xQvector1n.X(),3)-3.*xQvector1n.X()*pow(xQvector1n.Y(),2))+(xQvector2n.X()*xQvector1n.Y()+xQvector1n.X()*xQvector2n.Y())*(3.*xQvector1n.Y()*pow(xQvector1n.X(),2)-pow(xQvector1n.Y(),3));//Re[Q_{2n} Q_{n} Q_{n}^* Q_{n}^* Q_{n}^*]
 //Double_t imQ2nQ1nQ1nstarQ1nstarQ1nstar; //calculate and implement this (deleteMe)
 Double_t reQ2nQ2nQ2nstarQ1nstarQ1nstar = pow(xQvector2n.Mod(),2.)*(xQvector2n.X()*(pow(xQvector1n.X(),2.)-pow(xQvector1n.Y(),2.))+2.*xQvector2n.Y()*xQvector1n.X()*xQvector1n.Y());//Re[Q_{2n} Q_{2n} Q_{2n}^* Q_{n}^* Q_{n}^*]
 //Double_t imQ2nQ2nQ2nstarQ1nstarQ1nstar = pow(Qvector2n.Mod(),2.)*(Qvector2n.Y()*(pow(Qvector1n.X(),2.)-pow(Qvector1n.Y(),2.))-2.*Qvector2n.X()*Qvector1n.X()*Qvector1n.Y());//Im[Q_{2n} Q_{2n} Q_{2n}^* Q_{n}^* Q_{n}^*]
 Double_t reQ4nQ1nstarQ1nstarQ1nstarQ1nstar = pow(xQvector1n.X(),4.)*xQvector4n.X()-6.*pow(xQvector1n.X(),2.)*xQvector4n.X()*pow(xQvector1n.Y(),2.)+pow(xQvector1n.Y(),4.)*xQvector4n.X()+4.*pow(xQvector1n.X(),3.)*xQvector1n.Y()*xQvector4n.Y()-4.*pow(xQvector1n.Y(),3.)*xQvector1n.X()*xQvector4n.Y();
 //Double_t imQ4nQ1nstarQ1nstarQ1nstarQ1nstar = pow(Qvector1n.X(),4.)*Qvector4n.Y()-6.*pow(Qvector1n.X(),2.)*Qvector4n.Y()*pow(Qvector1n.Y(),2.)+pow(Qvector1n.Y(),4.)*Qvector4n.Y()+4.*pow(Qvector1n.Y(),3.)*Qvector1n.X()*Qvector4n.X()-4.*pow(Qvector1n.X(),3.)*Qvector1n.Y()*Qvector4n.X();

 
 //---------------------------------------------------------------------------------------------------------
 
 //---------------------------------------------------------------------------------------------------------
 //********************************
 //* multi-particle correlations: *
 //********************************

 // binning of fQCorrelations: 
 // 1st bin: <2>_{n|n} = two_1n1n
 // 2nd bin: <2>_{2n|2n} = two_2n2n
 // 3rd bin: <2>_{3n|3n} = two_3n3n
 // 4th bin: <2>_{4n|4n} = two_4n4n
 // 6th bin: <3>_{2n|n,n} = three_2n1n1n
 // 7th bin: <3>_{3n|2n,n} = three_3n2n1n
 // 8th bin: <3>_{4n|2n,2n} = three_4n2n2n
 // 9th bin: <3>_{4n|3n,n} = three_4n3n1n
 //11th bin: <4>_{n,n|n,n} = four_1n1n1n1n
 //12th bin: <4>_{2n,n|2n,n} = four_2n1n2n1n
 //13th bin: <4>_{3n|n,n,n} = four_3n1n1n1n
 //14th bin: <4>_{4n|2n,n,n} = four_4n2n1n1n
 //15th bin: <4>_{3n,n|2n,2n} = four_3n1n2n2n
 //16th bin: <5>_{2n|n,n,n,n} = five_2n1n1n1n1n
 //17th bin: <5>_{2n,2n|2n,n,n} = five_2n2n2n1n1n
 //18th bin: <5>_{3n,n|2n,n,n} = five_3n1n2n1n1n
 //19th bin: <5>_{4n|n,n,n,n} = five_4n1n1n1n1n  
 //21th bin: <6>_{n,n,n|n,n,n} = six_1n1n1n1n1n1n
 
 //31th bin: <4>_{2n,2n|2n,2n} = four_2n2n2n2n //to be improved
 //32nd bin: <4>_{3n,n|3n,n} = four_3n1n3n1n   //to be improved
 
 // binning of fQProduct (all correlations are evaluated in harmonic n): 
 // 1st bin: <2>*<4>
 // 2nd bin: <2>*<6>
 // 3rd bin: <2>*<8> 
 // 4th bin: <4>*<6>
 // 5th bin: <4>*<8>
 // 6th bin: <6>*<8>
         
 //2-particle
 Double_t two1n1n=0., two2n2n=0., two3n3n=0., two4n4n=0.; 
 if(xMult>1)
 {
  two1n1n = (pow(xQvector1n.Mod(),2.)-xMult)/(xMult*(xMult-1.)); //<2>_{n|n}   = <cos(n*(phi1-phi2))>
  two2n2n = (pow(xQvector2n.Mod(),2.)-xMult)/(xMult*(xMult-1.)); //<2>_{2n|2n} = <cos(2n*(phi1-phi2))>
  two3n3n = (pow(xQvector3n.Mod(),2.)-xMult)/(xMult*(xMult-1.)); //<2>_{3n|3n} = <cos(3n*(phi1-phi2))>
  two4n4n = (pow(xQvector4n.Mod(),2.)-xMult)/(xMult*(xMult-1.)); //<2>_{4n|4n} = <cos(4n*(phi1-phi2))>
    
  fQCorrelations->Fill(0.,two1n1n,xMult*(xMult-1.)); 
  fQCorrelations->Fill(1.,two2n2n,xMult*(xMult-1.)); 
  fQCorrelations->Fill(2.,two3n3n,xMult*(xMult-1.)); 
  fQCorrelations->Fill(3.,two4n4n,xMult*(xMult-1.)); 
  
  f2pDistribution->Fill(two1n1n,xMult*(xMult-1.)); 
 }
 
 //3-particle
 Double_t three2n1n1n=0., three3n2n1n=0., three4n2n2n=0., three4n3n1n=0.;
 if(xMult>2)
 {
  three2n1n1n = (reQ2nQ1nstarQ1nstar-2.*pow(xQvector1n.Mod(),2.)-pow(xQvector2n.Mod(),2.)+2.*xMult)/(xMult*(xMult-1.)*(xMult-2.)); //Re[<3>_{2n|n,n}] = Re[<3>_{n,n|2n}] = <cos(n*(2.*phi1-phi2-phi3))>
  three3n2n1n = (reQ3nQ2nstarQ1nstar-pow(xQvector3n.Mod(),2.)-pow(xQvector2n.Mod(),2.)-pow(xQvector1n.Mod(),2.)+2.*xMult)/(xMult*(xMult-1.)*(xMult-2.)); //Re[<3>_{3n|2n,n}] = Re[<3>_{2n,n|3n}] = <cos(n*(3.*phi1-2.*phi2-phi3))>
  three4n2n2n = (reQ4nQ2nstarQ2nstar-2.*pow(xQvector2n.Mod(),2.)-pow(xQvector4n.Mod(),2.)+2.*xMult)/(xMult*(xMult-1.)*(xMult-2.)); //Re[<3>_{4n|2n,2n}] = Re[<3>_{2n,2n|4n}] = <cos(n*(4.*phi1-2.*phi2-2.*phi3))>
  three4n3n1n = (reQ4nQ3nstarQ1nstar-pow(xQvector4n.Mod(),2.)-pow(xQvector3n.Mod(),2.)-pow(xQvector1n.Mod(),2.)+2.*xMult)/(xMult*(xMult-1.)*(xMult-2.)); //Re[<3>_{4n|3n,n}] = Re[<3>_{3n,n|4n}] = <cos(n*(4.*phi1-3.*phi2-phi3))>
 
  fQCorrelations->Fill(5.,three2n1n1n,xMult*(xMult-1.)*(xMult-2.)); 
  fQCorrelations->Fill(6.,three3n2n1n,xMult*(xMult-1.)*(xMult-2.));
  fQCorrelations->Fill(7.,three4n2n2n,xMult*(xMult-1.)*(xMult-2.)); 
  fQCorrelations->Fill(8.,three4n3n1n,xMult*(xMult-1.)*(xMult-2.));    
 }
 
 //4-particle
 Double_t four1n1n1n1n=0., four2n2n2n2n=0., four2n1n2n1n=0., four3n1n1n1n=0., four4n2n1n1n=0., four3n1n2n2n=0., four3n1n3n1n=0.;  
 if(xMult>3)
 {
  four1n1n1n1n = (2.*xMult*(xMult-3.)+pow(xQvector1n.Mod(),4.)-4.*(xMult-2.)*pow(xQvector1n.Mod(),2.)-2.*reQ2nQ1nstarQ1nstar+pow(xQvector2n.Mod(),2.))/(xMult*(xMult-1)*(xMult-2.)*(xMult-3.));//<4>_{n,n|n,n}
  four2n2n2n2n = (2.*xMult*(xMult-3.)+pow(xQvector2n.Mod(),4.)-4.*(xMult-2.)*pow(xQvector2n.Mod(),2.)-2.*reQ4nQ2nstarQ2nstar+pow(xQvector4n.Mod(),2.))/(xMult*(xMult-1)*(xMult-2.)*(xMult-3.));//<4>_{2n,2n|2n,2n}
  four2n1n2n1n = (xQ2nQ1nQ2nstarQ1nstar-2.*reQ3nQ2nstarQ1nstar-2.*reQ2nQ1nstarQ1nstar)/(xMult*(xMult-1.)*(xMult-2.)*(xMult-3.))-((xMult-5.)*pow(xQvector1n.Mod(),2.)+(xMult-4.)*pow(xQvector2n.Mod(),2.)-pow(xQvector3n.Mod(),2.))/(xMult*(xMult-1.)*(xMult-2.)*(xMult-3.))+(xMult-6.)/((xMult-1.)*(xMult-2.)*(xMult-3.));//Re[<4>_{2n,n|2n,n}]
  four3n1n1n1n = (reQ3nQ1nstarQ1nstarQ1nstar-3.*reQ3nQ2nstarQ1nstar-3.*reQ2nQ1nstarQ1nstar)/(xMult*(xMult-1.)*(xMult-2.)*(xMult-3.))+(2.*pow(xQvector3n.Mod(),2.)+3.*pow(xQvector2n.Mod(),2.)+6.*pow(xQvector1n.Mod(),2.)-6.*xMult)/(xMult*(xMult-1.)*(xMult-2.)*(xMult-3.));//Re[<4>_{3n|n,n,n}]
  four4n2n1n1n = (reQ4nQ2nstarQ1nstarQ1nstar-2.*reQ4nQ3nstarQ1nstar-reQ4nQ2nstarQ2nstar-2.*reQ3nQ2nstarQ1nstar)/(xMult*(xMult-1.)*(xMult-2.)*(xMult-3.))-(reQ2nQ1nstarQ1nstar-2.*pow(xQvector4n.Mod(),2.)-2.*pow(xQvector3n.Mod(),2.)-3.*pow(xQvector2n.Mod(),2.)-4.*pow(xQvector1n.Mod(),2.))/(xMult*(xMult-1.)*(xMult-2.)*(xMult-3.))-(6.)/((xMult-1.)*(xMult-2.)*(xMult-3.));//Re[<4>_{4n|2n,n,n}]
  four3n1n2n2n = (reQ3nQ1nQ2nstarQ2nstar-reQ4nQ2nstarQ2nstar-reQ3nQ1nQ4nstar-2.*reQ3nQ2nstarQ1nstar)/(xMult*(xMult-1.)*(xMult-2.)*(xMult-3.))-(2.*reQ1nQ1nQ2nstar-pow(xQvector4n.Mod(),2.)-2.*pow(xQvector3n.Mod(),2.)-4.*pow(xQvector2n.Mod(),2.)-4.*pow(xQvector1n.Mod(),2.))/(xMult*(xMult-1.)*(xMult-2.)*(xMult-3.))-(6.)/((xMult-1.)*(xMult-2.)*(xMult-3.));//Re[<4>_{3n,n|2n,2n}] 
  four3n1n3n1n = (pow(xQvector3n.Mod(),2.)*pow(xQvector1n.Mod(),2.)-2.*reQ4nQ3nstarQ1nstar-2.*reQ3nQ2nstarQ1nstar)/(xMult*(xMult-1.)*(xMult-2.)*(xMult-3.))+(pow(xQvector4n.Mod(),2.)-(xMult-4.)*pow(xQvector3n.Mod(),2.)+pow(xQvector2n.Mod(),2.)-(xMult-4.)*pow(xQvector1n.Mod(),2.))/(xMult*(xMult-1.)*(xMult-2.)*(xMult-3.))+(xMult-6.)/((xMult-1.)*(xMult-2.)*(xMult-3.));//<4>_{3n,n|3n,n}
  //four_3n1n3n1n = Q3nQ1nQ3nstarQ1nstar/(M*(M-1.)*(M-2.)*(M-3.))-(2.*three_3n2n1n+2.*three_4n3n1n)/(M-3.)-(two_4n4n+M*two_3n3n+two_2n2n+M*two_1n1n)/((M-2.)*(M-3.))-M/((M-1.)*(M-2.)*(M-3.));//<4>_{3n,n|3n,n}
  
  fQCorrelations->Fill(10.,four1n1n1n1n,xMult*(xMult-1.)*(xMult-2.)*(xMult-3.));
  
  fQCorrelations->Fill(30.,four2n2n2n2n,xMult*(xMult-1.)*(xMult-2.)*(xMult-3.));//to be improved
  fQCorrelations->Fill(31.,four3n1n3n1n,xMult*(xMult-1.)*(xMult-2.)*(xMult-3.));//to be improved 
    
  fQCorrelations->Fill(11.,four2n1n2n1n,xMult*(xMult-1.)*(xMult-2.)*(xMult-3.));
  fQCorrelations->Fill(12.,four3n1n1n1n,xMult*(xMult-1.)*(xMult-2.)*(xMult-3.));
  fQCorrelations->Fill(13.,four4n2n1n1n,xMult*(xMult-1.)*(xMult-2.)*(xMult-3.)); 
  fQCorrelations->Fill(14.,four3n1n2n2n,xMult*(xMult-1.)*(xMult-2.)*(xMult-3.));
  
  f4pDistribution->Fill(four1n1n1n1n,xMult*(xMult-1.)*(xMult-2.)*(xMult-3.));  
     
  fQProduct->Fill(0.,two1n1n*four1n1n1n1n,xMult*(xMult-1.)*xMult*(xMult-1.)*(xMult-2.)*(xMult-3.));
 }

 //5-particle
 Double_t five2n1n1n1n1n=0., five2n2n2n1n1n=0., five3n1n2n1n1n=0., five4n1n1n1n1n=0.;
 if(xMult>4)
 {
  five2n1n1n1n1n = (reQ2nQ1nQ1nstarQ1nstarQ1nstar-reQ3nQ1nstarQ1nstarQ1nstar+6.*reQ3nQ2nstarQ1nstar)/(xMult*(xMult-1.)*(xMult-2.)*(xMult-3.)*(xMult-4.))-(reQ2nQ1nQ3nstar+3.*(xMult-6.)*reQ2nQ1nstarQ1nstar+3.*reQ1nQ1nQ2nstar)/(xMult*(xMult-1.)*(xMult-2.)*(xMult-3.)*(xMult-4.))-(2.*pow(xQvector3n.Mod(),2.)+3.*pow(xQvector2n.Mod()*xQvector1n.Mod(),2.)-3.*(xMult-4.)*pow(xQvector2n.Mod(),2.))/(xMult*(xMult-1.)*(xMult-2.)*(xMult-3.)*(xMult-4.))-3.*(pow(xQvector1n.Mod(),4.)-2.*(2*xMult-5.)*pow(xQvector1n.Mod(),2.)+2.*xMult*(xMult-4.))/(xMult*(xMult-1.)*(xMult-2.)*(xMult-3.)*(xMult-4.));//Re[<5>_{2n,n|n,n,n}]
  
  five2n2n2n1n1n = (reQ2nQ2nQ2nstarQ1nstarQ1nstar-reQ4nQ2nstarQ1nstarQ1nstar-2.*reQ2nQ2nQ3nstarQ1nstar)/(xMult*(xMult-1.)*(xMult-2.)*(xMult-3.)*(xMult-4.))+2.*(reQ4nQ2nstarQ2nstar+4.*reQ3nQ2nstarQ1nstar+reQ3nQ1nQ4nstar)/(xMult*(xMult-1.)*(xMult-2.)*(xMult-3.)*(xMult-4.))+(reQ2nQ2nQ4nstar-2.*(xMult-5.)*reQ2nQ1nstarQ1nstar+2.*reQ1nQ1nQ2nstar)/(xMult*(xMult-1.)*(xMult-2.)*(xMult-3.)*(xMult-4.))-(2.*pow(xQvector4n.Mod(),2.)+4.*pow(xQvector3n.Mod(),2.)+1.*pow(xQvector2n.Mod(),4.)-2.*(3.*xMult-10.)*pow(xQvector2n.Mod(),2.))/(xMult*(xMult-1.)*(xMult-2.)*(xMult-3.)*(xMult-4.))-(4.*pow(xQvector1n.Mod(),2.)*pow(xQvector2n.Mod(),2.)-4.*(xMult-5.)*pow(xQvector1n.Mod(),2.)+4.*xMult*(xMult-6.))/(xMult*(xMult-1.)*(xMult-2.)*(xMult-3.)*(xMult-4.));//Re[<5>_{2n,2n|2n,n,n}]  

  //five_2n2n2n1n1n = reQ2nQ2nQ2nstarQ1nstarQ1nstar/(M*(M-1.)*(M-2.)*(M-3.)*(M-4.))-(4.*four_2n1n2n1n+2.*four_3n1n2n2n+1.*four_2n2n2n2n+four_4n2n1n1n)/(M-4.)-(2.*three_4n3n1n+three_4n2n2n+three_4n2n2n+2.*three_3n2n1n)/((M-3.)*(M-4.))-(4.*three_3n2n1n+(2.*M-1.)*three_2n1n1n+2.*three_2n1n1n)/((M-3.)*(M-4.))-(two_4n4n+2.*two_3n3n+4.*(M-1.)*two_2n2n+2.*(2.*M-1.)*two_1n1n)/((M-2.)*(M-3.)*(M-4.))-(2.*M-1.)/((M-1.)*(M-2.)*(M-3.)*(M-4.));  
   
  five4n1n1n1n1n = (reQ4nQ1nstarQ1nstarQ1nstarQ1nstar-6.*reQ4nQ2nstarQ1nstarQ1nstar-4.*reQ3nQ1nstarQ1nstarQ1nstar)/(xMult*(xMult-1.)*(xMult-2.)*(xMult-3.)*(xMult-4.))+(8.*reQ4nQ3nstarQ1nstar+3.*reQ4nQ2nstarQ2nstar+12.*reQ3nQ2nstarQ1nstar+12.*reQ2nQ1nstarQ1nstar)/(xMult*(xMult-1.)*(xMult-2.)*(xMult-3.)*(xMult-4.))-(6.*pow(xQvector4n.Mod(),2.)+8.*pow(xQvector3n.Mod(),2.)+12.*pow(xQvector2n.Mod(),2.)+24.*pow(xQvector1n.Mod(),2.)-24.*xMult)/(xMult*(xMult-1.)*(xMult-2.)*(xMult-3.)*(xMult-4.));//Re[<5>_{4n|n,n,n,n}] 
  
  //five_4n1n1n1n1n = reQ4nQ1nstarQ1nstarQ1nstarQ1nstar/(M*(M-1.)*(M-2.)*(M-3.)*(M-4.)) -  (4.*four_3n1n1n1n+6.*four_4n2n1n1n)/(M-4.)  -  (6.*three_2n1n1n  + 12.*three_3n2n1n + 4.*three_4n3n1n + 3.*three_4n2n2n)/((M-3.)*(M-4.))  -  (4.*two_1n1n + 6.*two_2n2n + 4.*two_3n3n + 1.*two_4n4n)/((M-2.)*(M-3.)*(M-4.)) - 1./((M-1.)*(M-2.)*(M-3.)*(M-4.));
  
  fQCorrelations->Fill(15.,five2n1n1n1n1n,xMult*(xMult-1.)*(xMult-2.)*(xMult-3.)*(xMult-4.)); 
  fQCorrelations->Fill(16.,five2n2n2n1n1n,xMult*(xMult-1.)*(xMult-2.)*(xMult-3.)*(xMult-4.));
  fQCorrelations->Fill(17.,five3n1n2n1n1n,xMult*(xMult-1.)*(xMult-2.)*(xMult-3.)*(xMult-4.));
  fQCorrelations->Fill(18.,five4n1n1n1n1n,xMult*(xMult-1.)*(xMult-2.)*(xMult-3.)*(xMult-4.));
 }

 //6-particle
 Double_t six1n1n1n1n1n1n=0.;
 if(xMult>5)
 {
  six1n1n1n1n1n1n = (pow(xQvector1n.Mod(),6.)+9.*xQ2nQ1nQ2nstarQ1nstar-6.*reQ2nQ1nQ1nstarQ1nstarQ1nstar)/(xMult*(xMult-1)*(xMult-2)*(xMult-3)*(xMult-4)*(xMult-5))+4.*(reQ3nQ1nstarQ1nstarQ1nstar-3.*reQ3nQ2nstarQ1nstar)/(xMult*(xMult-1)*(xMult-2)*(xMult-3)*(xMult-4)*(xMult-5))+2.*(9.*(xMult-4.)*reQ2nQ1nstarQ1nstar+2.*pow(xQvector3n.Mod(),2.))/(xMult*(xMult-1)*(xMult-2)*(xMult-3)*(xMult-4)*(xMult-5))-9.*(pow(xQvector1n.Mod(),4.)+pow(xQvector2n.Mod(),2.))/(xMult*(xMult-1)*(xMult-2)*(xMult-3)*(xMult-5))+(18.*pow(xQvector1n.Mod(),2.))/(xMult*(xMult-1)*(xMult-3)*(xMult-4))-(6.)/((xMult-1)*(xMult-2)*(xMult-3));//<6>_{n,n,n|n,n,n}
  
  fQCorrelations->Fill(20.,six1n1n1n1n1n1n,xMult*(xMult-1.)*(xMult-2.)*(xMult-3.)*(xMult-4.)*(xMult-5.));  
  
  f6pDistribution->Fill(six1n1n1n1n1n1n,xMult*(xMult-1.)*(xMult-2.)*(xMult-3.)*(xMult-4.)*(xMult-5.)); 
  
  fQProduct->Fill(1.,two1n1n*six1n1n1n1n1n1n,xMult*(xMult-1.)*xMult*(xMult-1.)*(xMult-2.)*(xMult-3.)*(xMult-4.)*(xMult-5.));
  fQProduct->Fill(3.,four1n1n1n1n*six1n1n1n1n1n1n,xMult*(xMult-1.)*(xMult-2.)*(xMult-3.)*xMult*(xMult-1.)*(xMult-2.)*(xMult-3.)*(xMult-4.)*(xMult-5.));
 }
 //---------------------------------------------------------------------------------------------------------
 
 
 //--------------------------------------------------------------------------------------------------------- 
 // DIFFERENTIAL FLOW
 
 Double_t xQx  = xQvector1n.X();
 Double_t xQy  = xQvector1n.Y();
 Double_t xQ2x = xQvector2n.X();
 Double_t xQ2y = xQvector2n.Y();

 Double_t qx=0.,qy=0.,q2x=0.,q2y=0.,m=0.;
 
 for(Int_t i=0;i<xMult;i++) //check if nPrim == M
 { 
  fTrack=anEvent->GetTrack(i);
  if(fTrack)
  {
   fReq1n->Fill(fTrack->Pt(),cos(n*(fTrack->Phi())),1.);
   fImq1n->Fill(fTrack->Pt(),sin(n*(fTrack->Phi())),1.);
   fReq2n->Fill(fTrack->Pt(),cos(2.*n*(fTrack->Phi())),1.);
   fImq2n->Fill(fTrack->Pt(),sin(2.*n*(fTrack->Phi())),1.);
  }
 } 
  
 Double_t twoDiff1n1n=0.,twoDiff2n2n=0.,threeDiff2n1n1n=0.,threeDiff1n1n2n=0.,fourDiff1n1n1n1n=0.;
 
 for(Int_t bin=1;bin<(fnBinsPt+1);bin++)//loop over pt-bins 
 { 
  qx = (fReq1n->GetBinContent(bin))*(fReq1n->GetBinEntries(bin));
  qy = (fImq1n->GetBinContent(bin))*(fImq1n->GetBinEntries(bin)); 
  q2x = (fReq2n->GetBinContent(bin))*(fReq2n->GetBinEntries(bin));  
  q2y = (fImq2n->GetBinContent(bin))*(fImq2n->GetBinEntries(bin)); 
  m = fReq1n->GetBinEntries(bin);          
 
  if(m>0&&xMult>1)
  {
   twoDiff1n1n = (qx*xQx+qy*xQy-m)/(m*(xMult-1.));
   f2PerBin1n1n->Fill((bin-1)*0.1,twoDiff1n1n,m*(xMult-1.));//<2'>_{n|n}
   
   twoDiff2n2n = (q2x*xQ2x+q2y*xQ2y-m)/(m*(xMult-1.));
   f2PerBin2n2n->Fill((bin-1)*0.1,twoDiff2n2n,m*(xMult-1.));//<2'>_{2n|2n} 
  }
  
  if(m>0&&xMult>2)
  {
   threeDiff2n1n1n = (q2x*(xQx*xQx-xQy*xQy)+2.*q2y*xQx*xQy-2.*(qx*xQx+qy*xQy)-(q2x*xQ2x+q2y*xQ2y)+2.*m)/(m*(xMult-1.)*(xMult-2.));
   f3PerBin2n1n1n->Fill((bin-1)*0.1,threeDiff2n1n1n,m*(xMult-1.)*(xMult-2.));//Re[<3'>_{2n|n,n}]
   
   threeDiff1n1n2n = (xQ2x*(qx*xQx-qy*xQy)+xQ2y*(qx*xQy+qy*xQx)-2.*(qx*xQx+qy*xQy)-(q2x*xQ2x+q2y*xQ2y)+2.*m)/(m*(xMult-1.)*(xMult-2.));
   f3PerBin1n1n2n->Fill((bin-1)*0.1,threeDiff1n1n2n,m*(xMult-1.)*(xMult-2.));//Re[<3'>_{n,n|2n}]
  }
  
  if(m>0&&xMult>3)
  {
   fourDiff1n1n1n1n = ((xQx*xQx+xQy*xQy)*(qx*xQx+qy*xQy)-(q2x*(xQx*xQx-xQy*xQy)+2.*q2y*xQx*xQy)-(xQ2x*(qx*xQx-qy*xQy)+xQ2y*(qx*xQy+qy*xQx))+(q2x*xQ2x+q2y*xQ2y)-2.*(xMult-3.)*(qx*xQx+qy*xQy)-2.*m*(xQx*xQx+xQy*xQy)+2.*(xQx*qx+xQy*qy)+2.*m*(xMult-3.))/(m*(xMult-1.)*(xMult-2.)*(xMult-3.));
   f4PerBin1n1n1n1n->Fill((bin-1)*0.1,fourDiff1n1n1n1n,m*(xMult-1.)*(xMult-2.)*(xMult-3.));//Re[<4'>_{n,n|n,n}]
  }
   
 } 
  
 fReq1n->Reset();
 fImq1n->Reset();
 fReq2n->Reset();
 fImq2n->Reset();
//---------------------------------------------------------------------------------------------------------






  






 /*
 
 Int_t nSelTracksIntFlow = 0;//cross-checking the selected multiplicity 



 


 //calculating Q-vector of event (needed for errors) in n-th harmonic
 AliFlowVector fQvector;
 fQvector.Set(0.,0.);
 fQvector.SetMult(0);
 fQvector=anEvent->GetQ(n);                             //get the Q vector for this event
   
 nPrim = fQvector.GetMult();        
 nEventNSelTracksIntFlow = fQvector.GetMult();
 nEventNSelTracksIntFlow=nPrim; 
 
 
 */
 
 
 /*
 //calculating Q-vector of event (needed for errors)
 AliFlowVector fQvector;
 fQvector.Set(0.,0.);
 fQvector.SetMult(0);
 fQvector=anEvent->GetQ();                             //get the Q vector for this event
 
 */
 
 //cout<<" multiplicity = "<<fQvector.GetMult()<<endl;
 
 
 //binning is organized as follows:
 //1st bin fill Q_x
 //2nd bin fill Q_y
 //3rd bin fill (Q_x)^2the selected multiplicity
 //4th bin fill (Q_y)^2
 //5th bin fill |Q_n|^2
 //6th bin fill |Q_n|^4
 //7th bin fill |Q_2n|^2
 //8th bin fill Q_2n (Q_n)* (Q_n)*// Remark: * here denotes complex conjugate!!! //Re[Q_n^2 * Q_2n^*] 
 //9th bin fill Q_2n (Q_n)* (Q_n)*// Remark: * here denotes complex conjugate!!! //Im[Q_n^2 * Q_2n^*] 
 //10th bin fill |Q_3n|^2
 //11th bin fill |Q_4n|^2
 //12th bin fill |Q_5n|^2
 //13th bin fill Re<Q_{3n} Q_{2n}^* Q_{n}^*>
 //14th bin fill Re<Q_{3n} Q_{n}^* Q_{n}^* Q_{n}^*>
 //15th bin fill <|Q_{2n}|^2 |Q_{n}|^2>
 //16th bin fill Re<Q_{2n} Q_{n} Q_{n}^* Q_{n}^* Q_{n}^*>
 //17th bin fill |Q_n|^6
 
 /*
 
 fQvectorComponents->Fill(0.,fQvector.X(),1);          //in the 1st bin fill Q_x
 fQvectorComponents->Fill(1.,fQvector.Y(),1);          //in the 2nd bin fill Q_y
 fQvectorComponents->Fill(2.,pow(fQvector.X(),2.),1);  //in the 3rd bin fill (Q_x)^2
 fQvectorComponents->Fill(3.,pow(fQvector.Y(),2.),1);  //in the 4th bin fill (Q_y)^2
 fQvectorComponents->Fill(4.,pow(fQvector.Mod(),2.),1);//in the 5th bin fill |Q|^2
 fQvectorComponents->Fill(5.,pow(fQvector.Mod(),4.),1);//in the 6th bin fill |Q|^4

 //flow vector in 2n-th harmonic
 AliFlowVector fQvector2n;
 fQvector2n.Set(0.,0.);
 fQvector2n.SetMult(0);
 fQvector2n=anEvent->GetQ(2*n);      //get the Q vector in 2n-th harmonic for this event
 
 fQvectorComponents->Fill(6.,pow(fQvector2n.Mod(),2.),1);//in the 7th bin fill |Q_2n|^2

 Double_t
 Q2nQnstarQnstar=pow(fQvector.X(),2.)*fQvector2n.X()+2.*fQvector.X()*fQvector.Y()*fQvector2n.Y()-pow(fQvector.Y(),2.)*fQvector2n.X();
 
 Double_t
 Q2nQnstarQnstarTemp=-2.*fQvector.X()*fQvector2n.X()*fQvector.Y()+fQvector2n.Y()*pow(fQvector.X(),2.)-fQvector2n.Y()*pow(fQvector.Y(),2.);
 
 
 fQvectorComponents->Fill(7.,Q2nQnstarQnstar,1); //in the 8th bin fill Re[Q_n^2 * Q_2n^*]       
 fQvectorComponents->Fill(8.,Q2nQnstarQnstarTemp,1); //in the 9th bin fill Im[Q_n^2 * Q_2n^*]       
 

 //flow vector in 3n-th harmonic
 AliFlowVector fQvector3n;
 fQvector3n.Set(0.,0.);
 fQvector3n.SetMult(0);
 fQvector3n=anEvent->GetQ(3*n);      //get the Q vector in 3n-th harmonic for this event
 fQvectorComponents->Fill(9.,pow(fQvector3n.Mod(),2.),1);//in the 10th bin fill |Q_3n|^2

 //flow vector in 4n-th harmonic
 AliFlowVector fQvector4n;
 fQvector4n.Set(0.,0.);
 fQvector4n.SetMult(0);
 fQvector4n=anEvent->GetQ(4*n);      //get the Q vector in 4n-th harmonic for this event
 fQvectorComponents->Fill(10.,pow(fQvector4n.Mod(),2.),1);//in the 11th bin fill |Q_4n|^2
 
 //flow vector in 5n-th harmonic
 AliFlowVector fQvector5n;
 fQvector5n.Set(0.,0.);
 fQvector5n.SetMult(0);
 fQvector5n=anEvent->GetQ(5*n);      //get the Q vector in 5n-th harmonic for this event
 fQvectorComponents->Fill(11.,pow(fQvector5n.Mod(),2.),1);//in the 12th bin fill |Q_5n|^2
  
 
 
 Double_t
 Q3nQ2nstarQnstar=fQvector3n.X()*fQvector2n.X()*fQvector.X()-fQvector3n.X()*fQvector2n.Y()*fQvector.Y()+fQvector3n.Y()*fQvector2n.X()*fQvector.Y()+fQvector3n.Y()*fQvector2n.Y()*fQvector.X();
 fQvectorComponents->Fill(12.,Q3nQ2nstarQnstar,1); //in the 13th bin fill Re<Q_{3n} Q_{2n}^* Q_{n}^*>
 
 //fDirectCorrelations->Fill(2.,16,1);
 
 
 Double_t
 Q3nQnstarQnstarQnstar=fQvector3n.X()*pow(fQvector.X(),3)-3.*fQvector.X()*fQvector3n.X()*pow(fQvector.Y(),2)+3.*fQvector.Y()*fQvector3n.Y()*pow(fQvector.X(),2)-fQvector3n.Y()*pow(fQvector.Y(),3); 
 fQvectorComponents->Fill(13.,Q3nQnstarQnstarQnstar,1); //in the 14th bin fill Re<Q_{3n} Q_{n}^* Q_{n}^* Q_{n}^*>
 
 
 
 Double_t
 Q2nQnQ2nstarQnstar=pow(fQvector2n.Mod()*fQvector.Mod(),2); 
 fQvectorComponents->Fill(14.,Q2nQnQ2nstarQnstar,1); //in the 15th bin fill <|Q_{2n}|^2 |Q_{n}|^2>
 
 
 
 Double_t
 Q2nQnQnstarQnstarQnstar=(fQvector2n.X()*fQvector.X()-fQvector2n.Y()*fQvector.Y())*(pow(fQvector.X(),3)-3.*fQvector.X()*pow(fQvector.Y(),2))+(fQvector2n.X()*fQvector.Y()+fQvector.X()*fQvector2n.Y())*(3.*fQvector.Y()*pow(fQvector.X(),2)-pow(fQvector.Y(),3));
 fQvectorComponents->Fill(15.,Q2nQnQnstarQnstarQnstar,1); //in the 16th bin fill Re<Q_{2n} Q_{n} Q_{n}^* Q_{n}^* Q_{n}^*>
 
 
 fQvectorComponents->Fill(16.,pow(fQvector.Mod(),6),1); //in the 17th bin fill |Q|^6
 */
 
 /*
 Double_t
 Q2nQnQnstarQnstarQnstar22222=(pow(fQvector.X(),2)+pow(fQvector.Y(),2))*(pow(fQvector.X(),2)*fQvector2n.X()-fQvector2n.X()*pow(fQvector.Y(),2)+2.*fQvector.X()*fQvector.Y()*fQvector2n.Y()); 
 fQvectorComponents->Fill(16.,Q2nQnQnstarQnstarQnstar22222,1); //in the 16th bin fill Re<Q_{2n} Q_{n} Q_{n}^* Q_{n}^* Q_{n}^*>
 */
 













/*





 //-------------------------------------------------------------------------------------------------------------------------------- 
 // multi-particle correlations calculated with nested loops (for integrated flow)
 
 // fDirectCorrelations was declared as TProfile:
 // fDirectCorrelations = new TProfile("fDirectCorrelations","multi-particle correlations with nested loops",80,0,80);
 
 // phik is the azimuth of the kth particle in laboratory frame

 // binning in fDirectCorrelations: 0..40 correlations needed for integrated flow; 40..80 0..40 correlations needed for differential flow 

 // integrated flow
 // 1st bin: <2>_{n|n}
 // 2nd bin: <2>_{2n|2n}
 // 3rd bin: <2>_{3n|3n}
 // 4th bin: <2>_{4n|4n}
 // 6th bin: <3>_{2n|n,n}
 // 7th bin: <3>_{3n|2n,n}
 // 8th bin: <3>_{4n|2n,2n}
 // 9th bin: <3>_{4n|3n,n}
 //11th bin: <4>_{n,n|n,n}
 //12th bin: <4>_{2n,n|2n,n}
 //13th bin: <4>_{3n|n,n,n}
 //14th bin: <4>_{4n|2n,n,n}
 //15th bin: <4>_{3n,n|2n,2n}
 //16th bin: <5>_{2n,n|n,n,n}
 //17th bin: <5>_{2n,2n|2n,n,n}
 //18th bin: <5>_{3n,n|2n,n,n}
 //19th bin: <5>_{4n|n,n,n,n}
 //21th bin: <6>_{n,n,n|n,n,n}
 
 
 //31th bin: <4>_{2n,2n|2n,2n} //to be improved
 //32nd bin: <4>_{3n,n|3n,n}   //to be improved
 
 
 //differential flow
 //41st bin: <2'>_{n|n}
 //42nd bin: <2'>_{2n|2n}
 //46th bin: <3'>_{2n|n,n}
 //47th bin: <3'>_{n,n|2n}
 //51st bin: <4'>_{n,n|n,n}

 Double_t phi1=0.,phi2=0.;
 Double_t phi3=0.,phi4=0.;
 Double_t phi5=0.,phi6=0.;

 //<2>_{kn|kn} (k=1,2,3,4)
 for(Int_t i=0;i<xMult;i++)
 {
  fTrack=anEvent->GetTrack(i);
  phi1=fTrack->Phi();
  for(Int_t j=0;j<xMult;j++)
  {
   if(j==i)continue;
   fTrack=anEvent->GetTrack(j);
   phi2=fTrack->Phi();
   fDirectCorrelations->Fill(0.,cos(n*(phi1-phi2)),1);    //<2>_{n|n}
   fDirectCorrelations->Fill(1.,cos(2.*n*(phi1-phi2)),1); //<2>_{2n|2n}
   fDirectCorrelations->Fill(2.,cos(3.*n*(phi1-phi2)),1); //<2>_{3n|3n}
   fDirectCorrelations->Fill(3.,cos(4.*n*(phi1-phi2)),1); //<2>_{4n|4n} 
  }
 }  
     
 //<3>_{2n|n,n}, <3>_{3n|2n,n}, <3>_{4n|2n,2n} and <3>_{4n|3n,n}
 for(Int_t i=0;i<xMult;i++)
 {
  fTrack=anEvent->GetTrack(i);
  phi1=fTrack->Phi();
  for(Int_t j=0;j<xMult;j++)
  {
   if(j==i)continue;
   fTrack=anEvent->GetTrack(j);
   phi2=fTrack->Phi();
   for(Int_t k=0;k<xMult;k++)
   {
    if(k==i||k==j)continue;
    fTrack=anEvent->GetTrack(k);
    phi3=fTrack->Phi();
    fDirectCorrelations->Fill(5.,cos(2*n*phi1-n*(phi2+phi3)),1);        //<3>_{2n|n,n}
    fDirectCorrelations->Fill(6.,cos(3.*n*phi1-2.*n*phi2-n*phi3),1);    //<3>_{3n|2n,n}
    fDirectCorrelations->Fill(7.,cos(4.*n*phi1-2.*n*phi2-2.*n*phi3),1); //<3>_{4n|2n,2n}
    fDirectCorrelations->Fill(8.,cos(4.*n*phi1-3.*n*phi2-n*phi3),1);    //<3>_{4n|3n,n}
   }
  }
 }
  
 //<4>_{n,n|n,n}, <4>_{2n,n|2n,n}, <4>_{3n|n,n,n}, <4>_{4n|2n,n,n}, <4>_{2n,2n|2n,2n} and <4>_{3n,n|3n,n}
 for(Int_t i=0;i<xMult;i++)
 {
  fTrack=anEvent->GetTrack(i);
  phi1=fTrack->Phi();
  for(Int_t j=0;j<xMult;j++)
  {
   if(j==i)continue;
   fTrack=anEvent->GetTrack(j);
   phi2=fTrack->Phi();
   for(Int_t k=0;k<xMult;k++)
   {
    if(k==i||k==j)continue;
    fTrack=anEvent->GetTrack(k);
    phi3=fTrack->Phi();
    for(Int_t l=0;l<xMult;l++)
    {
     if(l==i||l==j||l==k)continue;
     fTrack=anEvent->GetTrack(l);
     phi4=fTrack->Phi();
     fDirectCorrelations->Fill(10.,cos(n*phi1+n*phi2-n*phi3-n*phi4),1);          //<4>_{n,n|n,n}
     fDirectCorrelations->Fill(11.,cos(2.*n*phi1+n*phi2-2.*n*phi3-n*phi4),1);    //<4>_{2n,n|2n,n}
     fDirectCorrelations->Fill(12.,cos(3.*n*phi1-n*phi2-n*phi3-n*phi4),1);       //<4>_{3n|n,n,n}
     fDirectCorrelations->Fill(13.,cos(4.*n*phi1-2.*n*phi2-n*phi3-n*phi4),1);    //<4>_{4n|2n,n,n}
     fDirectCorrelations->Fill(14.,cos(3.*n*phi1+n*phi2-2.*n*phi3-2.*n*phi4),1); //<4>_{3n,n|2n,2n}
     fDirectCorrelations->Fill(30.,cos(2.*n*phi1+2*n*phi2-2.*n*phi3-2.*n*phi4),1); //<4>_{2n,2n|2n,2n}//to be improved
     fDirectCorrelations->Fill(31.,cos(3.*n*phi1+n*phi2-3.*n*phi3-n*phi4),1);      //<4>_{3n,n|3n,n}//to be improved
    }  
   }
  }
 }
 
 
 
 
 

 
 
 
 
 
 
 
 
 
 //<5>_{2n,n,n,n,n}
 for(Int_t i=0;i<xMult;i++)
 {
  cout<<"i = "<<i<<endl;
  fTrack=anEvent->GetTrack(i);
  phi1=fTrack->Phi();
  for(Int_t j=0;j<xMult;j++)
  {
   if(j==i)continue;
   fTrack=anEvent->GetTrack(j);
   phi2=fTrack->Phi();
   for(Int_t k=0;k<xMult;k++)
   {
    if(k==i||k==j)continue;
    fTrack=anEvent->GetTrack(k);
    phi3=fTrack->Phi();
    for(Int_t l=0;l<xMult;l++)
    {
     if(l==i||l==j||l==k)continue;
     fTrack=anEvent->GetTrack(l);
     phi4=fTrack->Phi();
     for(Int_t m=0;m<xMult;m++)
     {
      if(m==i||m==j||m==k||m==l)continue;
      fTrack=anEvent->GetTrack(m);
      phi5=fTrack->Phi();
      fDirectCorrelations->Fill(15.,cos(2.*n*phi1+n*phi2-n*phi3-n*phi4-n*phi5),1);      //<5>_{2n,n|n,n,n}
      fDirectCorrelations->Fill(16.,cos(2.*n*phi1+2.*n*phi2-2.*n*phi3-n*phi4-n*phi5),1);//<5>_{2n,2n|2n,n,n}
      fDirectCorrelations->Fill(17.,cos(3.*n*phi1+n*phi2-2.*n*phi3-n*phi4-n*phi5),1);   //<5>_{3n,n|2n,n,n}
      fDirectCorrelations->Fill(18.,cos(4.*n*phi1-n*phi2-n*phi3-n*phi4-n*phi5),1);      //<5>_{4n|n,n,n,n}
     }
    }  
   }
  }
 }
 
 




 

 
 //<6>_{n,n,n,n,n,n}
 for(Int_t i=0;i<xMult;i++)
 {
  //cout<<"i = "<<i<<endl;
  fTrack=anEvent->GetTrack(i);
  phi1=fTrack->Phi();
  for(Int_t j=0;j<xMult;j++)
  {
   if(j==i)continue;
   fTrack=anEvent->GetTrack(j);
   phi2=fTrack->Phi();
   for(Int_t k=0;k<xMult;k++)
   {
    if(k==i||k==j)continue;
    fTrack=anEvent->GetTrack(k);
    phi3=fTrack->Phi();
    for(Int_t l=0;l<xMult;l++)
    {
     if(l==i||l==j||l==k)continue;
     fTrack=anEvent->GetTrack(l);
     phi4=fTrack->Phi();
     for(Int_t m=0;m<xMult;m++)
     {
      if(m==i||m==j||m==k||m==l)continue;
      fTrack=anEvent->GetTrack(m);
      phi5=fTrack->Phi();
      for(Int_t nn=0;nn<xMult;nn++)
      {
       if(nn==i||nn==j||nn==k||nn==l||nn==m)continue;
       fTrack=anEvent->GetTrack(nn);
       phi6=fTrack->Phi(); 
       fDirectCorrelations->Fill(20.,cos(n*phi1+n*phi2+n*phi3-n*phi4-n*phi5-n*phi6),1); //<6>_{n,n,n,n,n,n}
      } 
     }
    }  
   }
  }
 }
 
 //-------------------------------------------------------------------------------------------------------------------------------- 



















 //-------------------------------------------------------------------------------------------------------------------------------- 
 // multi-particle correlations calculated with nested loops (for differential flow)
 
 //differential flow (see above how the binning in fDirectCorrelations is organized)
 //41st bin: <2'>_{n|n}
 //42nd bin: <2'>_{2n|2n}
 //46th bin: <3'>_{2n|n,n}
 //47th bin: <3'>_{n,n|2n}
 //51st bin: <4'>_{n,n|n,n}
 
 //<2'>_{n|n}
 for(Int_t i=0;i<xMult;i++)
 {
  fTrack=anEvent->GetTrack(i);
  if(fTrack->Pt()>=0.5&&fTrack->Pt()<0.6)
  {
   phi1=fTrack->Phi();
   for(Int_t j=0;j<xMult;j++)
   {
    if(j==i)continue;
    fTrack=anEvent->GetTrack(j);
    phi2=fTrack->Phi(); 
    fDirectCorrelations->Fill(40.,cos(1.*n*(phi1-phi2)),1); //<2'>_{n,n}
    fDirectCorrelations->Fill(41.,cos(2.*n*(phi1-phi2)),1); //<2'>_{2n,2n}  
   }
  }
 }  

 //<3'>_{2n|n,n}
 for(Int_t i=0;i<xMult;i++)
 {
  fTrack=anEvent->GetTrack(i);
  if(fTrack->Pt()>=0.5&&fTrack->Pt()<0.6)
  {
   phi1=fTrack->Phi();
   for(Int_t j=0;j<xMult;j++)
   {
    if(j==i)continue;
    fTrack=anEvent->GetTrack(j);
    phi2=fTrack->Phi();
    for(Int_t k=0;k<xMult;k++)
    {
     if(k==i||k==j)continue;
     fTrack=anEvent->GetTrack(k);
     phi3=fTrack->Phi();           
     fDirectCorrelations->Fill(45.,cos(n*(2.*phi1-phi2-phi3)),1); //<3'>_{2n|n,n}
     fDirectCorrelations->Fill(46.,cos(n*(phi1+phi2-2.*phi3)),1); //<3'>_{n,n|2n}    
    }
   }  
  }  
 }
  
   
  
  
 //<4'>_{n,n|n,n}
 for(Int_t i=0;i<xMult;i++)
 {
  fTrack=anEvent->GetTrack(i);
  if(fTrack->Pt()>=0.5&&fTrack->Pt()<0.6)
  {
   phi1=fTrack->Phi();
   for(Int_t j=0;j<xMult;j++)
   {
    if(j==i)continue;
    fTrack=anEvent->GetTrack(j);
    phi2=fTrack->Phi();
    for(Int_t k=0;k<xMult;k++)
    {
     if(k==i||k==j)continue;
     fTrack=anEvent->GetTrack(k);
     phi3=fTrack->Phi();
     for(Int_t l=0;l<xMult;l++)
     {
      if(l==i||l==j||l==k)continue;
      fTrack=anEvent->GetTrack(l);
      phi4=fTrack->Phi();
      fDirectCorrelations->Fill(50.,cos(n*(phi1+phi2-phi3-phi4)),1); //<4'>_{n,n|n,n}   
     } 
    }
   }  
  }  
 }
 //--------------------------------------------------------------------------------------------------------------------------------  


 
*/











//}//end of if


}//end of Make()

//================================================================================================================

void AliFlowAnalysisWithQCumulants::Finish()
{
 //calculate the final results
 AliQCumulantsFunctions finalResults(fIntFlowResultsQC,fDiffFlowResults2ndOrderQC,fDiffFlowResults4thOrderQC,fCovariances,fAvMultIntFlowQC,fQvectorComponents,fQCorrelations, fQProduct,fDirectCorrelations, f2PerBin1n1n,f2PerBin2n2n,f3PerBin2n1n1n,f3PerBin1n1n2n,f4PerBin1n1n1n1n,fCommonHistsResults2nd, fCommonHistsResults4th,fCommonHistsResults6th,fCommonHistsResults8th);
         
 finalResults.Calculate();  
}

//================================================================================================================

void AliFlowAnalysisWithQCumulants::WriteHistograms(TString* outputFileName)
{
 //store the final results in output .root file
 TFile *output = new TFile(outputFileName->Data(),"RECREATE");
 fHistList->Write(); 
 delete output;
}

//================================================================================================================

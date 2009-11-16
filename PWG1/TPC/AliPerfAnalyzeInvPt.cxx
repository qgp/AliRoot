//------------------------------------------------------------------------------
// Implementation of AliPerfAnalyzeInvPt class. It analyzes the output of
// AliPerformancePtCalib.cxx  and AliPerformancePtCalibMC.cxx:
// Projection of 1/pt vs theta and vs phi resp. histoprams will be fitted with either
// polynomial or gaussian fit function to extract minimum position of 1/pt.
// Fit options and theta, phi bins can be set by user.
// Attention: use the Set* functions of AliPerformancePtCalib.h and AliPerformancePtCalibMC.h
// when running AliPerformancePtCalib*::Analyse()
// The result of the analysis (histograms/graphs) are stored in the folder which is
// a data member of AliPerformancePtCalib*.
//
// Author: S.Schuchmann 11/13/2009 
//------------------------------------------------------------------------------

/*
 
// after running comparison task, read the file, and get component
gROOT->LoadMacro("$ALICE_ROOT/PWG1/Macros/LoadMyLibs.C");
LoadMyLibs();

TFile f("Output.root");
AliPerformancePtCalib * compObj = (AliPerformancePtCalib*)coutput->FindObject("AliPerformancePtCalib");
 
// analyse comparison data
compObj->Analyse();

// the output histograms/graphs will be stored in the folder "folderRes" 
compObj->GetAnalysisFolder()->ls("*");

// user can save whole comparison object (or only folder with anlysed histograms) 
// in the seperate output file (e.g.)
TFile fout("Analysed_InvPt.root","recreate");
compObj->Write(); // compObj->GetAnalysisFolder()->Write();
fout.Close();

*/

#include "TNamed.h"
#include "TF1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"
#include "TH1D.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TObjArray.h"

#include "AliPerfAnalyzeInvPt.h"

ClassImp(AliPerfAnalyzeInvPt)


// fit functions
//____________________________________________________________________________________________________________________________________________
   Double_t AliPerfAnalyzeInvPt::Polynomial(Double_t *x, const Double_t *par)
{
   // fit function for fitting of 1/pt with a polynomial of 4th order, rejecting points between  +/-par[4]


   if (x[0] > -par[4] && x[0] < par[4]) {
      TF1::RejectPoint();
      return 0;
   }
   return  par[2]+par[0]*pow((x[0]-par[1]),2)+par[3]*pow((x[0]-par[1]),4);
  
}
//____________________________________________________________________________________________________________________________________________
Double_t AliPerfAnalyzeInvPt::PolynomialRejP(Double_t *x, const Double_t *par)
{
   // fit function for fitting of 1/pt with a polynomial of 4th order to improve result (fit range and rejection zone is adjusted to first guess of minimum position)
 
   Double_t pos  = par[5];
   Double_t neg = - par[5];
   pos += par[4];
   neg += par[4];
  
   if (x[0] > neg && x[0] < pos) {
      TF1::RejectPoint();
      return 0;
   }
   return   par[2]+par[0]*pow((x[0]-par[1]),2)+par[3]*pow((x[0]-par[1]),4);
}
//____________________________________________________________________________________________________________________________________________
Double_t AliPerfAnalyzeInvPt::InvGauss(Double_t *x, const Double_t *par)
{
   // fit function for fitting of 1/pt with gaussian, rejecting points between  +/-par[4]
   if (x[0] > -par[6] && x[0] < par[6]) {
      TF1::RejectPoint();
      return 0;
   }

   return par[3]+par[0]*(TMath::Exp(-0.5*(TMath::Power((x[0]-par[1])/par[2], 2.0)))+par[4]*pow((x[0]-par[1]),2)+par[5]*pow((x[0]-par[1]),4)) ;
}
//____________________________________________________________________________________________________________________________________________
Double_t AliPerfAnalyzeInvPt::InvGaussRejP(Double_t *x, const Double_t *par)
{
   // fit function for fitting of 1/pt with gaussian to improve result (fit range and rejection zone is adjusted to first guess of minimum position)
   Double_t pos  = par[7];//0.12;
   Double_t neg = - par[7];//0.12;
   pos += par[6];
   neg += par[6];
  
   if (x[0] > neg && x[0] < pos) {
      TF1::RejectPoint();
      return 0;
   }


   return par[3]+par[0]*TMath::Exp(-0.5*(TMath::Power((x[0]-par[1])/par[2], 2.0)))+par[4]*pow((x[0]-par[1]),2)+par[5]*pow((x[0]-par[1]),4) ;
}


//_____________________________________________________________________________________________________________________________________________
AliPerfAnalyzeInvPt::AliPerfAnalyzeInvPt():
   TNamed("AliPerfAnalyzeInvPt","AliPerfAnalyzeInvPt"),
   fNThetaBins(0), 
   fNPhiBins(0),
   fRange(0),
   fExclRange(0),
   fFitGaus(0) ,
   fHistH2InvPtTheta(0),
   fHistH2InvPtPhi(0), 
   fGrMinPosTheta(0),
   fGrMinPosPhi(0),
   fFitMinPos(0),
   fFitMinPosRejP(0),
   fFitInvGauss(0),
   fFitInvGaussRejP(0)
{
   // Default constructor
  
   fFitGaus = kFALSE;
   fNThetaBins = 0;
   fNPhiBins = 0;
   fRange = 0;
   fExclRange = 0;
   fFitGaus = 0;
   
   for(Int_t i=0;i<100;i++){
      
      fHistFitTheta[i] = NULL;
      fHistFitPhi[i] = NULL;
   }
 
   
}
//_____________________________________________________________________________________________________________________________________________
AliPerfAnalyzeInvPt::AliPerfAnalyzeInvPt(Char_t* name="AliAnalyzeInvPt",Char_t* title="AliAnalyzeInvPt"):
   TNamed(name, title),
   fNThetaBins(0), 
   fNPhiBins(0),
   fRange(0),
   fExclRange(0),
   fFitGaus(0) ,
   fHistH2InvPtTheta(0),
   fHistH2InvPtPhi(0), 
   fGrMinPosTheta(0),
   fGrMinPosPhi(0),
   fFitMinPos(0),
   fFitMinPosRejP(0),
   fFitInvGauss(0),
   fFitInvGaussRejP(0)
{
   
   fFitGaus = kFALSE;
   fNThetaBins = 0;
   fNPhiBins =0;
   fRange = 0;
   fExclRange = 0;
   fFitGaus = 0;

   for(Int_t i=0;i<100;i++){
    
      fHistFitTheta[i] = NULL;
      fHistFitPhi[i] = NULL;
   }
  
}


//______________________________________________________________________________________________________________________________________
void AliPerfAnalyzeInvPt::InitGraphs(Double_t *binsXTheta,Double_t *fitParamTheta,Double_t *errFitParamTheta,Double_t *binsXPhi,Double_t *fitParamPhi,Double_t *errFitParamPhi){

   // initialize graphs for storing fit parameter
  
   fGrMinPosTheta = new TGraphErrors(fNThetaBins,binsXTheta,fitParamTheta,0, errFitParamTheta);  
   fGrMinPosTheta->SetMarkerStyle(20);
   fGrMinPosTheta->SetMarkerColor(2);
   fGrMinPosTheta->SetLineColor(2);
   fGrMinPosTheta->GetYaxis()->SetTitle("min pos (Gev/c)^{-1}");
   fGrMinPosTheta->GetXaxis()->SetTitle("#theta bin no.");
   fGrMinPosTheta->GetYaxis()->SetTitleOffset(1.2);   
   fGrMinPosTheta->SetTitle("#theta bins ");

   fGrMinPosPhi = new TGraphErrors(fNPhiBins,binsXPhi,fitParamPhi,0,errFitParamPhi);  
   fGrMinPosPhi->SetMarkerStyle(20);
   fGrMinPosPhi->SetMarkerColor(4);
   fGrMinPosPhi->SetLineColor(4);
   fGrMinPosPhi->GetYaxis()->SetTitle("min pos (Gev/c)^{-1}");
   fGrMinPosPhi->GetXaxis()->SetTitle("#phi bin no.");
   fGrMinPosPhi->GetYaxis()->SetTitleOffset(1.2);   
   fGrMinPosPhi->SetTitle("#phi bins ");
}

//______________________________________________________________________________________________________________________________________

void AliPerfAnalyzeInvPt::InitFitFcn(){
   // fit functions
   fFitMinPos = new TF1("fFitMinPos", Polynomial,-4.0,4.0,5);
   fFitMinPos->SetLineColor(4);
   fFitMinPos->SetLineWidth(1);
   fFitMinPos->SetParameter(0,1.0);
   fFitMinPos->SetParameter(1,0.0);
   fFitMinPos->SetParameter(2,0.0);

   fFitMinPosRejP = new TF1("fFitMinPosRejP",PolynomialRejP,-4.0,4.0,6);
   fFitMinPosRejP->SetLineColor(2);
   fFitMinPosRejP->SetLineWidth(1);
   fFitMinPosRejP->SetParameter(0,1.0);
   fFitMinPosRejP->SetParameter(1,0.0);
   fFitMinPosRejP->SetParameter(2,0.0);
 
  
   fFitInvGauss = new TF1("fFitInvGauss", InvGauss,-4.0,4.0,6);
   fFitInvGauss->SetLineColor(4);
   fFitInvGauss->SetLineWidth(1);
   fFitInvGauss->SetParameter(2,1.0);
  
   fFitInvGaussRejP = new TF1("fFitInvGaussRejP", InvGaussRejP,-4.0,4.0,7);
   fFitInvGaussRejP->SetLineColor(2);
   fFitInvGaussRejP->SetLineWidth(1);
   fFitInvGaussRejP->SetParameter(2,1.0);
 
}
//______________________________________________________________________________________________________________________________________
void AliPerfAnalyzeInvPt::StartAnalysis(const TH2F *histThetaInvPt, const TH2F *histPhiInvPt, TObjArray* aFolderObj){
   //start analysis: fitting 1/pt spectra

  
   
   if(!histThetaInvPt) {
      Printf("warning: no 1/pt histogram to analyse in theta bins!");
   }
   if(!histPhiInvPt) {
      Printf("warning: no 1/pt histogram to analyse in phit bins!");
   }

   Double_t thetaBins[9] = {0.77,0.97,1.17,1.37,1.57,1.77,1.97,2.17,2.37};                 // theta bins
   Double_t phiBins[13] = {0.0,0.5,1.0,1.5,2.0,2.5,3.0,3.5,4.0,4.5,5.0,5.5,6.5};           // phi bins
  
   Int_t nThetaBins = 9;
   Int_t nPhiBins = 13;
   Double_t range = 1.0;        // fit range
   Double_t  exclRange  = 0.13; // range of point rejection in fit
   Bool_t fitGaus=kFALSE;       // fit with Gaussian or with polynomial (kFALSE)
   
   if(!fNThetaBins){
      fNThetaBins = nThetaBins;
      for(Int_t k = 0;k<fNThetaBins;k++){
	 fThetaBins[k] = thetaBins[k];
      }
      Printf("warning: for theta bins no user intput! bins are set to default values.");
   }

   if(!fNPhiBins){
      fNPhiBins = nPhiBins;
      for(Int_t k = 0;k<fNPhiBins;k++){
	 fPhiBins[k] = phiBins[k];
      }
    
      Printf("warning: for phi bins no user intput! bins are set to default values.");
    
   }

   if(!fRange){
      fRange = range;
      fExclRange = exclRange;
      fFitGaus = fitGaus;
      Printf("warning: no fit range is set. default fitting conditions are used.");
   }

      
   //param arrays
   Double_t fitParamTheta[100],fitParamPhi[100];
   Double_t errFitParamTheta[100], errFitParamPhi[100];
   Double_t binsXTheta[100],binsXPhi[100];

   fHistH2InvPtTheta  = (TH2F*)histThetaInvPt->Clone("invPtVsTheta");
   fHistH2InvPtPhi    = (TH2F*)histPhiInvPt->Clone("invPtVsPhi");

   InitFitFcn();
	 
   TCanvas *theCan =new TCanvas("theCan","invPt theta bins",1200,900);
   theCan->Divide((fNThetaBins+2)/2,2);
   TCanvas *phiCan =new TCanvas("phiCan","invPt phi bins",1200,900);
   phiCan->Divide((fNPhiBins+2)/2,2);
   Int_t countPad = 1;
	 
   // analyse 1/pt in bins of theta 
   for(Int_t i=0;i<fNThetaBins;i++){
      TString name = "fit_theta_";
      name +=i;
      Int_t firstBin= fHistH2InvPtTheta->GetYaxis()->FindBin(fThetaBins[i]);
      if(i>0) firstBin +=1;
      Int_t lastBin = fHistH2InvPtTheta->GetYaxis()->FindBin(fThetaBins[i+1]);
      if( i == fNThetaBins-1) {
	 firstBin= fHistH2InvPtTheta->GetYaxis()->FindBin(fThetaBins[0]);
	 lastBin = fHistH2InvPtTheta->GetYaxis()->FindBin(fThetaBins[i]);
      }
    
      fHistH2InvPtTheta->SetName(name.Data());
      fHistFitTheta[i] =  (TH1F*)fHistH2InvPtTheta->ProjectionX("_px",firstBin,lastBin,"e");
      
      Char_t titleTheta[50];
      if(TMath::Abs(i-(fNThetaBins-1))<0.5) sprintf(titleTheta,"1/pt (GeV/c) integrated over #theta");
      else  sprintf(titleTheta,"1/pt (GeV/c) for #theta range: %1.3f - %1.3f",fThetaBins[i],fThetaBins[i+1]);
      
      fHistFitTheta[i]->SetTitle(titleTheta);
   
      Double_t invPtMinPos  = 0;
      Double_t invPtMinPosErr = 0;
      Double_t invPtMinPosImpr  = 0;
      Double_t invPtMinPosErrImpr = 0;
   

      //start fitting
      if(!fFitGaus){
	 Printf("making polynomial fit in 1/pt in theta bins");
	 theCan->cd(countPad);
	 MakeFit(fHistFitTheta[i],fFitMinPos, invPtMinPos,invPtMinPosErr, fExclRange,fRange);
	 MakeFitBetter(fHistFitTheta[i],fFitMinPosRejP, invPtMinPosImpr, invPtMinPosErrImpr, invPtMinPos,fExclRange,fRange);

	
	 fHistFitTheta[i]->DrawCopy();
	 fFitMinPos->DrawCopy("L,same");
	 fFitMinPosRejP->DrawCopy("L,same");
      }
      else{
	 Printf("making gauss fit in 1/pt in theta bins");
	 theCan->cd(countPad);
	 MakeFitInvGauss(fHistFitTheta[i],fFitInvGauss, invPtMinPos, invPtMinPosErr,fExclRange,fRange);
	 MakeFitInvGaussBetter(fHistFitTheta[i],fFitInvGaussRejP, invPtMinPosImpr, invPtMinPosErrImpr,invPtMinPos,fExclRange,fRange);
      
	 fHistFitTheta[i]->DrawCopy();
	 fFitInvGauss->DrawCopy("L,same");
	 fFitInvGaussRejP->DrawCopy("L,same");
      }
    
      aFolderObj->Add(fHistFitTheta[i]);
    
      fitParamTheta[i] = invPtMinPosImpr;
      errFitParamTheta[i] = invPtMinPosErrImpr;
    
      binsXTheta[i] = i+1.0;
      countPad++;
   }
      
      
   countPad = 1;

   
   // analyse 1/pt in bins of phi 
  
   for(Int_t i=0;i<fNPhiBins;i++){
      TString name = "fit_phi_";
      name +=i;
    
      fHistH2InvPtPhi->SetName(name.Data());
      Int_t  firstBin = fHistH2InvPtPhi->GetYaxis()->FindBin(fPhiBins[i]);
      if(i>0) firstBin +=1;
      Int_t   lastBin =  fHistH2InvPtPhi->GetYaxis()->FindBin(fPhiBins[i+1]);
      if(TMath::Abs(i-(fNPhiBins-1))<0.5){
	 firstBin = fHistH2InvPtPhi->GetYaxis()->FindBin(fPhiBins[0]);
	 lastBin =  fHistH2InvPtPhi->GetYaxis()->FindBin(fPhiBins[i]);
      }
      fHistFitPhi[i] =  (TH1F*) fHistH2InvPtPhi->ProjectionX("_px",firstBin,lastBin,"e");
      
      Char_t titlePhi[50];
      if(TMath::Abs(i-(fNPhiBins-1))<0.5) sprintf(titlePhi,"1/pt (GeV/c) integrated over #phi");
      else  sprintf(titlePhi,"1/pt (GeV/c) for #phi range: %1.3f - %1.3f",fPhiBins[i],fPhiBins[i+1]);
     
      fHistFitPhi[i]->SetTitle(titlePhi);
  
      Double_t invPtMinPos  = 0;
      Double_t invPtMinPosErr = 0;
      Double_t invPtMinPosImpr  = 0;
      Double_t invPtMinPosErrImpr = 0;
    
      if(!fFitGaus){
	 Printf("making polynomial fit in 1/pt in phi bins");
	 phiCan->cd(countPad);
	 MakeFit(fHistFitPhi[i],fFitMinPos, invPtMinPos, invPtMinPosErr,fExclRange,fRange);
	 MakeFitBetter(fHistFitPhi[i],fFitMinPosRejP, invPtMinPosImpr, invPtMinPosErrImpr,invPtMinPos,fExclRange,fRange);
	 
	 fHistFitPhi[i]->DrawCopy();
	 fFitMinPos->DrawCopy("L,same");
	 fFitMinPosRejP->DrawCopy("L,same");

      }
      else {
	 Printf("making gauss fit in 1/pt in phi bins");
	 phiCan->cd(countPad);
	 MakeFitInvGauss(fHistFitPhi[i],fFitInvGauss, invPtMinPos, invPtMinPosErr, exclRange,fRange);
	 MakeFitInvGaussBetter(fHistFitPhi[i],fFitInvGaussRejP, invPtMinPosImpr, invPtMinPosErrImpr, invPtMinPos, fExclRange,fRange);
	 
	 fHistFitPhi[i]->DrawCopy();
	 fFitInvGauss->DrawCopy("L,same");
	 fFitInvGaussRejP->DrawCopy("L,same");
      }
    
      aFolderObj->Add(fHistFitPhi[i]);
    
      fitParamPhi[i] = invPtMinPosImpr;
      errFitParamPhi[i] = invPtMinPosErrImpr;
    
      binsXPhi[i] = i+1.0;
      countPad++;
   }
      
   InitGraphs(binsXTheta,fitParamTheta,errFitParamTheta,binsXPhi,fitParamPhi,errFitParamPhi);

   //plot fit values = minimum positions of charge/pt
   TCanvas *canFitVal = new TCanvas("canFitVal","min pos histos",800,400);
   canFitVal->Divide(2,1);

   canFitVal->cd(1);
   fGrMinPosTheta->Draw("ALP");
   canFitVal->cd(2);
   fGrMinPosPhi->Draw("ALP");

   Printf("AliPerfAnalyzeInvPt: NOTE: last bin is always fit result  of integral over all angle ranges which have been set by user!");

   //add objects to folder
   aFolderObj->Add(fGrMinPosTheta);
   aFolderObj->Add(fGrMinPosPhi);
   aFolderObj->Add(fHistH2InvPtTheta);
   aFolderObj->Add(fHistH2InvPtPhi);
  
  
}


//____________________________________________________________________________________________________________________________________________

void AliPerfAnalyzeInvPt::MakeFit(TH1F *hproy, TF1 * fitpb, Double_t &mean, Double_t &errMean, Double_t &excl,Double_t &range)
{
   // fit 1/pt and extract minimum position
   
   fitpb->SetRange(-range,range);
   fitpb->SetParLimits(1,-0.05,0.05);
   fitpb->SetParameter(0,1.0);
   fitpb->FixParameter(4,excl);
   fitpb->FixParameter(2,0.0);
    
   hproy->Fit(fitpb,"RM");
   mean = fitpb->GetParameter(1);
   errMean = fitpb->GetParError(1);
   if(!mean)  errMean = 0.0;
}
//____________________________________________________________________________________________________________________________________________
void AliPerfAnalyzeInvPt::MakeFitBetter(TH1F *hproy, TF1 * fitpb2, Double_t &mean, Double_t &errMean, Double_t &f, Double_t &excl, Double_t &range)
{
   // adjust fit range to minimum position of AliPerfAnalyzeInvPt::MakeFit and fit 1/pt and extract new minimum position
   
   fitpb2->FixParameter(5,excl);
   fitpb2->FixParameter(4,f);
   // fitpb2->FixParameter(2,0.0);
   fitpb2->SetRange(-range+f,range+f);
   fitpb2->SetParLimits(1,-0.05,0.05);
   fitpb2->SetParameter(0,1.0);
   hproy->Fit(fitpb2,"RM");
   mean = fitpb2->GetParameter(1);
   errMean = fitpb2->GetParError(1);
   if(!mean)   errMean = 0.0;

}
//____________________________________________________________________________________________________________________________________________
void AliPerfAnalyzeInvPt::MakeFitInvGauss(TH1F *hproy, TF1 * fitpb, Double_t &mean, Double_t &errMean, Double_t &excl,Double_t &range)
{
   // fit 1/pt and extract minimum position
   
   fitpb->FixParameter(6,excl);
   fitpb->SetRange(-range,range);
   fitpb->SetParameter(0,-1.0);
   fitpb->SetParameter(2,1.0);
   fitpb->SetParameter(3,25000.0);
   fitpb->SetParameter(4,-1.0);
   fitpb->SetParLimits(1,-0.02,0.02);
   hproy->Fit(fitpb,"RM");
   mean = fitpb->GetParameter(1);
   errMean = fitpb->GetParError(1);
   if(!mean)   errMean = 0.0;
  
}
//____________________________________________________________________________________________________________________________________________
void AliPerfAnalyzeInvPt::MakeFitInvGaussBetter(TH1F *hproy, TF1 * fitpb2, Double_t &mean, Double_t &errMean, Double_t &f, Double_t &excl, Double_t &range)
{
   // adjust fit range to minimum position of AliPerfAnalyzeInvPt::MakeFitInvGauss and fit 1/pt and extract new minimum position
   
   fitpb2->FixParameter(7,excl);
   fitpb2->FixParameter(6,f);
   fitpb2->SetRange(-range+f,range+f);
   fitpb2->SetParameter(0,-1.0);
   fitpb2->SetParameter(2,1.0);
   fitpb2->SetParameter(3,25000.0);
   fitpb2->SetParameter(4,-1.0);
   fitpb2->SetParLimits(1,-0.02,0.02);
   hproy->Fit(fitpb2,"RM");
   mean = fitpb2->GetParameter(1);
   errMean = fitpb2->GetParError(1);
   if(!mean)   errMean = 0.0;
   
}


//____________________________________________________________________________________________________________________________________________

// set variables for analysis
void AliPerfAnalyzeInvPt::SetProjBinsPhi(const Double_t *phiBinArray,const Int_t nphBins){

   // set theta bins for Analyse()
   //set phi bins as array and set number of this array which is equal to number of bins analysed
   //the last analysed bin will always be the projection from first to last bin in the array
   if(nphBins){
      fNPhiBins = nphBins;
  
      for(Int_t k = 0;k<fNPhiBins;k++){
	 fPhiBins[k] = phiBinArray[k];
      }
      Printf("AliPerfAnalyzeInvPt::SetProjBinsPhi:number of bins in phi set to %i",fNPhiBins);
   }
   else Printf("AliPerfAnalyzeInvPt::SetProjBinsPhi: number of bins in theta NOT set. Default values are used.");
}
//____________________________________________________________________________________________________________________________________________
void AliPerfAnalyzeInvPt::SetProjBinsTheta(const Double_t *thetaBinArray, const Int_t nthBins){
   // set theta bins for Analyse()
   //set theta bins as array and set number of this array which is equal to number of bins analysed
   //the last analysed bin will always be the projection from first to last bin in the array
   if(nthBins){
      fNThetaBins = nthBins;
      for(Int_t k = 0;k<fNThetaBins;k++){
	 fThetaBins[k] = thetaBinArray[k];
      }
      Printf("AliPerfAnalyzeInvPt::SetProjBinsTheta: number of bins in theta set to %i",fNThetaBins);
   }
   else Printf("AliPerfAnalyzeInvPt::SetProjBinsTheta: number of bins in theta NOT set. Default values are used.");
}
//____________________________________________________________________________________________________________________________________________
void AliPerfAnalyzeInvPt::SetMakeFitOption(const Bool_t setGausFit, const Double_t exclusionR,const Double_t fitR ){

   //set the fit options:
   //for usage of gaussian function instead of polynomial (default) set setGausFit=kTRUE
   //set the range of rejection of points around 0 via exclusionR
   //set the fit range around 0 with fitR
   if(fitR){
      fFitGaus = setGausFit;
      fExclRange  = exclusionR;
      fRange = fitR;
  
      if(fFitGaus) Printf("set MakeGausFit with fit range %2.3f and exclusion range in 1/pt: %2.3f",fRange,fExclRange);
      else  Printf("set standard polynomial fit with fit range %2.3f and exclusion range in 1/pt: %2.3f",fRange,fExclRange);
   }
   else Printf(" AliPerfAnalyzeInvPt::SetMakeFitOption: no user input. Set standard polynomial fit with fit range 1.0 and exclusion range 0.13.");
}

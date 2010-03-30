/**************************************************************************
 * Copyright(c) 1998-2009, ALICE Experiment at CERN, All rights reserved. *
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

/////////////////////////////////////////////////////////////
//
// AliHFMassFitter for the fit of invariant mass distribution
// of charmed mesons
//
// Author: C.Bianchin, chiara.bianchin@pd.infn.it
/////////////////////////////////////////////////////////////

#include <TCanvas.h>
#include <TMinuit.h>
#include <TGraph.h>
#include <TVirtualFitter.h>
#include <TClonesArray.h>

#include "AliHFMassFitter.h"


ClassImp(AliHFMassFitter)

 //************** constructors
AliHFMassFitter::AliHFMassFitter() : 
  TNamed(),
  fhistoInvMass(0),
  fminMass(0),
  fmaxMass(0),
  fNbin(0),
  fParsSize(1),
  fFitPars(0),
  fWithBkg(0),
  ftypeOfFit4Bkg(0),
  ftypeOfFit4Sgn(0),
  ffactor(1),
  fntuParam(0),
  fMass(1.85),
  fSigmaSgn(0.012),
  fSideBands(0),
  fSideBandl(0),
  fSideBandr(0),
  fcounter(0),
  fContourGraph(0)
{
  // default constructor

  cout<<"Default constructor"<<endl;

}

//___________________________________________________________________________

AliHFMassFitter::AliHFMassFitter (TH1F *histoToFit, Double_t minvalue, Double_t maxvalue, Int_t rebin,Int_t fittypeb,Int_t fittypes): 
 TNamed(),
 fhistoInvMass(0),
 fminMass(0),
 fmaxMass(0),
 fNbin(0),
 fParsSize(1),
 fFitPars(0),
 fWithBkg(0),
 ftypeOfFit4Bkg(0),
 ftypeOfFit4Sgn(0),
 ffactor(1),
 fntuParam(0),
 fMass(1.85),
 fSigmaSgn(0.012),
 fSideBands(0),
 fSideBandl(0),
 fSideBandr(0),
 fcounter(0),
 fContourGraph(0)
{
  // standard constructor

  fhistoInvMass= (TH1F*)histoToFit->Clone("fhistoInvMass");
  fminMass=minvalue; 
  fmaxMass=maxvalue;
  if(rebin!=1) RebinMass(rebin); 
  else fNbin=(Int_t)fhistoInvMass->GetNbinsX();
  ftypeOfFit4Bkg=fittypeb;
  ftypeOfFit4Sgn=fittypes;
  if(ftypeOfFit4Bkg!=0 && ftypeOfFit4Bkg!=1 && ftypeOfFit4Bkg!=2) fWithBkg=kFALSE;
  else fWithBkg=kTRUE;
  if (!fWithBkg) cout<<"Fit Histogram of Signal only"<<endl;
  else  cout<<"Type of fit For Background = "<<ftypeOfFit4Bkg<<endl;

  ComputeParSize();
  fFitPars=new Float_t[fParsSize];

  fContourGraph=new TList();
  fContourGraph->SetOwner();

}


AliHFMassFitter::AliHFMassFitter(const AliHFMassFitter &mfit):
  TNamed(),
  fhistoInvMass(mfit.fhistoInvMass),
  fminMass(mfit.fminMass),
  fmaxMass(mfit.fmaxMass),
  fNbin(mfit.fNbin),
  fParsSize(mfit.fParsSize),
  fFitPars(0),
  fWithBkg(mfit.fWithBkg),
  ftypeOfFit4Bkg(mfit.ftypeOfFit4Bkg),
  ftypeOfFit4Sgn(mfit.ftypeOfFit4Sgn),
  ffactor(mfit.ffactor),
  fntuParam(mfit.fntuParam),
  fMass(mfit.fMass),
  fSigmaSgn(mfit.fSigmaSgn),
  fSideBands(mfit.fSideBands),
  fSideBandl(mfit.fSideBandl),
  fSideBandr(mfit.fSideBandr),
  fcounter(mfit.fcounter),
  fContourGraph(mfit.fContourGraph)
{
  //copy constructor
  
  if(mfit.fParsSize > 0){
    fFitPars=new Float_t[fParsSize];
    memcpy(fFitPars,mfit.fFitPars,mfit.fParsSize*sizeof(Float_t));
  }
  //for(Int_t i=0;i<fParsSize;i++) fFitPars[i]=mfit.fFitPars[i];
}

//_________________________________________________________________________

AliHFMassFitter::~AliHFMassFitter() {
  cout<<"AliHFMassFitter destructor called"<<endl;
  if(fhistoInvMass) {
    cout<<"deleting histogram..."<<endl;
    delete fhistoInvMass;
    fhistoInvMass=NULL;
  }
  if(fntuParam){
    cout<<"deleting ntuple..."<<endl;
    delete fntuParam;

    fntuParam=NULL;
  }

  if(fFitPars) {
    delete[] fFitPars;
    cout<<"deleting parameter array..."<<endl;
    fFitPars=NULL;
  }

  fcounter = 0;
}

//_________________________________________________________________________

AliHFMassFitter& AliHFMassFitter::operator=(const AliHFMassFitter &mfit){

  //assignment operator

  if(&mfit == this) return *this;
  fhistoInvMass= mfit.fhistoInvMass;
  fminMass= mfit.fminMass;
  fmaxMass= mfit.fmaxMass;
  fNbin= mfit.fNbin;
  fParsSize= mfit.fParsSize;
  fWithBkg= mfit.fWithBkg;
  ftypeOfFit4Bkg= mfit.ftypeOfFit4Bkg;
  ftypeOfFit4Sgn= mfit.ftypeOfFit4Sgn;
  ffactor= mfit.ffactor;
  fntuParam= mfit.fntuParam;
  fMass= mfit.fMass;
  fSigmaSgn= mfit.fSigmaSgn;
  fSideBands= mfit.fSideBands;
  fSideBandl= mfit.fSideBandl;
  fSideBandr= mfit.fSideBandr;
  fcounter= mfit.fcounter;
  fContourGraph= mfit.fContourGraph;

  if(mfit.fParsSize > 0){
    if(fFitPars) {
      delete[] fFitPars;
      fFitPars=NULL;
    }
    fFitPars=new Float_t[fParsSize];
    memcpy(fFitPars,mfit.fFitPars,mfit.fParsSize*sizeof(Float_t));
  }
// fFitPars=new Float_t[fParsSize];
//   for(Int_t i=0;i<fParsSize;i++) fFitPars[i]=mfit.fFitPars[i];

  return *this;
}

//************ tools & settings

//__________________________________________________________________________

void AliHFMassFitter::ComputeParSize() {
  switch (ftypeOfFit4Bkg) {//npar backround func
  case 0:
    fParsSize = 2*3;
    break;
  case 1:
    fParsSize = 2*3;
    break;
  case 2:
    fParsSize = 3*3;
    break;
  case 3:
    fParsSize = 1*3;
    break;
  default:
    cout<<"Error in computing fParsSize: check ftypeOfFit4Bkg"<<endl;
    break;
  }

  fParsSize += 3; // npar refl
  fParsSize += 3; // npar signal gaus

  fParsSize*=2;   // add errors
  cout<<"Parameters array size "<<fParsSize<<endl;
}

//___________________________________________________________________________
void AliHFMassFitter::SetHisto(TH1F *histoToFit){
  //fhistoInvMass = (TH1F*)histoToFit->Clone();
  fhistoInvMass = new TH1F(*histoToFit);
  cout<<"SetHisto pointer "<<fhistoInvMass<<endl;
}

//___________________________________________________________________________

  void AliHFMassFitter::SetType(Int_t fittypeb, Int_t fittypes) {
    ftypeOfFit4Bkg = fittypeb; 
    ftypeOfFit4Sgn = fittypes; 
    ComputeParSize();
    fFitPars = new Float_t[fParsSize];

}

//___________________________________________________________________________

void AliHFMassFitter::Reset() {
  cout<<"Reset called: delete histogram, set mean value to 1.85 and sigma to 0.012"<<endl;
  fMass=1.85;
  fSigmaSgn=0.012;
  cout<<"Reset "<<fhistoInvMass<<endl;
  if(fhistoInvMass) {
    //cout<<"esiste"<<endl;
    //delete fhistoInvMass;
    fhistoInvMass=NULL;
    cout<<fhistoInvMass<<endl;
  }
  else cout<<"histogram doesn't exist, do not delete"<<endl;
  
}

//_________________________________________________________________________

void AliHFMassFitter::InitNtuParam(char *ntuname) {
  // Create ntuple to keep fit parameters
  fntuParam=0;
  fntuParam=new TNtuple(ntuname,"Contains fit parameters","intbkg1:slope1:conc1:intGB:meanGB:sigmaGB:intbkg2:slope2:conc2:inttot:slope3:conc3:intsgn:meansgn:sigmasgn:intbkg1Err:slope1Err:conc1Err:intGBErr:meanGBErr:sigmaGBErr:intbkg2Err:slope2Err:conc2Err:inttotErr:slope3Err:conc3Err:intsgnErr:meansgnErr:sigmasgnErr");
  
}

//_________________________________________________________________________

void AliHFMassFitter::FillNtuParam() {
  // Fill ntuple with fit parameters

  Float_t nothing=0.;

  if (ftypeOfFit4Bkg==2) {
      fntuParam->SetBranchAddress("intbkg1",&fFitPars[0]);
      fntuParam->SetBranchAddress("slope1",&fFitPars[1]);
      fntuParam->SetBranchAddress("conc1",&fFitPars[2]);
      fntuParam->SetBranchAddress("intGB",&fFitPars[3]);
      fntuParam->SetBranchAddress("meanGB",&fFitPars[4]);
      fntuParam->SetBranchAddress("sigmaGB",&fFitPars[5]);
      fntuParam->SetBranchAddress("intbkg2",&fFitPars[6]);
      fntuParam->SetBranchAddress("slope2",&fFitPars[7]);
      fntuParam->SetBranchAddress("conc2",&fFitPars[8]);
      fntuParam->SetBranchAddress("inttot",&fFitPars[9]);
      fntuParam->SetBranchAddress("slope3",&fFitPars[10]);
      fntuParam->SetBranchAddress("conc3",&fFitPars[11]);
      fntuParam->SetBranchAddress("intsgn",&fFitPars[12]);
      fntuParam->SetBranchAddress("meansgn",&fFitPars[13]);
      fntuParam->SetBranchAddress("sigmasgn",&fFitPars[14]);

      fntuParam->SetBranchAddress("intbkg1Err",&fFitPars[15]);
      fntuParam->SetBranchAddress("slope1Err",&fFitPars[16]);
      fntuParam->SetBranchAddress("conc1Err",&fFitPars[17]);
      fntuParam->SetBranchAddress("intGBErr",&fFitPars[18]);
      fntuParam->SetBranchAddress("meanGBErr",&fFitPars[19]);
      fntuParam->SetBranchAddress("sigmaGBErr",&fFitPars[20]);
      fntuParam->SetBranchAddress("intbkg2Err",&fFitPars[21]);
      fntuParam->SetBranchAddress("slope2Err",&fFitPars[22]);
      fntuParam->SetBranchAddress("conc2Err",&fFitPars[23]);
      fntuParam->SetBranchAddress("inttotErr",&fFitPars[24]);
      fntuParam->SetBranchAddress("slope3Err",&fFitPars[25]);
      fntuParam->SetBranchAddress("conc3Err",&fFitPars[26]);
      fntuParam->SetBranchAddress("intsgnErr",&fFitPars[27]);
      fntuParam->SetBranchAddress("meansgnErr",&fFitPars[28]);
      fntuParam->SetBranchAddress("sigmasgnErr",&fFitPars[29]);
    
  } else {
    
    if(ftypeOfFit4Bkg==3){
      fntuParam->SetBranchAddress("intbkg1",&fFitPars[0]);
      fntuParam->SetBranchAddress("slope1",&nothing);
      fntuParam->SetBranchAddress("conc1",&nothing);
      fntuParam->SetBranchAddress("intGB",&fFitPars[1]);
      fntuParam->SetBranchAddress("meanGB",&fFitPars[2]);
      fntuParam->SetBranchAddress("sigmaGB",&fFitPars[3]);
      fntuParam->SetBranchAddress("intbkg2",&fFitPars[4]);
      fntuParam->SetBranchAddress("slope2",&nothing);
      fntuParam->SetBranchAddress("conc2",&nothing);
      fntuParam->SetBranchAddress("inttot",&fFitPars[6]);
      fntuParam->SetBranchAddress("slope3",&nothing);
      fntuParam->SetBranchAddress("conc3",&nothing);
      fntuParam->SetBranchAddress("intsgn",&fFitPars[6]);
      fntuParam->SetBranchAddress("meansgn",&fFitPars[7]);
      fntuParam->SetBranchAddress("sigmasgn",&fFitPars[8]);

      fntuParam->SetBranchAddress("intbkg1Err",&fFitPars[9]);
      fntuParam->SetBranchAddress("slope1Err",&nothing);
      fntuParam->SetBranchAddress("conc1Err",&nothing);
      fntuParam->SetBranchAddress("intGBErr",&fFitPars[10]);
      fntuParam->SetBranchAddress("meanGBErr",&fFitPars[11]);
      fntuParam->SetBranchAddress("sigmaGBErr",&fFitPars[12]);
      fntuParam->SetBranchAddress("intbkg2Err",&fFitPars[13]);
      fntuParam->SetBranchAddress("slope2Err",&nothing);
      fntuParam->SetBranchAddress("conc2Err",&nothing);
      fntuParam->SetBranchAddress("inttotErr",&fFitPars[15]);
      fntuParam->SetBranchAddress("slope3Err",&nothing);
      fntuParam->SetBranchAddress("conc3Err",&nothing);
      fntuParam->SetBranchAddress("intsgnErr",&fFitPars[15]);
      fntuParam->SetBranchAddress("meansgnErr",&fFitPars[16]);
      fntuParam->SetBranchAddress("sigmasgnErr",&fFitPars[17]);

    }
    else{
      fntuParam->SetBranchAddress("intbkg1",&fFitPars[0]);
      fntuParam->SetBranchAddress("slope1",&fFitPars[1]);
      fntuParam->SetBranchAddress("conc1",&nothing);
      fntuParam->SetBranchAddress("intGB",&fFitPars[2]);
      fntuParam->SetBranchAddress("meanGB",&fFitPars[3]);
      fntuParam->SetBranchAddress("sigmaGB",&fFitPars[4]);
      fntuParam->SetBranchAddress("intbkg2",&fFitPars[5]);
      fntuParam->SetBranchAddress("slope2",&fFitPars[6]);
      fntuParam->SetBranchAddress("conc2",&nothing);
      fntuParam->SetBranchAddress("inttot",&fFitPars[7]);
      fntuParam->SetBranchAddress("slope3",&fFitPars[8]);
      fntuParam->SetBranchAddress("conc3",&nothing);
      fntuParam->SetBranchAddress("intsgn",&fFitPars[9]);
      fntuParam->SetBranchAddress("meansgn",&fFitPars[10]);
      fntuParam->SetBranchAddress("sigmasgn",&fFitPars[11]);

      fntuParam->SetBranchAddress("intbkg1Err",&fFitPars[12]);
      fntuParam->SetBranchAddress("slope1Err",&fFitPars[13]);
      fntuParam->SetBranchAddress("conc1Err",&nothing);
      fntuParam->SetBranchAddress("intGBErr",&fFitPars[14]);
      fntuParam->SetBranchAddress("meanGBErr",&fFitPars[15]);
      fntuParam->SetBranchAddress("sigmaGBErr",&fFitPars[16]);
      fntuParam->SetBranchAddress("intbkg2Err",&fFitPars[17]);
      fntuParam->SetBranchAddress("slope2Err",&fFitPars[18]);
      fntuParam->SetBranchAddress("conc2Err",&nothing);
      fntuParam->SetBranchAddress("inttotErr",&fFitPars[19]);
      fntuParam->SetBranchAddress("slope3Err",&fFitPars[20]);
      fntuParam->SetBranchAddress("conc3Err",&nothing);
      fntuParam->SetBranchAddress("intsgnErr",&fFitPars[21]);
      fntuParam->SetBranchAddress("meansgnErr",&fFitPars[22]);
      fntuParam->SetBranchAddress("sigmasgnErr",&fFitPars[23]);
    }
     
  }
  fntuParam->TTree::Fill();
}

//_________________________________________________________________________

TNtuple* AliHFMassFitter::NtuParamOneShot(char *ntuname){
  // Create, fill and return ntuple with fit parameters

  InitNtuParam(ntuname);
  FillNtuParam();
  return fntuParam;
}
//_________________________________________________________________________

void AliHFMassFitter::RebinMass(Int_t bingroup){
  // Rebin invariant mass histogram

  if(bingroup<1){
    cout<<"Error! Cannot group "<<bingroup<<" bins\n";
    fNbin=fhistoInvMass->GetNbinsX();
    cout<<"Kept original number of bins: "<<fNbin<<endl;
  } else{
    fhistoInvMass->Rebin(bingroup);
    fNbin = fhistoInvMass->GetNbinsX();
    cout<<"New number of bins: "<<fNbin<<endl;
  } 
  
       
}

//************* fit

//___________________________________________________________________________

Double_t AliHFMassFitter::FitFunction4MassDistr (Double_t *x, Double_t *par){
  // Fit function for signal+background


  //exponential or linear fit
  //
  // par[0] = tot integral
  // par[1] = slope
  // par[2] = gaussian integral
  // par[3] = gaussian mean
  // par[4] = gaussian sigma
  
  Double_t total,bkg=0,sgn=0;
  
  if (ftypeOfFit4Bkg==0 || ftypeOfFit4Bkg==1) {
    if(ftypeOfFit4Sgn == 0) {

      Double_t parbkg[2] = {par[0]-par[2], par[1]};
      bkg = FitFunction4Bkg(x,parbkg);
    }
    if(ftypeOfFit4Sgn == 1) {
      Double_t parbkg[5] = {par[2],par[3],ffactor*par[4],par[0]-2*par[2], par[1]};
      bkg = FitFunction4Bkg(x,parbkg);
    }

    sgn = FitFunction4Sgn(x,&par[2]);  

  }

  //polynomial fit

    // par[0] = tot integral
    // par[1] = coef1
    // par[2] = coef2
    // par[3] = gaussian integral
    // par[4] = gaussian mean
    // par[5] = gaussian sigma

  if (ftypeOfFit4Bkg==2) {
    
    if(ftypeOfFit4Sgn == 0) {
      
      Double_t parbkg[3] = {par[0]-par[3], par[1], par[2]};
      bkg = FitFunction4Bkg(x,parbkg);
    }
    if(ftypeOfFit4Sgn == 1) {
      
      Double_t parbkg[6] = {par[3],par[4],ffactor*par[5],par[0]-2*par[3], par[1], par[2]};
     bkg = FitFunction4Bkg(x,parbkg);
    }
    
    sgn = FitFunction4Sgn(x,&par[3]);
  }

  if (ftypeOfFit4Bkg==3) {
   
    if(ftypeOfFit4Sgn == 0) {
	bkg=FitFunction4Bkg(x,par);
	sgn=FitFunction4Sgn(x,&par[1]);
    }
    if(ftypeOfFit4Sgn == 1) {
      Double_t parbkg[4]={par[1],par[2],ffactor*par[3],par[0]};
      bkg=FitFunction4Bkg(x,parbkg);
      sgn=FitFunction4Sgn(x,&par[1]);
    }
  }

  total = bkg + sgn;
  
  return  total;
}

//_________________________________________________________________________
Double_t AliHFMassFitter::FitFunction4Sgn (Double_t *x, Double_t *par){
  // Fit function for the signal

  //gaussian = A/(sigma*sqrt(2*pi))*exp(-(x-mean)^2/2/sigma^2)
  //Par:
  // * [0] = integralSgn
  // * [1] = mean
  // * [2] = sigma
  //gaussian = [0]/TMath::Sqrt(2.*TMath::Pi())/[2]*exp[-(x-[1])*(x-[1])/(2*[2]*[2])]

  return par[0]/TMath::Sqrt(2.*TMath::Pi())/par[2]*TMath::Exp(-(x[0]-par[1])*(x[0]-par[1])/2./par[2]/par[2]);

}

//__________________________________________________________________________

Double_t AliHFMassFitter::FitFunction4Bkg (Double_t *x, Double_t *par){
  // Fit function for the background

  Double_t maxDeltaM = 4.*fSigmaSgn;
  if(fSideBands && TMath::Abs(x[0]-fMass) < maxDeltaM) {
    TF1::RejectPoint();
    return 0;
  }
  Int_t firstPar=0;
  Double_t gaus2=0,total=-1;
  if(ftypeOfFit4Sgn == 1){
    firstPar=3;
    //gaussian = A/(sigma*sqrt(2*pi))*exp(-(x-mean)^2/2/sigma^2)
    //Par:
    // * [0] = integralSgn
    // * [1] = mean
    // * [2] = sigma
    //gaussian = [0]/TMath::Sqrt(2.*TMath::Pi())/[2]*exp[-(x-[1])*(x-[1])/(2*[2]*[2])]
    gaus2 = FitFunction4Sgn(x,par);
  }

  switch (ftypeOfFit4Bkg){
  case 0:
    //exponential
    //exponential = A*exp(B*x) -> integral(exponential)=A/B*exp(B*x)](min,max)
    //-> A = B*integral/(exp(B*max)-exp(B*min)) where integral can be written
    //as integralTot- integralGaus (=par [2])
    //Par:
    // * [0] = integralBkg;
    // * [1] = B;
    //exponential = [1]*[0]/(exp([1]*max)-exp([1]*min))*exp([1]*x)
    total = par[0+firstPar]*par[1+firstPar]/(TMath::Exp(par[1+firstPar]*fmaxMass)-TMath::Exp(par[1+firstPar]*fminMass))*TMath::Exp(par[1+firstPar]*x[0]);
    break;
  case 1:
    //linear
    //y=a+b*x -> integral = a(max-min)+1/2*b*(max^2-min^2) -> a = (integral-1/2*b*(max^2-min^2))/(max-min)=integral/(max-min)-1/2*b*(max+min)
    // * [0] = integralBkg;
    // * [1] = b;
    total= par[0+firstPar]/(fmaxMass-fminMass)+par[1+firstPar]*(x[0]-0.5*(fmaxMass+fminMass));
    break;
  case 2:
    //polynomial
    //y=a+b*x+c*x**2 -> integral = a(max-min) + 1/2*b*(max^2-min^2) +
    //+ 1/3*c*(max^3-min^3) -> 
    //a = (integral-1/2*b*(max^2-min^2)-1/3*c*(max^3-min^3))/(max-min)
    // * [0] = integralBkg;
    // * [1] = b;
    // * [2] = c;
    total = par[0+firstPar]/(fmaxMass-fminMass)+par[1]*(x[0]-0.5*(fmaxMass+fminMass))+par[2+firstPar]*(x[0]*x[0]-1/3.*(fmaxMass*fmaxMass*fmaxMass-fminMass*fminMass*fminMass)/(fmaxMass-fminMass));
    break;
  case 3:
    total=par[0+firstPar];
    break;
//   default:
//     Types of Fit Functions for Background:
//     * 0 = exponential;
//     * 1 = linear;
//     * 2 = polynomial 2nd order
//     * 3 = no background"<<endl;

  }
  return total+gaus2;
}

//__________________________________________________________________________
Bool_t AliHFMassFitter::SideBandsBounds(){

  Double_t width=fhistoInvMass->GetBinWidth(8);
  if (fNbin==0) fNbin=fhistoInvMass->GetNbinsX();
  Double_t minHisto=fhistoInvMass->GetBinLowEdge(1);
  Double_t maxHisto=fhistoInvMass->GetBinLowEdge(fNbin)+width;

  if(fMass-fminMass < 0 || fmaxMass-fMass <0) {
    cout<<"Left limit of range > mean or right limit of range < mean: change left/right limit or initial mean value"<<endl;
    return kFALSE;
  } 
  
  if((fminMass!=minHisto || fmaxMass!= maxHisto) && (fMass-4.*fSigmaSgn-fminMass) <= 0){
    Double_t coeff = (fMass-fminMass)/fSigmaSgn;
    
    fSideBandl=(Int_t)((fMass-0.5*coeff*fSigmaSgn-fminMass)/width);
    fSideBandr=(Int_t)((fMass+0.5*coeff*fSigmaSgn-fminMass)/width);
    cout<<"Changed number of sigma from 4 to "<<0.5*coeff<<" for the estimation of the side bands"<<endl;
    if (coeff<3) cout<<"Side bands inside 3 sigma, may be better use ftypeOfFit4Bkg = 3 (only signal)"<<endl;

    if (coeff<2) {
      cout<<"Side bands inside 2 sigma. Change mode: ftypeOfFit4Bkg = 3"<<endl;
      ftypeOfFit4Bkg=3;
      //set binleft and right without considering SetRangeFit- anyway no bkg!
      fSideBandl=(Int_t)((fMass-4.*fSigmaSgn-minHisto)/width);
      fSideBandr=(Int_t)((fMass+4.*fSigmaSgn-minHisto)/width);
    }
  }
  else {
  fSideBandl=(Int_t)((fMass-4.*fSigmaSgn-minHisto)/width);
  fSideBandr=(Int_t)((fMass+4.*fSigmaSgn-minHisto)/width);
//   cout<<"\tfMass = "<<fMass<<"\tfSigmaSgn = "<<fSigmaSgn<<"\tminHisto = "<<minHisto<<endl;
//   cout<<"\tbinleft = "<<fSideBandl<<"\tbinright = "<<fSideBandr<<endl;
  }

  if (fSideBandl==0) {
    cout<<"Error! Range too little"; 
    return kFALSE;
  }
  return kTRUE;
}

//__________________________________________________________________________

void AliHFMassFitter::GetSideBandsBounds(Int_t &left, Int_t &right) const{
  if (fSideBandl==0 && fSideBandr==0){
    cout<<"Use MassFitter method first"<<endl;
    return;
  }
  left=fSideBandl;
  right=fSideBandr;
}

//__________________________________________________________________________

Bool_t AliHFMassFitter::MassFitter(Bool_t draw){  
  // Main method of the class: performs the fit of the histogram
  
  //Set default fitter Minuit in order to use gMinuit in the contour plots    
  TVirtualFitter::SetDefaultFitter("Minuit");

  Int_t nFitPars=0; //total function's number of parameters
  switch (ftypeOfFit4Bkg){
  case 0:
    nFitPars=5; //3+2
    break;
  case 1:
    nFitPars=5; //3+2
    break;
  case 2:
    nFitPars=6; //3+3
    break;
  case 3:
    nFitPars=4; //3+1
    break;
  }

  Int_t bkgPar = nFitPars-3; //background function's number of parameters

  cout<<"nFitPars = "<<nFitPars<<"\tbkgPar = "<<bkgPar<<endl;


  TString listname="contourplot";
  listname+=fcounter;
  if(!fContourGraph){  
    fContourGraph=new TList();
    fContourGraph->SetOwner();
  }

  fContourGraph->SetName(listname);


  //function names
  TString bkgname="funcbkg";
  TString bkg1name="funcbkg1";
  TString massname="funcmass";

  //Total integral
  Double_t totInt = fhistoInvMass->Integral("width");
  cout<<"Integral"<<endl;

  fSideBands = kTRUE;
  Double_t width=fhistoInvMass->GetBinWidth(8);
  cout<<"fNbin"<<fNbin<<endl;
  if (fNbin==0) fNbin=fhistoInvMass->GetNbinsX();
  Double_t minHisto=fhistoInvMass->GetBinLowEdge(1);
  Double_t maxHisto=fhistoInvMass->GetBinLowEdge(fNbin)+width;
  Bool_t ok=SideBandsBounds();
  if(!ok) return kFALSE;
  
  //sidebands integral - first approx (from histo)
  Double_t sideBandsInt=(Double_t)fhistoInvMass->Integral(1,fSideBandl,"width") + (Double_t)fhistoInvMass->Integral(fSideBandr,fNbin,"width");
  cout<<"------nbin = "<<fNbin<<"\twidth = "<<width<<"\tbinleft = "<<fSideBandl<<"\tbinright = "<<fSideBandr<<endl;
  cout<<"------sideBandsInt - first approx = "<<sideBandsInt<<endl;
  if (sideBandsInt<=0) {
    cout<<"! sideBandsInt <=0. There's a problem, cannot start the fit"<<endl;
    return kFALSE;
  }
  
  /*Fit Bkg*/


  TF1 *funcbkg = new TF1(bkgname.Data(),this,&AliHFMassFitter::FitFunction4Bkg,minHisto,maxHisto,bkgPar,"AliHFMassFitter","FitFunction4Bkg");
  cout<<"Function name = "<<funcbkg->GetName()<<endl<<endl;

  funcbkg->SetLineColor(2); //red

  //first fit for bkg: approx bkgint
 
  switch (ftypeOfFit4Bkg) {
  case 0: //gaus+expo
    funcbkg->SetParNames("BkgInt","Slope"); 
    funcbkg->SetParameters(sideBandsInt,-2.); 
    break;
  case 1:
    funcbkg->SetParNames("BkgInt","Slope");
    funcbkg->SetParameters(sideBandsInt,-100.); 
    break;
  case 2:
    funcbkg->SetParNames("BkgInt","Coef1","Coef2");
    funcbkg->SetParameters(sideBandsInt,-10.,5);
    break;
  case 3:
    if(ftypeOfFit4Sgn==0){
      funcbkg->SetParNames("Const");
      funcbkg->SetParameter(0,0.);
      funcbkg->FixParameter(0,0.);
    }
    break;
  default:
    cout<<"Wrong choise of ftypeOfFit4Bkg ("<<ftypeOfFit4Bkg<<")"<<endl;
    return kFALSE;
    break;
  }
  cout<<"\nBACKGROUND FIT - only combinatorial"<<endl;
  Int_t ftypeOfFit4SgnBkp=ftypeOfFit4Sgn;
  
  Double_t intbkg1=0,slope1=0,conc1=0;
  //if only signal and reflection: skip
  if (!(ftypeOfFit4Bkg==3 && ftypeOfFit4Sgn==1)) {
    ftypeOfFit4Sgn=0;
    fhistoInvMass->Fit(bkgname.Data(),"R,L,E,0");
   
    for(Int_t i=0;i<bkgPar;i++){
      fFitPars[i]=funcbkg->GetParameter(i);
      //cout<<i<<"\t"<<funcbkg->GetParameter(i)<<"\t";
      fFitPars[nFitPars+2*bkgPar+3+i]= funcbkg->GetParError(i);
      //cout<<nFitPars+2*bkgPar+3+i<<"\t"<< funcbkg->GetParError(i)<<endl;
    }
    fSideBands = kFALSE;
    //intbkg1 = funcbkg->GetParameter(0);
    funcbkg->SetRange(fminMass,fmaxMass);
    intbkg1 = funcbkg->Integral(fminMass,fmaxMass);
    if(ftypeOfFit4Bkg!=3) slope1 = funcbkg->GetParameter(1);
    if(ftypeOfFit4Bkg==2) conc1 = funcbkg->GetParameter(2);
    cout<<"Primo fit: \nintbkg1 = "<<intbkg1<<"\t(Compare with par0 = "<<funcbkg->GetParameter(0)<<")\nslope1= "<<slope1<<"\nconc1 = "<<conc1<<endl;
  } 
  else cout<<"\t\t//"<<endl;
  
  ftypeOfFit4Sgn=ftypeOfFit4SgnBkp;
  TF1 *funcbkg1=0;
  if (ftypeOfFit4Sgn == 1) {
    cout<<"\nBACKGROUND FIT WITH REFLECTION"<<endl;
    bkgPar+=3;
    
    cout<<"nFitPars = "<<nFitPars<<"\tbkgPar = "<<bkgPar<<endl;

    funcbkg1 = new TF1(bkg1name.Data(),this,&AliHFMassFitter::FitFunction4Bkg,fminMass,fmaxMass,bkgPar,"AliHFMassFitter","FitFunction4Bkg");
    cout<<"Function name = "<<funcbkg1->GetName()<<endl;

    funcbkg1->SetLineColor(2); //red

    if(ftypeOfFit4Bkg==2){
      cout<<"*** Polynomial Fit ***"<<endl;
      funcbkg1->SetParNames("IntGB","MeanGB","SigmaGB","BkgInt","Coef1","Coef2");
      funcbkg1->SetParameters(0.5*(totInt-intbkg1),fMass,ffactor*fSigmaSgn,intbkg1,slope1,conc1);
//     cout<<"Parameters set to: "<<0.5*(totInt-intbkg1)<<"\t"<<fMass<<"\t"<<ffactor*fSigmaSgn<<"\t"<<intbkg1<<"\t"<<slope1<<"\t"<<conc1<<"\t"<<endl;
//     cout<<"Limits: ("<<fminMass<<","<<fmaxMass<<")\tnPar = "<<bkgPar<<"\tgsidebands = "<<fSideBands<<endl;
    } else{
      if(ftypeOfFit4Bkg==3) //no background: gaus sign+ gaus broadened
	{
	  cout<<"*** No background Fit ***"<<endl;
	  funcbkg1->SetParNames("IntGB","MeanGB","SigmaGB","Const");
	  funcbkg1->SetParameters(0.5*totInt,fMass,ffactor*fSigmaSgn,0.); 
	  funcbkg1->FixParameter(3,0.);
	} else{ //expo or linear
	  if(ftypeOfFit4Bkg==0) cout<<"*** Exponential Fit ***"<<endl;
	  if(ftypeOfFit4Bkg==1) cout<<"*** Linear Fit ***"<<endl;
	  funcbkg1->SetParNames("IntGB","MeanGB","SigmaGB","BkgInt","Slope");
	  funcbkg1->SetParameters(0.5*(totInt-intbkg1),fMass,ffactor*fSigmaSgn,intbkg1,slope1);
	}
    }
    fhistoInvMass->Fit(bkg1name.Data(),"R,L,E,+,0");
  
    for(Int_t i=0;i<bkgPar;i++){
      fFitPars[bkgPar-3+i]=funcbkg1->GetParameter(i);
      //cout<<bkgPar-3+i<<"\t"<<funcbkg1->GetParameter(i);
      fFitPars[nFitPars+3*bkgPar-6+i]= funcbkg1->GetParError(i);
      //cout<<"\t"<<nFitPars+3*bkgPar-6+i<<"\t"<<funcbkg1->GetParError(i)<<endl; 
    }

    intbkg1=funcbkg1->GetParameter(3);
    if(ftypeOfFit4Bkg!=3) slope1 = funcbkg1->GetParameter(4);
    if(ftypeOfFit4Bkg==2) conc1 = funcbkg1->GetParameter(5);

  } else {
    bkgPar+=3;

    for(Int_t i=0;i<3;i++){
      fFitPars[bkgPar-3+i]=0.;
      cout<<bkgPar-3+i<<"\t"<<0.<<"\t";
      fFitPars[nFitPars+3*bkgPar-6+i]= 0.;
      cout<<nFitPars+3*bkgPar-6+i<<"\t"<<0.<<endl;
    }
  
    for(Int_t i=0;i<bkgPar-3;i++){
      fFitPars[bkgPar+i]=funcbkg->GetParameter(i);
      cout<<bkgPar+i<<"\t"<<funcbkg->GetParameter(i)<<"\t";
      fFitPars[nFitPars+3*bkgPar-3+i]= funcbkg->GetParError(i);
      cout<<nFitPars+3*bkgPar-3+i<<"\t"<< funcbkg->GetParError(i)<<endl;
    }

   
  }

  //sidebands integral - second approx (from fit)
  fSideBands = kFALSE;
  Double_t bkgInt;
  cout<<"Compare intbkg1 = "<<intbkg1<<" and integral = ";
  if(ftypeOfFit4Sgn == 1) bkgInt=funcbkg1->Integral(fminMass,fmaxMass);
  else bkgInt=funcbkg->Integral(fminMass,fmaxMass);
  cout<</*"------BkgInt(Fit) = "<<*/bkgInt<<endl;

  //Signal integral - first approx
  Double_t sgnInt;
  sgnInt = totInt-bkgInt;
  cout<<"------TotInt = "<<totInt<<"\tsgnInt = "<<sgnInt<<endl;

  /*Fit All Mass distribution with exponential + gaussian (+gaussiam braodened) */
  TF1 *funcmass = new TF1(massname.Data(),this,&AliHFMassFitter::FitFunction4MassDistr,fminMass,fmaxMass,nFitPars,"AliHFMassFitter","FitFunction4MassDistr");
  cout<<"Function name = "<<funcmass->GetName()<<endl<<endl;
  funcmass->SetLineColor(4); //blue

  //Set parameters
  cout<<"\nTOTAL FIT"<<endl;

  if(nFitPars==5){
    funcmass->SetParNames("TotInt","Slope","SgnInt","Mean","Sigma");
    funcmass->SetParameters(totInt,slope1,sgnInt,fMass,fSigmaSgn);
    //cout<<"Parameters set to: "<<totInt<<"\t"<<slope1<<"\t"<<sgnInt<<"\t"<<fMass<<"\t"<<fSigmaSgn<<"\t"<<endl;
//     cout<<"Limits: ("<<fminMass<<","<<fmaxMass<<")\tnPar = "<<nFitPars<<"\tgsidebands = "<<fSideBands<<endl;
    funcmass->FixParameter(0,totInt);
  }
  if (nFitPars==6){
    funcmass->SetParNames("TotInt","Coef1","Coef2","SgnInt","Mean","Sigma");
    funcmass->SetParameters(totInt,slope1,conc1,sgnInt,fMass,fSigmaSgn);
//     cout<<"Parameters set to: "<<totInt<<"\t"<<slope1<<"\t"<<conc1<<"\t"<<sgnInt<<"\t"<<fMass<<"\t"<<fSigmaSgn<<"\t"<<endl;
//     cout<<"Limits: ("<<fminMass<<","<<fmaxMass<<")\tnPar = "<<nFitPars<<"\tgsidebands = "<<fSideBands<<endl;
    funcmass->FixParameter(0,totInt);
  }
  if(nFitPars==4){
    funcmass->SetParNames("Const","SgnInt","Mean","Sigma");
    if(ftypeOfFit4Sgn == 1) funcmass->SetParameters(0.,0.5*totInt,fMass,fSigmaSgn);
    else funcmass->SetParameters(0.,totInt,fMass,fSigmaSgn);
    funcmass->FixParameter(0,0.);
    //cout<<"Parameters set to: "<<0.5*totInt<<"\t"<<fMass<<"\t"<<fSigmaSgn<<"\t"<<endl;
    //cout<<"Limits: ("<<fminMass<<","<<fmaxMass<<")\tnPar = "<<nFitPars<<"\tgsidebands = "<<fSideBands<<endl;

  }
  
  fhistoInvMass->Fit(massname.Data(),"R,L,E,+,0");
  cout<<"fit done"<<endl;
  //reset value of fMass and fSigmaSgn to those found from fit
  fMass=funcmass->GetParameter(nFitPars-2);
  fSigmaSgn=funcmass->GetParameter(nFitPars-1);
  
  for(Int_t i=0;i<nFitPars;i++){
    fFitPars[i+2*bkgPar-3]=funcmass->GetParameter(i);
    fFitPars[nFitPars+4*bkgPar-6+i]= funcmass->GetParError(i);
    //cout<<i+2*bkgPar-3<<"\t"<<funcmass->GetParameter(i)<<"\t\t"<<nFitPars+4*bkgPar-6+i<<"\t"<<funcmass->GetParError(i)<<endl;
  }
  /*
  //check: cout parameters  
  for(Int_t i=0;i<2*(nFitPars+2*bkgPar-3);i++){
    cout<<i<<"\t"<<fFitPars[i]<<endl;
    }
  */
  
//   if(draw){
//     TCanvas *canvas=new TCanvas(fhistoInvMass->GetName(),fhistoInvMass->GetName());
//     TH1F *fhistocopy=new TH1F(*fhistoInvMass);
//     canvas->cd();
//     fhistocopy->DrawClone();
//     if(ftypeOfFit4Sgn == 1) funcbkg1->DrawClone("sames");
//     else funcbkg->DrawClone("sames");
//     funcmass->DrawClone("sames");
//     cout<<"Drawn"<<endl;
//   }

  if(funcmass->GetParameter(nFitPars-1) <0 || funcmass->GetParameter(nFitPars-2) <0 || funcmass->GetParameter(nFitPars-3) <0 ) {
    cout<<"IntS or mean or sigma negative. You may tray to SetInitialGaussianSigma(..) and SetInitialGaussianMean(..)"<<endl;
    return kFALSE;
  }

  //increase counter of number of fits done
  fcounter++;

  //contour plots
  if(draw){

    for (Int_t kpar=1; kpar<nFitPars;kpar++){

      for(Int_t jpar=kpar+1;jpar<nFitPars;jpar++){
	cout<<"Par "<<kpar<<" and "<<jpar<<endl;
	
	// produce 2 contours per couple of parameters
	TGraph* cont[2] = {0x0, 0x0};
	const Double_t errDef[2] = {1., 4.}; 
	for (Int_t i=0; i<2; i++) {
	  gMinuit->SetErrorDef(errDef[i]);
	  cont[i] = (TGraph*)gMinuit->Contour(80,kpar,jpar);
	}
	
	if(!cont[0] || !cont[1]){
	  cout<<"Skipping par "<<kpar<<" vs par "<<jpar<<endl;
	  continue;
	}
	  
	// set graph titles and add them to the list
	TString title = "Contour plot";
	TString titleX = funcmass->GetParName(kpar);
	TString titleY = funcmass->GetParName(jpar);
	for (Int_t i=0; i<2; i++) {
	  cont[i]->SetName( Form("cperr%d_%d%d", i, kpar, jpar) );
	  cont[i]->SetTitle(title);
	  cont[i]->GetXaxis()->SetTitle(titleX);
	  cont[i]->GetYaxis()->SetTitle(titleY);
	  cont[i]->GetYaxis()->SetLabelSize(0.033);
	  cont[i]->GetYaxis()->SetTitleSize(0.033);
	  cont[i]->GetYaxis()->SetTitleOffset(1.67);
	  
	  fContourGraph->Add(cont[i]);
	}
	
	// plot them
	TString cvname = Form("c%d%d", kpar, jpar);
	TCanvas *c4=new TCanvas(cvname,cvname,600,600);
	c4->cd();
	cont[1]->SetFillColor(38);
	cont[1]->Draw("alf");
	cont[0]->SetFillColor(9);
	cont[0]->Draw("lf");
        
      }
      
    }
    
  }

  if (ftypeOfFit4Sgn == 1 && funcbkg1) {
    delete funcbkg1;
    funcbkg1=NULL;
  }
  if (funcbkg) {
    delete funcbkg;
    funcbkg=NULL;
  }
  if (funcmass) {
    delete funcmass;
    funcmass=NULL;
  }

  AddFunctionsToHisto();
  if (draw) DrawFit();
 

  return kTRUE;
}
//_________________________________________________________________________
Double_t AliHFMassFitter::GetChiSquare() const{
  TF1 *funcmass=(TF1*)fhistoInvMass->GetFunction("funcmass");
  return funcmass->GetChisquare();
}

//_________________________________________________________________________
Double_t AliHFMassFitter::GetReducedChiSquare() const{
  TF1 *funcmass=(TF1*)fhistoInvMass->GetFunction("funcmass");
  return funcmass->GetChisquare()/funcmass->GetNDF();
}

//*********output

//_________________________________________________________________________
void  AliHFMassFitter::GetFitPars(Float_t *vector) const {
  // Return fit parameters

  for(Int_t i=0;i<fParsSize;i++){
    vector[i]=fFitPars[i];
  }
}


//_________________________________________________________________________
void AliHFMassFitter::AddFunctionsToHisto(){

  cout<<"AddFunctionsToHisto called"<<endl;
  TString bkgname = "funcbkg";
  Int_t np=-99;
  switch (ftypeOfFit4Bkg){
  case 0: //expo
    np=2;
    break;
  case 1: //linear
    np=2;
    break;
  case 2: //pol2
    np=3;
    break;
  case 3: //no bkg
    np=1;
    break;
  }
  if (ftypeOfFit4Sgn == 1) {
    bkgname += 1;
    np+=3;
  }

  TString bkgnamesave=bkgname;
  TString testname=bkgname;
  testname += "FullRange";
  TF1 *testfunc=(TF1*)fhistoInvMass->FindObject(testname.Data());
  if(testfunc){
    cout<<"AddFunctionsToHisto already used: exiting...."<<endl;
    return;
  }

  TList *hlist=fhistoInvMass->GetListOfFunctions();
  hlist->ls();

  TF1 *b=(TF1*)hlist->FindObject(bkgname.Data());
  if(!b){
    cout<<bkgname<<" not found, cannot produce "<<bkgname<<"FullRange and "<<bkgname<<"Recalc"<<endl;
    return;
  }

  bkgname += "FullRange";
  TF1 *bfullrange=new TF1(bkgname.Data(),this,&AliHFMassFitter::FitFunction4Bkg,fminMass,fmaxMass,np,"AliHFMassFitter","FitFunction4Bkg");
  //cout<<bfullrange->GetName()<<endl;
  for(Int_t i=0;i<np;i++){
    //cout<<i<<" di "<<np<<endl;
    bfullrange->SetParName(i,b->GetParName(i));
    bfullrange->SetParameter(i,b->GetParameter(i));
    bfullrange->SetParError(i,b->GetParError(i));
  }
  bfullrange->SetLineStyle(4);
  bfullrange->SetLineColor(14);

  bkgnamesave += "Recalc";

  TF1 *blastpar=new TF1(bkgnamesave.Data(),this,&AliHFMassFitter::FitFunction4Bkg,fminMass,fmaxMass,np,"AliHFMassFitter","FitFunction4Bkg");

  TF1 *mass=fhistoInvMass->GetFunction("funcmass");

  if (!mass){
    cout<<"funcmass doesn't exist "<<endl;
    return;
  }

  blastpar->SetParameter(0,mass->GetParameter(0)-mass->GetParameter(np));
  blastpar->SetParError(0,mass->GetParError(np));
  if (np>=2) {
    blastpar->SetParameter(1,mass->GetParameter(1));
    blastpar->SetParError(1,mass->GetParError(1));
  }
  if (np==3) {
    blastpar->SetParameter(2,mass->GetParameter(2));
    blastpar->SetParError(2,mass->GetParError(2));
  }

  blastpar->SetLineStyle(1);
  blastpar->SetLineColor(2);

  hlist->Add(bfullrange->Clone());
  hlist->Add(blastpar->Clone());
  hlist->ls();
  
  if(bfullrange) {
    delete bfullrange;
    bfullrange=NULL;
  }
  if(blastpar) {
    delete blastpar;
    blastpar=NULL;
  }
}

//_________________________________________________________________________

TH1F* AliHFMassFitter::GetHistoClone() const{

  TH1F* hout=(TH1F*)fhistoInvMass->Clone(fhistoInvMass->GetName());
  return hout;
}
//_________________________________________________________________________

void AliHFMassFitter::WriteHisto(TString path) {
  if (fcounter == 0) {
    cout<<"Use MassFitter method before WriteHisto"<<endl;
    return;
  }
  TH1F* hget=(TH1F*)fhistoInvMass->Clone();

  path += "HFMassFitterOutput.root";
  TFile *output;
 
  if (fcounter == 1) output = new TFile(path.Data(),"recreate");
  else output = new TFile(path.Data(),"update");
  output->cd();
  hget->Write();
  fContourGraph->Write();
  output->Close();

  cout<<fcounter<<" "<<hget->GetName()<<" written in "<<path<<endl;

  if(output) {
    delete output;
    output=NULL;
  }

}

//_________________________________________________________________________

void AliHFMassFitter::WriteNtuple(TString path) const{
  //TNtuple* nget=(TNtuple*)fntuParam->Clone();
  path += "HFMassFitterOutput.root";
  TFile *output = new TFile(path.Data(),"update");
  output->cd();
  fntuParam->Write();
  //nget->Write();
  output->Close();
  //cout<<nget->GetName()<<" written in "<<path<<endl;
  cout<<fntuParam->GetName()<<" written in "<<path<<endl;
  /*
  if(nget) {
    //delete nget;
    nget=NULL;
  }
  */
  if(output) {
    delete output;
    output=NULL;
  }
}

//_________________________________________________________________________

void AliHFMassFitter::DrawFit() const{

  TString cvtitle="fit of ";
  cvtitle+=fhistoInvMass->GetName();
  TString cvname="c";
  cvname+=fcounter;

  TCanvas *c=new TCanvas(cvname,cvtitle);
  c->cd();
  GetHistoClone()->Draw("hist");
  GetHistoClone()->GetFunction("funcbkgFullRange")->Draw("same");
  GetHistoClone()->GetFunction("funcbkgRecalc")->Draw("same");
  GetHistoClone()->GetFunction("funcmass")->Draw("same");


}


//_________________________________________________________________________

void AliHFMassFitter::PrintParTitles() const{
  TF1 *f=fhistoInvMass->GetFunction("funcmass");
  if(!f) {
    cout<<"Fit function not found"<<endl;
    return;
  }

  Int_t np=0;
  switch (ftypeOfFit4Bkg){
  case 0: //expo
    np=2;
    break;
  case 1: //linear
    np=2;
    break;
  case 2: //pol2
    np=3;
    break;
  case 3: //no bkg
    np=1;
    break;
  }

  np+=3; //3 parameter for signal
  if (ftypeOfFit4Sgn == 1) np+=3;

  cout<<"Parameter Titles \n";
  for(Int_t i=0;i<np;i++){
    cout<<"Par "<<i<<": "<<f->GetParName(i)<<endl;
  }
  cout<<endl;

}

//************ significance

//_________________________________________________________________________

void AliHFMassFitter::Signal(Double_t nOfSigma,Double_t &signal,Double_t &errsignal) const {
  // Return signal integral in mean+- n sigma

  if(fcounter==0) {
    cout<<"Use MassFitter method before Signal"<<endl;
    return;
  }

  //functions names
  TString bkgname= "funcbkgRecalc";
  TString bkg1name="funcbkg1Recalc";
  TString massname="funcmass";


  TF1 *funcbkg=0;
  TF1 *funcmass=fhistoInvMass->GetFunction(massname.Data());
  if(!funcmass){
    cout<<"AliHFMassFitter::Signal() ERROR -> Mass distr function not found!"<<endl;
    return;
  }

  if(ftypeOfFit4Sgn == 0) funcbkg=fhistoInvMass->GetFunction(bkgname.Data());
  else funcbkg=fhistoInvMass->GetFunction(bkg1name.Data());

  if(!funcbkg){
    cout<<"AliHFMassFitter::Signal() ERROR -> Bkg function not found!"<<endl;
    return;
  }

  Int_t np=-99;
  switch (ftypeOfFit4Bkg){
  case 0: //expo
    np=2;
    break;
  case 1: //linear
    np=2;
    break;
  case 2: //pol2
    np=3;
    break;
  case 3: //no bkg
    np=1;
    break;
  }

  Double_t intS,intSerr;

 //relative error evaluation
  intS=funcmass->GetParameter(np);
  intSerr=funcmass->GetParError(np);
  cout<<"Sgn relative error evaluation from fit: "<<intSerr/intS<<endl;
  Double_t background,errbackground;
  Background(nOfSigma,background,errbackground);

  //signal +/- error in nsigma
  Double_t min=fMass-nOfSigma*fSigmaSgn;
  Double_t max=fMass+nOfSigma*fSigmaSgn;

  Double_t mass=funcmass->Integral(min, max)/fhistoInvMass->GetBinWidth(4);
  signal=mass - background;
  errsignal=TMath::Sqrt((intSerr/intS*mass)*(intSerr/intS*mass)/*assume relative error is the same as for total integral*/ + errbackground*errbackground);
  return;
}

//_________________________________________________________________________

void AliHFMassFitter::Signal(Double_t min, Double_t max, Double_t &signal,Double_t &errsignal) const {

  // Return signal integral in a range
  
  if(fcounter==0) {
    cout<<"Use MassFitter method before Signal"<<endl;
    return;
  }

  //functions names
  TString bkgname="funcbkgRecalc";
  TString bkg1name="funcbkg1Recalc";
  TString massname="funcmass";


  TF1 *funcbkg=0;
  TF1 *funcmass=fhistoInvMass->GetFunction(massname.Data());
  if(!funcmass){
    cout<<"AliHFMassFitter::Signal() ERROR -> Mass distr function not found!"<<endl;
    return;
  }

  if(ftypeOfFit4Sgn == 0) funcbkg=fhistoInvMass->GetFunction(bkgname.Data());
  else funcbkg=fhistoInvMass->GetFunction(bkg1name.Data());

  if(!funcbkg){
    cout<<"AliHFMassFitter::Signal() ERROR -> Bkg function not found!"<<endl;
    return;
  }

  Int_t np=-99;
  switch (ftypeOfFit4Bkg){
  case 0: //expo
    np=2;
    break;
  case 1: //linear
    np=2;
    break;
  case 2: //pol2
    np=3;
    break;
  case 3: //no bkg
    np=1;
    break;
  }

  Double_t intS,intSerr;

 //relative error evaluation
  intS=funcmass->GetParameter(np);
  intSerr=funcmass->GetParError(np);

  cout<<"Sgn relative error evaluation from fit: "<<intSerr/intS<<endl;
  Double_t background,errbackground;
  Background(min,max,background,errbackground);

  //signal +/- error in the range

  Double_t mass=funcmass->Integral(min, max)/fhistoInvMass->GetBinWidth(4);
  signal=mass - background;
  errsignal=(intSerr/intS)*signal;/*assume relative error is the same as for total integral*/

}

//_________________________________________________________________________

void AliHFMassFitter::Background(Double_t nOfSigma,Double_t &background,Double_t &errbackground) const {
  // Return background integral in mean+- n sigma

  if(fcounter==0) {
    cout<<"Use MassFitter method before Background"<<endl;
    return;
  }

  //functions names
  TString bkgname="funcbkgRecalc";
  TString bkg1name="funcbkg1Recalc";

  TF1 *funcbkg=0;
  if(ftypeOfFit4Sgn == 0) funcbkg=fhistoInvMass->GetFunction(bkgname.Data());
  else funcbkg=fhistoInvMass->GetFunction(bkg1name.Data());
  if(!funcbkg){
    cout<<"AliHFMassFitter::Background() ERROR -> Bkg function not found!"<<endl;
    return;
  }

  Double_t intB,intBerr;

  //relative error evaluation: from final parameters of the fit
  if(ftypeOfFit4Bkg==3 && ftypeOfFit4Sgn == 0) cout<<"No background fit: Bkg relative error evaluation put to zero"<<endl;
  else{
    intB=funcbkg->GetParameter(0);
    intBerr=funcbkg->GetParError(0);
    cout<<"Bkg relative error evaluation: from final parameters of the fit: "<<intBerr/intB<<endl;
  }

  Double_t min=fMass-nOfSigma*fSigmaSgn;
  Double_t max=fMass+nOfSigma*fSigmaSgn;
  
  //relative error evaluation: from histo
   
  intB=fhistoInvMass->Integral(1,fSideBandl)+fhistoInvMass->Integral(fSideBandr,fNbin);
  Double_t sum2=0;
  for(Int_t i=1;i<=fSideBandl;i++){
    sum2+=fhistoInvMass->GetBinError(i)*fhistoInvMass->GetBinError(i);
  }
  for(Int_t i=fSideBandr;i<=fNbin;i++){
    sum2+=fhistoInvMass->GetBinError(i)*fhistoInvMass->GetBinError(i);
  }

  intBerr=TMath::Sqrt(sum2);
  cout<<"Bkg relative error evaluation: from histo: "<<intBerr/intB<<endl;
  
  cout<<"Last estimation of bkg error is used"<<endl;

  //backround +/- error in nsigma
  if (ftypeOfFit4Bkg == 3 && ftypeOfFit4Sgn == 0) {
    background = 0;
    errbackground = 0;
  }
  else{
    background=funcbkg->Integral(min,max)/(Double_t)fhistoInvMass->GetBinWidth(2);
    errbackground=intBerr/intB*background; // assume relative error is the same as for total integral
    //cout<<"integral = "<<funcbkg->Integral(min, max)<<"\tbinW = "<<fhistoInvMass->GetBinWidth(2)<<endl;
  }
  return;

}
//___________________________________________________________________________

void AliHFMassFitter::Background(Double_t min, Double_t max, Double_t &background,Double_t &errbackground) const {
  // Return background integral in a range

  if(fcounter==0) {
    cout<<"Use MassFitter method before Background"<<endl;
    return;
  }

  //functions names
  TString bkgname="funcbkgRecalc";
  TString bkg1name="funcbkg1Recalc";

  TF1 *funcbkg=0;
  if(ftypeOfFit4Sgn == 0) funcbkg=fhistoInvMass->GetFunction(bkgname.Data());
  else funcbkg=fhistoInvMass->GetFunction(bkg1name.Data());
  if(!funcbkg){
    cout<<"AliHFMassFitter::Background() ERROR -> Bkg function not found!"<<endl;
    return;
  }


  Double_t intB,intBerr;

  //relative error evaluation: from final parameters of the fit
  if(ftypeOfFit4Bkg==3 && ftypeOfFit4Sgn == 0) cout<<"No background fit: Bkg relative error evaluation put to zero"<<endl;
  else{
    intB=funcbkg->GetParameter(0);
    intBerr=funcbkg->GetParError(0);
    cout<<"Bkg relative error evaluation: from final parameters of the fit: "<<intBerr/intB<<endl;
  }

  //relative error evaluation: from histo
   
  intB=fhistoInvMass->Integral(1,fSideBandl)+fhistoInvMass->Integral(fSideBandr,fNbin);
  Double_t sum2=0;
  for(Int_t i=1;i<=fSideBandl;i++){
    sum2+=fhistoInvMass->GetBinError(i)*fhistoInvMass->GetBinError(i);
  }
  for(Int_t i=fSideBandr;i<=fNbin;i++){
    sum2+=fhistoInvMass->GetBinError(i)*fhistoInvMass->GetBinError(i);
  }

  intBerr=TMath::Sqrt(sum2);
  cout<<"Bkg relative error evaluation: from histo: "<<intBerr/intB<<endl;
  
  cout<<"Last estimation of bkg error is used"<<endl;

  //backround +/- error in the range
  if (ftypeOfFit4Bkg == 3 && ftypeOfFit4Sgn == 0) {
    background = 0;
    errbackground = 0;
  }
  else{
    background=funcbkg->Integral(min,max)/(Double_t)fhistoInvMass->GetBinWidth(2);
    errbackground=intBerr/intB*background; // assume relative error is the same as for total integral
    //cout<<"integral = "<<funcbkg->Integral(min, max)<<"\tbinW = "<<fhistoInvMass->GetBinWidth(2)<<endl;
  }
  return;

}


//__________________________________________________________________________

void AliHFMassFitter::Significance(Double_t nOfSigma,Double_t &significance,Double_t &errsignificance) const  {
  // Return significance in mean+- n sigma

  Double_t signal,errsignal,background,errbackground;
  Signal(nOfSigma,signal,errsignal);
  Background(nOfSigma,background,errbackground);

  significance =  signal/TMath::Sqrt(signal+background);
  
  errsignificance = TMath::Sqrt(significance*significance/(signal+background)/(signal+background)*(1/4.*errsignal*errsignal+errbackground*errbackground)+significance*significance/signal/signal*errsignal*errsignal);
  
  return;
}

//__________________________________________________________________________

void AliHFMassFitter::Significance(Double_t min, Double_t max, Double_t &significance,Double_t &errsignificance) const {
  // Return significance integral in a range

  Double_t signal,errsignal,background,errbackground;
  Signal(min, max,signal,errsignal);
  Background(min, max,background,errbackground);

  significance =  signal/TMath::Sqrt(signal+background);
  
  errsignificance = TMath::Sqrt(significance*significance/(signal+background)/(signal+background)*(1/4.*errsignal*errsignal+errbackground*errbackground)+significance*significance/signal/signal*errsignal*errsignal);
  
  return;
}



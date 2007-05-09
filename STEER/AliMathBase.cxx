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


///////////////////////////////////////////////////////////////////////////
// Class AliMathBase
// 
// Subset of  matheamtical functions  not included in the TMath
//

///////////////////////////////////////////////////////////////////////////
#include "TMath.h"
#include "AliMathBase.h"
#include "Riostream.h"
#include "TH1F.h"
#include "TF1.h"
#include "TLinearFitter.h"

//
// includes neccessary for test functions
//

#include "TSystem.h"
#include "TRandom.h"
#include "TStopwatch.h"
#include "TTreeStream.h"
 
ClassImp(AliMathBase) // Class implementation to enable ROOT I/O
 
AliMathBase::AliMathBase() : TObject()
{
  //
  // Default constructor
  //
}
///////////////////////////////////////////////////////////////////////////
AliMathBase::~AliMathBase()
{
  //
  // Destructor
  //
}


//_____________________________________________________________________________
void AliMathBase::EvaluateUni(Int_t nvectors, Double_t *data, Double_t &mean
                           , Double_t &sigma, Int_t hh)
{
  //
  // Robust estimator in 1D case MI version - (faster than ROOT version)
  //
  // For the univariate case
  // estimates of location and scatter are returned in mean and sigma parameters
  // the algorithm works on the same principle as in multivariate case -
  // it finds a subset of size hh with smallest sigma, and then returns mean and
  // sigma of this subset
  //

  if (hh==0)
    hh=(nvectors+2)/2;
  Double_t faclts[]={2.6477,2.5092,2.3826,2.2662,2.1587,2.0589,1.9660,1.879,1.7973,1.7203,1.6473};
  Int_t *index=new Int_t[nvectors];
  TMath::Sort(nvectors, data, index, kFALSE);
  
  Int_t    nquant = TMath::Min(Int_t(Double_t(((hh*1./nvectors)-0.5)*40))+1, 11);
  Double_t factor = faclts[TMath::Max(0,nquant-1)];
  
  Double_t sumx  =0;
  Double_t sumx2 =0;
  Int_t    bestindex = -1;
  Double_t bestmean  = 0; 
  Double_t bestsigma = data[index[nvectors-1]]-data[index[0]];   // maximal possible sigma
  for (Int_t i=0; i<hh; i++){
    sumx  += data[index[i]];
    sumx2 += data[index[i]]*data[index[i]];
  }
  
  Double_t norm = 1./Double_t(hh);
  Double_t norm2 = 1./Double_t(hh-1);
  for (Int_t i=hh; i<nvectors; i++){
    Double_t cmean  = sumx*norm;
    Double_t csigma = (sumx2 - hh*cmean*cmean)*norm2;
    if (csigma<bestsigma){
      bestmean  = cmean;
      bestsigma = csigma;
      bestindex = i-hh;
    }
    
    sumx  += data[index[i]]-data[index[i-hh]];
    sumx2 += data[index[i]]*data[index[i]]-data[index[i-hh]]*data[index[i-hh]];
  }
  
  Double_t bstd=factor*TMath::Sqrt(TMath::Abs(bestsigma));
  mean  = bestmean;
  sigma = bstd;
  delete [] index;

}



void AliMathBase::EvaluateUniExternal(Int_t nvectors, Double_t *data, Double_t &mean, Double_t &sigma, Int_t hh,  Float_t externalfactor)
{
  // Modified version of ROOT robust EvaluateUni
  // robust estimator in 1D case MI version
  // added external factor to include precision of external measurement
  // 

  if (hh==0)
    hh=(nvectors+2)/2;
  Double_t faclts[]={2.6477,2.5092,2.3826,2.2662,2.1587,2.0589,1.9660,1.879,1.7973,1.7203,1.6473};
  Int_t *index=new Int_t[nvectors];
  TMath::Sort(nvectors, data, index, kFALSE);
  //
  Int_t    nquant = TMath::Min(Int_t(Double_t(((hh*1./nvectors)-0.5)*40))+1, 11);
  Double_t factor = faclts[0];
  if (nquant>0){
    // fix proper normalization - Anja
    factor = faclts[nquant-1];
  }

  //
  //
  Double_t sumx  =0;
  Double_t sumx2 =0;
  Int_t    bestindex = -1;
  Double_t bestmean  = 0; 
  Double_t bestsigma = -1;
  for (Int_t i=0; i<hh; i++){
    sumx  += data[index[i]];
    sumx2 += data[index[i]]*data[index[i]];
  }
  //   
  Double_t kfactor = 2.*externalfactor - externalfactor*externalfactor;
  Double_t norm = 1./Double_t(hh);
  for (Int_t i=hh; i<nvectors; i++){
    Double_t cmean  = sumx*norm;
    Double_t csigma = (sumx2*norm - cmean*cmean*kfactor);
    if (csigma<bestsigma ||  bestsigma<0){
      bestmean  = cmean;
      bestsigma = csigma;
      bestindex = i-hh;
    }
    //
    //
    sumx  += data[index[i]]-data[index[i-hh]];
    sumx2 += data[index[i]]*data[index[i]]-data[index[i-hh]]*data[index[i-hh]];
  }
  
  Double_t bstd=factor*TMath::Sqrt(TMath::Abs(bestsigma));
  mean  = bestmean;
  sigma = bstd;
  delete [] index;
}


//_____________________________________________________________________________
Int_t AliMathBase::Freq(Int_t n, const Int_t *inlist
                        , Int_t *outlist, Bool_t down)
{    
  //
  //  Sort eleements according occurancy 
  //  The size of output array has is 2*n 
  //

  Int_t * sindexS = new Int_t[n];     // temp array for sorting
  Int_t * sindexF = new Int_t[2*n];   
  for (Int_t i=0;i<n;i++) sindexF[i]=0;
  //
  TMath::Sort(n,inlist, sindexS, down);  
  Int_t last      = inlist[sindexS[0]];
  Int_t val       = last;
  sindexF[0]      = 1;
  sindexF[0+n]    = last;
  Int_t countPos  = 0;
  //
  //  find frequency
  for(Int_t i=1;i<n; i++){
    val = inlist[sindexS[i]];
    if (last == val)   sindexF[countPos]++;
    else{      
      countPos++;
      sindexF[countPos+n] = val;
      sindexF[countPos]++;
      last =val;
    }
  }
  if (last==val) countPos++;
  // sort according frequency
  TMath::Sort(countPos, sindexF, sindexS, kTRUE);
  for (Int_t i=0;i<countPos;i++){
    outlist[2*i  ] = sindexF[sindexS[i]+n];
    outlist[2*i+1] = sindexF[sindexS[i]];
  }
  delete [] sindexS;
  delete [] sindexF;
  
  return countPos;

}

//___AliMathBase__________________________________________________________________________
void AliMathBase::TruncatedMean(TH1F * his, TVectorD *param, Float_t down, Float_t up, Bool_t verbose){
  //
  //
  //
  Int_t nbins    = his->GetNbinsX();
  Float_t nentries = his->GetEntries();
  Float_t sum      =0;
  Float_t mean   = 0;
  Float_t sigma2 = 0;
  Float_t ncumul=0;  
  for (Int_t ibin=1;ibin<nbins; ibin++){
    ncumul+= his->GetBinContent(ibin);
    Float_t fraction = Float_t(ncumul)/Float_t(nentries);
    if (fraction>down && fraction<up){
      sum+=his->GetBinContent(ibin);
      mean+=his->GetBinCenter(ibin)*his->GetBinContent(ibin);
      sigma2+=his->GetBinCenter(ibin)*his->GetBinCenter(ibin)*his->GetBinContent(ibin);      
    }
  }
  mean/=sum;
  sigma2= TMath::Sqrt(TMath::Abs(sigma2/sum-mean*mean));
  if (param){
    (*param)[0] = his->GetMaximum();
    (*param)[1] = mean;
    (*param)[2] = sigma2;
    
  }
  if (verbose)  printf("Mean\t%f\t Sigma2\t%f\n", mean,sigma2);
}

void AliMathBase::LTM(TH1F * his, TVectorD *param , Float_t fraction,  Bool_t verbose){
  //
  // LTM
  //
  Int_t nbins    = his->GetNbinsX();
  Int_t nentries = (Int_t)his->GetEntries();
  Double_t *data  = new Double_t[nentries];
  Int_t npoints=0;
  for (Int_t ibin=1;ibin<nbins; ibin++){
    Float_t entriesI = his->GetBinContent(ibin);
    Float_t xcenter= his->GetBinCenter(ibin);
    for (Int_t ic=0; ic<entriesI; ic++){
      if (npoints<nentries){
	data[npoints]= xcenter;
	npoints++;
      }
    }
  }
  Double_t mean, sigma;
  Int_t npoints2=TMath::Min(Int_t(fraction*Float_t(npoints)),npoints-1);
  npoints2=TMath::Max(Int_t(0.5*Float_t(npoints)),npoints2);
  AliMathBase::EvaluateUni(npoints, data, mean,sigma,npoints2);
  delete [] data;
  if (verbose)  printf("Mean\t%f\t Sigma2\t%f\n", mean,sigma);if (param){
    (*param)[0] = his->GetMaximum();
    (*param)[1] = mean;
    (*param)[2] = sigma;    
  }
}

Double_t  AliMathBase::FitGaus(TH1F* his, TVectorD *param, TMatrixD *matrix, Float_t xmin, Float_t xmax, Bool_t verbose){
  //
  //  Fit histogram with gaussian function
  //  
  //  Prameters:
  //       return value- chi2 - if negative ( not enough points)
  //       his        -  input histogram
  //       param      -  vector with parameters 
  //       xmin, xmax -  range to fit - if xmin=xmax=0 - the full histogram range used
  //  Fitting:
  //  1. Step - make logarithm
  //  2. Linear  fit (parabola) - more robust - always converge
  //  3. In case of small statistic bins are averaged
  //  
  static TLinearFitter fitter(3,"pol2");
  TVectorD  par(3);
  TVectorD  sigma(3);
  TMatrixD mat(3,3);
  if (his->GetMaximum()<4) return -1;  
  if (his->GetEntries()<12) return -1;  
  if (his->GetRMS()<mat.GetTol()) return -1;
  Float_t maxEstimate   = his->GetEntries()*his->GetBinWidth(1)/TMath::Sqrt((TMath::TwoPi()*his->GetRMS()));
  Int_t dsmooth = TMath::Nint(6./TMath::Sqrt(maxEstimate));

  if (maxEstimate<1) return -1;
  Int_t nbins    = his->GetNbinsX();
  Int_t npoints=0;
  //


  if (xmin>=xmax){
    xmin = his->GetXaxis()->GetXmin();
    xmax = his->GetXaxis()->GetXmax();
  }
  for (Int_t iter=0; iter<2; iter++){
    fitter.ClearPoints();
    npoints=0;
    for (Int_t ibin=1;ibin<nbins+1; ibin++){
      Int_t countB=1;
      Float_t entriesI =  his->GetBinContent(ibin);
      for (Int_t delta = -dsmooth; delta<=dsmooth; delta++){
	if (ibin+delta>1 &&ibin+delta<nbins-1){
	  entriesI +=  his->GetBinContent(ibin+delta);
	  countB++;
	}
      }
      entriesI/=countB;
      Double_t xcenter= his->GetBinCenter(ibin);
      if (xcenter<xmin || xcenter>xmax) continue;
      Double_t error=1./TMath::Sqrt(countB);
      Float_t   cont=2;
      if (iter>0){
	if (par[0]+par[1]*xcenter+par[2]*xcenter*xcenter>20) return 0;
	cont = TMath::Exp(par[0]+par[1]*xcenter+par[2]*xcenter*xcenter);
	if (cont>1.) error = 1./TMath::Sqrt(cont*Float_t(countB));
      }
      if (entriesI>1&&cont>1){
	fitter.AddPoint(&xcenter,TMath::Log(Float_t(entriesI)),error);
	npoints++;
      }
    }  
    if (npoints>3){
      fitter.Eval();
      fitter.GetParameters(par);
    }else{
      break;
    }
  }
  if (npoints<=3){
    return -1;
  }
  fitter.GetParameters(par);
  fitter.GetCovarianceMatrix(mat);
  if (TMath::Abs(par[1])<mat.GetTol()) return -1;
  if (TMath::Abs(par[2])<mat.GetTol()) return -1;
  Double_t chi2 = fitter.GetChisquare()/Float_t(npoints);
  //fitter.GetParameters();
  if (!param)  param  = new TVectorD(3);
  if (!matrix) matrix = new TMatrixD(3,3);
  (*param)[1] = par[1]/(-2.*par[2]);
  (*param)[2] = 1./TMath::Sqrt(TMath::Abs(-2.*par[2]));
  (*param)[0] = TMath::Exp(par[0]+ par[1]* (*param)[1] +  par[2]*(*param)[1]*(*param)[1]);
  if (verbose){
    par.Print();
    mat.Print();
    param->Print();
    printf("Chi2=%f\n",chi2);
    TF1 * f1= new TF1("f1","[0]*exp(-(x-[1])^2/(2*[2]*[2]))",his->GetXaxis()->GetXmin(),his->GetXaxis()->GetXmax());
    f1->SetParameter(0, (*param)[0]);
    f1->SetParameter(1, (*param)[1]);
    f1->SetParameter(2, (*param)[2]);    
    f1->Draw("same");
  }
  return chi2;
}


Double_t  AliMathBase::FitGaus(Int_t size, Float_t *arr, Float_t firstBinX, Float_t binWidth, TVectorD *param, TMatrixD *matrix, Float_t xmin, Float_t xmax, Bool_t verbose){
  //
  //  Fit histogram with gaussian function
  //  
  //  Prameters:
  //       return value- chi2 - if negative ( not enough points)
  //       his        -  input histogram
  //       param      -  vector with parameters 
  //       xmin, xmax -  range to fit - if xmin=xmax=0 - the full histogram range used
  //  Fitting:
  //  1. Step - make logarithm
  //  2. Linear  fit (parabola) - more robust - always converge
  //  3. In case of small statistic bins are averaged
  //  
  static TLinearFitter fitter(3,"pol2");
  static TMatrixD mat(3,3);
  static Double_t kTol = mat.GetTol();
  fitter.StoreData(kFALSE);
  fitter.ClearPoints();
  TVectorD  par(3);
  TVectorD  sigma(3);
  TMatrixD A(3,3);
  TMatrixD b(3,1);
  Float_t rms = TMath::RMS(size,arr);
  Float_t max = TMath::MaxElement(size,arr);

  Float_t meanCOG = 0;
  Float_t rms2COG = 0;
  Float_t sumCOG  = 0;

  Float_t entries = 0;
  Int_t nfilled=0;

  for (Int_t i=0; i<size; i++){
      entries+=arr[i];
      if (arr[i]>0) nfilled++;
  }

  if (max<4) return -1;
  if (entries<12) return -1;
  if (rms<kTol) return -1;

  Int_t npoints=0;
  //


  if (xmin>=xmax){
    xmin = firstBinX;
    xmax = firstBinX+(size-1)*binWidth;
  }
  //
  for (Int_t ibin=0;ibin<size; ibin++){    
    Float_t entriesI = arr[ibin];
    if (entriesI>1){
      Double_t xcenter = firstBinX+ibin*binWidth;
      if (xcenter<xmin || xcenter>xmax) continue;
      
      Float_t error    = 1./TMath::Sqrt(entriesI);
      Float_t val = TMath::Log(Float_t(entriesI));
      fitter.AddPoint(&xcenter,val,error);
      npoints++;
    }
  }
  
  if (npoints<=3){
    for (Int_t ibin=0;ibin<size; ibin++){      
      Float_t entriesI = arr[ibin];
      if (entriesI>1){
	Double_t xcenter = firstBinX+ibin*binWidth;
	if (xcenter<xmin || xcenter>xmax) continue;	
	Float_t val = TMath::Log(Float_t(entriesI));
	// if less than 3 point the fitter will crash!
	// for three points calculate the parameters analytically
	// for one and two points use center of gravity
	A(npoints,0)=1;
	A(npoints,1)=xcenter;
	A(npoints,2)=xcenter*xcenter;
	b(npoints,0)=val;
	meanCOG+=xcenter*val;
	rms2COG +=xcenter*val*xcenter*val;
	sumCOG +=val;
	npoints++;
      }
    } 
  }


  
  Double_t chi2 = 0;
  if (npoints>=3){
      if ( npoints == 3 ){
	  //analytic calculation of the parameters for three points
	  A.Invert();
	  TMatrixD res(1,3);
	  res.Mult(A,b);
	  par[0]=res(0,0);
	  par[1]=res(0,1);
	  par[2]=res(0,2);
          chi2 = -3.;
      } else {
          // use fitter for more than three points
	  fitter.Eval();
	  fitter.GetParameters(par);
	  fitter.GetCovarianceMatrix(mat);
	  chi2 = fitter.GetChisquare()/Float_t(npoints);
      }
      if (TMath::Abs(par[1])<kTol) return -1;
      if (TMath::Abs(par[2])<kTol) return -1;

      if (!param)  param  = new TVectorD(3);
      if (!matrix) matrix = new TMatrixD(3,3);  // !!!!might be a memory leek. use dummy matrix pointer to call this function!

      (*param)[1] = par[1]/(-2.*par[2]);
      (*param)[2] = 1./TMath::Sqrt(TMath::Abs(-2.*par[2]));
      (*param)[0] = TMath::Exp(par[0]+ par[1]* (*param)[1] +  par[2]*(*param)[1]*(*param)[1]);
      if (verbose){
	  par.Print();
	  mat.Print();
	  param->Print();
	  printf("Chi2=%f\n",chi2);
	  TF1 * f1= new TF1("f1","[0]*exp(-(x-[1])^2/(2*[2]*[2]))",xmin,xmax);
	  f1->SetParameter(0, (*param)[0]);
	  f1->SetParameter(1, (*param)[1]);
	  f1->SetParameter(2, (*param)[2]);
	  f1->Draw("same");
      }
      return chi2;
  }

  if (npoints == 2){
      //use center of gravity for 2 points
      meanCOG/=sumCOG;
      rms2COG /=sumCOG;
      (*param)[0] = max;
      (*param)[1] = meanCOG;
      (*param)[2] = TMath::Sqrt(TMath::Abs(meanCOG*meanCOG-rms2COG));
      chi2=-2.;
  }
  if ( npoints == 1 ){
      (*param)[0] = max;
      (*param)[1] = meanCOG;
      (*param)[2] = binWidth/TMath::Sqrt(12);
      chi2=-1.;
  }
  return chi2;

}



///////////////////////////////////////////////////////////////
//////////////         TEST functions /////////////////////////
///////////////////////////////////////////////////////////////





void AliMathBase::TestGausFit(Int_t nhistos){
  //
  // Test performance of the parabolic - gaussian fit - compare it with 
  // ROOT gauss fit
  //  nhistos - number of histograms to be used for test
  //
  TTreeSRedirector *pcstream = new TTreeSRedirector("fitdebug.root");
  
  Float_t  *xTrue = new Float_t[nhistos];
  Float_t  *sTrue = new Float_t[nhistos];
  TVectorD **par1  = new TVectorD*[nhistos];
  TVectorD **par2  = new TVectorD*[nhistos];
  TMatrixD dummy(3,3);
  
  
  TH1F **h1f = new TH1F*[nhistos];
  TF1  *myg = new TF1("myg","gaus");
  TF1  *fit = new TF1("fit","gaus");
  gRandom->SetSeed(0);
  
  //init
  for (Int_t i=0;i<nhistos; i++){
    par1[i] = new TVectorD(3);
    par2[i] = new TVectorD(3);
    h1f[i]  = new TH1F(Form("h1f%d",i),Form("h1f%d",i),20,-10,10);
    xTrue[i]= gRandom->Rndm();
    gSystem->Sleep(2);
    sTrue[i]= .75+gRandom->Rndm()*.5;
    myg->SetParameters(1,xTrue[i],sTrue[i]);
    h1f[i]->FillRandom("myg");
  }
  
  TStopwatch s;
  s.Start();
  //standard gaus fit
  for (Int_t i=0; i<nhistos; i++){
    h1f[i]->Fit(fit,"0q");
    (*par1[i])(0) = fit->GetParameter(0);
    (*par1[i])(1) = fit->GetParameter(1);
    (*par1[i])(2) = fit->GetParameter(2);
  }
  s.Stop();
  printf("Gaussian fit\t");
  s.Print();
  
  s.Start();
  //AliMathBase gaus fit
  for (Int_t i=0; i<nhistos; i++){
    AliMathBase::FitGaus(20,h1f[i]->GetArray()+1,-9.5,1,par2[i],&dummy);
  }
  
  s.Stop();
  printf("Parabolic fit\t");
  s.Print();
  //write stream
  for (Int_t i=0;i<nhistos; i++){
    Float_t xt  = xTrue[i];
    Float_t st  = sTrue[i];
    (*pcstream)<<"data"
	       <<"xTrue="<<xt
	       <<"sTrue="<<st
	       <<"pg.="<<(par1[i])
	       <<"pa.="<<(par2[i])
	       <<"\n";
  }    
  //delete pointers
  for (Int_t i=0;i<nhistos; i++){
    delete par1[i];
    delete par2[i];
    delete h1f[i];
  }
  delete pcstream;
  delete []h1f;
  delete []xTrue;
  delete []sTrue;
  //
  delete []par1;
  delete []par2;

}





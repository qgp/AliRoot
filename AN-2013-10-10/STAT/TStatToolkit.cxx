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
// Class TStatToolkit
// 
// Subset of  matheamtical functions  not included in the TMath
//

///////////////////////////////////////////////////////////////////////////
#include "TMath.h"
#include "Riostream.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3.h"
#include "TF1.h"
#include "TTree.h"
#include "TChain.h"
#include "TObjString.h"
#include "TLinearFitter.h"
#include "TGraph2D.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TCut.h"

//
// includes neccessary for test functions
//
#include "TSystem.h"
#include "TRandom.h"
#include "TStopwatch.h"
#include "TTreeStream.h"

#include "TStatToolkit.h"


using std::cout;
using std::cerr;
using std::endl;

 
ClassImp(TStatToolkit) // Class implementation to enable ROOT I/O
 
TStatToolkit::TStatToolkit() : TObject()
{
  //
  // Default constructor
  //
}
///////////////////////////////////////////////////////////////////////////
TStatToolkit::~TStatToolkit()
{
  //
  // Destructor
  //
}


//_____________________________________________________________________________
void TStatToolkit::EvaluateUni(Int_t nvectors, Double_t *data, Double_t &mean
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
  Double_t bestsigma = (data[index[nvectors-1]]-data[index[0]]+1.);   // maximal possible sigma
  bestsigma *=bestsigma;

  for (Int_t i=0; i<hh; i++){
    sumx  += data[index[i]];
    sumx2 += data[index[i]]*data[index[i]];
  }
  
  Double_t norm = 1./Double_t(hh);
  Double_t norm2 = (hh-1)>0 ? 1./Double_t(hh-1):1;
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



void TStatToolkit::EvaluateUniExternal(Int_t nvectors, Double_t *data, Double_t &mean, Double_t &sigma, Int_t hh,  Float_t externalfactor)
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
Int_t TStatToolkit::Freq(Int_t n, const Int_t *inlist
                        , Int_t *outlist, Bool_t down)
{    
  //
  //  Sort eleements according occurancy 
  //  The size of output array has is 2*n 
  //

  Int_t * sindexS = new Int_t[n];     // temp array for sorting
  Int_t * sindexF = new Int_t[2*n];   
  for (Int_t i=0;i<n;i++) sindexS[i]=0;
  for (Int_t i=0;i<2*n;i++) sindexF[i]=0;
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

//___TStatToolkit__________________________________________________________________________
void TStatToolkit::TruncatedMean(const TH1 * his, TVectorD *param, Float_t down, Float_t up, Bool_t verbose){
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

void TStatToolkit::LTM(TH1F * his, TVectorD *param , Float_t fraction,  Bool_t verbose){
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
  TStatToolkit::EvaluateUni(npoints, data, mean,sigma,npoints2);
  delete [] data;
  if (verbose)  printf("Mean\t%f\t Sigma2\t%f\n", mean,sigma);if (param){
    (*param)[0] = his->GetMaximum();
    (*param)[1] = mean;
    (*param)[2] = sigma;    
  }
}

Double_t  TStatToolkit::FitGaus(TH1* his, TVectorD *param, TMatrixD */*matrix*/, Float_t xmin, Float_t xmax, Bool_t verbose){
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
  // if (!matrix) matrix = new TMatrixD(3,3); // Covariance matrix to be implemented
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

Double_t  TStatToolkit::FitGaus(Float_t *arr, Int_t nBins, Float_t xMin, Float_t xMax, TVectorD *param, TMatrixD */*matrix*/, Bool_t verbose){
  //
  //  Fit histogram with gaussian function
  //  
  //  Prameters:
  //     nbins: size of the array and number of histogram bins
  //     xMin, xMax: histogram range
  //     param: paramters of the fit (0-Constant, 1-Mean, 2-Sigma)
  //     matrix: covariance matrix -- not implemented yet, pass dummy matrix!!!
  //
  //  Return values:
  //    >0: the chi2 returned by TLinearFitter
  //    -3: only three points have been used for the calculation - no fitter was used
  //    -2: only two points have been used for the calculation - center of gravity was uesed for calculation
  //    -1: only one point has been used for the calculation - center of gravity was uesed for calculation
  //    -4: invalid result!!
  //
  //  Fitting:
  //  1. Step - make logarithm
  //  2. Linear  fit (parabola) - more robust - always converge
  //  
  static TLinearFitter fitter(3,"pol2");
  static TMatrixD mat(3,3);
  static Double_t kTol = mat.GetTol();
  fitter.StoreData(kFALSE);
  fitter.ClearPoints();
  TVectorD  par(3);
  TVectorD  sigma(3);
  TMatrixD matA(3,3);
  TMatrixD b(3,1);
  Float_t rms = TMath::RMS(nBins,arr);
  Float_t max = TMath::MaxElement(nBins,arr);
  Float_t binWidth = (xMax-xMin)/(Float_t)nBins;

  Float_t meanCOG = 0;
  Float_t rms2COG = 0;
  Float_t sumCOG  = 0;

  Float_t entries = 0;
  Int_t nfilled=0;

  for (Int_t i=0; i<nBins; i++){
      entries+=arr[i];
      if (arr[i]>0) nfilled++;
  }

  if (max<4) return -4;
  if (entries<12) return -4;
  if (rms<kTol) return -4;

  Int_t npoints=0;
  //

  //
  for (Int_t ibin=0;ibin<nBins; ibin++){
      Float_t entriesI = arr[ibin];
    if (entriesI>1){
      Double_t xcenter = xMin+(ibin+0.5)*binWidth;
      
      Float_t error    = 1./TMath::Sqrt(entriesI);
      Float_t val = TMath::Log(Float_t(entriesI));
      fitter.AddPoint(&xcenter,val,error);
      if (npoints<3){
	  matA(npoints,0)=1;
	  matA(npoints,1)=xcenter;
	  matA(npoints,2)=xcenter*xcenter;
	  b(npoints,0)=val;
	  meanCOG+=xcenter*entriesI;
	  rms2COG +=xcenter*entriesI*xcenter;
	  sumCOG +=entriesI;
      }
      npoints++;
    }
  }

  
  Double_t chi2 = 0;
  if (npoints>=3){
      if ( npoints == 3 ){
	  //analytic calculation of the parameters for three points
	  matA.Invert();
	  TMatrixD res(1,3);
	  res.Mult(matA,b);
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
      if (TMath::Abs(par[1])<kTol) return -4;
      if (TMath::Abs(par[2])<kTol) return -4;

      if (!param)  param  = new TVectorD(3);
      //if (!matrix) matrix = new TMatrixD(3,3);  // !!!!might be a memory leek. use dummy matrix pointer to call this function! // Covariance matrix to be implemented

      (*param)[1] = par[1]/(-2.*par[2]);
      (*param)[2] = 1./TMath::Sqrt(TMath::Abs(-2.*par[2]));
      Double_t lnparam0 = par[0]+ par[1]* (*param)[1] +  par[2]*(*param)[1]*(*param)[1];
      if ( lnparam0>307 ) return -4;
      (*param)[0] = TMath::Exp(lnparam0);
      if (verbose){
	  par.Print();
	  mat.Print();
	  param->Print();
	  printf("Chi2=%f\n",chi2);
	  TF1 * f1= new TF1("f1","[0]*exp(-(x-[1])^2/(2*[2]*[2]))",xMin,xMax);
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
      meanCOG/=sumCOG;
      (*param)[0] = max;
      (*param)[1] = meanCOG;
      (*param)[2] = binWidth/TMath::Sqrt(12);
      chi2=-1.;
  }
  return chi2;

}


Float_t TStatToolkit::GetCOG(const Short_t *arr, Int_t nBins, Float_t xMin, Float_t xMax, Float_t *rms, Float_t *sum)
{
    //
    //  calculate center of gravity rms and sum for array 'arr' with nBins an a x range xMin to xMax
    //  return COG; in case of failure return xMin
    //
    Float_t meanCOG = 0;
    Float_t rms2COG = 0;
    Float_t sumCOG  = 0;
    Int_t npoints   = 0;

    Float_t binWidth = (xMax-xMin)/(Float_t)nBins;

    for (Int_t ibin=0; ibin<nBins; ibin++){
	Float_t entriesI = (Float_t)arr[ibin];
	Double_t xcenter = xMin+(ibin+0.5)*binWidth;
	if ( entriesI>0 ){
	    meanCOG += xcenter*entriesI;
	    rms2COG += xcenter*entriesI*xcenter;
	    sumCOG  += entriesI;
	    npoints++;
	}
    }
    if ( sumCOG == 0 ) return xMin;
    meanCOG/=sumCOG;

    if ( rms ){
	rms2COG /=sumCOG;
	(*rms) = TMath::Sqrt(TMath::Abs(meanCOG*meanCOG-rms2COG));
	if ( npoints == 1 ) (*rms) = binWidth/TMath::Sqrt(12);
    }

    if ( sum )
        (*sum) = sumCOG;

    return meanCOG;
}



///////////////////////////////////////////////////////////////
//////////////         TEST functions /////////////////////////
///////////////////////////////////////////////////////////////





void TStatToolkit::TestGausFit(Int_t nhistos){
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
  //TStatToolkit gaus fit
  for (Int_t i=0; i<nhistos; i++){
    TStatToolkit::FitGaus(h1f[i]->GetArray()+1,h1f[i]->GetNbinsX(),h1f[i]->GetXaxis()->GetXmin(),h1f[i]->GetXaxis()->GetXmax(),par2[i],&dummy);
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



TGraph2D * TStatToolkit::MakeStat2D(TH3 * his, Int_t delta0, Int_t delta1, Int_t type){
  //
  //
  //
  // delta - number of bins to integrate
  // type - 0 - mean value

  TAxis * xaxis  = his->GetXaxis();
  TAxis * yaxis  = his->GetYaxis();
  //  TAxis * zaxis  = his->GetZaxis();
  Int_t   nbinx  = xaxis->GetNbins();
  Int_t   nbiny  = yaxis->GetNbins();
  char name[1000];
  Int_t icount=0;
  TGraph2D  *graph = new TGraph2D(nbinx*nbiny);
  TF1 f1("f1","gaus");
  for (Int_t ix=0; ix<nbinx;ix++)
    for (Int_t iy=0; iy<nbiny;iy++){
      Float_t xcenter = xaxis->GetBinCenter(ix); 
      Float_t ycenter = yaxis->GetBinCenter(iy); 
      snprintf(name,1000,"%s_%d_%d",his->GetName(), ix,iy);
      TH1 *projection = his->ProjectionZ(name,ix-delta0,ix+delta0,iy-delta1,iy+delta1);
      Float_t stat= 0;
      if (type==0) stat = projection->GetMean();
      if (type==1) stat = projection->GetRMS();
      if (type==2 || type==3){
	TVectorD vec(3);
	TStatToolkit::LTM((TH1F*)projection,&vec,0.7);
	if (type==2) stat= vec[1];
	if (type==3) stat= vec[0];	
      }
      if (type==4|| type==5){
	projection->Fit(&f1);
	if (type==4) stat= f1.GetParameter(1);
	if (type==5) stat= f1.GetParameter(2);
      }
      //printf("%d\t%f\t%f\t%f\n", icount,xcenter, ycenter, stat);
      graph->SetPoint(icount,xcenter, ycenter, stat);
      icount++;
    }
  return graph;
}

TGraph * TStatToolkit::MakeStat1D(TH3 * his, Int_t delta1, Int_t type){
  //
  //
  //
  // delta - number of bins to integrate
  // type - 0 - mean value

  TAxis * xaxis  = his->GetXaxis();
  TAxis * yaxis  = his->GetYaxis();
  //  TAxis * zaxis  = his->GetZaxis();
  Int_t   nbinx  = xaxis->GetNbins();
  Int_t   nbiny  = yaxis->GetNbins();
  char name[1000];
  Int_t icount=0;
  TGraph  *graph = new TGraph(nbinx);
  TF1 f1("f1","gaus");
  for (Int_t ix=0; ix<nbinx;ix++){
    Float_t xcenter = xaxis->GetBinCenter(ix); 
    //    Float_t ycenter = yaxis->GetBinCenter(iy); 
    snprintf(name,1000,"%s_%d",his->GetName(), ix);
    TH1 *projection = his->ProjectionZ(name,ix-delta1,ix+delta1,0,nbiny);
    Float_t stat= 0;
    if (type==0) stat = projection->GetMean();
    if (type==1) stat = projection->GetRMS();
    if (type==2 || type==3){
      TVectorD vec(3);
	TStatToolkit::LTM((TH1F*)projection,&vec,0.7);
	if (type==2) stat= vec[1];
	if (type==3) stat= vec[0];	
    }
    if (type==4|| type==5){
      projection->Fit(&f1);
      if (type==4) stat= f1.GetParameter(1);
      if (type==5) stat= f1.GetParameter(2);
    }
      //printf("%d\t%f\t%f\t%f\n", icount,xcenter, ycenter, stat);
    graph->SetPoint(icount,xcenter, stat);
    icount++;
  }
  return graph;
}





TString* TStatToolkit::FitPlane(TTree *tree, const char* drawCommand, const char* formula, const char* cuts, Double_t & chi2, Int_t &npoints, TVectorD &fitParam, TMatrixD &covMatrix, Float_t frac, Int_t start, Int_t stop,Bool_t fix0){
   //
   // fit an arbitrary function, specified by formula into the data, specified by drawCommand and cuts
   // returns chi2, fitParam and covMatrix
   // returns TString with fitted formula
   //

   TString formulaStr(formula); 
   TString drawStr(drawCommand);
   TString cutStr(cuts);
   TString ferr("1");

   TString strVal(drawCommand);
   if (strVal.Contains(":")){
     TObjArray* valTokens = strVal.Tokenize(":");
     drawStr = valTokens->At(0)->GetName();
     ferr       = valTokens->At(1)->GetName();     
     delete valTokens;
   }

      
   formulaStr.ReplaceAll("++", "~");
   TObjArray* formulaTokens = formulaStr.Tokenize("~"); 
   Int_t dim = formulaTokens->GetEntriesFast();
   
   fitParam.ResizeTo(dim);
   covMatrix.ResizeTo(dim,dim);
   
   TLinearFitter* fitter = new TLinearFitter(dim+1, Form("hyp%d",dim));
   fitter->StoreData(kTRUE);   
   fitter->ClearPoints();
   
   Int_t entries = tree->Draw(drawStr.Data(), cutStr.Data(), "goff",  stop-start, start);
   if (entries == -1) {
     delete formulaTokens;
     return new TString("An ERROR has occured during fitting!");
   }
   Double_t **values = new Double_t*[dim+1] ;
   for (Int_t i=0; i<dim+1; i++) values[i]=NULL; 
   //
   entries = tree->Draw(ferr.Data(), cutStr.Data(), "goff",  stop-start, start);
   if (entries == -1) {
     delete formulaTokens;
     delete []values;
     return new TString("An ERROR has occured during fitting!");
   }
   Double_t *errors = new Double_t[entries];
   memcpy(errors,  tree->GetV1(), entries*sizeof(Double_t));
   
   for (Int_t i = 0; i < dim + 1; i++){
      Int_t centries = 0;
      if (i < dim) centries = tree->Draw(((TObjString*)formulaTokens->At(i))->GetName(), cutStr.Data(), "goff", stop-start,start);
      else  centries = tree->Draw(drawStr.Data(), cutStr.Data(), "goff", stop-start,start);
      
      if (entries != centries) {
	delete []errors;
	delete []values;
	return new TString("An ERROR has occured during fitting!");
      }
      values[i] = new Double_t[entries];
      memcpy(values[i],  tree->GetV1(), entries*sizeof(Double_t)); 
   }
   
   // add points to the fitter
   for (Int_t i = 0; i < entries; i++){
      Double_t x[1000];
      for (Int_t j=0; j<dim;j++) x[j]=values[j][i];
      fitter->AddPoint(x, values[dim][i], errors[i]);
   }

   fitter->Eval();
   if (frac>0.5 && frac<1){
     fitter->EvalRobust(frac);
   }else{
     if (fix0) {
       fitter->FixParameter(0,0);
       fitter->Eval();     
     }
   }
   fitter->GetParameters(fitParam);
   fitter->GetCovarianceMatrix(covMatrix);
   chi2 = fitter->GetChisquare();
   npoints = entries;   
   TString *preturnFormula = new TString(Form("( %f+",fitParam[0])), &returnFormula = *preturnFormula; 
   
   for (Int_t iparam = 0; iparam < dim; iparam++) {
     returnFormula.Append(Form("%s*(%f)",((TObjString*)formulaTokens->At(iparam))->GetName(),fitParam[iparam+1]));
     if (iparam < dim-1) returnFormula.Append("+");
   }
   returnFormula.Append(" )");
   
   
   for (Int_t j=0; j<dim+1;j++) delete [] values[j];


   delete formulaTokens;
   delete fitter;
   delete[] values;
   delete[] errors;
   return preturnFormula;
}

TString* TStatToolkit::FitPlaneConstrain(TTree *tree, const char* drawCommand, const char* formula, const char* cuts, Double_t & chi2, Int_t &npoints, TVectorD &fitParam, TMatrixD &covMatrix, Float_t frac, Int_t start, Int_t stop,Double_t constrain){
   //
   // fit an arbitrary function, specified by formula into the data, specified by drawCommand and cuts
   // returns chi2, fitParam and covMatrix
   // returns TString with fitted formula
   //

   TString formulaStr(formula); 
   TString drawStr(drawCommand);
   TString cutStr(cuts);
   TString ferr("1");

   TString strVal(drawCommand);
   if (strVal.Contains(":")){
     TObjArray* valTokens = strVal.Tokenize(":");
     drawStr = valTokens->At(0)->GetName();
     ferr       = valTokens->At(1)->GetName();     
     delete valTokens;
   }

      
   formulaStr.ReplaceAll("++", "~");
   TObjArray* formulaTokens = formulaStr.Tokenize("~"); 
   Int_t dim = formulaTokens->GetEntriesFast();
   
   fitParam.ResizeTo(dim);
   covMatrix.ResizeTo(dim,dim);
   
   TLinearFitter* fitter = new TLinearFitter(dim+1, Form("hyp%d",dim));
   fitter->StoreData(kTRUE);   
   fitter->ClearPoints();
   
   Int_t entries = tree->Draw(drawStr.Data(), cutStr.Data(), "goff",  stop-start, start);
   if (entries == -1) {
     delete formulaTokens;
     return new TString("An ERROR has occured during fitting!");
   }
   Double_t **values = new Double_t*[dim+1] ; 
   for (Int_t i=0; i<dim+1; i++) values[i]=NULL; 
   //
   entries = tree->Draw(ferr.Data(), cutStr.Data(), "goff",  stop-start, start);
   if (entries == -1) {
     delete formulaTokens;
     delete [] values;
     return new TString("An ERROR has occured during fitting!");
   }
   Double_t *errors = new Double_t[entries];
   memcpy(errors,  tree->GetV1(), entries*sizeof(Double_t));
   
   for (Int_t i = 0; i < dim + 1; i++){
      Int_t centries = 0;
      if (i < dim) centries = tree->Draw(((TObjString*)formulaTokens->At(i))->GetName(), cutStr.Data(), "goff", stop-start,start);
      else  centries = tree->Draw(drawStr.Data(), cutStr.Data(), "goff", stop-start,start);
      
      if (entries != centries) {
	delete []errors;
	delete []values;
	delete formulaTokens;
	return new TString("An ERROR has occured during fitting!");
      }
      values[i] = new Double_t[entries];
      memcpy(values[i],  tree->GetV1(), entries*sizeof(Double_t)); 
   }
   
   // add points to the fitter
   for (Int_t i = 0; i < entries; i++){
      Double_t x[1000];
      for (Int_t j=0; j<dim;j++) x[j]=values[j][i];
      fitter->AddPoint(x, values[dim][i], errors[i]);
   }
   if (constrain>0){
     for (Int_t i = 0; i < dim; i++){
       Double_t x[1000];
       for (Int_t j=0; j<dim;j++) if (i!=j) x[j]=0;
       x[i]=1.;
       fitter->AddPoint(x, 0, constrain);
     }
   }


   fitter->Eval();
   if (frac>0.5 && frac<1){
     fitter->EvalRobust(frac);   
   }
   fitter->GetParameters(fitParam);
   fitter->GetCovarianceMatrix(covMatrix);
   chi2 = fitter->GetChisquare();
   npoints = entries;
   
   TString *preturnFormula = new TString(Form("( %f+",fitParam[0])), &returnFormula = *preturnFormula; 
   
   for (Int_t iparam = 0; iparam < dim; iparam++) {
     returnFormula.Append(Form("%s*(%f)",((TObjString*)formulaTokens->At(iparam))->GetName(),fitParam[iparam+1]));
     if (iparam < dim-1) returnFormula.Append("+");
   }
   returnFormula.Append(" )");
   
   for (Int_t j=0; j<dim+1;j++) delete [] values[j];
   


   delete formulaTokens;
   delete fitter;
   delete[] values;
   delete[] errors;
   return preturnFormula;
}



TString* TStatToolkit::FitPlaneFixed(TTree *tree, const char* drawCommand, const char* formula, const char* cuts, Double_t & chi2, Int_t &npoints, TVectorD &fitParam, TMatrixD &covMatrix, Float_t frac, Int_t start, Int_t stop){
   //
   // fit an arbitrary function, specified by formula into the data, specified by drawCommand and cuts
   // returns chi2, fitParam and covMatrix
   // returns TString with fitted formula
   //

   TString formulaStr(formula); 
   TString drawStr(drawCommand);
   TString cutStr(cuts);
   TString ferr("1");

   TString strVal(drawCommand);
   if (strVal.Contains(":")){
     TObjArray* valTokens = strVal.Tokenize(":");
     drawStr = valTokens->At(0)->GetName();
     ferr       = valTokens->At(1)->GetName();
     delete valTokens;
   }

      
   formulaStr.ReplaceAll("++", "~");
   TObjArray* formulaTokens = formulaStr.Tokenize("~"); 
   Int_t dim = formulaTokens->GetEntriesFast();
   
   fitParam.ResizeTo(dim);
   covMatrix.ResizeTo(dim,dim);
   TString fitString="x0";
   for (Int_t i=1; i<dim; i++) fitString+=Form("++x%d",i);     
   TLinearFitter* fitter = new TLinearFitter(dim, fitString.Data());
   fitter->StoreData(kTRUE);   
   fitter->ClearPoints();
   
   Int_t entries = tree->Draw(drawStr.Data(), cutStr.Data(), "goff",  stop-start, start);
   if (entries == -1) {
     delete formulaTokens;
     return new TString("An ERROR has occured during fitting!");
   }
   Double_t **values = new Double_t*[dim+1] ; 
   for (Int_t i=0; i<dim+1; i++) values[i]=NULL; 
   //
   entries = tree->Draw(ferr.Data(), cutStr.Data(), "goff",  stop-start, start);
   if (entries == -1) {
     delete []values;
     delete formulaTokens;
     return new TString("An ERROR has occured during fitting!");
   }
   Double_t *errors = new Double_t[entries];
   memcpy(errors,  tree->GetV1(), entries*sizeof(Double_t));
   
   for (Int_t i = 0; i < dim + 1; i++){
      Int_t centries = 0;
      if (i < dim) centries = tree->Draw(((TObjString*)formulaTokens->At(i))->GetName(), cutStr.Data(), "goff", stop-start,start);
      else  centries = tree->Draw(drawStr.Data(), cutStr.Data(), "goff", stop-start,start);
      
      if (entries != centries) {
	delete []errors;
	delete []values;
	delete formulaTokens;
	return new TString("An ERROR has occured during fitting!");
      }
      values[i] = new Double_t[entries];
      memcpy(values[i],  tree->GetV1(), entries*sizeof(Double_t)); 
   }
   
   // add points to the fitter
   for (Int_t i = 0; i < entries; i++){
      Double_t x[1000];
      for (Int_t j=0; j<dim;j++) x[j]=values[j][i];
      fitter->AddPoint(x, values[dim][i], errors[i]);
   }

   fitter->Eval();
   if (frac>0.5 && frac<1){
     fitter->EvalRobust(frac);
   }
   fitter->GetParameters(fitParam);
   fitter->GetCovarianceMatrix(covMatrix);
   chi2 = fitter->GetChisquare();
   npoints = entries;
   
   TString *preturnFormula = new TString("("), &returnFormula = *preturnFormula; 
   
   for (Int_t iparam = 0; iparam < dim; iparam++) {
     returnFormula.Append(Form("%s*(%f)",((TObjString*)formulaTokens->At(iparam))->GetName(),fitParam[iparam]));
     if (iparam < dim-1) returnFormula.Append("+");
   }
   returnFormula.Append(" )");
   
   
   for (Int_t j=0; j<dim+1;j++) delete [] values[j];
   
   delete formulaTokens;
   delete fitter;
   delete[] values;
   delete[] errors;
   return preturnFormula;
}





Int_t TStatToolkit::GetFitIndex(const TString fString, const TString subString){
  //
  // fitString - ++ separated list of fits
  // substring - ++ separated list of the requiered substrings
  //
  // return the last occurance of substring in fit string
  // 
  TObjArray *arrFit = fString.Tokenize("++");
  TObjArray *arrSub = subString.Tokenize("++");
  Int_t index=-1;
  for (Int_t i=0; i<arrFit->GetEntries(); i++){
    Bool_t isOK=kTRUE;
    TString str =arrFit->At(i)->GetName();
    for (Int_t isub=0; isub<arrSub->GetEntries(); isub++){
      if (str.Contains(arrSub->At(isub)->GetName())==0) isOK=kFALSE;
    }
    if (isOK) index=i;
  }
  delete arrFit;
  delete arrSub;
  return index;
}


TString  TStatToolkit::FilterFit(const TString &input, const TString filter, TVectorD &param, TMatrixD & covar){
  //
  // Filter fit expression make sub-fit
  //
  TObjArray *array0= input.Tokenize("++");
  TObjArray *array1= filter.Tokenize("++");
  //TString *presult=new TString("(0");
  TString result="(0.0";
  for (Int_t i=0; i<array0->GetEntries(); i++){
    Bool_t isOK=kTRUE;
    TString str(array0->At(i)->GetName());
    for (Int_t j=0; j<array1->GetEntries(); j++){
      if (str.Contains(array1->At(j)->GetName())==0) isOK=kFALSE;      
    }
    if (isOK) {
      result+="+"+str;
      result+=Form("*(%f)",param[i+1]);
      printf("%f\t%f\t%s\n",param[i+1], TMath::Sqrt(covar(i+1,i+1)),str.Data());    
    }
  }
  result+="-0.)";
  delete array0;
  delete array1;
  return result;
}

void TStatToolkit::Update1D(Double_t delta, Double_t sigma, Int_t s1, TMatrixD &vecXk, TMatrixD &covXk){
  //
  // Update parameters and covariance - with one measurement
  // Input:
  // vecXk - input vector - Updated in function 
  // covXk - covariance matrix - Updated in function
  // delta, sigma, s1 - new measurement, rms of new measurement and the index of measurement
  const Int_t knMeas=1;
  Int_t knElem=vecXk.GetNrows();
 
  TMatrixD mat1(knElem,knElem);            // update covariance matrix
  TMatrixD matHk(1,knElem);        // vector to mesurement
  TMatrixD vecYk(knMeas,1);        // Innovation or measurement residual
  TMatrixD matHkT(knElem,knMeas);  // helper matrix Hk transpose
  TMatrixD matSk(knMeas,knMeas);   // Innovation (or residual) covariance
  TMatrixD matKk(knElem,knMeas);   // Optimal Kalman gain
  TMatrixD covXk2(knElem,knElem);  // helper matrix
  TMatrixD covXk3(knElem,knElem);  // helper matrix
  TMatrixD vecZk(1,1);
  TMatrixD measR(1,1);
  vecZk(0,0)=delta;
  measR(0,0)=sigma*sigma;
  //
  // reset matHk
  for (Int_t iel=0;iel<knElem;iel++) 
    for (Int_t ip=0;ip<knMeas;ip++) matHk(ip,iel)=0; 
  //mat1
  for (Int_t iel=0;iel<knElem;iel++) {
    for (Int_t jel=0;jel<knElem;jel++) mat1(iel,jel)=0;
    mat1(iel,iel)=1;
  }
  //
  matHk(0, s1)=1;
  vecYk = vecZk-matHk*vecXk;               // Innovation or measurement residual
  matHkT=matHk.T(); matHk.T();
  matSk = (matHk*(covXk*matHkT))+measR;    // Innovation (or residual) covariance
  matSk.Invert();
  matKk = (covXk*matHkT)*matSk;            //  Optimal Kalman gain
  vecXk += matKk*vecYk;                    //  updated vector 
  covXk2= (mat1-(matKk*matHk));
  covXk3 =  covXk2*covXk;          
  covXk = covXk3;  
  Int_t nrows=covXk3.GetNrows();
  
  for (Int_t irow=0; irow<nrows; irow++)
    for (Int_t icol=0; icol<nrows; icol++){
      // rounding problems - make matrix again symteric
      covXk(irow,icol)=(covXk3(irow,icol)+covXk3(icol,irow))*0.5; 
    }
}



void   TStatToolkit::Constrain1D(const TString &input, const TString filter, TVectorD &param, TMatrixD & covar, Double_t mean, Double_t sigma){
  //
  // constrain linear fit
  // input  - string description of fit function
  // filter - string filter to select sub fits
  // param,covar - parameters and covariance matrix of the fit
  // mean,sigma  - new measurement uning which the fit is updated
  //
  
  TObjArray *array0= input.Tokenize("++");
  TObjArray *array1= filter.Tokenize("++");
  TMatrixD paramM(param.GetNrows(),1);
  for (Int_t i=0; i<=array0->GetEntries(); i++){paramM(i,0)=param(i);}
  
  if (filter.Length()==0){
    TStatToolkit::Update1D(mean, sigma, 0, paramM, covar);//
  }else{  
    for (Int_t i=0; i<array0->GetEntries(); i++){
      Bool_t isOK=kTRUE;
      TString str(array0->At(i)->GetName());
      for (Int_t j=0; j<array1->GetEntries(); j++){
	if (str.Contains(array1->At(j)->GetName())==0) isOK=kFALSE;      
      }
      if (isOK) {
	TStatToolkit::Update1D(mean, sigma, i+1, paramM, covar);//
      }
    }
  }
  for (Int_t i=0; i<=array0->GetEntries(); i++){
    param(i)=paramM(i,0);
  }
  delete array0;
  delete array1;
}

TString  TStatToolkit::MakeFitString(const TString &input, const TVectorD &param, const TMatrixD & covar, Bool_t verbose){
  //
  //
  //
  TObjArray *array0= input.Tokenize("++");
  TString result=Form("(%f",param[0]);
  printf("%f\t%f\t\n", param[0], TMath::Sqrt(covar(0,0))); 
  for (Int_t i=0; i<array0->GetEntries(); i++){
    TString str(array0->At(i)->GetName());
    result+="+"+str;
    result+=Form("*(%f)",param[i+1]);
    if (verbose) printf("%f\t%f\t%s\n", param[i+1], TMath::Sqrt(covar(i+1,i+1)),str.Data());    
  }
  result+="-0.)";
  delete array0;
  return result;
}

TGraphErrors * TStatToolkit::MakeGraphErrors(TTree * tree, const char * expr, const char * cut,  Int_t mstyle, Int_t mcolor, Float_t msize, Float_t offset){
  //
  // Query a graph errors
  // return TGraphErrors specified by expr and cut 
  // Example  usage TStatToolkit::MakeGraphError(tree,"Y:X:ErrY","X>0", 25,2,0.4)
  // tree   - tree with variable
  // expr   - examp 
  const Int_t entries =  tree->Draw(expr,cut,"goff");
  if (entries<=0) {
    TStatToolkit t;
    t.Error("TStatToolkit::MakeGraphError",Form("Empty or Not valid expression (%s) or cut *%s)", expr,cut));
    return 0;
  }
  if (  tree->GetV2()==0){
    TStatToolkit t;
    t.Error("TStatToolkit::MakeGraphError",Form("Not valid expression (%s) ", expr));
    return 0;
  }
  TGraphErrors * graph=0;
  if ( tree->GetV3()!=0){
    graph = new TGraphErrors (entries, tree->GetV2(),tree->GetV1(),0,tree->GetV3());
  }else{
    graph = new TGraphErrors (entries, tree->GetV2(),tree->GetV1(),0,0);
  }
  graph->SetMarkerStyle(mstyle); 
  graph->SetMarkerColor(mcolor);
  graph->SetLineColor(mcolor);
  if (msize>0) graph->SetMarkerSize(msize);
  for(Int_t i=0;i<graph->GetN();i++) graph->GetX()[i]+=offset;
  return graph;
  
}


TGraph * TStatToolkit::MakeGraphSparse(TTree * tree, const char * expr, const char * cut, Int_t mstyle, Int_t mcolor, Float_t msize, Float_t offset){
  //
  // Make a sparse draw of the variables
  // Format of expr : Var:Run or Var:Run:ErrorY or Var:Run:ErrorY:ErrorX
  // offset : points can slightly be shifted in x for better visibility with more graphs
  //
  // Written by Weilin.Yu
  // updated & merged with QA-code by Patrick Reichelt
  //
  const Int_t entries = tree->Draw(expr,cut,"goff");
  if (entries<=0) {
    TStatToolkit t;
    t.Error("TStatToolkit::MakeGraphSparse",Form("Empty or Not valid expression (%s) or cut (%s)", expr, cut));
    return 0;
  }
  //  TGraph * graph = (TGraph*)gPad->GetPrimitive("Graph"); // 2D

  Double_t *graphY, *graphX;
  graphY = tree->GetV1();
  graphX = tree->GetV2();

  // sort according to run number
  Int_t *index = new Int_t[entries*4];
  TMath::Sort(entries,graphX,index,kFALSE);

  // define arrays for the new graph
  Double_t *unsortedX = new Double_t[entries];
  Int_t *runNumber = new Int_t[entries];
  Double_t count = 0.5;

  // evaluate arrays for the new graph according to the run-number
  Int_t icount=0;
  //first entry
  unsortedX[index[0]] = count;
  runNumber[0] = graphX[index[0]];
  // loop the rest of entries
  for(Int_t i=1;i<entries;i++)
  {
    if(graphX[index[i]]==graphX[index[i-1]])
      unsortedX[index[i]] = count;
    else if(graphX[index[i]]!=graphX[index[i-1]]){
      count++;
      icount++;
      unsortedX[index[i]] = count;
      runNumber[icount]=graphX[index[i]];
    }
  }

  // count the number of xbins (run-wise) for the new graph
  const Int_t newNbins = int(count+0.5);
  Double_t *newBins = new Double_t[newNbins+1];
  for(Int_t i=0; i<=count+1;i++){
    newBins[i] = i;
  }

  // define and fill the new graph
  TGraph *graphNew = 0;
  if (tree->GetV3()) {
    if (tree->GetV4()) {
      graphNew = new TGraphErrors(entries,unsortedX,graphY,tree->GetV4(),tree->GetV3());
    }
    else { graphNew = new TGraphErrors(entries,unsortedX,graphY,0,tree->GetV3()); }
  }
  else { graphNew = new TGraphErrors(entries,unsortedX,graphY,0,0); }
  // with "Set(...)", the x-axis is being sorted
  graphNew->GetXaxis()->Set(newNbins,newBins);

  // set the bins for the x-axis, apply shifting of points
  Char_t xName[50];
  for(Int_t i=0;i<count;i++){
    snprintf(xName,50,"%d",runNumber[i]);
    graphNew->GetXaxis()->SetBinLabel(i+1,xName);
    graphNew->GetX()[i]+=offset;
  }

  graphNew->GetHistogram()->SetTitle("");
  graphNew->SetMarkerStyle(mstyle);
  graphNew->SetMarkerColor(mcolor);
  if (msize>0) graphNew->SetMarkerSize(msize);
  delete [] unsortedX;
  delete [] runNumber;
  delete [] index;
  delete [] newBins;
  //
  return graphNew;
}



//
// functions used for the trending
//

Int_t  TStatToolkit::MakeStatAlias(TTree * tree, const char * expr, const char * cut, const char * alias) 
{
  //
  // Add alias using statistical values of a given variable.
  // (by MI, Patrick Reichelt)
  //
  // tree - input tree
  // expr - variable expression
  // cut  - selection criteria
  // Output - return number of entries used to define variable
  // In addition mean, rms, median, and robust mean and rms (choosing fraction of data with smallest RMS)
  // 
  /* Example usage:
     1.) create the robust estimators for variable expr="QA.TPC.CPass1.meanTPCncl" and create a corresponding
     aliases with the prefix alias[0]="ncl", calculated using fraction alias[1]="0.90"

     TStatToolkit::MakeStatAlias(tree,"QA.TPC.CPass1.meanTPCncl","QA.TPC.CPass1.status>0","ncl:0.9");
     root [4] tree->GetListOfAliases().Print()
     OBJ: TNamed    ncl_Median      (130.964333+0)
     OBJ: TNamed    ncl_Mean        (122.120387+0)
     OBJ: TNamed    ncl_RMS         (33.509623+0)
     OBJ: TNamed    ncl_Mean90      (131.503862+0)
     OBJ: TNamed    ncl_RMS90       (3.738260+0)    
  */
  // 
  Int_t entries = tree->Draw(expr,cut,"goff");
  if (entries<=1){
    printf("Expression or cut not valid:\t%s\t%s\n", expr, cut);
    return 0;
  }
  //
  TObjArray* oaAlias = TString(alias).Tokenize(":");
  if (oaAlias->GetEntries()<2) return 0;
  Float_t entryFraction = atof( oaAlias->At(1)->GetName() );
  //
  Double_t median = TMath::Median(entries,tree->GetV1());
  Double_t mean   = TMath::Mean(entries,tree->GetV1());
  Double_t rms    = TMath::RMS(entries,tree->GetV1());
  Double_t meanEF=0, rmsEF=0;
  TStatToolkit::EvaluateUni(entries, tree->GetV1(), meanEF, rmsEF, entries*entryFraction);
  //
  tree->SetAlias(Form("%s_Median",oaAlias->At(0)->GetName()), Form("(%f+0)",median));
  tree->SetAlias(Form("%s_Mean",oaAlias->At(0)->GetName()), Form("(%f+0)",mean));
  tree->SetAlias(Form("%s_RMS",oaAlias->At(0)->GetName()), Form("(%f+0)",rms));
  tree->SetAlias(Form("%s_Mean%d",oaAlias->At(0)->GetName(),Int_t(entryFraction*100)), Form("(%f+0)",meanEF));
  tree->SetAlias(Form("%s_RMS%d",oaAlias->At(0)->GetName(),Int_t(entryFraction*100)), Form("(%f+0)",rmsEF));
  delete oaAlias; 
  return entries;
}

Int_t  TStatToolkit::SetStatusAlias(TTree * tree, const char * expr, const char * cut, const char * alias) 
{
  //
  // Add alias to trending tree using statistical values of a given variable.
  // (by MI, Patrick Reichelt)
  //
  // format of expr :  varname (e.g. meanTPCncl)
  // format of cut  :  char like in TCut
  // format of alias:  alias:query:entryFraction(EF) (fraction of entries used for uniformity evaluation)
  //            e.g.:  varname_Out:(abs(varname-meanEF)>6.*rmsEF):0.8
  // available internal variables are: 'varname, Median, Mean, MeanEF, RMS, RMSEF'
  // in the alias, 'varname' will be replaced by its content, and 'EF' by the percentage (e.g. MeanEF -> Mean80)
  //
  /* Example usage:
     1.) Define robust mean (possible, but easier done with TStatToolkit::MakeStatAlias(...)) 
     TStatToolkit::SetStatusAlias(tree, "meanTPCnclF", "meanTPCnclF>0", "meanTPCnclF_MeanEF:MeanEF:0.80") ;
     root [10] tree->GetListOfAliases()->Print()
               Collection name='TList', class='TList', size=1
               OBJ: TNamed    meanTPCnclF_Mean80      0.899308
     2.) create alias outlyers  - 6 sigma cut
     TStatToolkit::SetStatusAlias(tree, "meanTPCnclF", "meanTPCnclF>0", "meanTPCnclF_Out:(abs(meanTPCnclF-MeanEF)>6.*RMSEF):0.8")
     meanTPCnclF_Out ==> (abs(meanTPCnclF-0.899308)>6.*0.016590)
     3.) the same functionality as in 2.)
     TStatToolkit::SetStatusAlias(tree, "meanTPCnclF", "meanTPCnclF>0", "varname_Out2:(abs(varname-MeanEF)>6.*RMSEF):0.8") 
     meanTPCnclF_Out2 ==> (abs(meanTPCnclF-0.899308)>6.*0.016590)
  */
  //
  Int_t entries = tree->Draw(expr,cut,"goff");
  if (entries<1){
    printf("Expression or cut not valid:\t%s\t%s\n", expr, cut);
    return 0;
  }
  //
  TObjArray* oaVar = TString(expr).Tokenize(":");
  char varname[50];
  snprintf(varname,50,"%s", oaVar->At(0)->GetName());
  //
  TObjArray* oaAlias = TString(alias).Tokenize(":");
  if (oaAlias->GetEntries()<3) return 0;
  Float_t entryFraction = atof( oaAlias->At(2)->GetName() );
  //
  Double_t median = TMath::Median(entries,tree->GetV1());
  Double_t mean   = TMath::Mean(entries,tree->GetV1());
  Double_t rms    = TMath::RMS(entries,tree->GetV1());
  Double_t meanEF=0, rmsEF=0;
  TStatToolkit::EvaluateUni(entries, tree->GetV1(), meanEF, rmsEF, entries*entryFraction);
  //
  TString sAlias( oaAlias->At(0)->GetName() );
  sAlias.ReplaceAll("varname",varname);
  sAlias.ReplaceAll("MeanEF", Form("Mean%1.0f",entryFraction*100) );
  sAlias.ReplaceAll("RMSEF",  Form("RMS%1.0f",entryFraction*100) );
  TString sQuery( oaAlias->At(1)->GetName() );
  sQuery.ReplaceAll("varname",varname);
  sQuery.ReplaceAll("MeanEF", Form("%f",meanEF) );
  sQuery.ReplaceAll("RMSEF",  Form("%f",rmsEF) ); //make sure to replace 'RMSEF' before 'RMS'...
  sQuery.ReplaceAll("Median", Form("%f",median) );
  sQuery.ReplaceAll("Mean",   Form("%f",mean) );
  sQuery.ReplaceAll("RMS",    Form("%f",rms) );
  printf("define alias:\t%s = %s\n", sAlias.Data(), sQuery.Data());
  //
  char query[200];
  char aname[200];
  snprintf(query,200,"%s", sQuery.Data());
  snprintf(aname,200,"%s", sAlias.Data());
  tree->SetAlias(aname, query);
  delete oaVar;
  delete oaAlias;
  return entries;
}

TMultiGraph*  TStatToolkit::MakeStatusMultGr(TTree * tree, const char * expr, const char * cut, const char * alias, Int_t igr) 
{
  //
  // Compute a trending multigraph that shows for which runs a variable has outliers.
  // (by MI, Patrick Reichelt)
  //
  // format of expr :  varname:xaxis (e.g. meanTPCncl:run)
  // format of cut  :  char like in TCut
  // format of alias:  (1):(varname_Out==0):(varname_Out)[:(varname_Warning):...]
  // in the alias, 'varname' will be replaced by its content (e.g. varname_Out -> meanTPCncl_Out)
  // note: the aliases 'varname_Out' etc have to be defined by function TStatToolkit::SetStatusAlias(...)
  // counter igr is used to shift the multigraph in y when filling a TObjArray.
  //
  TObjArray* oaVar = TString(expr).Tokenize(":");
  if (oaVar->GetEntries()<2) return 0;
  char varname[50];
  char var_x[50];
  snprintf(varname,50,"%s", oaVar->At(0)->GetName());
  snprintf(var_x  ,50,"%s", oaVar->At(1)->GetName());
  //
  TString sAlias(alias);
  sAlias.ReplaceAll("varname",varname);
  TObjArray* oaAlias = TString(sAlias.Data()).Tokenize(":");
  if (oaAlias->GetEntries()<3) return 0;
  //
  char query[200];
  TMultiGraph* multGr = new TMultiGraph();
  Int_t marArr[6]    = {24+igr%2, 20+igr%2, 20+igr%2, 20+igr%2, 22, 23};
  Int_t colArr[6]    = {kBlack, kBlack, kRed, kOrange, kMagenta, kViolet};
  Double_t sizArr[6] = {1.2, 1.1, 1.0, 1.0, 1, 1};
  const Int_t ngr = oaAlias->GetEntriesFast();
  for (Int_t i=0; i<ngr; i++){
    if (i==2) continue; // the Fatal(Out) graph will be added in the end to be plotted on top!
    snprintf(query,200, "%f*(%s-0.5):%s", 1.+igr, oaAlias->At(i)->GetName(), var_x);
    multGr->Add( (TGraphErrors*) TStatToolkit::MakeGraphSparse(tree,query,cut,marArr[i],colArr[i],sizArr[i]) );
  }
  snprintf(query,200, "%f*(%s-0.5):%s", 1.+igr, oaAlias->At(2)->GetName(), var_x);
  multGr->Add( (TGraphErrors*) TStatToolkit::MakeGraphSparse(tree,query,cut,marArr[2],colArr[2],sizArr[2]) );
  //
  multGr->SetName(varname);
  multGr->SetTitle(varname); // used for y-axis labels. // details to be included!
  delete oaVar;
  delete oaAlias;
  return multGr;
}


void  TStatToolkit::AddStatusPad(TCanvas* c1, Float_t padratio, Float_t bottommargin)
{
  //
  // add pad to bottom of canvas for Status graphs (by Patrick Reichelt)
  // call function "DrawStatusGraphs(...)" afterwards
  //
  TCanvas* c1_clone = (TCanvas*) c1->Clone("c1_clone");
  c1->Clear();
  // produce new pads
  c1->cd();
  TPad* pad1 = new TPad("pad1", "pad1", 0., padratio, 1., 1.); 
  pad1->Draw();
  pad1->SetNumber(1); // so it can be called via "c1->cd(1);"
  c1->cd();
  TPad* pad2 = new TPad("pad2", "pad2", 0., 0., 1., padratio);
  pad2->Draw();
  pad2->SetNumber(2);
  // draw original canvas into first pad
  c1->cd(1);
  c1_clone->DrawClonePad();
  pad1->SetBottomMargin(0.001);
  pad1->SetRightMargin(0.01);
  // set up second pad
  c1->cd(2);
  pad2->SetGrid(3);
  pad2->SetTopMargin(0);
  pad2->SetBottomMargin(bottommargin); // for the long x-axis labels (runnumbers)
  pad2->SetRightMargin(0.01);
}


void  TStatToolkit::DrawStatusGraphs(TObjArray* oaMultGr)
{
  //
  // draw Status graphs into active pad of canvas (by MI, Patrick Reichelt)
  // ...into bottom pad, if called after "AddStatusPad(...)"
  //
  const Int_t nvars = oaMultGr->GetEntriesFast();
  TGraph* grAxis = (TGraph*) ((TMultiGraph*) oaMultGr->At(0))->GetListOfGraphs()->At(0);
  grAxis->SetMaximum(0.5*nvars+0.5);
  grAxis->SetMinimum(0); 
  grAxis->GetYaxis()->SetLabelSize(0);
  Int_t entries = grAxis->GetN();
  printf("entries (via GetN()) = %d\n",entries);
  grAxis->GetXaxis()->SetLabelSize(5.7*TMath::Min(TMath::Max(5./entries,0.01),0.03));
  grAxis->GetXaxis()->LabelsOption("v");
  grAxis->Draw("ap");
  //
  // draw multigraphs & names of status variables on the y axis
  for (Int_t i=0; i<nvars; i++){
    ((TMultiGraph*) oaMultGr->At(i))->Draw("p");
    TLatex* ylabel = new TLatex(-0.1, 0.5*i+0.5, ((TMultiGraph*) oaMultGr->At(i))->GetTitle());
    ylabel->SetTextAlign(32); //hor:right & vert:centered
    ylabel->SetTextSize(0.025/gPad->GetHNDC());
    ylabel->Draw();
  }
}


void TStatToolkit::DrawHistogram(TTree * tree, const char* drawCommand, const char* cuts, const char* histoname, const char* histotitle, Int_t nsigma, Float_t fraction )
{
  //
  // Draw histogram from TTree with robust range
  // Only for 1D so far!
  // 
  // Parameters:
  // - histoname:  name of histogram
  // - histotitle: title of histgram
  // - fraction:   fraction of data to define the robust mean
  // - nsigma:     nsigma value for range
  //

   TString drawStr(drawCommand);
   TString cutStr(cuts);
   Int_t dim = 1;

   if(!tree) {
     cerr<<" Tree pointer is NULL!"<<endl;
     return;
   }

   // get entries
   Int_t entries = tree->Draw(drawStr.Data(), cutStr.Data(), "goff");
   if (entries == -1) {
     cerr<<"TTree draw returns -1"<<endl;
     return;
   }

   // get dimension
   if(tree->GetV1()) dim = 1;
   if(tree->GetV2()) dim = 2;
   if(tree->GetV3()) dim = 3;
   if(dim > 2){
     cerr<<"TTree has more than 2 dimensions (not yet supported)"<<endl;
     return;
   }

   // draw robust
   Double_t meanX, rmsX=0;
   Double_t meanY, rmsY=0;
   TStatToolkit::EvaluateUni(entries, tree->GetV1(),meanX,rmsX, fraction*entries);
   if(dim==2){
     TStatToolkit::EvaluateUni(entries, tree->GetV1(),meanY,rmsY, fraction*entries);
     TStatToolkit::EvaluateUni(entries, tree->GetV2(),meanX,rmsX, fraction*entries);
   }
   TH1* hOut;
   if(dim==1){
     hOut = new TH1F(histoname, histotitle, 200, meanX-nsigma*rmsX, meanX+nsigma*rmsX);
     for (Int_t i=0; i<entries; i++) hOut->Fill(tree->GetV1()[i]);
     hOut->GetXaxis()->SetTitle(tree->GetHistogram()->GetXaxis()->GetTitle());
     hOut->Draw();
   }
   else if(dim==2){
     hOut = new TH2F(histoname, histotitle, 200, meanX-nsigma*rmsX, meanX+nsigma*rmsX,200, meanY-nsigma*rmsY, meanY+nsigma*rmsY);
     for (Int_t i=0; i<entries; i++) hOut->Fill(tree->GetV2()[i],tree->GetV1()[i]);
     hOut->GetXaxis()->SetTitle(tree->GetHistogram()->GetXaxis()->GetTitle());
     hOut->GetYaxis()->SetTitle(tree->GetHistogram()->GetYaxis()->GetTitle());
     hOut->Draw("colz");
   }

}

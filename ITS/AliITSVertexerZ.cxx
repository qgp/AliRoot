/**************************************************************************
 * Copyright(c) 1998-2003, ALICE Experiment at CERN, All rights reserved. *
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
#include "AliITSVertexerZ.h"
#include<TBranch.h>
#include<TClonesArray.h>
#include<TH1.h>
#include <TString.h>
#include<TTree.h>
#include "AliESDVertex.h"
#include "AliITSgeomTGeo.h"
#include "AliITSDetTypeRec.h"
#include "AliITSRecPoint.h"
#include "AliITSZPoint.h"

/////////////////////////////////////////////////////////////////
// this class implements a fast method to determine
// the Z coordinate of the primary vertex
// for p-p collisions it seems to give comparable or better results
// with respect to what obtained with AliITSVertexerPPZ
// It can be used successfully with Pb-Pb collisions
////////////////////////////////////////////////////////////////

ClassImp(AliITSVertexerZ)



//______________________________________________________________________
AliITSVertexerZ::AliITSVertexerZ():AliITSVertexer(),
fFirstL1(0),
fLastL1(0),
fFirstL2(0),
fLastL2(0),
fDiffPhiMax(0),
fZFound(0),
fZsig(0.),
fZCombc(0),
fLowLim(0.),
fHighLim(0.),
fStepCoarse(0),
fTolerance(0.),
fMaxIter(0),
fWindowWidth(0) {
  // Default constructor
  SetDiffPhiMax();
  SetFirstLayerModules();
  SetSecondLayerModules();
  SetLowLimit();
  SetHighLimit();
  SetBinWidthCoarse();
  SetTolerance();
  SetPPsetting();
  ConfigIterations();
  SetWindowWidth();
}

//______________________________________________________________________
AliITSVertexerZ::AliITSVertexerZ(Float_t x0, Float_t y0):AliITSVertexer(),
fFirstL1(0),
fLastL1(0),
fFirstL2(0),
fLastL2(0),
fDiffPhiMax(0),
fZFound(0),
fZsig(0.),
fZCombc(0),
fLowLim(0.),
fHighLim(0.),
fStepCoarse(0),
fTolerance(0.),
fMaxIter(0),
fWindowWidth(0) {
  // Standard Constructor
  SetDiffPhiMax();
  SetFirstLayerModules();
  SetSecondLayerModules();
  SetLowLimit();
  SetHighLimit();
  SetBinWidthCoarse();
  SetTolerance();
  SetPPsetting();
  ConfigIterations();
  SetWindowWidth();
  SetVtxStart((Double_t)x0,(Double_t)y0,0.);

}

//______________________________________________________________________
AliITSVertexerZ::~AliITSVertexerZ() {
  // Destructor
  delete fZCombc;
}

//______________________________________________________________________
void AliITSVertexerZ::ConfigIterations(Int_t noiter,Float_t *ptr){
  // configure the iterative procedure to gain efficiency for
  // pp events with very low multiplicity
  Float_t defaults[5]={0.05,0.1,0.2,0.3,0.5};
  fMaxIter=noiter;
  if(noiter>5){
    Error("ConfigIterations","Maximum number of iterations is 5\n");
    fMaxIter=5;
  }
  for(Int_t j=0;j<5;j++)fPhiDiffIter[j]=defaults[j];
  if(ptr)for(Int_t j=0;j<fMaxIter;j++)fPhiDiffIter[j]=ptr[j];
}

//______________________________________________________________________
Int_t AliITSVertexerZ::GetPeakRegion(TH1F*h, Int_t &binmin, Int_t &binmax) const {
  // Finds a region around a peak in the Z histogram
  // Case of 2 peaks is treated 
  Int_t imax=h->GetNbinsX();
  Float_t maxval=0;
  Int_t bi1=h->GetMaximumBin();
  Int_t bi2=0;
  for(Int_t i=imax;i>=1;i--){
    if(h->GetBinContent(i)>maxval){
      maxval=h->GetBinContent(i);
      bi2=i;
    }
  }
  Int_t npeaks=0;

  if(bi1==bi2){
    binmin=bi1-3;
    binmax=bi1+3;
    npeaks=1;
  }else{
    TH1F *copy = new TH1F(*h);
    copy->SetBinContent(bi1,0.);
    copy->SetBinContent(bi2,0.);
    Int_t l1=TMath::Max(bi1-3,1);
    Int_t l2=TMath::Min(bi1+3,h->GetNbinsX());
    Float_t cont1=copy->Integral(l1,l2);
    Int_t ll1=TMath::Max(bi2-3,1);
    Int_t ll2=TMath::Min(bi2+3,h->GetNbinsX());
    Float_t cont2=copy->Integral(ll1,ll2);
    if(cont1>cont2){
      binmin=l1;
      binmax=l2;
      npeaks=1;
    }
    if(cont2>cont1){
      binmin=ll1;
      binmax=ll2;
      npeaks=1;
    }
    if(cont1==cont2){
      binmin=l1;
      binmax=ll2;
      if(bi2-bi1==1) npeaks=1;
      else npeaks=2;
    }  
    delete copy;
  }    
  return npeaks;
}
//______________________________________________________________________
AliESDVertex* AliITSVertexerZ::FindVertexForCurrentEvent(TTree *itsClusterTree){
  // Defines the AliESDVertex for the current event
  VertexZFinder(itsClusterTree);
  Int_t ntrackl=0;
  for(Int_t iteraz=0;iteraz<fMaxIter;iteraz++){
    if(fCurrentVertex) ntrackl=fCurrentVertex->GetNContributors();
    if(!fCurrentVertex || ntrackl==0 || ntrackl==-1){
      Float_t diffPhiMaxOrig=fDiffPhiMax;
      fDiffPhiMax=GetPhiMaxIter(iteraz);
      VertexZFinder(itsClusterTree);
      fDiffPhiMax=diffPhiMaxOrig;
    }
  }
  FindMultiplicity(itsClusterTree);
  return fCurrentVertex;
}  

//______________________________________________________________________
void AliITSVertexerZ::VertexZFinder(TTree *itsClusterTree){
  // Defines the AliESDVertex for the current event
  fCurrentVertex = 0;
  AliITSDetTypeRec detTypeRec;

  TTree *tR = itsClusterTree;
  detTypeRec.SetTreeAddressR(tR);
  TClonesArray *itsRec  = 0;
  // lc1 and gc1 are local and global coordinates for layer 1
  Float_t lc1[3]; for(Int_t ii=0; ii<3; ii++) lc1[ii]=0.;
  Float_t gc1[3]; for(Int_t ii=0; ii<3; ii++) gc1[ii]=0.;
  // lc2 and gc2 are local and global coordinates for layer 2
  Float_t lc2[3]; for(Int_t ii=0; ii<3; ii++) lc2[ii]=0.;
  Float_t gc2[3]; for(Int_t ii=0; ii<3; ii++) gc2[ii]=0.;

  itsRec = detTypeRec.RecPoints();
  TBranch *branch;
  branch = tR->GetBranch("ITSRecPoints");

  Int_t nrpL1 = 0;
  Int_t nrpL2 = 0;

  // By default fFirstL1=0 and fLastL1=79
  for(Int_t module= fFirstL1; module<=fLastL1;module++){
    branch->GetEvent(module);
    nrpL1+= itsRec->GetEntries();
    detTypeRec.ResetRecPoints();
  }
  //By default fFirstL2=80 and fLastL2=239
  for(Int_t module= fFirstL2; module<=fLastL2;module++){
    branch->GetEvent(module);
    nrpL2+= itsRec->GetEntries();
    detTypeRec.ResetRecPoints();
  }
  if(nrpL1 == 0 || nrpL2 == 0){
    ResetHistograms();
    fCurrentVertex = new AliESDVertex(0.,5.3,-2);
    return;
  }
  // Force a coarse bin size of 200 microns if the number of clusters on layer 2
  // is low
  if(nrpL2<fPPsetting[0])SetBinWidthCoarse(fPPsetting[1]);
  // By default nbincoarse=(10+10)/0.01=2000
  Int_t nbincoarse = static_cast<Int_t>((fHighLim-fLowLim)/fStepCoarse);
  if(fZCombc)delete fZCombc;
  fZCombc = new TH1F("fZCombc","Z",nbincoarse,fLowLim,fLowLim+nbincoarse*fStepCoarse);

 /* Test the ffect of mutiple scatternig on error. Negligible
  // Multiple scattering
  Float_t beta=1.,pmed=0.875; //pmed=875 MeV (for tracks with dphi<0.01 rad)
  Float_t beta2=beta*beta;
  Float_t p2=pmed*pmed;
  Float_t rBP=3; //Beam Pipe radius = 3cm
  Float_t dBP=0.08/35.3; // 800 um of Be
  Float_t dL1=0.01; //approx. 1% of radiation length  
  Float_t theta2BP=14.1*14.1/(beta2*p2*1e6)*TMath::Abs(dBP);
  Float_t theta2L1=14.1*14.1/(beta2*p2*1e6)*TMath::Abs(dL1);
*/
  Int_t maxdim=TMath::Min(nrpL1*nrpL2,50000);  // temporary; to limit the size in PbPb
  static TClonesArray points("AliITSZPoint",maxdim);
  Int_t nopoints =0;
  for(Int_t modul1= fFirstL1; modul1<=fLastL1;modul1++){   // Loop on modules of layer 1
    UShort_t ladder=int(modul1/4)+1;  // ladders are numbered starting from 1
    branch->GetEvent(modul1);
    Int_t nrecp1 = itsRec->GetEntries();
    static TClonesArray prpl1("AliITSRecPoint",nrecp1);
    prpl1.SetOwner();
    for(Int_t j=0;j<nrecp1;j++){
      AliITSRecPoint *recp = (AliITSRecPoint*)itsRec->At(j);
      new(prpl1[j])AliITSRecPoint(*recp);
    }
    detTypeRec.ResetRecPoints();
    for(Int_t j1=0;j1<nrecp1;j1++){
      AliITSRecPoint *recp = (AliITSRecPoint*)prpl1.At(j1);
      /*
      lc1[0]=recp->GetDetLocalX();
      lc1[2]=recp->GetDetLocalZ();
      geom->LtoG(modul1,lc1,gc1);
      // Global coordinates of this recpoints
      */
      recp->GetGlobalXYZ(gc1);
      gc1[0]-=GetNominalPos()[0]; // Possible beam offset in the bending plane
      gc1[1]-=GetNominalPos()[1]; //   "               "
      Float_t r1=TMath::Sqrt(gc1[0]*gc1[0]+gc1[1]*gc1[1]);
      Float_t phi1 = TMath::ATan2(gc1[1],gc1[0]);
      if(phi1<0)phi1+=2*TMath::Pi();
      Float_t zc1=gc1[2];
      Float_t erz1=recp->GetSigmaZ2();
      for(Int_t ladl2=0 ; ladl2<fLadOnLay2*2+1;ladl2++){
	for(Int_t k=0;k<4;k++){
 	  Int_t ladmod=fLadders[ladder-1]+ladl2;
	  if(ladmod>AliITSgeomTGeo::GetNLadders(2)) ladmod=ladmod-AliITSgeomTGeo::GetNLadders(2);
	  Int_t modul2=AliITSgeomTGeo::GetModuleIndex(2,ladmod,k+1);
	  branch->GetEvent(modul2);
	  Int_t nrecp2 = itsRec->GetEntries();
	  for(Int_t j2=0;j2<nrecp2;j2++){
	    recp = (AliITSRecPoint*)itsRec->At(j2);
	    /*
	    lc2[0]=recp->GetDetLocalX();
	    lc2[2]=recp->GetDetLocalZ();
	    geom->LtoG(modul2,lc2,gc2);
	    */
	    recp->GetGlobalXYZ(gc2);
	    gc2[0]-=GetNominalPos()[0];
	    gc2[1]-=GetNominalPos()[1];
	    Float_t r2=TMath::Sqrt(gc2[0]*gc2[0]+gc2[1]*gc2[1]);
	    Float_t phi2 = TMath::ATan2(gc2[1],gc2[0]);
	    if(phi2<0)phi2+=2*TMath::Pi();
	    Float_t zc2=gc2[2];
	    Float_t erz2=recp->GetSigmaZ2();

	    Float_t diff = TMath::Abs(phi2-phi1); 
	    if(diff>TMath::Pi())diff=2.*TMath::Pi()-diff;
	    if(diff<fDiffPhiMax){
	      //	Float_t tgth=(zc2[j]-zc1[i])/(r2-r1); // slope (used for multiple scattering)
	      Float_t zr0=(r2*zc1-r1*zc2)/(r2-r1); //Z @ null radius
	      Float_t ezr0q=(r2*r2*erz1+r1*r1*erz2)/(r2-r1)/(r2-r1); //error on Z @ null radius
	 /*
         // Multiple scattering
        ezr0q+=r1*r1*(1+tgth*tgth)*theta2L1/2; // multiple scattering in layer 1
        ezr0q+=rBP*rBP*(1+tgth*tgth)*theta2BP/2; // multiple scattering in beam pipe
	*/
	      if(nopoints<maxdim) new(points[nopoints++])AliITSZPoint(zr0,ezr0q);	      
	      fZCombc->Fill(zr0);
	    }
	  }
	  detTypeRec.ResetRecPoints();
	}
      }
    }
    prpl1.Clear(); 
  }

  points.Sort();

  Double_t contents = fZCombc->GetEntries()- fZCombc->GetBinContent(0)-fZCombc->GetBinContent(nbincoarse+1);
  if(contents<1.){
    //    Warning("FindVertexForCurrentEvent","Insufficient number of rec. points\n");
    ResetHistograms();
    fCurrentVertex = new AliESDVertex(0.,5.3,-1);
    points.Clear();
    return;
  }

  TH1F *hc = fZCombc;

  
  if(hc->GetBinContent(hc->GetMaximumBin())<3)hc->Rebin(3);
  Int_t binmin,binmax;
  Int_t nPeaks=GetPeakRegion(hc,binmin,binmax);   
  if(nPeaks==2)AliWarning("2 peaks found");
  Float_t zm =0.;
  Float_t ezm =0.;
  Float_t lim1 = hc->GetBinLowEdge(binmin);
  Float_t lim2 = hc->GetBinLowEdge(binmax)+hc->GetBinWidth(binmax);

  if(nPeaks ==1 && (lim2-lim1)<fWindowWidth){
    Float_t c=(lim1+lim2)/2.;
    lim1=c-fWindowWidth/2.;
    lim2=c+fWindowWidth/2.;
  }
  Int_t niter = 0, ncontr=0;
  do {
    // symmetrization
    if(zm  !=0.){
      Float_t semilarg=TMath::Min((lim2-zm),(zm-lim1));
      lim1=zm - semilarg;
      lim2=zm + semilarg;
    }

    zm=0.;
    ezm=0.;
    ncontr=0;
    for(Int_t i =0; i<points.GetEntries(); i++){
      AliITSZPoint* p=(AliITSZPoint*)points.UncheckedAt(i);
      if(p->GetZ()>lim1 && p->GetZ()<lim2){
        Float_t deno = p->GetErrZ();
        zm+=p->GetZ()/deno;
        ezm+=1./deno;
        ncontr++;
      }
    }
    if(ezm>0) {
      zm/=ezm;
      ezm=TMath::Sqrt(1./ezm);
    }
    niter++;
  } while(niter<10 && TMath::Abs((zm-lim1)-(lim2-zm))>fTolerance);
  fCurrentVertex = new AliESDVertex(zm,ezm,ncontr);
  fCurrentVertex->SetTitle("vertexer: B");
  points.Clear();
  ResetHistograms();
  return;
}

//_____________________________________________________________________
void AliITSVertexerZ::ResetHistograms(){
  // delete TH1 data members
  if(fZCombc)delete fZCombc;
  fZCombc = 0;
}

//________________________________________________________
void AliITSVertexerZ::PrintStatus() const {
  // Print current status
  cout <<"=======================================================\n";
  cout <<" First layer first and last modules: "<<fFirstL1<<", ";
  cout <<fLastL1<<endl;
  cout <<" Second layer first and last modules: "<<fFirstL2<<", ";
  cout <<fLastL2<<endl;
  cout <<" Max Phi difference: "<<fDiffPhiMax<<endl;
  cout <<"Limits for Z histograms: "<<fLowLim<<"; "<<fHighLim<<endl;
  cout <<"Bin sizes for coarse z histos "<<fStepCoarse<<endl;
  cout <<" Current Z "<<fZFound<<"; Z sig "<<fZsig<<endl;
  if(fZCombc){
    cout<<"fZCombc exists - entries="<<fZCombc->GetEntries()<<endl;
  }
  else{
    cout<<"fZCombc does not exist\n";
  }
 
  cout <<"=======================================================\n";
}


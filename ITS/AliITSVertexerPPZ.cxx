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
#include <AliITSVertexerPPZ.h>
#include <Riostream.h>
#include <TArrayF.h>
#include <TDirectory.h>
#include <TH1F.h>
#include <TMath.h>
#include <TObjArray.h>
#include <TTree.h>
#include <TClonesArray.h>
#include "AliRun.h"
#include "AliITS.h"
#include "AliITSgeom.h"
#include "AliITSRecPoint.h"
/////////////////////////////////////////////////////////////////////////
//                                                                     //
// This class is intended to compute the Z coordinate of the           //
// primary vertex for p-p interactions, or in general when the         //
// number of secondaries is too slow to use the class                  //
// AliITSVertexerIons, which in turn is optimized for A-A collsions    //
// Origin: masera@to.infn.it      9/12/2002                            //
//                                                                     //
/////////////////////////////////////////////////////////////////////////
ClassImp(AliITSVertexerPPZ)



//______________________________________________________________________
AliITSVertexerPPZ::AliITSVertexerPPZ():AliITSVertexer() {
  // Default Constructor

  SetDiffPhiMax(0);
  fX0 = 0.;
  fY0 = 0.;
  SetFirstLayerModules(0);
  SetSecondLayerModules(0);
  fZFound = 0;
  fZsig = 0.;
  fITS = 0;
  SetWindow(0);
}

AliITSVertexerPPZ::AliITSVertexerPPZ(TFile *infile, TFile *outfile, Float_t x0, Float_t y0):AliITSVertexer(infile,outfile) {
  // Standard constructor
  if(!fInFile)Fatal("AliITSVertexerPPZ","No inputfile has been defined!");
  SetDiffPhiMax();
  fX0 = x0;
  fY0 = y0;
  SetFirstLayerModules();
  SetSecondLayerModules();
  fZFound = 0;
  fZsig = 0.;
  fITS = 0;
  SetWindow();
}

//______________________________________________________________________
AliITSVertexerPPZ::~AliITSVertexerPPZ() {
  // Default Destructor
  fITS = 0;
}

//________________________________________________________
void  AliITSVertexerPPZ::EvalZ(TH1F *hist,Int_t sepa, Int_t ncoinc, TArrayF *zval) {
  Float_t DeltaVal = hist->GetBinWidth(1)*fWindow; // max window in Z for searching
  fZFound=0;
  fZsig=0;
  Int_t N=0;
  Int_t NbinNotZero=0;
  Float_t totst = 0.;
  Float_t totst2 = 0.;
  Float_t curz = 0.;
  for(Int_t i=1;i<=sepa;i++){
    Float_t cont=hist->GetBinContent(i);
    if(cont!=0)curz = hist->GetBinLowEdge(i)+0.5*hist->GetBinWidth(i);
    totst+=cont;
    totst2+=cont*cont;
    N++;
    if(cont!=0)NbinNotZero++;
  }
  if(NbinNotZero==0){fZFound=-100; fZsig=-100; return;}
  if(NbinNotZero==1){
    fZFound = curz;
    fZsig=0;
    fCurrentVertex = new AliITSVertex(fZFound,fZsig,NbinNotZero);
    return;
  }
  Float_t errsq = totst2/(N-1)-totst*totst/N/(N-1);
  if(errsq>=0){
  totst2=TMath::Sqrt(errsq);
  }
  else {
    Error("EvalZ","Negative variance: %d - %12.7f - %12.7f",N,totst2,totst);
    fZFound=-100; 
    fZsig=-100;
    return;
  }
  totst /= N;
  Float_t cut = totst+totst2*2.;
  if(fDebug>1)cout<<"totst, totst2, cut: "<<totst<<", "<<totst2<<", "<<cut<<endl;
  Float_t val1=hist->GetBinLowEdge(sepa); 
  Float_t val2=hist->GetBinLowEdge(1);
  Float_t valm = 0.;
  Float_t zmax = 0.;
  for(Int_t i=1;i<=sepa;i++){
    Float_t cont=hist->GetBinContent(i);
    if(cont>valm){
      valm = cont;
      zmax = hist->GetBinLowEdge(i)+0.5*hist->GetBinWidth(1);
    }
    if(cont>cut){
      curz=hist->GetBinLowEdge(i);
      if(curz<val1)val1=curz;
      if(curz>val2)val2=curz;
    }
  }
  val2+=hist->GetBinWidth(1);
  if((val2-val1)>DeltaVal){
    val1 = zmax-DeltaVal/2.;
    val2 = zmax+DeltaVal/2.;
    if(fDebug>0)cout<<"val1 and val2 recomputed\n";
  }
  if(fDebug>0)cout<<"Values for Z finding: "<<val1<<" "<<val2<<" "<<val2-val1<<endl;
  fZFound=0;
  fZsig=0;
  N=0;
  for(Int_t i=0; i<ncoinc; i++){
    Float_t z=zval->At(i);
    if(z<val1)continue;
    if(z>val2)continue;
   
    fZFound+=z;
    fZsig+=z*z;
    /*   weights defined by the curvature
    Float_t wei = 1./curv->At(i);
    fZFound+=z*wei;
    fZsig+=wei;
    */
    N++;
  }
  if(N<1){fZFound=-110; fZsig=-110; return;}
  if(N==1){
    fZsig = 0;
    fCurrentVertex = new AliITSVertex(fZFound,fZsig,N);
    return;
  }
  errsq = (fZsig/(N-1)-fZFound*fZFound/N/(N-1))/N;
  if(errsq>=0.){
    fZsig=TMath::Sqrt(errsq);
  }
  else {
    Error("evalZ","Negative variance: %d - %12.7f %12.7f",N,fZsig,fZFound);
    fZsig=0;
  }
  fZFound=fZFound/N;
  /* weights defined by the curvature
  fZsig=1./fZsig;
  fZFound*=fZsig;
  fZsig = TMath::Sqrt(fZsig);
  */
  fCurrentVertex = new AliITSVertex(fZFound,fZsig,N);
}

//______________________________________________________________________
AliITSVertex* AliITSVertexerPPZ::FindVertexForCurrentEvent(Int_t evnumber){
  // Defines the AliITSVertex for the current event
  fCurrentVertex = 0;
  fZFound = -999;
  fZsig = -999;
  if(!gAlice)gAlice = (AliRun*)fInFile->Get("gAlice");
  if(!gAlice){
    Error("FindVertexForCurrentEvent","The AliRun object is not available - nothing done");
    return fCurrentVertex;
  }
  if(!fITS)  {
    fITS = (AliITS*)gAlice->GetModule("ITS");
    if(!fITS) {
      Error("FindVertexForCurrentEvent","AliITS object was not found");
      return fCurrentVertex;
    }
  }
  AliITSgeom *geom = fITS->GetITSgeom();
  if(!geom) {
    Error("FindVertexForCurrentEvent","ITS geometry is not defined");
    return fCurrentVertex;
  }
  TTree *TR=0;
  TClonesArray *ITSrec  = 0;
  Float_t lc[3]; for(Int_t ii=0; ii<3; ii++) lc[ii]=0.;
  Float_t gc[3]; for(Int_t ii=0; ii<3; ii++) gc[ii]=0.;
  Float_t lc2[3]; for(Int_t ii=0; ii<3; ii++) lc2[ii]=0.;
  Float_t gc2[3]; for(Int_t ii=0; ii<3; ii++) gc2[ii]=0.;

  TR = gAlice->TreeR();
  if(!TR){
    Error("FindVertexForCurrentEvent","TreeR not found");
    return fCurrentVertex;
  }
  ITSrec  = fITS->RecPoints();  // uses slow points or fast points if slow are
  // missing
  TBranch *branch = TR->GetBranch("ITSRecPoints");
  if(!branch){ 
    branch = TR->GetBranch("ITSRecPointsF");
    //   Warning("FindVertexForCurrentEvent","Using Fast points");
  }
  if(!branch){
   Error("FindVertexForCurrentEvent","branch for ITS rec points not found");
   return fCurrentVertex;
  }
  Float_t zave=0;
  Float_t rmszav=0;
  Float_t zave2=0;
  Int_t firipixe=0;
  for(Int_t module= fFirstL1; module<=fLastL1;module++){
    branch->GetEvent(module);
    Int_t nrecp1 = ITSrec->GetEntries();
    for(Int_t i=0; i<nrecp1;i++){
      AliITSRecPoint *current = (AliITSRecPoint*)ITSrec->At(i);
      lc[0]=current->GetX();
      lc[2]=current->GetZ();
      geom->LtoG(module,lc,gc);
      zave+=gc[2];
      zave2+=gc[2]*gc[2];
      firipixe++;
    }
    fITS->ResetRecPoints();
  }
  if(firipixe>1){
    Float_t rmszav2 = zave2/(firipixe-1)-zave*zave/firipixe/(firipixe-1);
    if (rmszav2 <0) {
      Warning("FindVertexForCurrentEvent","Negative squared RMS %f, set to 0", rmszav2);
      rmszav2=0;
    }
    rmszav=TMath::Sqrt(rmszav2);
    zave=zave/firipixe;
    if(fDebug>1)cout<<"++++++++++++++++++++++++++++++++++++++++++++++++++++++\n Number of firing pixels: "<<firipixe<<endl;
  }
  else {
    fZFound = -200;
    fZsig = -200;
    Warning("FindVertexForCurrentEvent","No rec points on first layer for this event");
    return fCurrentVertex;
  }
  Float_t zlim1=zave-rmszav;
  Float_t zlim2=zave+rmszav;
  Int_t sepa=(Int_t)((zlim2-zlim1)*10.+1.);
  zlim2=zlim1 + sepa/10.;
  TH1F *zvdis = new TH1F("z_ev","zv distr",sepa,zlim1,zlim2);
  if(fDebug>0){
    cout<<"Z limits: "<<zlim1<<" "<<zlim2<<"; Bins= "<<sepa<<endl;
    cout<<"Bin width: "<<zvdis->GetBinWidth(1)*10000.<<" micron\n";
  }
  Int_t sizarr=100;
  TArrayF *zval = new TArrayF(sizarr);
  //  TArrayF *curv = new TArrayF(sizarr);
  Int_t ncoinc=0;
  for(Int_t module= fFirstL1; module<=fLastL1;module++){
    if(fDebug>0)cout<<"processing module   "<<module<<"                  \r";
    branch->GetEvent(module);
    Int_t nrecp1 = ITSrec->GetEntries();
    TObjArray *poiL1 = new TObjArray(nrecp1);
    for(Int_t i=0; i<nrecp1;i++)poiL1->AddAt(ITSrec->At(i),i);
    fITS->ResetRecPoints();
    for(Int_t i=0; i<nrecp1;i++){
      AliITSRecPoint *current = (AliITSRecPoint*)poiL1->At(i);
      lc[0]=current->GetX();
      lc[2]=current->GetZ();
      geom->LtoG(module,lc,gc);
      gc[0]-=fX0;
      gc[1]-=fY0;
      Float_t r1=TMath::Sqrt(gc[0]*gc[0]+gc[1]*gc[1]);
      Float_t phi1 = TMath::ATan2(gc[1],gc[0]);
      if(phi1<0)phi1=2*TMath::Pi()+phi1;
      if(fDebug>1)cout<<"module "<<module<<" "<<gc[0]<<" "<<gc[1]<<" "<<gc[2]<<" "<<phi1<<"     \n";
      for(Int_t modul2=fFirstL2; modul2<=fLastL2; modul2++){
	branch->GetEvent(modul2);
	Int_t nrecp2 = ITSrec->GetEntries();
	for(Int_t j=0; j<nrecp2;j++){
	  AliITSRecPoint *recp = (AliITSRecPoint*)ITSrec->At(j);
	  lc2[0]=recp->GetX();
	  lc2[2]=recp->GetZ();
	  geom->LtoG(modul2,lc2,gc2);
	  gc2[0]-=fX0;
	  gc2[1]-=fY0;
	  Float_t r2=TMath::Sqrt(gc2[0]*gc2[0]+gc2[1]*gc2[1]);
	  Float_t zr0=(r2*gc[2]-r1*gc2[2])/(r2-r1);
	  Float_t phi2 = TMath::ATan2(gc2[1],gc2[0]);
	  if(phi2<0)phi2=2.*TMath::Pi()+phi2;
	  Float_t diff = TMath::Abs(phi2-phi1);
	  if(diff>TMath::Pi())diff=2.*TMath::Pi()-diff;
	  if(zr0>zlim1 && zr0<zlim2){
	    if(diff<fDiffPhiMax ){
	      zvdis->Fill(zr0);
	      zval->AddAt(zr0,ncoinc);
	      /* uncomment these lines to use curvature as a weight
	      Float_t cu = Curv(0.,0.,gc[0],gc[1],gc2[0],gc[1]);
	      curv->AddAt(cu,ncoinc);
	      */
	      ncoinc++;
	      if(ncoinc==(sizarr-1)){
		sizarr+=100;
		zval->Set(sizarr);
		//uncomment next line to use curvature as weight
		//		curv->Set(sizarr);
	      }
	    }
	  }
	}
	fITS->ResetRecPoints();
      }
    }
    delete poiL1;
  }         // loop on modules
  if(fDebug>0){
    cout<<endl<<"Number of coincidences = "<<ncoinc<<endl;
  }
  //  EvalZ(zvdis,sepa,ncoinc,zval,curv);
  EvalZ(zvdis,sepa,ncoinc,zval);
  delete zvdis;
  delete zval;
  //  delete curv;
  if(fCurrentVertex){
    char name[30];
    sprintf(name,"Vertex_%d",evnumber);
    fCurrentVertex->SetName(name);
    fCurrentVertex->SetTitle("vertexer: PPZ");
  }
  return fCurrentVertex;
}

//______________________________________________________________________
void AliITSVertexerPPZ::FindVertices(){
  // computes the vertices of the events in the range FirstEvent - LastEvent
  if(!fOutFile){
    Error("FindVertices","The output file is not available - nothing done");
    return;
  }
  if(!gAlice)gAlice = (AliRun*)fInFile->Get("gAlice");
  if(!gAlice){
    Error("FindVertices","The AliRun object is not available - nothing done");
    return;
  }
  TDirectory *curdir = gDirectory;
  fInFile->cd();
  for(Int_t i=fFirstEvent;i<=fLastEvent;i++){
    gAlice->GetEvent(i);
    FindVertexForCurrentEvent(i);
    if(fCurrentVertex){
      WriteCurrentVertex();
    }
    else {
      if(fDebug>0){
	cout<<"Vertex not found for event "<<i<<endl;
	cout<<"fZFound = "<<fZFound<<", fZsig= "<<fZsig<<endl;
      }
    }
  }
  curdir->cd();
}

//________________________________________________________
void AliITSVertexerPPZ::PrintStatus() const {
  // Print current status
  cout <<"=======================================================\n";
  cout <<" First layer first and last modules: "<<fFirstL1<<", ";
  cout <<fLastL1<<endl;
  cout <<" Second layer first and last modules: "<<fFirstL2<<", ";
  cout <<fLastL2<<endl;
  cout <<" Max Phi difference: "<<fDiffPhiMax<<endl;
  cout <<" Window for Z search: "<<fWindow<<endl;
  cout <<" Current Z "<<fZFound<<"; Z sig "<<fZsig<<endl;
  cout <<" Debug flag: "<<fDebug<<endl;
  if(fInFile)cout <<" The input file is open\n";
  if(!fInFile)cout <<"The input file is not open\n";
  if(fOutFile)cout <<"The output file is open\n";
  if(!fOutFile)cout <<"The output file is not open\n";
  cout<<"First event to be processed "<<fFirstEvent;
  cout<<"\n Last event to be processed "<<fLastEvent<<endl;
}

//________________________________________________________
Float_t AliITSVertexerPPZ::Curv(Double_t x1,Double_t y1,
                   Double_t x2,Double_t y2,
                   Double_t x3,Double_t y3) 
{
  //-----------------------------------------------------------------
  // Initial approximation of the track curvature (Y. Belikov) squared
  //-----------------------------------------------------------------
  Double_t d=(x2-x1)*(y3-y2)-(x3-x2)*(y2-y1);
  Double_t a=0.5*((y3-y2)*(y2*y2-y1*y1+x2*x2-x1*x1)-
                  (y2-y1)*(y3*y3-y2*y2+x3*x3-x2*x2));
  Double_t b=0.5*((x2-x1)*(y3*y3-y2*y2+x3*x3-x2*x2)-
                  (x3-x2)*(y2*y2-y1*y1+x2*x2-x1*x1));

  Double_t xr=TMath::Abs(d/(d*x1-a)), yr=d/(d*y1-b);

  Float_t val = (Float_t)(( -xr*yr/TMath::Sqrt(xr*xr+yr*yr))
			  *( -xr*yr/TMath::Sqrt(xr*xr+yr*yr)));
  return val; 
}

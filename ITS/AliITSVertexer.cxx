#include "AliLog.h"
#include "AliMultiplicity.h"
#include "AliITSgeomTGeo.h"
#include "AliITSVertexer.h"
#include "AliITSLoader.h"
#include "AliITSMultReconstructor.h"

const Float_t AliITSVertexer::fgkPipeRadius = 3.0;

ClassImp(AliITSVertexer)

//////////////////////////////////////////////////////////////////////
// Base class for primary vertex reconstruction                     //
// AliESDVertexer is a class for full 3D primary vertex finding     //
// derived classes: AliITSVertexerIons AliITSvertexer3D             //
//                  AliITSVertexerCosmics                           //
//////////////////////////////////////////////////////////////////////

//______________________________________________________________________
AliITSVertexer::AliITSVertexer():AliVertexer(),
fLadders(), 
fLadOnLay2(0),
fFirstEvent(0),
fLastEvent(-1)
{
  // Default Constructor
  SetLaddersOnLayer2();
}

//______________________________________________________________________
AliITSVertexer::~AliITSVertexer() {
  // Destructor
 if(fLadders) delete [] fLadders;
}

//______________________________________________________________________
void AliITSVertexer::FindMultiplicity(TTree *itsClusterTree){
  // Invokes AliITSMultReconstructor to determine the
  // charged multiplicity in the pixel layers
  if(fMult){delete fMult; fMult = 0;}
  Bool_t success=kTRUE;
  if(!fCurrentVertex)success=kFALSE;
  if(fCurrentVertex && fCurrentVertex->GetNContributors()<1)success=kFALSE;

  AliITSMultReconstructor multReco;

  if(!success){
    AliWarning("Tracklets multiplicity not determined because the primary vertex was not found");
    AliWarning("Just counting the number of cluster-fired chips on the SPD layers");
    if (!itsClusterTree) {
      AliError(" Invalid ITS cluster tree !\n");
      return;
    }
    multReco.LoadClusterFiredChips(itsClusterTree);
    Short_t nfcL1 = multReco.GetNFiredChips(0);
    Short_t nfcL2 = multReco.GetNFiredChips(1);
    fMult = new AliMultiplicity(0,0,0,0,0,0,0,0,0,nfcL1,nfcL2);
    return;
  }

  if (!itsClusterTree) {
    AliError(" Invalid ITS cluster tree !\n");
    return;
  }
  Double_t vtx[3];
  fCurrentVertex->GetXYZ(vtx);
  Float_t vtxf[3];
  for(Int_t i=0;i<3;i++)vtxf[i]=vtx[i];
  multReco.SetHistOn(kFALSE);
  multReco.Reconstruct(itsClusterTree,vtxf,vtxf);
  Int_t notracks=multReco.GetNTracklets();
  Float_t *tht = new Float_t [notracks];
  Float_t *phi = new Float_t [notracks];
  Float_t *dphi = new Float_t [notracks];
  Int_t *labels = new Int_t[notracks];
  Int_t *labelsL2 = new Int_t[notracks];
  for(Int_t i=0;i<multReco.GetNTracklets();i++){
    tht[i] = multReco.GetTracklet(i)[0];
    phi[i] =  multReco.GetTracklet(i)[1];
    dphi[i] = multReco.GetTracklet(i)[2];
    labels[i] = static_cast<Int_t>(multReco.GetTracklet(i)[3]);
    labelsL2[i] = static_cast<Int_t>(multReco.GetTracklet(i)[4]);
  }
  Int_t nosingleclus=multReco.GetNSingleClusters();
  Float_t *ths = new Float_t [nosingleclus];
  Float_t *phs = new Float_t [nosingleclus];
  for(Int_t i=0;i<nosingleclus;i++){
    ths[i] = multReco.GetCluster(i)[0];
    phs[i] = multReco.GetCluster(i)[1];
  }
  Short_t nfcL1 = multReco.GetNFiredChips(0);
  Short_t nfcL2 = multReco.GetNFiredChips(1);
  fMult = new AliMultiplicity(notracks,tht,phi,dphi,labels,labelsL2,nosingleclus,ths,phs,nfcL1,nfcL2);

  delete [] tht;
  delete [] phi;
  delete [] dphi;
  delete [] ths;
  delete [] phs;
  delete [] labels;
  delete [] labelsL2;

  return;
}

//______________________________________________________________________
void AliITSVertexer::SetLaddersOnLayer2(Int_t ladwid){
  // Calculates the array of ladders on layer 2 to be used with a 
  // given ladder on layer 1
  fLadOnLay2=ladwid;
  Int_t ladtot1=AliITSgeomTGeo::GetNLadders(1);
  if(fLadders) delete [] fLadders;
  fLadders=new UShort_t[ladtot1];


  Double_t pos1[3],pos2[3];
  Int_t mod1=AliITSgeomTGeo::GetModuleIndex(2,1,1);
  AliITSgeomTGeo::GetTranslation(mod1,pos1);  // position of the module in the MRS 
  Double_t phi0=TMath::ATan2(pos1[1],pos1[0]);
  if(phi0<0) phi0+=2*TMath::Pi();
  Int_t mod2=AliITSgeomTGeo::GetModuleIndex(2,2,1);
  AliITSgeomTGeo::GetTranslation(mod2,pos2);
  Double_t phi2=TMath::ATan2(pos2[1],pos2[0]); 
  if(phi2<0) phi2+=2*TMath::Pi();
  Double_t deltaPhi= phi0-phi2; // phi width of a layer2 module

  for(Int_t i= 0; i<ladtot1;i++){
    Int_t modlad= AliITSgeomTGeo::GetModuleIndex(1,i+1,1);
    Double_t posmod[3];
    AliITSgeomTGeo::GetTranslation(modlad,posmod);
    Double_t phimod=TMath::ATan2(posmod[1],posmod[0]); 
    if(phimod<0) phimod+=2*TMath::Pi();
    Double_t phi1= phimod+deltaPhi*double(fLadOnLay2);
    if(phi1<0) phi1+=2*TMath::Pi();
    if(phi1>2*TMath::Pi()) phi1-=2*TMath::Pi();
    Double_t philad1=phi0-phi1;
    UShort_t lad1;
    Double_t ladder1=(philad1)/(deltaPhi) +1.; 
    if(ladder1<1){ladder1=40+ladder1;}
    lad1=int(ladder1+0.5);
    fLadders[i]=lad1;
  }
}

#include "AliRunLoader.h"

//______________________________________________________________________
void AliITSVertexer::Init(TString filename){
  // Initialize the vertexer in case of
  // analysis of an entire file
  AliRunLoader *rl = AliRunLoader::GetRunLoader();
  if(!rl){
    Fatal("AliITSVertexer","Run Loader not found");
  }
  if (fLastEvent < 0) SetLastEvent(rl->GetNumberOfEvents()-1);

  AliITSLoader* itsloader =  (AliITSLoader*) rl->GetLoader("ITSLoader");
  if(!filename.Contains("default"))itsloader->SetVerticesFileName(filename);
  if(!filename.Contains("null"))itsloader->LoadVertices("recreate");
}

//______________________________________________________________________
void AliITSVertexer::WriteCurrentVertex(){
  // Write the current AliVertex object to file fOutFile
  AliRunLoader *rl = AliRunLoader::GetRunLoader();
  AliITSLoader* itsLoader =  (AliITSLoader*) rl->GetLoader("ITSLoader");
  fCurrentVertex->SetName("Vertex");
  //  const char * name = fCurrentVertex->GetName();
  //  itsLoader->SetVerticesContName(name);
  Int_t rc = itsLoader->PostVertex(fCurrentVertex);
  rc = itsLoader->WriteVertices();
}

//______________________________________________________________________
void AliITSVertexer::FindVertices(){
  // computes the vertices of the events in the range FirstEvent - LastEvent

  AliRunLoader *rl = AliRunLoader::GetRunLoader();
  AliITSLoader* itsloader =  (AliITSLoader*) rl->GetLoader("ITSLoader");
  itsloader->LoadRecPoints("read");
  for(Int_t i=fFirstEvent;i<=fLastEvent;i++){
    rl->GetEvent(i);
    TTree* cltree = itsloader->TreeR();
    FindVertexForCurrentEvent(cltree);
    if(fCurrentVertex){
      WriteCurrentVertex();
    }
    else {
      AliDebug(1,Form("Vertex not found for event %d",i));
    }
  }
}

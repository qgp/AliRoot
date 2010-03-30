//------------------------------------------------------------------------------
// Implementation of AliPerformanceTPC class. It keeps information from 
// comparison of reconstructed and MC particle tracks. In addtion, 
// it keeps selection cuts used during comparison. The comparison 
// information is stored in the ROOT histograms. Analysis of these 
// histograms can be done by using Analyse() class function. The result of 
// the analysis (histograms/graphs) are stored in the folder which is
// a data member of AliPerformanceTPC.
//
// Author: J.Otwinowski 04/02/2008 
//------------------------------------------------------------------------------

/*
 
  // after running comparison task, read the file, and get component
  gROOT->LoadMacro("$ALICE_ROOT/PWG1/Macros/LoadMyLibs.C");
  LoadMyLibs();

  TFile f("Output.root");
  AliPerformanceTPC * compObj = (AliPerformanceTPC*)coutput->FindObject("AliPerformanceTPC");
 
  // analyse comparison data
  compObj->Analyse();

  // the output histograms/graphs will be stored in the folder "folderTPC" 
  compObj->GetAnalysisFolder()->ls("*");

  // user can save whole comparison object (or only folder with anlysed histograms) 
  // in the seperate output file (e.g.)
  TFile fout("Analysed_TPC.root","recreate");
  compObj->Write(); // compObj->GetAnalysisFolder()->Write();
  fout.Close();

*/

#include "TCanvas.h"
#include "TH1.h"
#include "TH2.h"
#include "TAxis.h"
#include "TPostScript.h"

#include "AliPerformanceTPC.h" 
#include "AliESDEvent.h" 
#include "AliESDVertex.h"
#include "AliESDtrack.h"
#include "AliLog.h" 
#include "AliMCEvent.h" 
#include "AliHeader.h" 
#include "AliGenEventHeader.h" 
#include "AliStack.h" 
#include "AliMCInfoCuts.h" 
#include "AliRecInfoCuts.h" 
#include "AliTracker.h" 
#include "AliTreeDraw.h" 
#include "AliTPCTransform.h" 
#include "AliTPCseed.h" 
#include "AliTPCcalibDB.h" 
#include "AliESDfriend.h" 
#include "AliESDfriendTrack.h" 
#include "AliTPCclusterMI.h" 

using namespace std;

ClassImp(AliPerformanceTPC)

//_____________________________________________________________________________
AliPerformanceTPC::AliPerformanceTPC():
  AliPerformanceObject("AliPerformanceTPC"),
  fTPCClustHisto(0),
  fTPCEventHisto(0),
  fTPCTrackHisto(0),

  // Cuts 
  fCutsRC(0),  
  fCutsMC(0),  

  // histogram folder 
  fAnalysisFolder(0)
{
  Init();
}

//_____________________________________________________________________________
AliPerformanceTPC::AliPerformanceTPC(Char_t* name="AliPerformanceTPC", Char_t* title="AliPerformanceTPC",Int_t analysisMode=0,Bool_t hptGenerator=kFALSE):
  AliPerformanceObject(name,title),
  fTPCClustHisto(0),
  fTPCEventHisto(0),
  fTPCTrackHisto(0),

  // Cuts 
  fCutsRC(0),  
  fCutsMC(0),  

  // histogram folder 
  fAnalysisFolder(0)
{
  // named constructor	
  // 
  SetAnalysisMode(analysisMode);
  SetHptGenerator(hptGenerator);

  Init();
}

//_____________________________________________________________________________
AliPerformanceTPC::~AliPerformanceTPC()
{
  // destructor
   
  if(fTPCClustHisto) delete fTPCClustHisto; fTPCClustHisto=0;     
  if(fTPCEventHisto) delete fTPCEventHisto; fTPCEventHisto=0;     
  if(fTPCTrackHisto) delete fTPCTrackHisto; fTPCTrackHisto=0;     
  if(fAnalysisFolder) delete fAnalysisFolder; fAnalysisFolder=0;
}

//_____________________________________________________________________________
void AliPerformanceTPC::Init(){
  //
  // histogram bining
  //

  // set pt bins
  Int_t nPtBins = 50;
  Double_t ptMin = 1.e-2, ptMax = 10.;

  Double_t *binsPt = 0;
  if (IsHptGenerator())  { 
    nPtBins = 100; ptMax = 100.;
    binsPt = CreateLogAxis(nPtBins,ptMin,ptMax);
  } else {
    binsPt = CreateLogAxis(nPtBins,ptMin,ptMax);
  }

  /*
  Int_t nPtBins = 31;
  Double_t binsPt[32] = {0.,0.05,0.1,0.15,0.2,0.25,0.3,0.35,0.4,0.45,0.5,0.55,0.6,0.7,0.8,0.9,1.0,1.2,1.4,1.6,1.8,2.0,2.25,2.5,2.75,3.,3.5,4.,5.,6.,8.,10.};
  Double_t ptMin = 0., ptMax = 10.; 

  if(IsHptGenerator() == kTRUE) {
    nPtBins = 100;
    ptMin = 0.; ptMax = 100.; 
  }
  */
  // 
  //gclX:gclY:TPCSide
  Int_t binsTPCClustHisto[3]=  { 500,   500,  2 };
  Double_t minTPCClustHisto[3]={-250., -250., 0.};
  Double_t maxTPCClustHisto[3]={ 250.,  250., 2.};

  fTPCClustHisto = new THnSparseF("fTPCClustHisto","gclX:gclY:TPCSide",3,binsTPCClustHisto,minTPCClustHisto,maxTPCClustHisto);
  fTPCClustHisto->GetAxis(0)->SetTitle("gclX (cm)");
  fTPCClustHisto->GetAxis(1)->SetTitle("gclY (cm)");
  fTPCClustHisto->GetAxis(2)->SetTitle("TPCSide");
  fTPCClustHisto->Sumw2();
 

  // Xv:Yv:Zv:mult:multP:multN:vertStatus
  Int_t binsTPCEventHisto[7]=  {100,  100,   100,  151,   151,   151, 2   };
  Double_t minTPCEventHisto[7]={-10., -10., -30.,  -0.5,  -0.5,  -0.5, 0.  };
  Double_t maxTPCEventHisto[7]={ 10.,  10.,  30.,  150.5, 150.5, 150.5, 2. };

  fTPCEventHisto = new THnSparseF("fTPCEventHisto","Xv:Yv:Zv:mult:multP:multN:vertStatus",7,binsTPCEventHisto,minTPCEventHisto,maxTPCEventHisto);
  fTPCEventHisto->GetAxis(0)->SetTitle("Xv (cm)");
  fTPCEventHisto->GetAxis(1)->SetTitle("Yv (cm)");
  fTPCEventHisto->GetAxis(2)->SetTitle("Zv (cm)");
  fTPCEventHisto->GetAxis(3)->SetTitle("mult");
  fTPCEventHisto->GetAxis(4)->SetTitle("multP");
  fTPCEventHisto->GetAxis(5)->SetTitle("multN");
  fTPCEventHisto->GetAxis(6)->SetTitle("vertStatus");
  fTPCEventHisto->Sumw2();


  // nTPCClust:chi2PerTPCClust:nTPCClustFindRatio:DCAr:DCAz:eta:phi:pt:charge
  Int_t binsTPCTrackHisto[9]=  { 160,  50,  60,  100, 100,  30,   144,             nPtBins,  3 };
  Double_t minTPCTrackHisto[9]={ 0.,   0.,  0., -10,  -10., -1.5, 0.,             ptMin,   -1.5};
  Double_t maxTPCTrackHisto[9]={ 160., 10., 1.2, 10,   10.,  1.5, 2.*TMath::Pi(), ptMax,    1.5};

  fTPCTrackHisto = new THnSparseF("fTPCTrackHisto","nClust:chi2PerClust:nClust/nFindableClust:DCAr:DCAz:eta:phi:pt:charge",9,binsTPCTrackHisto,minTPCTrackHisto,maxTPCTrackHisto);
  fTPCTrackHisto->SetBinEdges(7,binsPt);

  fTPCTrackHisto->GetAxis(0)->SetTitle("nClust");
  fTPCTrackHisto->GetAxis(1)->SetTitle("chi2PerClust");
  fTPCTrackHisto->GetAxis(2)->SetTitle("nClust/nFindableClust");
  fTPCTrackHisto->GetAxis(3)->SetTitle("DCAr (cm)");
  fTPCTrackHisto->GetAxis(4)->SetTitle("DCAz (cm)");
  fTPCTrackHisto->GetAxis(5)->SetTitle("#eta");
  fTPCTrackHisto->GetAxis(6)->SetTitle("#phi (rad)");
  fTPCTrackHisto->GetAxis(7)->SetTitle("p_{T} (GeV/c)");
  fTPCTrackHisto->GetAxis(8)->SetTitle("charge");
  fTPCTrackHisto->Sumw2();

  // Init cuts 
  if(!fCutsMC) 
    AliDebug(AliLog::kError, "ERROR: Cannot find AliMCInfoCuts object");
  if(!fCutsRC) 
    AliDebug(AliLog::kError, "ERROR: Cannot find AliRecInfoCuts object");

  // init folder
  fAnalysisFolder = CreateFolder("folderTPC","Analysis Resolution Folder");
}

//_____________________________________________________________________________
void AliPerformanceTPC::ProcessTPC(AliStack* const stack, AliESDtrack *const esdTrack)
{
  if(!esdTrack) return;

  // Fill TPC only resolution comparison information 
  const AliExternalTrackParam *track = esdTrack->GetTPCInnerParam();
  if(!track) return;

  Float_t dca[2], cov[3]; // dca_xy, dca_z, sigma_xy, sigma_xy_z, sigma_z
  esdTrack->GetImpactParametersTPC(dca,cov);

  Float_t q = esdTrack->Charge();
  Float_t pt = track->Pt();
  Float_t eta = track->Eta();
  Float_t phi = track->Phi();
  Int_t nClust = esdTrack->GetTPCclusters(0);
  Int_t nFindableClust = esdTrack->GetTPCNclsF();

  Float_t chi2PerCluster = 0.;
  if(nClust>0.) chi2PerCluster = esdTrack->GetTPCchi2()/Float_t(nClust);

  Float_t clustPerFindClust = 0.;
  if(nFindableClust>0.) clustPerFindClust = Float_t(nClust)/nFindableClust;
  
  //
  // select primaries
  //
  Double_t dcaToVertex = -1;
  if( fCutsRC->GetDCAToVertex2D() ) 
  {
      dcaToVertex = TMath::Sqrt(dca[0]*dca[0]/fCutsRC->GetMaxDCAToVertexXY()/fCutsRC->GetMaxDCAToVertexXY()                    + dca[1]*dca[1]/fCutsRC->GetMaxDCAToVertexZ()/fCutsRC->GetMaxDCAToVertexZ()); 
  }
  if(fCutsRC->GetDCAToVertex2D() && dcaToVertex > 1) return;
  if(!fCutsRC->GetDCAToVertex2D() && TMath::Abs(dca[0]) > fCutsRC->GetMaxDCAToVertexXY()) return;
  if(!fCutsRC->GetDCAToVertex2D() && TMath::Abs(dca[1]) > fCutsRC->GetMaxDCAToVertexZ()) return;

  Double_t vTPCTrackHisto[9] = {nClust,chi2PerCluster,clustPerFindClust,dca[0],dca[1],eta,phi,pt,q};
  fTPCTrackHisto->Fill(vTPCTrackHisto); 
 
  //
  // Fill rec vs MC information
  //
  if(!stack) return;

}

//_____________________________________________________________________________
void AliPerformanceTPC::ProcessTPCITS(AliStack* const /*stack*/, AliESDtrack *const /*esdTrack*/)
{
  // Fill comparison information (TPC+ITS) 
  AliDebug(AliLog::kWarning, "Warning: Not implemented");
}
 
//_____________________________________________________________________________
void AliPerformanceTPC::ProcessConstrained(AliStack* const /*stack*/, AliESDtrack *const /*esdTrack*/)
{
  // Fill comparison information (constarained parameters) 
  AliDebug(AliLog::kWarning, "Warning: Not implemented");
}
 
//_____________________________________________________________________________
void AliPerformanceTPC::Exec(AliMCEvent* const mcEvent, AliESDEvent *const esdEvent, AliESDfriend *const esdFriend, const Bool_t bUseMC, const Bool_t bUseESDfriend)
{
  // Process comparison information 
  //
  if(!esdEvent) 
  {
    Error("Exec","esdEvent not available");
    return;
  }
  AliHeader* header = 0;
  AliGenEventHeader* genHeader = 0;
  AliStack* stack = 0;
  TArrayF vtxMC(3);
  
  if(bUseMC)
  {
    if(!mcEvent) {
      Error("Exec","mcEvent not available");
      return;
    }
    // get MC event header
    header = mcEvent->Header();
    if (!header) {
      Error("Exec","Header not available");
      return;
    }
    // MC particle stack
    stack = mcEvent->Stack();
    if (!stack) {
      Error("Exec","Stack not available");
      return;
    }
    // get MC vertex
    genHeader = header->GenEventHeader();
    if (!genHeader) {
      Error("Exec","Could not retrieve genHeader from Header");
      return;
    }
    genHeader->PrimaryVertex(vtxMC);
  } 
  
  // use ESD friends
  if(bUseESDfriend) {
    if(!esdFriend) {
      Error("Exec","esdFriend not available");
      return;
    }
  }

  // trigger
  if(!bUseMC) {
    Bool_t isEventTriggered = esdEvent->IsTriggerClassFired(GetTriggerClass());
    if(!isEventTriggered) return; 
  }

  // get TPC event vertex
  const AliESDVertex *vtxESD = esdEvent->GetPrimaryVertexTPC();

  //  events with rec. vertex
  Int_t mult=0; Int_t multP=0; Int_t multN=0;
  if(vtxESD->GetStatus() >0)
  {
  //  Process ESD events
  for (Int_t iTrack = 0; iTrack < esdEvent->GetNumberOfTracks(); iTrack++) 
  { 
    AliESDtrack *track = esdEvent->GetTrack(iTrack);
    if(!track) continue;

    if(bUseESDfriend) {
    AliESDfriendTrack *friendTrack=esdFriend->GetTrack(iTrack);
    if(!friendTrack) continue;

      TObject *calibObject=0;
      AliTPCseed *seed=0;
      if (!friendTrack) continue;
        for (Int_t j=0;(calibObject=friendTrack->GetCalibObject(j));++j) {
	  if ((seed=dynamic_cast<AliTPCseed*>(calibObject)))
	  break;
        }

        //AliTPCTransform *transform = AliTPCcalibDB::Instance()->GetTransform() ;
	for (Int_t irow=0;irow<159;irow++) {
	if(!seed) continue;
	  
	  AliTPCclusterMI *cluster=seed->GetClusterPointer(irow);
	  if (!cluster) continue;

	      Float_t gclf[3];
	      cluster->GetGlobalXYZ(gclf);

	      //Double_t x[3]={cluster->GetRow(),cluster->GetPad(),cluster->GetTimeBin()};
	      //Int_t i[1]={cluster->GetDetector()};
              //transform->Transform(x,i,0,1);
	      //printf("gx %f gy  %f  gz %f \n", cluster->GetX(), cluster->GetY(),cluster->GetZ());
	      //printf("gclf[0] %f gclf[1]  %f  gclf[2] %f \n", gclf[0], gclf[1],  gclf[2]);
     
             Int_t TPCside; 
	     if(gclf[2]>0.) TPCside=0; // A side 
	     else TPCside=1;

             Double_t vTPCClust[3] = { gclf[0], gclf[1],  TPCside };
             fTPCClustHisto->Fill(vTPCClust);
         }
    }
    if(GetAnalysisMode() == 0) ProcessTPC(stack,track);
    else if(GetAnalysisMode() == 1) ProcessTPCITS(stack,track);
    else if(GetAnalysisMode() == 2) ProcessConstrained(stack,track);
    else {
      printf("ERROR: AnalysisMode %d \n",fAnalysisMode);
      return;
    }

   // TPC only
   AliESDtrack *tpcTrack = AliESDtrackCuts::GetTPCOnlyTrack(esdEvent,iTrack);
   if(!tpcTrack) continue;

   // track selection
   if( fCutsRC->AcceptTrack(tpcTrack) ) { 
     mult++;
     if(tpcTrack->Charge()>0.) multP++;
     if(tpcTrack->Charge()<0.) multN++;
   }

   if(tpcTrack) delete tpcTrack;
  }
  }
  //
  
  Double_t vTPCEvent[7] = {vtxESD->GetXv(),vtxESD->GetYv(),vtxESD->GetZv(),mult,multP,multN,vtxESD->GetStatus()};
  fTPCEventHisto->Fill(vTPCEvent);
}

//_____________________________________________________________________________
void AliPerformanceTPC::Analyse() {
  //
  // Analyse comparison information and store output histograms
  // in the folder "folderTPC"
  //
  TH1::AddDirectory(kFALSE);
  TH1F *h=0;
  TH2D *h2D=0;
  TObjArray *aFolderObj = new TObjArray;
  char name[256];
  char title[256];

  //
  // Cluster histograms
  //
  fTPCClustHisto->GetAxis(2)->SetRange(1,1); // A-side
  h2D = fTPCClustHisto->Projection(1,0);
  h2D->SetName("h_clust_A_side");
  h2D->SetTitle("gclX:gclY - A_side");
  aFolderObj->Add(h2D);

  fTPCClustHisto->GetAxis(2)->SetRange(2,2); // C-side
  h2D = fTPCClustHisto->Projection(1,0);
  h2D->SetName("h_clust_C_side");
  h2D->SetTitle("gclX:gclY - C_side");
  aFolderObj->Add(h2D);

  //
  // event histograms
  //
  for(Int_t i=0; i<6; i++) 
  {
      h = (TH1F*)fTPCEventHisto->Projection(i);
      sprintf(name,"h_tpc_event_%d",i);
      h->SetName(name);
      h->GetXaxis()->SetTitle(fTPCEventHisto->GetAxis(i)->GetTitle());
      h->GetYaxis()->SetTitle("events");
      sprintf(title,"%s",fTPCEventHisto->GetAxis(i)->GetTitle());
      h->SetTitle(title);

      aFolderObj->Add(h);
  }

  // reconstructed vertex status > 0
  fTPCEventHisto->GetAxis(6)->SetRange(2,2);
  for(Int_t i=0; i<6; i++) 
  {
      h = (TH1F*)fTPCEventHisto->Projection(i);
      sprintf(name,"h_tpc_event_recVertex%d",i);
      h->SetName(name);
      h->GetXaxis()->SetTitle(fTPCEventHisto->GetAxis(i)->GetTitle());
      h->GetYaxis()->SetTitle("events");
      sprintf(title,"%s rec. vertex",fTPCEventHisto->GetAxis(i)->GetTitle());
      h->SetTitle(title);

      aFolderObj->Add(h);
  }

  //
  // Track histograms
  //
  for(Int_t i=0; i<9; i++) 
  {
      h = (TH1F*)fTPCTrackHisto->Projection(i);
      sprintf(name,"h_tpc_track_%d",i);
      h->SetName(name);
      h->GetXaxis()->SetTitle(fTPCTrackHisto->GetAxis(i)->GetTitle());
      h->GetYaxis()->SetTitle("tracks");
      sprintf(title,"%s",fTPCTrackHisto->GetAxis(i)->GetTitle());
      h->SetTitle(title);

      if(i==7) h->Scale(1,"width");
      aFolderObj->Add(h);
  }

  //
  for(Int_t i=0; i<8; i++) 
  {
    for(Int_t j=i+1; j<9; j++) 
    {
      h2D = fTPCTrackHisto->Projection(i,j);
      sprintf(name,"h_tpc_track_%d_vs_%d",i,j);
      h2D->SetName(name);
      h2D->GetXaxis()->SetTitle(fTPCTrackHisto->GetAxis(j)->GetTitle());
      h2D->GetYaxis()->SetTitle(fTPCTrackHisto->GetAxis(i)->GetTitle());
      sprintf(title,"%s vs %s",fTPCTrackHisto->GetAxis(j)->GetTitle(),fTPCTrackHisto->GetAxis(i)->GetTitle());
      h2D->SetTitle(title);

      if(j==7) h2D->SetBit(TH1::kLogX);
      aFolderObj->Add(h2D);
    }  
  }

  // export objects to analysis folder
  fAnalysisFolder = ExportToFolder(aFolderObj);

  // delete only TObjArray
  if(aFolderObj) delete aFolderObj;
}

//_____________________________________________________________________________
TFolder* AliPerformanceTPC::ExportToFolder(TObjArray * array) 
{
  // recreate folder avery time and export objects to new one
  //
  AliPerformanceTPC * comp=this;
  TFolder *folder = comp->GetAnalysisFolder();

  TString name, title;
  TFolder *newFolder = 0;
  Int_t i = 0;
  Int_t size = array->GetSize();

  if(folder) { 
     // get name and title from old folder
     name = folder->GetName();  
     title = folder->GetTitle();  

	 // delete old one
     delete folder;

	 // create new one
     newFolder = CreateFolder(name.Data(),title.Data());
     newFolder->SetOwner();

	 // add objects to folder
     while(i < size) {
	   newFolder->Add(array->At(i));
	   i++;
	 }
  }

return newFolder;
}

//_____________________________________________________________________________
Long64_t AliPerformanceTPC::Merge(TCollection* const list) 
{
  // Merge list of objects (needed by PROOF)

  if (!list)
  return 0;

  if (list->IsEmpty())
  return 1;

  TIterator* iter = list->MakeIterator();
  TObject* obj = 0;

  // collection of generated histograms
  Int_t count=0;
  while((obj = iter->Next()) != 0) 
  {
    AliPerformanceTPC* entry = dynamic_cast<AliPerformanceTPC*>(obj);
    if (entry == 0) continue; 

    fTPCClustHisto->Add(entry->fTPCClustHisto);
    fTPCEventHisto->Add(entry->fTPCEventHisto);
    fTPCTrackHisto->Add(entry->fTPCTrackHisto);

    count++;
  }

return count;
}

//_____________________________________________________________________________
TFolder* AliPerformanceTPC::CreateFolder(TString name,TString title) { 
// create folder for analysed histograms
//
TFolder *folder = 0;
  folder = new TFolder(name.Data(),title.Data());

  return folder;
}

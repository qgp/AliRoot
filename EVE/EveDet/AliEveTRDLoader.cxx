// $Id$
// Main authors: Matevz Tadel & Alja Mrak-Tadel: 2006, 2007

/**************************************************************************
 * Copyright(c) 1998-2008, ALICE Experiment at CERN, all rights reserved. *
 * See http://aliceinfo.cern.ch/Offline/AliRoot/License.html for          *
 * full copyright notice.                                                 *
 **************************************************************************/

#include "AliEveTRDLoader.h"
#include "AliEveTRDModuleImp.h"
#include "EveBase/AliEveEventManager.h"

#include <TEveManager.h>
#include <TEveGValuators.h>

#include "TGeoManager.h"
#include "TSystem.h"
#include "TFile.h"
#include "TTree.h"
#include "TObjString.h"
#include "TObjArray.h"
#include "TClonesArray.h"

#include <TGLabel.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <TGFileDialog.h>

#include "AliLog.h"
#include "AliCDBManager.h"

#include "AliTRDhit.h"
#include "AliTRDcluster.h"
#include "AliTRDmcmTracklet.h"
#include "AliTRDdigitsManager.h"
#include "AliTRDgeometry.h"

class AliTRDdataArrayI;

ClassImp(AliEveTRDLoader)
ClassImp(AliEveTRDLoaderEditor)

///////////////////////////////////////////////////////////
/////////////     AliEveTRDLoader     /////////////////////
///////////////////////////////////////////////////////////


//______________________________________________________________________________
AliEveTRDLoader::AliEveTRDLoader(const Text_t* n, const Text_t* t) : TEveElementList(n, t)
  ,fDataType(0)
  ,fSM(-1)
  ,fStack(-1)
  ,fLy(-1)
  ,fEvent(-1)
  ,fGeo(0x0)
  ,fFilename("")
  ,fDir(".")
{
  // Constructor.

  AliCDBManager *fCDBManager=AliCDBManager::Instance();
  fCDBManager->SetDefaultStorage("local://$ALICE_ROOT");
  fCDBManager->SetRun(0);

  if(!AliEveEventManager::AssertGeometry()){
    TGeoManager::Import("geometry.root");
  }

  fGeo = new AliTRDgeometry();
  //fGeo->CreateClusterMatrixArray();
}

//______________________________________________________________________________
void AliEveTRDLoader::AddChambers(int sm, int stk, int ly)
{
  // Add specified chambers.

  fSM=sm; fStack=stk; fLy=ly;
  Int_t ismStart = (sm == -1) ?  0 : sm;
  Int_t ismStop  = (sm == -1) ? 18 : sm+1;
  Int_t istkStart= (stk == -1)?  0 : stk;
  Int_t istkStop = (stk == -1)?  5 : stk+1;
  Int_t ilyStart = (ly == -1) ?  0 : ly;
  Int_t ilyStop  = (ly == -1) ?  6 : ly+1;

  List_i ichmb;
  ichmb = fChildren.begin();
  while(ichmb != fChildren.end()){
    (*ichmb)->SetRnrSelf(kFALSE);
    ichmb++;
  }

  AliEveTRDNode *lSM=0x0, *lSTK=0x0;
  AliEveTRDChamber *lCHMB = 0x0;
  int det;
  for (int ism=ismStart; ism<ismStop; ism++){
    if(!(lSM = (AliEveTRDNode*)FindChild(Form("SM%03d", ism)))){
      AddElement(lSM = new AliEveTRDNode("SM", ism));
      lSM->SetElementTitle(Form("Supermodule %2d", ism));
    }
    lSM->SetRnrSelf(kTRUE);

    for (int istk=istkStart; istk<istkStop; istk++) {
      if(!(lSTK = (AliEveTRDNode*)lSM->FindChild("Stack%03d"))){
        lSM->AddElement(lSTK = new AliEveTRDNode("Stack", istk));
        lSTK->SetElementTitle(Form("SM %2d Stack %1d", ism, istk));
      }
      lSTK->SetRnrSelf(kTRUE);
      
      for (int ily=ilyStart; ily<ilyStop; ily++) {
        det = fGeo->GetDetector(ily, istk, ism);
        if(!(lCHMB = (AliEveTRDChamber*)lSTK->FindChild(Form("Chmb%03d", det)))){
          lSTK->AddElement(lCHMB = new AliEveTRDChamber(det));
          lCHMB->SetGeometry(fGeo);
          lCHMB->SetElementTitle(Form("SM %2d Stack %1d Layer %1d", ism, istk, ily));
        }
        lCHMB->SetRnrSelf(kTRUE);
      }
    }
  }
  gEve->Redraw3D();
}

//______________________________________________________________________________
AliEveTRDChamber* AliEveTRDLoader::GetChamber(int d)
{
  // Get given chamber.

  Int_t ism  = fGeo->GetSector(d), 
        istk = fGeo->GetChamber(d); 
  
  AliEveTRDNode *node = 0x0;
  if(!(node = (AliEveTRDNode*)FindChild(Form("SM%03d", ism)))) return 0x0;
  if(!(node = (AliEveTRDNode*)node->FindChild(Form("Stack%03d", istk)))) return 0x0;
  return (AliEveTRDChamber*)node->FindChild(Form("Chmb%03d", d));
}

//______________________________________________________________________________
Bool_t AliEveTRDLoader::GoToEvent(int ev)
{
  // Go to given event.

  if(!fChildren.size()){
    AliWarning("Please select first the chamber that you want to monitor from \"Chamber(s) selector\".");
    return kFALSE;
  }

  fEvent = ev;

  Unload();

  TTree *t = 0x0;
  TFile *f = TFile::Open(Form("%s/%s", fDir.Data(), fFilename.Data()));
  if(! f->cd(Form("Event%d", ev))){
    AliError(Form("Couldn't find event %d in file \"%s/%s\".", ev, fDir.Data(), fFilename.Data()));
    f->Close(); //delete f;
    return kFALSE;
  }

  if(fDataType&kTRDHits){
    t = (TTree*)gDirectory->Get("TreeH");
    if(!t) return kFALSE;
    if(!LoadHits(t)) return kFALSE;
  } else if(fDataType&kTRDDigits){
    t = (TTree*)gDirectory->Get("TreeD");
    if(!t) return kFALSE;
    if(!LoadDigits(t)) return kFALSE;
  } else if(fDataType&kTRDClusters){
    t = (TTree*)gDirectory->Get("TreeR");
    if(!t) return kFALSE;
    if(!LoadClusters(t)) return kFALSE;
  } else if(fDataType&kTRDTracklets){
    t = (TTree*)gDirectory->Get("TreeT");
    if(!t) return kFALSE;
    if(!LoadTracklets(t)) return kFALSE;
  } else AliWarning("Please select first the type of data that you want to monitor and then hit the \"Load\" button.");

  f->Close(); //delete f;

  gEve->Redraw3D();

  return kTRUE;
}


//______________________________________________________________________________
Bool_t AliEveTRDLoader::LoadHits(TTree *tH)
{
  // Load hits.

  AliInfo("Loading ...");
  if(!fChildren.size()) return kFALSE;

  AliEveTRDChamber *chmb = 0x0;
  TClonesArray *hits = new TClonesArray("AliTRDhit", 100);
  tH->SetBranchAddress("TRD", &hits);
  Int_t idx, nhits;
  for(int iTrack=0; iTrack<tH->GetEntries(); iTrack++){
    if(!tH->GetEvent(iTrack)) continue;
    if(!(nhits = hits->GetEntriesFast())) continue;

    idx = 0;
    while(idx < nhits){
      Int_t det = ((AliTRDhit*)hits->UncheckedAt(idx))->GetDetector();
      chmb = GetChamber(det);
      if(chmb) chmb->LoadHits(hits, idx);
      else{
        AliTRDhit *hit = 0x0;
        while(idx < nhits){
          hit = (AliTRDhit*)hits->UncheckedAt(idx);
          if(hit->GetDetector() != det) break;
          idx++;
        }
      }
    }
    hits->Delete();
  }
  return kTRUE;
}


//______________________________________________________________________________
Bool_t AliEveTRDLoader::LoadClusters(TTree *tC)
{
  // Load clusters.

  AliInfo("Loading ...");
  if(!fChildren.size()) return kTRUE;

  TObjArray *clusters = new TObjArray();
  tC->SetBranchAddress("TRDcluster", &clusters);

  AliEveTRDChamber *chmb = 0x0;
  AliTRDcluster *c=0x0;
  for(int idet=0; idet<540; idet++){
    tC->GetEntry(idet);
    if(!clusters->GetEntriesFast()) continue;
    c = (AliTRDcluster*)clusters->UncheckedAt(0);
    if(!c) continue;
    if((chmb = GetChamber(c->GetDetector()))) chmb->LoadClusters(clusters);
  }
  return kTRUE;
}


//______________________________________________________________________________
Bool_t AliEveTRDLoader::LoadDigits(TTree *tD)
{
  // Load digits.

  AliInfo("Loading ...");

  if(!fChildren.size()) return kTRUE;

  AliEveTRDChamber *chmb;
  AliTRDdigitsManager dm;
  dm.ReadDigits(tD);
  for(int idet=0; idet<540; idet++){
    if(!(chmb=GetChamber(idet))) continue;
    //  digits = dm.GetDigits(idet);
    //  if(!digits) continue;
    //  chmb->LoadDigits(digits);
    chmb->LoadDigits(&dm);
  }
  return kTRUE;
}


//______________________________________________________________________________
Bool_t AliEveTRDLoader::LoadTracklets(TTree *tT)
{
  // Load tracklets.

  AliInfo("Loading ...");
  if(!fChildren.size()) return kTRUE;

  TObjArray *tracks = new TObjArray();
  tT->SetBranchAddress("TRDmcmTracklet",&tracks);

  AliEveTRDChamber *chmb = 0x0;
  AliTRDmcmTracklet *trk=0x0;
  for(int idet=0; idet<540; idet++){
    if(!tT->GetEntry(idet)) continue;
    if(tracks->GetEntriesFast()) trk = (AliTRDmcmTracklet*)tracks->UncheckedAt(0);
    if((chmb = GetChamber(trk->GetDetector()))) chmb->LoadTracklets(tracks);
  }

  return kTRUE;
}


//______________________________________________________________________________
Bool_t AliEveTRDLoader::Open(const char *filename, const char *dir)
{
  // Open given file in given directory.

  fFilename = filename;
  fDir = dir;
  Int_t count = 0;
  count += fDataType&kTRDHits;
  count += fDataType&kTRDDigits;
  count += fDataType&kTRDClusters;
  count += fDataType&kTRDTracklets;

  TObjArray *so = fFilename.Tokenize(".");

  if(((TObjString*)(*so)[0])->GetString().CompareTo("TRD") != 0){
    AliError("Filename didn't fulfill naming conventions. No TRD data will be loaded.");
    return kFALSE;
  }

  if(((TObjString*)(*so)[1])->GetString().CompareTo("Hits") == 0){
    if(count && !fDataType&kTRDHits){ 
      AliWarning("Data type set to HITS according to file name. Previous settings will be overwritten.");
      fDataType = 0; 
    }
    fDataType|=kTRDHits;
  } else   if(((TObjString*)(*so)[1])->GetString().CompareTo("Digits") == 0){
    if(count && !fDataType&kTRDDigits){ 
      AliWarning("Data type set to DIGITS according to file name. Previous settings will be overwritten.");
      fDataType = 0; 
    }
    fDataType|=kTRDDigits;
  } else if(((TObjString*)(*so)[1])->GetString().CompareTo("RecPoints") == 0){
    if(count && !fDataType&kTRDClusters){ 
      AliWarning("Data type set to CLUSTERS according to file name. Previous settings will be overwritten.");
      fDataType = 0; 
    }
    fDataType|=kTRDClusters;  
  } else if(((TObjString*)(*so)[1])->GetString().CompareTo("Tracks") == 0){
    if(count && !fDataType&kTRDTracklets){ 
      AliWarning("Data type set to TRACKLETS according to file name. Previous settings will be overwritten.");
      fDataType = 0; 
    }
    fDataType|=kTRDTracklets; 
  } else {
    AliError("Filename didn't fulfill naming conventions. No data will be loaded.");
    return kFALSE;
  }

  SetDataLinked();
  return kTRUE;
}

//______________________________________________________________________________
void AliEveTRDLoader::Paint(Option_t *option)
{
  // Paint object.

  List_i ichmb = fChildren.begin();
  while(ichmb != fChildren.end()){
    (dynamic_cast<AliEveTRDModule*>(*ichmb))->Paint(option);
    ichmb++;
  }
}


//______________________________________________________________________________
void AliEveTRDLoader::Unload()
{
  // Unload module data.

  List_i ichmb = fChildren.begin();
  while(ichmb != fChildren.end()){
    (dynamic_cast<AliEveTRDModule*>(*ichmb))->Reset();
    ichmb++;
  }
}

///////////////////////////////////////////////////////////
/////////////   AliEveTRDLoaderEditor       /////////////////////
///////////////////////////////////////////////////////////

//______________________________________________________________________________
AliEveTRDLoaderEditor::AliEveTRDLoaderEditor(const TGWindow* p, Int_t width, Int_t height,
					     UInt_t options, Pixel_t back) :
  TGedFrame(p, width, height, options | kVerticalFrame, back),
  fM(0), fFile(0), fBrowse(0x0), fEvent(0),
  fSMNumber(0), fStackNumber(0), fPlaneNumber(0)
{
  // Constructor.

  MakeTitle("AliEveTRDLoader");

  // file browser frame
  Int_t labelW = 42;
  TGHorizontalFrame* f = new TGHorizontalFrame(this);
  TGHorizontalFrame* g = new TGHorizontalFrame(f, labelW, 0, kFixedWidth);
  TGLabel* l = new TGLabel(g, "File: ");
  g->AddFrame(l, new TGLayoutHints(kLHintsLeft, 0,0,4,0));
  f->AddFrame(g);
  fFile = new TGTextEntry(f);
  fFile->SetToolTipText("Select TRD data file or galice.root");
  fFile->SetWidth(140);
  fFile->Connect("DoubleClicked()", "AliEveTRDLoaderEditor", this, "FileOpen()");
  f->AddFrame(fFile);

  fBrowse = new TGTextButton(f, "Browse");
  f->AddFrame(fBrowse);
  fBrowse->Connect("Clicked()", "AliEveTRDLoaderEditor", this, "FileOpen()");
  AddFrame(f, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY | kLHintsExpandX,5,5,5,5));


  // "Chamber(s) selector" group frame
  TGGroupFrame *fGroupFrame1974 = new TGGroupFrame(this,"Chamber(s) selector");
  TGVerticalFrame *fVerticalFrame1974 = new TGVerticalFrame(fGroupFrame1974, 150, 50,kVerticalFrame);

  fSMNumber = new TEveGValuator(fVerticalFrame1974, "SM:", 0, 0);
  fSMNumber->SetShowSlider(kFALSE);
  fSMNumber->SetLabelWidth(labelW);
  fSMNumber->SetNELength(6);
  fSMNumber->Build();
  fSMNumber->SetLimits(-1, 17);
  fSMNumber->SetToolTip("Supermodule id [-1 for all]");
  fVerticalFrame1974->AddFrame(fSMNumber, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterX | kLHintsExpandY,2,2,2,2));

  fStackNumber = new TEveGValuator(fVerticalFrame1974, "Stack:", 0, 0);
  fStackNumber->SetShowSlider(kFALSE);
  fStackNumber->SetLabelWidth(labelW);
  fStackNumber->SetNELength(6);
  fStackNumber->Build();
  fStackNumber->SetLimits(-1, 4);
  fStackNumber->SetToolTip("Stack id [-1 for all in this SM]");
  fVerticalFrame1974->AddFrame(fStackNumber, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterX | kLHintsExpandY,2,2,2,2));

  fPlaneNumber = new TEveGValuator(fVerticalFrame1974, "Plane:", 0, 0);
  fPlaneNumber->SetShowSlider(kFALSE);
  fPlaneNumber->SetLabelWidth(labelW);
  fPlaneNumber->SetNELength(6);
  fPlaneNumber->Build();
  fPlaneNumber->SetLimits(-1, 5);
  fPlaneNumber->SetToolTip("Plane id [-1 for all in this stack]");

  fVerticalFrame1974->AddFrame(fPlaneNumber, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterX | kLHintsExpandY,2,2,2,2));

  fGroupFrame1974->AddFrame(fVerticalFrame1974, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandY | kLHintsCenterX,2,2,2,2));

  TGTextButton *fTextButton2037 = new TGTextButton(fGroupFrame1974,"Select");
  fTextButton2037->SetTextJustify(36);
  fGroupFrame1974->AddFrame(fTextButton2037, new TGLayoutHints(kLHintsExpandY | kLHintsCenterX,2,2,2,2));
  fTextButton2037->SetToolTipText("Apply selection", 400);
  fTextButton2037->Connect("Clicked()",
                           "AliEveTRDLoaderEditor", this, "AddChambers()");

  fGroupFrame1974->SetLayoutManager(new TGHorizontalLayout(fGroupFrame1974));
  AddFrame(fGroupFrame1974, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY | kLHintsExpandX,5,5,5,5));


  // Event steering frame
  f = new TGHorizontalFrame(this);
  TGTextButton *fGoTo = new TGTextButton(f, "GoTo");
  fGoTo->SetTextJustify(36);
  fGoTo->Resize(164,22);
  f->AddFrame(fGoTo, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY | kLHintsExpandX,2,2,2,2));
  fGoTo->SetToolTipText("Skip to event", 400);
  fGoTo->Connect("Clicked()", "AliEveTRDLoaderEditor", this, "GoTo()");

  fEvent = new TEveGValuator(f, "Event:", 110, 0);
  fEvent->SetShowSlider(kFALSE);
  fEvent->SetLabelWidth(labelW);
  fEvent->SetNELength(6);
  fEvent->Build();
  fEvent->SetLimits(-1, 1000);
  fEvent->SetToolTip("Set event number to be monitored");
  fEvent->Connect("ValueSet(Double_t)",
                  "AliEveTRDLoaderEditor", this, "SetEvent(Double_t)");
  f->AddFrame(fEvent, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 2,2,2,2));

  TGTextButton *fNext = new TGTextButton(f, "Next");
  fNext->SetTextJustify(36);
  fNext->Resize(164,22);
  f->AddFrame(fNext, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY | kLHintsExpandX,2,2,2,2));
  fNext->SetToolTipText("Next event", 400);
  fNext->Connect("Clicked()", "AliEveTRDLoaderEditor", this, "Next()");

  AddFrame(f,new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY | kLHintsExpandX,5,5,5,5));

}

//______________________________________________________________________________
void AliEveTRDLoaderEditor::SetModel(TObject* obj)
{
  // Set model object.

  fM = dynamic_cast<AliEveTRDLoader*>(obj);

  fFile->SetEnabled(!fM->IsDataLinked());
  fFile->SetText(gSystem->BaseName(fM->fFilename.Data()));
  fBrowse->SetEnabled(!fM->IsDataLinked());

  fEvent->SetEnabled(fM->IsDataLinked());
  fEvent->GetEntry()->SetIntNumber(fM->fEvent);

  fSMNumber->SetEnabled(fM->IsDataLinked());
  fSMNumber->GetEntry()->SetIntNumber(fM->fSM);


  fStackNumber->SetEnabled(fM->IsDataLinked());
  fStackNumber->GetEntry()->SetIntNumber(fM->fStack);


  fPlaneNumber->SetEnabled(fM->IsDataLinked());
  fPlaneNumber->GetEntry()->SetIntNumber(fM->fLy);
}

//______________________________________________________________________________
void AliEveTRDLoaderEditor::AddChambers()
{
  // Slot to add chambers.

  fM->fSM    = (int)fSMNumber->GetEntry()->GetNumber();
  fM->fStack = (int)fStackNumber->GetEntry()->GetNumber();
  fM->fLy    = (int)fPlaneNumber->GetEntry()->GetNumber();
  fM->AddChambers(fM->fSM, fM->fStack, fM->fLy);
}

//______________________________________________________________________________
void AliEveTRDLoaderEditor::FileOpen()
{
  // Slot for opening of file.

  TGFileInfo fi;
  fi.fIniDir    = StrDup(gSystem->DirName (fM->fFilename.Data()));
  fi.fFilename  = StrDup(gSystem->BaseName(fM->fFilename.Data()));
  //  fi.fFileTypes = tpcfiletypes;

  new TGFileDialog(fClient->GetRoot(), gEve->GetMainWindow(), kFDOpen, &fi);
  if (!fi.fFilename) return;

  if(fM->Open(gSystem->BaseName(fi.fFilename), gSystem->DirName (fi.fFilename))){ 
    fFile->SetToolTipText(gSystem->DirName (fi.fFilename));
    fFile->SetText       (gSystem->BaseName(fi.fFilename));
  } else fFile->Clear();

  this->SetModel(fM);
}

void AliEveTRDLoaderEditor::GoTo()
{
  // Slot for loading of event.

  fM->GoToEvent(fM->fEvent);
}

void AliEveTRDLoaderEditor::Next()
{
  // Slot for loading of event.

  fM->NextEvent();
}

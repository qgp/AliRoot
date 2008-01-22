// $Id$
// Main authors: Matevz Tadel & Alja Mrak-Tadel: 2006, 2007

/**************************************************************************
 * Copyright(c) 1998-2008, ALICE Experiment at CERN, all rights reserved. *
 * See http://aliceinfo.cern.ch/Offline/AliRoot/License.html for          *
 * full copyright notice.                                                 *
 **************************************************************************/

#include "AliEveTRDLoader.h"
#include "AliEveTRDModuleImp.h"

#include <TEveManager.h>
#include <TEveGValuators.h>

#include "TSystem.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TObjArray.h"

#include <TGLabel.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <TGNumberEntry.h>
#include <TGFileDialog.h>
#include <TGListTree.h>
#include <TGToolTip.h>

#include "AliLog.h"
#include "AliCDBManager.h"

#include "AliTRDv1.h"
#include "AliTRDhit.h"
#include "AliTRDcluster.h"
#include "AliTRDmcmTracklet.h"
#include "AliTRDdigitsManager.h"
#include "AliTRDgeometry.h"

#include <algorithm>

class AliTRDdataArrayI;

ClassImp(AliEveTRDLoader)
ClassImp(AliEveTRDLoaderEditor)

///////////////////////////////////////////////////////////
/////////////     AliEveTRDLoader     /////////////////////
///////////////////////////////////////////////////////////


//______________________________________________________________________________
AliEveTRDLoader::AliEveTRDLoader(const Text_t* n, const Text_t* t) :
  TEveElementList(n, t),
  fLoadHits     (kFALSE), fLoadDigits (kFALSE),
  fLoadClusters (kFALSE), fLoadTracks (kFALSE),
  fSM           (-1),     fStack      (-1),     fLy(-1),
  fFilename     (""),     fDir        ("."),
  fEvent        (-1),
  fTRD          (0x0),
  fGeo          (new AliTRDgeometry())
{
  AliCDBManager *fCDBManager=AliCDBManager::Instance();
  fCDBManager->SetDefaultStorage("local://$ALICE_ROOT");
  fCDBManager->SetRun(0);
}

//______________________________________________________________________________
AliEveTRDLoader::~AliEveTRDLoader()
{
  // if(fChambers) {fChambers->clear(); delete fChambers;}
}

//______________________________________________________________________________
template<class T>
class ID
{
public:
  ID( int value ) : id(value) {}
  bool operator()(const T &t) const {
    return ((dynamic_cast<AliEveTRDModule*>(t))->GetID() == id);
  }
private:
  const int id;
};

void AliEveTRDLoader::AddChambers(int sm, int stk, int ly)
{
  Int_t ism_start = (sm == -1) ?  0 : sm;
  Int_t ism_stop  = (sm == -1) ? 18 : sm+1;
  Int_t istk_start= (stk == -1)?  0 : stk;
  Int_t istk_stop = (stk == -1)?  5 : stk+1;
  Int_t ily_start = (ly == -1) ?  0 : ly;
  Int_t ily_stop  = (ly == -1) ?  6 : ly+1;

  List_i ichmb;
  ichmb = fChildren.begin();
  while(ichmb != fChildren.end()){
    (*ichmb)->SetRnrSelf(kFALSE);
    ichmb++;
  }

  AliEveTRDNode *SM=0x0, *STK=0x0;
  AliEveTRDChamber *CHMB = 0x0;
  int det;
  for(int ism=ism_start; ism<ism_stop; ism++){
    ichmb = find_if(fChildren.begin(), fChildren.end(), ID<TEveElement*>(ism));
    if(ichmb != fChildren.end()){
      SM = (AliEveTRDNode*)(*ichmb);
      SM->SetRnrSelf(kTRUE);
    }else{
      gEve->AddElement(SM = new AliEveTRDNode("SM", ism), this);
      SM->FindListTreeItem(gEve->GetListTree())->SetTipText(Form("Supermodule %2d", ism));
    }
    for(int istk=istk_start; istk<istk_stop; istk++){
      ichmb = find_if(SM->begin(), SM->end(), ID<TEveElement*>(istk));
      if(ichmb != SM->end()){
        STK = (AliEveTRDNode*)(*ichmb);
        STK->SetRnrSelf(kTRUE);
      }else{
        gEve->AddElement(STK = new AliEveTRDNode("Stack", istk), SM);
        STK->FindListTreeItem(gEve->GetListTree())->SetTipText(Form("SM %2d Stack %1d", ism, istk));
      }
      for(int ily=ily_start; ily<ily_stop; ily++){
        det = fGeo->GetDetector(ily, istk, ism);
        ichmb = find_if(STK->begin(), STK->end(), ID<TEveElement*>(det));
        if(ichmb != STK->end()) (*ichmb)->SetRnrSelf(kTRUE);
        else{
          gEve->AddElement(CHMB = new AliEveTRDChamber(det), STK);
          CHMB->SetGeometry(fGeo);
          CHMB->FindListTreeItem(gEve->GetListTree())->SetTipText(Form("SM %2d Stack %1d Layer %1d", ism, istk, ily));
        }
      }
    }
  }
  gEve->Redraw3D();
}

//______________________________________________________________________________
AliEveTRDChamber* AliEveTRDLoader::GetChamber(int d)
{
  List_i ism, istack, ichmb;

  ism = find_if(fChildren.begin(), fChildren.end(), ID<TEveElement*>(fGeo->GetSector(d)));
  if(ism == fChildren.end()) return 0x0;
  istack = find_if(((AliEveTRDNode*)(*ism))->begin(), ((AliEveTRDNode*)(*ism))->end(), ID<TEveElement*>(fGeo->GetChamber(d)));
  if(istack == ((AliEveTRDNode*)(*ism))->end()) return 0x0;
  ichmb = find_if(((AliEveTRDNode*)(*istack))->begin(), ((AliEveTRDNode*)(*istack))->end(), ID<TEveElement*>(d));
  if(ichmb == ((AliEveTRDNode*)(*istack))->end()) return 0x0;
  return dynamic_cast<AliEveTRDChamber*>(*ichmb);
}

//______________________________________________________________________________
Bool_t AliEveTRDLoader::GoToEvent(int ev)
{
  if(!fChildren.size()){
    AliWarning("Please select first the chamber that you want to monitor from \"Chamber(s) selector\".");
    return kFALSE;
  }

  fEvent = ev;

  Unload();

  TTree *t = 0x0;
  TFile *f = new TFile(Form("%s/%s", fDir.Data(), fFilename.Data()));
  if(! f->cd(Form("AliEveEventManager%d", ev))){
    AliError(Form("Couldn't find event %d in file \"%s/%s\".", ev, fDir.Data(), fFilename.Data()));
    f->Close(); delete f;
    return kFALSE;
  }

  if(fLoadDigits){
    t = (TTree*)gDirectory->Get("TreeD");
    if(!t) return kFALSE;
    if(!LoadDigits(t)) return kFALSE;
  } else if(fLoadClusters){
    t = (TTree*)gDirectory->Get("TreeR");
    if(!t) return kFALSE;
    if(!LoadClusters(t)) return kFALSE;
  } else if(fLoadTracks){
    t = (TTree*)gDirectory->Get("TreeT");
    if(!t) return kFALSE;
    if(!LoadTracklets(t)) return kFALSE;
  } else AliWarning("Please select first the type of data that you want to monitor and then hit the \"Load\" button.");

  f->Close(); delete f;

  gEve->Redraw3D();

  return kTRUE;
}


//______________________________________________________________________________
Bool_t AliEveTRDLoader::LoadClusters(TTree *tC)
{
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
  fFilename = filename;
  fDir = dir;
  Int_t count = 0;
  count += fLoadDigits ? 1 : 0;
  count += fLoadClusters ? 1 : 0;
  count += fLoadTracks ? 1 : 0;

  TObjArray *so = fFilename.Tokenize(".");

  if(((TObjString*)(*so)[0])->GetString().CompareTo("TRD") != 0){
    if(!count){
      AliWarning("Filename didn't fulfill naming conventions. No TRD data will be loaded.");
      return kFALSE;
    } else {
      Warning("Open()", "Filename didn't fulfill naming conventions.");
      return kTRUE;
    }
  }
  if(((TObjString*)(*so)[1])->GetString().CompareTo("Digits") == 0){
    if(!fLoadDigits) AliWarning("Data type set to DIGITS according to file name. Previous settings with SetDataType() will be discarded.");
    fLoadDigits = kTRUE;
  } else if(((TObjString*)(*so)[1])->GetString().CompareTo("RecPoints") == 0){
    if(!fLoadClusters) AliWarning("Data type set to CLUSTERS according to file name. Previous settings with SetDataType() will be discarded.");
    fLoadClusters = kTRUE;
  } else if(((TObjString*)(*so)[1])->GetString().CompareTo("Tracks") == 0){
    if(!fLoadTracks) AliWarning("Data type set to TRACKLETS according to file name. Previous settings with SetDataType() will be discarded.");
    fLoadTracks = kTRUE;
  } else if(count){
    AliWarning("Filename didn't fulfill naming conventions.");
    return kTRUE;
  } else {
    AliError("Filename didn't fulfill naming conventions. No data will be loaded.");
    return kFALSE;
  }

  return kTRUE;
}



//______________________________________________________________________________
void AliEveTRDLoader::Paint(Option_t *option)
{
  List_i ichmb = fChildren.begin();
  while(ichmb != fChildren.end()){
    (dynamic_cast<AliEveTRDModule*>(*ichmb))->Paint(option);
    ichmb++;
  }
}

//______________________________________________________________________________
void AliEveTRDLoader::SetDataType(TRDDataTypes type)
{
  fLoadHits     = kFALSE;
  fLoadDigits   = kFALSE;
  fLoadClusters = kFALSE;
  fLoadTracks   = kFALSE;
  switch(type){
    case kHits:     fLoadHits = kTRUE; break;
    case kDigits:   fLoadDigits = kTRUE; break;
    case kClusters: fLoadClusters = kTRUE; break;
    case kTracks:   fLoadTracks = kTRUE; break;
    case kRawRoot: break;
    case kRawData: break;
  }
}

//______________________________________________________________________________
void AliEveTRDLoader::Unload()
{
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
  fM(0), fFile(0), fEvent(0),
  fSMNumber(0), fStackNumber(0), fPlaneNumber(0)
{
  MakeTitle("AliEveTRDLoader");

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

  TGTextButton* openFile = new TGTextButton(f, "Browse");
  f->AddFrame(openFile);
  openFile->Connect("Clicked()", "AliEveTRDLoaderEditor", this, "FileOpen()");
  AddFrame(f);


  fEvent = new TEveGValuator(this, "AliEveEventManager:", 110, 0);
  fEvent->SetShowSlider(kFALSE);
  fEvent->SetLabelWidth(labelW);
  fEvent->SetNELength(6);
  fEvent->Build();
  fEvent->SetLimits(-1, 1000);
  fEvent->SetToolTip("Set event number to be monitored");
  fEvent->Connect("ValueSet(Double_t)",
                  "AliEveTRDLoaderEditor", this, "SetEvent(Double_t)");
  AddFrame(fEvent);


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
  AddFrame(fGroupFrame1974, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY | kLHintsExpandX,2,2,2,2));


  TGTextButton *fTextButton2004 = new TGTextButton(this,"Load");
  fTextButton2004->SetTextJustify(36);
  fTextButton2004->Resize(164,22);
  AddFrame(fTextButton2004, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY | kLHintsExpandX,2,2,2,2));
  fTextButton2004->SetToolTipText("Load data according to selection", 400);
  fTextButton2004->Connect("Clicked()", "AliEveTRDLoaderEditor", this, "Load()");
}

//______________________________________________________________________________
AliEveTRDLoaderEditor::~AliEveTRDLoaderEditor()
{}

//______________________________________________________________________________
void AliEveTRDLoaderEditor::SetModel(TObject* obj)
{

  fM = dynamic_cast<AliEveTRDLoader*>(obj);

  fFile->SetText(gSystem->BaseName(fM->fFilename.Data()));

  Bool_t kFile = kTRUE;
  if(fM->fFilename.CompareTo("") == 0) kFile = kFALSE;

  fEvent->SetEnabled(kFile);
  fEvent->GetEntry()->SetIntNumber(fM->fEvent);

  fSMNumber->SetEnabled(kFile);
  fSMNumber->GetEntry()->SetIntNumber(fM->fSM);


  fStackNumber->SetEnabled(kFile);
  fStackNumber->GetEntry()->SetIntNumber(fM->fStack);


  fPlaneNumber->SetEnabled(kFile);
  fPlaneNumber->GetEntry()->SetIntNumber(fM->fLy);
}

//______________________________________________________________________________
void AliEveTRDLoaderEditor::AddChambers()
{
  fM->fSM    = (int)fSMNumber->GetEntry()->GetNumber();
  fM->fStack = (int)fStackNumber->GetEntry()->GetNumber();
  fM->fLy    = (int)fPlaneNumber->GetEntry()->GetNumber();
  fM->AddChambers(fM->fSM, fM->fStack, fM->fLy);
}

//______________________________________________________________________________
void AliEveTRDLoaderEditor::FileOpen()
{
  TGFileInfo fi;
  fi.fIniDir    = StrDup(gSystem->DirName (fM->fFilename.Data()));
  fi.fFilename  = StrDup(gSystem->BaseName(fM->fFilename.Data()));
  //  fi.fFileTypes = tpcfiletypes;

  new TGFileDialog(fClient->GetRoot(), gEve->GetMainWindow(), kFDOpen, &fi);
  if (!fi.fFilename) return;

  fFile->SetToolTipText(gSystem->DirName (fi.fFilename));
  fFile->SetText       (gSystem->BaseName(fi.fFilename));

  fM->Open(gSystem->BaseName(fi.fFilename), gSystem->DirName (fi.fFilename));

  this->SetModel(fM);
}

void AliEveTRDLoaderEditor::Load()
{
  fM->GoToEvent(fM->fEvent);
}

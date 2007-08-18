
#include "VSDSelector.h"
#include "VSDEvent.h"
#include "RGTopFrame.h"

#include <Reve/Track.h>
#include <Reve/PointSet.h>

#include <Reve/PODs.h>
#include <Reve/TTreeTools.h>

#include <TEventList.h>
#include <TGLabel.h>
#include <TGXYLayout.h>
#include <TGClient.h>

using namespace Reve;

using Reve::Exc_t;

VSDSelector::VSDSelector(TGCompositeFrame *tFrame) :
  VSD(),

  mParticleSelection(0),
  fRecursiveSelect(0),

  mHitSelection(0),   
  mClusterSelection(0), 
  mRecSelection(0)
{
  //create gui
  TGGroupFrame *gframe = new TGGroupFrame(tFrame, "Options", kVerticalFrame);
  TGLayoutHints* lh0 = new TGLayoutHints(kLHintsTop | kLHintsLeft |  kLHintsExpandX | kLHintsExpandY  , 5, 5, 5, 5);
  gframe->SetTitlePos(TGGroupFrame::kRight); // right aligned
  tFrame->AddFrame(gframe, lh0);
  
  TGXYLayout* xyl = new TGXYLayout(gframe);
  gframe->SetLayoutManager(xyl);
  xyl->Layout();

  TGXYLayoutHints* lh;

  UInt_t wH     = 2;
  UInt_t labelw = 15;
  UInt_t entryw = 39;
  UInt_t butw   = 10;

  Float_t x = 2, y = 2;

  {
    // particles
    TGLabel* label = new TGLabel(gframe, "ParticleSelection");
    label->Resize(labelw, wH);
    label->SetTextJustify(kTextLeft | kTextCenterY);
    lh = new TGXYLayoutHints(x, y, labelw, wH,0);  lh->SetPadLeft(2); lh->SetPadRight(2);
    gframe->AddFrame(label,lh);
    x += labelw ;
  
    mParticleSelection = new TGTextEntry(gframe, "fMother[0] == -1 && Pt() > 1");
    mParticleSelection->Resize(entryw, wH);
    lh = new TGXYLayoutHints(x, y, entryw, wH,0);  lh->SetPadLeft(2); lh->SetPadRight(2);
    gframe->AddFrame(mParticleSelection,lh);
    x += entryw+1;
  
    TGTextButton* but = new TGTextButton(gframe, "Select");
    but->Resize(butw, wH);
    but->SetTextJustify(kTextCenterX | kTextCenterY);
    lh = new TGXYLayoutHints(x, y, butw, wH,0);  lh->SetPadLeft(4); lh->SetPadRight(2);
    gframe->AddFrame(but,lh);
    but->Connect("Pressed()", "Reve::VSDSelector", this, "SelectParticles()");
   
    UInt_t rbw = 11;
    x = x + butw + 0.5;
    fRecursiveSelect = new TGCheckButton(gframe, "Recursive");
    fRecursiveSelect->Resize(rbw, wH);
    lh = new TGXYLayoutHints(x, y, rbw, wH,0);  lh->SetPadLeft(4); lh->SetPadRight(2);
    gframe->AddFrame(fRecursiveSelect,lh);
  }
  x = 2.;
  y+= wH;  y+= 1.;


  {    // hits
    TGLabel* label = new TGLabel(gframe, "HitSelection");
    label->Resize(labelw, wH);
    label->SetTextJustify(kTextLeft | kTextCenterY);
    lh = new TGXYLayoutHints(x, y, labelw, wH,0);  lh->SetPadLeft(2); lh->SetPadRight(2);
    gframe->AddFrame(label,lh);
    x += labelw;
  
    mHitSelection  = new TGTextEntry(gframe, "det_id == 0");
    mHitSelection->Resize(entryw, wH);
    lh = new TGXYLayoutHints(x, y, entryw, wH,0);  lh->SetPadLeft(2); lh->SetPadRight(2);
    gframe->AddFrame(mHitSelection,lh);
    x += entryw +1;
  
    TGTextButton* but = new TGTextButton(gframe, "Select");
    but->Resize(butw, wH);
    but->SetTextJustify(kTextCenterX | kTextCenterY);
    lh = new TGXYLayoutHints(x, y, butw, wH,0);  lh->SetPadLeft(4); lh->SetPadRight(2);
    gframe->AddFrame(but,lh);
    but->Connect("Pressed()", "Reve::VSDSelector", this, "SelectHits()");

  }

  x = 2.;
  y+= wH;  y+= 1.;

  {    // particle selection
    TGLabel* label = new TGLabel(gframe, "ClusterSelection");
    label->Resize(labelw, wH);
    label->SetTextJustify(kTextLeft | kTextCenterY);
    lh = new TGXYLayoutHints(x, y, labelw, wH,0);  lh->SetPadLeft(2); lh->SetPadRight(2);
    gframe->AddFrame(label,lh);
    x += labelw;
  
    mClusterSelection = new TGTextEntry(gframe, "C.V.R() > 70");
    mClusterSelection->Resize(entryw, wH);
    lh = new TGXYLayoutHints(x, y, entryw, wH,0);  lh->SetPadLeft(2); lh->SetPadRight(2);
    gframe->AddFrame(mClusterSelection,lh);
    x += entryw +1;
  
    TGTextButton* but = new TGTextButton(gframe, "Select");
    but->Resize(butw, wH);
    but->SetTextJustify(kTextCenterX | kTextCenterY);
    lh = new TGXYLayoutHints(x, y, butw, wH,0);  lh->SetPadLeft(4); lh->SetPadRight(2);
    gframe->AddFrame(but,lh);
    but->Connect("Pressed()", "Reve::VSDSelector", this, "SelectClusters()");
  }

  x = 2.;
  y+= wH;  y+= 1.;

  {    // reconstructed tracks selection
    TGLabel* label = new TGLabel(gframe, "RecSelection");
    label->Resize(labelw, wH);
    label->SetTextJustify(kTextLeft | kTextCenterY);
    lh = new TGXYLayoutHints(x, y, labelw, wH,0);  lh->SetPadLeft(2); lh->SetPadRight(2);
    gframe->AddFrame(label,lh);
    x += labelw;
  
    mRecSelection = new TGTextEntry(gframe, "Pt() > 1");
    mRecSelection->Resize(entryw, wH);
    lh = new TGXYLayoutHints(x, y, entryw, wH,0);  lh->SetPadLeft(2); lh->SetPadRight(2);
    gframe->AddFrame(mRecSelection,lh);
    x += entryw +1;
  
    TGTextButton* but = new TGTextButton(gframe, "Select");
    but->Resize(butw, wH);
    but->SetTextJustify(kTextCenterX | kTextCenterY);
    lh = new TGXYLayoutHints(x, y, butw, wH,0);  lh->SetPadLeft(4); lh->SetPadRight(2);
    gframe->AddFrame(but,lh);
    but->Connect("Pressed()", "Reve::VSDSelector", this, "SelectRecTracks()");
  }

  gframe->Resize(60, 30); // resize to default size 
  gframe->MapSubwindows();
  gframe->MapWindow();
  tFrame->MapSubwindows();

}

/**************************************************************************/

void VSDSelector::LoadVSD(const Text_t* vsd_file_name,
			  const Text_t* dir_name)
{
  VSD::LoadVSD(vsd_file_name, dir_name);
  // !!!! Should create VSDEvent ... but it is not done yet.
  EventBase* ev = new EventBase(vsd_file_name, dir_name);
  gReve->AddEvent(ev); 
}

/**************************************************************************/
//  selection methods
/**************************************************************************/

void VSDSelector::SelectParticles(const Text_t* selection)
{
  static const Exc_t eH("VSDSelector::SelectParticles ");

  if(mTreeK == 0) 
    throw (eH + "kinematics not available.");

  if(selection == 0)
    selection = mParticleSelection->GetText();

  TTreeQuery evl;
  Int_t n = evl.Select(mTreeK, selection);
  // printf("%d entries in selection '%s'.\n", n,  selection);

  if(n == 0)
    throw (eH + "no entries found for selection in kinematics.");

  TrackList* cont   = new TrackList();
  cont->SetMainColor(4);

  gReve->AddRenderElement(cont);

  if(n > 0) {
    for(Int_t i=0; i<n; i++) {
      Int_t label = evl.GetEntry(i);
      mTreeK->GetEntry(label);
      Track* track = new Track(mpK, cont->GetRnrStyle());
      track->SetName(Form("%s daughters:%d", mK.GetName(), mK.GetNDaughters()));
      gReve->AddRenderElement(track, cont);

      // printf("select daugters %s selection %s\n",mpK->GetName(),Form("fMother[0] == %d", track->GetLabel()));
      if(fRecursiveSelect->IsOn()) {
        if(mK.GetNDaughters())
	  ImportDaughtersRec(track, cont, mK.GetFirstDaughter(), mK.GetLastDaughter());
        // add decay point to path marks
        if(mK.decayed) {
	  Reve::PathMark* pm = new Reve::PathMark(Reve::PathMark::Decay);
          pm->V.x = mK.V_decay.x;
          pm->V.y = mK.V_decay.y;
          pm->V.z = mK.V_decay.z;
	  track->AddPathMark(pm);
	}
      }
      track->MakeTrack();
    }
  }
}

void VSDSelector::ImportDaughtersRec(RenderElement* parent, TrackList* cont,
				     Int_t first, Int_t last)
{
  Track* mother = dynamic_cast<Track*>(parent);

  for(Int_t i=first; i<=last; i++) {
    mTreeK->GetEntry(i); 
    Track* track = new Track(mpK, cont->GetRnrStyle());
    track->SetName(Form("%s daughters:%d", mK.GetName(), mK.GetNDaughters()));
    gReve->AddRenderElement(track, parent);
    cont->AddElement(track); // ?? is this ok ??
    if(mK.GetNDaughters())
      ImportDaughtersRec(track, cont, mK.GetFirstDaughter(), mK.GetLastDaughter());

    // add daughter mark to mother
    Reve::PathMark* dam = new Reve::PathMark(Reve::PathMark::Daughter);
    dam->V.x = mK.Vx();
    dam->V.y = mK.Vy();
    dam->V.z = mK.Vz();
    mother->AddPathMark(dam);

    if(mK.decayed) {
      Reve::PathMark* decm = new Reve::PathMark(Reve::PathMark::Decay);
      decm->V.x = mK.V_decay.x;
      decm->V.y = mK.V_decay.y;
      decm->V.z = mK.V_decay.z;
      track->AddPathMark(decm);

    }
    track->MakeTrack();
  }
}

/**************************************************************************/
/**************************************************************************/

void VSDSelector::SelectHits()
{
  static const Exc_t eH("VSDSelector::SelectHits ");

  if(mTreeH == 0) 
    throw (eH + "hits not available.");

  const Text_t* selection;
  if(mHitSelection)
    selection  = mHitSelection->GetText();
  else 
    selection  ="1";

  TTreeQuery evl;
  Int_t n = evl.Select(mTreeH, selection);
  // printf("ImportHitsWithSelection %d entries for selection %s\n", n, selection);
  
  if(n==0)
    throw(eH + "no hits matching selection.");

  PointSet* container = new PointSet
    (Form("Hits %s", selection), n);
  for(Int_t i=0; i<n; i++) {
    const Int_t entry = evl.GetEntry(i);
    mTreeH->GetEntry(entry);
    container->SetPoint(i, mH.V.x, mH.V.y, mH.V.z);
  }

  container->SetTitle(Form("N=%d", container->GetN()));
  container->SetMarkerColor(2);
  container->SetMarkerStyle(20);
  container->SetMarkerSize(0.5);
  gReve->AddRenderElement(container);
  gReve->Redraw3D();
}

/**************************************************************************/

void VSDSelector::SelectClusters()
{
  static const Exc_t eH("VSDSelector::SelectClusters ");

  if(mTreeC == 0) 
    throw (eH + "clusters not available.");

  const Text_t* selection;
  if (mClusterSelection)
    selection = mClusterSelection->GetText();
  else 
    selection  ="1";

  TTreeQuery evl;
  Int_t n = evl.Select(mTreeC, selection);
  printf(" cluster Selection %d entries for selection %s\n", n, selection);
  
  if(n==0)
    throw(eH + "no clusters matching selection.");

  PointSet* container = new PointSet
    (Form("Clusters %s", selection), n);
  for(Int_t i=0; i<n; i++) {
    const Int_t entry = evl.GetEntry(i);
    mTreeC->GetEntry(entry);
    container->SetPoint(i, mC.V.x, mC.V.y, mC.V.z);
  }

  container->SetTitle(Form("N=%d", container->GetN()));
  container->SetMarkerColor(9);
  container->SetMarkerStyle(20);
  container->SetMarkerSize(0.5);
  gReve->AddRenderElement(container);
  gReve->Redraw3D();
}

/**************************************************************************/

void VSDSelector::SelectRecTracks()
{
  static const Exc_t eH("VSDSelector::SelectRecTracks ");

  if(mTreeR == 0) 
    throw (eH + "reconstructed tracks not available.");

  const Text_t* selection;
  if(mRecSelection)
    selection = mRecSelection->GetText();
  else 
    selection = "Pt() > 1";
  
  TTreeQuery evl;
  Int_t n = evl.Select(mTreeR, selection);
  // printf("%d entries in selection %s \n", n,  selection);

  if (n == 0)
    throw (eH + "No entries found in ESD data.");

  if(n > 0) {
    TrackList* cont = new TrackList(Form("RecTracks %s [%d]",selection, n));
    cont->SetMainColor(6);

    gReve->AddRenderElement(cont);

    for (Int_t i=0; i<n; i++) {
      Int_t label = evl.GetEntry(i);
      mTreeR->GetEntry(label);
      Track* track = new Track(mpR, cont->GetRnrStyle());
      track->MakeTrack();

      gReve->AddRenderElement(track, cont);
    }
    gReve->Redraw3D();
  }
}

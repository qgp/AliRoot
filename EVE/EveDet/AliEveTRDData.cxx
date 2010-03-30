// $Id$
// Main authors: Matevz Tadel & Alja Mrak-Tadel: 2006, 2007

/**************************************************************************
 * Copyright(c) 1998-2008, ALICE Experiment at CERN, all rights reserved. *
 * See http://aliceinfo.cern.ch/Offline/AliRoot/License.html for          *
 * full copyright notice.                                                 *
 **************************************************************************/

#include "TROOT.h"
#include "TVector.h"
#include "TLinearFitter.h"
#include "TCanvas.h"

#include "TEveTrans.h"
#include "TEveManager.h"

#include "EveBase/AliEveEventManager.h"

#include "AliEveTRDData.h"
#include "AliEveTRDModuleImp.h"
#include "AliEveTRDLoader.h"
#include "AliEveTRDLoaderImp.h"

#include "AliGeomManager.h"
#include "AliESDtrack.h"
#include "AliLog.h"
#include "AliPID.h"
#include "AliTrackPointArray.h"
#include "AliRieman.h"

#include "AliTRDhit.h"
#include "AliTRDcluster.h"
#include "AliTRDseedV1.h"
#include "AliTRDtrackletMCM.h"
#include "AliTRDtrackletWord.h"
#include "AliTRDmcmSim.h"
#include "AliTRDtrackV1.h"
#include "AliTRDtrackerV1.h"
#include "AliTRDpadPlane.h"
#include "AliTRDdigitsManager.h"
#include "AliTRDmcmSim.h"
#include "AliTRDarrayADC.h"
#include "AliTRDSignalIndex.h"
#include "AliTRDgeometry.h"
#include "AliTRDtransform.h"
#include "AliTRDReconstructor.h"
#include "AliTRDrecoParam.h"

ClassImp(AliEveTRDHits)
ClassImp(AliEveTRDDigits)
ClassImp(AliEveTRDClusters)
ClassImp(AliEveTRDTracklet)
ClassImp(AliEveTRDTrack)
ClassImp(AliEveTRDTrackletOnline)
ClassImp(AliEveTRDmcm)

///////////////////////////////////////////////////////////
/////////////   AliEveTRDDigits       /////////////////////
///////////////////////////////////////////////////////////

//______________________________________________________________________________
AliEveTRDDigits::AliEveTRDDigits(AliEveTRDChamber *p) :
  TEveQuadSet("digits", ""), fParent(p), fBoxes(), fData()
{
  // Constructor.
}

//______________________________________________________________________________
AliEveTRDDigits::~AliEveTRDDigits()
{
//  AliInfo(GetTitle());
}

//______________________________________________________________________________
void AliEveTRDDigits::ComputeRepresentation()
{
  // Calculate digits representation according to user settings. The
  // user can set the following parameters:
  // - digits scale (log/lin)
  // - digits threshold
  // - digits apparence (quads/boxes)

  if(!fData.HasData()){
    return;
  }

  TEveQuadSet::Reset(TEveQuadSet::kQT_RectangleYZ, kTRUE, 64);

  Double_t scale, dy, dz;
  Int_t q, color;
  Int_t nrows = fData.GetNrow(),
        ncols = fData.GetNcol(),
        ntbs  = fData.GetNtime(),
        det   = fParent->GetID(),
        ly    = AliTRDgeometry::GetLayer(det),
        stk   = AliTRDgeometry::GetStack(det),
        sec   = AliTRDgeometry::GetSector(det),
        vid   = AliGeomManager::LayerToVolUID(AliGeomManager::kTRD1 + ly, stk + AliTRDgeometry::Nstack() * sec);
  Float_t threshold = fParent->GetDigitsThreshold();
  Short_t sig[7]={0,0,0,10,0,0,0};

  AliTRDtransform transform(det);
  AliTRDgeometry *geo = fParent->fGeo;
  AliTRDpadPlane *pp = geo->GetPadPlane(geo->GetLayer(det), geo->GetStack(det));

  // express position in tracking coordinates
  AliTRDcluster c;
  fData.Expand();
  for (Int_t ir = 0; ir < nrows; ir++) {
    dz = pp->GetRowSize(ir);
    for (Int_t ic = 0; ic < ncols; ic++) {
      dy = pp->GetColSize(ic);
      for (Int_t it = 0; it < ntbs; it++) {
        q = fData.GetData(ir, ic, it);
        if (q < threshold) continue;
        
/*        Double_t x[6] = {0., 0., Double_t(q), 0., 0., 0.}; 
        Int_t  roc[3] = {ir, ic, 0}; 
        Bool_t    out = kTRUE;*/
        
        new (&c) AliTRDcluster(det, ic, ir, it, sig, vid);
        transform.Transform(&c);

        scale = q < 512 ? q/512. : 1.;
        color  = 50+int(scale*50.);
        
        AliDebug(4, Form("y[%f] z[%f] x[%f] w[%f] h[%f]\n", c.GetY(), c.GetZ(), c.GetX(), .9*dy, dz*scale));
        AddQuad(c.GetY(), c.GetZ(), c.GetX(), .9*dy, dz*scale);
        QuadValue(q);
        QuadColor(color);
        QuadId(new TNamed(Form("Charge%d", q), "dummy title"));
      }  // end time loop
    }  // end col loop
  }  // end row loop
  fData.Compress();
  
  // rotate to global coordinates
  //RefitPlex();
  TEveTrans& t = RefMainTrans();
  t.SetRotByAngles((sec+.5)*AliTRDgeometry::GetAlpha(), 0.,0.);
}

//______________________________________________________________________________
void AliEveTRDDigits::SetData(AliTRDdigitsManager *digits)
{
  // Set data source.

  Int_t det(fParent->GetID());
  AliTRDarrayADC *data = digits->GetDigits(det);
  if(!data->GetDim()) return;
  data->Expand();

  AliTRDSignalIndex *indexes = digits->GetIndexes(det);
  if(!indexes->IsAllocated()) digits->BuildIndexes(det);

  if(!fData.HasData()) fData.Allocate(data->GetNrow(), data->GetNcol(), data->GetNtime());
  fData.Expand();

  Int_t row, col, time, adc;
  indexes->ResetCounters();
  while (indexes->NextRCIndex(row, col)){
    indexes->ResetTbinCounter();
    while (indexes->NextTbinIndex(time)){
      if(data->IsPadCorrupted(row, col, time)){
        // we should mark this position
        break;
      }
      adc = data->GetData(row, col, time);
      if(adc <= 1) continue;
      fData.SetData(row, col, time, adc);
      //fIndex->AddIndexTBin(row,col,time);
      //printf("\tr[%d] c[%d] t[%d] ADC[%d]\n", row, col, time, adc);
    } 
  }
  fData.Compress();
}


// //______________________________________________________________________________
// void AliEveTRDDigits::Paint(Option_t *option)
// {
//   // Paint the object.
// 
//   if(fParent->GetDigitsBox()) fBoxes.Paint(option);
//   else TEveQuadSet::Paint(option);
// }

//______________________________________________________________________________
void AliEveTRDDigits::Reset()
{
  // Reset raw and visual data.

  TEveQuadSet::Reset(TEveQuadSet::kQT_RectangleYZ, kTRUE, 64);
  // MT fBoxes.fBoxes.clear();
  fData.Reset();
}

///////////////////////////////////////////////////////////
/////////////   AliEveTRDHits         /////////////////////
///////////////////////////////////////////////////////////

//______________________________________________________________________________
AliEveTRDHits::AliEveTRDHits() : TEvePointSet("hits", 20)
{
  // Constructor.
  SetMarkerSize(.1);
  SetMarkerColor(kGreen);
  SetOwnIds(kTRUE);
}

//______________________________________________________________________________
AliEveTRDHits::~AliEveTRDHits()
{
  //AliInfo(GetTitle());
}

//______________________________________________________________________________
void AliEveTRDHits::PointSelected(Int_t n)
{
  // Handle an individual point selection from GL.

  AliTRDhit *h = 0x0;
  if(!(h = dynamic_cast<AliTRDhit*>(GetPointId(n)))) return;
  printf("Id[%3d] Det[%3d] Reg[%c] TR[%c] Q[%3d] MC[%d] t[%f]\n", 
    n, h->GetDetector(), 
    h->FromAmplification() ? 'A' : 'D', 
    h->FromTRphoton() ? 'y' : 'n', 
    h->GetCharge(), h->GetTrack(), h->GetTime());
}


///////////////////////////////////////////////////////////
/////////////   AliEveTRDClusters         /////////////////////
///////////////////////////////////////////////////////////

//______________________________________________________________________________
AliEveTRDClusters::AliEveTRDClusters():AliEveTRDHits()
{
  // Constructor.
  SetName("clusters");

  SetMarkerSize(.4);
  SetMarkerStyle(24);
  SetMarkerColor(kGray);
  SetOwnIds(kTRUE);
}

//______________________________________________________________________________
void AliEveTRDClusters::PointSelected(Int_t n)
{
  // Handle an individual point selection from GL.

  AliTRDcluster *c = dynamic_cast<AliTRDcluster*>(GetPointId(n));
  printf("\nDetector             : %d\n", c->GetDetector());
  printf("Charge               : %f\n", c->GetQ());
  printf("Sum S                : %4.0f\n", c->GetSumS());
  printf("Time bin             : %d\n", c->GetLocalTimeBin());
  printf("Signals              : ");
  Short_t *cSignals = c->GetSignals();
  for(Int_t ipad=0; ipad<7; ipad++) printf("%d ", cSignals[ipad]); printf("\n");
  printf("Central pad          : %d\n", c->GetPadCol());
  printf("MC track labels      : ");
  for(Int_t itrk=0; itrk<3; itrk++) printf("%d ", c->GetLabel(itrk)); printf("\n");
  // Bool_t	AliCluster::GetGlobalCov(Float_t* cov) const
  // Bool_t	AliCluster::GetGlobalXYZ(Float_t* xyz) const
  // Float_t	AliCluster::GetSigmaY2() const
  // Float_t	AliCluster::GetSigmaYZ() const
  // Float_t	AliCluster::GetSigmaZ2() const
}

//______________________________________________________________________________
void AliEveTRDClusters::Print(Option_t *o) const
{
  AliTRDcluster *c = 0x0;

  for(Int_t n = GetN(); n--;){
    if(!(c = dynamic_cast<AliTRDcluster*>(GetPointId(n)))) continue;
    c->Print(o);
  }
}

//______________________________________________________________________________
void AliEveTRDClusters::Load(Char_t *w, Bool_t stk) const
{
  Int_t typ = -1;
  if(strcmp(w, "hit")==0) typ = 0;
  else if(strcmp(w, "dig")==0) typ = 1;
  else if(strcmp(w, "cls")==0) typ = 2;
  else if(strcmp(w, "all")==0) typ = 3;
  else{
    AliInfo("The following arguments are accepted:");
    AliInfo("   \"hit\" : loading of MC hits");
    AliInfo("   \"dig\" : loading of digits");
    AliInfo("   \"cls\" : loading of reconstructed clusters");
    AliInfo("   \"all\" : loading of MC hits+digits+clusters");
    return;
  }

  AliTRDcluster *c = 0x0;
  Int_t n = 0;
  while((n = GetN() && !(c = dynamic_cast<AliTRDcluster*>(GetPointId(n))))) n++;
  if(!c) return;

  Int_t det = c->GetDetector();
  AliEveTRDLoader *loader = 0x0;
  switch(typ){
  case 0:  
    loader = new AliEveTRDLoader("Hits");
    if(!loader->Open("TRD.Hits.root")){ 
      delete loader;
      return;
    }
    loader->SetDataType(AliEveTRDLoader::kTRDHits);
    break;
  case 1:
    loader = new AliEveTRDLoader("Digits");
    if(!loader->Open("TRD.Digits.root")){ 
      delete loader;
      return;
    }
    loader->SetDataType(AliEveTRDLoader::kTRDDigits);
    break;
  case 2:
    loader = new AliEveTRDLoader("Clusters");
    if(!loader->Open("TRD.RecPoints.root")){ 
      delete loader;
      return;
    }
    loader->SetDataType(AliEveTRDLoader::kTRDClusters);
    break;
  case 3:
    loader = new AliEveTRDLoaderSim("MC");
    if(!loader->Open("galice.root")){ 
      delete loader;
      return;
    }
    loader->SetDataType(AliEveTRDLoader::kTRDHits | AliEveTRDLoader::kTRDDigits | AliEveTRDLoader::kTRDClusters);
    break;
  default: return;
  }

  loader->AddChambers(AliTRDgeometry::GetSector(det),AliTRDgeometry::GetStack(det), stk ? -1 : AliTRDgeometry::GetLayer(det));
  // load first event
  loader->GoToEvent(AliEveEventManager::GetCurrent()->GetEventId());
  
  // register loader with alieve
  gEve->AddElement(loader);
  //loader->SpawnEditor();
  gEve->Redraw3D();
}

///////////////////////////////////////////////////////////
/////////////   AliEveTRDTracklet         /////////////////////
///////////////////////////////////////////////////////////

//______________________________________________________________________________
AliEveTRDTracklet::AliEveTRDTracklet(AliTRDseedV1 *trklt):TEveLine()
  ,fClusters(0x0)
{
  // Constructor.
  SetName("tracklet");
  
  SetUserData(trklt);
  Float_t dx;
  Float_t x0   = trklt->GetX0();
  Float_t y0   = trklt->GetYref(0);
  Float_t z0   = trklt->GetZref(0);
  Float_t dydx = trklt->GetYref(1);
  Float_t dzdx = trklt->GetZref(1);
  Float_t tilt = trklt->GetTilt();
  Float_t g[3];
  AliTRDcluster *c = 0x0;
  for(Int_t ic=0; ic<AliTRDseedV1::kNclusters; ic++){
    if(!(c = trklt->GetClusters(ic))) continue;
    if(!fClusters) AddElement(fClusters = new AliEveTRDClusters());
    dx = x0 - c->GetX();
    //Float_t yt = y0 - dx*dydx;
    Float_t zt = z0 - dx*dzdx;
    // backup yc - for testing purposes
    Float_t yc = c->GetY(); 
    c->SetY(yc-tilt*(c->GetZ()-zt));
    c->GetGlobalXYZ(g); 
    Int_t id = fClusters->SetNextPoint(g[0], g[1], g[2]);    
    //Int_t id = fClusters->SetNextPoint(c->GetX(), c->GetY(), c->GetZ());    
    c->SetY(yc);
    fClusters->SetPointId(id, new AliTRDcluster(*c));
  } 
  if(fClusters){
    fClusters->SetTitle(Form("N[%d]", trklt->GetN2()));
    fClusters->SetMarkerColor(kMagenta);
  }

  SetTitle(Form("Det[%d] Plane[%d] P[%7.3f]", trklt->GetDetector(), trklt->GetPlane(), trklt->GetMomentum()));
  SetLineColor(kRed);
  //SetOwnIds(kTRUE);
  
  // init tracklet line
  Int_t sec = AliTRDgeometry::GetSector(trklt->GetDetector());
  Double_t alpha = AliTRDgeometry::GetAlpha() * (sec<9 ? sec + .5 : sec - 17.5); 

  //trklt->Fit(kTRUE);
  y0   = trklt->GetYfit(0);
  dydx = trklt->GetYfit(1);
  Double_t xg =  x0 * TMath::Cos(alpha) - y0 * TMath::Sin(alpha); 
  Double_t yg = x0 * TMath::Sin(alpha) + y0 * TMath::Cos(alpha);
  SetPoint(0, xg, yg, z0); 
  //SetPoint(0, x0, y0, z0);


  dx = .5*AliTRDgeometry::CamHght()+AliTRDgeometry::CdrHght();
  x0 -= dx; 
  y0 -= dydx*dx,
  z0 -= dzdx*dx; 
  xg = x0 * TMath::Cos(alpha) - y0 * TMath::Sin(alpha); 
  yg = x0 * TMath::Sin(alpha) + y0 * TMath::Cos(alpha);
  SetPoint(1, xg, yg, z0);
  //SetPoint(1, x0, y0, z0);
}


AliEveTRDTracklet::~AliEveTRDTracklet() 
{

}

//______________________________________________________________________________
void AliEveTRDTracklet::Print(Option_t *o) const
{
  AliTRDseedV1 *tracklet = (AliTRDseedV1*)GetUserData();
  if(!tracklet) return;
  tracklet->Print(o);
}

///////////////////////////////////////////////////////////
/////////////   AliEveTRDTrack         /////////////////////
///////////////////////////////////////////////////////////

//______________________________________________________________________________
AliEveTRDTrack::AliEveTRDTrack(AliTRDtrackV1 *trk) 
  :TEveLine()
  ,fTrackState(0)
  ,fESDStatus(0)
  ,fAlpha(0.)
  ,fPoints(0x0)
  ,fRim(0x0)
{
  // Constructor.
  SetUserData(trk);
  SetName("");

  AliTRDtrackerV1::SetNTimeBins(24);

  fRim = new AliRieman(trk->GetNumberOfClusters());
  AliTRDseedV1 *tracklet = 0x0;
  for(Int_t il=0; il<AliTRDgeometry::kNlayer; il++){
    if(!(tracklet = trk->GetTracklet(il))) continue;
    if(!tracklet->IsOK()) continue;
    AddElement(new AliEveTRDTracklet(tracklet));

    AliTRDcluster *c = 0x0;
//     tracklet->ResetClusterIter(kFALSE);
//     while((c = tracklet->PrevCluster())){
    for(Int_t ic=AliTRDseedV1::kNtb; ic--;){
      if(!(c=tracklet->GetClusters(ic))) continue;
      Float_t xc = c->GetX();
      Float_t yc = c->GetY();
      Float_t zc = c->GetZ();
      Float_t zt = tracklet->GetZref(0) - (tracklet->GetX0()-xc)*tracklet->GetZref(1); 
      yc -= tracklet->GetTilt()*(zc-zt);
      fRim->AddPoint(xc, yc, zc, .05, 2.3);
    }
  }
  if(trk->GetNumberOfTracklets()>1) fRim->Update();
  SetStatus(fTrackState);
}

//______________________________________________________________________________
AliEveTRDTrack::~AliEveTRDTrack()
{
  if(fPoints) delete [] fPoints; fPoints = 0x0;
  //delete dynamic_cast<AliTRDtrackV1*>(GetUserData());
}

//______________________________________________________________________________
void AliEveTRDTrack::Print(Option_t *o) const
{
  AliTRDtrackV1 *track = (AliTRDtrackV1*)GetUserData();
  if(!track) return;
  track->Print(o);
}

//______________________________________________________________________________
void AliEveTRDTrack::SetStatus(UChar_t s)
{
  // nothing to be done
  if(fPoints && fTrackState == s) return;

  const Int_t nc = AliTRDtrackV1::kMAXCLUSTERSPERTRACK;
  AliTRDtrackV1 *trk = (AliTRDtrackV1*)GetUserData();

  Bool_t BUILD = kFALSE;
  if(!fPoints){ 
    fPoints = new AliTrackPoint[nc];

    // define the radial span of the track in the TRD
    Double_t xmin = -1., xmax = -1.;
    Int_t det = 0;
    AliTRDseedV1 *trklt = 0x0;
    for(Int_t ily=AliTRDgeometry::kNlayer; ily--;){
      if(!(trklt = trk->GetTracklet(ily))) continue;
      if(xmin<0.) xmin = trklt->GetX0() - AliTRDgeometry::CamHght() - AliTRDgeometry::CdrHght();
      if(xmax<0.) xmax = trklt->GetX0();
      det = trklt->GetDetector();
    }
    Int_t sec = det/AliTRDgeometry::kNdets;
    fAlpha = AliTRDgeometry::GetAlpha() * (sec<9 ? sec + .5 : sec - 17.5); //trk->GetAlpha()

    Double_t dx =(xmax - xmin)/nc;
    for(Int_t ip=0; ip<nc; ip++){
      fPoints[ip].SetXYZ(xmin, 0., 0.);
      xmin+=dx;
    }
    BUILD = kTRUE;
  }

  // select track model
  if(BUILD || ((s&12) != (fTrackState&12))){
    if(TESTBIT(s, kTrackCosmics)){
      //printf("Straight track\n");
      AliTRDtrackerV1::FitLine(trk, 0x0, kFALSE, nc, fPoints);
    } else {
      if(TESTBIT(s, kTrackModel)){
        //printf("Kalman track\n");
        if(trk->GetNumberOfTracklets() >=4) AliTRDtrackerV1::FitKalman(trk, 0x0, kFALSE, nc, fPoints);
      } else { 
        //printf("Rieman track\n");
        // if(trk->GetNumberOfTracklets() >=4) AliTRDtrackerV1::FitRiemanTilt(trk, 0x0, kTRUE, nc, fPoints);

/*        Float_t x = 0.;
        for(Int_t ip = nc; ip--;){
          x = fPoints[ip].GetX();
          fPoints[ip].SetXYZ(x, fRim->GetYat(x), fRim->GetZat(x));
        }*/
      }
    }
  
    Float_t global[3];
    for(Int_t ip=0; ip<0/*nc*/; ip++){
      fPoints[ip].Rotate(-fAlpha).GetXYZ(global);
      SetPoint(ip, global[0], global[1], global[2]);
    }
    SetSmooth(kTRUE);
  }

  // set color
  if(BUILD || ((s&3) != (fTrackState&3))){
    if(TESTBIT(s, kSource)){
      //printf("Source color\n");
      if(fESDStatus&AliESDtrack::kTRDin){
        SetMarkerColor(kGreen);
        SetLineColor(kGreen);
      } else {
        SetMarkerColor(kMagenta);
        SetLineColor(kMagenta);
      }
    } else {
      if(TESTBIT(s, kPID) == AliTRDpidUtil::kLQ){
        //printf("PID color kLQPID\n");
        //trk->GetReconstructor()->SetOption("!nn");
      } else {
        //printf("PID color kNNPID\n");
        //trk->GetReconstructor()->SetOption("nn");
      }
      trk->CookPID();
  
      Int_t species = 0; Float_t pid = 0.;
      for(Int_t is=0; is<AliPID::kSPECIES; is++) 
        if(trk->GetPID(is) > pid){
          pid = trk->GetPID(is);
          species = is;
        }
      switch(species){
      case AliPID::kElectron:
        SetMarkerColor(kRed);
        SetLineColor(kRed);
        break;
      default:
        SetMarkerColor(kBlue);
        SetLineColor(kBlue);
        break;
      }
    }
    SetLineWidth(2);
  }
  
  const Char_t *model = "line";
  if(!TESTBIT(s, kTrackCosmics)){
    if(TESTBIT(s, kTrackModel)) model = "kalman";
    else model = "rieman";
  }
  Int_t species = 0; Float_t pid = 0.;
  for(Int_t is=0; is<AliPID::kSPECIES; is++) 
    if(trk->GetPID(is) > pid){
      pid = trk->GetPID(is);
      species = is;
    }

  SetTitle(Form(
    "Tracklets[%d] Clusters[%d]\n"
    "Reconstruction Source[%s]\n"
    "PID[%4.1f %4.1f %4.1f %4.1f %4.1f]\n"
    "MC[%d]", trk->GetNumberOfTracklets(), trk->GetNumberOfClusters(), fESDStatus&AliESDtrack::kTRDin ? "barrel" : "sa",
    1.E2*trk->GetPID(0), 1.E2*trk->GetPID(1),
    1.E2*trk->GetPID(2), 1.E2*trk->GetPID(3), 1.E2*trk->GetPID(4), trk->GetLabel()));

  if(GetName()){
    char id[6]; strncpy(id, GetName(), 6); 
    SetName(Form("%s %s", id, AliPID::ParticleName(species)));
  }

  // save track status
  fTrackState = s;
}


AliEveTRDTrackletOnline::AliEveTRDTrackletOnline(AliTRDtrackletMCM *tracklet) :
  TEveLine(),
  fDetector(-1),
  fROB(-1),
  fMCM(-1)
{
  AliTRDtrackletMCM *trkl = new AliTRDtrackletMCM(*tracklet);
  SetUserData(trkl);

  fDetector = trkl->GetDetector();
  fROB = trkl->GetROB();
  fMCM = trkl->GetMCM();

  SetName("TRD tracklet");
  SetTitle(Form("Det: %i, ROB: %i, MCM: %i, Label: %i\n0x%08x", 
                trkl->GetDetector(), trkl->GetROB(), trkl->GetMCM(), trkl->GetLabel(),
                trkl->GetTrackletWord()));
  SetLineColor(kGreen);

  AliTRDgeometry *geo = new AliTRDgeometry();
//  TGeoHMatrix *matrix = geo->GetClusterMatrix(trkl->GetDetector());

  Float_t length = 3.;
  Double_t x[3];
  Double_t p[3];
  x[0] = trkl->GetX();
  x[1] = trkl->GetY();
  x[2] = trkl->GetZ();

  fDetector = trkl->GetDetector();
  AliTRDpadPlane *pp = geo->GetPadPlane(geo->GetLayer(fDetector), geo->GetStack(fDetector));
  fROB = 2 * (trkl->GetZbin() / 4) + (trkl->GetY() > 0 ? 1 : 0);
  fMCM = (((Int_t) ((trkl->GetY()) / pp->GetWidthIPad()) + 72) / 18) % 4 
    + 4 * (trkl->GetZbin() % 4) ;
  AliInfo(Form("From position/tracklet: ROB: %i/%i, MCM: %i/%i", 
               fROB, trkl->GetROB(), fMCM, trkl->GetMCM()));  
  
  geo->RotateBack(trkl->GetDetector(), x, p);
//  matrix->LocalToMaster(x, p);
  SetPoint(0, p[0], p[1], p[2]);

  x[0] -= length;
  x[1] -= length * trkl->GetdYdX();
  x[2] *= x[0] / (x[0] + length);
  geo->RotateBack(trkl->GetDetector(), x, p);
//  matrix->LocalToMaster(x, p);
  SetPoint(1, p[0], p[1], p[2]);
  delete geo;
}

AliEveTRDTrackletOnline::AliEveTRDTrackletOnline(AliTRDtrackletWord *tracklet) :
  TEveLine(),
  fDetector(-1),
  fROB(-1),
  fMCM(-1)
{
  AliTRDtrackletWord *trkl = new AliTRDtrackletWord(*tracklet);
  SetUserData(trkl);

  AliTRDgeometry *geo = new AliTRDgeometry();
  fDetector = trkl->GetDetector();
  AliTRDpadPlane *pp = geo->GetPadPlane(geo->GetLayer(fDetector), geo->GetStack(fDetector));
  fROB = 2 * (trkl->GetZbin() / 4) + (trkl->GetY() > 0 ? 1 : 0);
  fMCM = (((Int_t) ((trkl->GetY()) / pp->GetWidthIPad()) + 72) / 18) % 4 
    + 4 * (trkl->GetZbin() % 4) ;

  SetName("TRD tracklet");
  SetTitle(Form("Det: %i, ROB: %i, MCM: %i, Label: %i\n0x%08x", 
                trkl->GetDetector(), fROB, fMCM, -1,
                trkl->GetTrackletWord()));
  SetLineColor(kRed);

//  AliTRDgeometry *geo = new AliTRDgeometry();

  Float_t length = 3.;
  Double_t x[3];
  Double_t p[3];
  x[0] = trkl->GetX();
  x[1] = trkl->GetY();
  x[2] = trkl->GetZ();
  
  geo->RotateBack(trkl->GetDetector(), x, p);
  SetPoint(0, p[0], p[1], p[2]);

  x[0] -= length;
  x[1] -= length * trkl->GetdYdX();
  x[2] *= x[0] / (x[0] + length);
  geo->RotateBack(trkl->GetDetector(), x, p);
  SetPoint(1, p[0], p[1], p[2]);
  delete geo;
}

AliEveTRDTrackletOnline::~AliEveTRDTrackletOnline() 
{
  AliTRDtrackletMCM *trkl = dynamic_cast<AliTRDtrackletMCM*> ((AliTRDtrackletBase*) GetUserData());
  printf("trkl: %p\n", (void*)trkl);
//  delete trkl;
  AliTRDtrackletWord *trklWord = dynamic_cast<AliTRDtrackletWord*> ((AliTRDtrackletBase*) GetUserData());
  printf("trklWord: %p\n", (void*)trklWord);
//  delete trklWord;
}

void AliEveTRDTrackletOnline::ShowMCM(Option_t *opt) const
{
  if (fDetector < 0 || fROB < 0 || fMCM < 0)
    return;

  AliEveTRDmcm *evemcm = new AliEveTRDmcm();
  evemcm->Init(fDetector, fROB, fMCM);
  evemcm->LoadDigits();
  evemcm->Draw(opt);

  TEveElementList *mcmlist = 0x0;
  if (gEve->GetCurrentEvent()) 
    mcmlist = (TEveElementList*) gEve->GetCurrentEvent()->FindChild("TRD MCMs");
  if (!mcmlist) {
    mcmlist = new TEveElementList("TRD MCMs");
    gEve->AddElement(mcmlist);
  }
  gEve->AddElement(evemcm, mcmlist);
}


AliEveTRDmcm::AliEveTRDmcm() :
  TEveElement(),
  TNamed(),
  fMCM(new AliTRDmcmSim())
{
  SetName("MCM");
  SetTitle("Unknown MCM");
}

AliEveTRDmcm::~AliEveTRDmcm()
{
  delete fMCM;
}

Bool_t AliEveTRDmcm::Init(Int_t det, Int_t rob, Int_t mcm)
{
  SetName(Form("MCM: Det. %i, ROB %i, MCM %i", det, rob, mcm));
  SetTitle(Form("MCM: Det. %i, ROB %i, MCM %i", det, rob, mcm));
  fMCM->Init(det, rob, mcm);
  fMCM->Reset();
  return kTRUE;
}

Bool_t AliEveTRDmcm::LoadDigits()
{
  AliRunLoader *rl = AliEveEventManager::AssertRunLoader();
  return fMCM->LoadMCM(rl, fMCM->GetDetector(), 
                       fMCM->GetRobPos(), fMCM->GetMcmPos());
}

Bool_t AliEveTRDmcm::Filter()
{
  fMCM->Filter();
  return kTRUE;
}

Bool_t AliEveTRDmcm::Tracklet()
{
  fMCM->Tracklet();
  return kTRUE;
}

void AliEveTRDmcm::Draw(Option_t* option)
{
  const char *mcmname = Form("mcm_%i_%i_%i", fMCM->GetDetector(),
                             fMCM->GetRobPos(), fMCM->GetMcmPos());

  TCanvas *c = dynamic_cast<TCanvas*> (gROOT->FindObject(mcmname));
  if (!c)
    c = gEve->AddCanvasTab("TRD MCM");
  c->SetTitle(Form("MCM %i on ROB %i of det. %i", 
                   fMCM->GetMcmPos(), fMCM->GetRobPos(), fMCM->GetDetector()));
  c->SetName(mcmname);
  c->cd();
  fMCM->Draw(option);
}

Bool_t AliEveTRDmcm::AssignPointer(const char* ptrname)
{
  gROOT->ProcessLine(Form("AliTRDmcmSim* %s = (AliTRDmcmSim *) 0x%x", ptrname, fMCM));
  return kTRUE;
}

// @(#) $Id$

// Author: Anders Vestbo <mailto:vestbo@fi.uib.no>
//*-- Copyright &copy ALICE HLT Group 

#include "AliL3StandardIncludes.h"
#include <TCanvas.h>
#include <TView.h>
#include <TPolyMarker3D.h>
#include <TPolyLine3D.h>
#include <TH2.h>
#include <TTree.h>
#include <TNode.h>
#include <TGeometry.h>
#include <TShape.h>
#include <TParticle.h>
#include <TFile.h>
#ifdef use_aliroot
#include <TClonesArray.h>
#include <AliRun.h>
#include <AliSimDigits.h>
#include <AliTPCParam.h>
#endif

#include "AliL3Logging.h"
#include "AliL3Display.h"
#include "AliL3Transform.h"
#include "AliL3Track.h"
#include "AliL3TrackArray.h"
#include "AliL3SpacePointData.h"
#include "AliL3MemHandler.h"

#if __GNUC__ == 3
using namespace std;
#endif

/** \class AliL3Display
<pre>
//_____________________________________________________________
// AliL3Display
//
// Simple display class for the HLT tracks.
</pre>
*/

ClassImp(AliL3Display)

AliL3Display::AliL3Display()
{
  //constructor
  fGeom = NULL;
  fTracks = NULL;
}

AliL3Display::AliL3Display(Int_t *slice,Char_t *gfile)
{
  //Ctor. Specify which slices you want to look at.

  TFile *file = TFile::Open(gfile);
  if(!file)
    {
      LOG(AliL3Log::kError,"AliL3Display::AliL3Display","File Open")
	<<"Geometry file " << gfile << " does not exist!"<<ENDLOG;
      return;
    }
  
  fGeom = (TGeometry*)file->Get("AliceGeom");
  fMinSlice = slice[0];
  fMaxSlice = slice[1];

  file->Close();
  delete file;
}

AliL3Display::~AliL3Display()
{
  //destructor
  if(fTracks)
    delete fTracks;
}

void AliL3Display::Setup(Char_t *trackfile,Char_t *path,Int_t event,Bool_t sp)
{
  //Read in the hit and track information from produced files.
  
  Char_t fname[256];
  AliL3MemHandler *clusterfile[36][6];
  memset(fClusters,0,36*6*sizeof(AliL3SpacePointData*));
  for(Int_t s=fMinSlice; s<=fMaxSlice; s++)
    {
      for(Int_t p=0; p<AliL3Transform::GetNPatches(); p++)
	{
	  Int_t patch;
	  if(sp==kTRUE)
	    patch=-1;
	  else
	    patch=p;
	  clusterfile[s][p] = new AliL3MemHandler();
	  if(event<0)
	    sprintf(fname,"%s/points_%d_%d.raw",path,s,patch);
	  else
	    sprintf(fname,"%s/points_%d_%d_%d.raw",path,event,s,patch);
	  if(!clusterfile[s][p]->SetBinaryInput(fname))
	    {
	      LOG(AliL3Log::kError,"AliL3Evaluation::Setup","File Open")
		<<"Inputfile "<<fname<<" does not exist"<<ENDLOG; 
	      delete clusterfile[s][p];
              clusterfile[s][p] = 0; 
	      continue;
	    }
	  fClusters[s][p] = (AliL3SpacePointData*)clusterfile[s][p]->Allocate();
	  clusterfile[s][p]->Binary2Memory(fNcl[s][p],fClusters[s][p]);
	  clusterfile[s][p]->CloseBinaryInput();
	  if(sp==kTRUE)
	    break;
	}
    }
  
  if(!trackfile) return;
  AliL3MemHandler *tfile = new AliL3MemHandler();
  if(!tfile->SetBinaryInput(trackfile))
    {
      LOG(AliL3Log::kError,"AliL3Evaluation::Setup","File Open")
	<<"Inputfile "<<trackfile<<" does not exist"<<ENDLOG; 
      return;
    }
  fTracks = new AliL3TrackArray();
  tfile->Binary2TrackArray(fTracks);
  tfile->CloseBinaryInput();
  delete tfile;

}

void AliL3Display::DisplayTracks(Int_t min_hits,Bool_t x3don,Float_t thr)
{
  //Display the found tracks.

  TCanvas *c1 = new TCanvas("c1","",700,700);
  c1->cd();
  
  TView *v = new TView(1);
  v->SetRange(-430,-560,-430,430,560,1710);
  c1->Clear();
  c1->SetFillColor(1);
  c1->SetTheta(45.);
  c1->SetPhi(0.);
    
  Int_t ntracks = fTracks->GetNTracks();
  TPolyLine3D *line = new TPolyLine3D[ntracks];
  Float_t xcl[176];
  Float_t ycl[176];
  Float_t zcl[176];
  
  for(Int_t j=0; j<ntracks; j++)
    {
      AliL3Track *gtrack = fTracks->GetCheckedTrack(j); 
      if(!gtrack) continue;
      if((thr>=0)&&(gtrack->GetPt()<thr)) continue;        
      Int_t nHits = gtrack->GetNHits();
      UInt_t *hitnum = gtrack->GetHitNumbers();
      if(nHits < min_hits) continue;
      TPolyMarker3D *pm = new TPolyMarker3D(nHits);
      Int_t hitcount=0;
      for(Int_t h=0; h<nHits; h++)
	{

	  UInt_t id=hitnum[h];
	  Int_t slice = (id>>25) & 0x7f;
	  Int_t patch = (id>>22) & 0x7;
	  UInt_t pos = id&0x3fffff;	      
	  //cout << h << " id " << pos << endl;
	  AliL3SpacePointData *points = fClusters[slice][patch];
	  if(slice < fMinSlice || slice > fMaxSlice)
	    continue;

	  if(!points) {
	    LOG(AliL3Log::kError,"AliL3Display::DisplayTracks","Clusterarray")
	      <<"No points at slice "<<slice<<" patch "<<patch<<" pos "<<pos<<ENDLOG;
	    continue;
	  }
	  if(pos>=fNcl[slice][patch]){
	    LOG(AliL3Log::kError,"AliL3Display::DisplayTracks","Clusterarray")
	      <<"Pos is too large: pos "<<pos <<" ncl "<<fNcl[slice][patch]<<ENDLOG;
	    continue;
	  }

	  Float_t xyz_tmp[3];
	  xyz_tmp[0] = points[pos].fX;
	  xyz_tmp[1] = points[pos].fY;
	  xyz_tmp[2] = points[pos].fZ;
	  	  
	  xcl[h] = xyz_tmp[0];
	  ycl[h] = xyz_tmp[1];
	  zcl[h] = xyz_tmp[2];
	  
	  pm->SetPoint(h,xcl[h],ycl[h],zcl[h]);
	  hitcount++;
	}
      if(hitcount==0) continue;
      pm->SetMarkerColor(2);
      pm->Draw();
      TPolyLine3D *current_line = &(line[j]);
      current_line = new TPolyLine3D(nHits,xcl,ycl,zcl,"");
      
      current_line->SetLineColor(4);
      current_line->Draw("same");
            
    }
  
  //Take this if you want black&white display for printing.
  Char_t fname[256];
  Int_t i;
  Int_t color = 1;
  c1->SetFillColor(10);
  for(i=0; i<10; i++)
    {
      sprintf(fname,"LS0%d",i);
      fGeom->GetNode(fname)->SetLineColor(color);
      sprintf(fname,"US0%d",i);
      fGeom->GetNode(fname)->SetLineColor(color);
    }
  for(i=10; i<18; i++)
    {
      sprintf(fname,"LS%d",i);
      fGeom->GetNode(fname)->SetLineColor(color);
      sprintf(fname,"US%d",i);
      fGeom->GetNode(fname)->SetLineColor(color);
    }
  
  fGeom->Draw("same");
  
  if(x3don) c1->x3d();
  
}

void AliL3Display::DisplayClusters(Bool_t x3don)
{
  //Display all clusters.
  
  TCanvas *c1 = new TCanvas("c1","",700,700);
  c1->cd();

  TView *v = new TView(1);
  v->SetRange(-430,-560,-430,430,560,1710);
  c1->Clear();
  c1->SetFillColor(1);
  c1->SetTheta(90.);
  c1->SetPhi(0.);
  
  for(Int_t s=fMinSlice; s<=fMaxSlice; s++)
    {
      for(Int_t p=0;p<6;p++)
	{
	  AliL3SpacePointData *points = fClusters[s][p];
	  if(!points) continue;
	  Int_t npoints = fNcl[s][p];
	  TPolyMarker3D *pm = new TPolyMarker3D(npoints);
	  
	  Float_t xyz[3];
	  for(Int_t i=0; i<npoints; i++)
	    {
	      xyz[0] = points[i].fX;
	      xyz[1] = points[i].fY;
	      xyz[2] = points[i].fZ;
	      //AliL3Transform::Local2Global(xyz,s);
	      pm->SetPoint(i,xyz[0],xyz[1],xyz[2]); 
 	    }
	  pm->SetMarkerColor(2);
	  pm->Draw("");
	}
    }
  fGeom->Draw("same");
  
  if(x3don) c1->x3d(); 
}


void AliL3Display::DisplayAll(Int_t min_hits,Bool_t x3don)
{
  //Display tracks & all hits.

  TCanvas *c1 = new TCanvas("c1","",700,700);
  c1->cd();
  TView *v = new TView(1);
  v->SetRange(-430,-560,-430,430,560,1710);
  c1->Clear();
  c1->SetFillColor(1);
  c1->SetTheta(90.);
  c1->SetPhi(0.);
  
  for(Int_t s=fMinSlice; s<=fMaxSlice; s++)
    {
      for(Int_t p=0;p<6;p++)
	{
	  AliL3SpacePointData *points = fClusters[s][p];
	  if(!points) continue;
	  Int_t npoints = fNcl[s][p];
	  TPolyMarker3D *pm = new TPolyMarker3D(npoints);
	  
	  Float_t xyz[3];
	  for(Int_t i=0; i<npoints; i++){
	    xyz[0] = points[i].fX;
	    xyz[1] = points[i].fY;
	    xyz[2] = points[i].fZ;

	    pm->SetPoint(i,xyz[0],xyz[1],xyz[2]); 
	    
	  }
	  pm->SetMarkerColor(2);
	  pm->Draw("");
	}
    }
  
  Int_t ntracks = fTracks->GetNTracks();
  TPolyLine3D *line = new TPolyLine3D[ntracks];
  Float_t xcl[176];
  Float_t ycl[176];
  Float_t zcl[176];
  
  for(Int_t j=0; j<ntracks; j++)
    {
      AliL3Track *gtrack = fTracks->GetCheckedTrack(j); 
      if(!gtrack) continue;        
      Int_t nHits = gtrack->GetNHits();
      UInt_t *hitnum = gtrack->GetHitNumbers();
      if(nHits < min_hits) continue;
      TPolyMarker3D *pm = new TPolyMarker3D(nHits);
      Int_t hitcount=0;
      for(Int_t h=0; h<nHits; h++)
	{
	  UInt_t id=hitnum[h];
	  Int_t slice = (id>>25) & 0x7f;
	  Int_t patch = (id>>22) & 0x7;
	  UInt_t pos = id&0x3fffff;	      
	  if(slice < fMinSlice || slice > fMaxSlice)
	    continue;
	  
	  AliL3SpacePointData *points = fClusters[slice][patch];
	  if(!points) {
	    LOG(AliL3Log::kError,"AliL3Display::DisplayAll","Clusterarray")
	      <<"No points at slice "<<slice<<" patch "<<patch<<" pos "<<pos<<ENDLOG;
	    continue;
	  }
	  if(pos>=fNcl[slice][patch]) {
	    LOG(AliL3Log::kError,"AliL3Display::DisplayAll","Clusterarray")
	      <<"Pos is too large: pos "<<pos <<" ncl "<<fNcl[slice][patch]<<ENDLOG;
	    continue;
	  }
	  xcl[h] = points[pos].fX;
	  ycl[h] = points[pos].fY;
	  zcl[h] = points[pos].fZ;
	  pm->SetPoint(h,xcl[h],ycl[h],zcl[h]);
	  hitcount++;
	}
      if(hitcount==0) continue;
      pm->SetMarkerColor(3);
      pm->Draw();
      TPolyLine3D *current_line = &(line[j]);
      current_line = new TPolyLine3D(nHits,xcl,ycl,zcl,"");
      current_line->SetLineColor(4);
      current_line->SetLineWidth(2);
      current_line->Draw("same");
    }
  
  Char_t fname[256];
  Int_t i;
  Int_t color = 1;
  c1->SetFillColor(10);
  for(i=0; i<10; i++)
    {
      sprintf(fname,"LS0%d",i);
      fGeom->GetNode(fname)->SetLineColor(color);
      sprintf(fname,"US0%d",i);
      fGeom->GetNode(fname)->SetLineColor(color);
    }
  for(i=10; i<18; i++)
    {
      sprintf(fname,"LS%d",i);
      fGeom->GetNode(fname)->SetLineColor(color);
      sprintf(fname,"US%d",i);
      fGeom->GetNode(fname)->SetLineColor(color);
    }
    
  fGeom->Draw("same");
  
  if(x3don) c1->x3d();
}

void AliL3Display::DisplayClusterRow(Int_t slice,Int_t padrow,Char_t *digitsFile,Char_t *type)
{
  //Display the found clusters on this row together with the raw data.
  
#ifdef use_aliroot
  TFile *file = new TFile(digitsFile);
  AliTPCParam *param = (AliTPCParam*)file->Get(AliL3Transform::GetParamName());

  Char_t dname[100];
  sprintf(dname,"TreeD_%s_0",AliL3Transform::GetParamName());
  TTree *TD=(TTree*)file->Get(dname);
  AliSimDigits da, *digits=&da;
  TD->GetBranch("Segment")->SetAddress(&digits); //Return pointer to branch segment.
  
  Int_t sector,row;
  AliL3Transform::Slice2Sector(slice,padrow,sector,row);
  Int_t npads = param->GetNPads(sector,row);
  Int_t ntimes = param->GetMaxTBin();
  TH2F *histdig = new TH2F("histdig","",npads,0,npads-1,ntimes,0,ntimes-1);
  TH2F *histfast = new TH2F("histfast","",npads,0,npads-1,ntimes,0,ntimes-1);
  TH2F *histpart = new TH2F("histpart","",npads,0,npads-1,ntimes,0,ntimes-1);

  
  Int_t sectors_by_rows=(Int_t)TD->GetEntries();
  Int_t i;
  for (i=0; i<sectors_by_rows; i++) {
    if (!TD->GetEvent(i)) continue;
    Int_t sec,ro;
    param->AdjustSectorRow(digits->GetID(),sec,ro);
    
    if(sec != sector) continue;
    if(ro < row) continue;
    if(ro != row) break;
    printf("sector %d row %d\n",sec,ro);
    digits->First();
    while (digits->Next()) {
      Int_t it=digits->CurrentRow(), ip=digits->CurrentColumn();
      Short_t dig = digits->GetDigit(it,ip);
      if(dig<=param->GetZeroSup()) continue;
      /*
      if(it < param->GetMaxTBin()-1 && it > 0)
	if(digits->GetDigit(it+1,ip) <= param->GetZeroSup()
	   && digits->GetDigit(it-1,ip) <= param->GetZeroSup())
	  continue;
      */
      histdig->Fill(ip,it,dig);
    }
  }
  
  /*file->cd();
  AliRun *gAlice = (AliRun*)file->Get("gAlice");
  gAlice->GetEvent(0);
  TClonesArray *fParticles=gAlice->Particles(); 
  TParticle *part = (TParticle*)fParticles->UncheckedAt(0);
  AliL3Evaluate *eval = new AliL3Evaluate();
  Float_t xyz_cross[3];
  */
  
  for(Int_t p=0;p<6;p++)
    {
      AliL3SpacePointData *points = fClusters[slice][p];
      if(!points) continue;
      
      Int_t npoints = fNcl[slice][p];     
      Float_t xyz[3];
      for(Int_t i=0; i<npoints; i++)
	{
	  if(points[i].fPadRow != padrow) continue;
	  xyz[0] = points[i].fX;
	  xyz[1] = points[i].fY;
	  xyz[2] = points[i].fZ;
	  AliL3Transform::Global2Raw(xyz,sector,row);
	  //AliL3Transform::Local2Raw(xyz,sector,row);
	  histfast->Fill(xyz[1],xyz[2],1);
	  
	  
	}
      
    }
  
  TCanvas *c1 = new TCanvas("c1","",900,900);
  c1->cd();
  histdig->Draw();
  histfast->SetMarkerColor(2);
  histfast->SetMarkerStyle(4);
  histpart->SetMarkerColor(2);
  histpart->SetMarkerStyle(3);

  histdig->GetXaxis()->SetTitle("Pad #");
  histdig->GetYaxis()->SetTitle("Timebin #");
  histdig->Draw(type);
  histfast->Draw("psame");
  //histpart->Draw("psame");

#endif
  return;
}


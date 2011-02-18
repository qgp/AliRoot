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


///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//     Class to analyse tracks for calibration                               //
//     to be used as a component in AliTPCSelectorTracks                     //
//     In the constructor you have to specify name and title                 //
//     to get the Object out of a file.                                      //
//     The parameter 'clusterParam', a AliTPCClusterParam object             //
//      (needed for TPC cluster error and shape parameterization)            //
//     Normally you get this object out of the file 'TPCClusterParam.root'   //
//     In the parameter 'cuts' the cuts are specified, that decide           //
//     weather a track will be accepted for calibration or not.              //
//                                                                           //
//       
//     The data flow:
//     
/*
   Raw Data -> Local Reconstruction -> Tracking ->     Calibration -> RefData (component itself)
               Offline/HLT             Offline/HLT                    OCDB entries (AliTPCClusterParam) 
*/            


                                                               //
///////////////////////////////////////////////////////////////////////////////

//
// ROOT includes 
//
#include <iostream>
#include <fstream>
using namespace std;

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH3F.h>
#include <TProfile.h>

//
//#include <TPDGCode.h>
#include <TStyle.h>
#include "TLinearFitter.h"
//#include "TMatrixD.h"
#include "TTreeStream.h"
#include "TF1.h"
#include <TCanvas.h>
#include <TGraph2DErrors.h>
#include "TPostScript.h"
#include "TCint.h"

#include <TH2D.h>
#include <TF2.h>
#include <TSystem.h>
#include <TCollection.h>
#include <iostream>
#include <TLinearFitter.h>
#include <TString.h>

//
// AliROOT includes 
//
#include "AliMagF.h"
#include "AliTracker.h"
#include "AliESD.h"
#include "AliESDtrack.h"
#include "AliESDfriend.h"
#include "AliESDfriendTrack.h" 
#include "AliTPCseed.h"
#include "AliTPCclusterMI.h"
#include "AliTPCROC.h"

#include "AliTPCParamSR.h"
#include "AliTrackPointArray.h"
#include "AliTPCcalibTracks.h"
#include "AliTPCClusterParam.h"
#include "AliTPCcalibTracksCuts.h"
#include "AliTPCCalPad.h"
#include "AliTPCCalROC.h"
#include "TText.h"
#include "TPaveText.h"
#include "TSystem.h"
#include "TStatToolkit.h"
#include "TCut.h"
#include "THnSparse.h"
#include "AliRieman.h"



ClassImp(AliTPCcalibTracks)


AliTPCcalibTracks::AliTPCcalibTracks():
  AliTPCcalibBase(),
  fClusterParam(0),
  fROC(0),
  fHisDeltaY(0),    // THnSparse - delta Y 
  fHisDeltaZ(0),    // THnSparse - delta Z 
  fHisRMSY(0),      // THnSparse - rms Y 
  fHisRMSZ(0),      // THnSparse - rms Z 
  fHisQmax(0),      // THnSparse - qmax 
  fHisQtot(0),      // THnSparse - qtot 
  fArrayQDY(0), 
  fArrayQDZ(0), 
  fArrayQRMSY(0),
  fArrayQRMSZ(0),
  fResolY(0),
  fResolZ(0),
  fRMSY(0),
  fRMSZ(0),
  fCuts(0),
  fRejectedTracksHisto(0),
  fClusterCutHisto(0),
  fCalPadClusterPerPad(0),
  fCalPadClusterPerPadRaw(0)
{ 
   // 
   // AliTPCcalibTracks default constructor
   //    
  if (GetDebugLevel() > 0) cout << "AliTPCcalibTracks' default constructor called" << endl;  
}   



AliTPCcalibTracks::AliTPCcalibTracks(const AliTPCcalibTracks& calibTracks):
  AliTPCcalibBase(calibTracks),
  fClusterParam(0),
  fROC(0),
  fHisDeltaY(0),    // THnSparse - delta Y 
  fHisDeltaZ(0),    // THnSparse - delta Z 
  fHisRMSY(0),      // THnSparse - rms Y 
  fHisRMSZ(0),      // THnSparse - rms Z 
  fHisQmax(0),      // THnSparse - qmax 
  fHisQtot(0),      // THnSparse - qtot 
  fArrayQDY(0), 
  fArrayQDZ(0), 
  fArrayQRMSY(0),
  fArrayQRMSZ(0),
  fResolY(0),
  fResolZ(0),
  fRMSY(0),
  fRMSZ(0),
  fCuts(0),
  fRejectedTracksHisto(0),
  fClusterCutHisto(0),
  fCalPadClusterPerPad(0),
  fCalPadClusterPerPadRaw(0)
{
   // 
   // AliTPCcalibTracks copy constructor
   // 
  if (GetDebugLevel() > 0) cout << " ***** this is AliTPCcalibTracks' copy constructor ***** " << endl;
   
   Bool_t dirStatus = TH1::AddDirectoryStatus();
   TH1::AddDirectory(kFALSE);
   
   Int_t length = -1;
   
   (calibTracks.fArrayQDY) ? length = calibTracks.fArrayQDY->GetEntriesFast() : length = -1;
   fArrayQDY= new TObjArray(length);
   fArrayQDZ= new TObjArray(length);
   fArrayQRMSY= new TObjArray(length);
   fArrayQRMSZ= new TObjArray(length);
   for (Int_t i = 0; i < length; i++) {
      fArrayQDY->AddAt( ((TH1F*)calibTracks.fArrayQDY->At(i)->Clone()), i);
      fArrayQDZ->AddAt( ((TH1F*)calibTracks.fArrayQDZ->At(i)->Clone()), i);
      fArrayQRMSY->AddAt( ((TH1F*)calibTracks.fArrayQRMSY->At(i)->Clone()), i);
      fArrayQRMSZ->AddAt( ((TH1F*)calibTracks.fArrayQRMSZ->At(i)->Clone()), i);
   }
   
   (calibTracks.fResolY) ? length = calibTracks.fResolY->GetEntriesFast() : length = -1;
   fResolY = new TObjArray(length);
   fResolZ = new TObjArray(length);
   fRMSY = new TObjArray(length);
   fRMSZ = new TObjArray(length);
   for (Int_t i = 0; i < length; i++) {
      fResolY->AddAt( ((TH1F*)calibTracks.fResolY->At(i)->Clone()), i);
      fResolZ->AddAt( ((TH1F*)calibTracks.fResolZ->At(i)->Clone()), i);
      fRMSY->AddAt( ((TH1F*)calibTracks.fRMSY->At(i)->Clone()), i);
      fRMSZ->AddAt( ((TH1F*)calibTracks.fRMSZ->At(i)->Clone()), i);
   } 
   
   
   fClusterCutHisto = (TH2I*)calibTracks.fClusterCutHisto->Clone();
   fRejectedTracksHisto    = (TH1I*)calibTracks.fRejectedTracksHisto->Clone();
   fCalPadClusterPerPad    = (AliTPCCalPad*)calibTracks.fCalPadClusterPerPad->Clone();
   fCalPadClusterPerPadRaw = (AliTPCCalPad*)calibTracks.fCalPadClusterPerPadRaw->Clone();

   fCuts = new AliTPCcalibTracksCuts(calibTracks.fCuts->GetMinClusters(), calibTracks.fCuts->GetMinRatio(), 
      calibTracks.fCuts->GetMax1pt(), calibTracks.fCuts->GetEdgeYXCutNoise(), calibTracks.fCuts->GetEdgeThetaCutNoise());
   SetNameTitle(calibTracks.GetName(), calibTracks.GetTitle());
   TH1::AddDirectory(dirStatus); // set status back to original status
//    cout << "+++++ end of copy constructor +++++" << endl;   // TO BE REMOVED
}


AliTPCcalibTracks & AliTPCcalibTracks::operator=(const AliTPCcalibTracks& calibTracks){
  //
  // assgnment operator
  //
  if (this != &calibTracks) {
    new (this) AliTPCcalibTracks(calibTracks);
  }
  return *this;

}


AliTPCcalibTracks::AliTPCcalibTracks(const Text_t *name, const Text_t *title, AliTPCClusterParam *clusterParam,  AliTPCcalibTracksCuts* cuts, Int_t logLevel) : 
  AliTPCcalibBase(),
  fClusterParam(0),
  fROC(0),
  fHisDeltaY(0),    // THnSparse - delta Y 
  fHisDeltaZ(0),    // THnSparse - delta Z 
  fHisRMSY(0),      // THnSparse - rms Y 
  fHisRMSZ(0),      // THnSparse - rms Z 
  fHisQmax(0),      // THnSparse - qmax 
  fHisQtot(0),      // THnSparse - qtot 
  fArrayQDY(0), 
  fArrayQDZ(0), 
  fArrayQRMSY(0),
  fArrayQRMSZ(0),
  fResolY(0),
  fResolZ(0),
  fRMSY(0),
  fRMSZ(0),
  fCuts(0),
  fRejectedTracksHisto(0),
  fClusterCutHisto(0),
  fCalPadClusterPerPad(0),
  fCalPadClusterPerPadRaw(0)
 {
   // 
   // AliTPCcalibTracks constructor
   // specify 'name' and 'title' of your object
   // specify 'clusterParam', (needed for TPC cluster error and shape parameterization)
   // In the parameter 'cuts' the cuts are specified, that decide           
   // weather a track will be accepted for calibration or not.              
   //
   // fDebugLevel - debug output: -1: silence, 0: default, 1: things like constructor called, 5: write fDebugStreamer, 6: waste your screen
   // 
   // All histograms are instatiated in this constructor.
   // 
   this->SetName(name);
   this->SetTitle(title);

   if (GetDebugLevel() > 0) cout << " ***** this is AliTPCcalibTracks' main constructor ***** " << endl;
   G__SetCatchException(0);     
   
   fClusterParam = clusterParam;
   if (fClusterParam){
     fClusterParam->SetInstance(fClusterParam);
   }
   else {
     Error("AliTPCcalibTracks","No cluster parametrization found! A valid clusterParam object is needed in the constructor. (To be found in 'TPCClusterParam.root'.)");
   } 
   fCuts = cuts;
   SetDebugLevel(logLevel);
   MakeHistos();
   
   TH1::AddDirectory(kFALSE);
   
   fRejectedTracksHisto    = new TH1I("RejectedTracksHisto", "Rejected tracks, sorted by failed cut", 100, -1, 10);
   fCalPadClusterPerPad    = new AliTPCCalPad("fCalPadClusterPerPad", "clusters per pad");
   fCalPadClusterPerPadRaw = new AliTPCCalPad("fCalPadClusterPerPadRaw", "clusters per pad, before cutting clusters");
   fClusterCutHisto = new TH2I("fClusterCutHisto", "Cutted cluster over padRow; Cut Criterium; PadRow", 5,1,5, 160,0,159);
   
   
   TH1::AddDirectory(kFALSE);
   
   
   fResolY = new TObjArray(3);
   fResolZ = new TObjArray(3);
   fRMSY   = new TObjArray(3);
   fRMSZ   = new TObjArray(3);
   TH3F * his3D;
   //
   his3D = new TH3F("Resol Y0","Resol Y0", 5,20,250, 4, 0,1., 50, -1,1);
   fResolY->AddAt(his3D,0);	
   his3D = new TH3F("Resol Y1","Resol Y1", 5,20,250, 4, 0,1., 50, -1,1);
   fResolY->AddAt(his3D,1);
   his3D = new TH3F("Resol Y2","Resol Y2", 5,20,250, 4, 0,0.8, 50, -1,1);
   fResolY->AddAt(his3D,2);
   //
   his3D = new TH3F("Resol Z0","Resol Z0", 5,20,250, 4, 0,1, 50, -1,1);
   fResolZ->AddAt(his3D,0);
   his3D = new TH3F("Resol Z1","Resol Z1", 5,20,250, 4, 0,1, 50, -1,1);
   fResolZ->AddAt(his3D,1);
   his3D = new TH3F("Resol Z2","Resol Z2", 5,20,250, 4, 0,1, 50, -1,1);
   fResolZ->AddAt(his3D,2);
   //
   his3D = new TH3F("RMS Y0","RMS Y0", 5,20,250, 4, 0,1., 50, 0,0.8);
   fRMSY->AddAt(his3D,0);
   his3D = new TH3F("RMS Y1","RMS Y1", 5,20,250, 4, 0,1., 50, 0,0.8);
   fRMSY->AddAt(his3D,1);
   his3D = new TH3F("RMS Y2","RMS Y2", 5,20,250, 4, 0,0.8, 50, 0,0.8);
   fRMSY->AddAt(his3D,2);
   //
   his3D = new TH3F("RMS Z0","RMS Z0", 5,20,250, 4, 0,1, 50, 0,0.8);
   fRMSZ->AddAt(his3D,0);
   his3D = new TH3F("RMS Z1","RMS Z1", 5,20,250, 4, 0,1, 50, 0,0.8);
   fRMSZ->AddAt(his3D,1);
   his3D = new TH3F("RMS Z2","RMS Z2", 5,20,250, 4, 0,1, 50, 0,0.8);
   fRMSZ->AddAt(his3D,2);
   //
      
   TH1::AddDirectory(kFALSE);
   
   fArrayQDY = new TObjArray(300);
   fArrayQDZ = new TObjArray(300);
   fArrayQRMSY = new TObjArray(300);
   fArrayQRMSZ = new TObjArray(300);
   for (Int_t iq = 0; iq <= 10; iq++){
      for (Int_t ipad = 0; ipad < 3; ipad++){
         Int_t   bin   = GetBin(iq, ipad);
         Float_t qmean = GetQ(bin);
         char hname[200];
         snprintf(hname,100,"ResolY Pad%d Qmiddle%f",ipad, qmean);
         his3D = new TH3F(hname, hname, 20,10,250, 20, 0,1.5, 100, -1,1);
         fArrayQDY->AddAt(his3D, bin);
         snprintf(hname,100,"ResolZ Pad%d Qmiddle%f",ipad, qmean);
         his3D = new TH3F(hname, hname, 20,10,250, 20, 0,1.5, 100, -1,1);
         fArrayQDZ->AddAt(his3D, bin);
         snprintf(hname,100,"RMSY Pad%d Qmiddle%f",ipad, qmean);
         his3D = new TH3F(hname, hname, 20,10,250, 20, 0,1.5, 100, 0,0.6);
         fArrayQRMSY->AddAt(his3D, bin);
         snprintf(hname,100,"RMSZ Pad%d Qmiddle%f",ipad, qmean);
         his3D = new TH3F(hname, hname, 20,10,250, 20, 0,1.5, 100, 0,0.6);
         fArrayQRMSZ->AddAt(his3D, bin);
      }
   }
   
   

   if (GetDebugLevel() > 1) cout << "AliTPCcalibTracks object sucessfully constructed: " << GetName() << endl; 
   cout << "end of main constructor" << endl; // TO BE REMOVED
}    


AliTPCcalibTracks::~AliTPCcalibTracks() {
   // 
   // AliTPCcalibTracks destructor
   // 
   
  if (GetDebugLevel() > 0) cout << "AliTPCcalibTracks' destuctor called." << endl;
   Int_t length = 0;
   
   
   if (fResolY) {
     length = fResolY->GetEntriesFast();
     for (Int_t i = 0; i < length; i++){
       delete fResolY->At(i);
       delete fResolZ->At(i);
       delete fRMSY->At(i);
       delete fRMSZ->At(i);
     }
     delete fResolY;
     delete fResolZ;
     delete fRMSY;
     delete fRMSZ;
   }
   
   if (fArrayQDY) {
     length = fArrayQDY->GetEntriesFast();
     for (Int_t i = 0; i < length; i++){
       delete fArrayQDY->At(i);
       delete fArrayQDZ->At(i);
       delete fArrayQRMSY->At(i);
       delete fArrayQRMSZ->At(i);
     }
   }
   
   
    
   delete fArrayQDY;
   delete fArrayQDZ;
   delete fArrayQRMSY;
   delete fArrayQRMSZ;
   
  delete fRejectedTracksHisto;
  delete fClusterCutHisto;
  if (fCalPadClusterPerPad)    delete fCalPadClusterPerPad;
  if (fCalPadClusterPerPadRaw) delete fCalPadClusterPerPadRaw;
  delete fHisDeltaY;    // THnSparse - delta Y 
  delete fHisDeltaZ;    // THnSparse - delta Z 
  delete fHisRMSY;      // THnSparse - rms Y 
  delete fHisRMSZ;      // THnSparse - rms Z 
  delete fHisQmax;      // THnSparse - qmax 
  delete fHisQtot;      // THnSparse - qtot 

}
   
  

void AliTPCcalibTracks::Process(AliTPCseed *track){
   // 
   // To be called in the selector
   // first AcceptTrack is evaluated, then calls all the following analyse functions: 
   // FillResolutionHistoLocal(track)

   // 
  if (GetDebugLevel() > 5) Info("Process","Starting to process the track...");
   Int_t accpetStatus = AcceptTrack(track);
   if (accpetStatus == 0) {
      FillResolutionHistoLocal(track);
   }
   else fRejectedTracksHisto->Fill(accpetStatus);
}



Int_t AliTPCcalibTracks::GetBin(Float_t q, Int_t pad){
  //
  // calculate bins for given q and pad type 
  // used in TObjArray
  //
  Int_t res = TMath::Max( TMath::Nint((TMath::Sqrt(q) - 3.)), 0 );  
  res *= 3;
  res += pad;
  return res;
}


Int_t AliTPCcalibTracks::GetBin(Int_t iq, Int_t pad){
  //
  // calculate bins for given iq and pad type 
  // used in TObjArray
  //
  return iq * 3 + pad;;
}


Float_t AliTPCcalibTracks::GetQ(Int_t bin){
   // 
   // returns to bin belonging charge
   // (bin / 3 + 3)^2
   // 
   Int_t bin0 = bin / 3;
   bin0 += 3;
   return bin0 * bin0;
}


Float_t AliTPCcalibTracks::GetPad(Int_t bin){
   // 
   // returns to bin belonging pad
   // bin % 3
   // 
   return bin % 3; 
}



Int_t AliTPCcalibTracks::AcceptTrack(AliTPCseed * track){
  //
  // Function, that decides wheather a given track is accepted for 
  // the analysis or not. 
  // The cuts are specified in the AliTPCcalibTracksCuts object 'fCuts'
  // Returns 0 if a track is accepted or an integer different from 0 
  // to indicate the failed cut
  //
  const Int_t   kMinClusters  = fCuts->GetMinClusters();
  const Float_t kMinRatio     = fCuts->GetMinRatio();
  const Float_t kMax1pt       = fCuts->GetMax1pt();
  const Float_t kEdgeYXCutNoise    = fCuts->GetEdgeYXCutNoise();
  const Float_t kEdgeThetaCutNoise = fCuts->GetEdgeThetaCutNoise();
  
  //
  // edge induced noise tracks - NEXT RELEASE will be removed during tracking
  if ( TMath::Abs(track->GetY() / track->GetX()) > kEdgeYXCutNoise )
    if ( TMath::Abs(track->GetTgl()) < kEdgeThetaCutNoise ) return 1;
  if (track->GetNumberOfClusters() < kMinClusters) return 2;
  Float_t ratio = track->GetNumberOfClusters() / (track->GetNFoundable() + 1.);
  if (ratio < kMinRatio) return 3;
//   Float_t mpt = track->Get1Pt();       // Get1Pt() doesn't exist any more
  Float_t mpt = track->GetSigned1Pt();
  if (TMath::Abs(mpt) > kMax1pt) return 4;
  //if (TMath::Abs(track->GetZ())>240.) return kFALSE;
  //if (TMath::Abs(track->GetZ())<10.) return kFALSE;
  //if (TMath::Abs(track->GetTgl())>0.03) return kFALSE;
  
  if (GetDebugLevel() > 20) Info("AcceptTrack","Track has been accepted.");  
  return 0;
}


void  AliTPCcalibTracks::FillResolutionHistoLocal(AliTPCseed * track){
   //
   // fill resolution histograms - localy - tracklet in the neighborhood
   // write debug information to 'TPCSelectorDebug.root'
   // 
   // _ the main function, called during track analysis _
   // 
   // loop over all padrows along the track
   // fit tracklets (length: 13 clusters) calculate mean chi^2 for this track-fit in Y and Z direction
   // 
   // loop again over all padrows along the track
   // fit tracklet (clusters in given padrow +- kDelta padrows) 
   // with polynom of 2nd order and two polynoms of 1st order
   // take both polynoms of 1st order, calculate difference of their parameters
   // add covariance matrixes and calculate chi2 of this difference
   // if this chi2 is bigger than a given threshold, assume that the current cluster is
   // a kink an goto next padrow
   // if not:
   // fill fRMSY, fRMSZ, fArrayQRMSY and fArrayQRMSZ, fResolY, fResolZ, fArrayQDY, fArrayQDY
   // 
   // write debug information to 'TPCSelectorDebug.root'
   // only for every kDeltaWriteDebugStream'th padrow to reduce data volume 
   // and to avoid redundant data
   // 

  static TLinearFitter fFitterParY(3,"pol2");    // 
  static TLinearFitter fFitterParZ(3,"pol2");    //
  static TLinearFitter fFitterParYWeight(3,"pol2");    // 
  static TLinearFitter fFitterParZWeight(3,"pol2");    //
  fFitterParY.StoreData(kTRUE);
  fFitterParZ.StoreData(kTRUE);
  fFitterParYWeight.StoreData(kTRUE);
  fFitterParZWeight.StoreData(kTRUE);
  if (GetDebugLevel() > 5) Info("FillResolutionHistoLocal"," ***** Start of FillResolutionHistoLocal *****");
  const Int_t   kDelta     = 10;          // delta rows to fit
  const Float_t kMinRatio  = 0.75;        // minimal ratio
  const Float_t kChi2Cut   =  10.;          // cut chi2 - left right
  const Float_t kSigmaCut  = 3.;        //sigma cut
  const Float_t kdEdxCut=300;
  const Float_t kPtCut=0.300;

  if (track->GetTPCsignal()>kdEdxCut) return;  
  if (TMath::Abs(AliTracker::GetBz())>0.1 &&TMath::Abs(track->Pt())<kPtCut) return;  

  // estimate mean error
  Int_t nTrackletsAll = 0;       // number of tracklets for given track
  Float_t csigmaY     = 0;       // mean sigma for tracklet refit in Y direction
  Float_t csigmaZ     = 0;       // mean sigma for tracklet refit in Z direction
  Int_t nClusters     = 0;       // working variable, number of clusters per tracklet
  Int_t sectorG       = -1;      // working variable, sector of tracklet, has to stay constant for one tracklet
  Double_t refX=0;
  // ---------------------------------------------------------------------
  for (Int_t irow = 0; irow < 159; irow++){
    // loop over all rows along the track
    // fit tracklets (length: 13 rows) with pol2 in Y and Z direction
    // calculate mean chi^2 for this track-fit in Y and Z direction
    AliTPCclusterMI * cluster0 = track->GetClusterPointer(irow);
    if (!cluster0) continue;  // no cluster found
    Int_t sector = cluster0->GetDetector();
    
    Int_t ipad = TMath::Nint(cluster0->GetPad());
    Float_t value = fCalPadClusterPerPadRaw->GetCalROC(sector)->GetValue((sector<36)?irow:irow-64, TMath::Nint(cluster0->GetPad()));
    fCalPadClusterPerPadRaw->GetCalROC(sector)->SetValue((sector<36)?irow:irow-64, ipad, value + 1 );
    
    if (sector != sectorG){
      // track leaves sector before it crossed enough rows to fit / initialization
      nClusters = 0;
      fFitterParY.ClearPoints();
      fFitterParZ.ClearPoints();
      sectorG = sector;
    }
    else {
      nClusters++;
      if (refX<1) refX=cluster0->GetX()+kDelta*0.5;
      Double_t x = cluster0->GetX()-refX;
      fFitterParY.AddPoint(&x, cluster0->GetY(), 1);
      fFitterParZ.AddPoint(&x, cluster0->GetZ(), 1);
      //
      if ( nClusters >= kDelta + 3 ){  
	// if more than 13 (kDelta+3) clusters were added to the fitters
	// fit the tracklet, increase trackletCounter
	fFitterParY.Eval();
	fFitterParZ.Eval();
	nTrackletsAll++;
	csigmaY += fFitterParY.GetChisquare() / (nClusters - 3.);
	csigmaZ += fFitterParZ.GetChisquare() / (nClusters - 3.);
	nClusters = -1;
	fFitterParY.ClearPoints();
	fFitterParZ.ClearPoints();
	refX=0;
      }
    }
  }      // for (Int_t irow = 0; irow < 159; irow++)
  // mean chi^2 for all tracklet fits in Y and in Z direction: 
  csigmaY = TMath::Sqrt(TMath::Abs(csigmaY) / (nTrackletsAll+0.1));
  csigmaZ = TMath::Sqrt(TMath::Abs(csigmaZ) / (nTrackletsAll+0.1));
  // ---------------------------------------------------------------------
  //
  //

  for (Int_t irow = kDelta; irow < 159-kDelta; irow++){
    // loop again over all rows along the track
    // do analysis
    // 
    Int_t nclFound = 0;  // number of clusters in the neighborhood
    Int_t ncl0 = 0;      // number of clusters in rows < rowOfCenterCluster
    Int_t ncl1 = 0;      // number of clusters in rows > rowOfCenterCluster
    AliTPCclusterMI * cluster0 = track->GetClusterPointer(irow);
    if (!cluster0) continue;
    Int_t sector = cluster0->GetDetector();
    Float_t xref = cluster0->GetX();
    
    // Make Fit
    fFitterParY.ClearPoints();
    fFitterParZ.ClearPoints();    
    fFitterParYWeight.ClearPoints();
    fFitterParZWeight.ClearPoints();    
    // fit tracklet (clusters in given padrow +- kDelta padrows) 
    // with polynom of 2nd order and two polynoms of 1st order
    // take both polynoms of 1st order, calculate difference of their parameters
    // add covariance matrixes and calculate chi2 of this difference
    // if this chi2 is bigger than a given threshold, assume that the current cluster is
    // a kink an goto next padrow    
    AliRieman riemanFit(2*kDelta);
    AliRieman riemanFitW(2*kDelta);
    for (Int_t idelta = -kDelta; idelta <= kDelta; idelta++){
      // loop over irow +- kDelta rows (neighboured rows)
      // 
      // 
      if (idelta == 0) continue;                                // don't use center cluster
      if (idelta + irow < 0 || idelta + irow > 159) continue;   // don't go out of ROC
      AliTPCclusterMI * currentCluster = track->GetClusterPointer(irow + idelta);
      if (!currentCluster) continue;
      if (currentCluster->GetType() < 0) continue;
      if (currentCluster->GetDetector() != sector) continue;
      nclFound++;
      if (idelta < 0){
	ncl0++;
      }
      if (idelta > 0){
	ncl1++;
      }
      riemanFit.AddPoint(currentCluster->GetX(), currentCluster->GetY(),currentCluster->GetZ(), csigmaY,csigmaZ);
      riemanFitW.AddPoint(currentCluster->GetX(), currentCluster->GetY(),currentCluster->GetZ(), csigmaY*TMath::Sqrt(1+TMath::Abs(idelta)),csigmaZ*TMath::Sqrt(1+TMath::Abs(idelta)));
    }  // loop over neighbourhood for fitter filling 
    if (nclFound < kDelta * kMinRatio) fRejectedTracksHisto->Fill(10);
    if (nclFound < kDelta * kMinRatio) fClusterCutHisto->Fill(1, irow);
    if (nclFound < kDelta * kMinRatio) continue;    // if not enough clusters (7.5) found in neighbourhood goto next padrow
    riemanFit.Update();
    riemanFitW.Update();
    Double_t chi2R=TMath::Sqrt(riemanFit.CalcChi2()/(2*nclFound-5));
    Double_t chi2RW=TMath::Sqrt(riemanFitW.CalcChi2()/(2*nclFound-5));
    Double_t paramYR[3], paramZR[3];
    Double_t paramYRW[3], paramZRW[3];
    //
    paramYR[0]    = riemanFit.GetYat(xref);
    paramZR[0]    = riemanFit.GetZat(xref);
    paramYRW[0]   = riemanFitW.GetYat(xref);
    paramZRW[0]   = riemanFitW.GetZat(xref);
    //
    paramYR[1]    = riemanFit.GetDYat(xref);
    paramZR[1]    = riemanFit.GetDZat(xref);
    paramYRW[1]   = riemanFitW.GetDYat(xref);
    paramZRW[1]   = riemanFitW.GetDZat(xref);
    //
    Int_t reject=0;
    if (chi2R>kChi2Cut) reject+=1;
    if (chi2RW>kChi2Cut) reject+=2;
    if (TMath::Abs(paramYR[0]-paramYRW[0])>kSigmaCut*csigmaY) reject+=4;
    if (TMath::Abs(paramZR[0]-paramZRW[0])>kSigmaCut*csigmaZ) reject+=8;
    if (TMath::Abs(paramYR[1]-paramYRW[1])>csigmaY) reject+=16;
    if (TMath::Abs(paramZR[1]-paramZRW[1])>csigmaZ) reject+=32;
    //
    TTreeSRedirector *cstream = GetDebugStreamer();    
    // get fit parameters from pol2 fit:     
    Double_t tracky = paramYR[0];
    Double_t trackz = paramZR[0];
    Float_t  deltay = cluster0->GetY()-tracky;
    Float_t  deltaz = cluster0->GetZ()-trackz;
    Float_t  angley = paramYR[1];
    Float_t  anglez = paramZR[1];
    
    Int_t padSize = 0;                          // short pads
    if (cluster0->GetDetector() >= 36) {
      padSize = 1;                              // medium pads 
      if (cluster0->GetRow() > 63) padSize = 2; // long pads
    }
    Int_t ipad = TMath::Nint(cluster0->GetPad());
    if (cstream){
      Float_t zdrift = 250 - TMath::Abs(cluster0->GetZ());
      (*cstream)<<"Resol0"<<	
	"run="<<fRun<<              //  run number
	"event="<<fEvent<<          //  event number
	"time="<<fTime<<            //  time stamp of event
	"trigger="<<fTrigger<<      //  trigger
	"mag="<<fMagF<<             //  magnetic field	      
	"padSize="<<padSize<<
	//
	"reject="<<reject<<
	"cl.="<<cluster0<<          // cluster info
	"xref="<<xref<<             // cluster reference X
	//rieman fit
	"yr="<<paramYR[0]<<         // track position y - rieman fit
	"zr="<<paramZR[0]<<         // track position z - rieman fit 
	"yrW="<<paramYRW[0]<<         // track position y - rieman fit - weight
	"zrW="<<paramZRW[0]<<         // track position z - rieman fit - weight
	"dyr="<<paramYR[1]<<         // track position y - rieman fit
	"dzr="<<paramZR[1]<<         // track position z - rieman fit 
	"dyrW="<<paramYRW[1]<<         // track position y - rieman fit - weight
	"dzrW="<<paramZRW[1]<<         // track position z - rieman fit - weight
	//
	"angley="<<angley<<         // angle par fit
	"anglez="<<anglez<<         // angle par fit
	"zdr="<<zdrift<<            //
	"dy="<<deltay<<
	"dz="<<deltaz<<        
	"sy="<<csigmaY<<
	"sz="<<csigmaZ<<
	"chi2R="<<chi2R<<
	"chi2RW="<<chi2RW<<
	"\n";
    }    
    if (reject) continue;

    
    // =========================================
    // wirte collected information to histograms
    // =========================================
        
    Float_t value = fCalPadClusterPerPad->GetCalROC(sector)->GetValue((sector<36)?irow:irow-64, TMath::Nint(cluster0->GetPad()));
    fCalPadClusterPerPad->GetCalROC(sector)->SetValue((sector<36)?irow:irow-64, ipad, value + 1 );
    
    
    TH3F * his3 = 0;
    his3 = (TH3F*)fRMSY->At(padSize);
    if (his3) his3->Fill(250 - TMath::Abs(cluster0->GetZ()), TMath::Abs(angley), TMath::Sqrt(cluster0->GetSigmaY2()) );
    his3 = (TH3F*)fRMSZ->At(padSize);
    if (his3) his3->Fill( 250 - TMath::Abs(cluster0->GetZ()), TMath::Abs(anglez), TMath::Sqrt(cluster0->GetSigmaZ2()) );
    
    his3 = (TH3F*)fArrayQRMSY->At(GetBin(cluster0->GetMax(), padSize));
    if (his3) his3->Fill( 250 - TMath::Abs(cluster0->GetZ()), TMath::Abs(angley), TMath::Sqrt(cluster0->GetSigmaY2()) );
    his3 = (TH3F*)fArrayQRMSZ->At(GetBin(cluster0->GetMax(), padSize));
    if (his3) his3->Fill( 250 - TMath::Abs(cluster0->GetZ()), TMath::Abs(anglez), TMath::Sqrt(cluster0->GetSigmaZ2()) );
    
    
    his3 = (TH3F*)fResolY->At(padSize);
    if (his3) his3->Fill( 250 - TMath::Abs(cluster0->GetZ()), TMath::Abs(angley), deltay );
    his3 = (TH3F*)fResolZ->At(padSize);
    if (his3) his3->Fill( 250 - TMath::Abs(cluster0->GetZ()), TMath::Abs(anglez), deltaz );
    his3 = (TH3F*)fArrayQDY->At(GetBin(cluster0->GetMax(), padSize));
    if (his3) his3->Fill( 250 - TMath::Abs(cluster0->GetZ()),TMath::Abs(angley), deltay );
    his3 = (TH3F*)fArrayQDZ->At(GetBin(cluster0->GetMax(), padSize));
    if (his3) his3->Fill( 250 - TMath::Abs(cluster0->GetZ()),TMath::Abs(anglez), deltaz );        
    //=============================================================================================    
    //
    // Fill THN histograms
    //
    Double_t xvar[9];
    xvar[1]=padSize;
    xvar[2]=cluster0->GetZ();
    xvar[3]=cluster0->GetMax();
    
    xvar[0]=deltay;
    xvar[4]=cluster0->GetPad()-Int_t(cluster0->GetPad());      
    xvar[5]=angley;
    fHisDeltaY->Fill(xvar);
    xvar[0]=TMath::Sqrt(cluster0->GetSigmaY2());
    fHisRMSY->Fill(xvar);
    
    xvar[0]=deltaz;
    xvar[4]=cluster0->GetTimeBin()-Int_t(cluster0->GetTimeBin());
    xvar[5]=anglez;
    fHisDeltaZ->Fill(xvar);
    xvar[0]=TMath::Sqrt(cluster0->GetSigmaZ2());
    fHisRMSZ->Fill(xvar);
    
  }    // loop over all padrows along the track: for (Int_t irow = 0; irow < 159; irow++)
}  // FillResolutionHistoLocal(...)








void  AliTPCcalibTracks::SetStyle() const {
   // 
   // set style, can be called by all draw functions
   // 
   gROOT->SetStyle("Plain");
   gStyle->SetFillColor(10);
   gStyle->SetPadColor(10);
   gStyle->SetCanvasColor(10);
   gStyle->SetStatColor(10);
   gStyle->SetPalette(1,0);
   gStyle->SetNumberContours(60);
}



void AliTPCcalibTracks::MakeReport(Int_t stat, const char* pathName){ 
   // 
   // all functions are called, that produce pictures
   // the histograms are written to the directory 'pathName'
   // 'stat' is a threshhold: only histograms with more than 'stat' entries are wirtten to file
   // 'stat' is also the number of minEntries for MakeResPlotsQTree
   // 

  if (GetDebugLevel() > 0) Info("MakeReport","Writing plots and trees to '%s'.", pathName);
  MakeResPlotsQTree(stat, pathName);
}
   



void AliTPCcalibTracks::MakeResPlotsQTree(Int_t minEntries, const char* pathName){
   //
   // Make tree with resolution parameters
   // the result is written to 'resol.root' in directory 'pathname'
   // file information are available in fileInfo
   // available variables in the tree 'Resol':
   //  Entries: number of entries for this resolution point
   //  nbins:   number of bins that were accumulated
   //  Dim:     direction, Dim==0: y-direction, Dim==1: z-direction
   //  Pad:     padSize; short, medium and long
   //  Length:  pad length, 0.75, 1, 1.5
   //  QMean:   mean charge of current charge bin and its neighbours, Qmean<0: integrated spectra
   //  Zc:      center of middle bin in drift direction
   //  Zm:      mean dirftlength for accumulated Delta-Histograms
   //  Zs:      width of driftlength bin
   //  AngleC:  center of middle bin in Angle-Direction
   //  AngleM:  mean angle for accumulated Delta-Histograms
   //  AngleS:  width of Angle-bin
   //  Resol:   sigma for gaus fit through Delta-Histograms
   //  Sigma:   error of sigma for gaus fit through Delta Histograms
   //  MeanR:   mean of the Delta-Histogram
   //  SigmaR:  rms of the Delta-Histogram
   //  RMSm:    mean of the gaus fit through RMS-Histogram
   //  RMS:     sigma of the gaus fit through RMS-Histogram
   //  RMSe0:   error of mean of gaus fit in RMS-Histogram
   //  RMSe1:   error of sigma of gaus fit in RMS-Histogram
   //  
      
  if (GetDebugLevel() > -1) cout << " ***** this is MakeResPlotsQTree *****" << endl;
  if (GetDebugLevel() > -1) cout << "    relax, the calculation will take a while..." << endl;
  
   gSystem->MakeDirectory(pathName);
   gSystem->ChangeDirectory(pathName);
   TString kFileName = "resol.root";
   TTreeSRedirector fTreeResol(kFileName.Data());
   
   TH3F *resArray[2][3][11];
   TH3F *rmsArray[2][3][11];
  
   // load histograms from fArrayQDY and fArrayQDZ 
   // into resArray and rmsArray
   // that is all we need here
   for (Int_t idim = 0; idim < 2; idim++){
      for (Int_t ipad = 0; ipad < 3; ipad++){
         for (Int_t iq = 0; iq <= 10; iq++){
            rmsArray[idim][ipad][iq]=0;
            resArray[idim][ipad][iq]=0;
            Int_t bin = GetBin(iq,ipad); 
            TH3F *hresl = 0;
            if (idim == 0) hresl = (TH3F*)fArrayQDY->At(bin);
            if (idim == 1) hresl = (TH3F*)fArrayQDZ->At(bin);
            if (!hresl) continue;
            resArray[idim][ipad][iq] = (TH3F*) hresl->Clone();
            resArray[idim][ipad][iq]->SetDirectory(0);
            TH3F * hreslRMS = 0;
            if (idim == 0) hreslRMS = (TH3F*)fArrayQRMSY->At(bin);
            if (idim == 1) hreslRMS = (TH3F*)fArrayQRMSZ->At(bin);
            if (!hreslRMS) continue;
            rmsArray[idim][ipad][iq] = (TH3F*) hreslRMS->Clone();
            rmsArray[idim][ipad][iq]->SetDirectory(0);
         }
      }
   }
   if (GetDebugLevel() > -1) cout << "Histograms loaded, starting to proces..." << endl;
   
   //--------------------------------------------------------------------------------------------
   
   char name[200];
   Double_t qMean;
   Double_t zMean, angleMean, zCenter, angleCenter;
   Double_t zSigma, angleSigma;
   TH1D *projectionRes = new TH1D("projectionRes", "projectionRes", 50, -1, 1);
   TH1D *projectionRms = new TH1D("projectionRms", "projectionRms", 50, -1, 1);
   TF1 *fitFunction = new TF1("fitFunction", "gaus");
   Float_t entriesQ = 0;
   Int_t loopCounter = 1;
  
   for (Int_t idim = 0; idim < 2; idim++){
      // Loop y-z corrdinate
      for (Int_t ipad = 0; ipad < 3; ipad++){
         // loop pad type
         for (Int_t iq = -1; iq < 10; iq++){
            // LOOP Q
	   if (GetDebugLevel() > -1) 
               cout << "Loop-counter, this is loop " << loopCounter << " of 66, (" 
                  << (Int_t)((loopCounter)/66.*100) << "% done), " 
                  << "idim = " << idim << ", ipad = " << ipad << ", iq = " << iq << "  \r" << std::flush;
            loopCounter++;
            TH3F *hres = 0;
            TH3F *hrms = 0;
            qMean    = 0;
            entriesQ = 0;
            
            // calculate qMean
            if (iq == -1){
               // integrated spectra
               for (Int_t iql = 0; iql < 10; iql++){    
                  Int_t bin = GetBin(iql,ipad); 
                  TH3F *hresl = resArray[idim][ipad][iql];
                  TH3F *hrmsl = rmsArray[idim][ipad][iql];
                  if (!hresl) continue;
                  if (!hrmsl) continue;	    
                  entriesQ += hresl->GetEntries();
                  qMean += hresl->GetEntries() * GetQ(bin);      
                  if (!hres) {
                     hres = (TH3F*)hresl->Clone();
                     hrms = (TH3F*)hrmsl->Clone();
                  }
                  else{
                     hres->Add(hresl);
                     hrms->Add(hrmsl);
                  }
               }
               qMean /= entriesQ;
               qMean *= -1.;  // integral mean charge
            }
            else {
               // loop over neighboured Q-bins 
               // accumulate entries from neighboured Q-bins
               for (Int_t iql = iq - 1; iql <= iq + 1; iql++){		    
                  if (iql < 0) continue;
                  Int_t bin = GetBin(iql,ipad);
                  TH3F * hresl = resArray[idim][ipad][iql];
                  TH3F * hrmsl = rmsArray[idim][ipad][iql];
                  if (!hresl) continue;
                  if (!hrmsl) continue;
                  entriesQ += hresl->GetEntries(); 
                  qMean += hresl->GetEntries() * GetQ(bin);      
                  if (!hres) {
                     hres = (TH3F*) hresl->Clone();
                     hrms = (TH3F*) hrmsl->Clone();
                  }
                  else{
                     hres->Add(hresl);
                     hrms->Add(hrmsl);
                  }
               }
               qMean/=entriesQ;
            }
	    if (!hres) continue;
	    if (!hrms) continue;

            TAxis *xAxisDriftLength = hres->GetXaxis();   // driftlength / z - axis
            TAxis *yAxisAngle       = hres->GetYaxis();   // angle axis
            TAxis *zAxisDelta       = hres->GetZaxis();   // delta axis
            TAxis *zAxisRms         = hrms->GetZaxis();   // rms axis
            
            // loop over all angle bins
            for (Int_t ibinyAngle = 1; ibinyAngle <= yAxisAngle->GetNbins(); ibinyAngle++) {
               angleCenter = yAxisAngle->GetBinCenter(ibinyAngle);
               // loop over all driftlength bins
               for (Int_t ibinxDL = 1; ibinxDL <= xAxisDriftLength->GetNbins(); ibinxDL++) {
                  zCenter    = xAxisDriftLength->GetBinCenter(ibinxDL);
                  zSigma     = xAxisDriftLength->GetBinWidth(ibinxDL);
                  angleSigma = yAxisAngle->GetBinWidth(ibinyAngle); 
                  zMean      = zCenter;      // changens, when more statistic is accumulated
                  angleMean  = angleCenter;  // changens, when more statistic is accumulated
                  
                  // create 2 1D-Histograms, projectionRes and projectionRms
                  // these histograms are delta histograms for given direction, padSize, chargeBin,
                  // angleBin and driftLengthBin
                  // later on they will be fitted with a gausian, its sigma is the resoltuion...
                  sprintf(name,"%s, zCenter: %f, angleCenter: %f", hres->GetName(), zCenter, angleCenter);
                  // TH1D * projectionRes = new TH1D(name, name, zAxisDelta->GetNbins(), zAxisDelta->GetXmin(), zAxisDelta->GetXmax());
                  projectionRes->SetNameTitle(name, name);
                  sprintf(name,"%s, zCenter: %f, angleCenter: %f", hrms->GetName(),zCenter,angleCenter);
                  // TH1D * projectionRms =  new TH1D(name, name, zAxisDelta->GetNbins(), zAxisRms->GetXmin(), zAxisRms->GetXmax());
                  projectionRms->SetNameTitle(name, name);
                  
                  projectionRes->Reset();
                  projectionRes->SetBins(zAxisDelta->GetNbins(), zAxisDelta->GetXmin(), zAxisDelta->GetXmax());
                  projectionRms->Reset();
                  projectionRms->SetBins(zAxisRms->GetNbins(), zAxisRms->GetXmin(), zAxisRms->GetXmax());
                  projectionRes->SetDirectory(0);
                  projectionRms->SetDirectory(0);
                  
                  Double_t entries = 0;
                  Int_t    nbins   = 0;   // counts, how many bins were accumulated
                  zMean     = 0;
                  angleMean = 0;
                  entries   = 0;
                  
                  // fill projectionRes and projectionRms for given dim, ipad and iq, 
                  // as well as for given angleBin and driftlengthBin
                  // if this gives not enough statistic, include neighbourhood 
                  // (angle and driftlength) successifely
                  for (Int_t dbin = 0; dbin <= 8; dbin++){              // delta-bins around centered angleBin and driftlengthBin
                     for (Int_t dbiny2 = -1; dbiny2 <= 1; dbiny2++) {   // delta-bins in angle direction
                        for (Int_t dbinx2 = -3; dbinx2 <= 3; dbinx2++){ // delta-bins in driftlength direction
                           if (TMath::Abs(dbinx2) + TMath::Abs(dbiny2) != dbin) continue;   // add each bin only one time !
                           Int_t binx2 = ibinxDL + dbinx2;                       // position variable in x (driftlength) direction
                           Int_t biny2 = ibinyAngle + dbiny2;                    // position variable in y (angle)  direction
                           if (binx2 < 1 || biny2 < 1) continue;                 // don't go out of the histogram!
                           if (binx2 >= xAxisDriftLength->GetNbins()) continue;  // don't go out of the histogram!
                           if (biny2 >= yAxisAngle->GetNbins()) continue;        // don't go out of the histogram!
                           nbins++;                                              // count the number of accumulated bins
                           // Fill resolution histo
                           for (Int_t ibin3 = 1; ibin3 < zAxisDelta->GetNbins(); ibin3++) {
                              // Int_t content = (Int_t)hres->GetBinContent(binx2, biny2, ibin3);     // unused variable
                              projectionRes->Fill(zAxisDelta->GetBinCenter(ibin3), hres->GetBinContent(binx2, biny2, ibin3));
                              entries   += hres->GetBinContent(binx2, biny2, ibin3);
                              zMean     += hres->GetBinContent(binx2, biny2, ibin3) * xAxisDriftLength->GetBinCenter(binx2);
                              angleMean += hres->GetBinContent(binx2, biny2, ibin3) * yAxisAngle->GetBinCenter(biny2);
                           }  // ibin3 loop
                           // fill RMS histo
                           for (Int_t ibin3 = 1; ibin3 < zAxisRms->GetNbins(); ibin3++) {
                              projectionRms->Fill(zAxisRms->GetBinCenter(ibin3), hrms->GetBinContent(binx2, biny2, ibin3));
                           }
                        }  //dbinx2 loop
                        if (entries > minEntries) break; // enough statistic accumulated
                     }  // dbiny2 loop
                     if (entries > minEntries) break;    // enough statistic accumulated
                  }  // dbin loop
                  if ( entries< minEntries) continue;  // when it was absolutly impossible to get enough statistic, don't write this point into the resolution tree  
                  zMean /= entries;
                  angleMean /= entries;
                  
                  if (entries > minEntries) {
                     //  when enough statistic is accumulated
                     //  fit Delta histograms with a gausian
                     //  of the gausian is the resolution (resol), its fit error is sigma
                     //  store also mean and RMS of the histogram
                     Float_t xmin     = projectionRes->GetMean() - 2. * projectionRes->GetRMS() - 0.2;
                     Float_t xmax     = projectionRes->GetMean() + 2. * projectionRes->GetRMS() + 0.2;
                     
//                      projectionRes->Fit("gaus", "q0", "", xmin, xmax);
//                      Float_t resol    = projectionRes->GetFunction("gaus")->GetParameter(2);
//                      Float_t sigma    = projectionRes->GetFunction("gaus")->GetParError(2);
                     fitFunction->SetMaximum(xmax);
                     fitFunction->SetMinimum(xmin);
                     projectionRes->Fit("fitFunction", "qN0", "", xmin, xmax);
                     Float_t resol    = fitFunction->GetParameter(2);
                     Float_t sigma    = fitFunction->GetParError(2);
                     
                     Float_t meanR    = projectionRes->GetMean();
                     Float_t sigmaR   = projectionRes->GetRMS();
                     // fit also RMS histograms with a gausian
                     // store mean and sigma of the gausian in rmsMean and rmsSigma
                     // store also the fit errors in errorRMS and errorSigma
                     xmin = projectionRms->GetMean() - 2. * projectionRes->GetRMS() - 0.2;
                     xmax = projectionRms->GetMean() + 2. * projectionRes->GetRMS() + 0.2;
                     
//                      projectionRms->Fit("gaus","q0","",xmin,xmax);
//                      Float_t rmsMean    = projectionRms->GetFunction("gaus")->GetParameter(1);
//                      Float_t rmsSigma   = projectionRms->GetFunction("gaus")->GetParameter(2);
//                      Float_t errorRMS   = projectionRms->GetFunction("gaus")->GetParError(1);
//                      Float_t errorSigma = projectionRms->GetFunction("gaus")->GetParError(2);
                     projectionRms->Fit("fitFunction", "qN0", "", xmin, xmax);
                     Float_t rmsMean    = fitFunction->GetParameter(1);
                     Float_t rmsSigma   = fitFunction->GetParameter(2);
                     Float_t errorRMS   = fitFunction->GetParError(1);
                     Float_t errorSigma = fitFunction->GetParError(2);
                    
                     Float_t length = 0.75;
                     if (ipad == 1) length = 1;
                     if (ipad == 2) length = 1.5;
                     
                     fTreeResol<<"Resol"<<
                        "Entries="<<entries<<      // number of entries for this resolution point
                        "nbins="<<nbins<<          // number of bins that were accumulated
                        "Dim="<<idim<<             // direction, Dim==0: y-direction, Dim==1: z-direction
                        "Pad="<<ipad<<             // padSize; short, medium and long
                        "Length="<<length<<        // pad length, 0.75, 1, 1.5
                        "QMean="<<qMean<<          // mean charge of current charge bin and its neighbours, Qmean<0: integrated spectra
                        "Zc="<<zCenter<<           // center of middle bin in drift direction
                        "Zm="<<zMean<<             // mean dirftlength for accumulated Delta-Histograms
                        "Zs="<<zSigma<<            // width of driftlength bin
                        "AngleC="<<angleCenter<<   // center of middle bin in Angle-Direction
                        "AngleM="<<angleMean<<     // mean angle for accumulated Delta-Histograms
                        "AngleS="<<angleSigma<<    // width of Angle-bin
                        "Resol="<<resol<<          // sigma for gaus fit through Delta-Histograms
                        "Sigma="<<sigma<<          // error of sigma for gaus fit through Delta Histograms
                        "MeanR="<<meanR<<          // mean of the Delta-Histogram
                        "SigmaR="<<sigmaR<<        // rms of the Delta-Histogram
                        "RMSm="<<rmsMean<<         // mean of the gaus fit through RMS-Histogram
                        "RMSs="<<rmsSigma<<        // sigma of the gaus fit through RMS-Histogram
                        "RMSe0="<<errorRMS<<       // error of mean of gaus fit in RMS-Histogram
                        "RMSe1="<<errorSigma<<     // error of sigma of gaus fit in RMS-Histogram
                        "\n";
                     if (GetDebugLevel() > 5) {
                        projectionRes->SetDirectory(fTreeResol.GetFile());
                        projectionRes->Write(projectionRes->GetName());
                        projectionRes->SetDirectory(0);
                        projectionRms->SetDirectory(fTreeResol.GetFile());
                        projectionRms->Write(projectionRms->GetName());
                        projectionRes->SetDirectory(0);
                     }
                  }  // if (projectionRes->GetSum() > minEntries)
               }  // for (Int_t ibinxDL = 1; ibinxDL <= xAxisDriftLength->GetNbins(); ibinxDL++)
            }  // for (Int_t ibinyAngle = 1; ibinyAngle <= yAxisAngle->GetNbins(); ibinyAngle++)
            
         }  // iq-loop
      }  // ipad-loop
   }  // idim-loop
   delete projectionRes;
   delete projectionRms;
   
//    TFile resolFile(fTreeResol.GetFile());
   TObjString fileInfo(Form("Resolution tree, minEntries = %i", minEntries));
   fileInfo.Write("fileInfo");
//    resolFile.Close();
//    fTreeResol.GetFile()->Close();
   if (GetDebugLevel() > -1) cout << endl;
   if (GetDebugLevel() > -1) cout << "MakeResPlotsQTree done, results are in '"<< kFileName.Data() <<"'." << endl;
   gSystem->ChangeDirectory("..");
}





Long64_t AliTPCcalibTracks::Merge(TCollection *collectionList) {
   // 
   // function to merge several AliTPCcalibTracks objects after PROOF calculation
   // The object's histograms are merged via their merge functions
   // Be carefull: histograms are linked to a file, switch this off by TH1::AddDirectory(kFALSE) !!!
   // 
   
  if (GetDebugLevel() > 0) cout << " *****  this is AliTPCcalibTracks::Merge(TCollection *collectionList)  *****"<< endl;  
   if (!collectionList) return 0;
   if (collectionList->IsEmpty()) return -1;
   
   if (GetDebugLevel() > 1) cout << "the collectionList contains " << collectionList->GetEntries() << " entries." << endl;     //    REMOVE THIS LINE!!!!!!!!!!!!!!!!!1
   if (GetDebugLevel() > 5) cout << " the list in the merge-function looks as follows: " << endl;
   collectionList->Print();
   
   // create a list for each data member
   TList* deltaYList = new TList;
   TList* deltaZList = new TList;
   TList* arrayAmpRowList = new TList;
   TList* rejectedTracksList = new TList;
   TList* clusterCutHistoList = new TList;
   TList* arrayAmpList = new TList;
   TList* arrayQDYList = new TList;
   TList* arrayQDZList = new TList;
   TList* arrayQRMSYList = new TList;
   TList* arrayQRMSZList = new TList;
   TList* resolYList = new TList;
   TList* resolZList = new TList;
   TList* rMSYList = new TList;
   TList* rMSZList = new TList;
   
//    TList* nRowsList = new TList;
//    TList* nSectList = new TList;
//    TList* fileNoList = new TList;
   
   TIterator *listIterator = collectionList->MakeIterator();
   AliTPCcalibTracks *calibTracks = 0;
   if (GetDebugLevel() > 1) cout << "start to iterate, filling lists" << endl;    
   Int_t counter = 0;
   while ( (calibTracks = dynamic_cast<AliTPCcalibTracks*> (listIterator->Next())) ){
      // loop over all entries in the collectionList and get dataMembers into lists
      
      arrayQDYList->Add(calibTracks->GetfArrayQDY());
      arrayQDZList->Add(calibTracks->GetfArrayQDZ());
      arrayQRMSYList->Add(calibTracks->GetfArrayQRMSY());
      arrayQRMSZList->Add(calibTracks->GetfArrayQRMSZ());
      resolYList->Add(calibTracks->GetfResolY());
      resolZList->Add(calibTracks->GetfResolZ());
      rMSYList->Add(calibTracks->GetfRMSY());
      rMSZList->Add(calibTracks->GetfRMSZ());
      rejectedTracksList->Add(calibTracks->GetfRejectedTracksHisto());
      clusterCutHistoList->Add(calibTracks->GetfClusterCutHisto());
      //
      if (fCalPadClusterPerPad && calibTracks->GetfCalPadClusterPerPad())
	fCalPadClusterPerPad->Add(calibTracks->GetfCalPadClusterPerPad());      
      //      fCalPadClusterPerPadRaw->Add(calibTracks->GetfCalPadClusterPerPadRaw());
      counter++;
      if (GetDebugLevel() > 5) cout << "filling lists, object " << counter << " added." << endl;
      AddHistos(calibTracks);
   }
   
   
   // merge data members
   if (GetDebugLevel() > 0) cout << "histogram's merge-functins are called... " << endl; 
   fClusterCutHisto->Merge(clusterCutHistoList);
   fRejectedTracksHisto->Merge(rejectedTracksList);
   
   TObjArray* objarray = 0;
   TH1* hist = 0;
   TList* histList = 0;
   TIterator *objListIterator = 0;
   
      
   if (GetDebugLevel() > 0) cout << "merging fArrayQDY..." << endl;
   // merge fArrayQDY
   for (Int_t i = 0; i < fArrayQDY->GetEntriesFast(); i++) { // loop over data member, i < 300
      objListIterator = arrayQDYList->MakeIterator();
      histList = new TList;
      while (( objarray =  (TObjArray*)objListIterator->Next() )) { 
         // loop over arrayQDYList, get TObjArray, get object at position i, cast it into TH3F
         hist = (TH3F*)(objarray->At(i));
         histList->Add(hist);
      }
      ((TH3F*)(fArrayQDY->At(i)))->Merge(histList);
      delete histList;
      delete objListIterator;
   }

   if (GetDebugLevel() > 0) cout << "merging fArrayQDZ..." << endl;
   // merge fArrayQDZ
   for (Int_t i = 0; i < fArrayQDZ->GetEntriesFast(); i++) { // loop over data member, i < 300
      objListIterator = arrayQDZList->MakeIterator();
      histList = new TList;
      while (( objarray =  (TObjArray*)objListIterator->Next() )) { 
         // loop over arrayQDZList, get TObjArray, get object at position i, cast it into TH3F
         if (!objarray) continue;
         hist = (TH3F*)(objarray->At(i));
         histList->Add(hist);
      }
      ((TH3F*)(fArrayQDZ->At(i)))->Merge(histList);
      delete histList;
      delete objListIterator;
   }

   if (GetDebugLevel() > 0) cout << "merging fArrayQRMSY..." << endl;
   // merge fArrayQRMSY
   for (Int_t i = 0; i < fArrayQRMSY->GetEntriesFast(); i++) { // loop over data member, i < 300
      objListIterator = arrayQRMSYList->MakeIterator();
      histList = new TList;
      while (( objarray =  (TObjArray*)objListIterator->Next() )) { 
         // loop over arrayQDZList, get TObjArray, get object at position i, cast it into TH3F
         if (!objarray) continue;
         hist = (TH3F*)(objarray->At(i));
         histList->Add(hist);
      }
      ((TH3F*)(fArrayQRMSY->At(i)))->Merge(histList);
      delete histList;
      delete objListIterator;
   }   

   if (GetDebugLevel() > 0) cout << "merging fArrayQRMSZ..." << endl;
   // merge fArrayQRMSZ
   for (Int_t i = 0; i < fArrayQRMSZ->GetEntriesFast(); i++) { // loop over data member, i < 300
      objListIterator = arrayQRMSZList->MakeIterator();
      histList = new TList;
      while (( objarray =  (TObjArray*)objListIterator->Next() )) { 
         // loop over arrayQDZList, get TObjArray, get object at position i, cast it into TH3F
         if (!objarray) continue;
         hist = (TH3F*)(objarray->At(i));
         histList->Add(hist);
      }
      ((TH3F*)(fArrayQRMSZ->At(i)))->Merge(histList);
      delete histList;
      delete objListIterator;
   } 
   
   
  
   
        
   
   if (GetDebugLevel() > 0) cout << "starting to merge the rest: fResolY, fResolZ , fRMSY, fRMSZ..." << endl;
   // merge fResolY
   for (Int_t i = 0; i < fResolY->GetEntriesFast(); i++) { // loop over data member, i < 3
      objListIterator = resolYList->MakeIterator();
      histList = new TList;
      while (( objarray =  (TObjArray*)objListIterator->Next() )) { 
         // loop over arrayQDZList, get TObjArray, get object at position i, cast it into TH3F
         if (!objarray) continue;
         hist = (TH3F*)(objarray->At(i));
         histList->Add(hist);
      }
      ((TH3F*)(fResolY->At(i)))->Merge(histList);
      delete histList;
      delete objListIterator;
   }
   
   // merge fResolZ
   for (Int_t i = 0; i < fResolZ->GetEntriesFast(); i++) { // loop over data member, i < 3
      objListIterator = resolZList->MakeIterator();
      histList = new TList;
      while (( objarray =  (TObjArray*)objListIterator->Next() )) { 
         // loop over arrayQDZList, get TObjArray, get object at position i, cast it into TH3F
         if (!objarray) continue;
         hist = (TH3F*)(objarray->At(i));
         histList->Add(hist);
      }
      ((TH3F*)(fResolZ->At(i)))->Merge(histList);
      delete histList;
      delete objListIterator;
   }

   // merge fRMSY
   for (Int_t i = 0; i < fRMSY->GetEntriesFast(); i++) { // loop over data member, i < 3
      objListIterator = rMSYList->MakeIterator();
      histList = new TList;
      while (( objarray =  (TObjArray*)objListIterator->Next() )) { 
         // loop over arrayQDZList, get TObjArray, get object at position i, cast it into TH3F
         if (!objarray) continue;
         hist = (TH3F*)(objarray->At(i));
         histList->Add(hist);
      }
      ((TH3F*)(fRMSY->At(i)))->Merge(histList);
      delete histList;
      delete objListIterator;
   }
         
   // merge fRMSZ
   for (Int_t i = 0; i < fRMSZ->GetEntriesFast(); i++) { // loop over data member, i < 3
      objListIterator = rMSZList->MakeIterator();
      histList = new TList;
      while (( objarray =  (TObjArray*)objListIterator->Next() )) { 
         // loop over arrayQDZList, get TObjArray, get object at position i, cast it into TH3F
         if (!objarray) continue;
         hist = (TH3F*)(objarray->At(i));
         histList->Add(hist);
      }
      ((TH3F*)(fRMSZ->At(i)))->Merge(histList);
      delete histList;
      delete objListIterator;
   }
   
   delete deltaYList;
   delete deltaZList;
   delete arrayAmpRowList;
   delete arrayAmpList;
   delete arrayQDYList;
   delete arrayQDZList;
   delete arrayQRMSYList;
   delete arrayQRMSZList;
   delete resolYList;
   delete resolZList;
   delete rMSYList;
   delete rMSZList;
   delete listIterator;
   
   if (GetDebugLevel() > 0) cout << "merging done!" << endl;
   
   return 1;
}




void AliTPCcalibTracks::MakeHistos(){
  //
  ////make THnSparse
  //
  //THnSparse  *fHisDeltaY;    // THnSparse - delta Y 
  //THnSparse  *fHisDeltaZ;    // THnSparse - delta Z 
  //THnSparse  *fHisRMSY;      // THnSparse - rms Y 
  //THnSparse  *fHisRMSZ;      // THnSparse - rms Z 
  //THnSparse  *fHisQmax;      // THnSparse - qmax 
  //THnSparse  *fHisQtot;      // THnSparse - qtot 
  // cluster  performance bins
  // 0 - variable of interest
  // 1 - pad type   - 0- short 1-medium 2-long pads
  // 2 - drift length - drift length -0-1
  // 3 - Qmax         - Qmax  - 2- 400
  // 4 - cog          - COG position - 0-1
  // 5 - tan(phi)     - local y angle
  // 6 - tan(theta)   - local z angle
  // 7 - sector       - sector number
  Double_t xminTrack[8], xmaxTrack[8];
  Int_t binsTrack[8];
  TString axisName[8];
  
  //
  binsTrack[0]=200;
  axisName[0]  ="var";

  binsTrack[1] =3;
  xminTrack[1] =0; xmaxTrack[1]=3;
  axisName[1]  ="pad type";
  //
  binsTrack[2] =20;
  xminTrack[2] =-250; xmaxTrack[2]=250;
  axisName[2]  ="z";
  //
  binsTrack[3] =10;
  xminTrack[3] =1; xmaxTrack[3]=400;
  axisName[3]  ="Qmax";
  //
  binsTrack[4] =20;
  xminTrack[4] =0; xmaxTrack[4]=1;
  axisName[4]  ="cog";
  //
  binsTrack[5] =15;
  xminTrack[5] =-1.5; xmaxTrack[5]=1.5;
  axisName[5]  ="tan(angle)";
  //
  //
  xminTrack[0] =-1.5; xmaxTrack[0]=1.5;
  fHisDeltaY=new THnSparseF("#Delta_{y} (cm)","#Delta_{y} (cm)", 6, binsTrack,xminTrack, xmaxTrack);
  xminTrack[0] =-1.5; xmaxTrack[0]=1.5;
  fHisDeltaZ=new THnSparseF("#Delta_{z} (cm)","#Delta_{z} (cm)", 6, binsTrack,xminTrack, xmaxTrack);
  xminTrack[0] =0.; xmaxTrack[0]=0.5;
  fHisRMSY=new THnSparseF("#RMS_{y} (cm)","#RMS_{y} (cm)", 6, binsTrack,xminTrack, xmaxTrack);
  xminTrack[0] =0.; xmaxTrack[0]=0.5;
  fHisRMSZ=new THnSparseF("#RMS_{z} (cm)","#RMS_{z} (cm)", 6, binsTrack,xminTrack, xmaxTrack);
  xminTrack[0] =0.; xmaxTrack[0]=100;
  fHisQmax=new THnSparseF("Qmax (ADC)","Qmax (ADC)", 6, binsTrack,xminTrack, xmaxTrack);

  xminTrack[0] =0.; xmaxTrack[0]=250;
  fHisQtot=new THnSparseF("Qtot (ADC)","Qtot (ADC)", 6, binsTrack,xminTrack, xmaxTrack);


  for (Int_t ivar=0;ivar<6;ivar++){
    fHisDeltaY->GetAxis(ivar)->SetName(axisName[ivar].Data());
    fHisDeltaZ->GetAxis(ivar)->SetName(axisName[ivar].Data());
    fHisRMSY->GetAxis(ivar)->SetName(axisName[ivar].Data());
    fHisRMSZ->GetAxis(ivar)->SetName(axisName[ivar].Data());
    fHisQmax->GetAxis(ivar)->SetName(axisName[ivar].Data());
    fHisQtot->GetAxis(ivar)->SetName(axisName[ivar].Data());
    fHisDeltaY->GetAxis(ivar)->SetTitle(axisName[ivar].Data());
    fHisDeltaZ->GetAxis(ivar)->SetName(axisName[ivar].Data());
    fHisRMSY->GetAxis(ivar)->SetName(axisName[ivar].Data());
    fHisRMSZ->GetAxis(ivar)->SetName(axisName[ivar].Data());
    fHisQmax->GetAxis(ivar)->SetName(axisName[ivar].Data());
    fHisQtot->GetAxis(ivar)->SetName(axisName[ivar].Data());
  }


  BinLogX(fHisDeltaY,3);
  BinLogX(fHisDeltaZ,3);
  BinLogX(fHisRMSY,3);
  BinLogX(fHisRMSZ,3);
  BinLogX(fHisQmax,3);
  BinLogX(fHisQtot,3);

}  

void    AliTPCcalibTracks::AddHistos(AliTPCcalibTracks* calib){
  //
  // Add histograms
  //
  if (calib->fHisDeltaY) fHisDeltaY->Add(calib->fHisDeltaY);
  if (calib->fHisDeltaZ) fHisDeltaZ->Add(calib->fHisDeltaZ);
  if (calib->fHisRMSY)   fHisRMSY->Add(calib->fHisRMSY);
  if (calib->fHisRMSZ)   fHisRMSZ->Add(calib->fHisRMSZ);
}



void AliTPCcalibTracks::MakeSummaryTree(THnSparse *hisInput, TTreeSRedirector *pcstream, Int_t ptype){
  //
  // Dump summary info
  //
  //  0.OBJ: TAxis     var
  //  1.OBJ: TAxis     pad type
  //  2.OBJ: TAxis     z
  //  3.OBJ: TAxis     Qmax
  //  4.OBJ: TAxis     cog
  //  5.OBJ: TAxis     tan(angle)
  //
  if (ptype>3) return;
  Int_t idim[6]={0,1,2,3,4,5};
  TString hname[4]={"dy","dz","rmsy","rmsz"};
  //
  Int_t nbins5=hisInput->GetAxis(5)->GetNbins();
  Int_t first5=hisInput->GetAxis(5)->GetFirst();
  Int_t last5 =hisInput->GetAxis(5)->GetLast();
  //
  for (Int_t ibin5=first5-1; ibin5<=last5; ibin5+=1){   // axis 5 - local angle
    Bool_t bin5All=kTRUE;
    if (ibin5>=first5){
      hisInput->GetAxis(5)->SetRange(TMath::Max(ibin5-1,first5),TMath::Min(ibin5+1,nbins5));
      if (ibin5==first5) hisInput->GetAxis(5)->SetRange(TMath::Max(ibin5,first5),TMath::Min(ibin5,nbins5));
      bin5All=kFALSE;
    }
    Double_t      x5= hisInput->GetAxis(5)->GetBinCenter(ibin5);
    THnSparse * his5= hisInput->Projection(5,idim);         //projected histogram according selection 5    
    Int_t nbins4=his5->GetAxis(4)->GetNbins();
    Int_t first4=his5->GetAxis(4)->GetFirst();
    Int_t last4 =his5->GetAxis(4)->GetLast();
    
    for (Int_t ibin4=first4-1; ibin4<=last4; ibin4+=1){   // axis 4 - cog
      Bool_t bin4All=kTRUE;
      if (ibin4>=first4){
	his5->GetAxis(4)->SetRange(TMath::Max(ibin4+1,first4),TMath::Min(ibin4-1,nbins4));
	if (ibin4==first4||ibin4==last4) his5->GetAxis(4)->SetRange(TMath::Max(ibin4,first4),TMath::Min(ibin4,nbins4));
	bin4All=kFALSE;
      }
      Double_t      x4= his5->GetAxis(4)->GetBinCenter(ibin4);
      THnSparse * his4= his5->Projection(4,idim);         //projected histogram according selection 4
      //
      Int_t nbins3=his4->GetAxis(3)->GetNbins();
      Int_t first3=his4->GetAxis(3)->GetFirst();
      Int_t last3 =his4->GetAxis(3)->GetLast();
      //
      for (Int_t ibin3=first3-1; ibin3<=last3; ibin3+=1){   // axis 3 - Qmax
	Bool_t bin3All=kTRUE;
	if (ibin3>=first3){
	  his4->GetAxis(3)->SetRange(TMath::Max(ibin3,first3),TMath::Min(ibin3,nbins3));
	  bin3All=kFALSE;
	}
	Double_t      x3= his4->GetAxis(3)->GetBinCenter(ibin3);
	THnSparse * his3= his4->Projection(3,idim);         //projected histogram according selection 3
	//
	Int_t nbins2    = his3->GetAxis(2)->GetNbins();
	Int_t first2    = his3->GetAxis(2)->GetFirst();
	Int_t last2     = his3->GetAxis(2)->GetLast();
	//
	for (Int_t ibin2=first2-1; ibin2<=last2; ibin2+=1){   // axis 2 - z	
	  Bool_t bin2All=kTRUE;
	  Double_t      x2= his3->GetAxis(2)->GetBinCenter(ibin2);
	  if (ibin2>=first2){
	    his3->GetAxis(2)->SetRange(TMath::Max(ibin2-1,first2),TMath::Min(ibin2+1,nbins2));
	    if (ibin2==first2||ibin2==last2||TMath::Abs(x2)<20) his3->GetAxis(2)->SetRange(TMath::Max(ibin2,first2),TMath::Min(ibin2,nbins2));
	    bin2All=kFALSE;
	  }
	  THnSparse * his2= his3->Projection(2,idim);         //projected histogram according selection 2
	  //
	  Int_t nbins1     = his2->GetAxis(1)->GetNbins();
	  Int_t first1     = his2->GetAxis(1)->GetFirst();
	  Int_t last1      = his2->GetAxis(1)->GetLast();
	  for (Int_t ibin1=first1-1; ibin1<=last1; ibin1++){   //axis 1 - pad type 
	    Bool_t bin1All=kTRUE;
	    if (ibin1>=first1){
	      his2->GetAxis(1)->SetRange(TMath::Max(ibin1,1),TMath::Min(ibin1,nbins1)); 	  
	      bin1All=kFALSE;
	    }
	    Double_t       x1= TMath::Nint(his2->GetAxis(1)->GetBinCenter(ibin1)-0.5);
	    TH1 * hisDelta = his2->Projection(0);
	    Double_t entries = hisDelta->GetEntries();
	    Double_t mean=0, rms=0;
	    if (entries>10){
	      mean    = hisDelta->GetMean();
	      rms = hisDelta->GetRMS();	  
	      hisDelta->GetXaxis()->SetRangeUser(mean-4*rms,mean+4*rms);
	      mean    = hisDelta->GetMean();
	      rms = hisDelta->GetRMS();
	    }
	    //
	    (*pcstream)<<hname[ptype].Data()<<
	      // flag - intgrated values for given bin
	      "angleA="<<bin5All<<
	      "cogA="<<bin4All<<
	      "qmaxA="<<bin3All<<
	      "zA="<<bin2All<<
	      "ipadA="<<bin1All<<
	      // center of bin value
	      "angle="<<x5<<
	      "cog="<<x4<<
	      "qmax="<<x3<<
	      "z="<<x2<<
	      "ipad="<<x1<<
	      // mean values
	      //
	      "entries="<<entries<<
	      "mean="<<mean<<
	      "rms="<<rms<<
	      "ptype="<<ptype<<   //
	      "\n";
	    delete hisDelta;
	    printf("%f\t%f\t%f\t%f\t%f\t%f\t%f\n",x5,x4,x3,x2,x1, entries,mean);	  
	  }//loop z
	  delete his2;
	} // loop Q max
	delete his3;
      } // loop COG
      delete his4;
    }//loop local angle
    delete his5;
  }
}

#ifndef ALITRDTRACK_H
#define ALITRDTRACK_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  Represents a reconstructed TRD track                                     //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "AliKalmanTrack.h"
#include "AliTRDtracklet.h"

class AliESDtrack;
class AliTrackReference;

const unsigned kMAXCLUSTERSPERTRACK = 210; 

class AliTRDtrack : public AliKalmanTrack {
   
  enum { kNdet      = 540
       , kNstacks   =  90
       , kNplane    =   6
       , kNcham     =   5
       , kNsect     =  18
       , kNslice    =   3
       , kNtimeBins =  22 };

  friend class AliTRDtracker;

 public:

   AliTRDtrack();
   AliTRDtrack(const AliTRDcluster *c, Int_t index, const Double_t xx[5],
               const Double_t cc[15], Double_t xr, Double_t alpha);  
   AliTRDtrack(const AliTRDtrack& t);    
   AliTRDtrack(const AliESDtrack& t);
   ~AliTRDtrack();

   AliTRDtrack(const AliKalmanTrack& t, Double_t alpha); 

   Int_t           Compare(const TObject *o) const;

   void SetdEdx(Double_t dedx) {fdEdx=dedx;}
   Double_t GetdEdx()  const {return fdEdx;}
   Double_t GetPIDsignal()  const {return GetdEdx();}

   Int_t GetClusterIndex(Int_t i) const {return fIndex[i];}

   Double_t GetC() const {return AliExternalTrackParam::GetC(GetBz());}

   Double_t GetPredictedChi2(const AliTRDcluster* c, Double_t h01) const;
   Bool_t PropagateTo(Double_t xr, Double_t x0=8.72, Double_t rho=5.86e-3);
   Bool_t Update(const AliTRDcluster* c, Double_t chi2, Int_t i, Double_t h01);
   Bool_t Rotate(Double_t angle, Bool_t absolute=kFALSE);

   void ResetClusters() { SetChi2(0.); SetNumberOfClusters(0); }

   void            CookdEdx(Double_t low=0.05, Double_t up=0.7);   
   Float_t         StatusForTOF();
 
   Int_t           GetSector() const;

   Float_t         GetClusterdQdl(Int_t i) const { return fdQdl[i]; }    

   Float_t GetPIDsignals(Int_t iPlane, Int_t iSlice) const 
           { return fdEdxPlane[iPlane][iSlice]; }
   Int_t  GetPIDTimBin(Int_t i) const { return fTimBinPlane[i];}

   Double_t GetLikelihoodElectron() const { return fLhElectron;    }

 
   Int_t   GetSeedLabel() const  { return fSeedLab; }

   Int_t *GetBackupIndexes() { return fIndexBackup;   }
   Int_t *GetIndexes()       { return fIndex;         }

   Int_t           GetProlongation(Double_t xk, Double_t &y, Double_t &z);

   void SetStop(Bool_t stop) { fStopped = stop; }
   Bool_t GetStop() const    { return fStopped; }

   Int_t           PropagateToX(Double_t xr, Double_t step);
   Int_t           PropagateToR(Double_t xr, Double_t step);

   void SetPIDsignals(Float_t dedx, Int_t iPlane, Int_t iSlice) 
                      { fdEdxPlane[iPlane][iSlice] = dedx; }
   void SetPIDTimBin(Int_t timbin, Int_t i)  { fTimBinPlane[i] = timbin; }
   void SetLikelihoodElectron(Float_t l)     { fLhElectron     = l;      }

   void SetSampledEdx(Float_t q, Int_t i);
   void SetSampledEdx(Float_t q);
   void SetSeedLabel(Int_t lab) { fSeedLab = lab; }

   Int_t UpdateMI(const AliTRDcluster* c, Double_t chi2, Int_t i, Double_t h01, Int_t plane);
  //Int_t  UpdateMI(const AliTRDtracklet & tracklet);

   void AddNWrong() { fNWrong++; }
  
   Int_t GetNWrong() const  { return fNWrong;  }
   Int_t GetNRotate() const { return fNRotate; }
   Int_t GetNCross() const  { return fNCross;  }
   void  IncCross() { fNCross++; if (fBackupTrack) fBackupTrack->IncCross(); }
   AliTRDtrack *GetBackupTrack() { return fBackupTrack; }
   void         MakeBackupTrack();

 protected:
   Double_t GetBz() const;
   Bool_t Update(const AliCluster */*c*/, Double_t /*chi2*/, Int_t /*idx*/) {
     return 0;
   }
   Double_t GetPredictedChi2(const AliCluster */*c*/) const {return 0.;}

   AliTRDtrack     &operator=(const AliTRDtrack &t);

   Int_t           fSeedLab;                               // track label taken from seeding  
   Float_t         fdEdx;                                  // dE/dx 
   Float_t         fdEdxT;                                 // dE/dx  - truncated mean
   Float_t         fDE;                                    // integrated delta energy
   Float_t         fdEdxPlane[kNplane][kNslice];           // dE/dx from all 6 planes in 3 slices each
   Int_t           fTimBinPlane[kNplane];                  // time bin of Max cluster from all 6 planes

   Bool_t          fStopped;                               // track stop indication
   Int_t           fIndex[kMAXCLUSTERSPERTRACK];           // global indexes of clusters  
   Int_t           fIndexBackup[kMAXCLUSTERSPERTRACK];     // backup indexes of clusters - used in iterations
   Float_t         fdQdl[kMAXCLUSTERSPERTRACK];            // cluster amplitudes corrected for track angles    
                           
   Float_t         fLhElectron;                            // Likelihood to be an electron    
   Int_t           fNWrong;                                // number of wrong clusters
   Int_t           fNRotate;                               // number of rotation
   Int_t           fNCross;                                // number of the cross materials
   Int_t           fNExpected;                             // expected number of cluster
   Int_t           fNLast;                                 // number of clusters in last 2 layers
   Int_t           fNExpectedLast;                         // number of expected clusters on last 2 layers
   Int_t           fNdedx;                                 // number of clusters for dEdx measurment
   Float_t         fChi2Last;                              // chi2 in the  last 2 layers
   AliTRDtracklet  fTracklets[6];                          // tracklets
   Float_t         fBudget[3];                             // integrated material budget
   AliTRDtrack    *fBackupTrack;                           //! backup track

   ClassDef(AliTRDtrack,7)                                 // TRD reconstructed tracks

};                     

#endif   

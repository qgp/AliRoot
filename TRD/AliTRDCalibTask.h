#ifndef ALITRDCALIBTASK_H
#define ALITRDCALIBTASK_H

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  TRD calibration task for offline calibration                             //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


class TList;
class TObject;
class TObjArray;
class TH2F;
class TH1F;
class TH1I;
class TProfile2D;
class TH2I;
class TTree;
class AliESDEvent;
class AliESDfriend;
class AliESDtrack;
class AliESDfriendTrack;
class AliTRDtrackV1;
class AliTRDCalibraFillHisto;
class AliTRDcluster;
class AliESDtrackCuts;
class AliTRDCalDet;

#include "TObjString.h"
#include "AliAnalysisTask.h" 

class AliTRDCalibTask : public AliAnalysisTask {
 public:
  AliTRDCalibTask(const char *name = "AliTRDCalibTask");
  virtual ~AliTRDCalibTask();
  
  virtual void   ConnectInputData(Option_t *);
  virtual void   CreateOutputObjects();
  virtual void   Exec(Option_t *option);
  virtual void   Terminate(Option_t *);
  virtual Bool_t Load(const Char_t *filename);
  void           Plot();

  void SetHisto2d(Bool_t histo2d)                                   {fHisto2d=histo2d;};
  void SetVector2d(Bool_t vector2d)                                 {fVector2d=vector2d;};
  void SetVdriftLinear(Bool_t vdriftLinear)                         {fVdriftLinear = vdriftLinear;};
  void SetNbTimeBins(Int_t nbTimeBins)                              {fNbTimeBins=nbTimeBins;};  
  
  void SetNz(Short_t nz, Int_t i)                                      {fNz[i]=nz;};
  void SetNrphi(Short_t nrphi, Int_t i)                                {fNrphi[i]=nrphi;};
  
  void AddSelectedTriggerClass(const char*name)                        {fSelectedTrigger->Add(new TObjString(name));};
  void SetReject(Bool_t rejected)                                      {fRejected = rejected;};
  
  void SetESDtrackCuts(AliESDtrackCuts * const esdtrackCuts)                  {fEsdTrackCuts = esdtrackCuts;};
  void SetRequirePrimaryVertex(Bool_t requirePrimaryVertex)            {fRequirePrimaryVertex = requirePrimaryVertex;};
  void SetMinNbOfContributors(Int_t minNbOfContributors)               {fMinNbContributors = minNbOfContributors;};  
  
  void SetLow(Int_t low)                                               {fLow=low;};
  void SetHigh(Int_t high)                                             {fHigh=high;};
  void SetFillZero(Bool_t fillZero)                                    {fFillZero =  fillZero;};
  void SetNormalizeNbOfCluster(Bool_t normalizeNbOfCluster =  kTRUE)   {fNormalizeNbOfCluster = normalizeNbOfCluster;};
  void SetMaxCluster(Float_t maxCluster)                               {fMaxCluster =  maxCluster; }; 
  void SetNbMaxCluster(Short_t nbMaxCluster)                           {fNbMaxCluster =  nbMaxCluster; }; 
  void SetOfflineTracks()                                              {fOfflineTracks=kTRUE; fStandaloneTracks=kFALSE; };
  void SetStandaloneTracks()                                           {fStandaloneTracks=kTRUE; fOfflineTracks=kFALSE; };

  void SetCalDetGain(AliTRDCalDet * const calDetGain)                         {fCalDetGain = calDetGain;};  

  void SetMaxEvent(Int_t nbevents)                                     { fMaxEvent = nbevents; };
  void SetDebug(Int_t debug)                                           { fDebug = debug; };

 private:
  AliESDEvent  *fESD;                            //! ESD object
  AliESDfriend *fESDfriend;                      //! ESD friend
  const AliESDtrack *fkEsdTrack;                  //! ESD track
  AliESDfriendTrack *fFriendTrack;               //! ESD friend track
  TObject *fCalibObject;                         //! calibration objects attached to the ESD friend
  AliTRDtrackV1 *fTrdTrack;                      //! trdtrack
  AliTRDcluster *fCl;                            //! cluster
  
  TList       *fListHist;                        //! list of histograms

  AliTRDCalibraFillHisto *fTRDCalibraFillHisto;  //! calibration analyse object

  TH1I        *fNEvents;                         //! counter  
  
  TH1F        *fNbTRDTrack;                      //! nb ESD tracks with TRD clusters
  TH1F        *fNbTRDTrackOffline;               //! nb ESD tracks with TRD clusters
  TH1F        *fNbTRDTrackStandalone;            //! nb ESD tracks with TRD clusters
  TH2F        *fNbTPCTRDtrack;                   //! nb TPC and TRD tracks when problems
   
  TH1F        *fNbTimeBin;                       //! nb Time Bin
  TH1F        *fNbTimeBinOffline;                //! nb Time Bin offline
  TH1F        *fNbTimeBinStandalone;             //! nb Time Bin standalone
  TH1F        *fNbClusters;                      //! nb Clusters
  TH1F        *fNbClustersOffline;               //! nb Clusters offline
  TH1F        *fNbClustersStandalone;            //! nb Clusters standalone
  TH1F        *fNbTracklets;                     //! nb Tracklets
  TH1F        *fNbTrackletsOffline;              //! nb Tracklets offline
  TH1F        *fNbTrackletsStandalone;           //! nb Tracklets standalone
  
  TH2I        *fCH2dSum;                         //! CH2d charge all
  TProfile2D  *fPH2dSum;                         //! PH2d PH all
  TH2I        *fCH2dSM;                          //! CH2d per SM
  TProfile2D  *fPH2dSM;                          //! PH2d per SM

  Bool_t      fHisto2d;                          //! histo
  Bool_t      fVector2d;                         //! vector
  Bool_t      fVdriftLinear;                     //! vdrift Linear

  Int_t       fNbTimeBins;                       //! number of timebins 

  Short_t     fNz[3];                            // Nz mode 
  Short_t     fNrphi[3];                         // Nrphi mode
   
  TObjArray   *fSelectedTrigger;                 // Trigger class names accepted/rejected
  Bool_t      fRejected;                         // Reject the selected trigger class
  
  AliESDtrackCuts *fEsdTrackCuts;                // Quality cut on the AliESDtrack
  Bool_t      fRequirePrimaryVertex;             // Primary Vertex
  Int_t       fMinNbContributors;                // Min number of contributors
  
  Int_t       fLow;                              // lower limit of nb of TRD clusters per tracklet
  Int_t       fHigh;                             // higher limit of nb of TRD clusters per tracklet
  Bool_t      fFillZero;                         // fill zero
  Bool_t      fNormalizeNbOfCluster;             // normalize with number of clusters (per default not) 
  Float_t     fRelativeScale;                    // relative scale for gas gain
  Float_t     fMaxCluster;                       // Maxcluster 
  Short_t     fNbMaxCluster;                     // Number of tb at the end
  Bool_t      fOfflineTracks;                    // Only Offline refitted tracks
  Bool_t      fStandaloneTracks;                 // Take only standalone tracks

  AliTRDCalDet *fCalDetGain;                     // Calib object gain

  Int_t       fMaxEvent;                         // max events
  Int_t       fCounter;                          // max events
  Int_t       fDebug;                            //! fDebug

  AliTRDCalibTask(const AliTRDCalibTask&); 
  AliTRDCalibTask& operator=(const AliTRDCalibTask&); 

  ClassDef(AliTRDCalibTask, 1); 
};

#endif

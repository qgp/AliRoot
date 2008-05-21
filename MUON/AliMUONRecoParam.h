#ifndef AliMUONRecoParam_H
#define AliMUONRecoParam_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
* See cxx source for full Copyright notice                               */

/// \ingroup rec
/// \class AliMUONRecoParam
/// \brief Class with MUON reconstruction parameters
///
//  Author: Philippe Pillot

#include "AliDetectorRecoParam.h"
#include "TString.h"

class AliMUONRecoParam : public AliDetectorRecoParam
{
 public: 
  AliMUONRecoParam();
  virtual ~AliMUONRecoParam();
  
  static AliMUONRecoParam *GetLowFluxParam();
  static AliMUONRecoParam *GetHighFluxParam();
  static AliMUONRecoParam *GetCosmicParam();
  
  /// set the calibration mode (see GetCalibrationMode() for possible modes)
  void SetCalibrationMode(Option_t* mode) { fCalibrationMode = mode; fCalibrationMode.ToUpper();}

  Option_t* GetCalibrationMode() const;
      
  /// set the clustering (pre-clustering) mode
  void      SetClusteringMode(Option_t* mode) {fClusteringMode = mode; fClusteringMode.ToUpper();}
  /// get the clustering (pre-clustering) mode
  Option_t* GetClusteringMode() const {return fClusteringMode.Data();}
  
  /// set the tracking mode
  void      SetTrackingMode(Option_t* mode) {fTrackingMode = mode; fTrackingMode.ToUpper();}
  /// get the tracking mode
  Option_t* GetTrackingMode() const {return fTrackingMode.Data();}
  
  /// switch on/off the combined cluster/track reconstruction
  void      CombineClusterTrackReco(Bool_t flag) {fCombinedClusterTrackReco = flag;}
  /// return kTRUE/kFALSE if the combined cluster/track reconstruction is on/off
  Bool_t    CombineClusterTrackReco() const {return fCombinedClusterTrackReco;}
  
  /// save all cluster info (including pads) in ESD, for the given percentage of events
  void      SaveFullClusterInESD(Bool_t flag, Double_t percentOfEvent = 100.) {fSaveFullClusterInESD = flag;
                                 fPercentOfFullClusterInESD = (fSaveFullClusterInESD) ? percentOfEvent : 0.;}
  /// return kTRUE/kFALSE depending on whether we save all cluster info in ESD or not
  Bool_t    SaveFullClusterInESD() const {return fSaveFullClusterInESD;}
  /// return the percentage of events for which all cluster info are stored in ESD
  Double_t  GetPercentOfFullClusterInESD() const {return fPercentOfFullClusterInESD;}
  
  /// set the most probable value (GeV/c) of momentum in bending plane
  /// needed to get some "reasonable" corrections for MCS and E loss even if B = 0
  void     SetMostProbBendingMomentum(Double_t val) {fMostProbBendingMomentum = val;}
  /// return the most probable value (GeV/c) of momentum in bending plane
  Double_t GetMostProbBendingMomentum() const {return fMostProbBendingMomentum;}
  
  /// set the minimum value (GeV/c) of momentum in bending plane
  void     SetMinBendingMomentum(Double_t val) {fMinBendingMomentum = val;}
  /// return the minimum value (GeV/c) of momentum in bending plane
  Double_t GetMinBendingMomentum() const {return fMinBendingMomentum;}
  /// set the maximum value (GeV/c) of momentum in bending plane
  void     SetMaxBendingMomentum(Double_t val) {fMaxBendingMomentum = val;}
  /// return the maximum value (GeV/c) of momentum in bending plane
  Double_t GetMaxBendingMomentum() const {return fMaxBendingMomentum;}
  /// set the maximum value of the non bending slope
  void     SetMaxNonBendingSlope(Double_t val) {fMaxNonBendingSlope = val;}
  /// return the maximum value of the non bending slope
  Double_t GetMaxNonBendingSlope() const {return fMaxNonBendingSlope;}
  /// set the maximum value of the bending slope
  void     SetMaxBendingSlope(Double_t val) {fMaxBendingSlope = val;}
  /// return the maximum value of the bending slope
  Double_t GetMaxBendingSlope() const {return fMaxBendingSlope;}
  
  /// set the vertex dispersion (cm) in non bending plane (used for original tracking only)
  void     SetNonBendingVertexDispersion(Double_t val) {fNonBendingVertexDispersion = val;} 
  /// return the vertex dispersion (cm) in bending plane (used for original tracking only)
  Double_t GetNonBendingVertexDispersion() const {return fNonBendingVertexDispersion;}
  /// set the vertex dispersion (cm) in non bending plane (used for original tracking only)
  void     SetBendingVertexDispersion(Double_t val) {fBendingVertexDispersion = val;} 
  /// return the vertex dispersion (cm) in bending plane (used for original tracking only)
  Double_t GetBendingVertexDispersion() const {return fBendingVertexDispersion;}
  
  /// set the maximum distance to the track to search for compatible cluster(s) in non bending direction
  void     SetMaxNonBendingDistanceToTrack(Double_t val) {fMaxNonBendingDistanceToTrack = val;} 
  /// return the maximum distance to the track to search for compatible cluster(s) in non bending direction
  Double_t GetMaxNonBendingDistanceToTrack() const {return fMaxNonBendingDistanceToTrack;}
  /// set the maximum distance to the track to search for compatible cluster(s) in bending direction
  void     SetMaxBendingDistanceToTrack(Double_t val) {fMaxBendingDistanceToTrack = val;} 
  /// return the maximum distance to the track to search for compatible cluster(s) in bending direction
  Double_t GetMaxBendingDistanceToTrack() const {return fMaxBendingDistanceToTrack;}
  
  /// set the cut in sigma to apply on cluster (local chi2) and track (global chi2) during tracking
  void     SetSigmaCutForTracking(Double_t val) {fSigmaCutForTracking = val;} 
  /// return the cut in sigma to apply on cluster (local chi2) and track (global chi2) during tracking
  Double_t GetSigmaCutForTracking() const {return fSigmaCutForTracking;}
  
  /// switch on/off the track improvement and keep the default cut in sigma to apply on cluster (local chi2)
  void     ImproveTracks(Bool_t flag) {fImproveTracks = flag;} 
  /// switch on/off the track improvement and set the cut in sigma to apply on cluster (local chi2)
  void     ImproveTracks(Bool_t flag, Double_t sigmaCut) {fImproveTracks = flag; fSigmaCutForImprovement = sigmaCut;} 
  /// return kTRUE/kFALSE if the track improvement is switch on/off
  Bool_t   ImproveTracks() const {return fImproveTracks;}
  /// return the cut in sigma to apply on cluster (local chi2) during track improvement
  Double_t GetSigmaCutForImprovement() const {return fSigmaCutForImprovement;}

  /// set the cut in sigma to apply on track during trigger hit pattern search
  void     SetSigmaCutForTrigger(Double_t val) {fSigmaCutForTrigger = val;} 
  /// return the cut in sigma to apply on track during trigger hit pattern search
  Double_t GetSigmaCutForTrigger() const {return fSigmaCutForTrigger;}
  /// set the cut in strips to apply on trigger track during trigger chamber efficiency
  void     SetStripCutForTrigger(Double_t val) {fStripCutForTrigger = val;} 
  /// return the cut in strips to apply on trigger track during trigger chamber efficiency
  Double_t GetStripCutForTrigger() const {return fStripCutForTrigger;}
  /// set the maximum search area in strips to apply on trigger track during trigger chamber efficiency
  void     SetMaxStripAreaForTrigger(Double_t val) {fMaxStripAreaForTrigger = val;} 
  /// return the maximum search area in strips to apply on trigger track during trigger chamber efficiency
  Double_t GetMaxStripAreaForTrigger() const {return fMaxStripAreaForTrigger;}
  
  /// set the maximum normalized chi2 of tracking/trigger track matching
  void     SetMaxNormChi2MatchTrigger(Double_t val) {fMaxNormChi2MatchTrigger = val;} 
  /// return the maximum normalized chi2 of tracking/trigger track matching
  Double_t GetMaxNormChi2MatchTrigger() const {return fMaxNormChi2MatchTrigger;}
  
  /// switch on/off the tracking of all the possible candidates (track only the best one if switched off)
  void     TrackAllTracks(Bool_t flag) {fTrackAllTracks = flag;} 
  /// return kTRUE/kFALSE if the tracking of all the possible candidates is switched on/off
  Bool_t   TrackAllTracks() const {return fTrackAllTracks;}
  
  /// switch on/off the recovering of tracks being lost during reconstruction
  void     RecoverTracks(Bool_t flag) {fRecoverTracks = flag;} 
  /// return kTRUE/kFALSE if the recovering of tracks being lost during reconstruction is switched on/off
  Bool_t   RecoverTracks() const {return fRecoverTracks;}
  
  /// switch on/off the fast building of track candidates (assuming linear propagation between stations 4 and 5)
  void     MakeTrackCandidatesFast(Bool_t flag) {fMakeTrackCandidatesFast = flag;} 
  /// return kTRUE/kFALSE if the fast building of track candidates is switched on/off
  Bool_t   MakeTrackCandidatesFast() const {return fMakeTrackCandidatesFast;}
  
  /// switch on/off the building of track candidates starting from 1 cluster in each of the stations 4 and 5
  void     MakeMoreTrackCandidates(Bool_t flag) {fMakeMoreTrackCandidates = flag;} 
  /// return kTRUE/kFALSE if the building of extra track candidates is switched on/off
  Bool_t   MakeMoreTrackCandidates() const {return fMakeMoreTrackCandidates;}
  
  /// switch on/off the completion of reconstructed track
  void     ComplementTracks(Bool_t flag) {fComplementTracks = flag;} 
  /// return kTRUE/kFALSE if completion of the reconstructed track is switched on/off
  Bool_t   ComplementTracks() const {return fComplementTracks;}
  
  /// switch on/off the use of the smoother
  void     UseSmoother(Bool_t flag) {fUseSmoother = flag;} 
  /// return kTRUE/kFALSE if the use of the smoother is switched on/off
  Bool_t   UseSmoother() const {return fUseSmoother;}
  
  /// switch on/off a chamber in the reconstruction
  void     UseChamber(Int_t iCh, Bool_t flag) {if (iCh >= 0 && iCh < 10) fUseChamber[iCh] = flag;}
  /// return kTRUE/kFALSE whether the chamber must be used or not
  Bool_t   UseChamber(Int_t iCh) const {return (iCh >= 0 && iCh < 10) ? fUseChamber[iCh] : kFALSE;}
  
  /// request or not at least one cluster in the station to validate the track
  void     RequestStation(Int_t iSt, Bool_t flag) {if (iSt >= 0 && iSt < 5) fRequestStation[iSt] = flag;}
  /// return kTRUE/kFALSE whether at least one cluster is requested in the station to validate the track
  Bool_t   RequestStation(Int_t iSt) const {return (iSt >= 0 && iSt < 5) ? fRequestStation[iSt] : kFALSE;}
  
  /// set the bypassSt45 value
  void BypassSt45(Bool_t value) { fBypassSt45 = value; }
  /// return kTRUE if we should replace clusters in St 4 and 5 by generated clusters from trigger tracks
  Bool_t BypassSt45() const { return fBypassSt45; }
  
  
  virtual void Print(Option_t *option = "") const;
  
 private:
  
  /// clustering mode:  NOCLUSTERING, PRECLUSTER, PRECLUSTERV2, PRECLUSTERV3, COG, <pre>
  ///                   SIMPLEFIT, SIMPLEFITV3, MLEM:DRAW, MLEM, MLEMV2, MLEMV3   </pre>
  TString fClusteringMode; ///< \brief name of the clustering (+ pre-clustering) mode
  
  /// tracking mode: ORIGINAL, KALMAN
  TString fTrackingMode; ///< \brief name of the tracking mode
  
  Double32_t fMostProbBendingMomentum; ///< most probable value (GeV/c) of muon momentum in bending plane (used when B = 0)
  
  Double32_t fMinBendingMomentum; ///< minimum value (GeV/c) of momentum in bending plane
  Double32_t fMaxBendingMomentum; ///< maximum value (GeV/c) of momentum in bending plane
  Double32_t fMaxNonBendingSlope; ///< maximum value of the non bending slope
  Double32_t fMaxBendingSlope;    ///< maximum value of the bending slope (used only if B = 0)
  
  Double32_t fNonBendingVertexDispersion; ///< vertex dispersion (cm) in non bending plane (used for original tracking only)
  Double32_t fBendingVertexDispersion;    ///< vertex dispersion (cm) in bending plane (used for original tracking only)
  
  Double32_t fMaxNonBendingDistanceToTrack; ///< maximum distance to the track to search for compatible cluster(s) in non bending direction
  Double32_t fMaxBendingDistanceToTrack;    ///< maximum distance to the track to search for compatible cluster(s) in bending direction
  
  Double32_t fSigmaCutForTracking; ///< cut in sigma to apply on cluster (local chi2) and track (global chi2) during tracking

  Double32_t fSigmaCutForImprovement; ///< cut in sigma to apply on cluster (local chi2) during track improvement
  
  Double32_t fSigmaCutForTrigger; ///< cut in sigma to apply on track during trigger hit pattern search

  Double32_t fStripCutForTrigger; ///< cut in strips to apply on trigger track during trigger chamber efficiency

  Double32_t fMaxStripAreaForTrigger; ///< max. search area in strips to apply on trigger track during trigger chamber efficiency
  
  Double32_t fMaxNormChi2MatchTrigger; ///< maximum normalized chi2 of tracking/trigger track matching
  
  Double32_t fPercentOfFullClusterInESD; ///< percentage of events for which all cluster info are stored in ESD
  
  Bool_t     fCombinedClusterTrackReco; ///< switch on/off the combined cluster/track reconstruction
  
  Bool_t     fTrackAllTracks; ///< kTRUE to track all the possible candidates; kFALSE to track only the best ones
  
  Bool_t     fRecoverTracks; ///< kTRUE to try to recover the tracks getting lost during reconstruction
  
  Bool_t     fMakeTrackCandidatesFast; ///< kTRUE to make candidate tracks assuming linear propagation between stations 4 and 5
  
  Bool_t     fMakeMoreTrackCandidates; ///< kTRUE to make candidate tracks starting from 1 cluster in each of the stations 4 and 5
  
  Bool_t     fComplementTracks; ///< kTRUE to try to complete the reconstructed tracks by adding missing clusters
  
  Bool_t     fImproveTracks; ///< kTRUE to try to improve the reconstructed tracks by removing bad clusters
  
  Bool_t     fUseSmoother; ///< kTRUE to use the smoother to compute track parameters/covariances and local chi2 at each cluster (used for Kalman tracking only)
  
  Bool_t     fSaveFullClusterInESD; ///< kTRUE to save all cluster info (including pads) in ESD
  
  /// calibration mode:  GAIN, NOGAIN, GAINCONSTANTCAPA
  TString fCalibrationMode; ///<\brief calibration mode
  
  Bool_t fBypassSt45; ///< kTRUE to use trigger tracks to generate "fake" clusters in St 4 and 5
  
  Bool_t     fUseChamber[10]; ///< kTRUE to use the chamber i in the tracking algorithm
  
  Bool_t     fRequestStation[5]; ///< kTRUE to request at least one cluster in station i to validate the track
  
  // functions
  void SetLowFluxParam();
  void SetHighFluxParam();
  void SetCosmicParam();
  
  
  ClassDef(AliMUONRecoParam,5) // MUON reco parameters
};

#endif


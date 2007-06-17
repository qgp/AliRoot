#ifndef ALIMUONTRACK_H
#define ALIMUONTRACK_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/*$Id$*/
// Revision of includes 07/05/2004

/// \ingroup rec
/// \class AliMUONTrack
/// \brief Reconstructed track in ALICE dimuon spectrometer
///
////////////////////////////////////////////////////
/// Reconstructed track in ALICE dimuon spectrometer
////////////////////////////////////////////////////

#include <TClonesArray.h>

#include "AliMUONTrackParam.h" // object belongs to the class

class AliMUONHitForRec;

class AliMUONTrack : public TObject 
{
 public:
  AliMUONTrack(); // Default constructor
  virtual ~AliMUONTrack(); // Destructor
  AliMUONTrack (const AliMUONTrack& AliMUONTrack); // copy constructor
  AliMUONTrack& operator=(const AliMUONTrack& AliMUONTrack); // assignment operator

  AliMUONTrack(AliMUONHitForRec* hitForRec1, AliMUONHitForRec* hitForRec2); // Constructor from a segment

	/// return pointeur to track parameters at vertex
  AliMUONTrackParam*         GetTrackParamAtVertex(void) {return &fTrackParamAtVertex;}
	/// set track parameters at vertex
  void                       SetTrackParamAtVertex(AliMUONTrackParam* trackParam) {fTrackParamAtVertex = *trackParam;}

	/// return array of track parameters at hit
  TClonesArray*              GetTrackParamAtHit(void) const {return fTrackParamAtHit;}
	/// reset array of track parameters at hit
  void                       ResetTrackParamAtHit(void) { fTrackParamAtHit->Delete(); }
  void                       AddTrackParamAtHit(AliMUONTrackParam *trackParam, AliMUONHitForRec *hitForRec); 
  
	/// return array of hitForRec at hit
  TClonesArray*              GetHitForRecAtHit(void) const {return fHitForRecAtHit;}
	/// reset array of hitForRec at hit
  void                       ResetHitForRecAtHit(void) { fHitForRecAtHit->Delete(); }
  void                       AddHitForRecAtHit(const AliMUONHitForRec *hitForRec); 

	/// return the number of hits attached to the track
  Int_t                      GetNTrackHits(void) const {return fNTrackHits;}
	/// set the number of hits attached to the track
  void                       SetNTrackHits(Int_t nTrackHits) {fNTrackHits = nTrackHits;}

	/// return pointeur to track parameters extrapolated to the next station
  AliMUONTrackParam*         GetExtrapTrackParam(void) {return &fExtrapTrackParam;}
	/// set track parameters extrapolated to next station
  void                       SetExtrapTrackParam(AliMUONTrackParam* trackParam) {fExtrapTrackParam = *trackParam;}

	/// return kTrue if the vertex must be used to constrain the fit, kFalse if not
  Bool_t                     GetFitWithVertex(void) const {return fFitWithVertex;}
	/// set the flag telling whether the vertex must be used to constrain the fit or not
  void                       SetFitWithVertex(Bool_t fitWithVertex) { fFitWithVertex = fitWithVertex; }
	/// return the vertex used during the tracking procedure
  AliMUONHitForRec*          GetVertex(void) const {return fVertex;}
  void                       SetVertex(AliMUONHitForRec* vertex);

	/// return the minimum value of the function minimized by the fit
  Double_t                   GetFitFMin(void) const {return fFitFMin;}
	/// set the minimum value of the function minimized by the fit
  void                       SetFitFMin(Double_t chi2) { fFitFMin = chi2; }
        /// return 1,2,3 if track matches with trigger track, 0 if not
  Int_t                     GetMatchTrigger(void) const {return fMatchTrigger;}
  /// returns the local trigger number corresponding to the trigger track 
  Int_t                      GetLoTrgNum(void) const {return floTrgNum;}
	/// set the flag telling whether track matches with trigger track or not
   void			     SetMatchTrigger(Int_t matchTrigger) {fMatchTrigger = matchTrigger;}
   /// set the local trigger number corresponding to the trigger track
   void			     SetLoTrgNum(Int_t loTrgNum) {floTrgNum = loTrgNum;}
	/// return the chi2 of trigger/track matching 
  Double_t                   GetChi2MatchTrigger(void) const {return fChi2MatchTrigger;}
	/// set the chi2 of trigger/track matching 
  void                       SetChi2MatchTrigger(Double_t chi2MatchTrigger) {fChi2MatchTrigger = chi2MatchTrigger;}
  
  Int_t                      HitsInCommon(AliMUONTrack* track) const;
  Bool_t*                    CompatibleTrack(AliMUONTrack* track, Double_t sigma2Cut) const; // return array of compatible chamber
  
	/// return track number in TrackRefs
  Int_t                      GetTrackID() const {return fTrackID;}
	/// set track number in TrackRefs
  void                       SetTrackID(Int_t trackID) {fTrackID = trackID;}

        /// set word telling which trigger chambers where hit by track
  UShort_t                   GetHitsPatternInTrigCh() const {return fHitsPatternInTrigCh;}
        /// set word telling which trigger chambers where hit by track
  void                       SetHitsPatternInTrigCh(UShort_t hitsPatternInTrigCh) {fHitsPatternInTrigCh = hitsPatternInTrigCh;}

  /// set local trigger information for the matched trigger track
  void SetLocalTrigger(Int_t loCirc, Int_t loStripX, Int_t loStripY, Int_t loDev, Int_t loLpt, Int_t loHpt);
  /// return local trigger information for the matched trigger track
  Int_t GetLocalTrigger(void) const { return fLocalTrigger;              }
  /// number of triggering circuit
  Int_t LoCircuit(void) const 
  { Int_t circ = fLocalTrigger & 0xFF; return (circ == 234) ? -1 : circ; }
  /// x-strip local trigger 
  Int_t LoStripX(void) const  { return fLocalTrigger >>  8 & 0x1F; }
  /// y-strip local trigger 
  Int_t LoStripY(void) const  { return fLocalTrigger >> 13 & 0x0F; }
  /// deviation local trigger 
  Int_t LoDev(void)    const  { return fLocalTrigger >> 17 & 0x1F; }
  /// low pt decision local trigger 
  Int_t LoLpt(void)    const  { return fLocalTrigger >> 22 & 0x03; }
  /// high pt decision local trigger 
  Int_t LoHpt(void)    const  { return fLocalTrigger >> 24 & 0x03; }

  Double_t                   TryOneHitForRec(AliMUONHitForRec* hitForRec);
  Double_t                   TryTwoHitForRec(AliMUONHitForRec* hitForRec1, AliMUONHitForRec* hitForRec2); 
  
  void                       RecursiveDump(void) const; // Recursive dump (with track hits)

  virtual void               Print(Option_t* opt="") const;

  virtual void Clear(Option_t* opt="");

 private:
  static const Double_t fgkMaxTrackingDistanceBending;    ///< Maximum distance to the track to search for compatible hitForRec(s) in bending direction
  static const Double_t fgkMaxTrackingDistanceNonBending; ///< Maximum distance to the track to search for compatible hitForRec(s) in non bending direction
  
  AliMUONTrackParam fTrackParamAtVertex; //!< Track parameters at vertex
  TClonesArray *fTrackParamAtHit; ///< Track parameters at hit
  TClonesArray *fHitForRecAtHit; ///< Cluster parameters at hit
  Int_t fNTrackHits; ///< Number of hits attached to the track
  
  AliMUONTrackParam fExtrapTrackParam; //!< Track parameters extrapolated to a given z position
  
  Bool_t fFitWithVertex; //!< 1 if using the vertex to constrain the fit, 0 if not
  AliMUONHitForRec *fVertex; //!< Vertex used during the tracking procedure if required
  
  Double_t fFitFMin; ///< minimum value of the function minimized by the fit
  Int_t fMatchTrigger;  ///<  0 track does not match trigger
                        ///<  1 track match but does not pass pt cut
                        ///<  2 track match Low pt cut
                        ///<  3 track match High pt cut
  Int_t floTrgNum; ///< the number of the corresponding loTrg, -1 if no matching
  Double_t fChi2MatchTrigger; ///< chi2 of trigger/track matching 
  
  Int_t fTrackID; ///< track ID = track number in TrackRefs
  UShort_t fHitsPatternInTrigCh; ///< Word containing info on the hits left in trigger chambers

  Int_t fLocalTrigger;    ///< packed local trigger information
  
  ClassDef(AliMUONTrack,6) // Reconstructed track in ALICE dimuon spectrometer
};
	
#endif

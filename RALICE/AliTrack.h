#ifndef ALITRACK_H
#define ALITRACK_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

// $Id$


#include "TObject.h"
#include "TObjArray.h"
#include "TArrayD.h"
 
#include "AliSignal.h"
#include "AliBoost.h"
#include "AliPositionObj.h"
 
class AliTrack : public TNamed,public Ali4Vector
{
 public:
  AliTrack();                           // Default constructor
  virtual ~AliTrack();                  // Destructor
  AliTrack(AliTrack& t);                // Copy constructor
  virtual TObject* Clone(char* name="");// Make a deep copy and provide its pointer
  virtual void Reset();                 // Reset all values to 0
  void Set4Momentum(Ali4Vector& p);     // Set track 4-momentum
  void Set3Momentum(Ali3Vector& p);     // Set track 3-momentum
  void SetMass(Double_t m,Double_t dm=0); // Set particle mass and error
  void SetMass();                       // Set mass and error to the values of the hypothesis with highest prob.
  void SetCharge(Float_t q);            // Set particle charge
  virtual void Data(TString f="car");   // Print track information for coord. frame f
  virtual void List(TString f="car");   // Print track and decay level 1 information for coord. frame f
  virtual void ListAll(TString f="car");// Print track and all decay level information for coord. frame f
  Ali3Vector Get3Momentum();            // Provide track 3-momentum
  Double_t GetMomentum();               // Provide value of track 3-momentum
  Double_t GetMass();                   // Provide particle mass
  Float_t GetCharge();                  // Provide particle charge
  Double_t GetEnergy();                 // Provide particle total energy
  void Decay(Double_t m1,Double_t m2,Double_t thcms,Double_t phicms); // Perform 2-body decay
  Int_t GetNdecay();                    // Provide number of decay products
  AliTrack* GetDecayTrack(Int_t j);     // Access to decay produced track number j
  void RemoveDecays();                  // Remove all the decay products of this track
  void AddSignal(AliSignal& s);         // Relate an AliSignal to this track
  void RemoveSignal(AliSignal& s);      // Remove related AliSignal from this track
  void RemoveSignals();                 // Remove all related AliSignals from this track
  Int_t GetNsignals();                  // Provide number of related AliSignals
  AliSignal* GetSignal(Int_t j);        // Access to the related AliSignal number j
  void SetBeginPoint(AliPosition& p);   // Set the track begin-point
  AliPosition* GetBeginPoint();         // Provide the track begin-point
  void SetEndPoint(AliPosition& p);     // Set the track end-point
  AliPosition* GetEndPoint();           // Provide the track end-point
  void AddTrackHypothesis(AliTrack& t); // Add track hypothesis
  void AddTrackHypothesis(Double_t prob,Double_t m,Double_t dm=0); // Add track hypothesis with mass data
  Int_t GetNhypotheses();               // Provide number of track hypotheses
  AliTrack* GetTrackHypothesis(Int_t j=0);  // Provide the j-th track hypothesis 
  void RemoveTrackHypothesis(AliTrack& t);  // Remove the specified track hypothesis 
  void RemoveTrackHypotheses();         // Remove all track hypotheses 
  Double_t GetPt();                     // Provide trans. momentum w.r.t. z-axis
  Double_t GetPl();                     // Provide long. momentum w.r.t. z-axis
  Double_t GetEt();                     // Provide trans. energy w.r.t. z-axis
  Double_t GetEl();                     // Provide long. energy w.r.t. z-axis
  Double_t GetMt();                     // Provide trans. mass w.r.t. z-axis
  Double_t GetRapidity();               // Provide rapidity value w.r.t. z-axis
  void SetImpactPoint(AliPosition& p,TString q); // Set the impact-point in plane "q=0"
  AliPosition* GetImpactPoint(TString q);        // Provide the impact-point in plane "q=0"
  void SetId(Int_t id);                 // Set the user defined unique track identifier
  Int_t GetId();                        // Provide the user defined unique track identifier
  void SetClosestPoint(AliPosition& p); // Set position p as point of closest approach w.r.t. some reference
  AliPosition* GetClosestPoint();       // Provide point of closest approach w.r.t. some reference
  void SetChi2(Float_t chi2);           // Set the chi-squared value of the track fit
  void SetNdf(Int_t ndf);               // Set the number of degrees of freedom for the track fit
  Float_t GetChi2();                    // Provide the chi-squared value of the track fit
  Int_t GetNdf();                       // Provide the number of degrees of freedom for the track fit
  void SetParticleCode(Int_t code);     // Set the user defined particle id code (e.g. the PDF convention)
  Int_t GetParticleCode();              // Provide the user defined particle id code
  void SetParentTrack(AliTrack* t);     // Set pointer to the parent track
  AliTrack* GetParentTrack();           // Provide pointer to the parent track
  void SetProb(Double_t prob);          // Set the hypothesis probability for this track
  Float_t GetProb();                    // Provide the hypothesis probability for this track

 
 protected:
  void Init();               // Initialisation of pointers etc...
  Float_t fQ;                // The charge of the particle
  TObjArray* fDecays;        // The array of decay produced tracks
  TObjArray* fSignals;       // The array of related AliSignals
  TObjArray* fHypotheses;    // The array of track hypotheses
  AliPositionObj* fBegin;    // The begin-point of the track 
  AliPositionObj* fEnd;      // The end-point of the track 
  AliPositionObj* fImpactXY; // The (extrapolated) impact-point in the plane z=0
  AliPositionObj* fImpactXZ; // The (extrapolated) impact-point in the plane y=0
  AliPositionObj* fImpactYZ; // The (extrapolated) impact-point in the plane x=0
  Int_t fUserId;             // The user defined identifier
  AliPositionObj* fClosest;  // The (extrapolated) point of closest approach w.r.t some reference
  Float_t fChi2;             // The Chi-squared of the track fit
  Int_t fNdf;                // The number of degrees of freedom of the track fit
  Int_t fCode;               // The user defined particle id code
  AliTrack* fParent;         // Pointer to the parent track
  Float_t fProb;             // Probability for this track as a hypothesis

 private:
  void Dumps(AliTrack* t,Int_t n,TString f); // Recursively print all decay levels
 
 ClassDef(AliTrack,8) // Handling of the attributes of a reconstructed particle track.
};
#endif

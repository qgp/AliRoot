#ifndef ALIMUONTRACKPARAM_H
#define ALIMUONTRACKPARAM_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/*$Id$*/
// Revision of includes 07/05/2004

/// \ingroup rec
/// \class AliMUONTrackParam
/// \brief Track parameters in ALICE dimuon spectrometer
///
////////////////////////////////////////////////////
/// Track parameters in ALICE dimuon spectrometer
////////////////////////////////////////////////////

#include <TObject.h>
class AliESDMuonTrack;

class AliMUONTrackParam : public TObject 
{
 public:
  AliMUONTrackParam(); // Constructor
  virtual ~AliMUONTrackParam(){} // Destructor
  
  AliMUONTrackParam(const AliMUONTrackParam& rhs);// copy constructor (should be added per default !)
  AliMUONTrackParam& operator=(const  AliMUONTrackParam& rhs);// (should be added per default !)
  void GetParamFrom(const AliESDMuonTrack& esdMuonTrack);
  void SetParamFor(AliESDMuonTrack& esdMuonTrack);

  // Get and Set methods for data
  Double_t GetInverseBendingMomentum(void) const {return fInverseBendingMomentum;}
  void SetInverseBendingMomentum(Double_t InverseBendingMomentum) {fInverseBendingMomentum = InverseBendingMomentum;}
  Double_t GetBendingSlope(void) const {return fBendingSlope;}
  void SetBendingSlope(Double_t BendingSlope) {fBendingSlope = BendingSlope;}
  Double_t GetNonBendingSlope(void) const {return fNonBendingSlope;}
  void SetNonBendingSlope(Double_t NonBendingSlope) {fNonBendingSlope = NonBendingSlope;}
  Double_t GetZ(void) const {return fZ;}
  void SetZ(Double_t Z) {fZ = Z;}
  Double_t GetBendingCoor(void) const {return fBendingCoor;}
  void SetBendingCoor(Double_t BendingCoor) {fBendingCoor = BendingCoor;}
  Double_t GetNonBendingCoor(void) const {return fNonBendingCoor;}
  void SetNonBendingCoor(Double_t NonBendingCoor) {fNonBendingCoor = NonBendingCoor;}
  Double_t Px();  // return px
  Double_t Py();  // return py
  Double_t Pz();  // return pz
  Double_t P();   // return total momentum

  void ExtrapToZ(Double_t Z);
  void ExtrapToStation(Int_t Station, AliMUONTrackParam *TrackParam);
  void ExtrapToVertex(Double_t xVtx, Double_t yVtx, Double_t zVtx);  // extrapolation to vertex through the absorber (with true vertex) 
   void BransonCorrection(Double_t xVtx, Double_t yVtx, Double_t zVtx); // makes Branson correction with true vertex  
   // returns total momentum after energy loss correction in the absorber
  Double_t TotalMomentumEnergyLoss(Double_t thetaLimit, Double_t pTotal, Double_t theta);
  void FieldCorrection(Double_t Z); // makes simple magnetic field correction through the absorber 

  void ExtrapOneStepHelix(Double_t charge, Double_t step, 
			  Double_t *vect, Double_t *vout) const;
  void ExtrapOneStepHelix3(Double_t field, Double_t step, 
			   Double_t *vect, Double_t *vout) const;

  void ExtrapOneStepRungekutta(Double_t charge, Double_t step, 
			       Double_t* vect, Double_t* vout) const;
 protected:
 private:
  Double_t fInverseBendingMomentum; // Inverse bending momentum (GeV/c ** -1) times the charge (assumed forward motion)
  Double_t fBendingSlope; // Bending slope (cm ** -1)
  Double_t fNonBendingSlope; // Non bending slope (cm ** -1)
  Double_t fZ; // Z coordinate (cm)
  Double_t fBendingCoor; // bending coordinate (cm)
  Double_t fNonBendingCoor; // non bending coordinate (cm)

  void SetGeant3Parameters(Double_t *VGeant3, Double_t ForwardBackward);
  void GetFromGeant3Parameters(Double_t *VGeant3, Double_t Charge);

  void GetField(Double_t *Position, Double_t *Field) const;

  ClassDef(AliMUONTrackParam, 1) // Track parameters in ALICE dimuon spectrometer
    };
	
#endif

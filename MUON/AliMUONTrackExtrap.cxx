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

/* $Id$ */

///////////////////////////////////////////////////
//
// Tools
// for
// track
// extrapolation
// in
// ALICE
// dimuon
// spectrometer
//
///////////////////////////////////////////////////

#include "AliMUONTrackExtrap.h" 
#include "AliMUONTrackParam.h"
#include "AliMUONConstants.h"

#include "AliMagF.h" 

#include <Riostream.h>
#include <TMath.h>
#include <TMatrixD.h>
#include <TGeoManager.h>

/// \cond CLASSIMP
ClassImp(AliMUONTrackExtrap) // Class implementation in ROOT context
/// \endcond

const AliMagF* AliMUONTrackExtrap::fgkField = 0x0;
const Bool_t   AliMUONTrackExtrap::fgkUseHelix = kFALSE;
const Int_t    AliMUONTrackExtrap::fgkMaxStepNumber = 5000;
const Double_t AliMUONTrackExtrap::fgkHelixStepLength = 6.;
const Double_t AliMUONTrackExtrap::fgkRungeKuttaMaxResidue = 0.002;

  //__________________________________________________________________________
Double_t AliMUONTrackExtrap::GetImpactParamFromBendingMomentum(Double_t bendingMomentum)
{
  /// Returns impact parameter at vertex in bending plane (cm),
  /// from the signed bending momentum "BendingMomentum" in bending plane (GeV/c),
  /// using simple values for dipole magnetic field.
  /// The sign of "BendingMomentum" is the sign of the charge.
  
  if (bendingMomentum == 0.) return 1.e10;
  
  Double_t simpleBPosition = 0.5 * (AliMUONConstants::CoilZ() + AliMUONConstants::YokeZ());
  Double_t simpleBLength = 0.5 * (AliMUONConstants::CoilL() + AliMUONConstants::YokeL());
  Float_t b[3], x[3] = {0.,0.,(Float_t) simpleBPosition};
  if (fgkField) fgkField->Field(x,b);
  else {
    cout<<"F-AliMUONTrackExtrap::GetField: fgkField = 0x0"<<endl;
    exit(-1);
  }
  Double_t simpleBValue = (Double_t) b[0];
  
  return (-0.0003 * simpleBValue * simpleBLength * simpleBPosition / bendingMomentum);
}

  //__________________________________________________________________________
Double_t AliMUONTrackExtrap::GetBendingMomentumFromImpactParam(Double_t impactParam)
{
  /// Returns signed bending momentum in bending plane (GeV/c),
  /// the sign being the sign of the charge for particles moving forward in Z,
  /// from the impact parameter "ImpactParam" at vertex in bending plane (cm),
  /// using simple values for dipole magnetic field.
  
  if (impactParam == 0.) return 1.e10;
  
  Double_t simpleBPosition = 0.5 * (AliMUONConstants::CoilZ() + AliMUONConstants::YokeZ());
  Double_t simpleBLength = 0.5 * (AliMUONConstants::CoilL() + AliMUONConstants::YokeL());
  Float_t b[3], x[3] = {0.,0.,(Float_t) simpleBPosition};
  if (fgkField) fgkField->Field(x,b);
  else {
    cout<<"F-AliMUONTrackExtrap::GetField: fgkField = 0x0"<<endl;
    exit(-1);
  }
  Double_t simpleBValue = (Double_t) b[0];
  
  return (-0.0003 * simpleBValue * simpleBLength * simpleBPosition / impactParam);
}

  //__________________________________________________________________________
void AliMUONTrackExtrap::ExtrapToZ(AliMUONTrackParam* trackParam, Double_t zEnd)
{
  /// Interface to track parameter extrapolation to the plane at "Z" using Helix or Rungekutta algorithm.
  /// On return, the track parameters resulting from the extrapolation are updated in trackParam.
  if (fgkUseHelix) AliMUONTrackExtrap::ExtrapToZHelix(trackParam,zEnd);
  else AliMUONTrackExtrap::ExtrapToZRungekutta(trackParam,zEnd);
}

  //__________________________________________________________________________
void AliMUONTrackExtrap::ExtrapToZHelix(AliMUONTrackParam* trackParam, Double_t zEnd)
{
  /// Track parameter extrapolation to the plane at "Z" using Helix algorithm.
  /// On return, the track parameters resulting from the extrapolation are updated in trackParam.
  if (trackParam->GetZ() == zEnd) return; // nothing to be done if same Z
  Double_t forwardBackward; // +1 if forward, -1 if backward
  if (zEnd < trackParam->GetZ()) forwardBackward = 1.0; // spectro. z<0 
  else forwardBackward = -1.0;
  Double_t v3[7], v3New[7]; // 7 in parameter ????
  Int_t i3, stepNumber;
  // For safety: return kTRUE or kFALSE ????
  // Parameter vector for calling EXTRAP_ONESTEP
  ConvertTrackParamForExtrap(trackParam, forwardBackward, v3);
  // sign of charge (sign of fInverseBendingMomentum if forward motion)
  // must be changed if backward extrapolation
  Double_t chargeExtrap = forwardBackward * TMath::Sign(Double_t(1.0), trackParam->GetInverseBendingMomentum());
  // Extrapolation loop
  stepNumber = 0;
  while (((-forwardBackward * (v3[2] - zEnd)) <= 0.0) && (stepNumber < fgkMaxStepNumber)) { // spectro. z<0
    stepNumber++;
    ExtrapOneStepHelix(chargeExtrap, fgkHelixStepLength, v3, v3New);
    if ((-forwardBackward * (v3New[2] - zEnd)) > 0.0) break; // one is beyond Z spectro. z<0
    // better use TArray ????
    for (i3 = 0; i3 < 7; i3++) {v3[i3] = v3New[i3];}
  }
  // check fgkMaxStepNumber ????
  // Interpolation back to exact Z (2nd order)
  // should be in function ???? using TArray ????
  Double_t dZ12 = v3New[2] - v3[2]; // 1->2
  if (TMath::Abs(dZ12) > 0) {
    Double_t dZ1i = zEnd - v3[2]; // 1-i
    Double_t dZi2 = v3New[2] - zEnd; // i->2
    Double_t xPrime = (v3New[0] - v3[0]) / dZ12;
    Double_t xSecond = ((v3New[3] / v3New[5]) - (v3[3] / v3[5])) / dZ12;
    Double_t yPrime = (v3New[1] - v3[1]) / dZ12;
    Double_t ySecond = ((v3New[4] / v3New[5]) - (v3[4] / v3[5])) / dZ12;
    v3[0] = v3[0] + xPrime * dZ1i - 0.5 * xSecond * dZ1i * dZi2; // X
    v3[1] = v3[1] + yPrime * dZ1i - 0.5 * ySecond * dZ1i * dZi2; // Y
    v3[2] = zEnd; // Z
    Double_t xPrimeI = xPrime - 0.5 * xSecond * (dZi2 - dZ1i);
    Double_t yPrimeI = yPrime - 0.5 * ySecond * (dZi2 - dZ1i);
    // (PX, PY, PZ)/PTOT assuming forward motion
    v3[5] = 1.0 / TMath::Sqrt(1.0 + xPrimeI * xPrimeI + yPrimeI * yPrimeI); // PZ/PTOT
    v3[3] = xPrimeI * v3[5]; // PX/PTOT
    v3[4] = yPrimeI * v3[5]; // PY/PTOT
  } else {
    cout<<"W-AliMUONTrackExtrap::ExtrapToZHelix: Extrap. to Z not reached, Z = "<<zEnd<<endl;
  }
  // Recover track parameters (charge back for forward motion)
  RecoverTrackParam(v3, chargeExtrap * forwardBackward, trackParam);
}

  //__________________________________________________________________________
void AliMUONTrackExtrap::ExtrapToZRungekutta(AliMUONTrackParam* trackParam, Double_t zEnd)
{
  /// Track parameter extrapolation to the plane at "Z" using Rungekutta algorithm.
  /// On return, the track parameters resulting from the extrapolation are updated in trackParam.
  if (trackParam->GetZ() == zEnd) return; // nothing to be done if same Z
  Double_t forwardBackward; // +1 if forward, -1 if backward
  if (zEnd < trackParam->GetZ()) forwardBackward = 1.0; // spectro. z<0 
  else forwardBackward = -1.0;
  // sign of charge (sign of fInverseBendingMomentum if forward motion)
  // must be changed if backward extrapolation
  Double_t chargeExtrap = forwardBackward * TMath::Sign(Double_t(1.0), trackParam->GetInverseBendingMomentum());
  Double_t v3[7], v3New[7];
  Double_t dZ, step;
  Int_t stepNumber = 0;
  
  // Extrapolation loop (until within tolerance)
  Double_t residue = zEnd - trackParam->GetZ();
  while (TMath::Abs(residue) > fgkRungeKuttaMaxResidue && stepNumber <= fgkMaxStepNumber) {
    dZ = zEnd - trackParam->GetZ();
    // step lenght assuming linear trajectory
    step = dZ * TMath::Sqrt(1.0 + trackParam->GetBendingSlope()*trackParam->GetBendingSlope() +
    			    trackParam->GetNonBendingSlope()*trackParam->GetNonBendingSlope());
    ConvertTrackParamForExtrap(trackParam, forwardBackward, v3);
    do { // reduce step lenght while zEnd oversteped
      if (stepNumber > fgkMaxStepNumber) {
        cout<<"W-AliMUONTrackExtrap::ExtrapToZRungekutta: Too many trials: "<<stepNumber<<endl;
	break;
      }
      stepNumber ++;
      step = TMath::Abs(step);
      AliMUONTrackExtrap::ExtrapOneStepRungekutta(chargeExtrap,step,v3,v3New);
      residue = zEnd - v3New[2];
      step *= dZ/(v3New[2]-trackParam->GetZ());
    } while (residue*dZ < 0 && TMath::Abs(residue) > fgkRungeKuttaMaxResidue);
    RecoverTrackParam(v3New, chargeExtrap * forwardBackward, trackParam);
  }
  
  // terminate the extropolation with a straight line up to the exact "zEnd" value
  trackParam->SetNonBendingCoor(trackParam->GetNonBendingCoor() + residue * trackParam->GetNonBendingSlope());
  trackParam->SetBendingCoor(trackParam->GetBendingCoor() + residue * trackParam->GetBendingSlope());
  trackParam->SetZ(zEnd);
}

  //__________________________________________________________________________
void AliMUONTrackExtrap::ConvertTrackParamForExtrap(AliMUONTrackParam* trackParam, Double_t forwardBackward, Double_t *v3)
{
  /// Set vector of Geant3 parameters pointed to by "v3" from track parameters in trackParam.
  /// Since AliMUONTrackParam is only geometry, one uses "forwardBackward"
  /// to know whether the particle is going forward (+1) or backward (-1).
  v3[0] = trackParam->GetNonBendingCoor(); // X
  v3[1] = trackParam->GetBendingCoor(); // Y
  v3[2] = trackParam->GetZ(); // Z
  Double_t pYZ = TMath::Abs(1.0 / trackParam->GetInverseBendingMomentum());
  Double_t pZ = pYZ / TMath::Sqrt(1.0 + trackParam->GetBendingSlope() * trackParam->GetBendingSlope());
  v3[6] = TMath::Sqrt(pYZ * pYZ + pZ * pZ * trackParam->GetNonBendingSlope() * trackParam->GetNonBendingSlope()); // PTOT
  v3[5] = -forwardBackward * pZ / v3[6]; // PZ/PTOT spectro. z<0
  v3[3] = trackParam->GetNonBendingSlope() * v3[5]; // PX/PTOT
  v3[4] = trackParam->GetBendingSlope() * v3[5]; // PY/PTOT
}

  //__________________________________________________________________________
void AliMUONTrackExtrap::RecoverTrackParam(Double_t *v3, Double_t charge, AliMUONTrackParam* trackParam)
{
  /// Set track parameters in trackParam from Geant3 parameters pointed to by "v3",
  /// assumed to be calculated for forward motion in Z.
  /// "InverseBendingMomentum" is signed with "charge".
  trackParam->SetNonBendingCoor(v3[0]); // X
  trackParam->SetBendingCoor(v3[1]); // Y
  trackParam->SetZ(v3[2]); // Z
  Double_t pYZ = v3[6] * TMath::Sqrt(1.0 - v3[3] * v3[3]);
  trackParam->SetInverseBendingMomentum(charge/pYZ);
  trackParam->SetBendingSlope(v3[4]/v3[5]);
  trackParam->SetNonBendingSlope(v3[3]/v3[5]);
}

  //__________________________________________________________________________
void AliMUONTrackExtrap::ExtrapToZCov(AliMUONTrackParam* trackParam, Double_t zEnd)
{
  /// Track parameters and their covariances extrapolated to the plane at "zEnd".
  /// On return, results from the extrapolation are updated in trackParam.
  
  if (trackParam->GetZ() == zEnd) return; // nothing to be done if same z
  
  // Save the actual track parameters
  AliMUONTrackParam trackParamSave(*trackParam);
  Double_t nonBendingCoor 	  = trackParamSave.GetNonBendingCoor();
  Double_t nonBendingSlope 	  = trackParamSave.GetNonBendingSlope();
  Double_t bendingCoor 		  = trackParamSave.GetBendingCoor();
  Double_t bendingSlope 	  = trackParamSave.GetBendingSlope();
  Double_t inverseBendingMomentum = trackParamSave.GetInverseBendingMomentum();
  Double_t zBegin		  = trackParamSave.GetZ();
  
  // Extrapolate track parameters to "zEnd"
  ExtrapToZ(trackParam,zEnd);
  Double_t extrapNonBendingCoor 	= trackParam->GetNonBendingCoor();
  Double_t extrapNonBendingSlope 	= trackParam->GetNonBendingSlope();
  Double_t extrapBendingCoor 		= trackParam->GetBendingCoor();
  Double_t extrapBendingSlope		= trackParam->GetBendingSlope();
  Double_t extrapInverseBendingMomentum = trackParam->GetInverseBendingMomentum();
  
  // Get the pointer to the parameter covariance matrix
  if (!trackParam->CovariancesExist()) {
    //cout<<"W-AliMUONTrackExtrap::ExtrapToZCov: track parameter covariance matrix does not exist"<<endl;
    //cout<<"                                    -> nothing to extrapolate !!"<<endl;
    return;
  }
  TMatrixD* paramCov = trackParam->GetCovariances();
  
  // Calculate the jacobian related to the track parameters extrapolation to "zEnd"
  TMatrixD jacob(5,5);
  jacob = 0.;
  Double_t dParam[5];
  for (Int_t i=0; i<5; i++) {
    // Skip jacobian calculation for parameters with no associated error
    if ((*paramCov)(i,i) == 0.) continue;
    // Small variation of parameter i only
    for (Int_t j=0; j<5; j++) {
      if (j==i) {
        dParam[j] = TMath::Sqrt((*paramCov)(i,i));
	if (j == 4) dParam[j] *= TMath::Sign(1.,-inverseBendingMomentum); // variation always in the same direction
      } else dParam[j] = 0.;
    }
    // Set new parameters
    trackParamSave.SetNonBendingCoor	    (nonBendingCoor	    + dParam[0]);
    trackParamSave.SetNonBendingSlope	    (nonBendingSlope	    + dParam[1]);
    trackParamSave.SetBendingCoor	    (bendingCoor	    + dParam[2]);
    trackParamSave.SetBendingSlope	    (bendingSlope	    + dParam[3]);
    trackParamSave.SetInverseBendingMomentum(inverseBendingMomentum + dParam[4]);
    trackParamSave.SetZ			    (zBegin);
    // Extrapolate new track parameters to "zEnd"
    ExtrapToZ(&trackParamSave,zEnd);
    // Calculate the jacobian
    jacob(0,i) = (trackParamSave.GetNonBendingCoor()	     - extrapNonBendingCoor 	   ) / dParam[i];
    jacob(1,i) = (trackParamSave.GetNonBendingSlope()	     - extrapNonBendingSlope 	   ) / dParam[i];
    jacob(2,i) = (trackParamSave.GetBendingCoor()	     - extrapBendingCoor    	   ) / dParam[i];
    jacob(3,i) = (trackParamSave.GetBendingSlope()	     - extrapBendingSlope   	   ) / dParam[i];
    jacob(4,i) = (trackParamSave.GetInverseBendingMomentum() - extrapInverseBendingMomentum) / dParam[i];
  }
  
  // Extrapolate track parameter covariances to "zEnd"
  TMatrixD tmp((*paramCov),TMatrixD::kMultTranspose,jacob);
  (*paramCov) = TMatrixD(jacob,TMatrixD::kMult,tmp);
  
}

  //__________________________________________________________________________
void AliMUONTrackExtrap::ExtrapToStation(AliMUONTrackParam* trackParamIn, Int_t station, AliMUONTrackParam *trackParamOut)
{
  /// Track parameters extrapolated from "trackParamIn" to both chambers of the station(0..) "station"
  /// are returned in the array (dimension 2) of track parameters pointed to by "TrackParamOut"
  /// (index 0 and 1 for first and second chambers).
  Double_t extZ[2], z1, z2;
  Int_t i1 = -1, i2 = -1; // = -1 to avoid compilation warnings
  // range of station to be checked ????
  z1 = AliMUONConstants::DefaultChamberZ(2 * station);
  z2 = AliMUONConstants::DefaultChamberZ(2 * station + 1);
  // First and second Z to extrapolate at
  if ((z1 > trackParamIn->GetZ()) && (z2 > trackParamIn->GetZ())) {i1 = 0; i2 = 1;}
  else if ((z1 < trackParamIn->GetZ()) && (z2 < trackParamIn->GetZ())) {i1 = 1; i2 = 0;}
  else {
    cout<<"E-AliMUONTrackExtrap::ExtrapToStation: Starting Z ("<<trackParamIn->GetZ()
    	<<") in between z1 ("<<z1<<") and z2 ("<<z2<<") of station(0..)"<<station<<endl;
    exit(-1);
  }
  extZ[i1] = z1;
  extZ[i2] = z2;
  // copy of track parameters
  trackParamOut[i1] = *trackParamIn;
  // first extrapolation
  ExtrapToZ(&(trackParamOut[i1]),extZ[0]);
  trackParamOut[i2] = trackParamOut[i1];
  // second extrapolation
  ExtrapToZ(&(trackParamOut[i2]),extZ[1]);
  return;
}

  //__________________________________________________________________________
void AliMUONTrackExtrap::ExtrapToVertexUncorrected(AliMUONTrackParam* trackParam, Double_t zVtx)
{
  /// Extrapolation to the vertex (at the z position "zVtx") without Branson and energy loss corrections.
  /// Returns the track parameters resulting from the extrapolation in the current TrackParam.
  /// Include multiple Coulomb scattering effects in trackParam covariances.
  
  if (trackParam->GetZ() == zVtx) return; // nothing to be done if already at vertex
  
  if (trackParam->GetZ() > zVtx) { // spectro. (z<0)
    cout<<"W-AliMUONTrackExtrap::ExtrapToVertexUncorrected: Starting Z ("<<trackParam->GetZ()
    	<<") upstream the vertex (zVtx = "<<zVtx<<")"<<endl;
    exit(-1);
  }
  
  // Check whether the geometry is available and get absorber boundaries
  if (!gGeoManager) {
    cout<<"E-AliMUONTrackExtrap::ExtrapToVertexUncorrected: no TGeo"<<endl;
    return;
  }
  TGeoNode *absNode = gGeoManager->GetVolume("ALIC")->GetNode("ABSM_1");
  if (!absNode) {
    cout<<"E-AliMUONTrackExtrap::ExtrapToVertexUncorrected: failed to get absorber node"<<endl;
    return;
  }
  Double_t zAbsBeg, zAbsEnd;
  absNode->GetVolume()->GetShape()->GetAxisRange(3,zAbsBeg,zAbsEnd);
  const Double_t *absPos = absNode->GetMatrix()->GetTranslation();
  zAbsBeg = absPos[2] - zAbsBeg; // spectro. (z<0)
  zAbsEnd = absPos[2] - zAbsEnd; // spectro. (z<0)
  
  // Check the vertex position relatively to the absorber
  if (zVtx < zAbsBeg && zVtx > zAbsEnd) { // spectro. (z<0)
    cout<<"W-AliMUONTrackExtrap::ExtrapToVertex: Ending Z ("<<zVtx
    	<<") inside the front absorber ("<<zAbsBeg<<","<<zAbsEnd<<")"<<endl;
  } else if (zVtx < zAbsEnd ) { // spectro. (z<0)
    cout<<"W-AliMUONTrackExtrap::ExtrapToVertex: Ending Z ("<<zVtx
    	<<") downstream the front absorber (zAbsorberEnd = "<<zAbsEnd<<")"<<endl;
    ExtrapToZCov(trackParam,zVtx);
    return;
  }
  
  // Check the track position relatively to the absorber and extrapolate track parameters to the end of the absorber if needed
  if (trackParam->GetZ() > zAbsBeg) { // spectro. (z<0)
    cout<<"W-AliMUONTrackExtrap::ExtrapToVertex: Starting Z ("<<trackParam->GetZ()
    	<<") upstream the front absorber (zAbsorberBegin = "<<zAbsBeg<<")"<<endl;
    ExtrapToZCov(trackParam,zVtx);
    return;
  } else if (trackParam->GetZ() > zAbsEnd) { // spectro. (z<0)
    cout<<"W-AliMUONTrackExtrap::ExtrapToVertex: Starting Z ("<<trackParam->GetZ()
    	<<") inside the front absorber ("<<zAbsBeg<<","<<zAbsEnd<<")"<<endl;
  } else {
    ExtrapToZCov(trackParam,zAbsEnd);
  }
  
  // Then add MCS effect in absorber to the parameters covariances
  AliMUONTrackParam trackParamIn(*trackParam);
  ExtrapToZ(&trackParamIn, TMath::Min(zVtx, zAbsBeg));
  Double_t trackXYZIn[3];
  trackXYZIn[0] = trackParamIn.GetNonBendingCoor();
  trackXYZIn[1] = trackParamIn.GetBendingCoor();
  trackXYZIn[2] = trackParamIn.GetZ();
  Double_t trackXYZOut[3];
  trackXYZOut[0] = trackParam->GetNonBendingCoor();
  trackXYZOut[1] = trackParam->GetBendingCoor();
  trackXYZOut[2] = trackParam->GetZ();
  Double_t pathLength = 0.;
  Double_t f0 = 0.;
  Double_t f1 = 0.;
  Double_t f2 = 0.;
  Double_t meanRho = 0.;
  GetAbsorberCorrectionParam(trackXYZIn,trackXYZOut,pathLength,f0,f1,f2,meanRho);
  AddMCSEffectInAbsorber(trackParam,pathLength,f0,f1,f2);
  
  // finally go to the vertex
  ExtrapToZCov(trackParam,zVtx);
  
}

  //__________________________________________________________________________
void AliMUONTrackExtrap::AddMCSEffectInAbsorber(AliMUONTrackParam* param, Double_t pathLength, Double_t f0, Double_t f1, Double_t f2)
{
  /// Add to the track parameter covariances the effects of multiple Coulomb scattering
  /// at the end of the front absorber using the absorber correction parameters
  
  // absorber related covariance parameters
  Double_t bendingSlope = param->GetBendingSlope();
  Double_t nonBendingSlope = param->GetNonBendingSlope();
  Double_t inverseBendingMomentum = param->GetInverseBendingMomentum();
  Double_t alpha2 = 0.0136 * 0.0136 * inverseBendingMomentum * inverseBendingMomentum * (1.0 + bendingSlope * bendingSlope) /
  					(1.0 + bendingSlope *bendingSlope + nonBendingSlope * nonBendingSlope); // velocity = 1
  Double_t varCoor = alpha2 * (pathLength * pathLength * f0 - 2. * pathLength * f1 + f2);
  Double_t covCorrSlope = alpha2 * (pathLength * f0 - f1);
  Double_t varSlop = alpha2 * f0;
  
  TMatrixD* paramCov = param->GetCovariances();
  // Non bending plane
  (*paramCov)(0,0) += varCoor;		(*paramCov)(0,1) += covCorrSlope;
  (*paramCov)(1,0) += covCorrSlope;	(*paramCov)(1,1) += varSlop;
  // Bending plane
  (*paramCov)(2,2) += varCoor;		(*paramCov)(2,3) += covCorrSlope;
  (*paramCov)(3,2) += covCorrSlope;	(*paramCov)(3,3) += varSlop;
  
}

  //__________________________________________________________________________
void AliMUONTrackExtrap::GetAbsorberCorrectionParam(Double_t trackXYZIn[3], Double_t trackXYZOut[3], Double_t &pathLength,
						    Double_t &f0, Double_t &f1, Double_t &f2, Double_t &meanRho)
{
  /// Parameters used to correct for Multiple Coulomb Scattering and energy loss in absorber
  /// Calculated assuming a linear propagation between track positions trackXYZIn and trackXYZOut
  // pathLength: path length between trackXYZIn and trackXYZOut (cm)
  // f0:         0th moment of z calculated with the inverse radiation-length distribution
  // f1:         1st moment of z calculated with the inverse radiation-length distribution
  // f2:         2nd moment of z calculated with the inverse radiation-length distribution
  // meanRho:    average density of crossed material (g/cm3)
  
  // Reset absorber's parameters
  pathLength = 0.;
  f0 = 0.;
  f1 = 0.;
  f2 = 0.;
  meanRho = 0.;
  
  // Check whether the geometry is available
  if (!gGeoManager) {
    cout<<"E-AliMUONTrackExtrap::GetAbsorberCorrectionParam: no TGeo"<<endl;
    return;
  }
  
  // Initialize starting point and direction
  pathLength = TMath::Sqrt((trackXYZOut[0] - trackXYZIn[0])*(trackXYZOut[0] - trackXYZIn[0])+
			   (trackXYZOut[1] - trackXYZIn[1])*(trackXYZOut[1] - trackXYZIn[1])+
			   (trackXYZOut[2] - trackXYZIn[2])*(trackXYZOut[2] - trackXYZIn[2]));
  if (pathLength < TGeoShape::Tolerance()) return;
  Double_t b[3];
  b[0] = (trackXYZOut[0] - trackXYZIn[0]) / pathLength;
  b[1] = (trackXYZOut[1] - trackXYZIn[1]) / pathLength;
  b[2] = (trackXYZOut[2] - trackXYZIn[2]) / pathLength;
  TGeoNode *currentnode = gGeoManager->InitTrack(trackXYZIn, b);
  if (!currentnode) {
    cout<<"E-AliMUONTrackExtrap::GetAbsorberCorrectionParam: start point out of geometry"<<endl;
    return;
  }
  
  // loop over absorber slices and calculate absorber's parameters
  Double_t rho = 0.; // material density (g/cm3)
  Double_t x0 = 0.;  // radiation-length (cm-1)
  Double_t localPathLength = 0;
  Double_t remainingPathLength = pathLength;
  Double_t zB = trackXYZIn[2];
  Double_t zE, dzB, dzE;
  do {
    // Get material properties
    TGeoMaterial *material = currentnode->GetVolume()->GetMedium()->GetMaterial();
    rho = material->GetDensity();
    x0 = material->GetRadLen();
    if (!material->IsMixture()) x0 /= rho; // different normalization in the modeler for mixture
    
    // Get path length within this material
    gGeoManager->FindNextBoundary(remainingPathLength);
    localPathLength = gGeoManager->GetStep() + 1.e-6;
    // Check if boundary within remaining path length. If so, make sure to cross the boundary to prepare the next step
    if (localPathLength >= remainingPathLength) localPathLength = remainingPathLength;
    else {
      currentnode = gGeoManager->Step();
      if (!currentnode) {
        cout<<"E-AliMUONTrackExtrap::GetAbsorberCorrectionParam: navigation failed"<<endl;
	f0 = f1 = f2 = meanRho = 0.;
	return;
      }
      if (!gGeoManager->IsEntering()) {
        // make another small step to try to enter in new absorber slice
        gGeoManager->SetStep(0.001);
	currentnode = gGeoManager->Step();
	if (!gGeoManager->IsEntering() || !currentnode) {
          cout<<"E-AliMUONTrackExtrap::GetAbsorberCorrectionParam: navigation failed"<<endl;
	  f0 = f1 = f2 = meanRho = 0.;
	  return;
	}
        localPathLength += 0.001;
      }
    }
    
    // calculate absorber's parameters
    zE = b[2] * localPathLength + zB;
    dzB = zB - trackXYZIn[2];
    dzE = zE - trackXYZIn[2];
    f0 += localPathLength / x0;
    f1 += (dzE*dzE - dzB*dzB) / b[2] / b[2] / x0 / 2.;
    f2 += (dzE*dzE*dzE - dzB*dzB*dzB) / b[2] / b[2] / b[2] / x0 / 3.;
    meanRho += localPathLength * rho;
    
    // prepare next step
    zB = zE;
    remainingPathLength -= localPathLength;
  } while (remainingPathLength > TGeoShape::Tolerance());
  
  meanRho /= pathLength;
}

  //__________________________________________________________________________
void AliMUONTrackExtrap::AddMCSEffect(AliMUONTrackParam *param, Double_t dZ, Double_t x0)
{
  /// Add to the track parameter covariances the effects of multiple Coulomb scattering
  /// through a material of thickness "dZ" and of radiation length "x0"
  /// assuming linear propagation and using the small angle approximation.
  
  Double_t bendingSlope = param->GetBendingSlope();
  Double_t nonBendingSlope = param->GetNonBendingSlope();
  Double_t inverseTotalMomentum2 = param->GetInverseBendingMomentum() * param->GetInverseBendingMomentum() *
  				   (1.0 + bendingSlope * bendingSlope) /
			  	   (1.0 + bendingSlope *bendingSlope + nonBendingSlope * nonBendingSlope); 
  // Path length in the material
  Double_t pathLength = TMath::Abs(dZ) * TMath::Sqrt(1.0 + bendingSlope*bendingSlope + nonBendingSlope*nonBendingSlope);
  Double_t pathLength2 = pathLength * pathLength;
  // relativistic velocity
  Double_t velo = 1.;
  // Angular dispersion square of the track (variance) in a plane perpendicular to the trajectory
  Double_t theta02 = 0.0136 / velo * (1 + 0.038 * TMath::Log(pathLength/x0));
  theta02 *= theta02 * inverseTotalMomentum2 * pathLength / x0;
  
  // Add effects of multiple Coulomb scattering in track parameter covariances
  TMatrixD* paramCov = param->GetCovariances();
  Double_t varCoor 	= pathLength2 * theta02 / 3.;
  Double_t varSlop 	= theta02;
  Double_t covCorrSlope = pathLength * theta02 / 2.;
  // Non bending plane
  (*paramCov)(0,0) += varCoor;		(*paramCov)(0,1) += covCorrSlope;
  (*paramCov)(1,0) += covCorrSlope;	(*paramCov)(1,1) += varSlop;
  // Bending plane
  (*paramCov)(2,2) += varCoor;		(*paramCov)(2,3) += covCorrSlope;
  (*paramCov)(3,2) += covCorrSlope;	(*paramCov)(3,3) += varSlop;
  
}

  //__________________________________________________________________________
void AliMUONTrackExtrap::ExtrapToVertex(AliMUONTrackParam* trackParam, Double_t xVtx, Double_t yVtx, Double_t zVtx,
					Bool_t CorrectForMCS, Bool_t CorrectForEnergyLoss)
{
  /// Extrapolation to the vertex.
  /// Returns the track parameters resulting from the extrapolation of the current TrackParam.
  /// Changes parameters according to Branson correction through the absorber and energy loss
  
  if (trackParam->GetZ() == zVtx) return; // nothing to be done if already at vertex
  
  if (trackParam->GetZ() > zVtx) { // spectro. (z<0)
    cout<<"F-AliMUONTrackExtrap::ExtrapToVertex: Starting Z ("<<trackParam->GetZ()
    	<<") upstream the vertex (zVtx = "<<zVtx<<")"<<endl;
    exit(-1);
  }
  
  // Check if correction required
  if (!CorrectForMCS && !CorrectForEnergyLoss) {
    ExtrapToZ(trackParam,zVtx);
    return;
  }
  
  // Check whether the geometry is available and get absorber boundaries
  if (!gGeoManager) {
    cout<<"E-AliMUONTrackExtrap::ExtrapToVertex: no TGeo"<<endl;
    return;
  }
  TGeoNode *absNode = gGeoManager->GetVolume("ALIC")->GetNode("ABSM_1");
  if (!absNode) {
    cout<<"E-AliMUONTrackExtrap::ExtrapToVertex: failed to get absorber node"<<endl;
    return;
  }
  Double_t zAbsBeg, zAbsEnd;
  absNode->GetVolume()->GetShape()->GetAxisRange(3,zAbsBeg,zAbsEnd);
  const Double_t *absPos = absNode->GetMatrix()->GetTranslation();
  zAbsBeg = absPos[2] - zAbsBeg; // spectro. (z<0)
  zAbsEnd = absPos[2] - zAbsEnd; // spectro. (z<0)
  
  // Check the vertex position relatively to the absorber
  if (zVtx < zAbsBeg && zVtx > zAbsEnd) { // spectro. (z<0)
    cout<<"W-AliMUONTrackExtrap::ExtrapToVertex: Ending Z ("<<zVtx
    	<<") inside the front absorber ("<<zAbsBeg<<","<<zAbsEnd<<")"<<endl;
  } else if (zVtx < zAbsEnd ) { // spectro. (z<0)
    cout<<"W-AliMUONTrackExtrap::ExtrapToVertex: Ending Z ("<<zVtx
    	<<") downstream the front absorber (zAbsorberEnd = "<<zAbsEnd<<")"<<endl;
    ExtrapToZ(trackParam,zVtx);
    return;
  }
  
  // Check the track position relatively to the absorber and extrapolate track parameters to the end of the absorber if needed
  if (trackParam->GetZ() > zAbsBeg) { // spectro. (z<0)
    cout<<"W-AliMUONTrackExtrap::ExtrapToVertex: Starting Z ("<<trackParam->GetZ()
    	<<") upstream the front absorber (zAbsorberBegin = "<<zAbsBeg<<")"<<endl;
    ExtrapToZ(trackParam,zVtx);
    return;
  } else if (trackParam->GetZ() > zAbsEnd) { // spectro. (z<0)
    cout<<"W-AliMUONTrackExtrap::ExtrapToVertex: Starting Z ("<<trackParam->GetZ()
    	<<") inside the front absorber ("<<zAbsBeg<<","<<zAbsEnd<<")"<<endl;
  } else {
    ExtrapToZ(trackParam,zAbsEnd);
  }
  
  // Get absorber correction parameters assuming linear propagation from vertex to the track position
  Double_t trackXYZOut[3];
  trackXYZOut[0] = trackParam->GetNonBendingCoor();
  trackXYZOut[1] = trackParam->GetBendingCoor();
  trackXYZOut[2] = trackParam->GetZ();
  Double_t trackXYZIn[3];
  trackXYZIn[2] = TMath::Min(zVtx, zAbsBeg); // spectro. (z<0)
  trackXYZIn[0] = trackXYZOut[0] + (xVtx - trackXYZOut[0]) / (zVtx - trackXYZOut[2]) * (trackXYZIn[2] - trackXYZOut[2]);
  trackXYZIn[1] = trackXYZOut[1] + (yVtx - trackXYZOut[1]) / (zVtx - trackXYZOut[2]) * (trackXYZIn[2] - trackXYZOut[2]);
  Double_t pathLength = 0.;
  Double_t f0 = 0.;
  Double_t f1 = 0.;
  Double_t f2 = 0.;
  Double_t meanRho = 0.;
  GetAbsorberCorrectionParam(trackXYZIn,trackXYZOut,pathLength,f0,f1,f2,meanRho);
  
  // Calculate energy loss
  Double_t pTot = trackParam->P();
  Double_t charge = TMath::Sign(Double_t(1.0), trackParam->GetInverseBendingMomentum());      
  Double_t deltaP = TotalMomentumEnergyLoss(pTot,pathLength,meanRho);
  
  // Correct for half of energy loss
  Double_t nonBendingSlope, bendingSlope;
  if (CorrectForEnergyLoss) {
    pTot += 0.5 * deltaP;
    nonBendingSlope = trackParam->GetNonBendingSlope();
    bendingSlope = trackParam->GetBendingSlope();
    trackParam->SetInverseBendingMomentum(charge / pTot *
  	  TMath::Sqrt(1.0 + nonBendingSlope*nonBendingSlope + bendingSlope*bendingSlope) /
  	  TMath::Sqrt(1.0 + bendingSlope*bendingSlope));
  }
  
  if (CorrectForMCS) {
    // Position of the Branson plane (spectro. (z<0))
    Double_t zB = (f1>0.) ? trackXYZIn[2] - f2/f1 : 0.;
    
    // Get track position in the Branson plane corrected for magnetic field effect
    ExtrapToZ(trackParam,zVtx);
    Double_t xB = trackParam->GetNonBendingCoor() + (zB - zVtx) * trackParam->GetNonBendingSlope();
    Double_t yB = trackParam->GetBendingCoor()    + (zB - zVtx) * trackParam->GetBendingSlope();
    
    // Get track slopes corrected for multiple scattering (spectro. (z<0))
    nonBendingSlope = (zB<0.) ? (xB - xVtx) / (zB - zVtx) : trackParam->GetNonBendingSlope();
    bendingSlope    = (zB<0.) ? (yB - yVtx) / (zB - zVtx) : trackParam->GetBendingSlope();
    
    // Set track parameters at vertex
    trackParam->SetNonBendingCoor(xVtx);
    trackParam->SetBendingCoor(yVtx);
    trackParam->SetZ(zVtx);
    trackParam->SetNonBendingSlope(nonBendingSlope);
    trackParam->SetBendingSlope(bendingSlope);
  } else {
    ExtrapToZ(trackParam,zVtx);
    nonBendingSlope = trackParam->GetNonBendingSlope();
    bendingSlope = trackParam->GetBendingSlope();
  }
  
  // Correct for second half of energy loss
  if (CorrectForEnergyLoss) pTot += 0.5 * deltaP;
  
  // Set track parameters at vertex
  trackParam->SetInverseBendingMomentum(charge / pTot *
        TMath::Sqrt(1.0 + nonBendingSlope*nonBendingSlope + bendingSlope*bendingSlope) /
        TMath::Sqrt(1.0 + bendingSlope*bendingSlope));
  
}

  //__________________________________________________________________________
Double_t AliMUONTrackExtrap::TotalMomentumEnergyLoss(AliMUONTrackParam* trackParam, Double_t xVtx, Double_t yVtx, Double_t zVtx)
{
  /// Calculate the total momentum energy loss in-between the track position and the vertex assuming a linear propagation
  
  if (trackParam->GetZ() == zVtx) return 0.; // nothing to be done if already at vertex
  
  // Check whether the geometry is available
  if (!gGeoManager) {
    cout<<"E-AliMUONTrackExtrap::TotalMomentumEnergyLoss: no TGeo"<<endl;
    return 0.;
  }
  
  // Get encountered material correction parameters assuming linear propagation from vertex to the track position
  Double_t trackXYZOut[3];
  trackXYZOut[0] = trackParam->GetNonBendingCoor();
  trackXYZOut[1] = trackParam->GetBendingCoor();
  trackXYZOut[2] = trackParam->GetZ();
  Double_t trackXYZIn[3];
  trackXYZIn[0] = xVtx;
  trackXYZIn[1] = yVtx;
  trackXYZIn[2] = zVtx;
  Double_t pathLength = 0.;
  Double_t f0 = 0.;
  Double_t f1 = 0.;
  Double_t f2 = 0.;
  Double_t meanRho = 0.;
  GetAbsorberCorrectionParam(trackXYZIn,trackXYZOut,pathLength,f0,f1,f2,meanRho);
  
  // Calculate energy loss
  Double_t pTot = trackParam->P();
  return TotalMomentumEnergyLoss(pTot,pathLength,meanRho);
}

  //__________________________________________________________________________
Double_t AliMUONTrackExtrap::TotalMomentumEnergyLoss(Double_t pTotal, Double_t pathLength, Double_t rho)
{
  /// Returns the total momentum energy loss in the front absorber
  Double_t muMass = 0.105658369;
  Double_t p2=pTotal*pTotal;
  Double_t beta2=p2/(p2 + muMass*muMass);
  Double_t dE=ApproximateBetheBloch(beta2)*pathLength*rho;
  
  return dE;
}

  //__________________________________________________________________________
Double_t AliMUONTrackExtrap::ApproximateBetheBloch(Double_t beta2) 
{
/// This is an approximation of the Bethe-Bloch formula with 
/// the density effect taken into account at beta*gamma > 3.5
/// (the approximation is reasonable only for solid materials) 

  if (beta2/(1-beta2)>3.5*3.5)
     return 0.153e-3/beta2*(log(3.5*5940)+0.5*log(beta2/(1-beta2)) - beta2);

  return 0.153e-3/beta2*(log(5940*beta2/(1-beta2)) - beta2);
}

 //__________________________________________________________________________
void AliMUONTrackExtrap::ExtrapOneStepHelix(Double_t charge, Double_t step, Double_t *vect, Double_t *vout)
{
/// <pre>
///    ******************************************************************
///    *                                                                *
///    *  Performs the tracking of one step in a magnetic field         *
///    *  The trajectory is assumed to be a helix in a constant field   *
///    *  taken at the mid point of the step.                           *
///    *  Parameters:                                                   *
///    *   input                                                        *
///    *     STEP =arc length of the step asked                         *
///    *     VECT =input vector (position,direction cos and momentum)   *
///    *     CHARGE=  electric charge of the particle                   *
///    *   output                                                       *
///    *     VOUT = same as VECT after completion of the step           *
///    *                                                                *
///    *    ==>Called by : <USER>, GUSWIM                               *
///    *       Author    m.hansroul  *********                          *
///    *       modified  s.egli, s.v.levonian                           *
///    *       modified  v.perevoztchikov
///    *                                                                *
///    ******************************************************************
/// </pre>

// modif: everything in double precision

    Double_t xyz[3], h[4], hxp[3];
    Double_t h2xy, hp, rho, tet;
    Double_t sint, sintt, tsint, cos1t;
    Double_t f1, f2, f3, f4, f5, f6;

    const Int_t kix  = 0;
    const Int_t kiy  = 1;
    const Int_t kiz  = 2;
    const Int_t kipx = 3;
    const Int_t kipy = 4;
    const Int_t kipz = 5;
    const Int_t kipp = 6;

    const Double_t kec = 2.9979251e-4;
    //
    //    ------------------------------------------------------------------
    //
    //       units are kgauss,centimeters,gev/c
    //
    vout[kipp] = vect[kipp];
    if (TMath::Abs(charge) < 0.00001) {
      for (Int_t i = 0; i < 3; i++) {
	vout[i] = vect[i] + step * vect[i+3];
	vout[i+3] = vect[i+3];
      }
      return;
    }
    xyz[0]    = vect[kix] + 0.5 * step * vect[kipx];
    xyz[1]    = vect[kiy] + 0.5 * step * vect[kipy];
    xyz[2]    = vect[kiz] + 0.5 * step * vect[kipz];

    //cmodif: call gufld (xyz, h) changed into:
    GetField (xyz, h);
 
    h2xy = h[0]*h[0] + h[1]*h[1];
    h[3] = h[2]*h[2]+ h2xy;
    if (h[3] < 1.e-12) {
      for (Int_t i = 0; i < 3; i++) {
	vout[i] = vect[i] + step * vect[i+3];
	vout[i+3] = vect[i+3];
      }
      return;
    }
    if (h2xy < 1.e-12*h[3]) {
      ExtrapOneStepHelix3(charge*h[2], step, vect, vout);
      return;
    }
    h[3] = TMath::Sqrt(h[3]);
    h[0] /= h[3];
    h[1] /= h[3];
    h[2] /= h[3];
    h[3] *= kec;

    hxp[0] = h[1]*vect[kipz] - h[2]*vect[kipy];
    hxp[1] = h[2]*vect[kipx] - h[0]*vect[kipz];
    hxp[2] = h[0]*vect[kipy] - h[1]*vect[kipx];
 
    hp = h[0]*vect[kipx] + h[1]*vect[kipy] + h[2]*vect[kipz];

    rho = -charge*h[3]/vect[kipp];
    tet = rho * step;

    if (TMath::Abs(tet) > 0.15) {
      sint = TMath::Sin(tet);
      sintt = (sint/tet);
      tsint = (tet-sint)/tet;
      cos1t = 2.*(TMath::Sin(0.5*tet))*(TMath::Sin(0.5*tet))/tet;
    } else {
      tsint = tet*tet/36.;
      sintt = (1. - tsint);
      sint = tet*sintt;
      cos1t = 0.5*tet;
    }

    f1 = step * sintt;
    f2 = step * cos1t;
    f3 = step * tsint * hp;
    f4 = -tet*cos1t;
    f5 = sint;
    f6 = tet * cos1t * hp;
 
    vout[kix] = vect[kix] + f1*vect[kipx] + f2*hxp[0] + f3*h[0];
    vout[kiy] = vect[kiy] + f1*vect[kipy] + f2*hxp[1] + f3*h[1];
    vout[kiz] = vect[kiz] + f1*vect[kipz] + f2*hxp[2] + f3*h[2];
 
    vout[kipx] = vect[kipx] + f4*vect[kipx] + f5*hxp[0] + f6*h[0];
    vout[kipy] = vect[kipy] + f4*vect[kipy] + f5*hxp[1] + f6*h[1];
    vout[kipz] = vect[kipz] + f4*vect[kipz] + f5*hxp[2] + f6*h[2];
 
    return;
}

 //__________________________________________________________________________
void AliMUONTrackExtrap::ExtrapOneStepHelix3(Double_t field, Double_t step, Double_t *vect, Double_t *vout)
{
/// <pre>
///	******************************************************************
///	*								 *
///	*	Tracking routine in a constant field oriented		 *
///	*	along axis 3						 *
///	*	Tracking is performed with a conventional		 *
///	*	helix step method					 *
///	*								 *
///	*    ==>Called by : <USER>, GUSWIM				 *
///	*	Authors    R.Brun, M.Hansroul  *********		 *
///	*	Rewritten  V.Perevoztchikov
///	*								 *
///	******************************************************************
/// </pre>

    Double_t hxp[3];
    Double_t h4, hp, rho, tet;
    Double_t sint, sintt, tsint, cos1t;
    Double_t f1, f2, f3, f4, f5, f6;

    const Int_t kix  = 0;
    const Int_t kiy  = 1;
    const Int_t kiz  = 2;
    const Int_t kipx = 3;
    const Int_t kipy = 4;
    const Int_t kipz = 5;
    const Int_t kipp = 6;

    const Double_t kec = 2.9979251e-4;

// 
//     ------------------------------------------------------------------
// 
//       units are kgauss,centimeters,gev/c
// 
    vout[kipp] = vect[kipp];
    h4 = field * kec;

    hxp[0] = - vect[kipy];
    hxp[1] = + vect[kipx];
 
    hp = vect[kipz];

    rho = -h4/vect[kipp];
    tet = rho * step;
    if (TMath::Abs(tet) > 0.15) {
      sint = TMath::Sin(tet);
      sintt = (sint/tet);
      tsint = (tet-sint)/tet;
      cos1t = 2.* TMath::Sin(0.5*tet) * TMath::Sin(0.5*tet)/tet;
    } else {
      tsint = tet*tet/36.;
      sintt = (1. - tsint);
      sint = tet*sintt;
      cos1t = 0.5*tet;
    }

    f1 = step * sintt;
    f2 = step * cos1t;
    f3 = step * tsint * hp;
    f4 = -tet*cos1t;
    f5 = sint;
    f6 = tet * cos1t * hp;
 
    vout[kix] = vect[kix] + f1*vect[kipx] + f2*hxp[0];
    vout[kiy] = vect[kiy] + f1*vect[kipy] + f2*hxp[1];
    vout[kiz] = vect[kiz] + f1*vect[kipz] + f3;
 
    vout[kipx] = vect[kipx] + f4*vect[kipx] + f5*hxp[0];
    vout[kipy] = vect[kipy] + f4*vect[kipy] + f5*hxp[1];
    vout[kipz] = vect[kipz] + f4*vect[kipz] + f6;

    return;
}

 //__________________________________________________________________________
void AliMUONTrackExtrap::ExtrapOneStepRungekutta(Double_t charge, Double_t step, Double_t* vect, Double_t* vout)
{
/// <pre>
///	******************************************************************
///	*								 *
///	*  Runge-Kutta method for tracking a particle through a magnetic *
///	*  field. Uses Nystroem algorithm (See Handbook Nat. Bur. of	 *
///	*  Standards, procedure 25.5.20)				 *
///	*								 *
///	*  Input parameters						 *
///	*	CHARGE    Particle charge				 *
///	*	STEP	  Step size					 *
///	*	VECT	  Initial co-ords,direction cosines,momentum	 *
///	*  Output parameters						 *
///	*	VOUT	  Output co-ords,direction cosines,momentum	 *
///	*  User routine called  					 *
///	*	CALL GUFLD(X,F) 					 *
///	*								 *
///	*    ==>Called by : <USER>, GUSWIM				 *
///	*	Authors    R.Brun, M.Hansroul  *********		 *
///	*		   V.Perevoztchikov (CUT STEP implementation)	 *
///	*								 *
///	*								 *
///	******************************************************************
/// </pre>

    Double_t h2, h4, f[4];
    Double_t xyzt[3], a, b, c, ph,ph2;
    Double_t secxs[4],secys[4],seczs[4],hxp[3];
    Double_t g1, g2, g3, g4, g5, g6, ang2, dxt, dyt, dzt;
    Double_t est, at, bt, ct, cba;
    Double_t f1, f2, f3, f4, rho, tet, hnorm, hp, rho1, sint, cost;
    
    Double_t x;
    Double_t y;
    Double_t z;
    
    Double_t xt;
    Double_t yt;
    Double_t zt;

    Double_t maxit = 1992;
    Double_t maxcut = 11;

    const Double_t kdlt   = 1e-4;
    const Double_t kdlt32 = kdlt/32.;
    const Double_t kthird = 1./3.;
    const Double_t khalf  = 0.5;
    const Double_t kec = 2.9979251e-4;

    const Double_t kpisqua = 9.86960440109;
    const Int_t kix  = 0;
    const Int_t kiy  = 1;
    const Int_t kiz  = 2;
    const Int_t kipx = 3;
    const Int_t kipy = 4;
    const Int_t kipz = 5;
  
    // *.
    // *.    ------------------------------------------------------------------
    // *.
    // *             this constant is for units cm,gev/c and kgauss
    // *
    Int_t iter = 0;
    Int_t ncut = 0;
    for(Int_t j = 0; j < 7; j++)
      vout[j] = vect[j];

    Double_t  pinv   = kec * charge / vect[6];
    Double_t tl = 0.;
    Double_t h = step;
    Double_t rest;

 
    do {
      rest  = step - tl;
      if (TMath::Abs(h) > TMath::Abs(rest)) h = rest;
      //cmodif: call gufld(vout,f) changed into:

      GetField(vout,f);

      // *
      // *             start of integration
      // *
      x      = vout[0];
      y      = vout[1];
      z      = vout[2];
      a      = vout[3];
      b      = vout[4];
      c      = vout[5];

      h2     = khalf * h;
      h4     = khalf * h2;
      ph     = pinv * h;
      ph2    = khalf * ph;
      secxs[0] = (b * f[2] - c * f[1]) * ph2;
      secys[0] = (c * f[0] - a * f[2]) * ph2;
      seczs[0] = (a * f[1] - b * f[0]) * ph2;
      ang2 = (secxs[0]*secxs[0] + secys[0]*secys[0] + seczs[0]*seczs[0]);
      if (ang2 > kpisqua) break;

      dxt    = h2 * a + h4 * secxs[0];
      dyt    = h2 * b + h4 * secys[0];
      dzt    = h2 * c + h4 * seczs[0];
      xt     = x + dxt;
      yt     = y + dyt;
      zt     = z + dzt;
      // *
      // *              second intermediate point
      // *

      est = TMath::Abs(dxt) + TMath::Abs(dyt) + TMath::Abs(dzt);
      if (est > h) {
	if (ncut++ > maxcut) break;
	h *= khalf;
	continue;
      }
 
      xyzt[0] = xt;
      xyzt[1] = yt;
      xyzt[2] = zt;

      //cmodif: call gufld(xyzt,f) changed into:
      GetField(xyzt,f);

      at     = a + secxs[0];
      bt     = b + secys[0];
      ct     = c + seczs[0];

      secxs[1] = (bt * f[2] - ct * f[1]) * ph2;
      secys[1] = (ct * f[0] - at * f[2]) * ph2;
      seczs[1] = (at * f[1] - bt * f[0]) * ph2;
      at     = a + secxs[1];
      bt     = b + secys[1];
      ct     = c + seczs[1];
      secxs[2] = (bt * f[2] - ct * f[1]) * ph2;
      secys[2] = (ct * f[0] - at * f[2]) * ph2;
      seczs[2] = (at * f[1] - bt * f[0]) * ph2;
      dxt    = h * (a + secxs[2]);
      dyt    = h * (b + secys[2]);
      dzt    = h * (c + seczs[2]);
      xt     = x + dxt;
      yt     = y + dyt;
      zt     = z + dzt;
      at     = a + 2.*secxs[2];
      bt     = b + 2.*secys[2];
      ct     = c + 2.*seczs[2];

      est = TMath::Abs(dxt)+TMath::Abs(dyt)+TMath::Abs(dzt);
      if (est > 2.*TMath::Abs(h)) {
	if (ncut++ > maxcut) break;
	h *= khalf;
	continue;
      }
 
      xyzt[0] = xt;
      xyzt[1] = yt;
      xyzt[2] = zt;

      //cmodif: call gufld(xyzt,f) changed into:
      GetField(xyzt,f);

      z      = z + (c + (seczs[0] + seczs[1] + seczs[2]) * kthird) * h;
      y      = y + (b + (secys[0] + secys[1] + secys[2]) * kthird) * h;
      x      = x + (a + (secxs[0] + secxs[1] + secxs[2]) * kthird) * h;

      secxs[3] = (bt*f[2] - ct*f[1])* ph2;
      secys[3] = (ct*f[0] - at*f[2])* ph2;
      seczs[3] = (at*f[1] - bt*f[0])* ph2;
      a      = a+(secxs[0]+secxs[3]+2. * (secxs[1]+secxs[2])) * kthird;
      b      = b+(secys[0]+secys[3]+2. * (secys[1]+secys[2])) * kthird;
      c      = c+(seczs[0]+seczs[3]+2. * (seczs[1]+seczs[2])) * kthird;

      est    = TMath::Abs(secxs[0]+secxs[3] - (secxs[1]+secxs[2]))
	+ TMath::Abs(secys[0]+secys[3] - (secys[1]+secys[2]))
	+ TMath::Abs(seczs[0]+seczs[3] - (seczs[1]+seczs[2]));

      if (est > kdlt && TMath::Abs(h) > 1.e-4) {
	if (ncut++ > maxcut) break;
	h *= khalf;
	continue;
      }

      ncut = 0;
      // *               if too many iterations, go to helix
      if (iter++ > maxit) break;

      tl += h;
      if (est < kdlt32) 
	h *= 2.;
      cba    = 1./ TMath::Sqrt(a*a + b*b + c*c);
      vout[0] = x;
      vout[1] = y;
      vout[2] = z;
      vout[3] = cba*a;
      vout[4] = cba*b;
      vout[5] = cba*c;
      rest = step - tl;
      if (step < 0.) rest = -rest;
      if (rest < 1.e-5*TMath::Abs(step)) return;

    } while(1);

    // angle too big, use helix

    f1  = f[0];
    f2  = f[1];
    f3  = f[2];
    f4  = TMath::Sqrt(f1*f1+f2*f2+f3*f3);
    rho = -f4*pinv;
    tet = rho * step;
 
    hnorm = 1./f4;
    f1 = f1*hnorm;
    f2 = f2*hnorm;
    f3 = f3*hnorm;

    hxp[0] = f2*vect[kipz] - f3*vect[kipy];
    hxp[1] = f3*vect[kipx] - f1*vect[kipz];
    hxp[2] = f1*vect[kipy] - f2*vect[kipx];
 
    hp = f1*vect[kipx] + f2*vect[kipy] + f3*vect[kipz];

    rho1 = 1./rho;
    sint = TMath::Sin(tet);
    cost = 2.*TMath::Sin(khalf*tet)*TMath::Sin(khalf*tet);

    g1 = sint*rho1;
    g2 = cost*rho1;
    g3 = (tet-sint) * hp*rho1;
    g4 = -cost;
    g5 = sint;
    g6 = cost * hp;
 
    vout[kix] = vect[kix] + g1*vect[kipx] + g2*hxp[0] + g3*f1;
    vout[kiy] = vect[kiy] + g1*vect[kipy] + g2*hxp[1] + g3*f2;
    vout[kiz] = vect[kiz] + g1*vect[kipz] + g2*hxp[2] + g3*f3;
 
    vout[kipx] = vect[kipx] + g4*vect[kipx] + g5*hxp[0] + g6*f1;
    vout[kipy] = vect[kipy] + g4*vect[kipy] + g5*hxp[1] + g6*f2;
    vout[kipz] = vect[kipz] + g4*vect[kipz] + g5*hxp[2] + g6*f3;

    return;
}

//___________________________________________________________
 void  AliMUONTrackExtrap::GetField(Double_t *Position, Double_t *Field)
{
  /// interface for arguments in double precision (Why ? ChF)
  Float_t x[3], b[3];

  x[0] = Position[0]; x[1] = Position[1]; x[2] = Position[2];

  if (fgkField) fgkField->Field(x,b);
  else {
    cout<<"F-AliMUONTrackExtrap::GetField: fgkField = 0x0"<<endl;
    exit(-1);
  }
  
  Field[0] = b[0]; Field[1] = b[1]; Field[2] = b[2];

  return;
}

#ifndef ALIHLTMUONFULLTRACKER_H
#define ALIHLTMUONFULLTRACKER_H
/* This file is property of and copyright by the ALICE HLT Project        *
 * ALICE Experiment at CERN, All rights reserved.                         *
 * See cxx source for full Copyright notice                               */
/**********************************************************************
// Created on : 08/12/2009
// Purpose    : First version implementation of the Full tracker for dHLT.
// Author     : Indranil Das, HEP Division, SINP
// Email      : indra.das@saha.ac.in | indra.ehep@gmail.com
**********************************************************************/

#include "AliHLTLogging.h"
#include "TMatrixD.h"

class AliHLTMUONConstants;
class AliHLTMUONMansoTrackStruct;
class AliHLTMUONTriggerRecordStruct;
class AliHLTMUONRecHitStruct;
class AliMUONGeometryTransformer;
class AliMUONTrackParam;

class AliHLTMUONFullTracker : public AliHLTLogging
{

 public :
  ///Constructor
  AliHLTMUONFullTracker() ;
  ///Destructor
  ~AliHLTMUONFullTracker();

  ///Print message
  void Print();
  ///Set the input of trigrec blocks
  Bool_t SetInput(AliHLTInt32_t ddl, const AliHLTMUONTriggerRecordStruct  *data, AliHLTInt32_t size);
  ///Set the input of rechit blocks
  Bool_t SetInput(AliHLTInt32_t ddl, const AliHLTMUONRecHitStruct  *data, AliHLTInt32_t size);
  ///Main method to run and compute the tracks
  Bool_t Run(int iEvent,AliHLTMUONMansoTrackStruct *data, AliHLTUInt32_t& size);
  ///To be called once from DoInit method of component
  Bool_t Init();
  
 protected:

  /// copy constructor
  AliHLTMUONFullTracker(const AliHLTMUONFullTracker& rhs); 
  /// assignment operator
  AliHLTMUONFullTracker& operator=(const AliHLTMUONFullTracker& rhs); 

 private :

  /// intger pair needed for QuadTrackSeg method
  struct IntPair{
    Int_t fFirst,fSecond;  /// First and second element of the pair.
  };
  ///Structure for internal track segments
  struct TrackSeg{
    Int_t fIndex[4];    /// Index of the chamber rec hit point in fChPoint.
    AliHLTInt32_t fTrigRec;  /// The corresponding trigger record ID.
  };

  ///Sructure for clusters
  struct Cluster{
    Float_t fX,fY,fZ;       /// Cluster point X, Y, Z.
    Float_t fErrX2,fErrY2;  /// The square of the uncertainty (variance) on X and Y.
  };

  
  static const Float_t fgkTrackDetCoordinate[3]; /// set the constant value for third station position and size
  
  static const Double_t fgkAbsoedge[4] ;     /// edge of the absorber
  static const Double_t fgkRadLen[3] ;       /// radiation length of the main three matirials of the front absorber
  static const Double_t fgkRho[3] ;          /// density of the main three matirials of the front absorber
  static const Double_t fgkAtomicZ[3] ;      /// atomic number the main three matirials of the front absorber
  static const Double_t fgkAtomicA[3] ;      /// atomic mass of the main three matirials of the front absorber

  static const Int_t fgkMaxNofCellsPerCh ;      /// maximum number of cell are allowed to create
  static const Int_t fgkMaxNofPointsPerCh ;     /// maximim number of points per chamber
  static const Int_t fgkMaxNofCh ;              /// maximum number of chambrs
  static const Int_t fgkMaxNofTracks;           /// maximum number of allowed tracks
  static const Int_t fgkMaxNofConnectedTracks;  /// maximum number of back to front connected tracks
  
  AliMUONGeometryTransformer *fChamberGeometryTransformer;   /// Pointer to AliMUONGeometryTransformer
  
  AliHLTMUONRecHitStruct ***fChPoint;  /// array of pointer to rechit data
  AliHLTMUONTriggerRecordStruct **fChPoint11;   ///array of pointer to trigrec data
  TrackSeg *fBackTrackSeg;  /// track segments at the rear part of the spectrometer
  TrackSeg *fFrontTrackSeg;  /// track segments close the part of interaction point  of ALICE
  
  Float_t *fExtrapSt3X ;  /// Extrapolated x position in third station
  Float_t *fExtrapSt3Y ;  /// Extrapolated y position in third station
  Float_t *fInclinationBack;  /// values of inclination angle of back track segments

  Int_t *fNofConnectedfrontTrackSeg ;  /// nof connected tracks in front direction for each back track segments
  Int_t **fBackToFront;  /// Pointer to back to front segment mapping
  Float_t *fCharge;  /// Charge of the tracks
  Int_t *fNofPoints ;  /// Number of points for each stations
  AliMUONTrackParam *fTrackParam ;  /// track parameters;

  Int_t fTotNofPoints;  /// Total number of points received from all rechit source
  Int_t fTotTrackSeg;  /// Total number of track segments
  Int_t fNofCells[2];  /// Number of cells per station in QuadSeg
  Bool_t fOverflowed;  /// Check if overflowed
  Int_t fNofbackTrackSeg;  /// number of back track segments
  Int_t fNoffrontTrackSeg;  /// number of front track segments
  Int_t fNofConnected ;  /// number of connected track segments

  /// Slat Track segments 
  Bool_t SlatTrackSeg();
  /// Quad Track segments 
  Bool_t QuadTrackSeg();
  /// Kalman Chi2 test
  Bool_t KalmanChi2Test();
  /// track extrapolation through  dipole magnet to connect front and back track seg
  Bool_t SelectFront();
  /// Propagate tracks
  void PropagateTracks(Double_t charge, Float_t& px, Float_t& py, Float_t& pz,
		       Float_t& xr, Float_t& yr, Float_t& zr, Float_t zprop) const;
  /// extrapolate to origin
  Bool_t ExtrapolateToOrigin(Bool_t extrap);
  /// Clean after each run
  Bool_t Clear();


  /// Angle calculate
  Double_t Angle(const AliHLTMUONRecHitStruct *v1, const AliHLTMUONRecHitStruct *v2) const;
  /// Subtracktion of two point
  void Sub(const AliHLTMUONRecHitStruct *v1, const AliHLTMUONRecHitStruct *v2, AliHLTMUONRecHitStruct *v3) const;
  /// Kalman Filter
  Double_t KalmanFilter(AliMUONTrackParam &trackParamAtCluster, Cluster *cluster);
  /// Try onecluster
  Double_t TryOneCluster(const AliMUONTrackParam &trackParam, Cluster* cluster,
				AliMUONTrackParam &trackParamAtCluster, Bool_t updatePropagator);
  Bool_t TryOneClusterFast(const AliMUONTrackParam &trackParam, const Cluster* cluster) const;

  /// MCS effect correction
  void CorrectMCSEffectInAbsorber(AliMUONTrackParam* param,
					 Double_t xVtx, Double_t yVtx, Double_t zVtx,
					 Double_t absZBeg, 
					 Double_t f1, Double_t f2);
  /// Covariant handling function
  void Cov2CovP(const TMatrixD &param, TMatrixD &cov);
  /// Covariant handling function
  void CovP2Cov(const TMatrixD &param, TMatrixD &covP);
  /// Energy loss coreection in front absorber
  void CorrectELossEffectInAbsorber(AliMUONTrackParam* param, Double_t eLoss);
  /// Linear Extrapolation to Z position
  void LinearExtrapToZ(AliMUONTrackParam* trackParam, Double_t zEnd) const;
  /// Energy loss
  Double_t EnergyLossFluctuation2(Double_t pTotal, Double_t pathLength, Double_t rho, Double_t atomicA, Double_t atomicZ);
  /// Bethe Bloch formula of enrgy loss
  Double_t BetheBloch(Double_t pTotal, Double_t pathLength, Double_t rho, Double_t atomicA, Double_t atomicZ);
  
  /// Runge Kutta method of track extrapolation through mag field
  void OneStepRungekutta(Double_t charge, Double_t step, const Double_t* vect, Double_t* vout) const;
  /// Helix3 method of track extrapolation through mag field
  void OneStepHelix3(Double_t field, Double_t step, const Double_t *vect, Double_t *vout) const;
  /// Initialise GRP when running without reconstruction chain
  Bool_t InitGRP();
  /// Fill the tracks to output pointer
  Bool_t FillOutData(AliHLTMUONMansoTrackStruct *data, AliHLTUInt32_t& size) const;
  
};
#endif // ALIHLTMUONMANSOTRACKERFSM_H

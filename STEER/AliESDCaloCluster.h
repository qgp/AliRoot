#ifndef ALIESDCALOCLUSTER_H
#define ALIESDCALOCLUSTER_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */
/* $Id$ */
/* $Log $ */

//-------------------------------------------------------------------------
//                          Class AliESDCaloCluster
//   This is the class to deal with during the physics analysis of data
//
//   New container for calorimeter clusters, which are the effective 
//   "tracks" for calorimeter detectors.  Can be used by PHOS and EMCAL
//
//     J.L. Klay (LLNL)
//-------------------------------------------------------------------------

#include <TObject.h>
#include "AliPID.h"
#include "TArrayS.h"
#include "TArrayI.h"

class TLorentzVector;

class AliESDCaloCluster : public TObject 
{

public:

  AliESDCaloCluster();
  AliESDCaloCluster(const AliESDCaloCluster& clus);
  AliESDCaloCluster & operator=(const AliESDCaloCluster& source);
  virtual ~AliESDCaloCluster();
  virtual void Copy(TObject &) const;

  void SetID(Int_t id) {fID = id;}
  Int_t GetID() const {return fID;}

  //similar to AliAODCluster but offset by one for
  // backward comp. -1 was undefined, which only applied
  // for PHOS clusters before
  enum ESDClu_t {kUndef = -2, 
		 kPHOSCluster, 
		 kEMCALPseudoCluster, //Not any more in use, keep for backward comp.
		 kEMCALClusterv1};

  void SetClusterType(Int_t type) { fClusterType = type; }
  Char_t GetClusterType() const {return fClusterType; }

  Bool_t IsEMCAL() const {return (fClusterType == kEMCALClusterv1);}
  Bool_t IsPHOS() const {return (fClusterType == kPHOSCluster);}

  void SetPosition(const Float_t *pos) {
    fGlobalPos[0] = pos[0]; fGlobalPos[1] = pos[1]; fGlobalPos[2] = pos[2];
  }
  void GetPosition(Float_t *pos) const {
    pos[0] = fGlobalPos[0]; pos[1] = fGlobalPos[1]; pos[2] = fGlobalPos[2];
  }

  void SetE(Float_t ene) { fEnergy = ene;}
  Double_t E() const   { return fEnergy;}

  void SetClusterDisp(Float_t disp)  { fDispersion = disp; }
  Double_t GetClusterDisp() const     { return fDispersion; }

  void SetClusterChi2(Float_t chi2)  { fChi2 = chi2; }
  Double_t GetClusterChi2() const     { return fChi2; }

  void SetPid(const Float_t *p);
  Double_t *GetPid() {return fPID;}

  void SetM20(Float_t m20)                { fM20 = m20; }
  Double_t GetM20() const                  { return fM20; }

  void SetM02(Float_t m02)                { fM02 = m02; }
  Double_t GetM02() const                  { return fM02; }

  void SetNExMax(UChar_t nExMax)         { fNExMax = nExMax; }
  UChar_t GetNExMax() const              { return fNExMax; }

  void SetEmcCpvDistance(Float_t dEmcCpv) { fEmcCpvDistance = dEmcCpv; }
  Double_t GetEmcCpvDistance() const       { return fEmcCpvDistance; }
  void SetTrackDistance(Double_t dx, Double_t dz){fTrackDx=dx; fTrackDz=dz;}
  Double_t GetTrackDx(void)const {return fTrackDx;}
  Double_t GetTrackDz(void)const {return fTrackDz;}

  void SetDistanceToBadChannel(Float_t dist) {fDistToBadChannel=dist;}
  Double_t GetDistanceToBadChannel() const {return fDistToBadChannel;}

  void SetTOF(Double_t tof) { fTOF = tof; }
  Double_t GetTOF() const { return fTOF; }
  
  void AddTracksMatched(TArrayI & array)  { 
    if(!fTracksMatched)fTracksMatched   = new TArrayI(array);
    else *fTracksMatched = array;
  }
  void AddLabels(TArrayI & array)         { 
    if(!fLabels)fLabels = new TArrayI(array) ; 
    else *fLabels = array;
}
  
  TArrayI * GetTracksMatched() const  {return  fTracksMatched;}
  TArrayI * GetLabels() const         {return  fLabels;}
  
  Int_t GetTrackMatched() const   
  {if( fTracksMatched &&  fTracksMatched->GetSize() >0)  return  fTracksMatched->At(0); 
    else return -1;} //Most likely the track associated to the cluster
  Int_t GetLabel() const   
  {if( fLabels &&  fLabels->GetSize() >0)  return  fLabels->At(0); 
    else return -1;} //Most likely the track associated to the cluster
  
  Int_t GetNTracksMatched() const {if (fTracksMatched) return  fTracksMatched->GetSize(); 
    else return -1;}
  Int_t GetNLabels() const        { if (fLabels) return  fLabels->GetSize(); 
    else return -1;}

  void GetMomentum(TLorentzVector& p, Double_t * vertexPosition );

  // --- NEW ---
  void SetNCells(Int_t n) { fNCells = n;}
  Int_t GetNCells() const   { return fNCells;}
  
  void SetCellsAbsId(UShort_t *array) ;
  UShort_t *GetCellsAbsId() {return  fCellsAbsId;}
  
  void SetCellsAmplitudeFraction(Double32_t *array) ;
  Double32_t *GetCellsAmplitudeFraction() {return  fCellsAmpFraction;}
  
  Int_t GetCellAbsId(Int_t i) const {  
    if (fCellsAbsId && i >=0 && i < fNCells ) return fCellsAbsId[i];    
    else return -1;}
  
  Double_t GetCellAmplitudeFraction(Int_t i) const {  
    if (fCellsAmpFraction && i >=0 && i < fNCells ) return fCellsAmpFraction[i];    
    else return -1;}
    
  //_____________________________________________________
  //Not used anymore, kept to avoid backward incompatibility
  void AddDigitIndex(TArrayS & array)     { fDigitIndex   = new TArrayS(array) ; Warning("AddDigitAmplitude","This method is no more in use") ;}
  void AddDigitAmplitude(TArrayS & array) {  fDigitAmplitude   = new TArrayS(array) ; Warning("AddDigitAmplitude","This method is no more in use") ;}
  void AddDigitTime(TArrayS & array)      {  fDigitTime   = new TArrayS(array) ;Warning("AddDigitTime","This method is no more in use") ;}
  TArrayS * GetDigitAmplitude() const {return  fDigitAmplitude;}
  TArrayS * GetDigitTime() const      {return  fDigitTime;}
  TArrayS * GetDigitIndex() const     {return  fDigitIndex;}
  Int_t GetNumberOfDigits() const        { return -1;}
 //_____________________________________________________

protected:

  TArrayI * fTracksMatched; //Index of tracks close to cluster. First entry is the most likely match.
  TArrayI * fLabels;   //list of primaries that generated the cluster, ordered in deposited energy.

  //NEW
  Int_t  fNCells ;
  UShort_t *fCellsAbsId;   //[fNCells] array of cell absId numbers
  Double32_t *fCellsAmpFraction;    //[fNCells][0.,1.,16] array with cell amplitudes fraction.

  //__________________________________________________________
  //Not in use
  TArrayS * fDigitAmplitude;   //digit energy (integer units) 
  TArrayS * fDigitTime;        //time of this digit (integer units) 
  TArrayS * fDigitIndex;       //calorimeter digit index 
  //_________________________________________________________

  Double32_t   fGlobalPos[3];     // position in global coordinate systemD
  Double32_t   fEnergy;           // energy measured by calorimeter
  Double32_t   fDispersion;       // cluster dispersion, for shape analysis
  Double32_t   fChi2;             // chi2 of cluster fi
  Double32_t   fM20;              // 2-nd moment along the main eigen axis
  Double32_t   fM02;              // 2-nd moment along the second eigen axis

  Double32_t   fEmcCpvDistance;   // the distance from PHOS EMC rec.point to the closest CPV rec.point
  Double32_t   fTrackDx ;         // Distance to closest track in phi
  Double32_t   fTrackDz ;         // Distance to closest track in z

  Double32_t   fDistToBadChannel; // Distance to nearest bad channel
  Double32_t   fPID[AliPID::kSPECIESN]; //[0,1,8]"detector response  probabilities" (for the PID)
  Int_t       fID;               // Unique Id of the cluster
  UChar_t  fNExMax ;          // number of (Ex-)maxima before unfolding  
  Char_t  fClusterType;      // Flag for different cluster type/versions
  Double32_t fTOF; //[0,0,12] time-of-flight

  ClassDef(AliESDCaloCluster,9)  //ESDCaloCluster 
};

#endif 


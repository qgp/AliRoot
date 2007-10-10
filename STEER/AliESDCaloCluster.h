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

class TLorentzVector;

class AliESDCaloCluster : public TObject {

public:

  AliESDCaloCluster();
  AliESDCaloCluster(const AliESDCaloCluster& clus);
  AliESDCaloCluster & operator=(const AliESDCaloCluster& source);
  virtual ~AliESDCaloCluster();

  void SetID(Int_t id) {fID = id;}
  Int_t GetID() const {return fID;}

  enum ClusterType {kPseudoCluster, kClusterv1};//Two types of clusters stored
                                                //in EMCAL.
  void SetClusterType(Int_t type) { fClusterType = type; }
  Int_t GetClusterType() const {return fClusterType; }

  void SetEMCAL(Bool_t emc) { fEMCALCluster = emc;}
  Bool_t IsEMCAL() const {return fEMCALCluster;}

  void SetPHOS(Bool_t phos) { fPHOSCluster = phos;}
  Bool_t IsPHOS() const {return fPHOSCluster;}

  void SetPosition(const Float_t *pos) {
    fGlobalPos[0] = pos[0]; fGlobalPos[1] = pos[1]; fGlobalPos[2] = pos[2];
  }
  void GetPosition(Float_t *pos) const {
    pos[0] = fGlobalPos[0]; pos[1] = fGlobalPos[1]; pos[2] = fGlobalPos[2];
  }

  void SetE(Float_t ene) { fEnergy = ene;}
  Float_t E() const   { return fEnergy;}

  void SetClusterDisp(Float_t disp)  { fDispersion = disp; }
  Float_t GetClusterDisp() const     { return fDispersion; }

  void SetClusterChi2(Float_t chi2)  { fChi2 = chi2; }
  Float_t GetClusterChi2() const     { return fChi2; }

  void SetPid(const Float_t *p);
  Float_t *GetPid() {return fPID;}

  void SetM20(Float_t m20)                { fM20 = m20; }
  Float_t GetM20() const                  { return fM20; }

  void SetM02(Float_t m02)                { fM02 = m02; }
  Float_t GetM02() const                  { return fM02; }

  void SetM11(Float_t m11)                { fM11 = m11; }
  Float_t GetM11() const                  { return fM11; }

  void SetNExMax(UShort_t nExMax)         { fNExMax = nExMax; }
  UShort_t GetNExMax() const              { return fNExMax; }

  void SetEmcCpvDistance(Float_t dEmcCpv) { fEmcCpvDistance = dEmcCpv; }
  Float_t GetEmcCpvDistance() const       { return fEmcCpvDistance; }

  void SetDistanceToBadChannel(Float_t dist) {fDistToBadChannel=dist;}
  Float_t GetDistanceToBadChannel() const {return fDistToBadChannel;}

  void AddTracksMatched(TArrayS & array)  { fTracksMatched   = new TArrayS(array) ; }
  void AddLabels(TArrayS & array)         { fLabels = new TArrayS(array) ; }
  void AddDigitAmplitude(TArrayS & array) { fDigitAmplitude   = new TArrayS(array) ; }
  void AddDigitTime(TArrayS & array)      { fDigitTime = new TArrayS(array) ; }
  void AddDigitIndex(TArrayS & array)     { fDigitIndex   = new TArrayS(array) ; }

  TArrayS * GetTracksMatched() const  {return  fTracksMatched;}
  TArrayS * GetLabels() const         {return  fLabels;}
  TArrayS * GetDigitAmplitude() const {return  fDigitAmplitude;}
  TArrayS * GetDigitTime() const      {return  fDigitTime;}
  TArrayS * GetDigitIndex() const     {return  fDigitIndex;}
 
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
  Int_t GetNumberOfDigits() const        { if (fDigitAmplitude) return  fDigitAmplitude->GetSize(); 
    else return -1;}
 
  void GetMomentum(TLorentzVector& p, Double_t * vertexPosition );
  // Sep 7, 2007
  Int_t    GetTrueDigitAmplitude(Int_t i, Double_t cc);
  Double_t GetTrueDigitEnergy(Int_t i, Double_t cc);
  Double_t GetRecalibratedDigitEnergy(Int_t i, Double_t ccOld, Double_t ccNew);

protected:

  Int_t     fID;               // Unique Id of the cluster
  Int_t     fClusterType;      // Flag for different clustering versions
  Bool_t    fEMCALCluster;     // Is this is an EMCAL cluster?
  Bool_t    fPHOSCluster;      // Is this is a PHOS cluster?
  Float_t   fGlobalPos[3];     // position in global coordinate system
  Float_t   fEnergy;           // energy measured by calorimeter
  Float_t   fDispersion;       // cluster dispersion, for shape analysis
  Float_t   fChi2;             // chi2 of cluster fit
  Float_t   fPID[AliPID::kSPECIESN]; //"detector response probabilities" (for the PID)
  Float_t   fM20;              // 2-nd moment along the main eigen axis
  Float_t   fM02;              // 2-nd moment along the second eigen axis
  Float_t   fM11;              // 2-nd mixed moment Mxy
  UShort_t  fNExMax ;          // number of (Ex-)maxima before unfolding
  Float_t   fEmcCpvDistance;   // the distance from PHOS EMC rec.point to the closest CPV rec.point
 Float_t   fDistToBadChannel; // Distance to nearest bad channel

  TArrayS * fTracksMatched; //Index of tracks close to cluster. First entry is the most likely match.
  TArrayS * fLabels;   //list of primaries that generated the cluster, ordered in deposited energy.
  TArrayS * fDigitAmplitude;   //digit energy (integer units) 
  TArrayS * fDigitTime;        //time of this digit (integer units) 
  TArrayS * fDigitIndex;       //calorimeter digit index 

  ClassDef(AliESDCaloCluster,4)  //ESDCaloCluster 
};

#endif 


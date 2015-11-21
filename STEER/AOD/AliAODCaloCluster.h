#ifndef AliAODCaloCluster_H
#define AliAODCaloCluster_H
/* Copyright(c) 1998-2007, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

//-------------------------------------------------------------------------
//     AOD calorimeter cluster class (for PHOS and EMCAL)
//     Author: Markus Oldenburg, CERN, 
//             Gustavo Conesa, INFN
//-------------------------------------------------------------------------

#include "AliAODCluster.h"

#include <TRefArray.h>
#include <TArrayS.h>

class TLorentzVector;

class AliAODCaloCluster : public AliAODCluster {

 public:
  
  AliAODCaloCluster();
  AliAODCaloCluster(Int_t id,
		    UInt_t nLabel,
		    Int_t *label,
		    Double_t energy,
		    Double_t x[3],
		    Double_t pid[13],
		    Char_t ttype=kUndef,
		    UInt_t selectInfo=0);
  
  AliAODCaloCluster(Int_t id,
		    UInt_t nLabel,
		    Int_t *label,
		    Float_t energy,
		    Float_t x[3],
		    Float_t pid[13],
		    Char_t ttype=kUndef,
		    UInt_t selectInfo=0);
  
  virtual ~AliAODCaloCluster();
  AliAODCaloCluster(const AliAODCaloCluster& clus); 
  AliAODCaloCluster& operator=(const AliAODCaloCluster& clus);
  void Clear(const Option_t*);
  
  // getters
  Double_t GetDistanceToBadChannel() const { return fDistToBadChannel; }
  Double_t GetDispersion() const { return fDispersion; }
  Double_t GetM20() const { return fM20; }
  Double_t GetM02() const { return fM02; }
  Double_t GetEmcCpvDistance() const { return fEmcCpvDistance; }
  Double_t GetTrackDx(void)const {return fTrackDx;}
  Double_t GetTrackDz(void)const {return fTrackDz;}
  UChar_t  GetNExMax() const { return fNExMax; }
  Double_t GetTOF() const { return fTOF; }
  Double_t GetCoreEnergy() const           {return fCoreEnergy ; }
 
 

  Int_t    GetNTracksMatched() const { return fTracksMatched.GetEntriesFast(); }
  TObject *GetTrackMatched(Int_t i) const { return fTracksMatched.At(i); }
 
  void  SetNCells(Int_t n) { fNCells = n;}
  Int_t GetNCells() const   { return fNCells;}
  
  void SetCellsAbsId(UShort_t *array);
  UShort_t *GetCellsAbsId() {return  fCellsAbsId;}
  
  void SetCellsAmplitudeFraction(Double32_t *array);
  Double32_t *GetCellsAmplitudeFraction() {return  fCellsAmpFraction;}
  
  Int_t GetCellAbsId(Int_t i) const {  
    if (fCellsAbsId && i >=0 && i < fNCells ) return fCellsAbsId[i];    
    else return -1;}
  
  Double_t GetCellAmplitudeFraction(Int_t i) const {  
    if (fCellsAmpFraction && i >=0 && i < fNCells ) return fCellsAmpFraction[i];    
    else return -1;}
	
  
  // setters
  void SetDistanceToBadChannel(Double_t dist) { fDistToBadChannel = dist; }
  void SetDispersion(Double_t disp) { fDispersion = disp; }
  void SetM20(Double_t m20) { fM20 = m20; }
  void SetM02(Double_t m02) { fM02 = m02; }
  void SetEmcCpvDistance(Double_t emcCpvDist) { fEmcCpvDistance = emcCpvDist; }
  void SetNExMax(UChar_t nExMax) { fNExMax = nExMax; }
  void SetTOF(Double_t tof) { fTOF = tof; }
  void SetTrackDistance(Double_t dx, Double_t dz){fTrackDx=dx; fTrackDz=dz;}
  void SetCoreEnergy(Double_t e)         { fCoreEnergy=e; }

  void SetCaloCluster(Double_t dist = -999., 
		      Double_t disp = -1., 
		      Double_t m20 = 0., 
		      Double_t m02 = 0., 
		      Double_t emcCpvDist = -999., 
		      UShort_t nExMax = 0, 
		      Double_t tof = 0.) 
  {
    fDistToBadChannel = dist;
    fDispersion = disp;
    fM20 = m20;
    fM02 = m02;
    fEmcCpvDistance = emcCpvDist;
    fNExMax = nExMax;
    fTOF = tof ;
  }
  
  void GetMomentum(TLorentzVector& p, Double_t * vertexPosition ) const;
  void GetMomentum(TLorentzVector& p, Double_t * vertexPosition, VCluUserDefEnergy_t t ) const;

  void AddTrackMatched(TObject *trk) { 
    //Make sure we attach the object to correct process number
    if(fTracksMatched.GetEntries()==0) { TRefArray ref(TProcessID::GetProcessWithUID(trk)) ; fTracksMatched = ref ; }
    fTracksMatched.Add(trk) ; }
  
  void RemoveTrackMatched(TObject *trk) { fTracksMatched.Remove(trk); }
  Bool_t HasTrackMatched(TObject *trk) const;

  Double_t    GetMCEnergyFraction() const          { return fMCEnergyFraction ; }
  void        SetMCEnergyFraction(Double_t e)      { fMCEnergyFraction = e    ; }

  Bool_t      GetIsExotic() const                  { return fIsExotic         ; }
  void        SetIsExotic(Bool_t b)                { fIsExotic         = b    ; }

  Double_t    GetUserDefEnergy(Int_t t)               const                     { return AliVCluster::GetUserDefEnergy(t); }
  Double_t    GetUserDefEnergy(VCluUserDefEnergy_t t) const                     { return E()*fUserDefEnergy[t]                   ; }
  void        SetUserDefEnergy(Int_t t, Double_t e)                             { AliVCluster::SetUserDefEnergy(t,e);}
  void        SetUserDefEnergy(VCluUserDefEnergy_t t, Double_t e)               { fUserDefEnergy[t] = E() > 1e-6 ? e / E() : 1.  ; }
  void        SetUserDefEnergyCorrFactor(VCluUserDefEnergy_t t, Double_t f)     { fUserDefEnergy[t] = f                          ; }
  
 private :

  Double32_t   fDistToBadChannel; // Distance to nearest bad channel
  Double32_t   fDispersion;       // cluster dispersion, for shape analysis
  Double32_t   fM20;              // 2-nd moment along the main eigen axis
  Double32_t   fM02;              // 2-nd moment along the second eigen axis
  Double32_t   fEmcCpvDistance;   // the distance from PHOS EMC rec.point to the closest CPV rec.point
  Double32_t   fTrackDx ;         // Distance to closest track in phi
  Double32_t   fTrackDz ;         // Distance to closest track in z (eta)
  UShort_t     fNExMax;           // number of (Ex-)maxima before unfolding
  Double32_t   fTOF;              ////[0,0,12] time-of-flight
  Double32_t   fCoreEnergy;       // energy of the core of cluster

  TRefArray    fTracksMatched;    // references to tracks close to cluster. First entry is the most likely match.

  Int_t       fNCells ;
  UShort_t   *fCellsAbsId;        //[fNCells] array of cell absId numbers
  Double32_t *fCellsAmpFraction;  //[fNCells][0.,1.,16] array with cell amplitudes fraction.
  
  Double_t     fMCEnergyFraction;                    //!MC energy (embedding)
  Bool_t       fIsExotic;                            //!cluster marked as "exotic" (high energy deposition concentrated in a single cell)
  Double_t     fUserDefEnergy[kLastUserDefEnergy+1]; //!energy of the cluster after other higher level corrections (e.g. non-linearity, hadronic correction, ...)

  ClassDef(AliAODCaloCluster,8);
};

#endif

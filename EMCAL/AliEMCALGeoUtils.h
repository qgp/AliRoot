#ifndef ALIEMCALGEOUTILS_H
#define ALIEMCALGEOUTILS_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

//_________________________________________________________________________
// class for geometry transformations in EMCAL
// this class contains AliRoot-independent transformations,
// AliRoot part is in AliEMCALGeometry
// 
//*-- Author: Magali Estienne

// --- ROOT system ---
#include <TNamed.h>
#include <TMath.h>
#include <TArrayD.h>
#include <TVector3.h>
#include <TGeoMatrix.h> 
class TBrowser ;
class TParticle ;

// --- AliRoot header files ---
#include "AliEMCALEMCGeometry.h"
class AliEMCALShishKebabTrd1Module;
#include "AliLog.h"

class AliEMCALGeoUtils : public TNamed {

public: 

  AliEMCALGeoUtils();
  AliEMCALGeoUtils(const Text_t* name, const Text_t* title="");
  AliEMCALGeoUtils(const AliEMCALGeoUtils & geom);
  
  virtual ~AliEMCALGeoUtils(void); 
  AliEMCALGeoUtils & operator = (const AliEMCALGeoUtils  & rvalue);

  /////////////
  // TRD1 stuff
  void    CreateListOfTrd1Modules();
  TList  *GetShishKebabTrd1Modules() const {return fShishKebabTrd1Modules;}
  AliEMCALShishKebabTrd1Module *GetShishKebabModule(Int_t neta) const;

  void PrintGeometry(); 
  void PrintCellIndexes(Int_t absId=0, int pri=0, const char *tit="") const ;  //*MENU*
  virtual void Browse(TBrowser* b);
  virtual Bool_t  IsFolder() const;

  virtual Bool_t Impact(const TParticle *) const;
  void ImpactOnEmcal(TVector3 vtx, Double_t theta, Double_t phi, Int_t & absId, TVector3 & vimpact) const;
  Bool_t IsInEMCAL(Double_t x, Double_t y, Double_t z) const;


  AliEMCALEMCGeometry* GetEMCGeometry() const { return fEMCGeometry;}

  //////////////////////////
  // Global geometry methods
  //
  void GetGlobal(const Double_t *loc, Double_t *glob, int ind) const;
  void GetGlobal(const TVector3 &vloc, TVector3 &vglob, int ind) const;
  void GetGlobal(Int_t absId, Double_t glob[3]) const;
  void GetGlobal(Int_t absId, TVector3 &vglob) const;

  ////////////////////////////////////////
  // May 31, 2006; ALICE numbering scheme: 
  // see ALICE-INT-2003-038: ALICE Coordinate System and Software Numbering Convention
  // All indexes are stared from zero now.
  // 
  // abs id <-> indexes; Shish-kebab case, only TRD1 now.
  // EMCAL -> Super Module -> module -> tower(or cell) - logic tree of EMCAL
  // 
  //**  Usual name of variable - Dec 18,2006 **
  //  nSupMod - index of super module (SM)
  //  nModule - index of module in SM
  //  nIphi   - phi index of tower(cell) in module
  //  nIeta   - eta index of tower(cell) in module
  //  
  //  Inside SM
  //  iphim   - phi index of module in SM  
  //  ietam   - eta index of module in SM  
  //
  //  iphi    - phi index of tower(cell) in SM  
  //  ieta    - eta index of tower(cell) in SM  
  //
  // for a given tower index absId returns eta and phi of gravity center of tower.
  void    EtaPhiFromIndex(Int_t absId, Double_t &eta, Double_t &phi) const;
  void    EtaPhiFromIndex(Int_t absId, Float_t &eta, Float_t &phi) const;

  Bool_t  GetAbsCellIdFromEtaPhi(Double_t eta,Double_t phi, Int_t &absId) const;
  Bool_t  SuperModuleNumberFromEtaPhi(Double_t eta, Double_t phi, Int_t &nSupMod) const;
  Int_t   GetAbsCellId(Int_t nSupMod, Int_t nModule, Int_t nIphi, Int_t nIeta) const;
  Bool_t  CheckAbsCellId(Int_t absId) const;
  Bool_t  GetCellIndex(Int_t absId, Int_t &nSupMod, Int_t &nModule, Int_t &nIphi, 
		       Int_t &nIeta) const;
  // Local coordinate of Super Module 
  void    GetModulePhiEtaIndexInSModule(Int_t nSupMod, Int_t nModule, Int_t &iphim, 
					Int_t &ietam) const;
  void    GetCellPhiEtaIndexInSModule(Int_t nSupMod, Int_t nModule, Int_t nIphi, Int_t nIeta,
                                      Int_t &iphi, Int_t &ieta) const ;
  Int_t   GetSuperModuleNumber(Int_t absId)  const;
  Int_t   GetNumberOfModuleInPhiDirection(Int_t nSupMod)  const
  { 
    if(fKey110DEG == 1 && nSupMod>=10) return fNPhi/2;
    else                               return fNPhi;
  } 
  // From cell indexes to abs cell id
  void    GetModuleIndexesFromCellIndexesInSModule(Int_t nSupMod, Int_t iphi, Int_t ieta, 
					      Int_t &iphim, Int_t &ietam, Int_t &nModule) const;
  Int_t   GetAbsCellIdFromCellIndexes(Int_t nSupMod, Int_t iphi, Int_t ieta) const;

  // Methods for AliEMCALRecPoint - Feb 19, 2006
  Bool_t  RelPosCellInSModule(Int_t absId, Double_t &xr, Double_t &yr, Double_t &zr) const;
  Bool_t  RelPosCellInSModule(Int_t absId, Double_t loc[3]) const;
  Bool_t  RelPosCellInSModule(Int_t absId, TVector3 &vloc) const;

  // Local Coordinates of SM
  TArrayD  GetCentersOfCellsEtaDir() const {return fCentersOfCellsEtaDir;}        // size fNEta*fNETAdiv (for TRD1 only) (eta or z in SM, in cm)
  TArrayD  GetCentersOfCellsXDir()   const {return fCentersOfCellsXDir;}          // size fNEta*fNETAdiv (for TRD1 only) (       x in SM, in cm)
  TArrayD  GetCentersOfCellsPhiDir() const {return fCentersOfCellsPhiDir;}        // size fNPhi*fNPHIdiv (for TRD1 only) (phi or y in SM, in cm)
  //
  TArrayD  GetEtaCentersOfCells() const {return fEtaCentersOfCells;}           // [fNEta*fNETAdiv*fNPhi*fNPHIdiv], positive direction (eta>0); eta depend from phi position; 
  TArrayD  GetPhiCentersOfCells() const {return fPhiCentersOfCells;}           // [fNPhi*fNPHIdiv] from center of SM (-10. < phi < +10.)

  //
  // Tranforms Eta-Phi Module index in TRU into Eta-Phi index in Super Module
  void     GetModulePhiEtaIndexInSModuleFromTRUIndex(
	       Int_t itru, Int_t iphitru, Int_t ietatru, Int_t &ietaSM, Int_t &iphiSM) const;
  Int_t   GetAbsTRUNumberFromNumberInSm(const Int_t row, const Int_t col, const Int_t sm) const ;

	
  void     BuildFastOR2DMap();
  Bool_t   GetAbsFastORIndexFromTRU(const Int_t iTRU, const Int_t iADC, Int_t& id) const;
  Bool_t                    GetAbsFastORIndexFromPositionInTRU(const Int_t iTRU, const Int_t iEta, const Int_t iPhi, Int_t& id) const;	
  Bool_t                    GetAbsFastORIndexFromPositionInSM( const Int_t  iSM, const Int_t iEta, const Int_t iPhi, Int_t& id) const;	
  Bool_t                    GetAbsFastORIndexFromPositionInEMCAL(                const Int_t iEta, const Int_t iPhi, Int_t& id) const;
  Bool_t             GetTRUFromAbsFastORIndex(const Int_t id, Int_t& iTRU, Int_t& iADC) const;
  Bool_t   GetPositionInTRUFromAbsFastORIndex(const Int_t id, Int_t& iTRU, Int_t& iEta, Int_t& iPhi) const;
  Bool_t    GetPositionInSMFromAbsFastORIndex(const Int_t id, Int_t& iSM, Int_t& iEta, Int_t& iPhi) const;
  Bool_t GetPositionInEMCALFromAbsFastORIndex(const Int_t id, Int_t& iEta, Int_t& iPhi) const;
  Bool_t          GetFastORIndexFromCellIndex(const Int_t id, Int_t& idx) const;
  Bool_t          GetCellIndexFromFastORIndex(const Int_t id, Int_t idx[4]) const;
  Bool_t              GetTRUIndexFromSTUIndex(const Int_t id, Int_t& idx) const;
  Int_t               GetTRUIndexFromSTUIndex(const Int_t id) const;
  Bool_t            GetFastORIndexFromL0Index(const Int_t iTRU, const Int_t id, Int_t idx[], const Int_t size) const;
	
  ///////////////////
  // useful utilities
  //
  Float_t AngleFromEta(Float_t eta) const { // returns theta in radians for a given pseudorapidity
    return 2.0*TMath::ATan(TMath::Exp(-eta));
  }
  Float_t ZFromEtaR(Float_t r,Float_t eta) const { // returns z in for a given
    // pseudorapidity and r=sqrt(x*x+y*y).
    return r/TMath::Tan(AngleFromEta(eta));
  }

  //Method to set shift-rotational matrixes from ESDHeader
  void SetMisalMatrix(const TGeoHMatrix * m, Int_t smod) {
	  if (smod >= 0 && smod < fEMCGeometry->GetNumberOfSuperModules()){
            if(!fkSModuleMatrix[smod]) fkSModuleMatrix[smod] = new TGeoHMatrix(*m) ; //Set only if not set yet
          }
	  else AliFatal(Form("Wrong supermodule index -> %d",smod));
  }
	
protected:
  //Returns shift-rotational matrixes for different volumes
  const TGeoHMatrix * GetMatrixForSuperModule(Int_t smod)const ;
	
protected:

  // ctor only for internal usage (singleton)
  //  AliEMCALGeoUtils(const Text_t* name, const Text_t* title);
  AliEMCALEMCGeometry     *fEMCGeometry;   // Geometry object for Electromagnetic calorimeter

  void Init(void);     		     // initializes the parameters of EMCAL

  TString  fGeoName;                     //geometry name

  Int_t    fKey110DEG;               // for calculation abs cell id; 19-oct-05 
  Int_t    fNCellsInSupMod;          // number cell in super module
  Int_t    fNETAdiv;                 // number eta divizion of module
  Int_t    fNPHIdiv;                 // number phi divizion of module
  Int_t    fNCellsInModule;          // number cell in module
  TArrayD  fPhiBoundariesOfSM;       // phi boundaries of SM in rad; size is fNumberOfSuperModules;
  TArrayD  fPhiCentersOfSM;          // phi of centers of SMl size is fNumberOfSuperModules/2
  // Local Coordinates of SM
  TArrayD  fPhiCentersOfCells;       // [fNPhi*fNPHIdiv] from center of SM (-10. < phi < +10.)
  TArrayD  fCentersOfCellsEtaDir;    // size fNEta*fNETAdiv (for TRD1 only) (eta or z in SM, in cm)
  TArrayD  fCentersOfCellsPhiDir;    // size fNPhi*fNPHIdiv (for TRD1 only) (phi or y in SM, in cm)
  TArrayD  fEtaCentersOfCells;       // [fNEta*fNETAdiv*fNPhi*fNPHIdiv], positive direction (eta>0); eta depend from phi position; 
  Int_t    fNCells;                  // number of cells in calo
  Int_t    fNPhi;		             // Number of Towers in the PHI direction
  TArrayD  fCentersOfCellsXDir;      // size fNEta*fNETAdiv (for TRD1 only) (       x in SM, in cm)
  Float_t  fEnvelop[3];              // the GEANT TUB for the detector 
  Float_t  fArm1EtaMin;              // Minimum pseudorapidity position of EMCAL in Eta
  Float_t  fArm1EtaMax;              // Maximum pseudorapidity position of EMCAL in Eta
  Float_t  fArm1PhiMin;              // Minimum angular position of EMCAL in Phi (degrees)
  Float_t  fArm1PhiMax;              // Maximum angular position of EMCAL in Phi (degrees)
  Float_t  fEtaMaxOfTRD1;            // Max eta in case of TRD1 geometry (see AliEMCALShishKebabTrd1Module)
  TList    *fShishKebabTrd1Modules;  //! list of modules
  Float_t  *fParSM;                  // SM sizes as in GEANT (TRD1)
  Float_t  fPhiModuleSize;           // Phi -> X 
  Float_t  fEtaModuleSize;           // Eta -> Y 
  Float_t  fPhiTileSize;             // Size of phi tile
  Float_t  fEtaTileSize;             // Size of eta tile
  Int_t    fNZ;                      // Number of Towers in the Z direction
  Float_t  fIPDistance;		         // Radial Distance of the inner surface of the EMCAL
  Float_t  fLongModuleSize;          // Size of long module
  // Geometry Parameters
  Float_t  fShellThickness;		     // Total thickness in (x,y) direction
  Float_t  fZLength;			     // Total length in z direction
  Float_t  fSampling;			     // Sampling factor

  Int_t    fFastOR2DMap[48][64];	 // FastOR 2D Map over full EMCal
	
  TGeoHMatrix* fkSModuleMatrix[12] ; //Orientations of EMCAL super modules

	
  ClassDef(AliEMCALGeoUtils,1)       // EMCAL geometry class 

} ;

#endif // AliEMCALGEOUTILS_H


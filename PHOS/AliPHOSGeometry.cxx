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

//_________________________________________________________________________
// Geometry class  for PHOS : singleton  
// PHOS consists of the electromagnetic calorimeter (EMCA)
// and a charged particle veto either in the Subatech's version (PPSD)
// or in the IHEP's one (CPV).
// The EMCA/PPSD/CPV modules are parametrized so that any configuration
// can be easily implemented 
// The title is used to identify the version of CPV used.
//                  
//*-- Author: Yves Schutz (SUBATECH) & Dmitri Peressounko (RRC "KI" & SUBATECH)

// --- ROOT system ---

#include "TVector3.h"
#include "TRotation.h" 
#include "TFolder.h" 
#include "TROOT.h" 

// --- Standard library ---

#include <iostream.h>
#include <stdlib.h>

// --- AliRoot header files ---

#include "AliPHOSGeometry.h"
#include "AliPHOSEMCAGeometry.h" 
#include "AliPHOSRecPoint.h"
#include "AliConst.h"

ClassImp(AliPHOSGeometry) ;

// these initialisations are needed for a singleton
AliPHOSGeometry * AliPHOSGeometry::fgGeom = 0 ;
Bool_t            AliPHOSGeometry::fgInit = kFALSE ;

//____________________________________________________________________________
AliPHOSGeometry::~AliPHOSGeometry(void)
{
  // dtor

  if (fRotMatrixArray) fRotMatrixArray->Delete() ; 
  if (fRotMatrixArray) delete fRotMatrixArray ; 
  if (fPHOSAngle     ) delete[] fPHOSAngle ; 
}
//____________________________________________________________________________

void AliPHOSGeometry::Init(void)
{
  // Initializes the PHOS parameters :
  //  IHEP is the Protvino CPV (cathode pad chambers)
  //  GPS2 is the Subatech Pre-Shower (two micromegas sandwiching a passive lead converter)
  //  MIXT 4 PHOS modules withe the IHEP CPV and one PHOS module with the Subatech Pre-Shower
  
  TString test(GetName()) ; 
  if (test != "IHEP" && test != "GPS2" && test != "MIXT") {
    cerr << "ERROR: " << ClassName() << "::Init -> " << test.Data() 
	 << " is not a known geometry (choose among IHEP, GPS2 and MIXT)" << endl ; 
    abort() ;
  }

  fgInit     = kTRUE ; 
  
  fNModules     = 5;
  fAngle        = 20;
  
  fGeometryEMCA = new AliPHOSEMCAGeometry();
  
  fGeometryCPV  = new AliPHOSCPVGeometry ();
  
  fGeometrySUPP = new AliPHOSSupportGeometry();
  
  fPHOSAngle = new Float_t[fNModules] ;
  
  Float_t * emcParams = fGeometryEMCA->GetEMCParams() ;
  
  fPHOSParams[0] =  TMath::Max((Double_t)fGeometryCPV->GetCPVBoxSize(0)/2., 
			       (Double_t)(emcParams[0]*(fGeometryCPV->GetCPVBoxSize(1)+emcParams[3]) - 
				emcParams[1]* fGeometryCPV->GetCPVBoxSize(1))/emcParams[3] ) ;
  fPHOSParams[1] = emcParams[1] ;
  fPHOSParams[2] = TMath::Max((Double_t)emcParams[2], (Double_t)fGeometryCPV->GetCPVBoxSize(2)/2.);
  fPHOSParams[3] = emcParams[3] + fGeometryCPV->GetCPVBoxSize(1)/2. ;
  
  fIPtoUpperCPVsurface = fGeometryEMCA->GetIPtoOuterCoverDistance() - fGeometryCPV->GetCPVBoxSize(1) ;
  
  Int_t index ;
  for ( index = 0; index < fNModules; index++ )
    fPHOSAngle[index] = 0.0 ; // Module position angles are set in CreateGeometry()
  
  this->SetPHOSAngles() ; 
  fRotMatrixArray = new TObjArray(fNModules) ; 
  
}

//____________________________________________________________________________
AliPHOSGeometry *  AliPHOSGeometry::GetInstance() 
{ 
  // Returns the pointer of the unique instance; singleton specific
  
  return static_cast<AliPHOSGeometry *>( fgGeom ) ; 
}

//____________________________________________________________________________
AliPHOSGeometry *  AliPHOSGeometry::GetInstance(const Text_t* name, const Text_t* title) 
{
  // Returns the pointer of the unique instance
  // Creates it with the specified options (name, title) if it does not exist yet

  AliPHOSGeometry * rv = 0  ; 
  if ( fgGeom == 0 ) {
    if ( strcmp(name,"") == 0 ) 
      rv = 0 ;
    else {    
      fgGeom = new AliPHOSGeometry(name, title) ;
      if ( fgInit )
	rv = (AliPHOSGeometry * ) fgGeom ;
      else {
	rv = 0 ; 
	delete fgGeom ; 
	fgGeom = 0 ; 
      }
    }
  }
  else {
    if ( strcmp(fgGeom->GetName(), name) != 0 ) {
      cout << "AliPHOSGeometry <E> : current geometry is " << fgGeom->GetName() << endl
	   << "                      you cannot call     " << name << endl ; 
    }
    else
      rv = (AliPHOSGeometry *) fgGeom ; 
  } 
  return rv ; 
}

//____________________________________________________________________________
void AliPHOSGeometry::SetPHOSAngles() 
{ 
  // Calculates the position of the PHOS modules in ALICE global coordinate system
  
  Double_t const kRADDEG = 180.0 / kPI ;
  Float_t pphi =  2 * TMath::ATan( GetOuterBoxSize(0)  / ( 2.0 * GetIPtoUpperCPVsurface() ) ) ;
  pphi *= kRADDEG ;
  if (pphi > fAngle){ 
    cout << "AliPHOSGeometry: PHOS modules overlap!\n";
    cout <<  "pphi = " << pphi << " fAngle  " << fAngle << endl ;

  }
  pphi = fAngle;
  
  for( Int_t i = 1; i <= fNModules ; i++ ) {
    Float_t angle = pphi * ( i - fNModules / 2.0 - 0.5 ) ;
    fPHOSAngle[i-1] = -  angle ;
  } 
}

//____________________________________________________________________________
Bool_t AliPHOSGeometry::AbsToRelNumbering(const Int_t AbsId, Int_t * relid) const
{
  // Converts the absolute numbering into the following array/
  //  relid[0] = PHOS Module number 1:fNModules 
  //  relid[1] = 0 if PbW04
  //           = -1 if CPV
  //  relid[2] = Row number inside a PHOS module
  //  relid[3] = Column number inside a PHOS module

  Bool_t rv  = kTRUE ; 
  Float_t id = AbsId ;

  Int_t phosmodulenumber = (Int_t)TMath:: Ceil( id / GetNCristalsInModule() ) ; 
  
  if ( phosmodulenumber >  GetNModules() ) { // it is a CPV pad
    
    id -=  GetNPhi() * GetNZ() *  GetNModules() ; 
    Float_t nCPV  = GetNumberOfCPVPadsPhi() * GetNumberOfCPVPadsZ() ;
    relid[0] = (Int_t) TMath::Ceil( id / nCPV ) ;
    relid[1] = -1 ;
    id -= ( relid[0] - 1 ) * nCPV ; 
    relid[2] = (Int_t) TMath::Ceil( id / GetNumberOfCPVPadsZ() ) ;
    relid[3] = (Int_t) ( id - ( relid[2] - 1 ) * GetNumberOfCPVPadsZ() ) ; 
  } 
  else { // it is a PW04 crystal

    relid[0] = phosmodulenumber ;
    relid[1] = 0 ;
    id -= ( phosmodulenumber - 1 ) *  GetNPhi() * GetNZ() ; 
    relid[2] = (Int_t)TMath::Ceil( id / GetNZ() )  ;
    relid[3] = (Int_t)( id - ( relid[2] - 1 ) * GetNZ() ) ; 
  } 
  return rv ; 
}

//____________________________________________________________________________  
void AliPHOSGeometry::EmcModuleCoverage(const Int_t mod, Double_t & tm, Double_t & tM, Double_t & pm, Double_t & pM, Option_t * opt) const 
{
  // calculates the angular coverage in theta and phi of one EMC (=PHOS) module

 Double_t conv ; 
  if ( opt == Radian() ) 
    conv = 1. ; 
  else if ( opt == Degre() )
    conv = 180. / TMath::Pi() ; 
  else {
    cout << "<I>  AliPHOSGeometry::EmcXtalCoverage : " << opt << " unknown option; result in radian " << endl ; 
    conv = 1. ;
      }

  Float_t phi = GetPHOSAngle(mod) *  (TMath::Pi() / 180.)  ;  
  Float_t y0  = GetIPtoCrystalSurface() ; 
  Float_t * strip = fGeometryEMCA->GetStripHalfSize() ;
  Float_t x0  = fGeometryEMCA->GetNStripX()*strip[0] ;
  Float_t z0  = fGeometryEMCA->GetNStripZ()*strip[2] ;
  Double_t angle = TMath::ATan( x0 / y0 ) ;
  phi = phi + 1.5 * TMath::Pi() ; // to follow the convention of the particle generator(PHOS is between 220 and 320 deg.)
  Double_t max  = phi - angle ;
  Double_t min   = phi + angle ;
  pM = TMath::Max(max, min) * conv ;
  pm = TMath::Min(max, min) * conv ; 
  
  angle =  TMath::ATan( z0 /  y0 ) ;
  max  = TMath::Pi() / 2.  + angle ; // to follow the convention of the particle generator(PHOS is at 90 deg.)
  min  = TMath::Pi() / 2.  - angle ;
  tM = TMath::Max(max, min) * conv ;
  tm = TMath::Min(max, min) * conv ; 
 
}

//____________________________________________________________________________  
void AliPHOSGeometry::EmcXtalCoverage(Double_t & theta, Double_t & phi, Option_t * opt) const
{
  // calculates the angular coverage in theta and phi of a single crystal in a EMC(=PHOS) module

  Double_t conv ; 
  if ( opt == Radian() ) 
    conv = 1. ; 
  else if ( opt == Degre() )
    conv = 180. / TMath::Pi() ; 
  else {
    cout << "<I>  AliPHOSGeometry::EmcXtalCoverage : " << opt << " unknown option; result in radian " << endl ; 
    conv = 1. ;
      }

  Float_t y0  = GetIPtoCrystalSurface() ; 
  theta = 2 * TMath::ATan( GetCrystalSize(2) / (2 * y0) ) * conv ;
  phi   = 2 * TMath::ATan( GetCrystalSize(0) / (2 * y0) ) * conv ;
}
 

//____________________________________________________________________________
void AliPHOSGeometry::GetGlobal(const AliRecPoint* RecPoint, TVector3 & gpos, TMatrix & gmat) const
{
  // Calculates the coordinates of a RecPoint and the error matrix in the ALICE global coordinate system
 
  AliPHOSRecPoint * tmpPHOS = (AliPHOSRecPoint *) RecPoint ;  
  TVector3 localposition ;

  tmpPHOS->GetLocalPosition(gpos) ;


  if ( tmpPHOS->IsEmc() ) // it is a EMC crystal 
    {  gpos.SetY( - GetIPtoCrystalSurface()) ;  

    }
  else
    { // it is a CPV
      gpos.SetY(- GetIPtoUpperCPVsurface()  ) ; 
    }  

  Float_t phi           = GetPHOSAngle( tmpPHOS->GetPHOSMod()) ; 
  Double_t const kRADDEG = 180.0 / kPI ;
  Float_t rphi          = phi / kRADDEG ; 
  
  TRotation rot ;
  rot.RotateZ(-rphi) ; // a rotation around Z by angle  
  
  TRotation dummy = rot.Invert() ;  // to transform from original frame to rotate frame
  gpos.Transform(rot) ; // rotate the baby 

}

//____________________________________________________________________________
void AliPHOSGeometry::GetGlobal(const AliRecPoint* RecPoint, TVector3 & gpos) const 
{
  // Calculates the coordinates of a RecPoint in the ALICE global coordinate system 

  AliPHOSRecPoint * tmpPHOS = (AliPHOSRecPoint *) RecPoint ;  
  TVector3 localposition ;
  tmpPHOS->GetLocalPosition(gpos) ;


  if ( tmpPHOS->IsEmc() ) // it is a EMC crystal 
    {  gpos.SetY( - GetIPtoCrystalSurface()  ) ;  
    }
  else
    { // it is a CPV
	  gpos.SetY(- GetIPtoUpperCPVsurface()  ) ; 
    }  

  Float_t phi           = GetPHOSAngle( tmpPHOS->GetPHOSMod()) ; 
  Double_t const kRADDEG = 180.0 / kPI ;
  Float_t rphi          = phi / kRADDEG ; 
  
  TRotation rot ;
  rot.RotateZ(-rphi) ; // a rotation around Z by angle  
  
  TRotation dummy = rot.Invert() ;  // to transform from original frame to rotate frame
  gpos.Transform(rot) ; // rotate the baby 
}

//____________________________________________________________________________
void AliPHOSGeometry::ImpactOnEmc(const Double_t theta, const Double_t phi, Int_t & ModuleNumber, Double_t & z, Double_t & x) const
{
  // calculates the impact coordinates on PHOS of a neutral particle  
  // emitted in the direction theta and phi in the ALICE global coordinate system

  // searches for the PHOS EMC module
  ModuleNumber = 0 ; 
  Double_t tm, tM, pm, pM ; 
  Int_t index = 1 ; 
  while ( ModuleNumber == 0 && index <= GetNModules() ) { 
    EmcModuleCoverage(index, tm, tM, pm, pM) ; 
    if ( (theta >= tm && theta <= tM) && (phi >= pm && phi <= pM ) ) 
      ModuleNumber = index ; 
    index++ ;    
  }
  if ( ModuleNumber != 0 ) {
    Float_t phi0 =  GetPHOSAngle(ModuleNumber) *  (TMath::Pi() / 180.) + 1.5 * TMath::Pi()  ;  
    Float_t y0  =  GetIPtoCrystalSurface()  ;   
    Double_t angle = phi - phi0; 
    x = y0 * TMath::Tan(angle) ; 
    angle = theta - TMath::Pi() / 2 ; 
    z = y0 * TMath::Tan(angle) ; 
  }
}

Bool_t  AliPHOSGeometry::Impact(const TParticle * particle) const 
{
  Bool_t In=kFALSE;
  Int_t ModuleNumber=0;
  Double_t z,x;
  ImpactOnEmc(particle->Theta(),particle->Phi(),ModuleNumber,z,x);
  if(ModuleNumber) In=kTRUE;
  else In=kFALSE;
  return In;
}

//____________________________________________________________________________
Bool_t AliPHOSGeometry::RelToAbsNumbering(const Int_t * relid, Int_t &  AbsId) const
{
  // Converts the relative numbering into the absolute numbering
  // EMCA crystals:
  //  AbsId = from 1 to fNModules * fNPhi * fNZ
  // CPV pad:
  //  AbsId = from N(total PHOS crystals) + 1
  //          to NCPVModules * fNumberOfCPVPadsPhi * fNumberOfCPVPadsZ

  Bool_t rv = kTRUE ; 
  
  if ( relid[1] ==  0 ) {                            // it is a Phos crystal
    AbsId =
      ( relid[0] - 1 ) * GetNPhi() * GetNZ()         // the offset of PHOS modules
      + ( relid[2] - 1 ) * GetNZ()                   // the offset along phi
      +   relid[3] ;                                 // the offset along z
  }
  else { // it is a CPV pad
    AbsId =    GetNPhi() * GetNZ() *  GetNModules()         // the offset to separate EMCA crystals from CPV pads
      + ( relid[0] - 1 ) * GetNumberOfCPVPadsPhi() * GetNumberOfCPVPadsZ()   // the pads offset of PHOS modules 
      + ( relid[2] - 1 ) * GetNumberOfCPVPadsZ()                             // the pads offset of a CPV row
      +   relid[3] ;                                                         // the column number
  }
  
  return rv ; 
}

//____________________________________________________________________________

void AliPHOSGeometry::RelPosInAlice(const Int_t id, TVector3 & pos ) const
{
  // Converts the absolute numbering into the global ALICE coordinate system
  
    
    Int_t relid[4] ;
    
    AbsToRelNumbering(id , relid) ;
    
    Int_t phosmodule = relid[0] ; 
    
    Float_t y0 = 0 ; 
    
    if ( relid[1] == 0 )  // it is a PbW04 crystal 
      y0 =  - GetIPtoCrystalSurface() ;  
    else
      y0 =  - GetIPtoUpperCPVsurface() ; 

    Float_t x, z ; 
    RelPosInModule(relid, x, z) ; 
    
    pos.SetX(x) ;
    pos.SetZ(z) ;
    pos.SetY(y0) ;
    
    Float_t phi           = GetPHOSAngle( phosmodule) ; 
    Double_t const kRADDEG = 180.0 / kPI ;
    Float_t rphi          = phi / kRADDEG ; 
    
    TRotation rot ;
    rot.RotateZ(-rphi) ; // a rotation around Z by angle  
    
    TRotation dummy = rot.Invert() ;  // to transform from original frame to rotate frame
    
    pos.Transform(rot) ; // rotate the baby 
} 

//____________________________________________________________________________
void AliPHOSGeometry::RelPosInModule(const Int_t * relid, Float_t & x, Float_t & z) const 
{
  // Converts the relative numbering into the local PHOS-module (x, z) coordinates
  // Note: sign of z differs from that in the previous version (Yu.Kharlov, 12 Oct 2000)
  
  Int_t row        = relid[2] ; //offset along x axis
  Int_t column     = relid[3] ; //offset along z axis

  
  if ( relid[1] == 0 ) { // its a PbW04 crystal
    x = - ( GetNPhi()/2. - row    + 0.5 ) *  GetCellStep() ; // position of Xtal with respect
    z =   ( GetNZ()  /2. - column + 0.5 ) *  GetCellStep() ; // of center of PHOS module  
  }  
  else  {    
    x = - ( GetNumberOfCPVPadsPhi()/2. - row    - 0.5 ) * GetPadSizePhi()  ; // position of pad  with respect
    z =   ( GetNumberOfCPVPadsZ()  /2. - column - 0.5 ) * GetPadSizeZ()  ; // of center of PHOS module  
  }
}

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

/* $Id$*/

//_________________________________________________________________________
// Geometry class  for EMCAL : singleton  
// EMCAL consists of layers of scintillator and lead
// Places the the Barrel Geometry of The EMCAL at Midrapidity
// between 80 and 180(or 190) degrees of Phi and
// -0.7 to 0.7 in eta 
// Number of Modules and Layers may be controlled by 
// the name of the instance defined               
//*-- Author: Sahal Yacoob (LBL / UCT)
//     and  : Yves Schutz (SUBATECH)
//     and  : Jennifer Klay (LBL)
//     SHASHLYK : Aleksei Pavlinov (WSU)
//     SuperModules -> module(or tower) -> cell

// --- AliRoot header files ---
#include <assert.h>
#include "Riostream.h"

#include <TMath.h>
#include <TVector3.h>
	      //#include <TArrayD.h>
#include <TObjArray.h>
#include <TGeoManager.h>
#include <TGeoNode.h>
#include <TGeoMatrix.h>
#include <TGeoNode.h>
#include <TList.h>
#include <TMatrixD.h>
#include <TObjString.h>
#include <TClonesArray.h>

// -- ALICE Headers.
//#include "AliConst.h"
#include "AliLog.h"

// --- EMCAL headers
#include "AliEMCALGeometry.h"
#include "AliEMCALShishKebabTrd1Module.h"
#include "AliEMCALRecPoint.h"
#include "AliEMCALDigit.h"
#include "AliEMCALHistoUtilities.h"
#include "AliEMCALAlignData.h"

ClassImp(AliEMCALGeometry)

// these initialisations are needed for a singleton
AliEMCALGeometry  *AliEMCALGeometry::fgGeom      = 0;
Bool_t             AliEMCALGeometry::fgInit      = kFALSE;
AliEMCALAlignData *AliEMCALGeometry::fgAlignData = 0;



AliEMCALGeometry::AliEMCALGeometry() : AliGeometry() 
{ 
  // default ctor only for internal usage (singleton)
  // must be kept public for root persistency purposes, but should never be called by the outside world    
  //  CreateListOfTrd1Modules();
  AliDebug(2, "AliEMCALGeometry : default ctor ");
}
//______________________________________________________________________
AliEMCALGeometry::AliEMCALGeometry(const Text_t* name, const Text_t* title) :
AliGeometry(name, title) {// ctor only for internal usage (singleton)
  AliDebug(2, Form("AliEMCALGeometry(%s,%s) ", name,title));
  Init();
  CreateListOfTrd1Modules();
}
//______________________________________________________________________
AliEMCALGeometry::~AliEMCALGeometry(void){
    // dtor
}
//______________________________________________________________________
void AliEMCALGeometry::Init(void){
  // Initializes the EMCAL parameters
  // naming convention : GUV_WX_N_ gives the composition of a tower
  // WX inform about the composition of the EM calorimeter section: 
  //   thickness in mm of Pb radiator (W) and of scintillator (X), and number of scintillator layers (N)
  // New geometry: EMCAL_55_25
  // 24-aug-04 for shish-kebab
  // SHISH_25 or SHISH_62
  // 11-oct-05   - correction for pre final design
  // Feb 06,2006 - decrease the weight of EMCAL

  fAdditionalOpts[0] = "nl=";    // number of sampling layers (fNECLayers)
  fAdditionalOpts[1] = "pbTh=";  // cm, Thickness of the Pb   (fECPbRadThick)
  fAdditionalOpts[2] = "scTh=";  // cm, Thickness of the Sc    (fECScintThick)
  fAdditionalOpts[3] = "latSS=";  // cm, Thickness of lateral steel strip (fLateralSteelStrip)

  fNAdditionalOpts = sizeof(fAdditionalOpts) / sizeof(char*);

  fgInit = kFALSE; // Assume failed until proven otherwise.
  fGeoName   = GetName();
  fGeoName.ToUpper();
  fKey110DEG = 0;
  if(fGeoName.Contains("110DEG")) fKey110DEG = 1; // for GetAbsCellId
  fShishKebabTrd1Modules = 0;
  fTrd2AngleY = f2Trd2Dy2 = fEmptySpace = fTubsR = fTubsTurnAngle = 0;

  fNZ             = 114;	// granularity along Z (eta) 
  fNPhi           = 168;	// granularity in phi (azimuth)
  fArm1PhiMin     = 60.0;	// degrees, Starting EMCAL Phi position
  fArm1PhiMax     = 180.0;	// degrees, Ending EMCAL Phi position
  fArm1EtaMin     = -0.7;	// pseudorapidity, Starting EMCAL Eta position
  fArm1EtaMax     = +0.7;	// pseudorapidity, Ending EMCAL Eta position
  fIPDistance     = 454.0;      // cm, Radial distance to inner surface of EMCAL
  fPhiGapForSM    = 0.;         // cm, only for final TRD1 geometry
  for(int i=0; i<12; i++) fMatrixOfSM[i] = 0;

  // geometry
  if(fGeoName.Contains("SHISH")){ // Only shahslyk now
    // 7-sep-05; integration issue
    fArm1PhiMin     = 80.0;	// 60  -> 80
    fArm1PhiMax     = 180.0;	// 180 -> 190

    fNumberOfSuperModules = 10; // 12 = 6 * 2 (6 in phi, 2 in Z);
    fSteelFrontThick = 2.54;    //  9-sep-04
    fIPDistance      = 460.0;
    fFrontSteelStrip = fPassiveScintThick = 0.0; // 13-may-05
    fLateralSteelStrip = 0.025; // before MAY 2005 
    fPhiModuleSize   = fEtaModuleSize   = 11.4;
    fPhiTileSize = fEtaTileSize      = 5.52; // (11.4-5.52*2)/2. = 0.18 cm (wall thickness)
    fNPhi            = 14;
    fNZ              = 30;
    fAlFrontThick    = fGap2Active = 0;
    fNPHIdiv = fNETAdiv = 2;

    fNECLayers       = 62;
    fECScintThick    = fECPbRadThickness = 0.2;
    fSampling        = 1.;  // 30-aug-04 - should be calculated
    if(fGeoName.Contains("TWIST")) { // all about EMCAL module
      fNZ             = 27;  // 16-sep-04
    } else if(fGeoName.Contains("TRD")) {
      fIPDistance      = 428.0;  //  11-may-05
      fSteelFrontThick = 0.0;    // 3.17 -> 0.0; 28-mar-05 : no stell plate
      fNPhi            = 12;
      fSampling       = 12.327;
      fPhiModuleSize = fEtaModuleSize = 12.26;
      fNZ            = 26;     // 11-oct-04
      fTrd1Angle     = 1.3;    // in degree
// 18-nov-04; 1./0.08112=12.327
// http://pdsfweb01.nersc.gov/~pavlinov/ALICE/SHISHKEBAB/RES/linearityAndResolutionForTRD1.html
      if(fGeoName.Contains("TRD1")) {       // 30-jan-05
	// for final design
        fPhiGapForSM    = 2.;         // cm, only for final TRD1 geometry
        if(fGeoName.Contains("MAY05") || fGeoName.Contains("WSUC") || fGeoName.Contains("FINAL")){
          fNumberOfSuperModules = 12; // 20-may-05
          if(fGeoName.Contains("WSUC")) fNumberOfSuperModules = 1; // 27-may-05
          fNECLayers     = 77;       // (13-may-05 from V.Petrov)
          fPhiModuleSize = 12.5;     // 20-may-05 - rectangular shape
          fEtaModuleSize = 11.9;
          fECScintThick  = fECPbRadThickness = 0.16;// (13-may-05 from V.Petrov)
          fFrontSteelStrip   = 0.025;// 0.025cm = 0.25mm  (13-may-05 from V.Petrov)
          fLateralSteelStrip = 0.01; // 0.01cm  = 0.1mm   (13-may-05 from V.Petrov) - was 0.025
          fPassiveScintThick = 0.8;  // 0.8cm   = 8mm     (13-may-05 from V.Petrov)
          fNZ                = 24;
          fTrd1Angle         = 1.5;  // 1.3 or 1.5

          if(fGeoName.Contains("FINAL")) { // 9-sep-05
            fNumberOfSuperModules = 10;
            if(fGeoName.Contains("110DEG")) {
              fNumberOfSuperModules = 12;// last two modules have size 10 degree in phi (180<phi<190)
              fArm1PhiMax = 200.0; // for XEN1 and turn angle of super modules
	    }
            fPhiModuleSize = 12.26 - fPhiGapForSM / Float_t(fNPhi); // first assumption
            fEtaModuleSize = fPhiModuleSize;
            if(fGeoName.Contains("HUGE")) fNECLayers *= 3; // 28-oct-05 for analysing leakage    
          }
	}
      } else if(fGeoName.Contains("TRD2")) {       // 30-jan-05
        fSteelFrontThick = 0.0;         // 11-mar-05
        fIPDistance+= fSteelFrontThick; // 1-feb-05 - compensate absence of steel plate
        fTrd1Angle  = 1.64;             // 1.3->1.64
        fTrd2AngleY = fTrd1Angle;       //  symmetric case now
        fEmptySpace    = 0.2; // 2 mm
        fTubsR         = fIPDistance; // 31-jan-05 - as for Fred case

        fPhiModuleSize  = fTubsR*2.*TMath::Tan(fTrd2AngleY*TMath::DegToRad()/2.);
        fPhiModuleSize -= fEmptySpace/2.; // 11-mar-05  
        fEtaModuleSize  = fPhiModuleSize; // 20-may-05 
        fTubsTurnAngle  = 3.;
      }
      fNPHIdiv = fNETAdiv  = 2;   // 13-oct-04 - division again
      if(fGeoName.Contains("3X3")) {   // 23-nov-04
        fNPHIdiv = fNETAdiv  = 3;
      } else if(fGeoName.Contains("4X4")) {
        fNPHIdiv = fNETAdiv  = 4;
      }
    }
    if(fGeoName.Contains("25")){
      fNECLayers     = 25;
      fECScintThick  = fECPbRadThickness = 0.5;
    }
    if(fGeoName.Contains("WSUC")){ // 18-may-05 - about common structure
      fShellThickness = 30.; // should be change 
      fNPhi = fNZ = 4; 
    }

    CheckAdditionalOptions();
    DefineSamplingFraction();

    fPhiTileSize = fPhiModuleSize/2. - fLateralSteelStrip; // 13-may-05 
    fEtaTileSize = fEtaModuleSize/2. - fLateralSteelStrip; // 13-may-05 

    // constant for transition absid <--> indexes
    fNCellsInTower  = fNPHIdiv*fNETAdiv;
    fNCellsInSupMod = fNCellsInTower*fNPhi*fNZ;
    fNCells         = fNCellsInSupMod*fNumberOfSuperModules;
    if(fGeoName.Contains("110DEG")) fNCells -= fNCellsInSupMod;

    fLongModuleSize = fNECLayers*(fECScintThick + fECPbRadThickness);
    if(fGeoName.Contains("MAY05")) fLongModuleSize += (fFrontSteelStrip + fPassiveScintThick);

    // 30-sep-04
    if(fGeoName.Contains("TRD")) {
      f2Trd1Dx2 = fEtaModuleSize + 2.*fLongModuleSize*TMath::Tan(fTrd1Angle*TMath::DegToRad()/2.);
      if(fGeoName.Contains("TRD2")) {  // 27-jan-05
        f2Trd2Dy2 = fPhiModuleSize + 2.*fLongModuleSize*TMath::Tan(fTrd2AngleY*TMath::DegToRad()/2.);
      }
    }
  } else Fatal("Init", "%s is an undefined geometry!", fGeoName.Data()) ; 

  fNPhiSuperModule = fNumberOfSuperModules/2;
  if(fNPhiSuperModule<1) fNPhiSuperModule = 1;
  //There is always one more scintillator than radiator layer because of the first block of aluminium
  fShellThickness = fAlFrontThick + fGap2Active + fNECLayers*GetECScintThick()+(fNECLayers-1)*GetECPbRadThick();
  if(fGeoName.Contains("SHISH")) {
    fShellThickness = fSteelFrontThick + fLongModuleSize;
    if(fGeoName.Contains("TWIST")) { // 13-sep-04
      fShellThickness  = TMath::Sqrt(fLongModuleSize*fLongModuleSize + fPhiModuleSize*fEtaModuleSize);
      fShellThickness += fSteelFrontThick;
    } else if(fGeoName.Contains("TRD")) { // 1-oct-04
      fShellThickness  = TMath::Sqrt(fLongModuleSize*fLongModuleSize + f2Trd1Dx2*f2Trd1Dx2);
      fShellThickness += fSteelFrontThick;
      // Local coordinates
      fParSM[0] = GetShellThickness()/2.;        
      fParSM[1] = GetPhiModuleSize() * GetNPhi()/2.;
      fParSM[2] = 350./2.;
    }
  }

  fZLength        = 2.*ZFromEtaR(fIPDistance+fShellThickness,fArm1EtaMax); // Z coverage
  fEnvelop[0]     = fIPDistance; // mother volume inner radius
  fEnvelop[1]     = fIPDistance + fShellThickness; // mother volume outer r.
  fEnvelop[2]     = 1.00001*fZLength; // add some padding for mother volume. 
  
  if(fgAlignData != NULL) {
    // Number of modules is read from Alignment DB if exists
    fNumberOfSuperModules = fgAlignData->GetNSuperModules();
  }
 
  fgInit = kTRUE; 
  
  if (AliDebugLevel()>=2) {
    printf("Init: geometry of EMCAL named %s is as follows:\n", fGeoName.Data());
    printf( "               ECAL      : %d x (%f cm Pb, %f cm Sc) \n", 
    GetNECLayers(), GetECPbRadThick(), GetECScintThick() ) ; 
    printf("                fSampling %5.2f \n",  fSampling );
    if(fGeoName.Contains("SHISH")){
      printf(" fIPDistance       %6.3f cm \n", fIPDistance);
      if(fSteelFrontThick>0.) 
      printf(" fSteelFrontThick  %6.3f cm \n", fSteelFrontThick);
      printf(" fNPhi %i   |  fNZ %i \n", fNPhi, fNZ);
      printf(" fNCellsInTower %i : fNCellsInSupMod %i : fNCells %i\n",fNCellsInTower, fNCellsInSupMod, fNCells);
      if(fGeoName.Contains("MAY05")){
	printf(" fFrontSteelStrip         %6.4f cm (thickness of front steel strip)\n", 
        fFrontSteelStrip);
	printf(" fLateralSteelStrip       %6.4f cm (thickness of lateral steel strip)\n", 
        fLateralSteelStrip);
 	printf(" fPassiveScintThick  %6.4f cm (thickness of front passive Sc tile)\n",
        fPassiveScintThick);
      }
      printf(" X:Y module size     %6.3f , %6.3f cm \n", fPhiModuleSize, fEtaModuleSize);
      printf(" X:Y   tile size     %6.3f , %6.3f cm \n", fPhiTileSize, fEtaTileSize);
      printf(" #of sampling layers %i(fNECLayers) \n", fNECLayers);
      printf(" fLongModuleSize     %6.3f cm \n", fLongModuleSize);
      printf(" #supermodule in phi direction %i \n", fNPhiSuperModule );
    }
    if(fGeoName.Contains("TRD")) {
      printf(" fTrd1Angle %7.4f\n", fTrd1Angle);
      printf(" f2Trd1Dx2  %7.4f\n",  f2Trd1Dx2);
      if(fGeoName.Contains("TRD2")) {
        printf(" fTrd2AngleY     %7.4f\n", fTrd2AngleY);
        printf(" f2Trd2Dy2       %7.4f\n", f2Trd2Dy2);
        printf(" fTubsR          %7.2f cm\n", fTubsR);
        printf(" fTubsTurnAngle  %7.4f\n", fTubsTurnAngle);
        printf(" fEmptySpace     %7.4f cm\n", fEmptySpace);
      } else if(fGeoName.Contains("TRD1") && fGeoName.Contains("FINAL")){
        printf("SM dimensions(TRD1) : dx %7.2f dy %7.2f dz %7.2f (SMOD, BOX)\n", 
        fParSM[0],fParSM[1],fParSM[2]);
        printf(" fPhiGapForSM  %7.4f cm \n",  fPhiGapForSM);
        if(fGeoName.Contains("110DEG"))printf(" Last two modules have size 10 degree in  phi (180<phi<190)\n");
      }
    }
    printf("Granularity: %d in eta and %d in phi\n", GetNZ(), GetNPhi()) ;
    printf("Layout: phi = (%7.1f, %7.1f), eta = (%5.2f, %5.2f), IP = %7.2f\n",  
	   GetArm1PhiMin(), GetArm1PhiMax(),GetArm1EtaMin(), GetArm1EtaMax(), GetIPDistance() );
  }
  //TRU parameters. These parameters values are not the final ones.
  fNTRU    = 3 ;
  fNTRUEta = 3 ;
  fNTRUPhi = 1 ;
}

//______________________________________________________________________

void AliEMCALGeometry::CheckAdditionalOptions()
{
  // Feb 06,2006
  //Additional options that
  //can be used to select
  //the specific geometry of 
  //EMCAL to run

  fArrayOpts = new TObjArray;
  Int_t nopt = AliEMCALHistoUtilities::ParseString(fGeoName, *fArrayOpts);
  if(nopt==1) { // no aditional option(s)
    fArrayOpts->Delete();
    delete fArrayOpts;
    fArrayOpts = 0; 
    return;
  }  		 
  for(Int_t i=1; i<nopt; i++){
    TObjString *o = (TObjString*)fArrayOpts->At(i); 

    TString addOpt = o->String();
    Int_t indj=-1;
    for(Int_t j=0; j<fNAdditionalOpts; j++) {
      TString opt = fAdditionalOpts[j];
      if(addOpt.Contains(opt,TString::kIgnoreCase)) {
	  indj = j;
        break;
      }
    }
    if(indj<0) {
      AliDebug(2,Form("<E> option |%s| unavailable : ** look to the file AliEMCALGeometry.h **\n", 
		      addOpt.Data()));
      assert(0);
    } else {
      AliDebug(2,Form("<I> option |%s| is valid : number %i : |%s|\n", 
		      addOpt.Data(), indj, fAdditionalOpts[indj]));
      if       (addOpt.Contains("NL=",TString::kIgnoreCase))   {// number of sampling layers
        sscanf(addOpt.Data(),"NL=%i", &fNECLayers);
        AliDebug(2,Form(" fNECLayers %i (new) \n", fNECLayers));
      } else if(addOpt.Contains("PBTH=",TString::kIgnoreCase)) {//Thickness of the Pb(fECPbRadThicknes)
        sscanf(addOpt.Data(),"PBTH=%f", &fECPbRadThickness);
      } else if(addOpt.Contains("SCTH=",TString::kIgnoreCase)) {//Thickness of the Sc(fECScintThick)
        sscanf(addOpt.Data(),"SCTH=%f", &fECScintThick);
      } else if(addOpt.Contains("LATSS=",TString::kIgnoreCase)) {// Thickness of lateral steel strip (fLateralSteelStrip)
        sscanf(addOpt.Data(),"LATSS=%f", &fLateralSteelStrip);
        AliDebug(2,Form(" fLateralSteelStrip %f (new) \n", fLateralSteelStrip));
      }
    }
  }
}

void AliEMCALGeometry::DefineSamplingFraction()
{
  // Jun 05,2006
  // Look http://rhic.physics.wayne.edu/~pavlinov/ALICE/SHISHKEBAB/RES/linearityAndResolutionForTRD1.html
  // Keep for compatibilty
  //
  if(fNECLayers == 69) {        // 10% layer reduction
    fSampling = 12.55;
  } else if(fNECLayers == 61) { // 20% layer reduction
    fSampling = 12.80;
  } else if(fNECLayers == 77) {
    if       (fECScintThick>0.175 && fECScintThick<0.177) { // 10% Pb thicknes reduction
      fSampling = 10.5; // fECScintThick = 0.176, fECPbRadThickness=0.144;
    } else if(fECScintThick>0.191 && fECScintThick<0.193) { // 20% Pb thicknes reduction
      fSampling = 8.93; // fECScintThick = 0.192, fECPbRadThickness=0.128;
    }
  }
}

//____________________________________________________________________________
void AliEMCALGeometry::FillTRU(const TClonesArray * digits, TClonesArray * ampmatrix, TClonesArray * timeRmatrix) {


//  Orders digits ampitudes list in fNTRU TRUs (384 cells) per supermodule. 
//  Each TRU is a TMatrixD, and they are kept in TClonesArrays. The number of 
//  TRU in phi is fNTRUPhi, and the number of TRU in eta is fNTRUEta.
//  Last 2 modules are half size in Phi, I considered that the number of TRU
//  is maintained for the last modules but decision not taken. If different, 
//  then this must be changed. 
 

  //Check data members

  if(fNTRUEta*fNTRUPhi != fNTRU)
    Error("FillTRU"," Wrong number of TRUS per Eta or Phi");

  //Initilize and declare variables
  //List of TRU matrices initialized to 0.
  Int_t nCellsPhi  = fNPhi*2/fNTRUPhi;
  Int_t nCellsPhi2 = fNPhi/fNTRUPhi; //HalfSize modules
  Int_t nCellsEta  = fNZ*2/fNTRUEta;
  Int_t id      = -1; 
  Float_t amp   = -1;
  Float_t timeR = -1;
  Int_t iSupMod = -1;
  Int_t nTower  = -1;
  Int_t nIphi   = -1;
  Int_t nIeta   = -1;
  Int_t iphi    = -1;
  Int_t ieta    = -1;

  //List of TRU matrices initialized to 0.
  for(Int_t k = 0; k < fNTRU*fNumberOfSuperModules; k++){
    TMatrixD  * amptrus   = new TMatrixD(nCellsPhi,nCellsEta) ;
    TMatrixD  * timeRtrus = new TMatrixD(nCellsPhi,nCellsEta) ;
    for(Int_t i = 0; i < nCellsPhi; i++){
      for(Int_t j = 0; j < nCellsEta; j++){
	(*amptrus)(i,j) = 0.0;
	(*timeRtrus)(i,j) = 0.0;
      }
    }
    new((*ampmatrix)[k])   TMatrixD(*amptrus) ;
    new((*timeRmatrix)[k]) TMatrixD(*timeRtrus) ; 
  }
  
  AliEMCALDigit * dig ;
  
  //Digits loop to fill TRU matrices with amplitudes.
  for(Int_t idig = 0 ; idig < digits->GetEntriesFast() ; idig++){
    
    dig = dynamic_cast<AliEMCALDigit *>(digits->At(idig)) ;
    amp    = dig->GetAmp() ;   // Energy of the digit (arbitrary units)
    id     = dig->GetId() ;    // Id label of the cell
    timeR  = dig->GetTimeR() ; // Earliest time of the digit
   
    //Get eta and phi cell position in supermodule
    Bool_t bCell = GetCellIndex(id, iSupMod, nTower, nIphi, nIeta) ;
    if(!bCell)
      Error("FillTRU","Wrong cell id number") ;
    
    GetCellPhiEtaIndexInSModule(iSupMod,nTower,nIphi, nIeta,iphi,ieta);

    //Check to which TRU in the supermodule belongs the cell. 
    //Supermodules are divided in a TRU matrix of dimension 
    //(fNTRUPhi,fNTRUEta).
    //Each TRU is a cell matrix of dimension (nCellsPhi,nCellsEta)

    //First calculate the row and column in the supermodule 
    //of the TRU to which the cell belongs.
    Int_t col   = ieta/nCellsEta; 
    Int_t row   = iphi/nCellsPhi; 
    if(iSupMod > 9)
      row   = iphi/nCellsPhi2; 
    //Calculate label number of the TRU
    Int_t itru  = row + col*fNTRUPhi + iSupMod*fNTRU ;  
 
    //Fill TRU matrix with cell values
    TMatrixD * amptrus   = dynamic_cast<TMatrixD *>(ampmatrix->At(itru)) ;
    TMatrixD * timeRtrus = dynamic_cast<TMatrixD *>(timeRmatrix->At(itru)) ;

    //Calculate row and column of the cell inside the TRU with number itru
    Int_t irow = iphi - row *  nCellsPhi;
    if(iSupMod > 9)
      irow = iphi - row *  nCellsPhi2;
    Int_t icol = ieta - col *  nCellsEta;
    
    (*amptrus)(irow,icol) = amp ;
    (*timeRtrus)(irow,icol) = timeR ;

  }
}

//______________________________________________________________________
void AliEMCALGeometry::GetCellPhiEtaIndexInSModuleFromTRUIndex(const Int_t itru, const Int_t iphitru, const Int_t ietatru, Int_t &iphiSM, Int_t &ietaSM) const 
{
  
  // This method transforms the (eta,phi) index of cells in a 
  // TRU matrix into Super Module (eta,phi) index.
  
  // Calculate in which row and column where the TRU are 
  // ordered in the SM

  Int_t col = itru/ fNTRUPhi ;
  Int_t row = itru - col*fNTRUPhi ;
   
  //Calculate the (eta,phi) index in SM
  Int_t nCellsPhi = fNPhi*2/fNTRUPhi;
  Int_t nCellsEta = fNZ*2/fNTRUEta;
  
  iphiSM = nCellsPhi*row + iphitru  ;
  ietaSM = nCellsEta*col + ietatru  ; 
}

//______________________________________________________________________
AliEMCALGeometry *  AliEMCALGeometry::GetInstance(){ 
  // Returns the pointer of the unique instance
  
  AliEMCALGeometry * rv = static_cast<AliEMCALGeometry *>( fgGeom );
  return rv; 
}

//______________________________________________________________________
AliEMCALGeometry* AliEMCALGeometry::GetInstance(const Text_t* name,
						const Text_t* title){
    // Returns the pointer of the unique instance

    AliEMCALGeometry * rv = 0; 
    if ( fgGeom == 0 ) {
	if ( strcmp(name,"") == 0 ) rv = 0;
	else {
	    fgGeom = new AliEMCALGeometry(name, title);
	    if ( fgInit ) rv = (AliEMCALGeometry * ) fgGeom;
	    else {
		rv = 0; 
		delete fgGeom; 
		fgGeom = 0; 
	    } // end if fgInit
	} // end if strcmp(name,"")
    }else{
	if ( strcmp(fgGeom->GetName(), name) != 0) {
	  printf("\ncurrent geometry is %s : ", fgGeom->GetName());
	  printf(" you cannot call %s ", name);  
	}else{
	  rv = (AliEMCALGeometry *) fgGeom; 
	} // end 
    }  // end if fgGeom
    return rv; 
}

Bool_t AliEMCALGeometry::IsInEMCAL(Double_t x, Double_t y, Double_t z) const {
  // Checks whether point is inside the EMCal volume, used in AliEMCALv*.cxx
  //
  // Code uses cylindrical approximation made of inner radius (for speed)
  //
  // Points behind EMCAl, i.e. R > outer radius, but eta, phi in acceptance 
  // are considered to inside

  Double_t r=sqrt(x*x+y*y);

  if ( r > fEnvelop[0] ) {
     Double_t theta;
     theta  =    TMath::ATan2(r,z);
     Double_t eta;
     if(theta == 0) 
       eta = 9999;
     else 
       eta    =   -TMath::Log(TMath::Tan(theta/2.));
     if (eta < fArm1EtaMin || eta > fArm1EtaMax)
       return 0;
 
     Double_t phi = TMath::ATan2(y,x) * 180./TMath::Pi();
     if (phi > fArm1PhiMin && phi < fArm1PhiMax)
       return 1;
  }
  return 0;
}
// ==

//
// == Shish-kebab cases ==
//
Int_t AliEMCALGeometry::GetAbsCellId(Int_t nSupMod, Int_t nTower, Int_t nIphi, Int_t nIeta) const
{ 
  // 27-aug-04; 
  // corr. 21-sep-04; 
  //       13-oct-05; 110 degree case
  // May 31, 2006; ALICE numbering scheme:
  // 0 <= nSupMod < fNumberOfSuperModules
  // 0 <= nTower  < fNPHI * fNZ ( fNPHI * fNZ/2 for fKey110DEG=1)
  // 0 <= nIphi   < fNPHIdiv
  // 0 <= nIeta   < fNETAdiv
  // 0 <= absid   < fNCells
  static Int_t id=0; // have to change from 0 to fNCells-1
  if(fKey110DEG == 1 && nSupMod >= 10) { // 110 degree case; last two supermodules
    id  = fNCellsInSupMod*10 + (fNCellsInSupMod/2)*(nSupMod-10);
  } else {
    id  = fNCellsInSupMod*nSupMod;
  }
  id += fNCellsInTower *nTower;
  id += fNPHIdiv *nIphi;
  id += nIeta;
  if(id<0 || id >= fNCells) {
//     printf(" wrong numerations !!\n");
//     printf("    id      %6i(will be force to -1)\n", id);
//     printf("    fNCells %6i\n", fNCells);
//     printf("    nSupMod %6i\n", nSupMod);
//     printf("    nTower  %6i\n", nTower);
//     printf("    nIphi   %6i\n", nIphi);
//     printf("    nIeta   %6i\n", nIeta);
    id = -TMath::Abs(id); // if negative something wrong
  }
  return id;
}

Bool_t  AliEMCALGeometry::CheckAbsCellId(Int_t absId) const
{ 
  // May 31, 2006; only trd1 now
  if(absId<0 || absId >= fNCells) return kFALSE;
  else                            return kTRUE;
}

Bool_t AliEMCALGeometry::GetCellIndex(Int_t absId,Int_t &nSupMod,Int_t &nTower,Int_t &nIphi,Int_t &nIeta) const
{ 
  // 21-sep-04; 19-oct-05;
  // May 31, 2006; ALICE numbering scheme:
  static Int_t tmp=0, sm10=0;
  if(!CheckAbsCellId(absId)) return kFALSE;

  sm10 = fNCellsInSupMod*10;
  if(fKey110DEG == 1 && absId >= sm10) { // 110 degree case; last two supermodules  
    nSupMod = (absId-sm10) / (fNCellsInSupMod/2) + 10;
    tmp     = (absId-sm10) % (fNCellsInSupMod/2);
  } else {
    nSupMod = absId / fNCellsInSupMod;
    tmp     = absId % fNCellsInSupMod;
  }

  nTower  = tmp / fNCellsInTower;
  tmp     = tmp % fNCellsInTower;
  nIphi   = tmp / fNPHIdiv;
  nIeta   = tmp % fNPHIdiv;

  return kTRUE;
}

void AliEMCALGeometry::GetModulePhiEtaIndexInSModule(Int_t nSupMod, Int_t nTower,  int &iphim, int &ietam) const
{ 
  // added nSupMod; have to check  - 19-oct-05 !
  // Alice numbering scheme        - Jun 01,2006 
  static Int_t nphi;

  if(fKey110DEG == 1 && nSupMod>=10) nphi = fNPhi/2;
  else                               nphi = fNPhi;

  ietam = nTower/nphi; // have to change from 0 to fNZ-1
  iphim = nTower%nphi; // have to change from 0 to fNPhi-1
}

void AliEMCALGeometry::GetCellPhiEtaIndexInSModule(Int_t nSupMod, Int_t nTower, Int_t nIphi, Int_t nIeta, 
int &iphi, int &ieta) const
{ 
  // added nSupMod; Nov 25, 05
  // Alice numbering scheme        - Jun 01,2006 
  static Int_t iphim, ietam;

  GetModulePhiEtaIndexInSModule(nSupMod,nTower, iphim, ietam); 
  // have to change from 0 to (fNZ*fNETAdiv-1)
  ieta  = ietam*fNETAdiv + (1-nIeta); // x(module) = -z(SM) 
  // iphi - have to change from 0 to (fNPhi*fNPHIdiv-1)
  iphi  = iphim*fNPHIdiv + nIphi;     // y(module) =  y(SM) 
}

Int_t  AliEMCALGeometry::GetSuperModuleNumber(Int_t absId)  const
{
  //return the number of the 
  //supermodule given the absolute
  //ALICE numbering

  static Int_t nSupMod, nTower, nIphi, nIeta;
  GetCellIndex(absId, nSupMod, nTower, nIphi, nIeta);
  return nSupMod;
} 

// Methods for AliEMCALRecPoint - Feb 19, 2006
Bool_t AliEMCALGeometry::RelPosCellInSModule(Int_t absId, Double_t &xr, Double_t &yr, Double_t &zr) const
{
  // Look to see what the relative
  // position inside a given cell is
  // for a recpoint.
  // Alice numbering scheme - Jun 08, 2006

  static Int_t nSupMod, nTower, nIphi, nIeta, iphi, ieta;
  static Int_t phiIndexShift=6;
  if(!CheckAbsCellId(absId)) return kFALSE;

  GetCellIndex(absId, nSupMod, nTower, nIphi, nIeta);
  GetCellPhiEtaIndexInSModule(nSupMod,nTower,nIphi,nIeta, iphi, ieta); 
 
  xr = fXCentersOfCells.At(ieta);
  zr = fEtaCentersOfCells.At(ieta);

  if(nSupMod<10) {
    yr = fPhiCentersOfCells.At(iphi);
  } else {
    yr = fPhiCentersOfCells.At(iphi + phiIndexShift);
    //    cout<<" absId "<<absId<<" nSupMod "<<nSupMod << " iphi "<<iphi<<" ieta "<<ieta;
    //    cout<< " xr " << xr << " yr " << yr << " zr " << zr <<endl;
  }

  return kTRUE;
}

Bool_t AliEMCALGeometry::RelPosCellInSModule(Int_t absId, Double_t loc[3]) const
{
  // Alice numbering scheme - Jun 03, 2006
  loc[0] = loc[1] = loc[2]=0.0;
  if(RelPosCellInSModule(absId, loc[0],loc[1],loc[2])) {
    return kTRUE;
  }
  return kFALSE;
}

Bool_t AliEMCALGeometry::RelPosCellInSModule(Int_t absId, TVector3 &vloc) const
{
  static Double_t loc[3];
  if(RelPosCellInSModule(absId,loc)) {
    vloc.SetXYZ(loc[0], loc[1], loc[2]);
    return kTRUE;
  } else {
    vloc.SetXYZ(0,0,0);
    return kFALSE;
  }
  // Alice numbering scheme - Jun 03, 2006
}

void AliEMCALGeometry::CreateListOfTrd1Modules()
{
  //Generate the list of Trd1 modules
  //which will make up the EMCAL
  //geometry

  AliDebug(2,Form(" AliEMCALGeometry::CreateListOfTrd1Modules() started "));

  AliEMCALShishKebabTrd1Module *mod=0, *mTmp=0; // current module
  if(fShishKebabTrd1Modules == 0) {
    fShishKebabTrd1Modules = new TList;
    for(int iz=0; iz< GetNZ(); iz++) { 
      if(iz==0) { 
        mod  = new AliEMCALShishKebabTrd1Module(TMath::Pi()/2.,this);
      } else {
        mTmp  = new AliEMCALShishKebabTrd1Module(*mod);
        mod   = mTmp;
      }
      fShishKebabTrd1Modules->Add(mod);
    }
  } else {
    AliDebug(2,Form(" Already exits : "));
  }
  AliDebug(2,Form(" fShishKebabTrd1Modules has %i modules \n", 
		  fShishKebabTrd1Modules->GetSize()));
  // Feb 20,2006;
  // Jun 01, 2006 - ALICE numbering scheme
  // define grid for cells in eta(z) and x directions in local coordinates system of SM
  //  fEtaCentersOfCells = new TArrayD(fNZ *fNETAdiv);
  //  fXCentersOfCells = new TArrayD(fNZ *fNETAdiv);
  fEtaCentersOfCells.Set(fNZ *fNETAdiv);
  fXCentersOfCells.Set(fNZ *fNETAdiv);
  AliDebug(2,Form(" Cells grid in eta directions : size %i\n", fEtaCentersOfCells.GetSize()));
  Int_t iphi=0, ieta=0, nTower=0;
  Double_t xr, zr;
  for(Int_t it=0; it<fNZ; it++) { // array index
    AliEMCALShishKebabTrd1Module *trd1 = GetShishKebabModule(it);
    nTower = fNPhi*it;
    for(Int_t ic=0; ic<fNETAdiv; ic++) { // array index
      trd1->GetCenterOfCellInLocalCoordinateofSM(ic, xr, zr);
      GetCellPhiEtaIndexInSModule(0, nTower, 0, ic, iphi, ieta); // don't depend from phi - ieta in action
      fXCentersOfCells.AddAt(float(xr) - fParSM[0],ieta);
      fEtaCentersOfCells.AddAt(float(zr) - fParSM[2],ieta);
    }
  }
  for(Int_t i=0; i<fEtaCentersOfCells.GetSize(); i++) {
    AliDebug(2,Form(" ind %2.2i : z %8.3f : x %8.3f", i+1, 
                    fEtaCentersOfCells.At(i),fXCentersOfCells.At(i)));
  }

 // define grid for cells in phi(y) direction in local coordinates system of SM
  //  fPhiCentersOfCells = new TArrayD(fNPhi*fNPHIdiv);
  fPhiCentersOfCells.Set(fNPhi*fNPHIdiv);
  AliDebug(2,Form(" Cells grid in phi directions : size %i\n", fPhiCentersOfCells.GetSize()));
  Int_t ind=0;
  for(Int_t it=0; it<fNPhi; it++) { // array index
    Float_t ytLeftCenterModule = -fParSM[1] + fPhiModuleSize*(2*it+1)/2;         // module
    for(Int_t ic=0; ic<fNPHIdiv; ic++) { // array index
      Float_t ytLeftCenterCell = ytLeftCenterModule + fPhiTileSize *(2*ic-1)/2.; // tower(cell) 
      fPhiCentersOfCells.AddAt(ytLeftCenterCell,ind);
      AliDebug(2,Form(" ind %2.2i : y %8.3f ", ind, fPhiCentersOfCells.At(ind))); 
      ind++;
    }
  }
}

void  AliEMCALGeometry::GetTransformationForSM()
{
  //Uses the geometry manager to
  //load the transformation matrix
  //for the supermodules

  static Bool_t transInit=kFALSE;
  if(transInit) return;

  int i=0;
  if(gGeoManager == 0) {
    Info("CreateTransformationForSM() "," Load geometry : TGeoManager::Import()");
    assert(0);
  }
  TGeoNode *tn = gGeoManager->GetTopNode();
  TGeoNode *node=0, *xen1 = 0;
  for(i=0; i<tn->GetNdaughters(); i++) {
    node = tn->GetDaughter(i);
    TString ns(node->GetName());
    if(ns.Contains(GetNameOfEMCALEnvelope())) {
      xen1 = node;
      break;
    }
  }
  if(!xen1) {
    Info("CreateTransformationForSM() "," geometry has not EMCAL envelope with name %s", 
    GetNameOfEMCALEnvelope());
    assert(0);
  }
  printf(" i %i : EMCAL Envelope is %s : #SM %i \n", i, xen1->GetName(), xen1->GetNdaughters());
  for(i=0; i<xen1->GetNdaughters(); i++) {
    TGeoNodeMatrix *sm = (TGeoNodeMatrix*)xen1->GetDaughter(i);
    fMatrixOfSM[i] = sm->GetMatrix();
    //Compiler doesn't like this syntax...
    //    printf(" %i : matrix %x \n", i, fMatrixOfSM[i]);
  }
  transInit = kTRUE;
}

void AliEMCALGeometry::GetGlobal(const Double_t *loc, Double_t *glob, int ind) const
{
  // Figure out the global numbering
  // of a given supermodule from the
  // local numbering
  // Alice numbering - Jun 03,2006
  //  if(fMatrixOfSM[0] == 0) GetTransformationForSM();

  if(ind>=0 && ind < GetNumberOfSuperModules()) {
    fMatrixOfSM[ind]->LocalToMaster(loc, glob);
  }
}

void AliEMCALGeometry::GetGlobal(const TVector3 &vloc, TVector3 &vglob, int ind) const
{
  //Figure out the global numbering
  //of a given supermodule from the
  //local numbering given a 3-vector location

  static Double_t tglob[3], tloc[3];
  vloc.GetXYZ(tloc);
  GetGlobal(tloc, tglob, ind);
  vglob.SetXYZ(tglob[0], tglob[1], tglob[2]);
}

void AliEMCALGeometry::GetGlobal(Int_t absId , double glob[3]) const
{ 
  // Alice numbering scheme - Jun 03, 2006
  static Int_t nSupMod, nModule, nIphi, nIeta;
  static double loc[3];

  glob[0]=glob[1]=glob[2]=0.0; // bad case
  if(RelPosCellInSModule(absId, loc)) {
    GetCellIndex(absId, nSupMod, nModule, nIphi, nIeta);
    fMatrixOfSM[nSupMod]->LocalToMaster(loc, glob);
  }
}

void AliEMCALGeometry::GetGlobal(Int_t absId , TVector3 &vglob) const
{ 
  // Alice numbering scheme - Jun 03, 2006
  static Double_t glob[3];

  GetGlobal(absId, glob);
  vglob.SetXYZ(glob[0], glob[1], glob[2]);

}

void AliEMCALGeometry::GetGlobal(const AliRecPoint *rp, TVector3 &vglob) const
{
  // Figure out the global numbering
  // of a given supermodule from the
  // local numbering for RecPoints

  static TVector3 vloc;
  static Int_t nSupMod, nModule, nIphi, nIeta;

  AliRecPoint *rpTmp = (AliRecPoint*)rp; // const_cast ??
  if(!rpTmp) return;
  AliEMCALRecPoint *rpEmc = (AliEMCALRecPoint*)rpTmp;

  GetCellIndex(rpEmc->GetAbsId(0), nSupMod, nModule, nIphi, nIeta);
  rpTmp->GetLocalPosition(vloc);
  GetGlobal(vloc, vglob, nSupMod);
}

void AliEMCALGeometry::EtaPhiFromIndex(Int_t absId,Float_t &eta,Float_t &phi) const
{
  // Jun 03, 2006 - version for TRD1
  static TVector3 vglob;
  GetGlobal(absId, vglob);
  eta = vglob.Eta();
  phi = vglob.Phi();
}

AliEMCALShishKebabTrd1Module* AliEMCALGeometry::GetShishKebabModule(Int_t neta=0)
{
  //This method was too long to be
  //included in the header file - the
  //rule checker complained about it's
  //length, so we move it here.  It returns the
  //shishkebabmodule at a given eta index point.

  static AliEMCALShishKebabTrd1Module* trd1=0;
  if(fShishKebabTrd1Modules && neta>=0 && neta<fShishKebabTrd1Modules->GetSize()) {
    trd1 = (AliEMCALShishKebabTrd1Module*)fShishKebabTrd1Modules->At(neta);
  } else trd1 = 0;
  return trd1;
}

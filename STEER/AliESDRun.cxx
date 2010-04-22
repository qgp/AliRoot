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
#include <TNamed.h>
#include <TGeoMatrix.h>
#include <TGeoGlobalMagField.h>

#include "AliESDRun.h"
#include "AliESDVertex.h"
#include "AliLog.h"
#include "AliMagF.h"

//-------------------------------------------------------------------------
//                     Implementation Class AliESDRun
//   Run by run data
//   for the ESD   
//   Origin: Christian Klein-Boesing, CERN, Christian.Klein-Boesing@cern.ch 
//-------------------------------------------------------------------------

ClassImp(AliESDRun)  
 
//______________________________________________________________________________
AliESDRun::AliESDRun() :
  TObject(),
  fCurrentL3(0),
  fCurrentDip(0),
  fBeamEnergy(0),
  fMagneticField(0),
  fPeriodNumber(0),
  fRunNumber(0),
  fRecoVersion(0),
  fBeamType(""),
  fTriggerClasses(kNTriggerClasses),
  fDetInDAQ(0),
  fDetInReco(0)
{
  for (Int_t i=0; i<2; i++) fDiamondXY[i]=0.;
  fDiamondCovXY[0]=fDiamondCovXY[2]=3.*3.;
  fDiamondCovXY[1]=0.;
  fTriggerClasses.SetOwner(kTRUE);
  for (Int_t m=0; m<kNPHOSMatrix; m++) fPHOSMatrix[m]=NULL;
  for (Int_t sm=0; sm<kNEMCALMatrix; sm++) fEMCALMatrix[sm]=NULL;
}

//______________________________________________________________________________
AliESDRun::AliESDRun(const AliESDRun &esd) :
  TObject(esd),
  fCurrentL3(0),
  fCurrentDip(0),
  fBeamEnergy(0),
  fMagneticField(esd.fMagneticField),
  fPeriodNumber(esd.fPeriodNumber),
  fRunNumber(esd.fRunNumber),
  fRecoVersion(esd.fRecoVersion),
  fBeamType(""),
  fTriggerClasses(TObjArray(kNTriggerClasses)),
  fDetInDAQ(0),
  fDetInReco(0)
{ 
  // Copy constructor
  for (Int_t i=0; i<2; i++) fDiamondXY[i]=esd.fDiamondXY[i];
  for (Int_t i=0; i<3; i++) fDiamondCovXY[i]=esd.fDiamondCovXY[i];

  for(Int_t i = 0; i < kNTriggerClasses; i++) {
    TNamed *str = (TNamed *)((esd.fTriggerClasses).At(i));
    if (str) fTriggerClasses.AddAt(new TNamed(*str),i);
  }

  for(Int_t m=0; m<kNPHOSMatrix; m++){
    if(esd.fPHOSMatrix[m])
      fPHOSMatrix[m]=new TGeoHMatrix(*(esd.fPHOSMatrix[m])) ;
    else
      fPHOSMatrix[m]=NULL;
  }
	
  for(Int_t sm=0; sm<kNEMCALMatrix; sm++){
	if(esd.fEMCALMatrix[sm])
		fEMCALMatrix[sm]=new TGeoHMatrix(*(esd.fEMCALMatrix[sm])) ;
	else
		fEMCALMatrix[sm]=NULL;
  }
}

//______________________________________________________________________________
AliESDRun& AliESDRun::operator=(const AliESDRun &esd)
{ 
  // assigment operator
  if(this!=&esd) {
    TObject::operator=(esd);
    fRunNumber=esd.fRunNumber;
    fPeriodNumber=esd.fPeriodNumber;
    fRecoVersion=esd.fRecoVersion;
    fMagneticField=esd.fMagneticField;
    fBeamType = esd.fBeamType;
    fCurrentL3  = esd.fCurrentL3;
    fCurrentDip = esd.fCurrentDip;
    fBeamEnergy = esd.fBeamEnergy;
    for (Int_t i=0; i<2; i++) fDiamondXY[i]=esd.fDiamondXY[i];
    for (Int_t i=0; i<3; i++) fDiamondCovXY[i]=esd.fDiamondCovXY[i];
    fTriggerClasses.Clear();
    for(Int_t i = 0; i < kNTriggerClasses; i++) {
      TNamed *str = (TNamed *)((esd.fTriggerClasses).At(i));
      if (str) fTriggerClasses.AddAt(new TNamed(*str),i);
    }

    fDetInDAQ   = esd.fDetInDAQ;
    fDetInReco  = esd.fDetInReco;

    for(Int_t m=0; m<kNPHOSMatrix; m++){
      if(esd.fPHOSMatrix[m])
	fPHOSMatrix[m]=new TGeoHMatrix(*(esd.fPHOSMatrix[m])) ;
      else
	fPHOSMatrix[m]=0;
    }
	  
	for(Int_t sm=0; sm<kNEMCALMatrix; sm++){
	  if(esd.fEMCALMatrix[sm])
		  fEMCALMatrix[sm]=new TGeoHMatrix(*(esd.fEMCALMatrix[sm])) ;
	  else
		  fEMCALMatrix[sm]=0;
	}
  } 
  return *this;
}

void AliESDRun::Copy(TObject &obj) const{

  // this overwrites the virtual TOBject::Copy()
  // to allow run time copying without casting
  // in AliESDEvent

  if(this==&obj)return;
  AliESDRun *robj = dynamic_cast<AliESDRun*>(&obj);
  if(!robj)return; // not an aliesdrun
  *robj = *this;

}

//______________________________________________________________________________
AliESDRun::~AliESDRun() {
  // Destructor
  // Delete PHOS position matrices
  for(Int_t m=0; m<kNPHOSMatrix; m++) {
    if(fPHOSMatrix[m]) delete fPHOSMatrix[m] ;
    fPHOSMatrix[m] = NULL;
  }
  // Delete PHOS position matrices
  for(Int_t sm=0; sm<kNEMCALMatrix; sm++) {
	if(fEMCALMatrix[sm]) delete fEMCALMatrix[sm] ;
	fEMCALMatrix[sm] = NULL;
  }
}

void AliESDRun::SetDiamond(const AliESDVertex *vertex) {
  // set the interaction diamond
  fDiamondXY[0]=vertex->GetXv();
  fDiamondXY[1]=vertex->GetYv();
  Double32_t cov[6];
  vertex->GetCovMatrix(cov);
  fDiamondCovXY[0]=cov[0];
  fDiamondCovXY[1]=cov[1];
  fDiamondCovXY[2]=cov[2];
}


//______________________________________________________________________________
void AliESDRun::Print(const Option_t *) const
{
  // Print some data members
  printf("Mean vertex in RUN %d: X=%.4f Y=%.4f cm\n",
	 GetRunNumber(),GetDiamondX(),GetDiamondY());
  printf("Beam Type: %s, Energy: %.1f GeV\n",fBeamType.IsNull() ? "N/A":fBeamType.Data(),fBeamEnergy);
  printf("Magnetic field in IP= %f T | Currents: L3:%+.1f Dipole:%+.1f %s\n",
	 GetMagneticField(),fCurrentL3,fCurrentDip,TestBit(kUniformBMap) ? "(Uniform)":"");
  printf("Event from reconstruction version %d \n",fRecoVersion);
  
  printf("List of active trigger classes: ");
  for(Int_t i = 0; i < kNTriggerClasses; i++) {
    TNamed *str = (TNamed *)((fTriggerClasses).At(i));
    if (str) printf("%s ",str->GetName());
  }
  printf("\n");
}

void AliESDRun::Reset() 
{
  // reset data members
  fRunNumber = 0;
  fPeriodNumber = 0;
  fRecoVersion = 0;
  fMagneticField = 0;
  fCurrentL3 = 0;
  fCurrentDip = 0;
  fBeamEnergy = 0;
  fBeamType = "";
  ResetBit(kBInfoStored|kUniformBMap|kConvSqrtSHalfGeV);
  for (Int_t i=0; i<2; i++) fDiamondXY[i]=0.;
  fDiamondCovXY[0]=fDiamondCovXY[2]=3.*3.;
  fDiamondCovXY[1]=0.;
  fTriggerClasses.Clear();
  fDetInDAQ   = 0;
  fDetInReco  = 0;
}

//______________________________________________________________________________
void AliESDRun::SetTriggerClass(const char*name, Int_t index)
{
  // Fill the trigger class name
  // into the corresponding array
  if (index >= kNTriggerClasses || index < 0) {
    AliError(Form("Index (%d) is outside the allowed range (0,49)!",index));
    return;
  }

  fTriggerClasses.AddAt(new TNamed(name,NULL),index);
}

//______________________________________________________________________________
const char* AliESDRun::GetTriggerClass(Int_t index) const
{
  // Get the trigger class name at
  // specified position in the trigger mask
  TNamed *trclass = (TNamed *)fTriggerClasses.At(index);
  if (trclass)
    return trclass->GetName();
  else
    return "";
}

//______________________________________________________________________________
TString AliESDRun::GetActiveTriggerClasses() const
{
  // Construct and return
  // the list of trigger classes
  // which are present in the run
  TString trclasses;
  for(Int_t i = 0; i < kNTriggerClasses; i++) {
    TNamed *str = (TNamed *)((fTriggerClasses).At(i));
    if (str) {
      trclasses += " ";
      trclasses += str->GetName();
      trclasses += " ";
    }
  }

  return trclasses;
}

//______________________________________________________________________________
TString AliESDRun::GetFiredTriggerClasses(ULong64_t mask) const
{
  // Constructs and returns the
  // list of trigger classes that
  // have been fired. Uses the trigger
  // class mask as an argument.
  TString trclasses;
  for(Int_t i = 0; i < kNTriggerClasses; i++) {
    if (mask & (1ull << i)) {
      TNamed *str = (TNamed *)((fTriggerClasses).At(i));
      if (str) {
	trclasses += " ";
	trclasses += str->GetName();
      trclasses += " ";
      }
    }
  }

  return trclasses;
}

//______________________________________________________________________________
Bool_t AliESDRun::IsTriggerClassFired(ULong64_t mask, const char *name) const
{
  // Checks if the trigger class
  // identified by 'name' has been
  // fired. Uses the trigger class mask.

  TNamed *trclass = (TNamed *)fTriggerClasses.FindObject(name);
  if (!trclass) return kFALSE;

  Int_t iclass = fTriggerClasses.IndexOf(trclass);
  if (iclass < 0) return kFALSE;

  if (mask & (1ull << iclass))
    return kTRUE;
  else
    return kFALSE;
}

//_____________________________________________________________________________
Bool_t AliESDRun::InitMagneticField() const
{
  // Create mag field from stored information
  //
  if (!TestBit(kBInfoStored)) {
    AliError("No information on currents, cannot create field from run header");
    return kFALSE;
  }
  //
  if ( TGeoGlobalMagField::Instance()->IsLocked() ) {
    if (TGeoGlobalMagField::Instance()->GetField()->TestBit(AliMagF::kOverrideGRP)) {
      AliInfo("ExpertMode!!! Information on magnet currents will be ignored !");
      AliInfo("ExpertMode!!! Running with the externally locked B field !");
      return kTRUE;
    }
    else {
      AliInfo("Destroying existing B field instance!");
      delete TGeoGlobalMagField::Instance();
    }
  }
  //
  AliMagF* fld = AliMagF::CreateFieldMap(fCurrentL3,fCurrentDip,AliMagF::kConvLHC,
					 TestBit(kUniformBMap), GetBeamEnergy(), GetBeamType());
  if (fld) {
    TGeoGlobalMagField::Instance()->SetField( fld );
    TGeoGlobalMagField::Instance()->Lock();
    AliInfo("Running with the B field constructed out of the Run Header !");
    return kTRUE;
  }
  else {
    AliError("Failed to create a B field map !");
    return kFALSE;
  }
  //
}

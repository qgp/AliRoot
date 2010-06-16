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


#include <TClass.h>
#include <TFile.h>
#include <TSystem.h>
#include <TPRegexp.h>

#include "AliMagF.h"
#include "AliMagWrapCheb.h"
#include "AliLog.h"

ClassImp(AliMagF)

const Double_t AliMagF::fgkSol2DipZ    =  -700.;  
const UShort_t AliMagF::fgkPolarityConvention = AliMagF::kConvLHC;
/*
 Explanation for polarity conventions: these are the mapping between the
 current signs and main field components in L3 (Bz) and Dipole (Bx) (in Alice frame)
 1) kConvMap2005: used for the field mapping in 2005
 positive L3  current -> negative Bz
 positive Dip current -> positive Bx 
 2) kConvMapDCS2008: defined by the microswitches/cabling of power converters as of 2008 - 1st half 2009
 positive L3  current -> positive Bz
 positive Dip current -> positive Bx
 3) kConvLHC : defined by LHC
 positive L3  current -> positive Bz
 positive Dip current -> negative Bx
 
 Note: only "negative Bz(L3) with postive Bx(Dipole)" and its inverse was mapped in 2005. Hence 
 the GRP Manager will reject the runs with the current combinations (in the convention defined by the
 static Int_t AliMagF::GetPolarityConvention()) which do not lead to such field polarities.

 ----------------------------------------------- 

 Explanation on integrals in the TPC region
 GetTPCInt(xyz,b) and GetTPCRatInt(xyz,b) give integrals from point (x,y,z) to point (x,y,0) 
 (irrespectively of the z sign) of the following:
 TPCInt:    b contains int{bx}, int{by}, int{bz}
 TPCRatInt: b contains int{bx/bz}, int{by/bz}, int{(bx/bz)^2+(by/bz)^2}
  
 The same applies to integral in cylindrical coordinates:
 GetTPCIntCyl(rphiz,b)
 GetTPCIntRatCyl(rphiz,b)
 They accept the R,Phi,Z coordinate (-pi<phi<pi) and return the field 
 integrals in cyl. coordinates.

 Thus, to compute the integral from arbitrary xy_z1 to xy_z2, one should take
 b = b1-b2 with b1 and b2 coming from GetTPCInt(xy_z1,b1) and GetTPCInt(xy_z2,b2)

 Note: the integrals are defined for the range -300<Z<300 and 0<R<300
*/
//_______________________________________________________________________
AliMagF::AliMagF():
  TVirtualMagField(),
  fMeasuredMap(0),
  fMapType(k5kG),
  fSolenoid(0),
  fBeamType(kNoBeamField),
  fBeamEnergy(0),
  //
  fInteg(0),
  fPrecInteg(0),
  fFactorSol(1.),
  fFactorDip(1.),
  fMax(15),
  fDipoleOFF(kFALSE),
  //
  fQuadGradient(0),
  fDipoleField(0),
  fCCorrField(0), 
  fACorr1Field(0),
  fACorr2Field(0),
  fParNames("","")
{
  // Default constructor
  //
}

//_______________________________________________________________________
AliMagF::AliMagF(const char *name, const char* title, Double_t factorSol, Double_t factorDip, 
		 BMap_t maptype, BeamType_t bt, Double_t be,Int_t integ, Double_t fmax, const char* path):
  TVirtualMagField(name),
  fMeasuredMap(0),
  fMapType(maptype),
  fSolenoid(0),
  fBeamType(bt),
  fBeamEnergy(be),
  //
  fInteg(integ),
  fPrecInteg(1),
  fFactorSol(1.),
  fFactorDip(1.),
  fMax(fmax),
  fDipoleOFF(factorDip==0.),
  //
  fQuadGradient(0),
  fDipoleField(0),
  fCCorrField(0), 
  fACorr1Field(0),
  fACorr2Field(0),
  fParNames("","")
{
  // Initialize the field with Geant integration option "integ" and max field "fmax,
  // Impose scaling of parameterized L3 field by factorSol and of dipole by factorDip.
  // The "be" is the energy of the beam in GeV/nucleon
  //
  SetTitle(title);
  if(integ<0 || integ > 2) {
    AliWarning(Form("Invalid magnetic field flag: %5d; Helix tracking chosen instead",integ));
    fInteg = 2;
  }
  if (fInteg == 0) fPrecInteg = 0;
  //
  if (fBeamEnergy<=0 && fBeamType!=kNoBeamField) {
    if      (fBeamType == kBeamTypepp) fBeamEnergy = 7000.; // max proton energy
    else if (fBeamType == kBeamTypeAA) fBeamEnergy = 2750;  // max PbPb energy
    AliInfo("Maximim possible beam energy for requested beam is assumed");
  } 
  const char* parname = 0;
  //  
  if      (fMapType == k2kG) parname = fDipoleOFF ? "Sol12_Dip0_Hole":"Sol12_Dip6_Hole";
  else if (fMapType == k5kG) parname = fDipoleOFF ? "Sol30_Dip0_Hole":"Sol30_Dip6_Hole";
  else if (fMapType == k5kGUniform) parname = "Sol30_Dip6_Uniform";
  else AliFatal(Form("Unknown field identifier %d is requested\n",fMapType));
  //
  SetDataFileName(path);
  SetParamName(parname);
  //
  LoadParameterization();
  InitMachineField(fBeamType,fBeamEnergy);
  double xyz[3]={0.,0.,0.};
  fSolenoid = GetBz(xyz);
  SetFactorSol(factorSol);
  SetFactorDip(factorDip);
  Print("a");
}

//_______________________________________________________________________
AliMagF::AliMagF(const AliMagF &src):
  TVirtualMagField(src),
  fMeasuredMap(0),
  fMapType(src.fMapType),
  fSolenoid(src.fSolenoid),
  fBeamType(src.fBeamType),
  fBeamEnergy(src.fBeamEnergy),
  fInteg(src.fInteg),
  fPrecInteg(src.fPrecInteg),
  fFactorSol(src.fFactorSol),
  fFactorDip(src.fFactorDip),
  fMax(src.fMax),
  fDipoleOFF(src.fDipoleOFF),
  fQuadGradient(src.fQuadGradient),
  fDipoleField(src.fDipoleField),
  fCCorrField(src.fCCorrField), 
  fACorr1Field(src.fACorr1Field),
  fACorr2Field(src.fACorr2Field),
  fParNames(src.fParNames)
{
  if (src.fMeasuredMap) fMeasuredMap = new AliMagWrapCheb(*src.fMeasuredMap);
}

//_______________________________________________________________________
AliMagF::~AliMagF()
{
  delete fMeasuredMap;
}

//_______________________________________________________________________
Bool_t AliMagF::LoadParameterization()
{
  if (fMeasuredMap) {
    AliFatal(Form("Field data %s are already loaded from %s\n",GetParamName(),GetDataFileName()));
  }
  //
  char* fname = gSystem->ExpandPathName(GetDataFileName());
  TFile* file = TFile::Open(fname);
  if (!file) {
    AliFatal(Form("Failed to open magnetic field data file %s\n",fname)); 
  }
  //
  fMeasuredMap = dynamic_cast<AliMagWrapCheb*>(file->Get(GetParamName()));
  if (!fMeasuredMap) {
    AliFatal(Form("Did not find field %s in %s\n",GetParamName(),fname)); 
  }
  file->Close();
  delete file;
  return kTRUE;
}


//_______________________________________________________________________
void AliMagF::Field(const Double_t *xyz, Double_t *b)
{
  // Method to calculate the field at point  xyz
  //
  //  b[0]=b[1]=b[2]=0.0;
  if (fMeasuredMap && xyz[2]>fMeasuredMap->GetMinZ() && xyz[2]<fMeasuredMap->GetMaxZ()) {
    fMeasuredMap->Field(xyz,b);
    if (xyz[2]>fgkSol2DipZ || fDipoleOFF) for (int i=3;i--;) b[i] *= fFactorSol;
    else                                  for (int i=3;i--;) b[i] *= fFactorDip;    
  }
  else MachineField(xyz, b);
  //
}

//_______________________________________________________________________
Double_t AliMagF::GetBz(const Double_t *xyz) const
{
  // Method to calculate the field at point  xyz
  //
  if (fMeasuredMap && xyz[2]>fMeasuredMap->GetMinZ() && xyz[2]<fMeasuredMap->GetMaxZ()) {
    double bz = fMeasuredMap->GetBz(xyz);
    return (xyz[2]>fgkSol2DipZ || fDipoleOFF) ? bz*fFactorSol : bz*fFactorDip;    
  }
  else return 0.;
}

//_______________________________________________________________________
AliMagF& AliMagF::operator=(const AliMagF& src)
{
  if (this != &src && src.fMeasuredMap) { 
    if (fMeasuredMap) delete fMeasuredMap;
    fMeasuredMap = new AliMagWrapCheb(*src.fMeasuredMap);
    SetName(src.GetName());
    fSolenoid    = src.fSolenoid;
    fBeamType    = src.fBeamType;
    fBeamEnergy  = src.fBeamEnergy;
    fInteg       = src.fInteg;
    fPrecInteg   = src.fPrecInteg;
    fFactorSol   = src.fFactorSol;
    fFactorDip   = src.fFactorDip;
    fMax         = src.fMax;
    fDipoleOFF   = src.fDipoleOFF;
    fParNames    = src.fParNames;
  }
  return *this;
}

//_______________________________________________________________________
void AliMagF::InitMachineField(BeamType_t btype, Double_t benergy)
{
  if (btype==kNoBeamField) {
    fQuadGradient = fDipoleField = fCCorrField = fACorr1Field = fACorr2Field = 0.;
    return;
  }
  //
  double rigScale = benergy/7000.;   // scale according to ratio of E/Enominal
  // for ions assume PbPb (with energy provided per nucleon) and account for A/Z
  if (btype == kBeamTypeAA) rigScale *= 208./82.;
  //
  fQuadGradient = 22.0002*rigScale;
  fDipoleField  = 37.8781*rigScale;
  //
  // SIDE C
  fCCorrField   = -9.6980;
  // SIDE A
  fACorr1Field  = -13.2247;
  fACorr2Field  =  11.7905;
  //
}

//_______________________________________________________________________
void AliMagF::MachineField(const Double_t *x, Double_t *b) const
{
  // ---- This is the ZDC part
  // Compansators for Alice Muon Arm Dipole
  const Double_t kBComp1CZ = 1075., kBComp1hDZ = 260./2., kBComp1SqR = 4.0*4.0; 
  const Double_t kBComp2CZ = 2049., kBComp2hDZ = 153./2., kBComp2SqR = 4.5*4.5; 
  //  
  const Double_t kTripQ1CZ = 2615., kTripQ1hDZ = 637./2., kTripQ1SqR = 3.5*3.5;
  const Double_t kTripQ2CZ = 3480., kTripQ2hDZ = 550./2., kTripQ2SqR = 3.5*3.5;
  const Double_t kTripQ3CZ = 4130., kTripQ3hDZ = 550./2., kTripQ3SqR = 3.5*3.5;
  const Double_t kTripQ4CZ = 5015., kTripQ4hDZ = 637./2., kTripQ4SqR = 3.5*3.5;
  //
  const Double_t kDip1CZ = 6310.8,  kDip1hDZ = 945./2., kDip1SqRC = 4.5*4.5, kDip1SqRA = 3.375*3.375;
  const Double_t kDip2CZ = 12640.3, kDip2hDZ = 945./2., kDip2SqRC = 4.5*4.5, kDip2SqRA = 3.75*3.75;
  const Double_t kDip2DXC = 9.7, kDip2DXA = 9.4;
  //
  double rad2 = x[0] * x[0] + x[1] * x[1];
  //
  b[0] = b[1] = b[2] = 0;
  //
  // SIDE C **************************************************
  if(x[2]<0.){  
    if(TMath::Abs(x[2]+kBComp2CZ)<kBComp2hDZ && rad2 < kBComp2SqR){
      b[0] = fCCorrField*fFactorDip;
    } 
    else if(TMath::Abs(x[2]+kTripQ1CZ)<kTripQ1hDZ && rad2 < kTripQ1SqR){
      b[0] = fQuadGradient*x[1];
      b[1] = fQuadGradient*x[0];
    }
    else if(TMath::Abs(x[2]+kTripQ2CZ)<kTripQ2hDZ && rad2 < kTripQ2SqR){
      b[0] = -fQuadGradient*x[1];
      b[1] = -fQuadGradient*x[0];
    }
    else if(TMath::Abs(x[2]+kTripQ3CZ)<kTripQ3hDZ && rad2 < kTripQ3SqR){
      b[0] = -fQuadGradient*x[1];
      b[1] = -fQuadGradient*x[0];
    }
    else if(TMath::Abs(x[2]+kTripQ4CZ)<kTripQ4hDZ && rad2 < kTripQ4SqR){
      b[0] = fQuadGradient*x[1];
      b[1] = fQuadGradient*x[0];
    }
    else if(TMath::Abs(x[2]+kDip1CZ)<kDip1hDZ && rad2 < kDip1SqRC){
      b[1] = fDipoleField;
    }
    else if(TMath::Abs(x[2]+kDip2CZ)<kDip2hDZ && rad2 < kDip2SqRC) {
      double dxabs = TMath::Abs(x[0])-kDip2DXC;
      if ( (dxabs*dxabs + x[1]*x[1])<kDip2SqRC) {
	b[1] = -fDipoleField;
      }
    }
  }
  //
  // SIDE A **************************************************
  else{        
    if(TMath::Abs(x[2]-kBComp1CZ)<kBComp1hDZ && rad2 < kBComp1SqR) {
      // Compensator magnet at z = 1075 m 
      b[0] = fACorr1Field*fFactorDip;
    }
    //
    if(TMath::Abs(x[2]-kBComp2CZ)<kBComp2hDZ && rad2 < kBComp2SqR){
      b[0] = fACorr2Field*fFactorDip;
    }
    else if(TMath::Abs(x[2]-kTripQ1CZ)<kTripQ1hDZ && rad2 < kTripQ1SqR){
      b[0] = -fQuadGradient*x[1];
      b[1] = -fQuadGradient*x[0];
    }
    else if(TMath::Abs(x[2]-kTripQ2CZ)<kTripQ2hDZ && rad2 < kTripQ2SqR){
      b[0] =  fQuadGradient*x[1];
      b[1] =  fQuadGradient*x[0];
    }
    else if(TMath::Abs(x[2]-kTripQ3CZ)<kTripQ3hDZ && rad2 < kTripQ3SqR){
      b[0] =  fQuadGradient*x[1];
      b[1] =  fQuadGradient*x[0];
    }
    else if(TMath::Abs(x[2]-kTripQ4CZ)<kTripQ4hDZ && rad2 < kTripQ4SqR){
      b[0] = -fQuadGradient*x[1];
      b[1] = -fQuadGradient*x[0];
    }
    else if(TMath::Abs(x[2]-kDip1CZ)<kDip1hDZ && rad2 < kDip1SqRA){
      b[1] = -fDipoleField;
    }
    else if(TMath::Abs(x[2]-kDip2CZ)<kDip2hDZ && rad2 < kDip2SqRA) {
      double dxabs = TMath::Abs(x[0])-kDip2DXA;
      if ( (dxabs*dxabs + x[1]*x[1])<kDip2SqRA) {
	b[1] = fDipoleField;
      }
    }
  }
  //
}

//_______________________________________________________________________
void AliMagF::GetTPCInt(const Double_t *xyz, Double_t *b) const
{
  // Method to calculate the integral_0^z of br,bt,bz 
  b[0]=b[1]=b[2]=0.0;
  if (fMeasuredMap) {
    fMeasuredMap->GetTPCInt(xyz,b);
    for (int i=3;i--;) b[i] *= fFactorSol;
  }
}

//_______________________________________________________________________
void AliMagF::GetTPCRatInt(const Double_t *xyz, Double_t *b) const
{
  // Method to calculate the integral_0^z of bx/bz,by/bz and (bx/bz)^2+(by/bz)^2
  b[0]=b[1]=b[2]=0.0;
  if (fMeasuredMap) {
    fMeasuredMap->GetTPCRatInt(xyz,b);
    b[2] /= 100;
  }
}

//_______________________________________________________________________
void AliMagF::GetTPCIntCyl(const Double_t *rphiz, Double_t *b) const
{
  // Method to calculate the integral_0^z of br,bt,bz 
  // in cylindrical coordiates ( -pi<phi<pi convention )
  b[0]=b[1]=b[2]=0.0;
  if (fMeasuredMap) {
    fMeasuredMap->GetTPCIntCyl(rphiz,b);
    for (int i=3;i--;) b[i] *= fFactorSol;
  }
}

//_______________________________________________________________________
void AliMagF::GetTPCRatIntCyl(const Double_t *rphiz, Double_t *b) const
{
  // Method to calculate the integral_0^z of bx/bz,by/bz and (bx/bz)^2+(by/bz)^2
  // in cylindrical coordiates ( -pi<phi<pi convention )
  b[0]=b[1]=b[2]=0.0;
  if (fMeasuredMap) {
    fMeasuredMap->GetTPCRatIntCyl(rphiz,b);
    b[2] /= 100;
  }
}

//_______________________________________________________________________
void AliMagF::SetFactorSol(Float_t fc)
{
  // set the sign/scale of the current in the L3 according to fgkPolarityConvention
  switch (fgkPolarityConvention) {
  case kConvDCS2008: fFactorSol = -fc; break;
  case kConvLHC    : fFactorSol = -fc; break;
  default          : fFactorSol =  fc; break;  // case kConvMap2005: fFactorSol =  fc; break;
  }
}

//_______________________________________________________________________
void AliMagF::SetFactorDip(Float_t fc)
{
  // set the sign*scale of the current in the Dipole according to fgkPolarityConvention
  switch (fgkPolarityConvention) {
  case kConvDCS2008: fFactorDip =  fc; break;
  case kConvLHC    : fFactorDip = -fc; break;
  default          : fFactorDip =  fc; break;  // case kConvMap2005: fFactorDip =  fc; break;
  }
}

//_______________________________________________________________________
Double_t AliMagF::GetFactorSol() const
{
  // return the sign*scale of the current in the Dipole according to fgkPolarityConventionthe 
  switch (fgkPolarityConvention) {
  case kConvDCS2008: return -fFactorSol;
  case kConvLHC    : return -fFactorSol;
  default          : return  fFactorSol;       //  case kConvMap2005: return  fFactorSol;
  }
}

//_______________________________________________________________________
Double_t AliMagF::GetFactorDip() const
{
  // return the sign*scale of the current in the Dipole according to fgkPolarityConventionthe 
  switch (fgkPolarityConvention) {
  case kConvDCS2008: return  fFactorDip;
  case kConvLHC    : return -fFactorDip;
  default          : return  fFactorDip;       //  case kConvMap2005: return  fFactorDip;
  }
}

//_____________________________________________________________________________
AliMagF* AliMagF::CreateFieldMap(Float_t l3Cur, Float_t diCur, Int_t convention, Bool_t uniform,
				 Float_t beamenergy, const Char_t *beamtype, const Char_t *path) 
{
  //------------------------------------------------
  // The magnetic field map, defined externally...
  // L3 current 30000 A  -> 0.5 T
  // L3 current 12000 A  -> 0.2 T
  // dipole current 6000 A
  // The polarities must match the convention (LHC or DCS2008) 
  // unless the special uniform map was used for MC
  //------------------------------------------------
  const Float_t l3NominalCurrent1=30000.; // (A)
  const Float_t l3NominalCurrent2=12000.; // (A)
  const Float_t diNominalCurrent =6000. ; // (A)

  const Float_t tolerance=0.03; // relative current tolerance
  const Float_t zero=77.;       // "zero" current (A)
  //
  BMap_t map;
  double sclL3,sclDip;
  //
  Float_t l3Pol = l3Cur > 0 ? 1:-1;
  Float_t diPol = diCur > 0 ? 1:-1;
 
  l3Cur = TMath::Abs(l3Cur);
  diCur = TMath::Abs(diCur);
  //
  if (TMath::Abs((sclDip=diCur/diNominalCurrent)-1.) > tolerance && !uniform) {
    if (diCur <= zero) sclDip = 0.; // some small current.. -> Dipole OFF
    else {
      AliFatalGeneral("AliMagF",Form("Wrong dipole current (%f A)!",diCur));
    }
  }
  //
  if (uniform) { 
    // special treatment of special MC with uniform mag field (normalized to 0.5 T)
    // no check for scaling/polarities are done
    map   = k5kGUniform;
    sclL3 = l3Cur/l3NominalCurrent1; 
  }
  else {
    if      (TMath::Abs((sclL3=l3Cur/l3NominalCurrent1)-1.) < tolerance) map  = k5kG;
    else if (TMath::Abs((sclL3=l3Cur/l3NominalCurrent2)-1.) < tolerance) map  = k2kG;
    else if (l3Cur <= zero && diCur<=zero)   { sclL3=0; sclDip=0; map  = k5kGUniform;}
    else {
      AliFatalGeneral("AliMagF",Form("Wrong L3 current (%f A)!",l3Cur));
    }
  }
  //
  if (sclDip!=0 && map!=k5kGUniform) {
    if ( (l3Cur<=zero) || ((convention==kConvLHC && l3Pol!=diPol) || (convention==kConvDCS2008 && l3Pol==diPol)) ) { 
      AliFatalGeneral("AliMagF",Form("Wrong combination for L3/Dipole polarities (%c/%c) for convention %d",
				     l3Pol>0?'+':'-',diPol>0?'+':'-',GetPolarityConvention()));
    }
  }
  //
  if (l3Pol<0) sclL3  = -sclL3;
  if (diPol<0) sclDip = -sclDip;
  //
  BeamType_t btype = kNoBeamField;
  TString btypestr = beamtype;
  btypestr.ToLower();
  TPRegexp protonBeam("(proton|p)\\s*-?\\s*\\1");
  TPRegexp ionBeam("(lead|pb|ion|a)\\s*-?\\s*\\1");
  if (btypestr.Contains(ionBeam)) btype = kBeamTypeAA;
  else if (btypestr.Contains(protonBeam)) btype = kBeamTypepp;
  else AliInfoGeneral("AliMagF",Form("Assume no LHC magnet field for the beam type %s, ",beamtype));
  char ttl[80];
  sprintf(ttl,"L3: %+5d Dip: %+4d kA; %s | Polarities in %s convention",(int)TMath::Sign(l3Cur,float(sclL3)),
	  (int)TMath::Sign(diCur,float(sclDip)),uniform ? " Constant":"",
	  convention==kConvLHC ? "LHC":"DCS2008");
  // LHC and DCS08 conventions have opposite dipole polarities
  if ( GetPolarityConvention() != convention) sclDip = -sclDip;
  //
  return new AliMagF("MagneticFieldMap", ttl,sclL3,sclDip,map,btype,beamenergy,2,10.,path);
  //
}

//_____________________________________________________________________________
const char*  AliMagF::GetBeamTypeText() const
{
  const char *beamNA  = "No Beam";
  const char *beamPP  = "p-p";
  const char *beamPbPb= "Pb-Pb";
  switch ( fBeamType ) {
  case kBeamTypepp : return beamPP;
  case kBeamTypeAA : return beamPbPb;
  case kNoBeamField: 
  default:           return beamNA;
  }
}

//_____________________________________________________________________________
void AliMagF::Print(Option_t *opt) const
{
  // print short or long info
  TString opts = opt; opts.ToLower();
  AliInfo(Form("%s:%s",GetName(),GetTitle()));
  AliInfo(Form("Solenoid (%+.2f*)%.0f kG, Dipole %s (%+.2f) %s",
	       GetFactorSol(),(fMapType==k5kG||fMapType==k5kGUniform)?5.:2.,
	       fDipoleOFF ? "OFF":"ON",GetFactorDip(),fMapType==k5kGUniform?" |Constant Field!":""));
  if (opts.Contains("a")) {
    AliInfo(Form("Machine B fields for %s beam (%.0f GeV): QGrad: %.4f Dipole: %.4f",
		 fBeamType==kBeamTypeAA ? "A-A":(fBeamType==kBeamTypepp ? "p-p":"OFF"),
		 fBeamEnergy,fQuadGradient,fDipoleField));
    AliInfo(Form("Uses %s of %s",GetParamName(),GetDataFileName()));
  }
}

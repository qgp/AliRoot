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

/* $Id: AliITSMFTSegmentationPix.cxx 47180 2011-02-08 09:42:29Z masera $ */
#include <TGeoManager.h>
#include <TGeoVolume.h>
#include <TGeoBBox.h>
#include <TObjArray.h>
#include <TString.h>
#include <TSystem.h>
#include <TFile.h>
#include "AliITSMFTAux.h"
#include "AliLog.h"
#include "AliITSMFTSegmentationPix.h"

using namespace TMath;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Segmentation class for pixels                                                                          //
// Questions to solve: are guardrings needed and do they belong to the sensor or to the chip in TGeo    //
//                     At the moment assume that the local coord syst. is located at bottom left corner   //
//                     of the ACTIVE matrix. If the guardring to be accounted in the local coords, in     //
//                     the Z and X conversions one needs to first subtract the  fGuardLft and fGuardBot   //
//                     from the local Z,X coordinates                                                     //
//                                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////////////////////

ClassImp(AliITSMFTSegmentationPix)

const char* AliITSMFTSegmentationPix::fgkSegmListName = "ITSUSegmentations";

//_____________________________________________________________________________RS
AliITSMFTSegmentationPix::AliITSMFTSegmentationPix(UInt_t id, int nchips,int ncol,int nrow,
						   float pitchX,float pitchZ,
						   float thickness,
						   float pitchLftC,float pitchRgtC,
						   float edgL,float edgR,float edgT,float edgB)
: TObject()
  ,fGuardLft(edgL)
  ,fGuardRgt(edgR)
  ,fGuardTop(edgT)
  ,fGuardBot(edgB)
  ,fShiftXLoc(0.5*(edgT-edgB))
  ,fShiftZLoc(0.5*(edgR-edgL))
  ,fDxActive(0)
  ,fDzActive(0)
  ,fPitchX(pitchX)
  ,fPitchZ(pitchZ)
  ,fPitchZLftCol(pitchLftC<0 ? pitchZ:pitchLftC)
  ,fPitchZRgtCol(pitchRgtC<0 ? pitchZ:pitchRgtC)
  ,fChipDZ(0)
  ,fNChips(nchips)
  ,fNColPerChip(nchips>0 ? ncol/nchips:0)
  ,fNRow(nrow)
  ,fNCol(ncol)
  ,fDiodShiftMatNCol(0)
  ,fDiodShiftMatNRow(0)
  ,fDiodShiftMatDim(0)
  ,fDiodShidtMatX(0)
  ,fDiodShidtMatZ(0)
{
  // Default constructor, sizes in cm
  if (nchips) SetUniqueID(ComposeChipTypeID(id) );
  fChipDZ = (fNColPerChip-2)*fPitchZ + fPitchZLftCol + fPitchZRgtCol;;
  fDxActive = fNRow*fPitchX;
  fDzActive = fNChips*fChipDZ;
  SetDetSize( fDxActive + fGuardTop+fGuardBot,
	      fDzActive + fGuardLft+fGuardRgt,
	      thickness);
  //
}

//_____________________________________________________________________________RS
AliITSMFTSegmentationPix::~AliITSMFTSegmentationPix()
{
  // d-tor
  delete[] fDiodShidtMatX;
  delete[] fDiodShidtMatZ;
}


//_____________________________________________________________________________RS
void AliITSMFTSegmentationPix::GetPadIxz(Float_t x,Float_t z,Int_t &ix,Int_t &iz) const 
{
  //  Returns pixel coordinates (ix,iz) for given coordinates (x,z counted from corner of col/row 0:0)
  //  expects x, z in cm.
  ix = int(x/fPitchX);     
  iz = int(Z2Col(z));
  //  
  if      (iz<0)        { AliWarning(Form("Z=%f gives col=%d ouside [%d:%d)",z,iz,0,fNCol)); iz=0; }
  else if (iz >= fNCol) { AliWarning(Form("Z=%f gives col=%d ouside [%d:%d)",z,iz,0,fNCol)); iz= fNCol-1;}
  if      (ix<0)        { AliWarning(Form("X=%f gives row=%d ouside [%d:%d)",x,ix,0,fNRow)); ix=0; }
  else if (ix >= fNRow) { AliWarning(Form("X=%f gives row=%d ouside [%d:%d)",x,ix,0,fNRow)); ix= fNRow-1;}
  //
}

//_____________________________________________________________________________RS
void AliITSMFTSegmentationPix::GetPadTxz(Float_t &x,Float_t &z) const
{
  //  local transformation of real local coordinates (x,z)
  //  expects x, z in cm (wrt corner of col/row 0:0
  x /= fPitchX;
  z = Z2Col(z);
  //
}

//_____________________________________________________________________________RS
void AliITSMFTSegmentationPix::GetPadCxz(Int_t ix,Int_t iz,Float_t &x,Float_t&z) const
{
  // Transform from pixel to real local coordinates
  // returns x, z in cm. wrt corner of col/row 0:0
  x = Float_t((ix+0.5)*fPitchX);
  z = Col2Z(iz);
  //
}

//_____________________________________________________________________________RS
Float_t AliITSMFTSegmentationPix::Z2Col(Float_t z) const 
{
  // get column number (from 0) from local Z (wrt bottom left corner of the active matrix)
  int chip = int(z/fChipDZ);
  float col = chip*fNColPerChip;
  z -= chip*fChipDZ;
  if (z>fPitchZLftCol) col += 1+(z-fPitchZLftCol)/fPitchZ;
  return col;
}

//_____________________________________________________________________________RS
Float_t AliITSMFTSegmentationPix::Col2Z(Int_t col) const 
{
  // convert column number (from 0) to Z coordinate wrt bottom left corner of the active matrix
  int nchip = col/fNColPerChip;
  col %= fNColPerChip;
  float z = nchip*fChipDZ;
  if (col>0) {
    if (col<fNColPerChip-1) z += fPitchZLftCol + (col-0.5)*fPitchZ;
    else                    z += fChipDZ - fPitchZRgtCol/2;
  }
  else z += fPitchZLftCol/2;
  return z;
  //
}

//______________________________________________________________________RS
AliITSMFTSegmentationPix& AliITSMFTSegmentationPix::operator=(const AliITSMFTSegmentationPix &src)
{
  // = operator
  if(this==&src) return *this;
  TObject::operator=(src);
  fNCol  = src.fNCol;
  fNRow  = src.fNRow;
  fNColPerChip  = src.fNColPerChip;
  fNChips = src.fNChips;
  fChipDZ = src.fChipDZ;
  fPitchZRgtCol = src.fPitchZRgtCol;
  fPitchZLftCol = src.fPitchZLftCol;
  fPitchZ = src.fPitchZ;
  fPitchX = src.fPitchX;
  fShiftXLoc = src.fShiftXLoc;
  fShiftZLoc = src.fShiftZLoc;
  fDxActive = src.fDxActive;
  fDzActive = src.fDzActive;
  //
  fGuardBot = src.fGuardBot;
  fGuardTop = src.fGuardTop;
  fGuardRgt = src.fGuardRgt;
  fGuardLft = src.fGuardLft;
  //
  fDiodShiftMatNCol = src.fDiodShiftMatNCol;
  fDiodShiftMatNRow = src.fDiodShiftMatNRow;
  fDiodShiftMatDim  = src.fDiodShiftMatDim;
  delete fDiodShidtMatX; fDiodShidtMatX = 0;
  delete fDiodShidtMatZ; fDiodShidtMatZ = 0;
  if (fDiodShiftMatDim) {
    fDiodShidtMatX = new Float_t[fDiodShiftMatDim];
    fDiodShidtMatZ = new Float_t[fDiodShiftMatDim];
    for (int i=fDiodShiftMatDim;i--;) {
      fDiodShidtMatX[i] = src.fDiodShidtMatX[i];
      fDiodShidtMatZ[i] = src.fDiodShidtMatZ[i];
    }
  }
  //
  return *this;
}

//____________________________________________________________________________RS
AliITSMFTSegmentationPix::AliITSMFTSegmentationPix(const AliITSMFTSegmentationPix &src) :
  TObject(src)
  ,fGuardLft(src.fGuardLft)
  ,fGuardRgt(src.fGuardRgt)
  ,fGuardTop(src.fGuardTop)
  ,fGuardBot(src.fGuardBot)
  ,fShiftXLoc(src.fShiftXLoc)
  ,fShiftZLoc(src.fShiftZLoc)
  ,fDxActive(src.fDxActive)
  ,fDzActive(src.fDzActive)
  ,fPitchX(src.fPitchX)
  ,fPitchZ(src.fPitchZ)
  ,fPitchZLftCol(src.fPitchZLftCol)
  ,fPitchZRgtCol(src.fPitchZRgtCol)
  ,fChipDZ(src.fChipDZ)
  ,fNChips(src.fNChips)
  ,fNColPerChip(src.fNColPerChip)
  ,fNRow(src.fNRow)
  ,fNCol(src.fNCol)  
  ,fDiodShiftMatNCol(src.fDiodShiftMatNCol)
  ,fDiodShiftMatNRow(src.fDiodShiftMatNRow)
  ,fDiodShiftMatDim(src.fDiodShiftMatDim)
  ,fDiodShidtMatX(0)
  ,fDiodShidtMatZ(0)
{
  // copy constructor
  if (fDiodShiftMatDim) {
    fDiodShidtMatX = new Float_t[fDiodShiftMatDim];
    fDiodShidtMatZ = new Float_t[fDiodShiftMatDim];
    for (int i=fDiodShiftMatDim;i--;) {
      fDiodShidtMatX[i] = src.fDiodShidtMatX[i];
      fDiodShidtMatZ[i] = src.fDiodShidtMatZ[i];
    }
  }
}

//____________________________________________________________________________RS
Float_t AliITSMFTSegmentationPix::Dpx(Int_t ) const 
{
  //returs x pixel pitch for a give pixel
  return fPitchX;
}

//____________________________________________________________________________RS
Float_t AliITSMFTSegmentationPix::Dpz(Int_t col) const 
{
  // returns z pixel pitch for a given pixel (cols starts from 0)
  col %= fNColPerChip;
  if (!col) return fPitchZLftCol;
  if (col==fNColPerChip-1) return fPitchZRgtCol;
  return fPitchZ;
  //
}

//------------------------------
void AliITSMFTSegmentationPix::Neighbours(Int_t iX, Int_t iZ, Int_t* nlist, Int_t xlist[8], Int_t zlist[8]) const 
{
  // returns the neighbouring pixels for use in Cluster Finders and the like.
  //
  *nlist=8;
  xlist[0]=xlist[1]=iX;
  xlist[2]=iX-1;
  xlist[3]=iX+1;
  zlist[0]=iZ-1;
  zlist[1]=iZ+1;
  zlist[2]=zlist[3]=iZ;
  // Diagonal elements
  xlist[4]=iX+1;
  zlist[4]=iZ+1;
  //  
  xlist[5]=iX-1;
  zlist[5]=iZ-1;
  //
  xlist[6]=iX-1;
  zlist[6]=iZ+1;
  //
  xlist[7]=iX+1;
  zlist[7]=iZ-1;
  //
}

//______________________________________________________________________
Bool_t AliITSMFTSegmentationPix::LocalToDet(Float_t x,Float_t z,Int_t &ix,Int_t &iz) const 
{
  // Transformation from Geant detector centered local coordinates (cm) to
  // Pixel cell numbers ix and iz.
  // Input:
  //    Float_t   x        detector local coordinate x in cm with respect to
  //                       the center of the sensitive volume.
  //    Float_t   z        detector local coordinate z in cm with respect to
  //                       the center of the sensitive volulme.
  // Output:
  //    Int_t    ix        detector x cell coordinate. Has the range 
  //                       0<=ix<fNRow.
  //    Int_t    iz        detector z cell coordinate. Has the range 
  //                       0<=iz<fNCol.
  // Return:
  //   kTRUE if point x,z is inside sensitive volume, kFALSE otherwise.
  //   A value of -1 for ix or iz indecates that this point is outside of the
  //   detector segmentation as defined.
  x += 0.5*DxActive() + fShiftXLoc; // get X,Z wrt bottom/left corner
  z += 0.5*DzActive() + fShiftZLoc;
  ix = iz = -1;
  if(x<0 || x>DxActive()) return kFALSE; // outside x range.
  if(z<0 || z>DzActive()) return kFALSE; // outside z range.
  ix = int(x/fPitchX);
  iz = Z2Col(z);
  return kTRUE; // Found ix and iz, return.
}

//______________________________________________________________________
void AliITSMFTSegmentationPix::DetToLocal(Int_t ix,Int_t iz,Float_t &x,Float_t &z) const
{
// Transformation from Detector cell coordiantes to Geant detector centered 
// local coordinates (cm).
// Input:
// Int_t    ix        detector x cell coordinate. Has the range 0<=ix<fNRow.
// Int_t    iz        detector z cell coordinate. Has the range 0<=iz<fNCol.
// Output:
// Float_t   x        detector local coordinate x in cm with respect to the
//                    center of the sensitive volume.
// Float_t   z        detector local coordinate z in cm with respect to the
//                    center of the sensitive volulme.
// If ix and or iz is outside of the segmentation range a value of -0.5*Dx()
// or -0.5*Dz() is returned.
  //
  x = -0.5*DxActive(); // default value.
  z = -0.5*DzActive(); // default value.
  if(ix<0 || ix>=fNRow) {AliWarning(Form("Obtained row %d is not in range [%d:%d)",ix,0,fNRow)); return;} // outside of detector 
  if(iz<0 || iz>=fNCol) {AliWarning(Form("Obtained col %d is not in range [%d:%d)",ix,0,fNCol)); return;} // outside of detector 
  x += (ix+0.5)*fPitchX - fShiftXLoc;       // RS: we go to the center of the pad, i.e. + pitch/2, not to the boundary as in SPD
  z += Col2Z(iz)        - fShiftZLoc; 
  return; // Found x and z, return.
}

//______________________________________________________________________
void AliITSMFTSegmentationPix::CellBoundries(Int_t ix,Int_t iz,Double_t &xl,Double_t &xu,Double_t &zl,Double_t &zu) const
{
  // Transformation from Detector cell coordiantes to Geant detector centerd 
  // local coordinates (cm).
  // Input:
  // Int_t    ix        detector x cell coordinate. Has the range 0<=ix<fNRow.
  // Int_t    iz        detector z cell coordinate. Has the range 0<=iz<fNCol.
  // Output:
  // Double_t   xl       detector local coordinate cell lower bounds x in cm
  //                    with respect to the center of the sensitive volume.
  // Double_t   xu       detector local coordinate cell upper bounds x in cm 
  //                    with respect to the center of the sensitive volume.
  // Double_t   zl       detector local coordinate lower bounds z in cm with
  //                    respect to the center of the sensitive volulme.
  // Double_t   zu       detector local coordinate upper bounds z in cm with 
  //                    respect to the center of the sensitive volulme.
  // If ix and or iz is outside of the segmentation range a value of -0.5*DxActive()
  // and -0.5*DxActive() or -0.5*DzActive() and -0.5*DzActive() are returned.
  Float_t x,z;
  DetToLocal(ix,iz,x,z);
  //
  if( ix<0 || ix>=fNRow || iz<0 || iz>=fNCol) {
    xl = xu = -0.5*Dx(); // default value.
    zl = zu = -0.5*Dz(); // default value.
    return; // outside of detctor
  }
  float zpitchH = Dpz(iz)*0.5;
  float xpitchH = fPitchX*0.5;
  xl -= xpitchH;
  xu += xpitchH;
  zl -= zpitchH;
  zu += zpitchH;
  return; // Found x and z, return.
}

//______________________________________________________________________
Int_t AliITSMFTSegmentationPix::GetChipFromChannel(Int_t, Int_t iz) const 
{
  // returns chip number (in range 0-4) starting from channel number
  if(iz>=fNCol  || iz<0 ){
    AliWarning("Bad cell number");
    return -1;
  }
  return iz/fNColPerChip;
}

//______________________________________________________________________
Int_t AliITSMFTSegmentationPix::GetChipFromLocal(Float_t, Float_t zloc) const 
{
  // returns chip number (in range 0-4) starting from local Geant coordinates
  Int_t ix0,iz;
  if (!LocalToDet(0,zloc,ix0,iz)) {
    AliWarning("Bad local coordinate");
    return -1;
  } 
  return GetChipFromChannel(ix0,iz);
}

//______________________________________________________________________
Int_t AliITSMFTSegmentationPix::GetChipsInLocalWindow(Int_t* array, Float_t zmin, Float_t zmax, Float_t, Float_t) const 
{
  // returns the number of chips containing a road defined by given local Geant coordinate limits
  //
  if (zmin>zmax) {
    AliWarning("Bad coordinate limits: zmin>zmax!");
    return -1;
  } 
  //
  Int_t nChipInW = 0;
  //
  Float_t zminDet = -0.5*DzActive()-fShiftZLoc;
  Float_t zmaxDet =  0.5*DzActive()-fShiftZLoc;
  if(zmin<zminDet) zmin=zminDet;
  if(zmax>zmaxDet) zmax=zmaxDet;

  Int_t n1 = GetChipFromLocal(0,zmin);
  array[nChipInW] = n1;
  nChipInW++;

  Int_t n2 = GetChipFromLocal(0,zmax);

  if(n2!=n1){
    Int_t imin=Min(n1,n2);
    Int_t imax=Max(n1,n2);
    for(Int_t ichip=imin; ichip<=imax; ichip++){
      if(ichip==n1) continue;
      array[nChipInW]=ichip;
      nChipInW++;
    }
  }
  //
  return nChipInW;
}

//______________________________________________________________________
void AliITSMFTSegmentationPix::Init()
{
  // init settings
}

//______________________________________________________________________
Bool_t AliITSMFTSegmentationPix::Store(const char* outf)
{
  // store in the special list under given ID
  TString fns = outf;
  gSystem->ExpandPathName(fns);
  if (fns.IsNull()) {AliFatal("No file name provided"); return kFALSE;}
  TFile* fout = TFile::Open(fns.Data(),"update");
  if (!fout) {AliFatal(Form("Failed to open output file %s",outf)); return kFALSE;}
  TObjArray* arr = (TObjArray*)fout->Get(fgkSegmListName);
  int id = GetUniqueID();
  if (!arr) arr = new TObjArray();
  else if (arr->At(id)) {AliFatal(Form("Segmenation %d already exists in file %s",id,outf));return kFALSE;}
  //
  arr->AddAtAndExpand(this,id);
  arr->SetOwner(kTRUE);
  fout->WriteObject(arr,fgkSegmListName,"kSingleKey");
  fout->Close();
  delete fout;
  arr->RemoveAt(id);
  delete arr;
  AliInfo(Form("Stored segmentation %d in %s",id,outf));
  return kTRUE;
  //
}

//______________________________________________________________________
AliITSMFTSegmentationPix* AliITSMFTSegmentationPix::LoadWithID(UInt_t id, const char* inpf)
{
  // store in the special list under given ID
  TString fns = inpf;
  gSystem->ExpandPathName(fns);
  if (fns.IsNull()) {AliFatalGeneral("LoadWithID","No file name provided"); return 0;}
  TFile* finp = TFile::Open(fns.Data());
  if (!finp) {AliFatalGeneral("LoadWithID",Form("Failed to open file %s",inpf)); return 0;}
  TObjArray* arr = (TObjArray*)finp->Get(fgkSegmListName);
  if (!arr) {
    AliFatalGeneral("LoadWithID",Form("Failed to find segmenation array %s in %s",fgkSegmListName,inpf)); 
    return 0;
  }
  AliITSMFTSegmentationPix* segm = dynamic_cast<AliITSMFTSegmentationPix*>(arr->At(id));
  if (!segm || segm->GetUniqueID()!=id) {AliFatalGeneral("LoadWithID",Form("Failed to find segmenation %d in %s",id,inpf)); return 0;}
  //
  arr->RemoveAt(id);
  arr->SetOwner(kTRUE); // to not leave in memory other segmenations
  finp->Close();
  delete finp;
  delete arr;
  //
  return segm;
}

//______________________________________________________________________
void AliITSMFTSegmentationPix::LoadSegmentations(TObjArray* dest, const char* inpf)
{
  // store in the special list under given ID
  if (!dest) return;
  TString fns = inpf;
  gSystem->ExpandPathName(fns);
  if (fns.IsNull()) AliFatalGeneral("LoadWithID","No file name provided");
  TFile* finp = TFile::Open(fns.Data());
  if (!finp) AliFatalGeneral("LoadWithID",Form("Failed to open file %s",inpf));
  TObjArray* arr = (TObjArray*)finp->Get(fgkSegmListName);
  if (!arr) AliFatalGeneral("LoadWithID",Form("Failed to find segmenation array %s in %s",fgkSegmListName,inpf)); 
  int nent = arr->GetEntriesFast();
  TObject *segm = 0;
  for (int i=nent;i--;) if ((segm=arr->At(i))) dest->AddAtAndExpand(segm,segm->GetUniqueID());
  AliInfoGeneral("LoadSegmentations",Form("Loaded %d segmantions from %s",arr->GetEntries(),inpf));
  arr->SetOwner(kFALSE);
  arr->Clear();
  finp->Close();
  delete finp;
  delete arr;
  //
}

//______________________________________________________________________
void AliITSMFTSegmentationPix::SetDiodShiftMatrix(Int_t nrow,Int_t ncol, const Float_t *shiftX, const Float_t *shiftZ)
{
  // set matrix of periodic shifts of diod center. provided arrays must be in the format shift[nrow][ncol]
  if (fDiodShiftMatDim) {
    delete fDiodShidtMatX;
    delete fDiodShidtMatZ;
    fDiodShidtMatX = fDiodShidtMatZ = 0;
  }
  //
  fDiodShiftMatNCol = ncol;
  fDiodShiftMatNRow = nrow;
  fDiodShiftMatDim = fDiodShiftMatNCol*fDiodShiftMatNRow;
  if (fDiodShiftMatDim) {
    fDiodShidtMatX = new Float_t[fDiodShiftMatDim];
    fDiodShidtMatZ = new Float_t[fDiodShiftMatDim];    
    for (int ir=0;ir<fDiodShiftMatNRow;ir++) {
      for (int ic=0;ic<fDiodShiftMatNCol;ic++) {
	int cnt = ic+ir*fDiodShiftMatNCol;
	fDiodShidtMatX[cnt] = shiftX ? shiftX[cnt] : 0.;
	fDiodShidtMatZ[cnt] = shiftZ ? shiftZ[cnt] : 0.;
      }
    }
  }
  
}
//______________________________________________________________________
void AliITSMFTSegmentationPix::SetDiodShiftMatrix(Int_t nrow,Int_t ncol, const Double_t *shiftX, const Double_t *shiftZ)
{
  // set matrix of periodic shifts of diod center. provided arrays must be in the format shift[nrow][ncol]
  if (fDiodShiftMatDim) {
    delete fDiodShidtMatX;
    delete fDiodShidtMatZ;
    fDiodShidtMatX = fDiodShidtMatZ = 0;
  }
  //
  fDiodShiftMatNCol = ncol;
  fDiodShiftMatNRow = nrow;
  fDiodShiftMatDim = fDiodShiftMatNCol*fDiodShiftMatNRow;
  if (fDiodShiftMatDim) {
    fDiodShidtMatX = new Float_t[fDiodShiftMatDim];
    fDiodShidtMatZ = new Float_t[fDiodShiftMatDim];    
    for (int ir=0;ir<fDiodShiftMatNRow;ir++) {
      for (int ic=0;ic<fDiodShiftMatNCol;ic++) {
	int cnt = ic+ir*fDiodShiftMatNCol;
	fDiodShidtMatX[cnt] = shiftX ? shiftX[cnt] : 0.;
	fDiodShidtMatZ[cnt] = shiftZ ? shiftZ[cnt] : 0.;
      }
    }
  }
}

//______________________________________________________________________
void AliITSMFTSegmentationPix::Print(Option_t* /*option*/) const
{
  // print itself
  const double kmc=1e4;
  printf("Segmentation %d: Active Size: DX: %.1f DY: %.1f DZ: %.1f | Pitch: X:%.1f Z:%.1f\n",
	 GetUniqueID(),kmc*DxActive(),kmc*Dy(),kmc*DzActive(),kmc*Dpx(1),kmc*Dpz(1));
  printf("Passive Edges: Bottom: %.1f Right: %.1f Top: %.1f Left: %.1f -> DX: %.1f DZ: %.1f Shift: x:%.1f z:%.1f\n",
	 kmc*fGuardBot,kmc*fGuardRgt,kmc*fGuardTop,kmc*fGuardLft,kmc*Dx(),kmc*Dz(),kmc*fShiftXLoc,kmc*fShiftZLoc);
  printf("%d chips along Z: chip Ncol=%d Nrow=%d\n",fNChips, fNColPerChip,fNRow);
  if (Abs(fPitchZLftCol-fPitchZ)>1e-5) printf("Special left  column pitch: %.1f\n",fPitchZLftCol*kmc);
  if (Abs(fPitchZRgtCol-fPitchZ)>1e-5) printf("Special right column pitch: %.1f\n",fPitchZRgtCol*kmc);
  //
  if (fDiodShiftMatDim) {
    double dx,dz=0;
    printf("Diod shift (fraction of pitch) periodicity pattern (X,Z[row][col])\n");
    for (int irow=0;irow<fDiodShiftMatNRow;irow++) {
      for (int icol=0;icol<fDiodShiftMatNCol;icol++) {	
	GetDiodShift(irow,icol,dx,dz);
	printf("%.1f/%.1f |",dx,dz);
      }
      printf("\n");
    }
  }
}

//______________________________________________________________________
void AliITSMFTSegmentationPix::GetDiodShift(Int_t row,Int_t col, Float_t &dx,Float_t &dz) const
{
  // obtain optional diod shift
  if (!fDiodShiftMatDim) {dx=dz=0; return;}
  int cnt = (col%fDiodShiftMatNCol) + (row%fDiodShiftMatNRow)*fDiodShiftMatNCol;
  dx = fDiodShidtMatX[cnt];
  dz = fDiodShidtMatZ[cnt];  
  //
}
//______________________________________________________________________
 UInt_t AliITSMFTSegmentationPix::ComposeChipTypeID(UInt_t segmId)
 {
 if (segmId>=AliITSMFTAux::kMaxSegmPerChipType) AliFatalClass(Form("Id=%d is >= max.allowed %d",segmId,AliITSMFTAux::kMaxSegmPerChipType));
 return segmId + AliITSMFTAux::kChipTypePix*AliITSMFTAux::kMaxSegmPerChipType;
 }

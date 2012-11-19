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
#include <TGeoManager.h>
#include <TGeoVolume.h>
#include <TGeoBBox.h>
#include "AliITSsegmentationSPD.h"

//////////////////////////////////////////////////////
// Segmentation class for                           //
// pixels                                           //
//                                                  //
//////////////////////////////////////////////////////
const Int_t AliITSsegmentationSPD::fgkNchipsPerModule = 5;
const Int_t AliITSsegmentationSPD::fgkNcolumnsPerChip = 32;
ClassImp(AliITSsegmentationSPD)

//_____________________________________________________________________________
  AliITSsegmentationSPD::AliITSsegmentationSPD(Option_t *opt): AliITSsegmentation(),
fNpx(0),
fNpz(0){
  // Default constructor

  // Initialization to default values
  Init();

  // Initialization of detector dimensions from TGeo
  if(strstr(opt,"TGeo")){
    if(!gGeoManager){
      AliError("Geometry is not initialized\n Using hardwired default values");
      return;
    }
    TGeoVolume *v=NULL;
    v = gGeoManager->GetVolume("ITSSPDlay1-sensor");
    if(!v){
      AliWarning("TGeo volume ITSSPDlay1-sensor not found (hint: use v11Hybrid geometry)\n Using hardwired default values");
    }
    else {
      TGeoBBox *s=(TGeoBBox*)v->GetShape();
      SetDetSize(s->GetDX()*20000.,s->GetDZ()*20000.,s->GetDY()*20000.);
    }
  }
}

//_____________________________________________________________________________
Float_t AliITSsegmentationSPD::ColFromZ300(Float_t z) const {
// Get column number for each z-coordinate taking into account the 
// extra pixels in z direction assuming 300 micron sized pixels.
     Float_t col = 0.0;
     Float_t pitchz = 300.0;
     col = Float_t (z/pitchz);
     return col;
}
//_____________________________________________________________________________
Float_t AliITSsegmentationSPD::ZFromCol300(Int_t col) const {
// same comments as above
// Get z-coordinate for each colunm number
  Float_t pitchz = 300.0;
  Float_t z = 0.0;
  z = (col+0.5)*pitchz;
  return z;
}
//_____________________________________________________________________________
Float_t AliITSsegmentationSPD::Zpitch300() const {
  // returns Z pixel pitch for 300 micron pixels.

    return 300.0;
}
//_____________________________________________________________________________
Float_t AliITSsegmentationSPD::ColFromZ(Float_t z) const {
    // hard-wired - keep it like this till we can parametrise 
    // and get rid of AliITSgeomSPD425
    // Get column number for each z-coordinate taking into account the 
    // extra pixels in z direction 
    Int_t i;
    Float_t s,col;

    if(z<0||z>fDz){
	AliError(Form("z=%f outside of range 0.0<=z<fDz=%f",z,fDz));
	return 0.0; // error
    } // end if outsize of detector
    s = 0.0;
    i = -1;
    do {
      i++;
      s += fCellSizeZ[i];
    } while(z>s);
    s -= fCellSizeZ[i];
    col = (Float_t) i + (z-s)/fCellSizeZ[i];
    return col;

/*
  Float_t col = 0;
  Float_t pitchz = 425;
  if( z < 13175) {
    col = Float_t(z/pitchz);
  } else if( z < 14425) {  
    pitchz = 625;
    col = 31 + (z - 13175)/pitchz;
  } else if( z < 27175) {  
    col = 33 + (z - 14425)/pitchz;
  } else if( z < 28425) {  
    pitchz = 625;
    col = 63 + (z - 27175)/pitchz;
  } else if( z < 41175) {  
    col = 65 + (z - 28425)/pitchz;
  } else if( z < 42425) {  
    pitchz = 625;
    col = 95 + (z - 41175)/pitchz;
  } else if( z < 55175) {  
    col = 97 + (z - 42425)/pitchz;
  } else if( z < 56425) {  
    pitchz = 625;
    col = 127 + (z - 55175)/pitchz;
  } else if( z < 69175) {  
    col = 129 + (z - 56425)/pitchz;
  } else if( z < 70425) {  
    pitchz = 625;
    col = 159 + (z - 69175)/pitchz;
  } else if( z < 83600) {  
    col = 161 + (z - 70425)/pitchz;
  }   
  return TMath::Abs(col);*/
}

//_____________________________________________________________________________
Float_t AliITSsegmentationSPD::ZFromCol(Int_t col) const {
    // same comments as above
    // Get z-coordinate for each colunm number
    Int_t i;
    Float_t z;

    if(col<0||col>=fNpz){
	AliError(Form("col=%d outside of range 0<=col<fNpZ=%d",col,fNpz));
	return 0.0; // error
    } // end if outsize of detector
    z = 0.0;
    for(i=0;i<col;i++) z += fCellSizeZ[i];
    z += 0.5*fCellSizeZ[col];
    return z;

/* 
  Float_t pitchz = 425;
  Float_t z = 0;
  if( col >=0 && col <= 30 ) {  
    z = (col + 0.5)*pitchz;    
  } else if( col >= 31 && col <= 32) {  
    pitchz = 625;
    z = 13175 + (col -31 + 0.5)*pitchz;    
  } else if( col >= 33 && col <= 62) {  
    z = 14425 + (col -33 + 0.5)*pitchz;    
  } else if( col >= 63 && col <= 64) {  
    pitchz = 625;
    z = 27175 + (col -63 + 0.5)*pitchz;    
  } else if( col >= 65 && col <= 94) {  
    z = 28425 + (col -65 + 0.5)*pitchz;    
  } else if( col >= 95 && col <= 96) {  
    pitchz = 625;
    z = 41175 + (col -95 + 0.5)*pitchz;    
  } else if( col >= 97 && col <= 126) {  
    z = 42425 + (col -97 + 0.5)*pitchz;    
  } else if( col >= 127 && col <= 128) {  
    pitchz = 625;
    z = 55175 + (col -127 + 0.5)*pitchz;    
  } else if( col >= 129 && col <= 158) {  
    z = 56425 + (col -129 + 0.5)*pitchz;    
  } else if( col >= 159 && col <= 160) {  
    pitchz = 625;
    z = 69175 + (col -159 + 0.5)*pitchz;    
  } else if( col >= 161 && col <= 191) {  
    z = 70425 + (col -161 + 0.5)*pitchz;    
  }

  return z;*/
}
//______________________________________________________________________
Float_t AliITSsegmentationSPD::ZpitchFromCol(Int_t col) const {
    // Get pitch size in z direction for each colunm

   Float_t pitchz = 425.;
    if(col < 0){
	pitchz = 0.0;
    } else if(col >=  31 && col <=  32) {
	pitchz = 625.;
    } else if(col >=  63 && col <=  64) {
	pitchz = 625.;
    } else if(col >=  95 && col <=  96) {
	pitchz = 625.;
    } else if(col >= 127 && col <= 128) {
	pitchz = 625.;
    } else if(col >= 159 && col <= 160) {
	pitchz = 625.;
    } else if(col>=192){
	pitchz = 425.;
    }
    return pitchz;
}

//______________________________________________________________________
void AliITSsegmentationSPD::Copy(TObject &obj) const {
  // protected method. copy this to obj
  AliITSsegmentation::Copy(obj);
  ((AliITSsegmentationSPD& ) obj).fNpx  = fNpx;
  ((AliITSsegmentationSPD& ) obj).fNpz  = fNpz;
  Int_t i;
  for(i=0;i<256;i++) 
         ((AliITSsegmentationSPD& ) obj).fCellSizeX[i] = fCellSizeX[i];
  for(i=0;i<280;i++) 
         ((AliITSsegmentationSPD& ) obj).fCellSizeZ[i] = fCellSizeZ[i];
}

//______________________________________________________________________
AliITSsegmentationSPD& AliITSsegmentationSPD::operator=(const AliITSsegmentationSPD &source){
   // = operator
   if(this==&source) return *this;
   source.Copy(*this);
   return *this;
}
//____________________________________________________________________________
AliITSsegmentationSPD::AliITSsegmentationSPD(const AliITSsegmentationSPD &source) :
    AliITSsegmentation(source),
fNpx(0),
fNpz(0){
  for(Int_t i=0; i<256; i++)fCellSizeX[i]=0.;
  for(Int_t i=0; i<280; i++)fCellSizeZ[i]=0.;
  // copy constructor
  source.Copy(*this);
}
//----------------------------------------------------------------------
void AliITSsegmentationSPD::SetBinSize(Float_t *x,Float_t *z){
    // Fills the array of pixel sizes in x, microns
    // The input array x must have 256 elements.
    Int_t i;

    for(i=0;i<256;i++) fCellSizeX[i] = x[i];
    for(i=0;i<280;i++) fCellSizeZ[i] = z[i];
    return;
}
//----------------------------------------------------------------------
void AliITSsegmentationSPD::Init300(){
// Initialize infromation for 6 read out chip 300X50 micron pixel SPD 
// detectors. This chip is 150 microns thick by 1.28 cm in x by 8.37 cm
// long. It has 256  50 micron pixels in x and 279 300 micron size
// pixels in z.

    //const Float_t kconv=10000.;
    Int_t i;
    fNpx = 256; // The number of X pixel Cell same as in fCellSizeX array size
    fNpz = 279; // The number of Z pixel Cell same as in fCellSizeZ array size
    for(i=0;i<fNpx;i++) fCellSizeX[i] = 50.0; // microns all the same
    for(i=0;i<280;i++) fCellSizeZ[i] = Zpitch300(); // microns
//    for(i=fNpz;i<280;i++) fCellSizeZ[i] = 0.0; // zero out rest of array
    fDx = 0;
    for(i=0;i<fNpx;i++) fDx += fCellSizeX[i];
    fDz = 0;
    for(i=0;i<fNpz;i++) fDz += fCellSizeZ[i];
    fDy = 300.0; //microns  SPD sensitive layer thickness
}

//------------------------------
void AliITSsegmentationSPD::Init(){
// Initialize information for 5 read out chip 425X50 micron pixel SPD 
// detectors (ladder).
// Each readout chip is 150 micron thick.
// The ladder sensor is 200 micron thick by 1.28 cm in x by 6.96 cm in z.
// It has 256 50 micron pixels in x and 160 mostly 425 micron pixels in z.
// The two pixels at boundary between two adjacent readout chips are 
// 625 micron long.

  Float_t bx[256],bz[280];
  Int_t i;
  SetNPads(256,160);  // Number of Bins in x and z
  for(i=000;i<256;i++) bx[i] =  50.0; // in x all are 50 microns.
  for(i=000;i<160;i++) bz[i] = 425.0; // most are 425 microns except below
  for(i=160;i<280;i++) bz[i] =   0.0; // Outside of detector.
  bz[ 31] = bz[ 32] = 625.0; // first chip boundary
  bz[ 63] = bz[ 64] = 625.0; // first chip boundary
  bz[ 95] = bz[ 96] = 625.0; // first chip boundary
  bz[127] = bz[128] = 625.0; // first chip boundary
  bz[160] = 425.0; // Set so that there is no zero pixel size for fNz.
  SetBinSize(bx,bz); 
  SetDetSize(12800,69600,200);  // full lengths (x,z,y) in microns 

}
//------------------------------
void AliITSsegmentationSPD::SetNPads(Int_t p1, Int_t p2){
  // for SPD this function should be used ONLY when a beam test setup 
  // configuration is studied

    fNpx=p1;
    fNpz=p2;

}

//------------------------------
Float_t AliITSsegmentationSPD::Dpx(Int_t i) const {
    //returs x pixel pitch for a give pixel
    if(i<0||i>=256) return 0.0;
    return fCellSizeX[i];
}
//------------------------------
Float_t AliITSsegmentationSPD::Dpz(Int_t i) const {
    // returns z pixel pitch for a give pixel
    if(i<0||i>=280) return 0.0;
    return fCellSizeZ[i];
}
//------------------------------
void AliITSsegmentationSPD::GetPadIxz(Float_t x,Float_t z,Int_t &ix,Int_t &iz) const {
//  Returns pixel coordinates (ix,iz) for given real local coordinates (x,z)
//

    // expects x, z in microns

    // same segmentation on x
    Float_t dpx=Dpx(0);
    ix = (Int_t)(x/dpx + 1);
    // different segmentation on z
    iz = (Int_t)(ColFromZ(z) + 1);


    if (iz >  fNpz) iz= fNpz;
    if (ix >  fNpx) ix= fNpx;
    /*
    if (iz < -fNpz) iz= -fNpz;
    if (ix < -fNpx) ix=-fNpx;
    */
}

//------------------------------
void AliITSsegmentationSPD::GetPadTxz(Float_t &x,Float_t &z) const{
//  local transformation of real local coordinates (x,z)
//

    // expects x, z in microns

    // same segmentation on x
    Float_t dpx=Dpx(0);

    x /= dpx;
    z = ColFromZ(z);

}
//------------------------------
void AliITSsegmentationSPD::GetPadCxz(Int_t ix,Int_t iz,Float_t &x,Float_t&z) const {
    // Transform from pixel to real local coordinates

    // returns x, z in microns

    Float_t dpx=Dpx(0);

    x = (ix>0) ? Float_t(ix*dpx)-dpx/2. : Float_t(ix*dpx)+dpx/2.;
    z = ZFromCol(iz);


}
//------------------------------
void AliITSsegmentationSPD::
Neighbours(Int_t iX, Int_t iZ, Int_t* Nlist, Int_t Xlist[8], Int_t Zlist[8]) const {
  // returns the neighbouring pixels for use in Cluster Finders and the like.
  /*
    *Nlist=4;Xlist[0]=Xlist[1]=iX;Xlist[2]=iX-1;Xlist[3]=iX+1;
    Zlist[0]=iZ-1;Zlist[1]=iZ+1;Zlist[2]=Zlist[3]=iZ;
  */


    *Nlist=8;
    Xlist[0]=Xlist[1]=iX;
    Xlist[2]=iX-1;
    Xlist[3]=iX+1;
    Zlist[0]=iZ-1;
    Zlist[1]=iZ+1;
    Zlist[2]=Zlist[3]=iZ;

   // Diagonal elements
    Xlist[4]=iX+1;
    Zlist[4]=iZ+1;

    Xlist[5]=iX-1;
    Zlist[5]=iZ-1;

    Xlist[6]=iX-1;
    Zlist[6]=iZ+1;

    Xlist[7]=iX+1;
    Zlist[7]=iZ-1;
}
//______________________________________________________________________
Bool_t AliITSsegmentationSPD::LocalToDet(Float_t x,Float_t z,
                                         Int_t &ix,Int_t &iz) const {
    // Transformation from Geant detector centered local coordinates (cm) to
    // Pixel cell numbers ix and iz.
    // Input:
    //    Float_t   x        detector local coordinate x in cm with respect to
    //                       the center of the sensitive volume.
    //    Float_t   z        detector local coordinate z in cm with respect to
    //                       the center of the sensitive volulme.
    // Output:
    //    Int_t    ix        detector x cell coordinate. Has the range 
    //                       0<=ix<fNpx.
    //    Int_t    iz        detector z cell coordinate. Has the range 
    //                       0<=iz<fNpz.
    // Return:
    //   kTRUE if point x,z is inside sensitive volume, kFALSE otherwise.
    //   A value of -1 for ix or iz indecates that this point is outside of the
    //   detector segmentation as defined.
    Int_t i,j;
    Float_t dx,dz;
    const Float_t kconv = 1.0E-04; // converts microns to cm.

    dx = -0.5*kconv*Dx();
    dz = -0.5*kconv*Dz();
    ix = -1;
    iz = -1;
    if(x<dx) return kFALSE; // outside x range.
    if(z<dz) return kFALSE; // outside z range.
    for(i=0;i<Npx();i++){
	dx += kconv*fCellSizeX[i];
	if(x<dx) break;
    } // end for i
    if(i>=Npx()) return kFALSE; // outside x range.
    for(j=0;j<Npz();j++){
	dz += kconv*fCellSizeZ[j];
	if(z<dz) break;
    } // end for j
    if(j>=Npz()) return kFALSE; // outside z range.
    ix = i;
    iz = j;
    return kTRUE; // Found ix and iz, return.
}
//______________________________________________________________________
void AliITSsegmentationSPD::DetToLocal(Int_t ix,Int_t iz,Float_t &x,Float_t &z) const
{
// Transformation from Detector cell coordiantes to Geant detector centerd 
// local coordinates (cm).
// Input:
// Int_t    ix        detector x cell coordinate. Has the range 0<=ix<fNpx.
// Int_t    iz        detector z cell coordinate. Has the range 0<=iz<fNpz.
// Output:
// Float_t   x        detector local coordinate x in cm with respect to the
//                    center of the sensitive volume.
// Float_t   z        detector local coordinate z in cm with respect to the
//                    center of the sensitive volulme.
// If ix and or iz is outside of the segmentation range a value of -0.5*Dx()
// or -0.5*Dz() is returned.
    Int_t i,j;
    const Float_t kconv = 1.0E-04; // converts microns to cm.

    x = -0.5*kconv*Dx(); // default value.
    z = -0.5*kconv*Dz(); // default value.
    if(ix<0 || ix>=Npx()) return; // outside of detector
    if(iz<0 || iz>=Npz()) return; // outside of detctor
    for(i=0;i<ix;i++) x += kconv*fCellSizeX[i]; // sum up to cell ix-1
    x += 0.5*kconv*fCellSizeX[ix]; // add 1/2 of cell ix for center location.
    for(j=0;j<iz;j++) z += kconv*fCellSizeZ[j]; // sum up cell iz-1
    z += 0.5*kconv*fCellSizeZ[iz]; // add 1/2 of cell iz for center location.
    return; // Found x and z, return.
}
//______________________________________________________________________
void AliITSsegmentationSPD::CellBoundries(Int_t ix,Int_t iz,
					  Double_t &xl,Double_t &xu,
					  Double_t &zl,Double_t &zu) const
{
// Transformation from Detector cell coordiantes to Geant detector centerd 
// local coordinates (cm).
// Input:
// Int_t    ix        detector x cell coordinate. Has the range 0<=ix<fNpx.
// Int_t    iz        detector z cell coordinate. Has the range 0<=iz<fNpz.
// Output:
// Double_t   xl       detector local coordinate cell lower bounds x in cm
//                    with respect to the center of the sensitive volume.
// Double_t   xu       detector local coordinate cell upper bounds x in cm 
//                    with respect to the center of the sensitive volume.
// Double_t   zl       detector local coordinate lower bounds z in cm with
//                    respect to the center of the sensitive volulme.
// Double_t   zu       detector local coordinate upper bounds z in cm with 
//                    respect to the center of the sensitive volulme.
// If ix and or iz is outside of the segmentation range a value of -0.5*Dx()
// and -0.5*Dx() or -0.5*Dz() and -0.5*Dz() are returned.
    Int_t i,j;
    const Float_t kconv = 1.0E-04; // converts microns to cm.
    Float_t x,z;

    xl = xu = x = -0.5*kconv*Dx(); // default value.
    zl = zu = z = -0.5*kconv*Dz(); // default value.
    if(ix<0 || ix>=Npx()) return; // outside of detector
    if(iz<0 || iz>=Npz()) return; // outside of detctor
    for(i=0;i<ix;i++) x += kconv*fCellSizeX[i]; // sum up to cell ix-1
    xl = x;
    x += kconv*fCellSizeX[ix];
    xu = x;
    for(j=0;j<iz;j++) z += kconv*fCellSizeZ[j]; // sum up cell iz-1
    zl = z;
    z += kconv*fCellSizeZ[iz];
    zu = z;
    return; // Found x and z, return.
}
//----------------------------------------------------------------------
Int_t AliITSsegmentationSPD::GetChipFromChannel(Int_t, Int_t iz) const {
  // returns chip number (in range 0-4) starting from channel number
  if(iz>=fNpz  || iz<0 ){
    AliWarning("Bad cell number");
    return -1;
  }
  Int_t theChip=iz/fgkNcolumnsPerChip;
  return theChip;
}
//----------------------------------------------------------------------
Int_t AliITSsegmentationSPD::GetChipFromLocal(Float_t, Float_t zloc) const {
  // returns chip number (in range 0-4) starting from local coordinates
  Int_t ix0,iz;
  if (!LocalToDet(0,zloc,ix0,iz)) {
    AliWarning("Bad local coordinate");
    return -1;
  } 
  return GetChipFromChannel(ix0,iz);
}
//----------------------------------------------------------------------
Int_t AliITSsegmentationSPD::GetChipsInLocalWindow(Int_t* array, Float_t zmin, Float_t zmax, Float_t, Float_t) const {
  // returns the number of chips containing a road defined by given local coordinate limits

  const Float_t kconv = 1.0E-04; // converts microns to cm.

  if (zmin>zmax) {
    AliWarning("Bad coordinate limits: zmin>zmax!");
    return -1;
  } 

  Int_t nChipInW = 0;

  Float_t zminDet = -0.5*kconv*Dz();
  Float_t zmaxDet =  0.5*kconv*Dz();
  if(zmin<zminDet) zmin=zminDet;
  if(zmax>zmaxDet) zmax=zmaxDet;

  Int_t n1 = GetChipFromLocal(0,zmin);
  array[nChipInW] = n1;
  nChipInW++;

  Int_t n2 = GetChipFromLocal(0,zmax);

  if(n2!=n1){
    Int_t imin=TMath::Min(n1,n2);
    Int_t imax=TMath::Max(n1,n2);
    for(Int_t ichip=imin; ichip<=imax; ichip++){
      if(ichip==n1) continue;
      array[nChipInW]=ichip;
      nChipInW++;
    }
  }

  return nChipInW;
}
//----------------------------------------------------------------------

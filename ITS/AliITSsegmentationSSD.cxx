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
/*
$Log$
Revision 1.16  2002/10/22 14:45:43  alibrary
Introducing Riostream.h

Revision 1.15  2001/05/16 14:57:16  alibrary
New files for folders and Stack

Revision 1.14  2001/05/14 06:21:49  barbera
Some unuseful printout commented

Revision 1.13  2001/05/14 05:44:11  barbera
Version 1.11 reput in place to avoid problem with reconstruction

Revision 1.11  2001/05/03 16:12:37  nilsen
Fixed up LocalToDet, DetToLocal, GetCrossing, GetPadTxz to work with different
angles in layer 5 and 6.

Revision 1.10  2001/05/01 22:42:22  nilsen
Update of SSD simulation and reconstruction code by Boris and Enrico.

Revision 1.9  2001/04/27 14:16:50  nilsen
Remove dead and/or unused code and printout lines. i.e. cleaned it up a bit.

*/

#include <TMath.h>
#include <TF1.h>
#include <Riostream.h>
#include "AliITSsegmentationSSD.h"
#include "AliITSgeom.h"
#include "AliRun.h"
#include "AliModule.h"

ClassImp(AliITSsegmentationSSD)
AliITSsegmentationSSD::AliITSsegmentationSSD(){
    // default constructor
    fGeom  = 0;
    fCorr  = 0;
    fLayer = 0;
}
//----------------------------------------------------------------------
AliITSsegmentationSSD::AliITSsegmentationSSD(AliITSgeom *geom){
    // constuctor
    fGeom = geom;
    fCorr = 0;
    SetDetSize();
    SetPadSize();
    SetNPads();
    SetAngles();
    fLayer = 0;
}
//______________________________________________________________________
AliITSsegmentationSSD& AliITSsegmentationSSD::operator=(
                                             AliITSsegmentationSSD &source){
// Operator =
    if(this==&source) return *this;
    this->fNstrips = source.fNstrips;
    this->fStereoP = source.fStereoP;
    this->fStereoN = source.fStereoN;
    this->fStereoPl5 = source.fStereoPl5;
    this->fStereoNl5 = source.fStereoNl5;
    this->fStereoPl6 = source.fStereoPl6;
    this->fStereoNl6 = source.fStereoNl6;
    this->fLayer   = source.fLayer;
    this->fPitch   = source.fPitch;
    this->fDz      = source.fDz;
    this->fDx      = source.fDx;
    this->fDy      = source.fDy;
    this->fLayer   = source.fLayer;
    this->fGeom    = source.fGeom; // copy only the pointer
    this->fCorr    = new TF1(*(source.fCorr)); // make a proper copy
    return *this;     
}
//______________________________________________________________________
AliITSsegmentationSSD::AliITSsegmentationSSD(AliITSsegmentationSSD &source){
    // copy constructor
    *this = source;
}
//----------------------------------------------------------------------
void AliITSsegmentationSSD::Init(){
    // standard initalizer

    SetPadSize();
    SetNPads();
    SetAngles();
}
//----------------------------------------------------------------------
void AliITSsegmentationSSD::Angles(Float_t &aP,Float_t &aN){

    if (fLayer == 5){
	aP = fStereoPl5;
	aN = fStereoNl5;
    } // end if
    if (fLayer == 6){
	aP = fStereoPl6;
	aN = fStereoNl6;
    } // end if
}
//----------------------------------------------------------------------
void AliITSsegmentationSSD::SetLayer(Int_t l){

    if (l==5) fLayer =5;
    if (l==6) fLayer =6;
}
//----------------------------------------------------------------------
void AliITSsegmentationSSD::GetPadTxz(Float_t &x,Float_t &z){
    // returns P and N sided strip numbers for a given location.
    // Transformation from microns detector center local coordinates
    // to detector P and N side strip numbers..
    /*                       _-  Z
                    + angle /    ^
        fNstrips           v     |   N-Side        ...0
            \-------/------------|-----------\--------\
            |\\\\\\/////////////.|\\\\\\\\\\\\\\\\\\\\|
            |0\\\\/////////////..|.\\\\\\\\\\\\\\\\\\\|
            |00\\/////////////...|..\\\\\\\\\\\\\\\\\\|
       X <--|000/////////////... |...\\\\\\\\\\\\\\\\\|
            |00/////////////...  | ...\\\\\\\\\\\\\\\\|
            |0/////////////...   |  ...\\\\\\\\\\\\\\\|
            |//////////////...   |  ...\\\\\\\\\\\\\\\|
            /-----\--------------|--------------------/
        fNstrips-1             P-Side              ...0
                     |0\
                     |00\
	Dead region: |000/
                     |00/
                     |0/
    // expects x, z in microns
    */
    Float_t StereoP, StereoN;
    Angles(StereoP,StereoN);
    Float_t tanP = TMath::Tan(StereoP);
    Float_t tanN = TMath::Tan(-StereoN);
    Float_t x1 = x;
    Float_t z1 = z;
    x1 += fDx/2;
    z1 += fDz/2;
    x   = (x1 - z1*tanP)/fPitch;
    z   = (x1 - tanN*(z1 - fDz))/fPitch;
}
//----------------------------------------------------------------------
void AliITSsegmentationSSD::GetPadIxz(Float_t x,Float_t z,Int_t &iP,Int_t &iN){
  // returns P and N sided strip numbers for a given location.
    /*                       _-  Z
                    + angle /    ^
        fNstrips           v     |   N-Side        ...0
            \-------/------------|-----------\--------\
            |\\\\\\/////////////.|\\\\\\\\\\\\\\\\\\\\|
            |0\\\\/////////////..|.\\\\\\\\\\\\\\\\\\\|
            |00\\/////////////...|..\\\\\\\\\\\\\\\\\\|
       X <--|000/////////////... |...\\\\\\\\\\\\\\\\\|
            |00/////////////...  | ...\\\\\\\\\\\\\\\\|
            |0/////////////...   |  ...\\\\\\\\\\\\\\\|
            |//////////////...   |  ...\\\\\\\\\\\\\\\|
            /-----\--------------|--------------------/
        fNstrips-1             P-Side              ...0
                     |0\
                     |00\
	Dead region: |000/
                     |00/
                     |0/

    // expects x, z in microns
  */ 

    Float_t StereoP, StereoN;
    Angles(StereoP,StereoN);
    Float_t tanP=TMath::Tan(StereoP);
    Float_t tanN=TMath::Tan(StereoN);
    Float_t x1=x,z1=z;
    x1 += fDx/2;
    z1 += fDz/2;
    Float_t  ldX = x1 - z1*tanP;          // distance from left-down edge 
    iP = (Int_t)(ldX/fPitch);
    iP = (iP<0)? -1: iP;      
    iP = (iP>fNstrips)? -1: iP;

    ldX = x1 - tanN*(fDz - z1);
    iN = (Int_t)(ldX/fPitch);
    iN = (iN<0)? -1: iN;
    iN = (iN>fNstrips)? -1: iN;

}
//-------------------------------------------------------
void AliITSsegmentationSSD::GetPadCxz(Int_t iP,Int_t iN,Float_t &x,Float_t &z){
    // actually this is the GetCrossing(Float_t &,Float_t &)
    // returns local x, z  in microns !

    Float_t Dx = fDx; // detector size in x direction, microns
    Float_t Dz = fDz; // detector size in z direction, microns
    Float_t xP; // x coordinate in the P side from the first P strip
    Float_t xN; // x coordinate in the N side from the first N strip
    Float_t StereoP, StereoN;
    Angles(StereoP,StereoN);
    Float_t kP=TMath::Tan(StereoP);
    Float_t kN=TMath::Tan(StereoN);

    xP=iP*fPitch;
    xN=iN*fPitch; 
    x = xP + kP*(Dz*kN-xP+xN)/(kP+kN);
    z = (Dz*kN-xP+xN)/(kP+kN); 
    x -= Dx/2;
    z -= Dz/2;
    //if(TMath::Abs(z) > Dz/2) cout<<"Warning, wrong z local ="<<z<<endl; 
    // Check that zL is inside the detector for the 
    // correspondent xP and xN coordinates

    return;   
}
//______________________________________________________________________
void AliITSsegmentationSSD::LocalToDet(Float_t x,Float_t z,
				       Int_t &iP,Int_t &iN){
    // Transformation from Geant cm detector center local coordinates
    // to detector P and N side strip numbers..
    /*                       _-  Z
                    + angle /    ^
        fNstrips           v     |   N-Side        ...0
            \-------/------------|-----------\--------\
            |\\\\\\/////////////.|\\\\\\\\\\\\\\\\\\\\|
            |0\\\\/////////////..|.\\\\\\\\\\\\\\\\\\\|
            |00\\/////////////...|..\\\\\\\\\\\\\\\\\\|
       X <--|000/////////////... |...\\\\\\\\\\\\\\\\\|
            |00/////////////...  | ...\\\\\\\\\\\\\\\\|
            |0/////////////...   |  ...\\\\\\\\\\\\\\\|
            |//////////////...   |  ...\\\\\\\\\\\\\\\|
            /-----\--------------|--------------------/
        fNstrips-1             P-Side              ...0
                     |0\
                     |00\
	Dead region: |000/
                     |00/
                     |0/
    */
    const Double_t kconst = 1.0E-04; // convert microns to cm.

    x /= kconst;  // convert to microns
    z /= kconst;  // convert to microns
    this->GetPadTxz(x,z);

    // first for P side
    iP = (Int_t) x;
    if(iP<0 || iP>=fNstrips) iP=-1; // strip number must be in range.
    // Now for N side)
    iN = (Int_t) z;
    if(iN<0 || iN>=fNstrips) iN=-1; // strip number must be in range.
    return;
}
//----------------------------------------------------------------------
void AliITSsegmentationSSD::DetToLocal(Int_t ix,Int_t iPN,
				       Float_t &x,Float_t &z){
    // Transformation from detector segmentation/cell coordiantes starting
    // from 0. iPN=0 for P side and 1 for N side strip. Returned is z=0.0
    // and the corresponding x value..
    /*                       _-  Z
                    + angle /    ^
        fNstrips           v     |   N-Side        ...0
            \-------/------------|-----------\--------\
            |\\\\\\/////////////.|\\\\\\\\\\\\\\\\\\\\|
            |0\\\\/////////////..|.\\\\\\\\\\\\\\\\\\\|
            |00\\/////////////...|..\\\\\\\\\\\\\\\\\\|
       X <--|000/////////////... |...\\\\\\\\\\\\\\\\\|
            |00/////////////...  | ...\\\\\\\\\\\\\\\\|
            |0/////////////...   |  ...\\\\\\\\\\\\\\\|
            |//////////////...   |  ...\\\\\\\\\\\\\\\|
            /-----\--------------|--------------------/
        fNstrips-1             P-Side              ...0
                     |0\
                     |00\
	Dead region: |000/
                     |00/
                     |0/
    */
    // for strips p-side
    // x = a + b + z*tan(fStereoP); a = Dpx(iP)*(iP+0.5)-dx; b = dz*th;
    // for strips n-side
    // x = a + b + z*tan(fStereoP); a = Dpx(iN)*(iN+0.5)-dx; b = -dz*th;
    const Double_t kconst = 1.0E-04; // convert microns to cm.
    Float_t flag=kconst*Dx(); // error value
    Double_t th=0.0,dx,dz,i,a,b=0.0,xb[4],zb[4];
    Float_t StereoP, StereoN;
    Angles(StereoP,StereoN);

    z = 0.0;  // Strip center in z.
    if(iPN<0 || iPN>1){// if error return full detector size in x.
	x = z = flag; 
	return;
    } // end if
    if(ix<0 || ix>=fNstrips) { // if error return full detector size in x.
	x = z = flag;
	return;
    } // end if
    i  = (Double_t) ix;      // convert to double
    dx = 0.5*kconst*Dx();    // half distance in x in cm
    dz = 0.5*kconst*Dz();    // half distance in z in cm
    a  = kconst*Dpx(ix)*(i+0.5)-dx; // Min x value.
    if(iPN==0){ //P-side angle defined backwards.
	th = TMath::Tan(StereoP); 
	b  = dz*th;
    }else if(iPN==1){ // N-side
	 th = TMath::Tan(-StereoN);
	 b  = -dz*th;
    } // end if
    // compute average/center position of the strip.
    xb[0] = +dx; if(th!=0.0) zb[0] = (+dx-a-b)/th; else zb[0] = 0.0;
    xb[1] = -dx; if(th!=0.0) zb[1] = (-dx-a-b)/th; else zb[1] = 0.0;
    xb[2] = a+b+dz*th; zb[2] = +dz;
    xb[3] = a+b-dz*th; zb[3] = -dz;
    x = 0.0; z = 0.0;
    for(Int_t j=0;j<4;j++){
	if(xb[j]>=-dx && xb[j]<=dx && zb[j]>=-dz && zb[j]<=dz){
	    x += xb[j];
	    z += zb[j];
	} // end if
    } // end for
    x *= 0.5;
    z *= 0.5;
    return;
}
//----------------------------------------------------------------------
Bool_t AliITSsegmentationSSD::GetCrossing(Int_t iP,Int_t iN,
					  Float_t &x,Float_t &z,
					  Float_t c[2][2]){
    // Given one P side strip and one N side strip, Returns kTRUE if they
    // cross each other and the location of the two crossing strips and
    // their correxlation matrix c[2][2].
    /*                       _-  Z
                    + angle /    ^
        fNstrips           v     |   N-Side        ...0
            \-------/------------|-----------\--------\
            |\\\\\\/////////////.|\\\\\\\\\\\\\\\\\\\\|
            |0\\\\/////////////..|.\\\\\\\\\\\\\\\\\\\|
            |00\\/////////////...|..\\\\\\\\\\\\\\\\\\|
       X <--|000/////////////... |...\\\\\\\\\\\\\\\\\|
            |00/////////////...  | ...\\\\\\\\\\\\\\\\|
            |0/////////////...   |  ...\\\\\\\\\\\\\\\|
            |//////////////...   |  ...\\\\\\\\\\\\\\\|
            /-----\--------------|--------------------/
        fNstrips-1             P-Side              ...0
                     |0\
                     |00\
	Dead region: |000/
                     |00/
                     |0/
       c[2][2] is defined as follows
       /c[0][0]  c[0][1]\ /delta iP\ = /delta x\
       \c[1][0]  c[1][1]/ \delta iN/ = \delta z/
    */
    const Double_t kconst = 1.0E-04; // convert microns to cm.
    Double_t thp,thn,th,dx,dz,p,ip,in;
    Float_t StereoP, StereoN;
    Angles(StereoP,StereoN);
    
    thp = TMath::Tan(StereoP);
    thn = TMath::Tan(-StereoN);
    th  = thp-thn;
    if(th==0.0) { // parall strips then never cross.
	x = 0.0;
	z = 0.0;
	c[0][0] = c[1][0] = c[0][1] = c[1][1] = 0.0;
	return kFALSE;
    } // end if
    // The strips must cross some place in space.
    ip = (Double_t) iP;       // convert to double now for speed
    in = (Double_t) iN;       // convert to double now for speed
    dx = 0.5*kconst*Dx();     // half distance in x in cm
    dz = 0.5*kconst*Dz();     // half distance in z in cm
    p  = kconst*Dpx(iP);      // Get strip spacing/pitch now
    x  = 0.5*p+dx + (p*(in*thp-ip*thn)-2.0*dz*thp*thn)/th;
    z  =(p*(in-ip)-dz*(thp+thn))/th;
    // compute correlations.
    c[0][0] = -thn*p/th; // dx/diP
    c[1][1] = p/th;      // dz/diN
    c[0][1] = p*thp/th;  // dx/diN
    c[1][0] = -p/th;     // dz/diP
    if(x<-dx || x>dx || z<-dz || z>dz) return kFALSE; // crossing is outside
                                                      // of the detector so
                                                      // these strips don't
                                                      // cross.
    return kTRUE;
}

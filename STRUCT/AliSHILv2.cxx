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
Revision 1.4  2002/09/02 15:32:15  morsch
Gsbool calls to resolve MANY added (I. Hrivnacova)

Revision 1.3  2002/07/25 10:00:08  morsch
par4 size increased.

Revision 1.2  2002/07/15 08:16:35  morsch
New shield geometry.

*/


#include "AliSHILv2.h"
#include "AliRun.h"
#include "AliMC.h"
#include "AliConst.h"
#include "AliALIFE.h"

ClassImp(AliSHILv2)
 
//_____________________________________________________________________________
AliSHILv2::AliSHILv2()
{
  //
  // Default constructor for muon shield
  //
}
 
//_____________________________________________________________________________
AliSHILv2::AliSHILv2(const char *name, const char *title)
  : AliSHIL(name,title)
{
  //
  // Standard constructor for muon shield
  //
  SetMarkerColor(7);
  SetMarkerStyle(2);
  SetMarkerSize(0.4);
  // Pb  cone not yet compatible with muon chamber inner radii
  // Switched off by default
  SetWriteGeometry();
  SetPbCone();
}
 
//_____________________________________________________________________________
void AliSHILv2::CreateGeometry()
{
  // 
  // Build muon shield geometry
  //
  //
  //Begin_Html
  /*
    <img src="picts/AliSHILv2.gif">
  */
  //End_Html
  //Begin_Html
  /*
    <img src="picts/AliSHILv2Tree.gif">
  */
  //End_Html

    Float_t cpar[5], cpar0[5], tpar[3], par1[100], pars1[100], par2[100], par3[100], 
	par4[24], par0[100];
    Float_t dz, dZ;
    
    Int_t *idtmed = fIdtmed->GetArray()-1699;
    
#include "ABSOSHILConst.h"
#include "SHILConst2.h"
    
    enum {kC=1705, kAl=1708, kFe=1709, kCu=1710, kW=1711, kPb=1712,
	  kNiCuW=1720, kVacuum=1715, kAir=1714, kConcrete=1716,
	  kPolyCH2=1717, kSteel=1709, kInsulation=1713};	
//
// Material of the rear part of the shield
  Int_t iHeavy=kNiCuW;
  if (fPbCone) iHeavy=kPb;
//
//
// begin Fluka
  AliALIFE* flukaGeom = new AliALIFE("beamshield.alife", "beamshield_vol.inp");
  Int_t i=0,ifl=0;
  Float_t posfluka[3]={0., 0., 0.};
  Float_t zfluka[12], rfluka1[12], rfluka2[12], rfluka3[12] ;  
//
// end Fluka  
  
  
//
// Mother volume
//
  Float_t dRear1=dRear;
  
  Float_t zstart=zRear-dRear1;
  
  par0[0]  = 0.;
  par0[1]  = 360.;
  par0[2]  = 28.;

  Float_t dl=(zvac12-zstart)/2.;
  dz=zstart+dl;
//
// start
  par0[3]  = -dl;
  par0[4]  = 0.;
  par0[5]  = zstart * TMath::Tan(accMin);
// recess station 1
  par0[6]  = -dz+zch11;
  par0[7]  = 0.;
  par0[8]  = zch11 * TMath::Tan(accMin);

  par0[9]   = par0[6];
  par0[10]  = 0.;
  par0[11]  = 17.9;

  par0[12]  = -dz+zch12;
  par0[13]  = 0.;
  par0[14]  = 17.9;

  par0[15]  = par0[12];
  par0[16]  = 0.;
  par0[17]  = zch12 * TMath::Tan(accMin);
// recess station 2
  par0[18]  = -dz+zch21;
  par0[19]  = 0.;
  par0[20]  = zch21 * TMath::Tan(accMin);

  par0[21]  = -dz+zch21;
  par0[22] = 0.;
  par0[23] = 23.;

  par0[24]  = -dz+zch22;
  par0[25] = 0.;
  par0[26] = 23.;

  par0[27]  = -dz+zch22;
  par0[28]  = 0.;
  par0[29]  = zch22 * TMath::Tan(accMin);
//
  par0[30] = -dz+zvac6;
  par0[31] = 0.;
  par0[32] = zvac6 * TMath::Tan(accMin);
// end of 2 deg cone
  par0[33] = -dz+zConeE;
  par0[34] = 0.;
  par0[35] = 30.;

  par0[36] = -dz+zch31;
  par0[37] = 0.;
  par0[38] = 30.;

  par0[39] = -dz+zch31;
  par0[40] = 0.;
  par0[41] = 29.;

  par0[42] = -dz+zch32;
  par0[43] = 0.;
  par0[44] = 29.;
// start of 1.6 deg cone
  par0[45] = -dz+zch32;
  par0[46] = 0.;
  par0[47] = 30.+(zch32-zConeE)*TMath::Tan(thetaOpenPbO);
// recess station 4
  par0[48] = -dz+zch41;
  par0[49] = 0.;
  par0[50] = 30.+(zch41-zConeE)*TMath::Tan(thetaOpenPbO);

  par0[51] = -dz+zch41;
  par0[52] = 0.;
  par0[53] = 37.5;

  par0[54] = -dz+zch42;
  par0[55] = 0.;
  par0[56] = 37.5;

  par0[57] = -dz+zch42;
  par0[58] = 0.;
  par0[59] = 30.+(zch42-zConeE)*TMath::Tan(thetaOpenPbO);

// recess station 5

  par0[60] = -dz+zch51;
  par0[61] = 0.;
  par0[62] = 30.+(zch51-zConeE)*TMath::Tan(thetaOpenPbO);

  par0[63] = -dz+zch51;
  par0[64] = 0.;
  par0[65] = 37.5;

  par0[66] = -dz+zch52;
  par0[67] = 0.;
  par0[68] = 37.5;

  par0[69] = -dz+zch52;
  par0[70] = 0.;
  par0[71] = 30.+(zch52-zConeE)*TMath::Tan(thetaOpenPbO);

// end of cone

  par0[72] = -dz+zvac10;
  par0[73] = 0.;
  par0[74] = 30.+(zvac10-zConeE)*TMath::Tan(thetaOpenPbO);

  par0[75] = -dz+zvac10;
  par0[76] = 0.;
  par0[77] = R42;

  par0[78] = -dz+zvac11;
  par0[79] = 0.;
  par0[80] = R42;

  par0[81] = -dz+zvac11;
  par0[82] = 0.;
  par0[83] = R43;

  par0[84] = -dz+zvac12;
  par0[85] = 0.;
  par0[86] = R43;

  gMC->Gsvolu("YMOT", "PCON", idtmed[kVacuum], par0, 87);
  dz=zstart+dl;
  gMC->Gspos("YMOT", 1, "ALIC", 0., 0., dz, 0, "ONLY"); 
  gMC->Gsbool("YMOT","L3DO");
  gMC->Gsbool("YMOT","L3O1");
  gMC->Gsbool("YMOT","L3O2");
 
//

  dZ=-dl;

//
// First section: bellows below and behind front absorber 
// 
//
  par1[ 0]  = 0.;
  par1[ 1]  = 360.;
  par1[ 2]  = 14.;
  dl=(zvac4-zstart)/2.;
  
  par1[ 3]  = -dl;
  par1[ 4]  = rAbs+(zstart-zOpen) * TMath::Tan(thetaOpen1);
  par1[ 5]  = zstart * TMath::Tan(accMin);

  par1[ 6]  = -dl+dRear1;
  par1[ 7]  = par1[4] + dRear1 * TMath::Tan(thetaOpen1);
  par1[ 8]  = zRear * TMath::Tan(accMin);

  par1[ 9]  = -dl+dRear1;
  par1[10]  = par1[7];
  par1[11]  = R11;

  par1[12]  = -dl+zvac41-zstart;
  par1[13]  = rAbs + (zvac41-zOpen) * TMath::Tan(thetaOpen1);
  par1[14]  = R11;

  par1[15]  = par1[12];
  par1[16]  = par1[13];
  par1[17]  = R21;


  par1[18]  = -dl+zvac1-zstart;
  par1[19]  = rAbs+ (zvac1-zOpen) * TMath::Tan(thetaOpen1);
  par1[20]  = R21;

  par1[21]  = par1[18]+dr11/10.;
  par1[22]  = par1[19]+dr11;
  par1[23]  = R21;


  par1[24]  = -dl+(zvac1+dr11/10.+dB1-zstart);
  par1[25]  = par1[22];
  par1[26]  = R21;

  par1[27]  = par1[24]+dr12;
  par1[28]  = par1[25]+dr12;
  par1[29]  = R21;

  par1[30]  = par1[27]+dF1;
  par1[31]  = par1[28];
  par1[32]  = R21;

  par1[33]  = par1[30]+dr12;
  par1[34]  = par1[31]-dr12; 
  par1[35]  = R21;

  par1[36] = par1[33]+dB1;
  par1[37] = par1[34];
  par1[38] = R21;

  par1[39] = par1[36]+dr13;
  par1[40] = par1[37]-dr13;
  par1[41] = R21;

  par1[42] = -dl+zvac4-zstart;
  par1[43] = par1[40];
  par1[44] = R21;

  Float_t r2  = par1[43];
  Float_t rBox= par1[43]-0.1;
  Float_t rc1 = par1[7];

  gMC->Gsvolu("YGO1", "PCON", idtmed[kNiCuW], par1, 45);

//
// begin Fluka
  for (ifl=0; ifl<14; ifl++) {
      zfluka[ifl]=par1[3+3*ifl]+dl+zRear-dRear;
      rfluka1[ifl] = par1[4+3*ifl];
      rfluka2[ifl] = par1[5+3*ifl]; 
      if (ifl > 3)  rfluka2[ifl]=rfluka2[ifl]-dRSteel1;
  }

  
  Float_t rfluka0[8]={rBox,rBox,rBox,rBox,rBox,rBox,rBox,rBox};
  
  flukaGeom->Comment("1st part: Shield");
// Use default for first three cones
  flukaGeom->SetDefaultVolume("*ACR02");
  rfluka2[0]=rfluka2[1]=rfluka2[2]=-1;
//
  flukaGeom->Comment("Shield");         
  flukaGeom->PolyCone(rfluka1,    rfluka2,   zfluka,   12, posfluka, "NIW", "MF", "$SHS");
  flukaGeom->Comment("Vacuum");
  flukaGeom->PolyCone(rfluka0,  rfluka1+2, zfluka+2,   8, posfluka, "VACUUM", "MF", "$SHS");
//
// end Fluka

  for (i=0; i<45; i++)  pars1[i]  = par1[i];
  for (i=4; i<44; i+=3) pars1[i]  = 0.;

  gMC->Gsvolu("YMO1", "PCON", idtmed[kVacuum+40], pars1, 45);
  gMC->Gspos("YGO1", 1, "YMO1", 0., 0., 0., 0, "ONLY");  
  dZ+=dl;
  gMC->Gspos("YMO1", 1, "YMOT", 0., 0., dZ, 0, "ONLY");  
  dZ+=dl;


  tpar[0]=R21-0.6;
  tpar[1]=R21;
  tpar[2]=(zvac4-zvac41)/2.;
  gMC->Gsvolu("YSE1", "TUBE", idtmed[kSteel], tpar, 3);
  dz=dl-tpar[2];
  gMC->Gspos("YSE1", 1, "YGO1", 0., 0., dz, 0, "ONLY");


  tpar[0]=R11-0.6;
  tpar[1]=R11;
  tpar[2]=(zvac41-zstart-dRear1)/2.;
  gMC->Gsvolu("YSE2", "TUBE", idtmed[kSteel], tpar, 3);
  dz=dl-tpar[2]-(zvac4-zvac41);
  gMC->Gspos("YSE2", 1, "YGO1", 0., 0., dz, 0, "ONLY");

// begin Fluka
  flukaGeom->Comment("1st part: Steel Envelope");
  flukaGeom->Cylinder(tpar[0], tpar[1], zRear, zvac4, posfluka, "NIW", "MF", "$SHS");
//
// end Fluka

//
// 1st section: vacuum system
//
//
// Bellow 1
//
  tpar[0]=rB1;
  tpar[1]=rB1+hB1;
  tpar[2]=eB1/2.;
  gMC->Gsvolu("YB11", "TUBE", idtmed[kSteel+40], tpar, 3);
  Float_t dl1=tpar[2];
  
  tpar[0]=rB1+hB1-eB1;
  tpar[1]=rB1+hB1;
  tpar[2]=(lB1/2.-2.*eB1)/2.;
  gMC->Gsvolu("YB12", "TUBE", idtmed[kSteel+40], tpar, 3);
  Float_t dl2=tpar[2];

  tpar[0]=rB1-eB1;
  tpar[1]=rB1;
  tpar[2]=lB1/8.;
  gMC->Gsvolu("YB13", "TUBE", idtmed[kSteel+40], tpar, 3);
  Float_t dl3=tpar[2];


  tpar[0]=0;
  tpar[1]=rB1+hB1;
  tpar[2]=lB1/2.;
  gMC->Gsvolu("YBU1", "TUBE", idtmed[kVacuum+40], tpar, 3);

  dz=-tpar[2]+dl3;
  gMC->Gspos("YB13", 1, "YBU1", 0., 0., dz, 0, "ONLY"); 
  dz+=dl3;
  dz+=dl1;  
  gMC->Gspos("YB11", 1, "YBU1", 0., 0., dz, 0, "ONLY"); 
  dz+=dl1;  
  dz+=dl2;  
  gMC->Gspos("YB12", 1, "YBU1", 0., 0., dz, 0, "ONLY"); 
  dz+=dl2;  
  dz+=dl1;
  gMC->Gspos("YB11", 2, "YBU1", 0., 0., dz, 0, "ONLY"); 
  dz+=dl1;
  dz+=dl3;
  gMC->Gspos("YB13", 2, "YBU1", 0., 0., dz, 0, "ONLY"); 
  

  tpar[0]=0;
  tpar[1]=rB1+hB1+0.5;
  tpar[2]=12.*lB1/2.;
  gMC->Gsvolu("YBM1", "TUBE", idtmed[kVacuum+40], tpar, 3);
  Float_t bsize = tpar[2];
  tpar[0]=rB1+hB1;
  gMC->Gsvolu("YBI1", "TUBE", idtmed[kInsulation+40], tpar, 3);
  gMC->Gspos("YBI1", 2, "YBM1", 0., 0., 0., 0, "ONLY"); 

  dz=-bsize+lB1/2.;

  for (i=0; i<12; i++) {
    gMC->Gspos("YBU1", i+1 , "YBM1", 0., 0., dz, 0, "ONLY"); 
    dz+=lB1;
  }

  dz=-dl+(zvac1-zstart)+dr11/10.+bsize;
  gMC->Gspos("YBM1", 1, "YMO1", 0., 0., dz, 0, "ONLY"); 

//  dz=dl-dr13-(zvac4-zvac3)-bsize;
//  gMC->Gspos("YBM1", 2, "YMO1", 0., 0., dz, 0, "ONLY"); 


//
// Flange

  tpar[0]=0;
  tpar[1]=rF1+0.6;
  tpar[2]=dF1/2.;
  gMC->Gsvolu("YFM1", "TUBE", idtmed[kVacuum+40], tpar, 3);
// Steel
  tpar[0]=rB1;
  tpar[1]=rF1+0.6;
  tpar[2]=dF1/2.;
  gMC->Gsvolu("YF11", "TUBE", idtmed[kSteel+40], tpar, 3);
// Insulation
  tpar[0]=rF1;
  tpar[1]=rF1+0.5;
  tpar[2]=dF1/2.;
  gMC->Gsvolu("YF12", "TUBE", idtmed[kInsulation+40], tpar, 3);


  gMC->Gspos("YF11", 1, "YFM1", 0., 0., 0., 0, "ONLY"); 
  gMC->Gspos("YF12", 1, "YFM1", 0., 0., 0., 0, "ONLY"); 
  dz=-dl+(zvac3-zstart)-2.*dr13-tpar[2];
  gMC->Gspos("YFM1", 2, "YMO1", 0., 0., dz, 0, "ONLY"); 

//
// pipe between flange and bellows
//
// Steel 
  tpar[0] = rB1-dTubeS;
  tpar[1] = rB1+0.6;
  tpar[2] = (zvac3-zvac1-2.*dr13-dr11/10.-dF1-2.*bsize)/2.;
  gMC->Gsvolu("YPF1", "TUBE", idtmed[kSteel+40], tpar, 3);
// Insulation
  tpar[0]=rB1;
  tpar[1]=rB1+0.5;
  gMC->Gsvolu("YPS1", "TUBE", idtmed[kInsulation+40], tpar, 3);
  gMC->Gspos("YPS1", 1, "YPF1", 0., 0., 0., 0, "ONLY"); 
  dz=-dl+(zvac1-zstart)+dr11/10.+2.*bsize+tpar[2];
  gMC->Gspos("YPF1", 1, "YMO1", 0., 0., dz, 0, "ONLY"); 
//  dz=-dl+(zvac2-zstart)+dF1/2.+tpar[2];
//  gMC->Gspos("YPF1", 2, "YMO1", 0., 0., dz, 0, "ONLY"); 

//
// begin Fluka
  flukaGeom->Comment("First Bellow");
  Float_t z1=zvac1+dr11;
  Float_t z2;
  
  for (i=0; i<10; i++) {
      z2=z1+eB1;
      flukaGeom->Cylinder(0., rB1, z1, z2, posfluka, "VACUUM", "MF", "$SHH");
      flukaGeom->Cylinder(rB1, rB1+hB1, z1, z2, posfluka, "STEEL", "MF", "$SHH");
      
      z1=z2;
      z2+=lB1/2.-eB1;
      flukaGeom->Cylinder(0., rB1+hB1-eB1, z1, z2, posfluka, "VACUUM", "MF", "$SHH");
      flukaGeom->Cylinder(rB1+hB1-eB1, rB1+hB1, z1, z2, posfluka, "STEEL", "MF", "$SHH");
      z1=z2;
      z2=z1+eB1;
      flukaGeom->Cylinder(0., rB1, z1, z2, posfluka, "VACUUM", "MF", "$SHH");
      flukaGeom->Cylinder(rB1, rB1+hB1, z1, z2, posfluka, "STEEL", "MF", "$SHH");
      z1=z2;
      z2+=lB1/2.-eB1;
      flukaGeom->Cylinder(0., rB1, z1, z2, posfluka, "VACUUM", "MF", "$SHH");
      flukaGeom->Cylinder(rB1, rB1+eB1, z1, z2, posfluka, "STEEL", "MF", "$SHH");
      flukaGeom->Cylinder(rB1+eB1, rB1+hB1, z1, z2, posfluka, "AIR", "MF", "$SHH");
      z1=z2;
   }
  flukaGeom->Cylinder(rB1+hB1, rB1+hB1+0.5, zvac1+dr11, z1, posfluka, "AIR", "MF", "$SHH");
  flukaGeom->Cylinder(rB1+hB1+0.5, rBox, zvac1+dr11, z1, posfluka, "AIR", "MF", "$SHH");
  Float_t zcy1=z1;
  

  flukaGeom->Comment("Second Bellow");
  z1=zvac3-dr13;
  for (i=0; i<10; i++) {
      z2=z1-eB1;
      flukaGeom->Cylinder(0., rB1, z2, z1, posfluka, "VACUUM", "MF", "$SHH");
      flukaGeom->Cylinder(rB1, rB1+hB1, z2, z1, posfluka, "STEEL", "MF", "$SHH");
      z1=z2;
      z2-=lB1/2.-eB1;
      flukaGeom->Cylinder(0., rB1+hB1-eB1, z2, z1, posfluka, "VACUUM", "MF", "$SHH");
      flukaGeom->Cylinder(rB1+hB1-eB1, rB1+hB1, z2, z1, posfluka, "STEEL", "MF", "$SHH");
      z1=z2;
      z2=z1-eB1;
      flukaGeom->Cylinder(0., rB1, z2, z1, posfluka, "VACUUM", "MF", "$SHH");
      flukaGeom->Cylinder(rB1, rB1+hB1, z2, z1, posfluka, "STEEL", "MF", "$SHH");
      z1=z2;
      z2-=lB1/2.-eB1;
      flukaGeom->Cylinder(0., rB1, z2, z1, posfluka, "VACUUM", "MF", "$SHH");
      flukaGeom->Cylinder(rB1, rB1+eB1, z2, z1, posfluka, "STEEL", "MF", "$SHH");
      flukaGeom->Cylinder(rB1+eB1, rB1+hB1, z2, z1, posfluka, "AIR", "MF", "$SHH");
      z1=z2;
   }
  flukaGeom->Cylinder(rB1+hB1, rB1+hB1+0.5, z1, zvac3-dr13, posfluka, "AIR", "MF", "$SHH");
  flukaGeom->Cylinder(rB1+hB1+0.5, rBox,    z1, zvac3-dr13, posfluka, "AIR", "MF", "$SHH");

  Float_t zcy2=z1;
  flukaGeom->Comment("Flange");
  Float_t zfl=(zcy1+zcy2)/2.;

  z1=zfl-dF1/2.;
  z2=zfl+dF1/2.;  
  flukaGeom->Cylinder(0.,rF1-2.        , z1, z2, posfluka, "VACUUM", "MF", "$SHH");
  flukaGeom->Cylinder(rF1-2., rF1      , z1, z2, posfluka, "STEEL", "MF", "$SHH");
  flukaGeom->Cylinder(rF1, rF1+0.05     , z1, z2, posfluka, "AIR", "MF", "$SHH");
  flukaGeom->Cylinder(rF1+0.05, rBox    , z1, z2, posfluka, "AIR", "MF", "$SHH");
  z2=z1;
  z1=z2-dFlange;
  flukaGeom->Cylinder(0.,rB1           , z1, z2, posfluka, "VACUUM", "MF", "$SHH");
  flukaGeom->Cylinder(rB1, rF1         , z1, z2, posfluka, "STEEL", "MF", "$SHH");
  flukaGeom->Cylinder(rF1, rF1+0.5     , z1, z2, posfluka, "AIR", "MF", "$SHH");
  flukaGeom->Cylinder(rF1+0.5, rBox    , z1, z2, posfluka, "AIR", "MF", "$SHH");
  z2=z1;
  z1=zcy1;
  flukaGeom->Cylinder(0.,rB1           , z1, z2, posfluka, "VACUUM", "MF", "$SHH");
  flukaGeom->Cylinder(rB1, rB1+0.1     , z1, z2, posfluka, "STEEL", "MF", "$SHH");
  flukaGeom->Cylinder(rB1+0.1, rB1+0.6 , z1, z2, posfluka, "AIR", "MF", "$SHH");
  flukaGeom->Cylinder(rB1+0.6, rBox    , z1, z2, posfluka, "AIR", "MF", "$SHH");

  z1=zfl+dF1/2.;
  z2=z1+dFlange;
  flukaGeom->Cylinder(0.,rB1           , z1, z2, posfluka, "VACUUM", "MF", "$SHH");
  flukaGeom->Cylinder(rB1, rF1         , z1, z2, posfluka, "STEEL", "MF", "$SHH");
  flukaGeom->Cylinder(rF1, rF1+0.5     , z1, z2, posfluka, "AIR", "MF", "$SHH");
  flukaGeom->Cylinder(rF1+0.5, rBox    , z1, z2, posfluka, "AIR", "MF", "$SHH");
  z1=z2;
  z2=zcy2;
  flukaGeom->Cylinder(0.,rB1           , z1, z2, posfluka, "VACUUM", "MF", "$SHH");
  flukaGeom->Cylinder(rB1, rB1+0.1     , z1, z2, posfluka, "STEEL", "MF", "$SHH");
  flukaGeom->Cylinder(rB1+0.1, rB1+0.6 , z1, z2, posfluka, "AIR", "MF", "$SHH");
  flukaGeom->Cylinder(rB1+0.6, rBox    , z1, z2, posfluka, "AIR", "MF", "$SHH");
// end Fluka
//

// Pipe+Heating     1.5 mm 
// Heating Jacket   5.0 mm
// Protection       1.0 mm
// ========================
//                  7.5 mm
// pipe and heating jackets outside bellows
//
// left side
  cpar0[0]=(zvac1+dr11/10.-zstart)/2;
  cpar0[1]=rVacu-0.05  +(zstart-zOpen)*TMath::Tan(thetaOpen1);
  cpar0[2]=rVacu+0.7   +(zstart-zOpen)*TMath::Tan(thetaOpen1);
  cpar0[3]=cpar0[1]+2.*cpar0[0]*TMath::Tan(thetaOpen1);
  cpar0[4]=cpar0[2]+2.*cpar0[0]*TMath::Tan(thetaOpen1);
  gMC->Gsvolu("YV11", "CONE", idtmed[kSteel+40], cpar0, 5);
//
// insulation
  dTubeS=0.15;
  cpar[0]=cpar0[0];
  cpar[1]=cpar0[1]+0.15;
  cpar[2]=cpar0[1]+0.65;
  cpar[3]=cpar0[3]+0.15;
  cpar[4]=cpar0[3]+0.65;
  gMC->Gsvolu("YI11", "CONE", idtmed[kInsulation+40], cpar, 5);
  gMC->Gspos("YI11", 1, "YV11", 0., 0., 0., 0, "ONLY"); 
  dz=-dl+cpar0[0];
  gMC->Gspos("YV11", 1, "YMO1", 0., 0., dz, 0, "ONLY"); 

// begin Fluka
//
  Float_t rf1[10], rf2[10];
  rf1[0]=0.; rf2[0]=0.;
  rf1[1] = cpar0[1];
  rf2[1] = cpar0[3];


  rf1[2]=rf1[1]+0.15; rf1[3]=rf1[2]+0.5; rf1[4]=rf1[3]+0.1;
  rf1[5]=par1[4]; 
  rf2[2]=rf2[1]+0.15; rf2[3]=rf2[2]+0.5; rf2[4]=rf2[3]+0.1; 
  rf2[5]=par1[7];
  
  char* materialsA[7] 
      = {"VACUUM", "STEEL", "PIPEINSU", "STEEL", "AIR", "AIR"};
  char* fieldsA[7] 
      = {"MF", "MF", "MF", "MF", "MF", "MF"};
  char* cutsA[7] 
      = {"$SHH","$SHH","$SHH","$SHH","$SHH","$SHH","$SHH"};

  flukaGeom->Comment("1st part: Beam pipe lateral struture (left)");
  flukaGeom->OnionCone(rf1, rf2,  6 , zstart, zvac1, posfluka, materialsA, fieldsA, cutsA);
  for (i=0; i<7; i++) rf1[i]=rf2[i];
  for (i=1; i<7; i++) rf2[i]=rf1[i]+dr11*TMath::Tan(thetaOpen1);
  flukaGeom->OnionCone(rf1, rf2,  6 , zvac1, zvac1+dr11, posfluka, materialsA, fieldsA, cutsA);
  flukaGeom->Cone(rc1, rf2[5], rc1, rc1+dr11, zvac1 , zvac1+dr11, posfluka,"AIR", "MF", "$SHH");
//
// end Fluka


// right side
  dTubeS  = 0.35;
  dVacuS += 0.25;
  
  cpar0[0] = (zvac4-zvac3+2.*dr13)/2;
  cpar0[1] = rB1;
  cpar0[2] = cpar0[1]+dVacuS;
  cpar0[3] = cpar0[1]+2.*cpar0[0]*TMath::Tan(thetaOpenB);
  cpar0[4] = cpar0[2]+2.*cpar0[0]*TMath::Tan(thetaOpenB);
  gMC->Gsvolu("YV12", "CONE", idtmed[kSteel], cpar0, 5);
  Float_t r2V=cpar0[3];
//
// insulation
  cpar[0] = cpar0[0];
  cpar[1] = cpar0[1]+dTubeS;
  cpar[2] = cpar0[1]+dTubeS+dInsuS;
  cpar[3] = cpar0[3]+dTubeS;
  cpar[4] = cpar0[3]+dTubeS+dInsuS;
  gMC->Gsvolu("YI12", "CONE", idtmed[kInsulation], cpar, 5);
  gMC->Gspos("YI12", 1, "YV12", 0., 0., 0., 0, "ONLY"); 

  dz=dl-cpar0[0];
  gMC->Gspos("YV12", 1, "YMO1", 0., 0., dz, 0, "ONLY"); 

//
// begin Fluka
  char* materialsB[5] 
      = {"VACUUM", "STEEL", "PIPEINSU", "STEEL", "AIR"};
  
  char* fieldsB[5] 
      = {"MF", "MF", "MF", "MF", "MF"};
 
  char* cutsB[5] 
      = {"$SHH","$SHH","$SHH","$SHH","$SHH"};

  rf1[0]=rf2[0]=0.;
  rf1[1]=cpar0[1]; 
  rf2[1]=cpar0[3];

  rf1[2]=rf1[1]+dTubeS; rf1[3]=rf1[2]+dInsuS; rf1[4]=rf1[3]+dEnveS;
  rf1[5]=r2;
  
  rf2[2]=rf2[1]+dTubeS; rf2[3]=rf2[2]+dInsuS; rf2[4]=rf2[3]+dEnveS; 
  rf2[5]=r2;
  flukaGeom->Comment("1st part: Beam pipe lateral structure (right)");
  flukaGeom->OnionCone(rf1, rf2,  6 , zvac3, zvac4, posfluka, materialsB, fieldsB, cutsB);
  for (i=0; i<6; i++) rf2[i]=rf1[i];
  for (i=1; i<5; i++) rf1[i]=rf2[i];
  rf1[5]=rf2[5]+dr13;
  flukaGeom->OnionCone(rf1, rf2,  6 , zvac3-dr13, zvac3, posfluka, materialsB, fieldsB, cutsB);

//
// end Fluka

//
// Second Section
// Between first and second bellow section
//

  par2[0]  = 0.;
  par2[1]  = 360.;
  par2[2]  = 11.;
  dl=(zvac7-zvac4)/2.;
// recess station 2
  par2[3]  = -dl;
  par2[4]  = r2;
  par2[5]  = R21;

  par2[6]  = -dl+.1;
  par2[7]  = r2;
  par2[8]  = R21;

  par2[9]   = -dl+(zvac6-zvac4);
  par2[10]  = r2+(zvac6-zvac4-10.) * TMath::Tan(thetaOpen2);
  par2[11]  = R21;

  par2[12] = -dl+(zvac6-zvac4);
  par2[13] = par2[10];
  par2[14] = zvac6*TMath::Tan(accMin);

// Start of Pb section
  par2[15] = -dl+(zPb-zvac4);
  par2[16] = r2+(zPb-zvac4-10.) * TMath::Tan(thetaOpen2);
  par2[17] = zPb*TMath::Tan(accMin);
//
// end of cone following 2 deg line
  par2[18] = -dl+(zConeE-zvac4);
  par2[19] = r2+(zConeE-zvac4-10.) * TMath::Tan(thetaOpen2);
  par2[20] = 30.;
// recess station 3
  par2[21] = -dl+(zch31-zvac4);
  par2[22] = r2+(zch31-zvac4-10.) * TMath::Tan(thetaOpen2);
  par2[23] = 30.;

  par2[24] = -dl+(zch31-zvac4);
  par2[25] = r2+(zch31-zvac4-10.) * TMath::Tan(thetaOpen2);
  par2[26] = 29.;

  par2[27] = -dl+(zch32-zvac4);
  par2[28] = r2+(zch32-zvac4-10.) * TMath::Tan(thetaOpen2);
  par2[29] = 29.;

  par2[30] = -dl+(zch32-zvac4);
  par2[31] = r2+(zch32-zvac4-10.) * TMath::Tan(thetaOpen2);
  par2[32] = 30.;

  par2[33] = -dl+(zvac7-zvac4);
  par2[34] = r2+(zvac7-zvac4-10.) * TMath::Tan(thetaOpen2);
  par2[35] = 30.;

  gMC->Gsvolu("YGO2", "PCON", idtmed[kSteel+40], par2, 36);

//
// begin Fluka

  char* materials1[8] 
      = {"VACUUM", "STEEL", "PIPEINSU", "STEEL", "AIR", "NIW", "NIW", "STEEL"};
  char* fields1[8] 
      = {"MF", "MF", "MF", "MF", "MF", "MF", "MF", "MF"};
  char* cuts1[8] 
      = {"$SHH","$SHH","$SHH","$SHH","$SHH","$SHH","$SHH","$SHH"};

  flukaGeom->Comment("2nd part: Beam shield lateral struture (0)");
  // until end of recess 1
  rf1[0] = 0.; rf1[1] = r2V; rf1[2] = rf1[1] + dTubeS; rf1[3] = rf1[2] + dInsuS;
  rf1[4] = rf1[3] + dEnveS;  rf1[5] = r2; rf1[6] = rf1[5]+2.;
  rf1[7] = R11-dRSteel1;  rf1[8] = R21;

  for (i=1; i<7; i++) rf2[i]=rf1[i]+4.*TMath::Tan(thetaOpenB);
  rf2[7] = rf1[7];
  rf2[8] = rf1[8];
  flukaGeom->OnionCone(rf1, rf2,  9 , zvac4, zvac4+4, posfluka, materials1, fields1, cuts1);

  flukaGeom->Comment("2nd part: Beam shield lateral struture (1)");
  // until end of recess 2
  for (i=0; i<9; i++) rf1[i]=rf2[i];
  rf1[7] = R21-dRSteel2;  rf1[8] = R21;
  for (i=1; i<9; i++) rf2[i]=rf1[i]+(zvac6-zvac4-4.)*TMath::Tan(thetaOpenB);
  rf2[7] = rf1[7];
  rf2[8] = rf1[8];
  flukaGeom->OnionCone(rf1, rf2,  9 , zvac4+4, zvac6, posfluka, materials1, fields1, cuts1);

  flukaGeom->Comment("2nd part: Beam shield lateral struture (2)");
  // steel recess
  for (i=0; i<9; i++) rf1[i]=rf2[i];
  rf1[8] = zvac6*TMath::Tan(accMin);
  rf1[7] = R21-dRSteel2;
   
  for (i=1; i<9; i++) rf2[i]=rf1[i]+4.*TMath::Tan(thetaOpenB);
  rf2[7] = rf1[7];

  rf2[8] = -(rf1[8]+4.*TMath::Tan(accMin));
  rf1[8] = -rf1[8];

  flukaGeom->OnionCone(rf1, rf2, 9 , zvac6, zvac6+4, posfluka, materials1, fields1, cuts1);
  rf1[8] = -rf1[8];
  rf2[8] = -rf2[8];
  
  flukaGeom->Comment("2nd part: Beam shield lateral struture (3)");
  // until start of lead section
  for (i=0; i<9; i++) rf1[i]=rf2[i];
  for (i=1; i<9; i++) rf2[i]=rf1[i]+(zPb-zvac6-4.)*TMath::Tan(thetaOpenB);
  rf1[7] = rf1[8] - dRSteel2;
  rf2[8] = rf1[8] + (zPb-zvac6-4.)*TMath::Tan(accMin);
  rf2[7] = rf2[8] - dRSteel2;

  rf1[8]=-rf1[8];
  rf2[8]=-rf2[8];
  flukaGeom->OnionCone(rf1, rf2,  9 , zvac6+4, zPb, posfluka, materials1, fields1, cuts1);
  rf1[8]=-rf1[8];
  rf2[8]=-rf2[8];

  flukaGeom->Comment("2nd part: Beam shield lateral struture (4)");
  // until end of 2deg
  materials1[5] = "LEAD";
  materials1[6] = "LEAD";
  for (i=0; i<9; i++) rf1[i]=rf2[i];
  for (i=1; i<9; i++) rf2[i]=rf1[i]+(zConeE-zPb)*TMath::Tan(thetaOpenB);
  rf1[8] = -rf1[8];
  rf2[8] = -30.;
  rf2[7] = 26.;
  flukaGeom->OnionCone(rf1, rf2,  9 , zPb, zConeE, posfluka, materials1, fields1, cuts1);
  rf1[8]=-rf1[8];
  rf2[8]=-rf2[8];

  flukaGeom->Comment("2nd part: Beam shield lateral struture (4)");
  // until end of this section
  for (i=0; i<9; i++) rf1[i]=rf2[i];
  for (i=1; i<9; i++) rf2[i]=rf1[i]+(zvac7-zConeE)*TMath::Tan(thetaOpenB);
  rf2[8] = 30;
  rf2[7] = 26;
  flukaGeom->OnionCone(rf1, rf2,  9 , zConeE, zvac7, posfluka, materials1, fields1, cuts1);

  Float_t r3V = rf2[1];

// end Fluka

//
// Lead cone 
//
  Float_t parPb[18];
  parPb[ 0]  = 0.;
  parPb[ 1]  = 360.;
  parPb[ 2]  = 5.;
  Float_t dlPb=(zvac7-zPb)/2.;
  
  parPb[ 3]  = -dlPb;
  parPb[ 4]  =  r2+(zPb-zvac4-10.) * TMath::Tan(thetaOpen2);
  parPb[ 5]  =  zPb*TMath::Tan(accMin)-dRSteel2;
  
  parPb[ 6]  = -dlPb+(zConeE-zPb);
  parPb[ 7]  =  r2+(zConeE-zvac4-10.) * TMath::Tan(thetaOpen2);
  parPb[ 8]  = 26.;

  parPb[ 9]  = -dlPb+(zch32+4.-zPb);
  parPb[10]  =  r2+(zch32+4.-zvac4-10.) * TMath::Tan(thetaOpen2);
  parPb[11]  = 26.;

  parPb[12]  = -dlPb+(zch32+4.-zPb);
  parPb[13]  =  r2+(zch32+4.-zvac4-10.) * TMath::Tan(thetaOpen2);
  parPb[14]  = 30.;
  
  parPb[15]  = dlPb;
  parPb[16]  =  r2+(zvac7-zvac4-10.) * TMath::Tan(thetaOpen2);
  parPb[17]  = 30.;

  gMC->Gsvolu("YXO2", "PCON", idtmed[kPb], parPb, 18);	  
  gMC->Gspos("YXO2", 1, "YGO2", 0., 0., (zPb-zvac4)/2., 0, "ONLY");  
//
// Concrete replacing Pb
//
  Float_t parCC[9];
  Float_t zCC1 = 1066.;
  Float_t zCC2 = 1188.;
  
  parCC[ 0]  = 0.;
  parCC[ 1]  = 360.;
  parCC[ 2]  = 2.;
  Float_t dlCC=(zCC2-zCC1)/2.;
  parCC[ 3]  = -dlCC;
  parCC[ 4]  =  r2+(zCC1-zvac4-10.) * TMath::Tan(thetaOpen2);
  parCC[ 5]  =  30.;
  
  parCC[ 6]  =  dlCC;
  parCC[ 7]  =  r2+(zCC2-zvac4-10.) * TMath::Tan(thetaOpen2);
  parCC[ 8]  = 30.;
  gMC->Gsvolu("YCO2", "PCON", idtmed[kSteel], parCC, 9);	  
//  gMC->Gspos("YCO2", 1, "YXO2", 0., 0., dlPb-dlCC-(zvac7-zCC2), 0, "ONLY");  

  zCC1 = 751.75;
  zCC2 = zConeE;
  dlCC=(zCC2-zCC1)/2.;
  parCC[ 3]  = -dlCC;
  parCC[ 4]  =  r2+(zCC1-zvac4-10.) * TMath::Tan(thetaOpen2);
  parCC[ 5]  =  zCC1*TMath::Tan(accMin)-dRSteel2;
  
  parCC[ 6]  =  dlCC;
  parCC[ 7]  =  r2+(zCC2-zvac4-10.) * TMath::Tan(thetaOpen2);
  parCC[ 8]  = 26.;
  
  gMC->Gsvolu("YCO1", "PCON", idtmed[kSteel], parCC, 9);	  
//  gMC->Gspos("YCO1", 1, "YXO2", 0., 0., dlPb-dlCC-(zvac7-zCC2), 0, "ONLY");  
  
//
// W cone 
//
  Float_t parW[15];
  parW[0]  = 0.;
  parW[1]  = 360.;
  parW[2]  = 4.;
  Float_t dlW=(zPb-zvac4)/2.;
  
  parW[3]   = -dlW;
  parW[4]   =  r2;
  parW[5]   =  R21-dRSteel2;
  
  parW[6]   = -dlW+(zvac6-zvac4)+dRSteel2;
  parW[7]   =  r2+(zvac6-zvac4+dRSteel2) * TMath::Tan(thetaOpen2);
  parW[8]   =  R21-dRSteel2;
 
  parW[9]   = -dlW+(zvac6-zvac4)+dRSteel2;
  parW[10]  =  r2+(zvac6-zvac4+dRSteel2) * TMath::Tan(thetaOpen2);
  parW[11]  =  (zvac6+dRSteel2)*TMath::Tan(accMin)-dRSteel2;
 
  parW[12]   = dlW;
  parW[13]  =  r2+(zPb-zvac4) * TMath::Tan(thetaOpen2);
  parW[14]  = zPb*TMath::Tan(accMin)-dRSteel2;

  gMC->Gsvolu("YYO2", "PCON", idtmed[kNiCuW], parW, 15);	  
  gMC->Gspos("YYO2", 1, "YGO2", 0., 0., -(zvac7-zPb)/2., 0, "ONLY");  

  for (i=4; i<35; i+=3) par2[i]  = 0;
          
  gMC->Gsvolu("YMO2", "PCON", idtmed[kVacuum+40], par2, 36);
  gMC->Gspos("YGO2", 1, "YMO2", 0., 0., 0., 0, "ONLY");  
  dZ+=dl;
  gMC->Gspos("YMO2", 1, "YMOT", 0., 0., dZ, 0, "ONLY");  
  dZ+=dl;
//
//
// 2nd section: vacuum system 
//
  cpar0[0]=(zvac7-zvac4)/2;
  cpar0[1]=r2V;
  cpar0[2]=r2V+dVacuS;
  cpar0[3]=cpar0[1]+2.*cpar0[0]*TMath::Tan(thetaOpenB);
  cpar0[4]=cpar0[2]+2.*cpar0[0]*TMath::Tan(thetaOpenB);
  gMC->Gsvolu("YV21", "CONE", idtmed[kSteel+40], cpar0, 5);
//
// insulation
  cpar[0]=cpar0[0];
  cpar[1]=cpar0[1]+dTubeS;
  cpar[2]=cpar0[1]+dTubeS+dInsuS;
  cpar[3]=cpar0[3]+dTubeS;
  cpar[4]=cpar0[3]+dTubeS+dInsuS;
  gMC->Gsvolu("YI21", "CONE", idtmed[kInsulation+40], cpar, 5);
  gMC->Gspos("YI21", 1, "YV21", 0., 0., 0., 0, "ONLY"); 
  gMC->Gspos("YV21", 1, "YMO2", 0., 0., 0., 0, "ONLY"); 

//
// Third Section: Bellows and Flange 
//
  par3[0]  = 0.;
  par3[1]  = 360.;
  par3[2]  = 8.;
  dl=(zvac9-zvac7)/2.;
  
  par3[3]  = -dl;
  par3[4]  = r2+(zvac7-zvac3) * TMath::Tan(thetaOpen2);
  par3[5]  = 30.;

  par3[6]  = -dl+dr21;
  par3[7]  = par3[4]+dr21;
  par3[8]  = 30.;

  par3[9]  = par3[6]+dB2;
  par3[10] = par3[7];
  par3[11] = 30.;

  par3[12] = par3[9]+dr22;
  par3[13] = par3[10]+dr22;
  par3[14] = 30.;

  par3[15] = par3[12]+dF2;
  par3[16] = par3[13];
  par3[17] = 30.;

  par3[18] = par3[15]+dr22;
  par3[19] = par3[16]-dr22;
  par3[20] = 30.;

  par3[21] = par3[18]+dB2;
  par3[22] = par3[19];
  par3[23] = 30.;

  par3[24] = par3[21]+dr23;
  par3[25] = par3[22];
  par3[26] = 30.;
//
  rBox=par3[22]-0.1;
  Float_t r3=par3[25];
  
  gMC->Gsvolu("YGO3", "PCON", idtmed[iHeavy+40], par3, 27);

// begin Fluka
  Float_t rfvacu0[15];
  for (ifl=0; ifl<8; ifl++) {
      zfluka[ifl]=par3[3+3*ifl]+dl+zvac7;
      rfluka1[ifl] = par3[4+3*ifl];
      rfluka2[ifl] = par3[5+3*ifl]-4.; 
      rfluka3[ifl] = par3[5+3*ifl]; 
      rfvacu0[ifl] = 0.;
  }
  for (i=0; i<8; i++) rfluka0[i]=rBox;
  rfluka0[0]=0.; rfluka0[7]=0.;

  flukaGeom->Comment("3rd part: Shield");
  flukaGeom->PolyCone(rfluka1, rfluka2,  zfluka, 8, posfluka, "LEAD", "MF", "$SHS");
  flukaGeom->Comment("3rd part: Steel envelope");
  flukaGeom->PolyCone(rfluka2, rfluka3, zfluka, 8, posfluka, "STEEL", "MF", "$SHS");
  flukaGeom->Comment("3rd part: Vacuum");
  flukaGeom->PolyCone(rfluka0+1, rfluka1+1, zfluka+1, 6, posfluka, "AIR", "MF", "$SHH");

  flukaGeom->Comment("3rd part: Beam Pipe (left)");
  
  rf1[0]=0.; rf2[0]=0.;
  rf1[1] = r3V;
  rf2[1] = rf1[1]+dr21*TMath::Tan(thetaOpenB);
  rf1[2] = rf1[1]+dTubeS; rf1[3]=rf1[2]+dInsuS; rf1[4]=rf1[3]+dEnveS;
  rf1[5] = par3[4];
  rf2[2] = rf2[1]+dTubeS; rf2[3]=rf2[2]+dInsuS; rf2[4]=rf2[3]+dEnveS; 
  rf2[5] = rf1[5]+dr21;
  flukaGeom->OnionCone(rf1, rf2,  6 , zvac7, zvac7+dr21, posfluka, materialsB, fieldsB, cutsB);
  
  
  flukaGeom->Comment("3rd part: Beam Pipe (right)");
  
  rf1[0] = 0.;
  rf1[1] = rf2[1];
  rf1[2] = rf1[1]+dTubeS; rf1[3]=rf1[2]+dInsuS; rf1[4]=rf1[3]+dEnveS;
  rf1[5] = par3[25]; 
  flukaGeom->OnionCylinder(rf1,  6 , zvac9-dr23, zvac9, posfluka, materialsA, fieldsA, cutsA);

//
  flukaGeom->Comment("First Bellow");
  z1=zvac7+dr21;
  
  for (i=0; i<7; i++) {
      z2=z1+eB2;
      flukaGeom->Cylinder(0., rB2, z1, z2, posfluka, "VACUUM", "MF", "$SHH");
      flukaGeom->Cylinder(rB2, rB2+hB2, z1, z2, posfluka, "STEEL", "MF", "$SHH");
      
      z1=z2;
      z2+=lB2/2.-eB2;
      flukaGeom->Cylinder(0., rB2+hB2-eB2, z1, z2, posfluka, "VACUUM", "MF", "$SHH");
      flukaGeom->Cylinder(rB2+hB2-eB2, rB2+hB2, z1, z2, posfluka, "STEEL", "MF", "$SHH");
      z1=z2;
      z2=z1+eB2;
      flukaGeom->Cylinder(0., rB2, z1, z2, posfluka, "VACUUM", "MF", "$SHH");
      flukaGeom->Cylinder(rB2, rB2+hB2, z1, z2, posfluka, "STEEL", "MF", "$SHH");
      z1=z2;
      z2+=lB2/2.-eB2;
      flukaGeom->Cylinder(0., rB2, z1, z2, posfluka, "VACUUM", "MF", "$SHH");
      flukaGeom->Cylinder(rB2, rB2+eB2, z1, z2, posfluka, "STEEL", "MF", "$SHH");
      flukaGeom->Cylinder(rB2+eB2, rB2+hB2, z1, z2, posfluka, "AIR", "MF", "$SHH");
      z1=z2;
   }
  flukaGeom->Cylinder(rB2+hB2, rB2+hB2+0.2, zvac7+dr21, z1, posfluka, "AIR", "MF", "$SHH");
  flukaGeom->Cylinder(rB2+hB2+0.2, rBox, zvac7+dr21, z1, posfluka, "AIR", "MF", "$SHH");
  zcy1=z1;
  

  flukaGeom->Comment("Second Bellow");
  z1=zvac9-dr23;
  for (i=0; i<7; i++) {
      z2=z1-eB2;
      flukaGeom->Cylinder(0., rB2, z2, z1, posfluka, "VACUUM", "MF", "$SHH");
      flukaGeom->Cylinder(rB2, rB2+hB2, z2, z1, posfluka, "STEEL", "MF", "$SHH");
      z1=z2;
      z2-=lB2/2.-eB2;
      flukaGeom->Cylinder(0., rB2+hB2-eB2, z2, z1, posfluka, "VACUUM", "MF", "$SHH");
      flukaGeom->Cylinder(rB2+hB2-eB2, rB2+hB2, z2, z1, posfluka, "STEEL", "MF", "$SHH");
      z1=z2;
      z2=z1-eB2;
      flukaGeom->Cylinder(0., rB2, z2, z1, posfluka, "VACUUM", "MF", "$SHH");
      flukaGeom->Cylinder(rB2, rB2+hB2, z2, z1, posfluka, "STEEL", "MF", "$SHH");
      z1=z2;
      z2-=lB2/2.-eB2;
      flukaGeom->Cylinder(0., rB2, z2, z1, posfluka, "VACUUM", "MF", "$SHH");
      flukaGeom->Cylinder(rB2, rB2+eB2, z2, z1, posfluka, "STEEL", "MF", "$SHH");
      flukaGeom->Cylinder(rB2+eB2, rB2+hB2, z2, z1, posfluka, "AIR", "MF", "$SHH");
      z1=z2;
   }
  flukaGeom->Cylinder(rB2+hB2, rB2+hB2+0.2, z1, zvac9-dr23, posfluka, "AIR", "MF", "$SHH");
  flukaGeom->Cylinder(rB2+hB2+0.2, rBox,    z1, zvac9-dr23, posfluka, "AIR", "MF", "$SHH");

  zcy2=z1;
  flukaGeom->Comment("Flange");
  zfl=(zcy1+zcy2)/2.;

  z1=zfl-dF2/2.;
  z2=zfl+dF2/2.;  
  flukaGeom->Cylinder(0.,rF2-2.,   z1, z2, posfluka, "VACUUM", "MF", "$SHH");
  flukaGeom->Cylinder(rF2-2., rF2, z1, z2, posfluka, "STEEL", "MF", "$SHH");
  flukaGeom->Cylinder(rF2, rF2+0.02     , z1, z2, posfluka, "AIR", "MF", "$SHH");
  flukaGeom->Cylinder(rF2+0.02, rBox    , z1, z2, posfluka, "AIR", "MF", "$SHH");
  z2=z1;
  z1=z2-dFlange;
  flukaGeom->Cylinder(0.,rB2           , z1, z2, posfluka, "VACUUM", "MF", "$SHH");
  flukaGeom->Cylinder(rB2, rF2         , z1, z2, posfluka, "STEEL", "MF", "$SHH");
  flukaGeom->Cylinder(rF2, rF2+0.2     , z1, z2, posfluka, "AIR", "MF", "$SHH");
  flukaGeom->Cylinder(rF2+0.2, rBox    , z1, z2, posfluka, "AIR", "MF", "$SHH");
  z2=z1;
  z1=zcy1;
  flukaGeom->Cylinder(0.,rB2           , z1, z2, posfluka, "VACUUM", "MF", "$SHH");
  flukaGeom->Cylinder(rB2, rB2+0.1     , z1, z2, posfluka, "STEEL", "MF", "$SHH");
  flukaGeom->Cylinder(rB2+0.1, rB2+0.2 , z1, z2, posfluka, "AIR", "MF", "$SHH");
  flukaGeom->Cylinder(rB2+0.2, rBox    , z1, z2, posfluka, "AIR", "MF", "$SHH");

  z1=zfl+dF2/2.;
  z2=z1+dFlange;
  flukaGeom->Cylinder(0.,rB2           , z1, z2, posfluka, "VACUUM", "MF", "$SHH");
  flukaGeom->Cylinder(rB2, rF2         , z1, z2, posfluka, "STEEL", "MF", "$SHH");
  flukaGeom->Cylinder(rF2, rF2+0.2     , z1, z2, posfluka, "AIR", "MF", "$SHH");
  flukaGeom->Cylinder(rF2+0.2, rBox    , z1, z2, posfluka, "AIR", "MF", "$SHH");
  z1=z2;
  z2=zcy2;
  flukaGeom->Cylinder(0.,rB2           , z1, z2, posfluka, "VACUUM", "MF", "$SHH");
  flukaGeom->Cylinder(rB2, rB2+0.1     , z1, z2, posfluka, "STEEL", "MF", "$SHH");
  flukaGeom->Cylinder(rB2+0.1, rB2+0.2 , z1, z2, posfluka, "AIR", "MF", "$SHH");
  flukaGeom->Cylinder(rB2+0.2, rBox    , z1, z2, posfluka, "AIR", "MF", "$SHH");
//
// end Fluka

  for (i=4; i<26; i+=3) par3[i]  = 0;

  gMC->Gsvolu("YMO3", "PCON", idtmed[kVacuum+40], par3, 27);
  gMC->Gspos("YGO3", 1, "YMO3", 0., 0., 0., 0, "ONLY");  

//
// Steel envelope
//  tpar[0]=26;
//  tpar[1]=30;
//  tpar[2]=dl;
//  gMC->Gsvolu("YS31", "TUBE", idtmed[kSteel], tpar, 3);
//  gMC->Gspos("YS31", 1, "YGO3", 0., 0., 0., 0, "ONLY");  
  dZ+=dl;
  gMC->Gspos("YMO3", 1, "YMOT", 0., 0., dZ, 0, "ONLY");  
  dZ+=dl;

//
// 3rd section: vacuum system
//
//
// Bellow2
//
  tpar[0]=rB2;
  tpar[1]=rB2+hB2;
  tpar[2]=eB2/2.;
  gMC->Gsvolu("YB21", "TUBE", idtmed[kSteel+40], tpar, 3);
  dl1=tpar[2];
  
  tpar[0]=rB2+hB2-eB2;
  tpar[1]=rB2+hB2;
  tpar[2]=(lB2/2.-2.*eB2)/2.;
  gMC->Gsvolu("YB22", "TUBE", idtmed[kSteel+40], tpar, 3);
  dl2=tpar[2];

  tpar[0]=rB2-eB2;
  tpar[1]=rB2;
  tpar[2]=lB2/8.;
  gMC->Gsvolu("YB23", "TUBE", idtmed[kSteel+40], tpar, 3);
  dl3=tpar[2];


  tpar[0]=0;
  tpar[1]=rB2+hB2;
  tpar[2]=lB2/2.;
  gMC->Gsvolu("YBU2", "TUBE", idtmed[kVacuum+40], tpar, 3);

  dz=-tpar[2]+dl3;
  gMC->Gspos("YB23", 1, "YBU2", 0., 0., dz, 0, "ONLY"); 
  dz+=dl3;
  dz+=dl1;  
  gMC->Gspos("YB21", 1, "YBU2", 0., 0., dz, 0, "ONLY"); 
  dz+=dl1;  
  dz+=dl2;  
  gMC->Gspos("YB22", 1, "YBU2", 0., 0., dz, 0, "ONLY"); 
  dz+=dl2;  
  dz+=dl1;
  gMC->Gspos("YB21", 2, "YBU2", 0., 0., dz, 0, "ONLY"); 
  dz+=dl1;
  dz+=dl3;
  gMC->Gspos("YB23", 2, "YBU2", 0., 0., dz, 0, "ONLY"); 
  

  tpar[0]=0;
  tpar[1]=rB2+hB2;
  tpar[2]=7.*lB2/2.;
  gMC->Gsvolu("YBM2", "TUBE", idtmed[kVacuum+40], tpar, 3);
  dz=-tpar[2]+lB2/2.;

  for (i=0; i<7; i++) {
    gMC->Gspos("YBU2", i+1 , "YBM2", 0., 0.,dz , 0, "ONLY"); 
    dz+=lB2;
  }

  dz=-dl+dr21+tpar[2];
  gMC->Gspos("YBM2", 1, "YMO3", 0., 0., dz, 0, "ONLY"); 

  dz=dl-dr23-tpar[2];
  gMC->Gspos("YBM2", 2, "YMO3", 0., 0., dz, 0, "ONLY"); 

//
// Flange

  tpar[0]=0;
  tpar[1]=rF2;
  tpar[2]=dF2/2.;
  gMC->Gsvolu("YFM2", "TUBE", idtmed[kVacuum+40], tpar, 3);

  tpar[0]=rF2-2.;
  tpar[1]=rF2;
  tpar[2]=dF2/2.;
  gMC->Gsvolu("YF21", "TUBE", idtmed[kSteel+40], tpar, 3);
  gMC->Gspos("YF21", 1, "YFM2", 0., 0., 0., 0, "ONLY"); 

  tpar[0]=rB2;
  tpar[1]=rF2-2.;
  tpar[2]=dFlange/2.;
  gMC->Gsvolu("YF22", "TUBE", idtmed[kSteel+40], tpar, 3);
  dz=-dF2/2.+tpar[2];
  gMC->Gspos("YF22", 1, "YFM2", 0., 0., dz, 0, "ONLY"); 
  dz= dF2/2.-tpar[2];
  gMC->Gspos("YF22", 2, "YFM2", 0., 0., dz, 0, "ONLY"); 

  dz=dr21/2.-dr23/2.;
  gMC->Gspos("YFM2", 2, "YMO3", 0., 0., dz, 0, "ONLY"); 


//
// pipe between flange and bellows
  tpar[0]=rB2-dTubeS;
  tpar[1]=rB2;
  tpar[2]=2.*(dB2+dr22-7.*lB2)/4.;
  gMC->Gsvolu("YPF2", "TUBE", idtmed[kSteel+40], tpar, 3);
  dz=dr21/2.-dr23/2.-dF2/2.-tpar[2];
  gMC->Gspos("YPF2", 1, "YMO3", 0., 0., dz, 0, "ONLY"); 
  dz=dr21/2.-dr23/2.+dF2/2.+tpar[2];
  gMC->Gspos("YPF2", 2, "YMO3", 0., 0., dz, 0, "ONLY"); 

  Float_t dHorZ=20.;
  
//
// 4th section: rear shield and closing cone
//
  par4[0]  = 0.;
  par4[1]  = 360.;
  par4[2]  = 7.;
  dl=(zvac12-zvac9)/2.;
  
  par4[3]  = -dl;
  par4[4]  = r3;
  par4[5]  = 30.;

  par4[6]  = -dl+dHorZ;
  par4[7]  = r3;
  par4[8]  = 30.;

  par4[9]  = -dl+(zvac10-zvac9);
  par4[10]  = r3+(zvac10-zvac9-dHorZ) * TMath::Tan(thetaOpen3);
  par4[11]  = 30.;

  par4[12]  = par4[9];
  par4[13] = par4[10];
  par4[14] = R42;

  par4[15] = -dl+(zvac11-zvac9);
  par4[16] = r3+(zvac11-zvac9-dHorZ) * TMath::Tan(thetaOpen3);
  par4[17] = R42;

  par4[18] = par4[15];
  par4[19] = par4[16];
  par4[20] = R43;

  par4[21] = -dl+(zvac12-zvac9);
  par4[22] = rVacu+dVacuS;
  par4[23] = R43;

  gMC->Gsvolu("YGO4", "PCON", idtmed[iHeavy+40], par4, 24);


  for (i=4; i<23; i+=3) par4[i]  = 0;

  gMC->Gsvolu("YMO4", "PCON", idtmed[kVacuum+40], par4, 24);
  gMC->Gspos("YGO4", 1, "YMO4", 0., 0., 0., 0, "ONLY");  



  dZ+=dl;
  gMC->Gspos("YMO4", 1, "YMOT", 0., 0., dZ, 0, "ONLY");  
  dZ+=dl;
//
// Concrete replacing Pb
//
  zCC1 = 1316.;
  zCC2 = 1349.;
  
  parCC[ 0]  = 0.;
  parCC[ 1]  = 360.;
  parCC[ 2]  = 2.;
  dlCC=(zCC2-zCC1)/2.;
  parCC[ 3]  = -dlCC;
  parCC[ 4]  = r3+(zCC1-zvac9-dHorZ) * TMath::Tan(thetaOpen3);
  parCC[ 5]  =  30.;
  
  parCC[ 6]  =  dlCC;
  parCC[ 7]  =  r3+(zCC2-zvac9-dHorZ) * TMath::Tan(thetaOpen3);
  parCC[ 8]  = 30.;

  gMC->Gsvolu("YCO4", "PCON", idtmed[kSteel], parCC, 9);	  
//  gMC->Gspos("YCO4", 1, "YGO4", 0., 0., dl-dlCC-(zvac12-zCC2), 0, "ONLY");  

  zCC1 = 1471.;
  zCC2 = 1591.;

  dlCC=(zCC2-zCC1)/2.;
  parCC[ 3]  = -dlCC;
  parCC[ 4]  = r3+(zCC1-zvac9-dHorZ) * TMath::Tan(thetaOpen3);
  parCC[ 5]  = R41-dRSteel2;
  
  parCC[ 6]  =  dlCC;
  parCC[ 7]  =  r3+(zCC2-zvac9-dHorZ) * TMath::Tan(thetaOpen3);
  parCC[ 8]  =  R41-dRSteel2;

  gMC->Gsvolu("YCO5", "PCON", idtmed[kSteel], parCC, 9);	  
//  gMC->Gspos("YCO5", 1, "YGO4", 0., 0., dl-dlCC-(zvac12-zCC2), 0, "ONLY");  

//
// Closing concrete cone 
//
  cpar[0]=(zvac12-zvac11)/2.;
  cpar[1] = r3+(zvac11-zvac9-dHorZ) * TMath::Tan(thetaOpen3);
  cpar[2] = cpar[1]+0.001;
  cpar[3] = rVacu+dVacuS;
  cpar[4] = cpar[2];
  gMC->Gsvolu("YCC4", "CONE", idtmed[kConcrete+40], cpar, 5);
  dz=dl-cpar[0];
  gMC->Gspos("YCC4", 1, "YGO4", 0., 0., dz, 0, "ONLY");  

//
// begin Fluka

  Float_t r10=r3+(zvac10-zvac9-dHorZ) * TMath::Tan(thetaOpen3);
  Float_t r11=cpar[1];

  flukaGeom->Comment("4th part: Shield");
  
  flukaGeom->Cone(r3, r3, 26.0, 26.0, zvac9,  zvac9+dHorZ, 
	      posfluka, "LEAD", "NF", "$SHH");

  flukaGeom->Cone(r3, r10, 26.0, 26.0, zvac9+dHorZ,  zvac10, 
	      posfluka, "LEAD", "NF", "$SHH");
  flukaGeom->Cone(r10, r11, 30.0, 30.0, zvac10,  zvac11, 
	      posfluka, "LEAD", "NF", "$SHH");
  flukaGeom->Cylinder( cpar[1], 30.0, zvac11, zvac12, 
	      posfluka, "LEAD", "NF", "$SHH");

 
  flukaGeom->Comment("4th part: Steel Envelope");
  flukaGeom->Cylinder(26.0, 30., zvac9, zvac10, posfluka, "STEEL", "NF", "$SHH");
  flukaGeom->Comment("4th part: Closing Cone");
  flukaGeom->Cone(cpar[1]-0.1, cpar[3], cpar[1], cpar[1], zvac11, zvac12, 
	      posfluka, "PORTLAND", "NF", "$SHH");
  flukaGeom->Comment("4th part: VACUUM");
  flukaGeom->Cone(0., 0., cpar[1]-0.1, cpar[3], zvac11, zvac12, 
	      posfluka, "VACUUM", "NF", "$SHH");
//
// end Fluka
//
// Steel envelope
//
  dz=-dl;
  tpar[0]=26.;
  tpar[1]=30.;
  tpar[2]=(zvac10-zvac9)/2.;
  gMC->Gsvolu("YS41", "TUBE", idtmed[kSteel], tpar, 3);
  dz+=tpar[2];
//  gMC->Gspos("YS41", 1, "YGO4", 0., 0., dz, 0, "ONLY");  
  dz+=tpar[2];

  tpar[0]=R41-dRSteel2;
  tpar[1]=R41;
  tpar[2]=(zvac11-zvac10)/2.;
  gMC->Gsvolu("YS43", "TUBE", idtmed[kPb], tpar, 3);
  dz+=tpar[2];
  gMC->Gspos("YS43", 1, "YGO4", 0., 0., dz, 0, "ONLY");  
//
// rear lead shield
//
  tpar[0]=R41;
  tpar[1]=R42;
  tpar[2]=(zvac11-zvac10)/2.;
  gMC->Gsvolu("YPBI", "TUBE", idtmed[kPb+40], tpar, 3);
  dz-=0;
  gMC->Gspos("YPBI", 1, "YGO4", 0., 0., dz, 0, "ONLY"); 

  tpar[2]=(zCC2-zCC1)/2.;
  gMC->Gsvolu("YCO6", "TUBE", idtmed[kSteel+40], tpar, 3);
//  gMC->Gspos("YCO6", 1, "YPBI", 0., 0., -(zvac11-zvac10)/2.+tpar[2], 0, "ONLY"); 


  tpar[0]=R42-5;
  tpar[1]=R42;
  tpar[2]=(zvac11-zvac10)/2.;
  gMC->Gsvolu("YPBO", "TUBE", idtmed[kPb], tpar, 3);
  gMC->Gspos("YPBO", 1, "YPBI", 0., 0., 0., 0, "ONLY"); 

  tpar[2]=(zCC2-zCC1)/2.;
  gMC->Gsvolu("YCO7", "TUBE", idtmed[kSteel], tpar, 3);
//  gMC->Gspos("YCO7", 1, "YPBO", 0., 0., -(zvac11-zvac10)/2.+tpar[2], 0, "ONLY"); 
  
//
// rear Fe shield
//

  tpar[0]=31.;
  tpar[1]=R43;
  tpar[2]=(zvac12-zvac11)/2.;
  gMC->Gsvolu("YFEI", "TUBE", idtmed[kFe+40], tpar, 3);
  dz=dl-tpar[2];
  gMC->Gspos("YFEI", 1, "YGO4", 0., 0., dz, 0, "ONLY"); 

  tpar[0]=31.;
  tpar[1]=R43;
  tpar[2]=2.5;
  gMC->Gsvolu("YFEO", "TUBE", idtmed[kFe], tpar, 3);
  dz=-(zvac12-zvac11)/2.+tpar[2];
  gMC->Gspos("YFEO", 1, "YFEI", 0., 0., dz, 0, "ONLY"); 
//
// Magnet element 
//
  tpar[0]=0.;
  tpar[1]=R43;
  tpar[2]=50.;
  gMC->Gsvolu("YAEM", "TUBE", idtmed[kAir], tpar, 3);
  tpar[0]=rAbs;
  tpar[1]=R43;
  tpar[2]=50.;
  gMC->Gsvolu("YFEM", "TUBE", idtmed[kFe], tpar, 3);
  gMC->Gspos("YFEM", 1, "YAEM", 0., 0., 0., 0, "ONLY"); 

//

  dz=zvac12+50.;
  gMC->Gspos("YAEM", 1, "ALIC", 0., 0., dz, 0, "ONLY"); 


// 
//
// 4th section: vacuum system 
//
// up to closing cone
  
  r3V=r3-dr23+dVacuS-1.6;

  cpar0[0]=(zvac11-zvac9)/2;
  cpar0[1]=r3V-dVacuS;
  cpar0[2]=r3V;
  cpar0[3]=cpar0[1]+2.*cpar0[0]*TMath::Tan(thetaOpen3);
  cpar0[4]=cpar0[2]+2.*cpar0[0]*TMath::Tan(thetaOpen3);
  gMC->Gsvolu("YV31", "CONE", idtmed[kSteel+40], cpar0, 5);
//
// insulation
  cpar[0]=cpar0[0];
  cpar[1]=cpar0[1]+dTubeS;
  cpar[2]=cpar0[1]+dTubeS+dInsuS;
  cpar[3]=cpar0[3]+dTubeS;
  cpar[4]=cpar0[3]+dTubeS+dInsuS;
  gMC->Gsvolu("YI31", "CONE", idtmed[kInsulation+40], cpar, 5);
  gMC->Gspos("YI31", 1, "YV31", 0., 0., 0., 0, "ONLY"); 
  dz=-dl+cpar[0];
  gMC->Gspos("YV31", 1, "YMO4", 0., 0., dz, 0, "ONLY"); 

//
// begin Fluka
  flukaGeom->Comment("4th part: Beam pipe lateral structure");
  for (i=0; i<7; i++)  fieldsA[i] = "NF";

  rf1[0]=0.;       rf2[0]=0.;
  rf1[1]=rf2[1]; rf2[1]=rf1[1]+dHorZ*TMath::Tan(thetaOpen3);
  
  rf1[2]=rf1[1]+dTubeS; rf1[3]=rf1[2]+dInsuS; rf1[4]=rf1[3]+dEnveS;
  rf1[5]=r3; 

  rf2[2]=rf2[1]+dTubeS; rf2[3]=rf2[2]+dInsuS; rf2[4]=rf2[3]+dEnveS; 
  rf2[5]=r3;

  flukaGeom->OnionCone(rf1, rf2,  6 , zvac9 , zvac9+dHorZ, posfluka, materialsA, fieldsA, cutsA);

  rf1[0]=0.;       rf2[0]=0.;

  rf1[1]=rf2[1]; rf2[1]=rf1[1]+(zvac10-zvac9-dHorZ)*TMath::Tan(thetaOpen3);
  
  rf1[2]=rf1[1]+dTubeS; rf1[3]=rf1[2]+dInsuS; rf1[4]=rf1[3]+dEnveS;
  rf1[5]=r3; 

  rf2[2]=rf2[1]+dTubeS; rf2[3]=rf2[2]+dInsuS; rf2[4]=rf2[3]+dEnveS; 
  rf2[5]=r10;


  flukaGeom->OnionCone(rf1, rf2,  6 , zvac9+dHorZ, zvac10, posfluka, materialsA, fieldsA, cutsA);

  rf1[0]=0.;       rf2[0]=0.;
  rf1[1]=rf2[1];   rf2[1]=rf1[1]+(zvac11-zvac10)*TMath::Tan(thetaOpen3);

  rf1[2]=rf1[1]+dTubeS; rf1[3]=rf1[2]+dInsuS; rf1[4]=rf1[3]+dEnveS;
  rf1[5]=r10; 
  rf2[2]=rf2[1]+dTubeS; rf2[3]=rf2[2]+dInsuS; rf2[4]=rf2[3]+dEnveS; 
  rf2[5]=r11;

  flukaGeom->OnionCone(rf1, rf2,  6 , zvac10, zvac11, posfluka, materialsA, fieldsA, cutsA);
//  
// end Fluka
//
// closing cone
  cpar0[0]=(zvac12-zvac11)/2;
  cpar0[1]=r3V-dVacuS+(zvac11-zvac9)*TMath::Tan(thetaOpen3);
  cpar0[2]=r3V       +(zvac11-zvac9)*TMath::Tan(thetaOpen3);
  cpar0[3]=rVacu;
  cpar0[4]=rVacu+dTubeS+dInsuS+dProtS+dFreeS;
  gMC->Gsvolu("YV32", "CONE", idtmed[kSteel+40], cpar0, 5);
//
// insulation
  cpar[0]=cpar0[0];
  cpar[1]=cpar0[1]+dTubeS;
  cpar[2]=cpar0[1]+dTubeS+dInsuS;
  cpar[3]=cpar0[3]+dTubeS;
  cpar[4]=cpar0[3]+dTubeS+dInsuS;
  gMC->Gsvolu("YI32", "CONE", idtmed[kInsulation+40], cpar, 5);
  gMC->Gspos("YI32", 1, "YV32", 0., 0., 0., 0, "ONLY"); 
  
  dz=dl-cpar[0];
  gMC->Gspos("YV32", 1, "YMO4", 0., 0., dz, 0, "ONLY"); 
//
//
// MUON trigger wall
//  
  tpar[0] = 50.;
  tpar[1] = 310.;
  tpar[2] = (zFilterOut - zFilterIn) / 4.;
  gMC->Gsvolu("YFIM", "TUBE", idtmed[kFe+40], tpar, 3);
  dz = (zFilterIn + zFilterOut) / 2.;
  tpar[2] -= 10.;
  gMC->Gsvolu("YFII","TUBE", idtmed[kFe], tpar, 3);
  gMC->Gspos("YFII", 1, "YFIM", 0., 0., 0., 0, "ONLY");
  gMC->Gspos("YFIM", 1, "ALIC", 0., 0., dz, 0, "ONLY");
//
// Shielding close to chamber
//
//
  cpar[0]=(zch11-zRear)/2.;
  cpar[1]=R11;
  cpar[2]=zRear*TMath::Tan(accMin);
  cpar[3]=R11;
  cpar[4]=(zRear+2.*cpar[0])*TMath::Tan(accMin);
  gMC->Gsvolu("YCS1", "CONE", idtmed[kNiCuW], cpar, 5);
  dz=-(zvac12-zstart)/2.+(zRear-zstart)+cpar[0];
  gMC->Gspos("YCS1", 1, "YMOT", 0., 0., dz, 0, "ONLY");

  cpar[0]=(zvac4-zch12)/2.;
  cpar[1]=R11;
  cpar[2]=zch12*TMath::Tan(accMin);
  cpar[3]=R11;
  cpar[4]=(zch12+2.*cpar[0])*TMath::Tan(accMin);
  gMC->Gsvolu("YCS3", "CONE", idtmed[kNiCuW], cpar, 5);
  dz=-(zvac12-zstart)/2.+(zch12-zstart)+cpar[0];
  gMC->Gspos("YCS3", 1, "YMOT", 0., 0., dz, 0, "ONLY");


// Recess station 1

  cpar[0]=(zch12-zch11)/2.;
  cpar[1]=R11;
  cpar[2]=18.;
  cpar[3]=R11;
  cpar[4]=17.9;
  gMC->Gsvolu("YCS2", "CONE", idtmed[kAir], cpar, 5);
  dz=-(zvac12-zstart)/2.+(zch11-zstart)+cpar[0];
  gMC->Gspos("YCS2", 1, "YMOT", 0., 0., dz, 0, "ONLY");

  Float_t ptubs[5];
  ptubs[0] = R11;
  ptubs[1] = 17.9;
  ptubs[2] =   0.;
// phi_min, phi_max
  ptubs[3] =   0.;
  ptubs[4] =  90.;  
  gMC->Gsvolu("YCR0", "TUBS", idtmed[kNiCuW], ptubs, 0);
  Int_t idrotm[1799];
  
  AliMatrix(idrotm[1701],90.,   0., 90.,  90., 0., 0.);
  AliMatrix(idrotm[1702],90.,  90., 90., 180., 0., 0.);
  AliMatrix(idrotm[1703],90., 180., 90., 270., 0., 0.); 
  AliMatrix(idrotm[1704],90., 270., 90.,   0., 0., 0.); 
  //  Int_t ipos;
  
  dz=-cpar[0];
// 1.
  ptubs[2]=6.5/2.;
  dz+=ptubs[2];
  gMC->Gsposp("YCR0", 1, "YCS2", 0., 0., dz, idrotm[1701], "ONLY", ptubs, 5);
  gMC->Gsposp("YCR0", 2, "YCS2", 0., 0., dz, idrotm[1703], "ONLY", ptubs, 5);
  dz+=ptubs[2];
  dz+=1.5;
// 2.
  ptubs[2]=5.0/2.;
  dz+=ptubs[2];
  gMC->Gsposp("YCR0", 3, "YCS2", 0., 0., dz, idrotm[1702], "ONLY", ptubs, 5);
  gMC->Gsposp("YCR0", 4, "YCS2", 0., 0., dz, idrotm[1704], "ONLY", ptubs, 5);
  dz+=ptubs[2];
  dz+=1.5;
// 3. 
  ptubs[2]=5.0/2.;
  dz+=ptubs[2];
  gMC->Gsposp("YCR0", 5, "YCS2", 0., 0., dz, idrotm[1701], "ONLY", ptubs, 5);
  gMC->Gsposp("YCR0", 6, "YCS2", 0., 0., dz, idrotm[1703], "ONLY", ptubs, 5);
  dz+=ptubs[2];
  dz+=1.5;
// 4. 
  ptubs[2]=6.5/2.;
  dz+=ptubs[2];
  gMC->Gsposp("YCR0", 7, "YCS2", 0., 0., dz, idrotm[1702], "ONLY", ptubs, 5);
  gMC->Gsposp("YCR0", 8, "YCS2", 0., 0., dz, idrotm[1704], "ONLY", ptubs, 5);
  dz+=ptubs[2];
  dz+=1.5;


  
  cpar[0]=(zch21-zvac4)/2.;
  cpar[1]=R21;
  cpar[2]=zvac4*TMath::Tan(accMin);
  cpar[3]=R21;
  cpar[4]=(zvac4+2.*cpar[0])*TMath::Tan(accMin);
  gMC->Gsvolu("YCS4", "CONE", idtmed[kNiCuW], cpar, 5);
  dz=-(zvac12-zstart)/2.+(zvac4-zstart)+cpar[0];
  gMC->Gspos("YCS4", 1, "YMOT", 0., 0., dz, 0, "ONLY");

  cpar[0]=(zvac6-zch22)/2.;
  cpar[1]=R21;
  cpar[2]=zch22*TMath::Tan(accMin);
  cpar[3]=R21;
  cpar[4]=(zch22+2.*cpar[0])*TMath::Tan(accMin);
  gMC->Gsvolu("YCS6", "CONE", idtmed[kNiCuW], cpar, 5);
  dz=-(zvac12-zstart)/2.+(zch22-zstart)+cpar[0];
  gMC->Gspos("YCS6", 1, "YMOT", 0., 0., dz, 0, "ONLY");
  
// Recess station 2
 
  cpar[0]=(zch22-zch21)/2.;
  cpar[1]=R21;
  cpar[2]=23.;
  cpar[3]=R21;
  cpar[4]=23.;
  gMC->Gsvolu("YCS5", "CONE", idtmed[kAir], cpar, 5);
  dz=-(zvac12-zstart)/2.+(zch21-zstart)+cpar[0];
  gMC->Gspos("YCS5", 1, "YMOT", 0., 0., dz, 0, "ONLY");

  ptubs[0] = R21;
  ptubs[1] = 23;
  ptubs[2] =   0.;
  ptubs[3] =   0.;
  ptubs[4] =  90.;  
  gMC->Gsvolu("YCR1", "TUBS", idtmed[kNiCuW], ptubs, 0);

  dz=-cpar[0];
// 1.
  ptubs[2]=7.5/2.;
  dz+=ptubs[2];
  gMC->Gsposp("YCR1", 1, "YCS5", 0., 0., dz, idrotm[1701], "ONLY", ptubs, 5);
  gMC->Gsposp("YCR1", 2, "YCS5", 0., 0., dz, idrotm[1703], "ONLY", ptubs, 5);
  dz+=ptubs[2];
  dz+=1.5;
// 2.
  ptubs[2]=6.0/2.;
  dz+=ptubs[2];
  gMC->Gsposp("YCR1", 3, "YCS5", 0., 0., dz, idrotm[1702], "ONLY", ptubs, 5);
  gMC->Gsposp("YCR1", 4, "YCS5", 0., 0., dz, idrotm[1704], "ONLY", ptubs, 5);
  dz+=ptubs[2];
  dz+=1.5;
// 3. 
  ptubs[2]=6.0/2.;
  dz+=ptubs[2];
  gMC->Gsposp("YCR1", 5, "YCS5", 0., 0., dz, idrotm[1701], "ONLY", ptubs, 5);
  gMC->Gsposp("YCR1", 6, "YCS5", 0., 0., dz, idrotm[1703], "ONLY", ptubs, 5);
  dz+=ptubs[2];
  dz+=1.5;
// 4. 
  ptubs[2]=7.5/2.;
  dz+=ptubs[2];
  gMC->Gsposp("YCR1", 7, "YCS5", 0., 0., dz, idrotm[1702], "ONLY", ptubs, 5);
  gMC->Gsposp("YCR1", 8, "YCS5", 0., 0., dz, idrotm[1704], "ONLY", ptubs, 5);
  dz+=ptubs[2];
  dz+=1.5;


//
// begin Fluka
/*
  flukaGeom->Cone(R11, R11, -1.,  -1., 
		  zRear, zch11, posfluka,"NIW", "MF", "$SHS");

  flukaGeom->Cone(R11, R11, -1.,  -1., 
		  zch11, zch12, posfluka,"AIR", "MF", "$SHS");

  flukaGeom->Cone(R11, R11, -1.,  -1., 
		  zch12, zvac4, posfluka,"NIW", "MF", "$SHS");

  flukaGeom->Cone(R21, R21, -1.,  -1., 
	          zvac4, zch21, posfluka,"NIW", "MF", "$SHS");
  flukaGeom->Cone(R21, R21, -1.,  -1., 
	          zch21, zch22, posfluka,"AIR", "MF", "$SHS");
  flukaGeom->Cone(R21, R21, -1.,  -1., 
	          zch22, zvac6, posfluka,"NIW", "MF", "$SHS");

*/
  if (fWriteGeometry) flukaGeom->Finish();

// 
// end Fluka
//
// Outer Pb Cone

  if (fPbCone) {
      dl = (zvac10-zch32)/2.;
      dz = dl+zch32;
      
      par0[0]  = 0.;
      par0[1]  = 360.;
      par0[2]  = 10.;

      par0[ 3]  = -dl;
      par0[ 4]  = 30.;
      par0[ 5]  = 30.+(zch32-zConeE)*TMath::Tan(thetaOpenPbO);

//    4th station
      par0[ 6]  = -dz + zch41;
      par0[ 7]  = 30.;
      par0[ 8]  = 30.+(zch41-zConeE)*TMath::Tan(thetaOpenPbO);

      par0[ 9]  = -dz + zch41;
      par0[10]  = 30.;
      par0[11]  = 37.5;  
                                          // recess erice2000
      par0[12]  = -dz + zch42;
      par0[13]  = 30.;
      par0[14]  = par0[11];

      par0[15]  = -dz + zch42;
      par0[16]  = 30.;
      par0[17]  = 30.+(zch42-zConeE)*TMath::Tan(thetaOpenPbO);

//    5th station
      par0[18]  = -dz + zch51;
      par0[19]  = 30.;
      par0[20]  = 30.+(zch51-zConeE)*TMath::Tan(thetaOpenPbO);

      par0[21]  = -dz + zch51;
      par0[22]  = 30.;
      par0[23]  = 37.5;  // recess erice2000

      par0[24]  = -dz + zch52;
      par0[25]  = 30.;
      par0[26]  = par0[23];

      par0[27]  = -dz + zch52;
      par0[28]  = 30.;
      par0[29]  = 30.+(zch52-zConeE)*TMath::Tan(thetaOpenPbO);
// end of cone
      par0[30]  = +dl;
      par0[31]  = 30.;
      par0[32]  = par0[29];
//
      gMC->Gsvolu("YOPB", "PCON", idtmed[kPb], par0, 33);
      Float_t dzs = -(zvac12-zstart)/2. + (zch32-zstart) + dl;
      gMC->Gspos("YOPB", 1, "YMOT", 0., 0., dzs, 0, "ONLY");

//
// Steel envelope
//
      par0[ 0]  = 0.;
      par0[ 1]  = 360.;
      par0[ 2]  = 18.;
  
      par0[ 3]  = -dl;
      par0[ 5]  = 30.+(zch32-zConeE)*TMath::Tan(thetaOpenPbO);
      par0[ 4]  = par0[ 5] - 4.;
//    4th station
      par0[ 6]  = -dz + zch41 - 4.;
      par0[ 8]  = 30.+(zch41-4.-zConeE)*TMath::Tan(thetaOpenPbO);
      par0[ 7]  = par0[ 8] -4.;

      par0[ 9]  = -dz + zch41 - 4.;
      par0[11]  = par0[8];  
      par0[10]  = 33.5;

      par0[12]  = -dz + zch41;
      par0[14]  = 30.+(zch41-zConeE)*TMath::Tan(thetaOpenPbO);  
      par0[13]  = 33.5;

      par0[15]  = -dz + zch41;
      par0[17]  = 37.5;  
      par0[16]  = 33.5;
                      
      par0[18]  = -dz + zch42;
      par0[20]  = 37.5;
      par0[19]  = 33.5;

      par0[21]  = -dz + zch42;
      par0[23]  = 30.+(zch42-zConeE)*TMath::Tan(thetaOpenPbO);  
      par0[22]  = 33.5;

      par0[24]  = -dz + zch42 + 4.;
      par0[26]  = 30.+(zch42+4.-zConeE)*TMath::Tan(thetaOpenPbO);
      par0[25]  = 33.5;

      par0[27]  = -dz + zch42 + 4.;
      par0[29]  = 30.+(zch42+4.-zConeE)*TMath::Tan(thetaOpenPbO);
      par0[28]  = par0[29] - 4.;

//    5th station
      par0[30]  = -dz + zch51 - 4.;
      par0[32]  = 30.+(zch51-4.-zConeE)*TMath::Tan(thetaOpenPbO);
      par0[31]  = par0[32] - 4.;
      
      par0[33]  = -dz + zch51 - 4.;
      par0[35]  = par0[32];
      par0[34]  = 33.5;

      par0[36]  = -dz + zch51;
      par0[38]  = 30.+(zch51-zConeE)*TMath::Tan(thetaOpenPbO);
      par0[37]  = 33.5;

      par0[39]  = -dz + zch51;
      par0[41]  = 37.5;
      par0[40]  = 33.5;

      par0[42]  = -dz + zch52;
      par0[44]  = 37.5;
      par0[43]  = 33.5;

      par0[45]  = -dz + zch52;
      par0[47]  = 30.+(zch52-zConeE)*TMath::Tan(thetaOpenPbO);
      par0[46]  = 33.5;

      par0[48]  = -dz + zch52 + 4.;
      par0[50]  = 30.+(zch52+4.-zConeE)*TMath::Tan(thetaOpenPbO);
      par0[49]  = 33.5;

      par0[51]  = -dz + zch52 + 4.;
      par0[53]  = 30.+(zch52+4.-zConeE)*TMath::Tan(thetaOpenPbO);
      par0[52]  = par0[53] - 4.;

      par0[54]  = +dl;
      par0[56]  = par0[53];
      par0[55]  = par0[52];

      gMC->Gsvolu("YOSE",    "PCON", idtmed[kSteel], par0, 57);
      gMC->Gspos ("YOSE", 1, "YOPB", 0., 0., 0., 0, "ONLY");
//
//    Concrete replacing lead
//
      zCC1 = 1066.;
      zCC2 = 1188.;
      dlCC=(zCC2-zCC1)/2.;
      parCC[ 3]  = -dlCC;
      parCC[ 4]  =  30.;
      parCC[ 5]  =  30.+(zCC1-zConeE)*TMath::Tan(thetaOpenPbO)-4.;
      
      parCC[ 6]  =  dlCC;
      parCC[ 7]  =  30.;
      parCC[ 8]  =  30.+(zCC2-zConeE)*TMath::Tan(thetaOpenPbO)-4.;
      gMC->Gsvolu("YOC1", "PCON", idtmed[kSteel], parCC, 9);	  
//      gMC->Gspos("YOC1", 1, "YOPB", 0., 0., dl-dlCC-(zvac10-zCC2), 0, "ONLY");  

      zCC1 = 1316.;
      zCC2 = 1349.;
      dlCC=(zCC2-zCC1)/2.;
      parCC[ 3]  = -dlCC;
      parCC[ 4]  =  30.;
      parCC[ 5]  =  30.+(zCC1-zConeE)*TMath::Tan(thetaOpenPbO)-4.;
      
      parCC[ 6]  =  dlCC;
      parCC[ 7]  =  30.;
      parCC[ 8]  =  30.+(zCC2-zConeE)*TMath::Tan(thetaOpenPbO)-4.;
      gMC->Gsvolu("YOC2", "PCON", idtmed[kSteel], parCC, 9);	  
//      gMC->Gspos("YOC2", 1, "YOPB", 0., 0., dl-dlCC-(zvac10-zCC2), 0, "ONLY");  
  }
}

void AliSHILv2::Init()
{
  //
  // Initialise the muon shield after it has been built
  //
  Int_t i;
  //
  if(fDebug) {
    printf("\n%s: ",ClassName());
    for(i=0;i<35;i++) printf("*");
    printf(" SHILv2_INIT ");
    for(i=0;i<35;i++) printf("*");
    printf("\n%s: ",ClassName());
    //
    // Here the SHIL initialisation code (if any!)
    for(i=0;i<80;i++) printf("*");
    printf("\n");
  }
}

/***************************************************************************
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

//
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  Photon Multiplicity Detector Version 1                                   //
//                                                                           //
//Begin_Html
/*
<img src="picts/AliPMDv0Class.gif">
*/
//End_Html
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
////

#include <Riostream.h>
#include <TGeoManager.h>
#include <TGeoGlobalMagField.h>
#include <TVirtualMC.h>

#include "AliConst.h" 
#include "AliMagF.h" 
#include "AliPMDv0.h"
#include "AliRun.h"
#include "AliMC.h"
#include "AliLog.h"

const Int_t   AliPMDv0::fgkNcellHole  = 24;       // Hole dimension
const Float_t AliPMDv0::fgkCellRadius = 0.25;     // Radius of a hexagonal cell
const Float_t AliPMDv0::fgkCellWall   = 0.02;     // Thickness of cell Wall
const Float_t AliPMDv0::fgkCellDepth  = 0.50;     // Gas thickness
const Float_t AliPMDv0::fgkBoundary   = 0.7;      // Thickness of Boundary wall
const Float_t AliPMDv0::fgkThBase     = 0.3;      // Thickness of Base plate
const Float_t AliPMDv0::fgkThAir      = 0.1;      // Thickness of Air
const Float_t AliPMDv0::fgkThPCB      = 0.16;     // Thickness of PCB
const Float_t AliPMDv0::fgkThLead     = 1.5;      // Thickness of Pb
const Float_t AliPMDv0::fgkThSteel    = 0.5;      // Thickness of Steel
const Float_t AliPMDv0::fgkZdist      = 361.5;    // z-position of the detector
const Float_t AliPMDv0::fgkSqroot3    = 1.7320508;// Square Root of 3
const Float_t AliPMDv0::fgkSqroot3by2 = 0.8660254;// Square Root of 3 by 2
const Float_t AliPMDv0::fgkPi         = 3.14159;  // pi

ClassImp(AliPMDv0)
 
//_____________________________________________________________________________
AliPMDv0::AliPMDv0():
  fSMthick(0.),
  fSMLength(0.),
  fMedSens(0),
  fNcellSM(0)
{
  //
  // Default constructor 
  //
}
 
//_____________________________________________________________________________
AliPMDv0::AliPMDv0(const char *name, const char *title):
  AliPMD(name,title),
  fSMthick(0.),
  fSMLength(0.),
  fMedSens(0),
  fNcellSM(0)
{
  //
  // Standard constructor
  //
}

//_____________________________________________________________________________
void AliPMDv0::CreateGeometry()
{
  //
  // Create geometry for Photon Multiplicity Detector Version 3 :
  // April 2, 2001
  //
  //Begin_Html
  /*
    <img src="picts/AliPMDv0.gif">
  */
  //End_Html
  //Begin_Html
  /*
    <img src="picts/AliPMDv0Tree.gif">
  */
  //End_Html
  GetParameters();
  CreateSupermodule();
  CreatePMD();
}

//_____________________________________________________________________________
void AliPMDv0::CreateSupermodule()
{
  //
  // Creates the geometry of the cells, places them in  supermodule which
  // is a rhombus object.

  // *** DEFINITION OF THE GEOMETRY OF THE PMD  *** 
  // *** HEXAGONAL CELLS WITH CELL RADIUS 0.25 cm (see "GetParameters")
  // -- Author :     S. Chattopadhyay, 02/04/1999. 

  // Basic unit is ECAR, a hexagonal cell made of Ar+CO2, which is placed inside another 
  // hexagonal cell made of Cu (ECCU) with larger radius, compared to ECAR. The difference
  // in radius gives the dimension of half width of each cell wall.
  // These cells are placed as 72 x 72 array in a 
  // rhombus shaped supermodule (EHC1). The rhombus shaped modules are designed
  // to have closed packed structure.
  //
  // Each supermodule (ESMA, ESMB), made of G10 is filled with following components
  //  EAIR --> Air gap between gas hexagonal cells and G10 backing.
  //  EHC1 --> Rhombus shaped parallelopiped containing the hexagonal cells
  //  EAIR --> Air gap between gas hexagonal cells and G10 backing.
  //
  // ESMA, ESMB are placed in EMM1 along with EMPB (Pb converter) 
  // and EMFE (iron support) 

  // EMM1 made of
  //    ESMB --> Normal supermodule, mirror image of ESMA
  //    EMPB --> Pb converter
  //    EMFE --> Fe backing
  //    ESMA --> Normal supermodule
  //
  // ESMX, ESMY are placed in EMM2 along with EMPB (Pb converter) 
  // and EMFE (iron support) 

  // EMM2 made of 
  //    ESMY --> Special supermodule, mirror image of ESMX, 
  //    EMPB --> Pb converter
  //    EMFE --> Fe backing
  //    ESMX --> First of the two Special supermodules near the hole

 // EMM3 made of
  //    ESMQ --> Special supermodule, mirror image of ESMX, 
  //    EMPB --> Pb converter
  //    EMFE --> Fe backing
  //    ESMP --> Second of the two Special supermodules near the hole
  
  // EMM2 and EMM3 are used to create the hexagonal  HOLE

  //
  //  		                     EPMD
  //  				       |             
  //   				       |
  //   ---------------------------------------------------------------------------
  //   |              |                       |                     |            |
  //  EHOL           EMM1                    EMM2                  EMM3         EALM
  //                  |                       |                     |
  //      --------------------   --------------------      -------------------- 
  //      |    |      |     |    |     |      |     |      |     |      |     | 
  //     ESMB  EMPB  EMFE ESMA  ESMY  EMPB  EMFE  ESMX    ESMQ  EMPB  EMFE  ESMP
  //      |                      |		           |                 
  //   ------------          ------------	      -------------           
  //  |     |     |         |     |     |	      |     |     |           
  // EAIR EHC1   EAIR      EAIR  EHC2  EAIR	     EAIR  EHC3  EAIR          
  //        |                     |		            |                  
  //      ECCU                   ECCU		           ECCU                 
  //       |                      |			    |                  
  //      ECAR                   ECAR                      ECAR                 
  

  Int_t i, j;
  Float_t xb, yb, zb;
  Int_t number;
  Int_t ihrotm,irotdm;
  Int_t *idtmed = fIdtmed->GetArray()-599;
 
  AliMatrix(ihrotm, 90., 30.,   90.,  120., 0., 0.);
  AliMatrix(irotdm, 90., 180.,  90.,  270., 180., 0.);
 
  //Subhasis, dimensional parameters of rhombus (dpara) as given to gsvolu
  // rhombus to accomodate 72 x 72 hexagons, and with total 1.2cm extension  
  //(1mm tolerance on both side and 5mm thick G10 wall)
  // 
  // **** CELL SIZE 20 mm^2 EQUIVALENT
  // Inner hexagon filled with gas (Ar+CO2)

  Float_t hexd2[10] = {0.,360.,6,2,-0.25,0.,0.23,0.25,0.,0.23};

  hexd2[4]=  -fgkCellDepth/2.;
  hexd2[7]=   fgkCellDepth/2.;
  hexd2[6]=   fgkCellRadius - fgkCellWall;
  hexd2[9]=   fgkCellRadius - fgkCellWall;
  
  // Gas replaced by vacuum for v0(insensitive) version of PMD.

  TVirtualMC::GetMC()->Gsvolu("ECAR", "PGON", idtmed[697], hexd2,10);
  gGeoManager->SetVolumeAttribute("ECAR", "SEEN", 0);
  
  // Outer hexagon made of Copper
  
  Float_t hexd1[10] = {0.,360.,6,2,-0.25,0.,0.25,0.25,0.,0.25};

  hexd1[4]=  -fgkCellDepth/2.;
  hexd1[7]=   fgkCellDepth/2.;
  hexd1[6]=   fgkCellRadius;
  hexd1[9]=   fgkCellRadius;

  TVirtualMC::GetMC()->Gsvolu("ECCU", "PGON", idtmed[614], hexd1,10);
  gGeoManager->SetVolumeAttribute("ECCU", "SEEN", 1);

  // --- place  inner hex inside outer hex 

  TVirtualMC::GetMC()->Gspos("ECAR", 1, "ECCU", 0., 0., 0., 0, "ONLY");

  // Rhombus shaped supermodules (defined by PARA) 
  
  // volume for SUPERMODULE 
   
  Float_t dparasm1[6] = {12.5,12.5,0.8,30.,0.,0.};
  dparasm1[0] = (fNcellSM+0.25)*hexd1[6] ;
  dparasm1[1] = dparasm1[0] *fgkSqroot3by2;
  dparasm1[2] = fSMthick/2.;
  
  //
  TVirtualMC::GetMC()->Gsvolu("ESMA","PARA", idtmed[607], dparasm1, 6);
  gGeoManager->SetVolumeAttribute("ESMA", "SEEN", 0);
  //
  TVirtualMC::GetMC()->Gsvolu("ESMB","PARA", idtmed[607], dparasm1, 6);
  gGeoManager->SetVolumeAttribute("ESMB", "SEEN", 0);
  
  // Air residing between the PCB and the base
  
  Float_t dparaair[6] = {12.5,12.5,8.,30.,0.,0.};
  dparaair[0]= dparasm1[0];
  dparaair[1]= dparasm1[1];
  dparaair[2]= fgkThAir/2.;
  
  TVirtualMC::GetMC()->Gsvolu("EAIR","PARA", idtmed[698], dparaair, 6);
  gGeoManager->SetVolumeAttribute("EAIR", "SEEN", 0);
  
  // volume for honeycomb chamber EHC1 
  
  Float_t dpara1[6] = {12.5,12.5,0.4,30.,0.,0.};
  dpara1[0] = dparasm1[0];
  dpara1[1] = dparasm1[1];
  dpara1[2] = fgkCellDepth/2.;

  TVirtualMC::GetMC()->Gsvolu("EHC1","PARA", idtmed[698], dpara1, 6);
  gGeoManager->SetVolumeAttribute("EHC1", "SEEN", 1);
  
  // Place hexagonal cells ECCU cells  inside EHC1 (72 X 72)

  Int_t xrow = 1;

  yb = -dpara1[1] + (1./fgkSqroot3by2)*hexd1[6];
  zb = 0.;

  for (j = 1; j <= fNcellSM; ++j) {
    xb =-(dpara1[0] + dpara1[1]*0.577) + 2*hexd1[6]; //0.577=tan(30deg)
    if(xrow >= 2){
      xb = xb+(xrow-1)*hexd1[6];
    }
    for (i = 1; i <= fNcellSM; ++i) {
      number = i+(j-1)*fNcellSM;
      TVirtualMC::GetMC()->Gspos("ECCU", number, "EHC1", xb,yb,zb, ihrotm, "ONLY");
      xb += (hexd1[6]*2.);
    }
    xrow = xrow+1;
    yb += (hexd1[6]*fgkSqroot3);
  }


  // Place EHC1 and EAIR into  ESMA and ESMB

  Float_t zAir1,zAir2,zGas; 

  //ESMA is normal supermodule with base at bottom, with EHC1
  zAir1= -dparasm1[2] + fgkThBase + dparaair[2]; 
  TVirtualMC::GetMC()->Gspos("EAIR", 1, "ESMA", 0., 0., zAir1, 0, "ONLY");
  zGas=zAir1+dparaair[2]+ fgkThPCB + dpara1[2]; 
  //Line below Commented for version 0 of PMD routine
  //  TVirtualMC::GetMC()->Gspos("EHC1", 1, "ESMA", 0., 0., zGas, 0, "ONLY");
  zAir2=zGas+dpara1[2]+ fgkThPCB + dparaair[2]; 
  TVirtualMC::GetMC()->Gspos("EAIR", 2, "ESMA", 0., 0., zAir2, 0, "ONLY");

  // ESMB is mirror image of ESMA, with base at top, with EHC1

  zAir1= -dparasm1[2] + fgkThPCB + dparaair[2]; 
  TVirtualMC::GetMC()->Gspos("EAIR", 3, "ESMB", 0., 0., zAir1, 0, "ONLY");
  zGas=zAir1+dparaair[2]+ fgkThPCB + dpara1[2]; 
  //Line below Commented for version 0 of PMD routine
  //  TVirtualMC::GetMC()->Gspos("EHC1", 2, "ESMB", 0., 0., zGas, 0, "ONLY");
  zAir2=zGas+dpara1[2]+ fgkThPCB + dparaair[2]; 
  TVirtualMC::GetMC()->Gspos("EAIR", 4, "ESMB", 0., 0., zAir2, 0, "ONLY");


  // special supermodule EMM2(GEANT only) containing 6 unit modules
  // volume for SUPERMODULE 

  Float_t dparasm2[6] = {12.5,12.5,0.8,30.,0.,0.};
  dparasm2[0]=(fNcellSM+0.25)*hexd1[6] ;
  dparasm2[1] = (fNcellSM - fgkNcellHole + 0.25) * fgkSqroot3by2 * hexd1[6];
  dparasm2[2] = fSMthick/2.;

  TVirtualMC::GetMC()->Gsvolu("ESMX","PARA", idtmed[607], dparasm2, 6);
  gGeoManager->SetVolumeAttribute("ESMX", "SEEN", 0);
  //
  TVirtualMC::GetMC()->Gsvolu("ESMY","PARA", idtmed[607], dparasm2, 6);
  gGeoManager->SetVolumeAttribute("ESMY", "SEEN", 0);

  Float_t dpara2[6] = {12.5,12.5,0.4,30.,0.,0.};
  dpara2[0] = dparasm2[0];
  dpara2[1] = dparasm2[1];
  dpara2[2] = fgkCellDepth/2.;

  TVirtualMC::GetMC()->Gsvolu("EHC2","PARA", idtmed[698], dpara2, 6);
  gGeoManager->SetVolumeAttribute("EHC2", "SEEN", 1);


  // Air residing between the PCB and the base

  Float_t dpara2Air[6] = {12.5,12.5,8.,30.,0.,0.};
  dpara2Air[0]= dparasm2[0];
  dpara2Air[1]= dparasm2[1];
  dpara2Air[2]= fgkThAir/2.;

  TVirtualMC::GetMC()->Gsvolu("EAIX","PARA", idtmed[698], dpara2Air, 6);
  gGeoManager->SetVolumeAttribute("EAIX", "SEEN", 0);

  // Place hexagonal single cells ECCU inside EHC2
  // skip cells which go into the hole in top left corner.

  xrow=1;
  yb = -dpara2[1] + (1./fgkSqroot3by2)*hexd1[6];
  zb = 0.;
  for (j = 1; j <= (fNcellSM - fgkNcellHole); ++j) {
    xb =-(dpara2[0] + dpara2[1]*0.577) + 2*hexd1[6];
    if(xrow >= 2){
      xb = xb+(xrow-1)*hexd1[6];
    }
    for (i = 1; i <= fNcellSM; ++i) {
      number = i+(j-1)*fNcellSM;
 	    TVirtualMC::GetMC()->Gspos("ECCU", number, "EHC2", xb,yb,zb, ihrotm, "ONLY");
      xb += (hexd1[6]*2.);
    }
    xrow = xrow+1;
    yb += (hexd1[6]*fgkSqroot3);
  }


  // ESMX is normal supermodule with base at bottom, with EHC2
  
  zAir1= -dparasm2[2] + fgkThBase + dpara2Air[2]; 
  TVirtualMC::GetMC()->Gspos("EAIX", 1, "ESMX", 0., 0., zAir1, 0, "ONLY");
  zGas=zAir1+dpara2Air[2]+ fgkThPCB + dpara2[2]; 
  //Line below Commented for version 0 of PMD routine
  //  TVirtualMC::GetMC()->Gspos("EHC2", 1, "ESMX", 0., 0., zGas, 0, "ONLY");
  zAir2=zGas+dpara2[2]+ fgkThPCB + dpara2Air[2]; 
  TVirtualMC::GetMC()->Gspos("EAIX", 2, "ESMX", 0., 0., zAir2, 0, "ONLY");

  // ESMY is mirror image of ESMX with base at bottom, with EHC2
  
  zAir1= -dparasm2[2] + fgkThPCB + dpara2Air[2]; 
  TVirtualMC::GetMC()->Gspos("EAIX", 3, "ESMY", 0., 0., zAir1, 0, "ONLY");
  zGas=zAir1+dpara2Air[2]+ fgkThPCB + dpara2[2]; 
  //Line below Commented for version 0 of PMD routine
  //  TVirtualMC::GetMC()->Gspos("EHC2", 2, "ESMY", 0., 0., zGas, 0, "ONLY");
  zAir2=zGas+dpara2[2]+ fgkThPCB + dpara2Air[2]; 
  TVirtualMC::GetMC()->Gspos("EAIX", 4, "ESMY", 0., 0., zAir2, 0, "ONLY");

  //
  // special supermodule EMM3 (GEANT only) containing 2 unit modules
  // volume for SUPERMODULE 
  //
  Float_t dparaSM3[6] = {12.5,12.5,0.8,30.,0.,0.};
  dparaSM3[0]=(fNcellSM - fgkNcellHole +0.25)*hexd1[6] ;
  dparaSM3[1] = (fgkNcellHole + 0.25) * hexd1[6] * fgkSqroot3by2;
  dparaSM3[2] = fSMthick/2.;

  TVirtualMC::GetMC()->Gsvolu("ESMP","PARA", idtmed[607], dparaSM3, 6);
  gGeoManager->SetVolumeAttribute("ESMP", "SEEN", 0);
  //
  TVirtualMC::GetMC()->Gsvolu("ESMQ","PARA", idtmed[607], dparaSM3, 6);
  gGeoManager->SetVolumeAttribute("ESMQ", "SEEN", 0);

  Float_t dpara3[6] = {12.5,12.5,0.4,30.,0.,0.};
  dpara3[0] = dparaSM3[0];
  dpara3[1] = dparaSM3[1];
  dpara3[2] = fgkCellDepth/2.;

  TVirtualMC::GetMC()->Gsvolu("EHC3","PARA", idtmed[698], dpara3, 6);
  gGeoManager->SetVolumeAttribute("EHC3", "SEEN", 1);

  // Air residing between the PCB and the base

  Float_t dpara3Air[6] = {12.5,12.5,8.,30.,0.,0.};
  dpara3Air[0]= dparaSM3[0];
  dpara3Air[1]= dparaSM3[1];
  dpara3Air[2]= fgkThAir/2.;

  TVirtualMC::GetMC()->Gsvolu("EAIP","PARA", idtmed[698], dpara3Air, 6);
  gGeoManager->SetVolumeAttribute("EAIP", "SEEN", 0);


  // Place hexagonal single cells ECCU inside EHC3
  // skip cells which go into the hole in top left corner.

  xrow=1;
  yb = -dpara3[1] + (1./fgkSqroot3by2)*hexd1[6];
  zb = 0.;
  for (j = 1; j <= fgkNcellHole; ++j) {
    xb =-(dpara3[0] + dpara3[1]*0.577) + 2*hexd1[6];
    if(xrow >= 2){
      xb = xb+(xrow-1)*hexd1[6];
    }
    for (i = 1; i <= (fNcellSM - fgkNcellHole); ++i) {
      number = i+(j-1)*(fNcellSM - fgkNcellHole);
      TVirtualMC::GetMC()->Gspos("ECCU", number, "EHC3", xb,yb,zb, ihrotm, "ONLY");
      xb += (hexd1[6]*2.);
    }
    xrow = xrow+1;
    yb += (hexd1[6]*fgkSqroot3);
  }

  // ESMP is normal supermodule with base at bottom, with EHC3
  
  zAir1= -dparaSM3[2] + fgkThBase + dpara3Air[2]; 
  TVirtualMC::GetMC()->Gspos("EAIP", 1, "ESMP", 0., 0., zAir1, 0, "ONLY");
  zGas=zAir1+dpara3Air[2]+ fgkThPCB + dpara3[2]; 
  //Line below Commented for version 0 of PMD routine
  //  TVirtualMC::GetMC()->Gspos("EHC3", 1, "ESMP", 0., 0., zGas, 0, "ONLY");
  zAir2=zGas+dpara3[2]+ fgkThPCB + dpara3Air[2]; 
  TVirtualMC::GetMC()->Gspos("EAIP", 2, "ESMP", 0., 0., zAir2, 0, "ONLY");
  
  // ESMQ is mirror image of ESMP with base at bottom, with EHC3
  
  zAir1= -dparaSM3[2] + fgkThPCB + dpara3Air[2]; 
  TVirtualMC::GetMC()->Gspos("EAIP", 3, "ESMQ", 0., 0., zAir1, 0, "ONLY");
  zGas=zAir1+dpara3Air[2]+ fgkThPCB + dpara3[2]; 
  //Line below Commented for version 0 of PMD routine
  //  TVirtualMC::GetMC()->Gspos("EHC3", 2, "ESMQ", 0., 0., zGas, 0, "ONLY");
  zAir2=zGas+dpara3[2]+ fgkThPCB + dpara3Air[2]; 
  TVirtualMC::GetMC()->Gspos("EAIP", 4, "ESMQ", 0., 0., zAir2, 0, "ONLY");
  
}

//_____________________________________________________________________________

void AliPMDv0::CreatePMD()
{
  //
  // Create final detector from supermodules
  //
  // -- Author :     Y.P. VIYOGI, 07/05/1996. 
  // -- Modified:    P.V.K.S.Baba(JU), 15-12-97. 
  // -- Modified:    For New Geometry YPV, March 2001.

  Float_t  xp, yp, zp;
  Int_t i,j;
  Int_t nummod;
  Int_t jhrot12,jhrot13, irotdm;
  Int_t *idtmed = fIdtmed->GetArray()-599;
  
  //  VOLUMES Names : begining with "E" for all PMD volumes, 
  // The names of SIZE variables begin with S and have more meaningful
  // characters as shown below. 
  // 		VOLUME 	SIZE	MEDIUM	: 	REMARKS 
  // 		------	-----	------	: --------------------------- 
  // 		EPMD	GASPMD	 AIR	: INSIDE PMD  and its SIZE 
  // *** Define the  EPMD   Volume and fill with air *** 
  // Gaspmd, the dimension of HEXAGONAL mother volume of PMD,


  Float_t gaspmd[10] = {0.,360.,6,2,-4.,12.,150.,4.,12.,150.};

  gaspmd[5] = fgkNcellHole * fgkCellRadius * 2. * fgkSqroot3by2;
  gaspmd[8] = gaspmd[5];

  TVirtualMC::GetMC()->Gsvolu("EPMD", "PGON", idtmed[698], gaspmd, 10);
  gGeoManager->SetVolumeAttribute("EPMD", "SEEN", 0);

  AliMatrix(irotdm, 90., 0.,  90.,  90., 180., 0.);
   
  AliMatrix(jhrot12, 90., 120., 90., 210., 0., 0.);
  AliMatrix(jhrot13, 90., 240., 90., 330., 0., 0.);


  Float_t dmthick = 2. * fSMthick + fgkThLead + fgkThSteel;

  // dparaemm1 array contains parameters of the imaginary volume EMM1, 
  // EMM1 is a master module of type 1, which has 24 copies in the PMD.
  // EMM1 : normal volume as in old cases


  Float_t dparaemm1[6] = {12.5,12.5,0.8,30.,0.,0.};
  dparaemm1[0] = fSMLength/2.;
  dparaemm1[1] = dparaemm1[0] *fgkSqroot3by2;
  dparaemm1[2] = dmthick/2.;

  TVirtualMC::GetMC()->Gsvolu("EMM1","PARA", idtmed[698], dparaemm1, 6);
  gGeoManager->SetVolumeAttribute("EMM1", "SEEN", 1);

  //
  // --- DEFINE Modules, iron, and lead volumes 
  //   Pb Convertor for EMM1

  Float_t dparapb1[6] = {12.5,12.5,8.,30.,0.,0.};
  dparapb1[0] = fSMLength/2.;
  dparapb1[1] = dparapb1[0] * fgkSqroot3by2;
  dparapb1[2] = fgkThLead/2.;

  TVirtualMC::GetMC()->Gsvolu("EPB1","PARA", idtmed[600], dparapb1, 6);
  gGeoManager->SetVolumeAttribute ("EPB1", "SEEN", 0);

  //   Fe Support for EMM1
  Float_t dparafe1[6] = {12.5,12.5,8.,30.,0.,0.};
  dparafe1[0] = dparapb1[0];
  dparafe1[1] = dparapb1[1];
  dparafe1[2] = fgkThSteel/2.;

  TVirtualMC::GetMC()->Gsvolu("EFE1","PARA", idtmed[618], dparafe1, 6);
  gGeoManager->SetVolumeAttribute ("EFE1", "SEEN", 0);

  //  
  // position supermodule ESMA, ESMB, EPB1, EFE1 inside EMM1

  Float_t zps,zpb,zfe,zcv; 
  
  zps = -dparaemm1[2] + fSMthick/2.;
  TVirtualMC::GetMC()->Gspos("ESMB", 1, "EMM1", 0., 0., zps, 0, "ONLY");
  zpb = zps+fSMthick/2.+dparapb1[2];
  TVirtualMC::GetMC()->Gspos("EPB1", 1, "EMM1", 0., 0., zpb, 0, "ONLY");
  zfe = zpb+dparapb1[2]+dparafe1[2];
  TVirtualMC::GetMC()->Gspos("EFE1", 1, "EMM1", 0., 0., zfe, 0, "ONLY");
  zcv = zfe+dparafe1[2]+fSMthick/2.;
  TVirtualMC::GetMC()->Gspos("ESMA", 1, "EMM1", 0., 0., zcv, 0, "ONLY");

  // EMM2 : special master module having full row of cells but the number
  //        of rows limited by hole.

  Float_t dparaemm2[6] = {12.5,12.5,0.8,30.,0.,0.};
  dparaemm2[0] = fSMLength/2.;
  dparaemm2[1] = (fNcellSM - fgkNcellHole + 0.25)*fgkCellRadius*fgkSqroot3by2;
  dparaemm2[2] = dmthick/2.;

  TVirtualMC::GetMC()->Gsvolu("EMM2","PARA", idtmed[698], dparaemm2, 6);
  gGeoManager->SetVolumeAttribute("EMM2", "SEEN", 1);

  //   Pb Convertor for EMM2
  Float_t dparapb2[6] = {12.5,12.5,8.,30.,0.,0.};
  dparapb2[0] = dparaemm2[0];
  dparapb2[1] = dparaemm2[1];
  dparapb2[2] = fgkThLead/2.;

  TVirtualMC::GetMC()->Gsvolu("EPB2","PARA", idtmed[600], dparapb2, 6);
  gGeoManager->SetVolumeAttribute ("EPB2", "SEEN", 0);

  //   Fe Support for EMM2
  Float_t dparafe2[6] = {12.5,12.5,8.,30.,0.,0.};
  dparafe2[0] = dparapb2[0];
  dparafe2[1] = dparapb2[1];
  dparafe2[2] = fgkThSteel/2.;

  TVirtualMC::GetMC()->Gsvolu("EFE2","PARA", idtmed[618], dparafe2, 6);
  gGeoManager->SetVolumeAttribute ("EFE2", "SEEN", 0);

  // position supermodule  ESMX, ESMY inside EMM2

  zps = -dparaemm2[2] + fSMthick/2.;
  TVirtualMC::GetMC()->Gspos("ESMY", 1, "EMM2", 0., 0., zps, 0, "ONLY");
  zpb = zps + fSMthick/2.+dparapb2[2];
  TVirtualMC::GetMC()->Gspos("EPB2", 1, "EMM2", 0., 0., zpb, 0, "ONLY");
  zfe = zpb + dparapb2[2]+dparafe2[2];
  TVirtualMC::GetMC()->Gspos("EFE2", 1, "EMM2", 0., 0., zfe, 0, "ONLY");
  zcv = zfe + dparafe2[2]+fSMthick/2.;
  TVirtualMC::GetMC()->Gspos("ESMX", 1, "EMM2", 0., 0., zcv, 0, "ONLY");
  // 
  // EMM3 : special master module having truncated rows and columns of cells 
  //        limited by hole.

  Float_t dparaemm3[6] = {12.5,12.5,0.8,30.,0.,0.};
  dparaemm3[0] = dparaemm2[1]/fgkSqroot3by2;
  dparaemm3[1] = (fgkNcellHole + 0.25) * fgkCellRadius *fgkSqroot3by2;
  dparaemm3[2] = dmthick/2.;

  TVirtualMC::GetMC()->Gsvolu("EMM3","PARA", idtmed[698], dparaemm3, 6);
  gGeoManager->SetVolumeAttribute("EMM3", "SEEN", 1);

  //   Pb Convertor for EMM3
  Float_t dparapb3[6] = {12.5,12.5,8.,30.,0.,0.};
  dparapb3[0] = dparaemm3[0];
  dparapb3[1] = dparaemm3[1];
  dparapb3[2] = fgkThLead/2.;

  TVirtualMC::GetMC()->Gsvolu("EPB3","PARA", idtmed[600], dparapb3, 6);
  gGeoManager->SetVolumeAttribute ("EPB3", "SEEN", 0);

  //   Fe Support for EMM3
  Float_t dparafe3[6] = {12.5,12.5,8.,30.,0.,0.};
  dparafe3[0] = dparapb3[0];
  dparafe3[1] = dparapb3[1];
  dparafe3[2] = fgkThSteel/2.;

  TVirtualMC::GetMC()->Gsvolu("EFE3","PARA", idtmed[618], dparafe3, 6);
  gGeoManager->SetVolumeAttribute ("EFE3", "SEEN", 0);

  // position supermodule  ESMP, ESMQ inside EMM3

  zps = -dparaemm3[2] + fSMthick/2.;
  TVirtualMC::GetMC()->Gspos("ESMQ", 1, "EMM3", 0., 0., zps, 0, "ONLY");
  zpb = zps + fSMthick/2.+dparapb3[2];
  TVirtualMC::GetMC()->Gspos("EPB3", 1, "EMM3", 0., 0., zpb, 0, "ONLY");
  zfe = zpb + dparapb3[2]+dparafe3[2];
  TVirtualMC::GetMC()->Gspos("EFE3", 1, "EMM3", 0., 0., zfe, 0, "ONLY");
  zcv = zfe + dparafe3[2] + fSMthick/2.;
  TVirtualMC::GetMC()->Gspos("ESMP", 1, "EMM3", 0., 0., zcv, 0, "ONLY");
  // 

  // EHOL is a tube structure made of air
  //
  //Float_t d_hole[3];
  //d_hole[0] = 0.;
  //d_hole[1] = fgkNcellHole * fgkCellRadius *2. * fgkSqroot3by2 + boundary;
  //d_hole[2] = dmthick/2.;
  //
  //TVirtualMC::GetMC()->Gsvolu("EHOL", "TUBE", idtmed[698], d_hole, 3);
  //gGeoManager->SetVolumeAttribute("EHOL", "SEEN", 1);

  //Al-rod as boundary of the supermodules

  Float_t alRod[3] ;
  alRod[0] = fSMLength * 3/2. - gaspmd[5]/2 - fgkBoundary ;
  alRod[1] = fgkBoundary;
  alRod[2] = dmthick/2.;

  TVirtualMC::GetMC()->Gsvolu("EALM","BOX ", idtmed[698], alRod, 3);
  gGeoManager->SetVolumeAttribute ("EALM", "SEEN", 1);
  Float_t xalm[3];
  xalm[0]=alRod[0] + gaspmd[5] + 3.0*fgkBoundary;
  xalm[1]=-xalm[0]/2.;
  xalm[2]=xalm[1];

  Float_t yalm[3];
  yalm[0]=0.;
  yalm[1]=xalm[0]*fgkSqroot3by2;
  yalm[2]=-yalm[1];

  // delx = full side of the supermodule
  Float_t delx=fSMLength * 3.;
  Float_t x1= delx*fgkSqroot3by2 /2.;
  Float_t x4=delx/4.; 

  // placing master modules and Al-rod in PMD

  Float_t dx = fSMLength;
  Float_t dy = dx * fgkSqroot3by2;
  Float_t xsup[9] = {static_cast<Float_t>(-dx/2.), static_cast<Float_t>(dx/2.), static_cast<Float_t>(3.*dx/2.), 
		     -dx,    0.,       dx,
		     static_cast<Float_t>(-3.*dx/2.), static_cast<Float_t>(-dx/2.), static_cast<Float_t>(dx/2.)};

  Float_t ysup[9] = {dy,  dy,  dy, 
                     0.,  0.,  0., 
                    -dy, -dy, -dy};

  // xpos and ypos are the x & y coordinates of the centres of EMM1 volumes

  Float_t xoff = fgkBoundary * TMath::Tan(fgkPi/6.);
  Float_t xmod[3]={x4 + xoff , x4 + xoff, static_cast<Float_t>(-2.*x4-fgkBoundary/fgkSqroot3by2)};
  Float_t ymod[3] = {-x1 - fgkBoundary, x1 + fgkBoundary, 0.};
  Float_t xpos[9], ypos[9], x2, y2, x3, y3;

  Float_t xemm2 = fSMLength/2. - 
                  (fNcellSM + fgkNcellHole + 0.25) * fgkCellRadius * 0.5
                  + xoff;
  Float_t yemm2 = -(fNcellSM + fgkNcellHole + 0.25)*fgkCellRadius*fgkSqroot3by2
                  - fgkBoundary;

  Float_t xemm3 = (fNcellSM + 0.5 * fgkNcellHole + 0.25) * fgkCellRadius +
    xoff;
  Float_t yemm3 = - (fgkNcellHole - 0.25) * fgkCellRadius * fgkSqroot3by2 -
    fgkBoundary;

  Float_t theta[3] = {0., static_cast<Float_t>(2.*fgkPi/3.), static_cast<Float_t>(4.*fgkPi/3.)};
  Int_t irotate[3] = {0, jhrot12, jhrot13};
  
  nummod=0;
  for (j=0; j<3; ++j) {
     TVirtualMC::GetMC()->Gspos("EALM", j+1, "EPMD", xalm[j],yalm[j], 0., irotate[j], "ONLY");
     x2=xemm2*TMath::Cos(theta[j]) - yemm2*TMath::Sin(theta[j]);
     y2=xemm2*TMath::Sin(theta[j]) + yemm2*TMath::Cos(theta[j]);

     TVirtualMC::GetMC()->Gspos("EMM2", j+1, "EPMD", x2,y2, 0., irotate[j], "ONLY");

     x3=xemm3*TMath::Cos(theta[j]) - yemm3*TMath::Sin(theta[j]);
     y3=xemm3*TMath::Sin(theta[j]) + yemm3*TMath::Cos(theta[j]);

     TVirtualMC::GetMC()->Gspos("EMM3", j+4, "EPMD", x3,y3, 0., irotate[j], "ONLY");

     for (i=1; i<9; ++i) {
       xpos[i]=xmod[j] + xsup[i]*TMath::Cos(theta[j]) -
	 ysup[i]*TMath::Sin(theta[j]);
       ypos[i]=ymod[j] + xsup[i]*TMath::Sin(theta[j]) +
	 ysup[i]*TMath::Cos(theta[j]);
        
       AliDebugClass(1,Form("xpos: %f, ypos: %f", xpos[i], ypos[i]));
       
       nummod = nummod+1;
       
       AliDebugClass(1,Form("nummod %d",nummod));
       
       TVirtualMC::GetMC()->Gspos("EMM1", nummod + 6, "EPMD", xpos[i],ypos[i], 0., irotate[j], "ONLY");
       
     }
  }
  
  
  // place EHOL in the centre of EPMD
  // TVirtualMC::GetMC()->Gspos("EHOL", 1, "EPMD", 0.,0.,0., 0, "ONLY");
  
  // --- Place the EPMD in ALICE 
  xp = 0.;
  yp = 0.;
  zp = fgkZdist;
  
  TVirtualMC::GetMC()->Gspos("EPMD", 1, "ALIC", xp,yp,zp, 0, "ONLY");
    
}

 
//_____________________________________________________________________________
void AliPMDv0::CreateMaterials()
{
  //
  // Create materials for the PMD
  //
  // ORIGIN    : Y. P. VIYOGI 
  //
  
  //  cout << " Inside create materials " << endl;

  Int_t isxfld = ((AliMagF*)TGeoGlobalMagField::Instance()->GetField())->Integ();
  Float_t sxmgmx = ((AliMagF*)TGeoGlobalMagField::Instance()->GetField())->Max();
  
  // --- Define the various materials for GEANT --- 

  AliMaterial(1, "Pb    $", 207.19, 82., 11.35, .56, 18.5);
  
  // Argon

  Float_t dAr   = 0.001782;   // --- Ar density in g/cm3 --- 
  Float_t x0Ar = 19.55 / dAr;
  AliMaterial(2, "Argon$", 39.95, 18., dAr, x0Ar, 6.5e4);

  // --- CO2 --- 

  Float_t aCO2[2] = { 12.,16. };
  Float_t zCO2[2] = { 6.,8. };
  Float_t wCO2[2] = { 1.,2. };
  Float_t dCO2    = 0.001977;
  AliMixture(3, "CO2  $", aCO2, zCO2, dCO2, -2, wCO2);

  AliMaterial(4, "Al   $", 26.98, 13., 2.7, 8.9, 18.5);

  // ArCO2

  Float_t aArCO2[3] = {39.948,12.0107,15.9994};
  Float_t zArCO2[3] = {18.,6.,8.};
  Float_t wArCO2[3] = {0.7,0.08,0.22};
  Float_t dArCO2    = dAr * 0.7 + dCO2 * 0.3;
  AliMixture(5, "ArCO2$", aArCO2, zArCO2, dArCO2, 3, wArCO2);

  AliMaterial(6, "Fe   $", 55.85, 26., 7.87, 1.76, 18.5);

  // G10
  
  Float_t aG10[4]={1.,12.011,15.9994,28.086};
  Float_t zG10[4]={1.,6.,8.,14.};
  //PH  Float_t wG10[4]={0.148648649,0.104054054,0.483499056,0.241666667};
  Float_t wG10[4]={0.15201,0.10641,0.49444,0.24714};
  AliMixture(8,"G10",aG10,zG10,1.7,4,wG10);
  
  AliMaterial(15, "Cu   $", 63.54, 29., 8.96, 1.43, 15.);

  // Steel
  Float_t aSteel[4] = { 55.847,51.9961,58.6934,28.0855 };
  Float_t zSteel[4] = { 26.,24.,28.,14. };
  Float_t wSteel[4] = { .715,.18,.1,.005 };
  Float_t dSteel    = 7.88;
  AliMixture(19, "STAINLESS STEEL$", aSteel, zSteel, dSteel, 4, wSteel); 

  //Air

  Float_t aAir[4]={12.0107,14.0067,15.9994,39.948};
  Float_t zAir[4]={6.,7.,8.,18.};
  Float_t wAir[4]={0.000124,0.755267,0.231781,0.012827};
  Float_t dAir1 = 1.20479E-10;
  Float_t dAir = 1.20479E-3;
  AliMixture(98, "Vacum$", aAir,  zAir, dAir1, 4, wAir);
  AliMixture(99, "Air  $", aAir,  zAir, dAir , 4, wAir);

  // Define tracking media 
  AliMedium(1,  "Pb conv.$", 1,  0, 0, isxfld, sxmgmx, 1., .1, .01, .1);
  AliMedium(4,  "Al      $", 4,  0, 0, isxfld, sxmgmx, .1, .1, .01, .1);
  AliMedium(5,  "ArCO2   $", 5,  1, 0, isxfld, sxmgmx, .1, .1, .10, .1);
  AliMedium(6,  "Fe      $", 6,  0, 0, isxfld, sxmgmx, .1, .1, .01, .1);
  AliMedium(8,  "G10plate$", 8,  0, 0, isxfld, sxmgmx, 1., .1, .01, .1);
  AliMedium(15, "Cu      $", 15, 0, 0, isxfld, sxmgmx, .1, .1, .01, .1);
  AliMedium(19, "S  steel$", 19, 0, 0, isxfld, sxmgmx, 1., .1, .01, .1);
  AliMedium(98, "Vacuum  $", 98, 0, 0, isxfld, sxmgmx, 1., .1, .10, 10);
  AliMedium(99, "Air gaps$", 99, 0, 0, isxfld, sxmgmx, 1., .1, .10, .1);

}

//_____________________________________________________________________________
void AliPMDv0::Init()
{
  //
  // Initialises PMD detector after it has been built
  //
  Int_t i;
  //  kdet=1;
  //
  if(AliLog::GetGlobalDebugLevel()>0) {
      printf("\n%s: ",ClassName());
      for(i=0;i<35;i++) printf("*");
      printf(" PMD_INIT ");
      for(i=0;i<35;i++) printf("*");
      printf("\n%s: ",ClassName());
      printf("                 PMD simulation package (v0) initialised\n");
      printf("%s: parameters of pmd\n", ClassName());
      printf("%s: %10.2f %10.2f %10.2f \
      %10.2f\n",ClassName(),fgkCellRadius,fgkCellWall,fgkCellDepth,fgkZdist );
      printf("%s: ",ClassName());
      for(i=0;i<80;i++) printf("*");
      printf("\n");
  }
  Int_t *idtmed = fIdtmed->GetArray()-599;
  fMedSens=idtmed[605-1];
  // --- Generate explicitly delta rays in the iron, aluminium and lead --- 
  // removed all Gstpar and energy cut-offs moved to galice.cuts
}

//_____________________________________________________________________________
void AliPMDv0::StepManager()
{
  //
  // Called at each step in the PMD
  //
  Int_t   copy;
  Float_t hits[5], destep;
  Float_t center[3] = {0,0,0};
  Int_t   vol[6];
  //char *namep;
  
  if(TVirtualMC::GetMC()->CurrentMedium() == fMedSens && (destep = TVirtualMC::GetMC()->Edep())) {
  
    TVirtualMC::GetMC()->CurrentVolID(copy);
    vol[0] = copy;
    TVirtualMC::GetMC()->CurrentVolOffID(1,copy);
    vol[1] = copy;
    TVirtualMC::GetMC()->CurrentVolOffID(2,copy);
    vol[2] = copy;
    TVirtualMC::GetMC()->CurrentVolOffID(3,copy);
    vol[3] = copy;
    TVirtualMC::GetMC()->CurrentVolOffID(4,copy);
    vol[4] = copy;
    TVirtualMC::GetMC()->CurrentVolOffID(5,copy);
    vol[5] = copy;

    TVirtualMC::GetMC()->Gdtom(center,hits,1);
    hits[3] = destep*1e9; //Number in eV

   // this is for pile-up events
    hits[4] = TVirtualMC::GetMC()->TrackTime();

    AddHit(gAlice->GetMCApp()->GetCurrentTrackNumber(), vol, hits);
  }
}

  
//------------------------------------------------------------------------
// Get parameters

void AliPMDv0::GetParameters()
{
  // This gives all the parameters of the detector
  // such as Length of Supermodules
  // thickness of the Supermodule
  //
  Int_t ncellum, numum;
  ncellum  = 24;
  numum    = 3;
  fNcellSM  = ncellum * numum;  //no. of cells in a row in one supermodule
  fSMLength = (fNcellSM + 0.25 )*fgkCellRadius*2.;
  fSMthick  = fgkThBase + fgkThAir + fgkThPCB + fgkCellDepth +
    fgkThPCB + fgkThAir + fgkThPCB;
}

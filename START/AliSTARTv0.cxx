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

/////////////////////////////////////////////////////////////////////
//                                                                 //
// START ( T-zero) detector  version 0                        //
//
//Begin Html       
/*
<img src="gif/AliSTARTv0Class.gif">
*/
//End Html
//                                                                  //
//                                                                  //
//////////////////////////////////////////////////////////////////////

#include <Riostream.h>
#include <stdlib.h>

#include <TGeometry.h>
#include <TLorentzVector.h>
#include <TMath.h>
#include <TNode.h>
#include <TTUBE.h>
#include <TVirtualMC.h>

#include "AliMagF.h"
#include "AliRun.h"
#include "AliSTARThit.h"
#include "AliSTARTv0.h"

ClassImp(AliSTARTv0)

//--------------------------------------------------------------------
AliSTARTv0::AliSTARTv0(const char *name, const char *title):
 AliSTART(name,title)
{
  //
  // Standart constructor for START Detector version 0
  //
  fIdSens1=0;
//  setBufferSize(128000);
}
//-------------------------------------------------------------------------
void AliSTARTv0::CreateGeometry()
{
  //
  // Create the geometry of START Detector version 0
  //
  // begin Html
  /*
   <img src="gif/AliSTARTv0.gif">
  */
  //


  Int_t *idtmed = fIdtmed->GetArray();

  Int_t is;
  Int_t idrotm[999];
  Float_t x,y,z;

  Float_t pstart[3]={4.5,10.7,5.3};
  Float_t pinstart[3]={0.,1.3,5.25};
  Float_t ppmt[3]={0.,1.3,3.5};
  Float_t pdivider[3]={0.,1.2,1.75};
  Float_t pdiv2[3]={0.,1.2,1.25};
  Float_t pdiv1[3]={0.6,1.2,0.5};
  Float_t ptop[3]={0.,1.3,1.5};
  Float_t pbot[3]={0.6,1.2,0.1};
  Float_t pglass[3]={1.2,1.3,2.};
  Float_t pcer[3]={0.9,1.1,0.09};
  Float_t psteel[3]={0.9,1.1,0.01};
  Float_t ppins[3]={0.6,1.2,0.014};
  Float_t phole[3]={0.6,1.2,0.015};
  Float_t pknob[3]={0.5,0.6,0.4};
  Float_t pknob_vac[3]={0.,0.5,0.4};
  Float_t pknob_bot[3]={0.,0.6,0.05};
  Float_t pribber[3] = {0.,1.2,2.413/2.};
  Float_t presist[3] = {0.,1.2,0.087/2.};

  Float_t zdetRight=69.7,zdetLeft=350;
 //-------------------------------------------------------------------
 //  START volume 
 //-------------------------------------------------------------------
  
  //  Float_t theta=TMath::ATan(6.5/zdet);
  Float_t thetaRight=(180./3.1415)*TMath::ATan(6.5/zdetRight);
  Float_t thetaLeft=(180./3.1415)*TMath::ATan(6.5/zdetLeft);
    AliMatrix(idrotm[901], 90., 0., 90., 90., 180., 0.);
    
    gMC->Gsvolu("0STR","TUBE",idtmed[1],pstart,3);
    gMC->Gsvolu("0STL","TUBE",idtmed[1],pstart,3);
    gMC->Gspos("0STR",1,"ALIC",0.,0.,zdetRight+pstart[2],0,"ONLY");
    gMC->Gspos("0STL",1,"ALIC",0.,0.,-zdetLeft-pstart[2],idrotm[901],"ONLY");

//START interior
    gMC->Gsvolu("0INS","TUBE",idtmed[1],pinstart,3);
    gMC->Gsvolu("0PMT","TUBE",idtmed[3],ppmt,3);     
    gMC->Gsvolu("0DIV","TUBE",idtmed[3],pdivider,3);     

// first ring: 12 units of Scintillator+PMT+divider
    Double_t dang1 = 2*TMath::Pi()/12;
    for (is=1; is<=12; is++)
      {  
	AliMatrix(idrotm[901+is], 
		  90.-thetaRight, 30.*is, 
		  90.,       90.+30.*is,
		  thetaRight,    180.+30.*is); 
	x=6.5*TMath::Sin(is*dang1);
	y=6.5*TMath::Cos(is*dang1);
	z=-pstart[2]+pinstart[2];
	gMC->Gspos("0INS",is,"0STR",x,y,z,idrotm[901+is],"ONLY");

      }	
    for (is=1; is<=12; is++)
      {
        AliMatrix(idrotm[901+is],
                  90.-thetaLeft, 30.*is,
                  90.,       90.+30.*is,
                  thetaLeft,    180.+30.*is);

        x=6.5*TMath::Sin(is*dang1);
        y=6.5*TMath::Cos(is*dang1);
        z=-pstart[2]+pinstart[2];
        gMC->Gspos("0INS",is,"0STL",x,y,z,idrotm[901+is],"ONLY");

      }
     x=0;
    y=0;
    z=-pinstart[2]+ppmt[2];
    if(fDebug) printf("%s: is %d, z Divider %f\n",ClassName(),is,z);
    gMC->Gspos("0PMT",1,"0INS",x,y,z,0,"ONLY");
    z=pinstart[2]-pdivider[2];
    gMC->Gspos("0DIV",1,"0INS",x,y,z,0,"ONLY");
      
     /*  
//second ring: 20 units of Scintillator+PMT+divider
      Double_t dang2 = 2*TMath::Pi()/26;
      Double_t dang3 = 2*TMath::Pi()/20;
       for (is=14; is<=33;is++)  
     {  
     x=9.3*TMath::Sin(dang2+(is-13)*dang3);
     y=9.3*TMath::Cos(dang2+(is-13)*dang3);
      z=-pstart[2]+ppmt[2];
      gMC->Gspos("0PMT",is,"0STA",x,y,z,0,"ONLY");
      z=z+ppmt[2]+pdiv2[2];
      gMC->Gspos("0DIV",is,"0STA",x,y,z,0,"ONLY");
      }
     */
// PMT
      
    // Entry window (glass)
    // gMC->Gsvolu("0TOP","TUBE",idtmed[6],ptop,3);
       gMC->Gsvolu("0TOP","TUBE",idtmed[12],ptop,3); //lucite
     z=-ppmt[2]+ptop[2];
    gMC->Gspos("0TOP",1,"0PMT",0,0,z,0,"ONLY");
    //     printf("Z PTOP %f -ppmt[2] %f ptop[2] %f\n",z,-ppmt[2],ptop[2]);
    
    // Bottom glass
    gMC->Gsvolu("0BOT","TUBE",idtmed[6],pbot,3);
    z=ppmt[2]-pbot[2];
    if(fDebug) printf("%s: Z bottom %f\n",ClassName(),z);
    gMC->Gspos("0BOT",1,"0PMT",0,0,z,0,"ONLY");
    // Side cylinder glass
    gMC->Gsvolu("0OUT","TUBE",idtmed[6],pglass,3);
    z=ppmt[2]-pglass[2];
    //      printf("Z glass %f\n",z);
    gMC->Gspos("0OUT",1,"0PMT",0,0,z,0,"ONLY");
    //PMT electrodes support structure
    gMC->Gsvolu("0CER","TUBE",idtmed[4],pcer,3);
    gMC->Gsvolu("0STE","TUBE",idtmed[8],psteel,3);
    z=-ppmt[2]+2*ptop[2]+0.3;;
    //      printf("Z Cer 1 %f\n",z);
    for (is=1; is<=15; is++)
      {
	z=z+psteel[2]+pcer[2];
	gMC->Gspos("0CER",is,"0PMT",0,0,z,0,"ONLY");
	z=z+psteel[2]+pcer[2];
	gMC->Gspos("0STE",is,"0PMT",0,0,z,0,"ONLY");
      }
    
    // Divider
    // Knob at the bottom of PMT baloon
    
    gMC->Gsvolu("0NB","TUBE",idtmed[6],pknob,3);
    z=-pdivider[2]+pknob[2];
    //      printf("zknob %f\n",z);
    gMC->Gspos("0NB",1,"0DIV",0,0,z,0,"ONLY");
    gMC->Gsvolu("0KB","TUBE",idtmed[6],pknob_bot,3);
    z=-pdivider[2]+2*pknob[2]+pknob_bot[2];
    //      printf(knobbot %f\n",z);
    gMC->Gspos("0KB",1,"0DIV ",0,0,z,0,"ONLY");
    gMC->Gsvolu("0VAC","TUBE",idtmed[3],pknob_vac,3);
    z=-pdivider[2]+pknob_vac[2];
    //      printf("knobvac %f\n",z);
    gMC->Gspos("0VAC",1,"0DIV",0,0,z,0,"ONLY");
    //Steel pins + pin holes
    gMC->Gsvolu("0PIN","TUBE",idtmed[8],ppins,3);
    z=-pdivider[2]+ppins[2];
    gMC->Gspos("0PIN",1,"0DIV",0,0,z,0,"ONLY");
    gMC->Gsvolu("0HOL","TUBE",idtmed[11],phole,3);
    z=-pdivider[2]+2*ppins[2]+phole[2];
    gMC->Gspos("0HOL",1,"0DIV",0,0,z,0,"ONLY");
    
    //Socket
    gMC->Gsvolu("0V1","TUBE",idtmed[4],pdiv1,3);
    z=-pdivider[2]+pdiv1[2];
    gMC->Gspos("0V1",1,"0DIV",0,0,z,0,"ONLY");
    //Resistors
    gMC->Gsvolu("0V2","TUBE",idtmed[1],pdiv2,3);
    z=pdivider[2]-pdiv2[2];
    gMC->Gspos("0V2",1,"0DIV",0,0,z,0,"ONLY");
    gMC->Gsvolu("0RS","TUBE",idtmed[4],presist,3);
    z=-pdiv2[2]+presist[2];
    gMC->Gspos("0RS",1,"0V2",0,0,z,0,"ONLY");
    gMC->Gsvolu("0RB","TUBE",idtmed[9],pribber,3);
    z=pdiv2[2]-pribber[2];
    gMC->Gspos("0RB",1,"0V2",0,0,z,0,"ONLY");
    //      printf("z DRIB %f\n",z);
       
    
}    



    
//------------------------------------------------------------------------
void AliSTARTv0::CreateMaterials()
{
   Int_t isxfld   = gAlice->Field()->Integ();
   Float_t sxmgmx = gAlice->Field()->Max();
   Float_t a,z,d,radl,absl,buf[1];
   Int_t nbuf;

// Scintillator CH
   Float_t ascin[2]={1.01,12.01};
   Float_t zscin[2]={1,6};
   Float_t wscin[2]={1,1};
   Float_t denscin=1.03;
// PMT glass SiO2
   Float_t aglass[2]={28.0855,15.9994};
   Float_t zglass[2]={14.,8.};
   Float_t wglass[2]={1.,2.};
   Float_t dglass=2.65;
// Ceramic   97.2% Al2O3 , 2.8% SiO2
   Float_t acer[2],zcer[2],wcer[2]={0.972,0.028};
   Float_t aal2o3[2]  = { 26.981539,15.9994 };
   Float_t zal2o3[2]  = { 13.,8. };
   Float_t wal2o3[2]  = { 2.,3. };
   Float_t denscer  = 3.6;

// Brass 80% Cu, 20% Zn
   Float_t abrass[2] = {63.546,65.39};
   Float_t zbrass[2] = {29,30};
   Float_t wbrass[2] = {0.8,0.2};
   Float_t denbrass=8.96;

//Ribber C6H12S
   Float_t aribber[3] = {12.,1.,32.};
   Float_t zribber[3] = {6.,1.,16.};
   Float_t wribber[3] = {6.,12.,1.};
   Float_t denribber=0.8;
   /*
// Definition Cherenkov parameters
   Float_t ppckov[14] = { 5.63e-9,5.77e-9,5.9e-9,6.05e-9,6.2e-9,6.36e-9,6.52e-9,6.7e-9,6.88e-9,7.08e-9,7.3e-9,7.51e-9,7.74e-9,8e-9 };
   Float_t rindex_quarz[14] = { 1.528309,1.533333,
				1.538243,1.544223,1.550568,1.55777,
				1.565463,1.574765,1.584831,1.597027,
			        1.611858,1.6277,1.6472,1.6724 };
 
   Float_t absco_quarz[14] = { 20.126,16.27,13.49,11.728,9.224,8.38,7.44,7.17,
				6.324,4.483,1.6,.323,.073,0. };
   */
   //   Int_t *idtmed = fIdtmed->GetArray()-999;
    
   //  TGeant3 *geant3 = (TGeant3*) gMC;
    
//*** Definition Of avaible START materials ***
   AliMaterial(0, "START Steel$", 55.850,26.,7.87,1.76,999);
   AliMaterial(1, "START Vacuum$", 1.e-16,1.e-16,1.e-16,1.e16,999);
   AliMaterial(2, "START Air$", 14.61, 7.3, .001205, 30423.,999); 

   AliMixture( 3, "Al2O3   $", aal2o3, zal2o3, denscer, -2, wal2o3);
   AliMixture( 4, "PMT glass   $",aglass,zglass,dglass,-2,wglass);
   char namate[21];
   gMC->Gfmate((*fIdmate)[3], namate, a, z, d, radl, absl, buf, nbuf);
   acer[0]=a;
   zcer[0]=z;
   gMC->Gfmate((*fIdmate)[4], namate, a, z, d, radl, absl, buf, nbuf);
   acer[1]=a;
   zcer[1]=z;
   
   AliMixture( 9, "Ceramic    $", acer, zcer, denscer, 2, wcer);
   AliMixture( 5, "Scintillator$",ascin,zscin,denscin,-2,wscin);
   AliMixture( 6, "Brass    $", abrass, zbrass, denbrass, 2, wbrass);
   
   AliMixture( 7, "Ribber $",aribber,zribber,denribber,-3,wribber);
   
   
   AliMedium(1, "START Air$", 2, 0, isxfld, sxmgmx, 10., .1, 1., .003, .003);
   AliMedium(2, "Scintillator$", 5, 1, isxfld, sxmgmx, 10., .01, 1., .003, .003);
   AliMedium(3, "Vacuum$", 1, 0, isxfld, sxmgmx, 10., .01, .1, .003, .003);
   AliMedium(4, "Ceramic$", 9, 0, isxfld, sxmgmx, 10., .01, .1, .003, .003);
   AliMedium(6, "Glass$", 4, 0, isxfld, sxmgmx, 10., .01, .1, .003, .003);
   AliMedium(8, "Steel$", 0, 0, isxfld, sxmgmx, 1., .001, 1., .001, .001);
   AliMedium(11, "Brass  $", 6, 0, isxfld, sxmgmx, 10., .01, .1, .003, .003);
   AliMedium(9, "Ribber  $", 7, 0, isxfld, sxmgmx, 10., .01, .1, .003, .003);
   AliMedium(12, "Lucite$", 8, 1, isxfld, sxmgmx, 10., .01, 1., .003, .003);  

//  geant3->Gsckov(idtmed[2105], 14, ppckov, absco_quarz, effic_all,rindex_quarz);

}
//---------------------------------------------------------------------
void AliSTARTv0::DrawModule()
{
//
// Draw a shaded view of the Forward multiplicity detector version 0
//
  
  //Set ALIC mother transparent
  gMC->Gsatt("ALIC","SEEN",0);
  //
  //Set volumes visible
  gMC->Gsatt("0STA","SEEN",0);
  gMC->Gsatt("0PMT","SEEN",1);
  gMC->Gsatt("0DIV","SEEN",1);
  //
  gMC->Gdopt("hide","on");
  gMC->Gdopt("shad","on");
  gMC->SetClipBox(".");
  gMC->SetClipBox("*",0,1000,-1000,1000,-1000,1000);
  gMC->DefaultRange();
  gMC->Gdraw("alic",40,30,0,12,9.5,.7,0.7);
  gMC->Gdhead(1111,"T-Zero detector");
  gMC->Gdopt("hide","off");
}

//-------------------------------------------------------------------
void AliSTARTv0::Init()
{
// Initialises version 0 of the Forward Multiplicity Detector
//
//Int_t *idtmed  = gAlice->Idtmed();
  AliSTART::Init();
  fIdSens1=gMC->VolId("0TOP");
  printf("*** START version 0 initialized ***\n");
 
}

//-------------------------------------------------------------------

void AliSTARTv0::StepManager()
{
  //
  // Called for every step in the START Detector
  // See AliSTARTv1

}
//---------------------------------------------------------------------











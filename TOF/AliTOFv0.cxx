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
Revision 1.27  2002/07/24 16:13:56  vicinanz
Fixed bub in BuildGeometry

Revision 1.26  2002/05/08 13:24:50  vicinanz
AliTOFanalyzeMatching.C macro added and minor changes to the AliTOF code

Revision 1.25  2001/11/22 11:22:51  hristov
Updated version of TOF digitization, N^2 problem solved (J.Chudoba)

Revision 1.23  2001/09/27 10:39:20  vicinanz
SDigitizer and Merger added

Revision 1.22  2001/09/20 15:54:22  vicinanz
Updated Strip Structure (Double Stack)

Revision 1.21  2001/08/28 08:45:59  vicinanz
TTask and TFolder structures implemented

Revision 1.9  2001/05/04 10:09:48  vicinanz
Major upgrades to the strip structure

Revision 1.8  2000/12/04 08:48:20  alibrary
Fixing problems in the HEAD

Revision 1.7  2000/10/02 21:28:17  fca
Removal of useless dependecies via forward declarations

Revision 1.6  2000/05/10 16:52:18  vicinanz
New TOF version with holes for PHOS/RICH

Revision 1.4.2.1  2000/05/10 09:37:16  vicinanz
New version with Holes for PHOS/RICH

Revision 1.14  1999/11/05 22:39:06  fca
New hits structure

Revision 1.13  1999/11/02 11:26:39  fca
added stdlib.h for exit

Revision 1.12  1999/11/01 20:41:57  fca
Added protections against using the wrong version of FRAME

Revision 1.11  1999/10/22 08:04:14  fca
Correct improper use of negative parameters

Revision 1.10  1999/10/16 19:30:06  fca
Corrected Rotation Matrix and CVS log

Revision 1.9  1999/10/15 15:35:20  fca
New version for frame1099 with and without holes

Revision 1.8  1999/09/29 09:24:33  fca
Introduction of the Copyright and cvs Log

*/

///////////////////////////////////////////////////////////////////////////////
//
//  This class contains the functions for version 0 of the Time Of Flight    //
//  detector.                                                                //
//
//  VERSION WITH 5 MODULES AND TILTED STRIPS 
//  NO HITS DEFINED BY DEFAULT FOR THIS VERSION
//   FULL COVERAGE VERSION
//
//   Authors:
//
//   Alessio Seganti
//   Domenico Vicinanza
//
//   University of Salerno - Italy
//
//   Fabrizio Pierella
//   University of Bologna - Italy
//
//
//Begin_Html
/*
<img src="picts/AliTOFv0Class.gif">
*/
//End_Html
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include <iostream.h>
#include <stdlib.h>

#include "AliTOFv0.h"
#include "TBRIK.h"
#include "TGeometry.h"
#include "TNode.h"
#include <TLorentzVector.h>
#include "TObject.h"
#include "AliRun.h"
#include "AliMC.h"
#include "AliConst.h"

 
ClassImp(AliTOFv0)
 
//_____________________________________________________________________________
AliTOFv0::AliTOFv0()
{
  //
  // Default constructor
  //
}
 
//_____________________________________________________________________________
AliTOFv0::AliTOFv0(const char *name, const char *title)
        : AliTOF(name,title)
{
  //
  // Standard constructor
  //
  //
  // Check that FRAME is there otherwise we have no place where to
  // put TOF
  AliModule* frame=gAlice->GetModule("FRAME");
  if(!frame) {
    Error("Ctor","TOF needs FRAME to be present\n");
    exit(1);
  } else
    if(frame->IsVersion()!=1) {
      Error("Ctor","FRAME version 1 needed with this version of TOF\n");
      exit(1);
    }
 
}

//____________________________________________________________________________

void AliTOFv0::BuildGeometry()
{
  //
  // Build TOF ROOT geometry for the ALICE event display
  //
  TNode *node, *top;
  const int kColorTOF  = 27;
  
  // Find top TNODE
  top = gAlice->GetGeometry()->GetNode("alice");
  
  // Position the different copies
  const Float_t krTof  =(fRmax+fRmin)/2;
  const Float_t khTof  = fRmax-fRmin;
  const Int_t   kNTof = fNTof;
  const Float_t kPi   = TMath::Pi();
  const Float_t kangle = 2*kPi/kNTof;
  Float_t ang;
  
  // define offset for nodes
  Float_t zOffsetC = fZtof - fZlenC*0.5;
  Float_t zOffsetB = fZtof - fZlenC - fZlenB*0.5;
  Float_t zOffsetA = 0.;
  // Define TOF basic volume
  
  char nodeName0[7], nodeName1[7], nodeName2[7]; 
  char nodeName3[7], nodeName4[7], rotMatNum[7];
  
  new TBRIK("S_TOF_C","TOF box","void",
            fStripLn*0.5,khTof*0.5,fZlenC*0.5);
  new TBRIK("S_TOF_B","TOF box","void",
            fStripLn*0.5,khTof*0.5,fZlenB*0.5);
  new TBRIK("S_TOF_A","TOF box","void",
            fStripLn*0.5,khTof*0.5,fZlenA*0.5);
  
  for (Int_t nodeNum=1;nodeNum<19;nodeNum++){
    
    if (nodeNum<10) {
      sprintf(rotMatNum,"rot50%i",nodeNum);
      sprintf(nodeName0,"FTO00%i",nodeNum);
      sprintf(nodeName1,"FTO10%i",nodeNum);
      sprintf(nodeName2,"FTO20%i",nodeNum);
      sprintf(nodeName3,"FTO30%i",nodeNum);
      sprintf(nodeName4,"FTO40%i",nodeNum);
    }
    if (nodeNum>9) {
      sprintf(rotMatNum,"rot5%i",nodeNum);
      sprintf(nodeName0,"FTO0%i",nodeNum);
      sprintf(nodeName1,"FTO1%i",nodeNum);
      sprintf(nodeName2,"FTO2%i",nodeNum);
      sprintf(nodeName3,"FTO3%i",nodeNum);
      sprintf(nodeName4,"FTO4%i",nodeNum);
    }
    
    new TRotMatrix(rotMatNum,rotMatNum,90,-20*nodeNum,90,90-20*nodeNum,0,0);
    ang = (4.5-nodeNum) * kangle;
    
    top->cd();
    node = new TNode(nodeName0,nodeName0,"S_TOF_C",krTof*TMath::Cos(ang),krTof*TMath::Sin(ang),zOffsetC,rotMatNum);
    node->SetLineColor(kColorTOF);
    fNodes->Add(node); 
    
    top->cd(); 
    node = new TNode(nodeName1,nodeName1,"S_TOF_C",krTof*TMath::Cos(ang),krTof*TMath::Sin(ang),-zOffsetC,rotMatNum);
    node->SetLineColor(kColorTOF);
    fNodes->Add(node); 
    
    top->cd();
    node = new TNode(nodeName2,nodeName2,"S_TOF_B",krTof*TMath::Cos(ang),krTof*TMath::Sin(ang),zOffsetB,rotMatNum);
    node->SetLineColor(kColorTOF);
    fNodes->Add(node); 
    
    top->cd();
    node = new TNode(nodeName3,nodeName3,"S_TOF_B",krTof*TMath::Cos(ang),krTof*TMath::Sin(ang),-zOffsetB,rotMatNum);
    node->SetLineColor(kColorTOF);
    fNodes->Add(node); 
    
    top->cd();
    node = new TNode(nodeName4,nodeName4,"S_TOF_A",krTof*TMath::Cos(ang),krTof*TMath::Sin(ang),zOffsetA,rotMatNum);
    node->SetLineColor(kColorTOF);
    fNodes->Add(node); 
  } // end loop on nodeNum
}


 
//_____________________________________________________________________________
void AliTOFv0::CreateGeometry()
{
  //
  // Create geometry for Time Of Flight version 0
  //
  //Begin_Html
  /*
    <img src="picts/AliTOFv0.gif">
  */
  //End_Html
  //
  // Creates common geometry
  //
  AliTOF::CreateGeometry();
}
 
//_____________________________________________________________________________
void AliTOFv0::TOFpc(Float_t xtof, Float_t ytof, Float_t zlenC,
		     Float_t zlenB, Float_t zlenA, Float_t ztof0)
{
  //
  // Definition of the Time Of Fligh Resistive Plate Chambers
  // xFLT, yFLT, zFLT - sizes of TOF modules (large)
  
  Float_t  ycoor, zcoor;
  Float_t  par[3];
  Int_t    *idtmed = fIdtmed->GetArray()-499;
  Int_t    idrotm[100];
  Int_t    nrot = 0;
  Float_t  hTof = fRmax-fRmin;
  
  Float_t radius = fRmin+2.;//cm

  par[0] =  xtof * 0.5;
  par[1] =  ytof * 0.5;
  par[2] = zlenC * 0.5;
  gMC->Gsvolu("FTOC", "BOX ", idtmed[506], par, 3);
  par[2] = zlenB * 0.5;
  gMC->Gsvolu("FTOB", "BOX ", idtmed[506], par, 3);
  par[2] = zlenA * 0.5;
  gMC->Gsvolu("FTOA", "BOX ", idtmed[506], par, 3);


// Positioning of modules

   Float_t zcor1 = ztof0 - zlenC*0.5;
   Float_t zcor2 = ztof0 - zlenC - zlenB*0.5;
   Float_t zcor3 = 0.;

   AliMatrix(idrotm[0], 90.,  0., 0., 0., 90,-90.);
   AliMatrix(idrotm[1], 90.,180., 0., 0., 90, 90.);
   gMC->Gspos("FTOC", 1, "BTO1", 0,  zcor1, 0, idrotm[0], "ONLY");
   gMC->Gspos("FTOC", 2, "BTO1", 0, -zcor1, 0, idrotm[1], "ONLY");
   gMC->Gspos("FTOC", 1, "BTO2", 0,  zcor1, 0, idrotm[0], "ONLY");
   gMC->Gspos("FTOC", 2, "BTO2", 0, -zcor1, 0, idrotm[1], "ONLY");
   gMC->Gspos("FTOC", 1, "BTO3", 0,  zcor1, 0, idrotm[0], "ONLY");
   gMC->Gspos("FTOC", 2, "BTO3", 0, -zcor1, 0, idrotm[1], "ONLY");

   gMC->Gspos("FTOB", 1, "BTO1", 0,  zcor2, 0, idrotm[0], "ONLY");
   gMC->Gspos("FTOB", 2, "BTO1", 0, -zcor2, 0, idrotm[1], "ONLY");
   gMC->Gspos("FTOB", 1, "BTO2", 0,  zcor2, 0, idrotm[0], "ONLY");
   gMC->Gspos("FTOB", 2, "BTO2", 0, -zcor2, 0, idrotm[1], "ONLY");
   gMC->Gspos("FTOB", 1, "BTO3", 0,  zcor2, 0, idrotm[0], "ONLY");
   gMC->Gspos("FTOB", 2, "BTO3", 0, -zcor2, 0, idrotm[1], "ONLY");

   gMC->Gspos("FTOA", 0, "BTO1", 0, zcor3,  0, idrotm[0], "ONLY");
   gMC->Gspos("FTOA", 0, "BTO2", 0, zcor3,  0, idrotm[0], "ONLY");
   gMC->Gspos("FTOA", 0, "BTO3", 0, zcor3,  0, idrotm[0], "ONLY");

  Float_t db = 0.5;//cm
  Float_t xFLT, xFST, yFLT, zFLTA, zFLTB, zFLTC;

  xFLT = fStripLn;
  yFLT = ytof;
  zFLTA = zlenA;
  zFLTB = zlenB;
  zFLTC = zlenC;

  xFST = xFLT-fDeadBndX*2;//cm

// Sizes of MRPC pads

  Float_t yPad = 0.505;//cm 
  
// Large not sensitive volumes with Insensitive Freon
  par[0] = xFLT*0.5;
  par[1] = yFLT*0.5;
  
  cout <<"************************* TOF geometry **************************"<<endl;
 
  par[2] = (zFLTA *0.5);
  gMC->Gsvolu("FLTA", "BOX ", idtmed[512], par, 3); // Insensitive Freon
  gMC->Gspos ("FLTA", 0, "FTOA", 0., 0., 0., 0, "ONLY");

  par[2] = (zFLTB * 0.5);
  gMC->Gsvolu("FLTB", "BOX ", idtmed[512], par, 3); // Insensitive Freon
  gMC->Gspos ("FLTB", 0, "FTOB", 0., 0., 0., 0, "ONLY");

  par[2] = (zFLTC * 0.5);
  gMC->Gsvolu("FLTC", "BOX ", idtmed[512], par, 3); // Insensitive Freon
  gMC->Gspos ("FLTC", 0, "FTOC", 0., 0., 0., 0, "ONLY");
 
////////// Layers of Aluminum before and after detector //////////
////////// Aluminum Box for Modules (2.0 mm thickness)  /////////
////////// lateral walls not simulated
  par[0] = xFLT*0.5;
  par[1] = 0.1;//cm
  ycoor = -yFLT/2 + par[1];
  par[2] = (zFLTA *0.5);
  gMC->Gsvolu("FALA", "BOX ", idtmed[508], par, 3); // Alluminium
  gMC->Gspos ("FALA", 1, "FLTA", 0., ycoor, 0., 0, "ONLY");
  gMC->Gspos ("FALA", 2, "FLTA", 0.,-ycoor, 0., 0, "ONLY");
  par[2] = (zFLTB *0.5);
  gMC->Gsvolu("FALB", "BOX ", idtmed[508], par, 3); // Alluminium 
  gMC->Gspos ("FALB", 1, "FLTB", 0., ycoor, 0., 0, "ONLY");
  gMC->Gspos ("FALB", 2, "FLTB", 0.,-ycoor, 0., 0, "ONLY");
  par[2] = (zFLTC *0.5);
  gMC->Gsvolu("FALC", "BOX ", idtmed[508], par, 3); // Alluminium
  gMC->Gspos ("FALC", 1, "FLTC", 0., ycoor, 0., 0, "ONLY");
  gMC->Gspos ("FALC", 2, "FLTC", 0.,-ycoor, 0., 0, "ONLY");
  
///////////////// Detector itself //////////////////////
  const Float_t  kdeadBound  =  fDeadBndZ; //cm non-sensitive between the pad edge 
                                          //and the boundary of the strip
  const Int_t    knx    = fNpadX;          // number of pads along x
  const Int_t    knz    = fNpadZ;          // number of pads along z
  const Float_t  kspace = fSpace;            //cm distance from the front plate of the box

  Float_t zSenStrip  = fZpad*fNpadZ;//cm
  Float_t stripWidth = zSenStrip + 2*kdeadBound;
  par[0] = xFLT*0.5;
  par[1] = yPad*0.5; 
  par[2] = stripWidth*0.5;
  
// new description for strip volume -double stack strip-
// -- all constants are expressed in cm
// heigth of different layers
  const Float_t khhony = 1.      ;   // heigth of HONY  Layer
  const Float_t khpcby = 0.15    ;   // heigth of PCB   Layer
  const Float_t khmyly = 0.035   ;   // heigth of MYLAR Layer
  const Float_t khgraphy = 0.02  ;   // heigth of GRAPHITE Layer
  const Float_t khglasseiy = 0.17;   // 0.6 Ext. Glass + 1.1 i.e. (Int. Glass/2) (mm)
  const Float_t khsensmy = 0.11  ;   // heigth of Sensitive Freon Mixture
  const Float_t kwsensmz = 2*3.5 ;   // cm
  const Float_t klsensmx = 48*2.5;   // cm
  const Float_t kwpadz = 3.5;   // cm z dimension of the FPAD volume
  const Float_t klpadx = 2.5;   // cm x dimension of the FPAD volume
  
  // heigth of the FSTR Volume (the strip volume)
  const Float_t khstripy = 2*khhony+3*khpcby+4*(khmyly+khgraphy+khglasseiy)+2*khsensmy;
  // width  of the FSTR Volume (the strip volume)
  const Float_t kwstripz = 10.;
  // length of the FSTR Volume (the strip volume)
  const Float_t klstripx = 122.;
  
  Float_t parfp[3]={klstripx*0.5,khstripy*0.5,kwstripz*0.5};
// coordinates of the strip center in the strip reference frame; used for positioning
// internal strip volumes
  Float_t posfp[3]={0.,0.,0.};   

  
  // FSTR volume definition and filling this volume with non sensitive Gas Mixture
  gMC->Gsvolu("FSTR","BOX",idtmed[512],parfp,3);
  //-- HONY Layer definition
//  parfp[0] = -1;
  parfp[1] = khhony*0.5;
//  parfp[2] = -1;
  gMC->Gsvolu("FHON","BOX",idtmed[503],parfp,3);
  // positioning 2 HONY Layers on FSTR volume

  posfp[1]=-khstripy*0.5+parfp[1];
  gMC->Gspos("FHON",1,"FSTR",0., posfp[1],0.,0,"ONLY");
  gMC->Gspos("FHON",2,"FSTR",0.,-posfp[1],0.,0,"ONLY");

  //-- PCB Layer definition 
  parfp[1] = khpcby*0.5;
  gMC->Gsvolu("FPCB","BOX",idtmed[504],parfp,3);
  // positioning 2 PCB Layers on FSTR volume
  posfp[1]=-khstripy*0.5+khhony+parfp[1];
  gMC->Gspos("FPCB",1,"FSTR",0., posfp[1],0.,0,"ONLY");
  gMC->Gspos("FPCB",2,"FSTR",0.,-posfp[1],0.,0,"ONLY");
  // positioning the central PCB layer
  gMC->Gspos("FPCB",3,"FSTR",0.,0.,0.,0,"ONLY");



  //-- MYLAR Layer definition
  parfp[1] = khmyly*0.5;
  gMC->Gsvolu("FMYL","BOX",idtmed[511],parfp,3);
  // positioning 2 MYLAR Layers on FSTR volume
  posfp[1] = -khstripy*0.5+khhony+khpcby+parfp[1]; 
  gMC->Gspos("FMYL",1,"FSTR",0., posfp[1],0.,0,"ONLY");
  gMC->Gspos("FMYL",2,"FSTR",0.,-posfp[1],0.,0,"ONLY");
  // adding further 2 MYLAR Layers on FSTR volume
  posfp[1] = khpcby*0.5+parfp[1];
  gMC->Gspos("FMYL",3,"FSTR",0., posfp[1],0.,0,"ONLY");
  gMC->Gspos("FMYL",4,"FSTR",0.,-posfp[1],0.,0,"ONLY");


  //-- Graphite Layer definition
  parfp[1] = khgraphy*0.5;
  gMC->Gsvolu("FGRP","BOX",idtmed[502],parfp,3);
  // positioning 2 Graphite Layers on FSTR volume
  posfp[1] = -khstripy*0.5+khhony+khpcby+khmyly+parfp[1];
  gMC->Gspos("FGRP",1,"FSTR",0., posfp[1],0.,0,"ONLY");
  gMC->Gspos("FGRP",2,"FSTR",0.,-posfp[1],0.,0,"ONLY");
  // adding further 2 Graphite Layers on FSTR volume
  posfp[1] = khpcby*0.5+khmyly+parfp[1];
  gMC->Gspos("FGRP",3,"FSTR",0., posfp[1],0.,0,"ONLY");
  gMC->Gspos("FGRP",4,"FSTR",0.,-posfp[1],0.,0,"ONLY");
 

  //-- Glass (EXT. +Semi INT.) Layer definition
  parfp[1] = khglasseiy*0.5;
  gMC->Gsvolu("FGLA","BOX",idtmed[514],parfp,3);
  // positioning 2 Glass Layers on FSTR volume
  posfp[1] = -khstripy*0.5+khhony+khpcby+khmyly+khgraphy+parfp[1];
  gMC->Gspos("FGLA",1,"FSTR",0., posfp[1],0.,0,"ONLY");
  gMC->Gspos("FGLA",2,"FSTR",0.,-posfp[1],0.,0,"ONLY");
  // adding further 2 Glass Layers on FSTR volume
  posfp[1] = khpcby*0.5+khmyly+khgraphy+parfp[1];
  gMC->Gspos("FGLA",3,"FSTR",0., posfp[1],0.,0,"ONLY");
  gMC->Gspos("FGLA",4,"FSTR",0.,-posfp[1],0.,0,"ONLY");

  
  //-- Sensitive Mixture Layer definition
  parfp[0] = klsensmx*0.5;
  parfp[1] = khsensmy*0.5;
  parfp[2] = kwsensmz*0.5;
  gMC->Gsvolu("FSEN","BOX",idtmed[513],parfp,3);
  gMC->Gsvolu("FNSE","BOX",idtmed[512],parfp,3);
  // positioning 2 gas Layers on FSTR volume
  // the upper is insensitive freon
  // while the remaining is sensitive
  posfp[1] = khpcby*0.5+khmyly+khgraphy+khglasseiy+parfp[1];
  gMC->Gspos("FNSE",0,"FSTR", 0., posfp[1],0.,0,"ONLY");
  gMC->Gspos("FSEN",0,"FSTR", 0.,-posfp[1],0.,0,"ONLY");

  // dividing FSEN along z in knz=2 and along x in knx=48
  gMC->Gsdvn("FSEZ","FSEN",knz,3);
  gMC->Gsdvn("FSEX","FSEZ",knx,1);
  
  // FPAD volume definition
  parfp[0] = klpadx*0.5;    
  parfp[1] = khsensmy*0.5;
  parfp[2] = kwpadz*0.5;
  gMC->Gsvolu("FPAD","BOX",idtmed[513],parfp,3);
  // positioning the FPAD volumes on previous divisions
  gMC->Gspos("FPAD",0,"FSEX",0.,0.,0.,0,"ONLY");
  
////  Positioning the Strips  (FSTR) in the FLT volumes  /////

  // Plate A (Central) 
  
  Float_t t = zFLTC+zFLTB+zFLTA*0.5+ 2*db;//Half Width of Barrel

  Float_t gap  = fGapA+0.5; //cm  updated distance between the strip axis
  Float_t zpos = 0;
  Float_t ang  = 0;
  Int_t i=1,j=1;
  nrot  = 0;
  zcoor = 0;
  ycoor = -14.5 + kspace ; //2 cm over front plate

  AliMatrix (idrotm[0],  90.,  0.,90.,90.,0., 90.);   
  gMC->Gspos("FSTR",j,"FLTA",0.,ycoor, 0.,idrotm[0],"ONLY");

     printf("%f,  St. %2i, Pl.3 ",ang*kRaddeg,i); 
     printf("y = %f,  z = %f, zpos = %f \n",ycoor,zcoor,zpos);

  zcoor -= zSenStrip;
  j++;
  Int_t upDown = -1; // upDown=-1 -> Upper strip
                     // upDown=+1 -> Lower strip
  do{
     ang = atan(zcoor/radius);
     ang *= kRaddeg;
     AliMatrix (idrotm[nrot],  90.,  0.,90.-ang,90.,-ang, 90.);   
     AliMatrix (idrotm[nrot+1],90.,180.,90.+ang,90., ang, 90.);
     ang /= kRaddeg;
     ycoor = -14.5+ kspace; //2 cm over front plate
     ycoor += (1-(upDown+1)/2)*gap;
     gMC->Gspos("FSTR",j  ,"FLTA",0.,ycoor, zcoor,idrotm[nrot],  "ONLY");
     gMC->Gspos("FSTR",j+1,"FLTA",0.,ycoor,-zcoor,idrotm[nrot+1],"ONLY");

     printf("%f,  St. %2i, Pl.3 ",ang*kRaddeg,i); 
     printf("y = %f,  z = %f, zpos = %f \n",ycoor,zcoor,zpos);

     j += 2;
     upDown*= -1; // Alternate strips 
     zcoor = zcoor-(zSenStrip/2)/TMath::Cos(ang)-
             upDown*gap*TMath::Tan(ang)-
	     (zSenStrip/2)/TMath::Cos(ang);
  } while (zcoor-(stripWidth/2)*TMath::Cos(ang)>-t+zFLTC+zFLTB+db*2);
  
  zcoor = zcoor+(zSenStrip/2)/TMath::Cos(ang)+
          upDown*gap*TMath::Tan(ang)+
          (zSenStrip/2)/TMath::Cos(ang);

  gap = fGapB;
  zcoor = zcoor-(zSenStrip/2)/TMath::Cos(ang)-
          upDown*gap*TMath::Tan(ang)-
          (zSenStrip/2)/TMath::Cos(ang);

  ang = atan(zcoor/radius);
  ang *= kRaddeg;
  AliMatrix (idrotm[nrot],  90.,  0.,90.-ang,90.,-ang, 90.);   
  AliMatrix (idrotm[nrot+1],90.,180.,90.+ang,90., ang, 90.);
  ang /= kRaddeg;
	  
  ycoor = -14.5+ kspace; //2 cm over front plate
  ycoor += (1-(upDown+1)/2)*gap;
  gMC->Gspos("FSTR",j  ,"FLTA",0.,ycoor, zcoor,idrotm[nrot],  "ONLY");
  gMC->Gspos("FSTR",j+1,"FLTA",0.,ycoor,-zcoor,idrotm[nrot+1],"ONLY");
     printf("%f,  St. %2i, Pl.3 ",ang*kRaddeg,i); 
     printf("y = %f,  z = %f, zpos = %f \n",ycoor,zcoor,zpos);

  ycoor = -hTof/2.+ kspace;//2 cm over front plate

  // Plate  B

  nrot = 0;
  i=1;
  upDown = 1;
  Float_t deadRegion = 1.0;//cm
  
  zpos = zcoor - (zSenStrip/2)/TMath::Cos(ang)-
         upDown*gap*TMath::Tan(ang)-
	 (zSenStrip/2)/TMath::Cos(ang)-
	 deadRegion/TMath::Cos(ang);

  ang = atan(zpos/radius);
  ang *= kRaddeg;
  AliMatrix (idrotm[nrot], 90., 0., 90.-ang,90.,ang, 270.);
  ang /= kRaddeg;
  ycoor = -hTof*0.5+ kspace ; //2 cm over front plate
  ycoor += (1-(upDown+1)/2)*gap;
  zcoor = zpos+(zFLTA*0.5+zFLTB*0.5+db); // Moves to the system of the modulus FLTB
  gMC->Gspos("FSTR",i, "FLTB", 0., ycoor, zcoor,idrotm[nrot], "ONLY");

     printf("%f,  St. %2i, Pl.4 ",ang*kRaddeg,i); 
     printf("y = %f,  z = %f, zpos = %f \n",ycoor,zcoor,zpos);

  i++;
  upDown*=-1;

  do {
     zpos = zpos - (zSenStrip/2)/TMath::Cos(ang)-
            upDown*gap*TMath::Tan(ang)-
	    (zSenStrip/2)/TMath::Cos(ang);
     ang = atan(zpos/radius);
     ang *= kRaddeg;
     AliMatrix (idrotm[nrot], 90., 0., 90.-ang,90.,ang, 270.);
     ang /= kRaddeg;
     Float_t deltaSpaceinB=-0.5; // [cm] to avoid overlaps with the end of freon frame
     Float_t deltaGapinB=0.5;    // [cm] to avoid overlaps in between initial strips
     ycoor = -hTof*0.5+ kspace+deltaSpaceinB ; //2 cm over front plate
     ycoor += (1-(upDown+1)/2)*(gap+deltaGapinB);
     zcoor = zpos+(zFLTA*0.5+zFLTB*0.5+db); // Moves to the system of the modulus FLTB
     gMC->Gspos("FSTR",i, "FLTB", 0., ycoor, zcoor,idrotm[nrot], "ONLY");

     printf("%f,  St. %2i, Pl.4 ",ang*kRaddeg,i); 
     printf("y = %f,  z = %f, zpos = %f \n",ycoor,zcoor,zpos);

     upDown*=-1;
     i++;
  } while (TMath::Abs(ang*kRaddeg)<22.5);
  //till we reach a tilting angle of 22.5 degrees

  ycoor = -hTof*0.5+ kspace ; //2 cm over front plate
  zpos = zpos - zSenStrip/TMath::Cos(ang);
  // this avoid overlaps in between outer strips in plate B
  Float_t deltaMovingUp=0.8;    // [cm]
  Float_t deltaMovingDown=-0.5; // [cm]

  do {
     ang = atan(zpos/radius);
     ang *= kRaddeg;
     AliMatrix (idrotm[nrot], 90., 0., 90.-ang,90.,ang, 270.);
     ang /= kRaddeg;
     zcoor = zpos+(zFLTB/2+zFLTA/2+db);
     gMC->Gspos("FSTR",i, "FLTB", 0., ycoor+deltaMovingDown+deltaMovingUp, zcoor,idrotm[nrot], "ONLY");
     deltaMovingUp+=0.8; // update delta moving toward the end of the plate
     zpos = zpos - zSenStrip/TMath::Cos(ang);
     printf("%f,  St. %2i, Pl.4 ",ang*kRaddeg,i); 
     printf("y = %f,  z = %f, zpos = %f \n",ycoor,zcoor,zpos);
     i++;
  }  while (zpos-stripWidth*0.5/TMath::Cos(ang)>-t+zFLTC+db);

  // Plate  C
  
  zpos = zpos + zSenStrip/TMath::Cos(ang);

  zpos = zpos - (zSenStrip/2)/TMath::Cos(ang)+
         gap*TMath::Tan(ang)-
	 (zSenStrip/2)/TMath::Cos(ang);

  nrot = 0;
  i=0;
  Float_t deltaGap=-2.5; // [cm] update distance from strip center and plate
  ycoor= -hTof*0.5+kspace+gap+deltaGap;

  do {
     i++;
     ang = atan(zpos/radius);
     ang *= kRaddeg;
     AliMatrix (idrotm[nrot], 90., 0., 90.-ang,90.,ang, 270.);
     ang /= kRaddeg;
     zcoor = zpos+(zFLTC*0.5+zFLTB+zFLTA*0.5+db*2);
     gMC->Gspos("FSTR",i, "FLTC", 0., ycoor, zcoor,idrotm[nrot], "ONLY");

     printf("%f,  St. %2i, Pl.5 ",ang*kRaddeg,i); 
     printf("y = %f,  z = %f, zpos = %f \n",ycoor,zcoor,zpos);

     zpos = zpos - zSenStrip/TMath::Cos(ang);
  }  while (zpos-stripWidth*TMath::Cos(ang)*0.5>-t);


////////// Layers after strips /////////////////
// honeycomb (Polyethilene) Layer after (1.2cm)

  Float_t overSpace = fOverSpc;//cm

  par[0] = xFLT*0.5;
  par[1] = 0.6;
  par[2] = (zFLTA *0.5);
  ycoor = -yFLT/2 + overSpace + par[1];
  gMC->Gsvolu("FPEA", "BOX ", idtmed[503], par, 3); // Hony
  gMC->Gspos ("FPEA", 0, "FLTA", 0., ycoor, 0., 0, "ONLY");
  par[2] = (zFLTB *0.5);
  gMC->Gsvolu("FPEB", "BOX ", idtmed[503], par, 3); // Hony
  gMC->Gspos ("FPEB", 0, "FLTB", 0., ycoor, 0., 0, "ONLY");
  par[2] = (zFLTC *0.5);
  gMC->Gsvolu("FPEC", "BOX ", idtmed[503], par, 3); // Hony
  gMC->Gspos ("FPEC", 0, "FLTC", 0., ycoor, 0., 0, "ONLY");

// Electronics (Cu) after
  ycoor += par[1];
  par[0] = xFLT*0.5;
  par[1] = 1.43*0.05*0.5; // 5% of X0
  par[2] = (zFLTA *0.5);
  ycoor += par[1];
  gMC->Gsvolu("FECA", "BOX ", idtmed[501], par, 3); // Cu
  gMC->Gspos ("FECA", 0, "FLTA", 0., ycoor, 0., 0, "ONLY");
  par[2] = (zFLTB *0.5);
  gMC->Gsvolu("FECB", "BOX ", idtmed[501], par, 3); // Cu
  gMC->Gspos ("FECB", 0, "FLTB", 0., ycoor, 0., 0, "ONLY");
  par[2] = (zFLTC *0.5);
  gMC->Gsvolu("FECC", "BOX ", idtmed[501], par, 3); // Cu
  gMC->Gspos ("FECC", 0, "FLTC", 0., ycoor, 0., 0, "ONLY");

// cooling WAter after
  ycoor += par[1];
  par[0] = xFLT*0.5;
  par[1] = 36.1*0.02*0.5; // 2% of X0
  par[2] = (zFLTA *0.5);
  ycoor += par[1];
  gMC->Gsvolu("FWAA", "BOX ", idtmed[515], par, 3); // Water
  gMC->Gspos ("FWAA", 0, "FLTA", 0., ycoor, 0., 0, "ONLY");
  par[2] = (zFLTB *0.5);
  gMC->Gsvolu("FWAB", "BOX ", idtmed[515], par, 3); // Water
  gMC->Gspos ("FWAB", 0, "FLTB", 0., ycoor, 0., 0, "ONLY");
  par[2] = (zFLTC *0.5);
  gMC->Gsvolu("FWAC", "BOX ", idtmed[515], par, 3); // Water
  gMC->Gspos ("FWAC", 0, "FLTC", 0., ycoor, 0., 0, "ONLY");

// frame of Air
  ycoor += par[1];
  par[0] = xFLT*0.5;
  par[1] = (yFLT/2-ycoor-0.2)*0.5; // Aluminum layer considered (0.2 cm)
  par[2] = (zFLTA *0.5);
  ycoor += par[1];
  gMC->Gsvolu("FAIA", "BOX ", idtmed[500], par, 3); // Air
  gMC->Gspos ("FAIA", 0, "FLTA", 0., ycoor, 0., 0, "ONLY");
  par[2] = (zFLTB *0.5);
  gMC->Gsvolu("FAIB", "BOX ", idtmed[500], par, 3); // Air
  gMC->Gspos ("FAIB", 0, "FLTB", 0., ycoor, 0., 0, "ONLY");
  par[2] = (zFLTC *0.5);
  gMC->Gsvolu("FAIC", "BOX ", idtmed[500], par, 3); // Air
  gMC->Gspos ("FAIC", 0, "FLTC", 0., ycoor, 0., 0, "ONLY");
/* fp
//Back Plate honycomb (2cm)
  par[0] = -1;
  par[1] = 2 *0.5;
  par[2] = -1;
  ycoor = yFLT/2 - par[1];
  gMC->Gsvolu("FBPA", "BOX ", idtmed[503], par, 3); // Hony
  gMC->Gspos ("FBPA", 0, "FLTA", 0., ycoor, 0., 0, "ONLY");
  gMC->Gsvolu("FBPB", "BOX ", idtmed[503], par, 3); // Hony
  gMC->Gspos ("FBPB", 0, "FLTB", 0., ycoor, 0., 0, "ONLY");
  gMC->Gsvolu("FBPC", "BOX ", idtmed[503], par, 3); // Hony
  gMC->Gspos ("FBPC", 0, "FLTC", 0., ycoor, 0., 0, "ONLY");
fp */
}

//_____________________________________________________________________________
void AliTOFv0::DrawModule() const
{
  //
  // Draw a shaded view of the Time Of Flight version 0
  //
  // Set everything unseen
  gMC->Gsatt("*", "seen", -1);
  // 
  // Set ALIC mother transparent
  gMC->Gsatt("ALIC","SEEN",0);
  //
  // Set the volumes visible
  gMC->Gsatt("ALIC","SEEN",0);

  gMC->Gsatt("FTOA","SEEN",1);
  gMC->Gsatt("FTOB","SEEN",1);
  gMC->Gsatt("FTOC","SEEN",1);
  gMC->Gsatt("FLTA","SEEN",1);
  gMC->Gsatt("FLTB","SEEN",1);
  gMC->Gsatt("FLTC","SEEN",1);
  gMC->Gsatt("FPLA","SEEN",1);
  gMC->Gsatt("FPLB","SEEN",1);
  gMC->Gsatt("FPLC","SEEN",1);
  gMC->Gsatt("FSTR","SEEN",1);
  gMC->Gsatt("FPEA","SEEN",1);
  gMC->Gsatt("FPEB","SEEN",1);
  gMC->Gsatt("FPEC","SEEN",1);
  
  gMC->Gsatt("FLZ1","SEEN",0);
  gMC->Gsatt("FLZ2","SEEN",0);
  gMC->Gsatt("FLZ3","SEEN",0);
  gMC->Gsatt("FLX1","SEEN",0);
  gMC->Gsatt("FLX2","SEEN",0);
  gMC->Gsatt("FLX3","SEEN",0);
  gMC->Gsatt("FPAD","SEEN",0);

  gMC->Gdopt("hide", "on");
  gMC->Gdopt("shad", "on");
  gMC->Gsatt("*", "fill", 7);
  gMC->SetClipBox(".");
  gMC->SetClipBox("*", 0, 1000, -1000, 1000, -1000, 1000);
  gMC->DefaultRange();
  gMC->Gdraw("alic", 40, 30, 0, 12, 9.5, .02, .02);
  gMC->Gdhead(1111, "Time Of Flight");
  gMC->Gdman(18, 4, "MAN");
  gMC->Gdopt("hide","off");
}
//_____________________________________________________________________________
void AliTOFv0::DrawDetectorModules()
{
//
// Draw a shaded view of the TOF detector version 0
//
 
 AliMC* pMC = AliMC::GetMC();
 
//Set ALIC mother transparent
 pMC->Gsatt("ALIC","SEEN",0);

//
//Set volumes visible
// 
//=====> Level 1
  // Level 1 for TOF volumes
  gMC->Gsatt("B077","seen",0);
 
 
//==========> Level 2
  // Level 2
  gMC->Gsatt("B076","seen",-1); // all B076 sub-levels skipped -
  gMC->Gsatt("B071","seen",0);
  gMC->Gsatt("B074","seen",0);
  gMC->Gsatt("B075","seen",0);
  gMC->Gsatt("B080","seen",0); // B080 does not has sub-level                


  // Level 2 of B071
  gMC->Gsatt("B063","seen",-1); // all B063 sub-levels skipped   -
  gMC->Gsatt("B065","seen",-1); // all B065 sub-levels skipped   -
  gMC->Gsatt("B067","seen",-1); // all B067 sub-levels skipped   -
  gMC->Gsatt("B069","seen",-1); // all B069 sub-levels skipped   -
  gMC->Gsatt("B056","seen",0);  // B056 does not has sub-levels  -
  gMC->Gsatt("B059","seen",-1); // all B059 sub-levels skipped   -
  gMC->Gsatt("B072","seen",-1); // all B072 sub-levels skipped   -
  gMC->Gsatt("BTR1","seen",0);  // BTR1 do not have sub-levels   -
  gMC->Gsatt("BTO1","seen",0);

 
  // Level 2 of B074
  gMC->Gsatt("BTR2","seen",0); // BTR2 does not has sub-levels -
  gMC->Gsatt("BTO2","seen",0);

  // Level 2 of B075
  gMC->Gsatt("BTR3","seen",0); // BTR3 do not have sub-levels -
  gMC->Gsatt("BTO3","seen",0);

// ==================> Level 3
  // Level 3 of B071 / Level 2 of BTO1
  gMC->Gsatt("FTOC","seen",-2);
  gMC->Gsatt("FTOB","seen",-2);
  gMC->Gsatt("FTOA","seen",-2);
 
  // Level 3 of B074 / Level 2 of BTO2
  // -> cfr previous settings
 
  // Level 3 of B075 / Level 2 of BTO3
  // -> cfr previous settings

  gMC->Gdopt("hide","on");
  gMC->Gdopt("shad","on");
  gMC->Gsatt("*", "fill", 5);
  gMC->SetClipBox(".");
  gMC->SetClipBox("*", 0, 1000, 0, 1000, 0, 1000);
  gMC->DefaultRange();
  gMC->Gdraw("alic", 45, 40, 0, 10, 10, .015, .015);
  gMC->Gdhead(1111,"TOF detector V1");
  gMC->Gdman(18, 4, "MAN");
  gMC->Gdopt("hide","off");
}                                 

//_____________________________________________________________________________
void AliTOFv0::DrawDetectorStrips()
{
//
// Draw a shaded view of the TOF strips for version 0
//
 
 AliMC* pMC = AliMC::GetMC();
 
//Set ALIC mother transparent
 pMC->Gsatt("ALIC","SEEN",0);

//
//Set volumes visible 
//=====> Level 1
  // Level 1 for TOF volumes
  gMC->Gsatt("B077","seen",0);
  
//==========> Level 2
  // Level 2
  gMC->Gsatt("B076","seen",-1); // all B076 sub-levels skipped -
  gMC->Gsatt("B071","seen",0);
  gMC->Gsatt("B074","seen",0);
  gMC->Gsatt("B075","seen",0);
  gMC->Gsatt("B080","seen",0); // B080 does not has sub-level

  // Level 2 of B071
  gMC->Gsatt("B063","seen",-1); // all B063 sub-levels skipped   -
  gMC->Gsatt("B065","seen",-1); // all B065 sub-levels skipped   -
  gMC->Gsatt("B067","seen",-1); // all B067 sub-levels skipped   -
  gMC->Gsatt("B069","seen",-1); // all B069 sub-levels skipped   -
  gMC->Gsatt("B056","seen",0);  // B056 does not has sub-levels  -
  gMC->Gsatt("B059","seen",-1); // all B059 sub-levels skipped   -
  gMC->Gsatt("B072","seen",-1); // all B072 sub-levels skipped   -
  gMC->Gsatt("BTR1","seen",0);  // BTR1 do not have sub-levels   -
  gMC->Gsatt("BTO1","seen",0);

// ==================> Level 3
  // Level 3 of B071 / Level 2 of BTO1
  gMC->Gsatt("FTOC","seen",0);
  gMC->Gsatt("FTOB","seen",0);
  gMC->Gsatt("FTOA","seen",0);
 
  // Level 3 of B074 / Level 2 of BTO2
  // -> cfr previous settings
 
  // Level 3 of B075 / Level 2 of BTO3
  // -> cfr previous settings


// ==========================> Level 4
  // Level 4 of B071 / Level 3 of BTO1 / Level 2 of FTOC
  gMC->Gsatt("FLTC","seen",0);
  // Level 4 of B071 / Level 3 of BTO1 / Level 2 of FTOB
  gMC->Gsatt("FLTB","seen",0);
  // Level 4 of B071 / Level 3 of BTO1 / Level 2 of FTOA
  gMC->Gsatt("FLTA","seen",0);
 
  // Level 4 of B074 / Level 3 of BTO2 / Level 2 of FTOC
  // -> cfr previous settings
  // Level 4 of B074 / Level 3 of BTO2 / Level 2 of FTOB
  // -> cfr previous settings
 
  // Level 4 of B075 / Level 3 of BTO3 / Level 2 of FTOC
  // -> cfr previous settings

//======================================> Level 5
  // Level 5 of B071 / Level 4 of BTO1 / Level 3 of FTOC / Level 2 of FLTC
  gMC->Gsatt("FALC","seen",0); // no children for FALC
  gMC->Gsatt("FSTR","seen",-2);
  gMC->Gsatt("FPEC","seen",0); // no children for FPEC
  gMC->Gsatt("FECC","seen",0); // no children for FECC
  gMC->Gsatt("FWAC","seen",0); // no children for FWAC
  gMC->Gsatt("FAIC","seen",0); // no children for FAIC

  // Level 5 of B071 / Level 4 of BTO1 / Level 3 of FTOB / Level 2 of FLTB
  gMC->Gsatt("FALB","seen",0); // no children for FALB
//-->  gMC->Gsatt("FSTR","seen",-2);


  // -> cfr previous settings
  gMC->Gsatt("FPEB","seen",0); // no children for FPEB
  gMC->Gsatt("FECB","seen",0); // no children for FECB
  gMC->Gsatt("FWAB","seen",0); // no children for FWAB
  gMC->Gsatt("FAIB","seen",0); // no children for FAIB
 
  // Level 5 of B071 / Level 4 of BTO1 / Level 3 of FTOA / Level 2 of FLTA
  gMC->Gsatt("FALA","seen",0); // no children for FALB
//-->  gMC->Gsatt("FSTR","seen",-2);
  // -> cfr previous settings
  gMC->Gsatt("FPEA","seen",0); // no children for FPEA
  gMC->Gsatt("FECA","seen",0); // no children for FECA
  gMC->Gsatt("FWAA","seen",0); // no children for FWAA
  gMC->Gsatt("FAIA","seen",0); // no children for FAIA

  // Level 2 of B074
  gMC->Gsatt("BTR2","seen",0); // BTR2 does not has sub-levels -
  gMC->Gsatt("BTO2","seen",0);

  // Level 2 of B075
  gMC->Gsatt("BTR3","seen",0); // BTR3 do not have sub-levels -
  gMC->Gsatt("BTO3","seen",0);

// for others Level 5, cfr. previous settings

  gMC->Gdopt("hide","on");
  gMC->Gdopt("shad","on");
  gMC->Gsatt("*", "fill", 5);
  gMC->SetClipBox(".");
  gMC->SetClipBox("*", 0, 1000, 0, 1000, 0, 1000);
  gMC->DefaultRange();
  gMC->Gdraw("alic", 45, 40, 0, 10, 10, .015, .015);
  gMC->Gdhead(1111,"TOF Strips V1");
  gMC->Gdman(18, 4, "MAN");
  gMC->Gdopt("hide","off");
}

//_____________________________________________________________________________
void AliTOFv0::CreateMaterials()
{
  //
  // Define materials for the Time Of Flight
  //
  AliTOF::CreateMaterials();
}
 
//_____________________________________________________________________________
void AliTOFv0::Init()
{
  //
  // Initialise the detector after the geometry has been defined
  //
  printf("**************************************"
	 "  TOF  "
	 "**************************************\n");
  printf("\n   Version 0 of TOF initialing, "
	      "symmetric TOF - Full Coverage version\n");

  AliTOF::Init();

  fIdFTOA = gMC->VolId("FTOA");
  fIdFTOB = gMC->VolId("FTOB");
  fIdFTOC = gMC->VolId("FTOC");
  fIdFLTA = gMC->VolId("FLTA");
  fIdFLTB = gMC->VolId("FLTB");
  fIdFLTC = gMC->VolId("FLTC");

  if(fDebug) {
    printf("%s: **************************************"
	   "  TOF  "
	   "**************************************\n",ClassName());
  }
}
 
//_____________________________________________________________________________
void AliTOFv0::StepManager()
{
  //
  // Procedure called at each step in the Time Of Flight
  //
  TLorentzVector mom, pos;
  Float_t xm[3],pm[3],xpad[3],ppad[3];
  Float_t hits[13],phi,phid,z;
  Int_t   vol[5];
  Int_t   sector, plate, padx, padz, strip;
  Int_t   copy, padzid, padxid, stripid, i;
  Int_t   *idtmed = fIdtmed->GetArray()-499;
  Float_t incidenceAngle;
  
  if(gMC->GetMedium()==idtmed[513] && 
     gMC->IsTrackEntering() && gMC->TrackCharge()
     && gMC->CurrentVolID(copy)==fIdSens) 
  {    
    // getting information about hit volumes
    
    padzid=gMC->CurrentVolOffID(2,copy);
    padz=copy;  
    
    padxid=gMC->CurrentVolOffID(1,copy);
    padx=copy;  
    
    stripid=gMC->CurrentVolOffID(4,copy);
    strip=copy;  

    gMC->TrackPosition(pos);
    gMC->TrackMomentum(mom);

//    Double_t NormPos=1./pos.Rho();
    Double_t normMom=1./mom.Rho();

//  getting the cohordinates in pad ref system
    xm[0] = (Float_t)pos.X();
    xm[1] = (Float_t)pos.Y();
    xm[2] = (Float_t)pos.Z();

    pm[0] = (Float_t)mom.X()*normMom;
    pm[1] = (Float_t)mom.Y()*normMom;
    pm[2] = (Float_t)mom.Z()*normMom;
 
    gMC->Gmtod(xm,xpad,1);
    gMC->Gmtod(pm,ppad,2);

    incidenceAngle = TMath::ACos(ppad[1])*kRaddeg;

    z = pos[2];

    plate = 0;   
    if (TMath::Abs(z) <=  fZlenA*0.5)  plate = 3;
    if (z < (fZlenA*0.5+fZlenB) && 
        z >  fZlenA*0.5)               plate = 4;
    if (z >-(fZlenA*0.5+fZlenB) &&
        z < -fZlenA*0.5)               plate = 2;
    if (z > (fZlenA*0.5+fZlenB))       plate = 5;
    if (z <-(fZlenA*0.5+fZlenB))       plate = 1;

    phi = pos.Phi();
    phid = phi*kRaddeg+180.;
    sector = Int_t (phid/20.);
    sector++;

    for(i=0;i<3;++i) {
      hits[i]   = pos[i];
      hits[i+3] = pm[i];
    }

    hits[6] = mom.Rho();
    hits[7] = pos[3];
    hits[8] = xpad[0];
    hits[9] = xpad[1];
    hits[10]= xpad[2];
    hits[11]= incidenceAngle;
    hits[12]= gMC->Edep();
    
    vol[0]= sector;
    vol[1]= plate;
    vol[2]= strip;
    vol[3]= padx;
    vol[4]= padz;

    AddHit(gAlice->CurrentTrack(),vol, hits);
  }
}







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

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  Inner Traking System version 1                                           //
//  This class contains the base procedures for the Inner Tracking System    //
//                                                                           //
// Authors: R. Barbera, A. Morsch.
// version 1.
// Created  1998.
//
//  NOTE: THIS IS THE COARSE pre.TDR geometry of the ITS. THIS WILL NOT WORK
// with the geometry or module classes or any analysis classes. You are 
// strongly encouraged to uses AliITSv5.
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
 
#include <TCanvas.h>
#include <TClonesArray.h>
#include <TFile.h>    // only required for Tracking function?
#include <TGeometry.h>
#include <TMath.h>
#include <TNode.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TRandom.h>
#include <TTUBE.h>
#include <TVector.h>
#include <TVirtualMC.h>

#include "AliConst.h"
#include "AliITShit.h"
#include "AliITSv1.h"
#include "AliMagF.h"
#include "AliRun.h"

ClassImp(AliITSv1)
 
//_____________________________________________________________________________
AliITSv1::AliITSv1() {
////////////////////////////////////////////////////////////////////////
//    Standard default constructor for the ITS version 1.
////////////////////////////////////////////////////////////////////////

  fIdN    = 0;
  fIdName = 0;
  fIdSens = 0;
  fMajorVersion = 1;
  fMinorVersion = -1;
}
//_____________________________________________________________________________
AliITSv1::AliITSv1(const char *name, const char *title) : AliITS(name, title){
////////////////////////////////////////////////////////////////////////
//    Standard constructor for the ITS version 1.
////////////////////////////////////////////////////////////////////////

    fIdN    = 6;
/*
//  TObjArray of TObjStrings
    fIdName = new TObjArray(fIdN);
    fIdName->AddAt(new TObjString("ITS1"),0);
    fIdName->AddAt(new TObjString("ITS2"),1);
    fIdName->AddAt(new TObjString("ITS3"),2);
    fIdName->AddAt(new TObjString("ITS4"),3);
    fIdName->AddAt(new TObjString("ITS5"),4);
    fIdName->AddAt(new TObjString("ITS6"),5);
*/
//  Array of TStrings.
    fIdName    = new TString[fIdN];
    fIdName[0] = "ITS1";
    fIdName[1] = "ITS2";
    fIdName[2] = "ITS3";
    fIdName[3] = "ITS4";
    fIdName[4] = "ITS5";
    fIdName[5] = "ITS6";
    fIdSens    = new Int_t[fIdN];
    for (Int_t i=0;i<fIdN;i++) fIdSens[i] = 0;
    fMajorVersion = 1;
    fMinorVersion = 1;

}
//____________________________________________________________________________
AliITSv1::AliITSv1(const AliITSv1 &source){
////////////////////////////////////////////////////////////////////////
//     Copy Constructor for ITS version 1.
////////////////////////////////////////////////////////////////////////
    if(&source == this) return;
    printf("Not allowed to copy AliITSv1\n");
    return;
}
//_____________________________________________________________________________
AliITSv1& AliITSv1::operator=(const AliITSv1 &source){
////////////////////////////////////////////////////////////////////////
//    Assignment operator for the ITS version 1.
////////////////////////////////////////////////////////////////////////
  if(&source == this) return *this;
    printf("Not allowed to copy AliITSv1\n");
  return *this;
}
//_____________________________________________________________________________
AliITSv1::~AliITSv1() {
////////////////////////////////////////////////////////////////////////
//    Standard destructor for the ITS version 1.
////////////////////////////////////////////////////////////////////////
}

//__________________________________________________________________________
void AliITSv1::BuildGeometry(){
////////////////////////////////////////////////////////////////////////
//    Geometry builder for the ITS version 1.
////////////////////////////////////////////////////////////////////////
    TNode *node, *top;
    const int kColorITS=kYellow;
    //
    top = gAlice->GetGeometry()->GetNode("alice");

    new TTUBE("S_layer1","Layer1 of ITS","void",3.9,3.9+0.05475,12.25);
    top->cd();
    node = new TNode("Layer1","Layer1","S_layer1",0,0,0,"");
    node->SetLineColor(kColorITS);
    fNodes->Add(node);

    new TTUBE("S_layer2","Layer2 of ITS","void",7.6,7.6+0.05475,16.3);
    top->cd();
    node = new TNode("Layer2","Layer2","S_layer2",0,0,0,"");
    node->SetLineColor(kColorITS);
    fNodes->Add(node);

    new TTUBE("S_layer3","Layer3 of ITS","void",14,14+0.05288,21.1);
    top->cd();
    node = new TNode("Layer3","Layer3","S_layer3",0,0,0,"");
    node->SetLineColor(kColorITS);
    fNodes->Add(node);

    new TTUBE("S_layer4","Layer4 of ITS","void",24,24+0.05288,29.6);
    top->cd();
    node = new TNode("Layer4","Layer4","S_layer4",0,0,0,"");
    node->SetLineColor(kColorITS);
    fNodes->Add(node);

    new TTUBE("S_layer5","Layer5 of ITS","void",40,40+0.05382,45.1);
    top->cd();
    node = new TNode("Layer5","Layer5","S_layer5",0,0,0,"");
    node->SetLineColor(kColorITS);
    fNodes->Add(node);

    new TTUBE("S_layer6","Layer6 of ITS","void",45,45+0.05382,50.4);
    top->cd();
    node = new TNode("Layer6","Layer6","S_layer6",0,0,0,"");
    node->SetLineColor(kColorITS);
    fNodes->Add(node);
}
//_____________________________________________________________________________
void AliITSv1::CreateGeometry(){
////////////////////////////////////////////////////////////////////////
//    This routine defines and Creates the geometry for version 1 of the ITS.
////////////////////////////////////////////////////////////////////////
  
  Float_t drcer[6] = { 0.,0.,.08,.08,0.,0. };           //CERAMICS THICKNESS
  Float_t drepx[6] = { 0.,0.,0.,0.,.5357,.5357 };       //EPOXY THICKNESS
  Float_t drpla[6] = { 0.,0.,0.,0.,.1786,.1786 };       //PLASTIC THICKNESS
  Float_t dzb[6]   = { 0.,0.,15.,15.,4.,4. };           //LENGTH OF BOXES
  Float_t dphi[6]  = { 72.,72.,72.,72.,50.6,45. };      //COVERED PHI-RANGE FOR LAYERS 1-6
  Float_t rl[6]    = { 3.9,7.6,14.,24.,40.,45. };       //SILICON LAYERS INNER RADIUS
  Float_t drl[6]   = { .755,.755,.809,.809,.7,.7 };     //THICKNESS OF LAYERS (in % radiation length)
  Float_t dzl[6]   = { 12.67,16.91,20.85,29.15,45.11,50.975 };//HALF LENGTH OF LAYERS
  Float_t drpcb[6] = { 0.,0.,.06,.06,0.,0. };           //PCB THICKNESS
  Float_t drcu[6]  = { 0.,0.,.0504,.0504,.0357,.0357 }; //COPPER THICKNESS
  Float_t drsi[6]  = { 0.,0.,.006,.006,.3571,.3571 };   //SILICON THICKNESS

  Float_t drca, dzfc;
  Int_t i, nsec;
  Float_t rend, drcatpc, dzco, zend, dits[3], rlim, drsu, zmax;
  Float_t zpos, dzco1, dzco2;
  Float_t drcac[6], acone, dphii;
  Float_t pcits[15], xltpc;
  Float_t rzcone, rstep, r0, z0, acable, fp, dz, zi, ri;
  Int_t idrotm[399];
  Float_t dgh[15];
  
  Int_t *idtmed = fIdtmed->GetArray()-199;
  
  //     CONVERT INTO CM (RL(SI)=9.36 CM) 
  for (i = 0; i < 6; ++i) {
    drl[i] = drl[i] / 100. * 9.36;
  }
  
  //     SUPPORT ENDPLANE THICKNESS 
  drsu = 2.*0.06+1./20;  // 1./20. is 1 cm of honeycomb (1/20 carbon density);
  
  //     CONE BELOW TPC 
  
  drcatpc = 1.2/4.;
  
  //     CABLE THICKNESS (CONICAL CABLES CONNECTING THE LAYERS) 

  drca = 0.2;
  
  //     ITS CONE ANGLE 
  
  acone  = 45.;
  acone *= kDegrad;
  
  //     CONE RADIUS AT 1ST LAYER 
  
  rzcone = 30.;
  
  //     FIELD CAGE HALF LENGTH 
  
  dzfc  = 64.5;
  rlim  = 48.;
  zmax  = 80.;
  xltpc = 275.;
  
  
  //     PARAMETERS FOR SMALL (1/2) ITS 
/*
  for (i = 0; i < 6; ++i) {
    dzl[i] /= 2.;
    dzb[i] /= 2.;
  }
  drca     /= 2.;
  acone    /= 2.;
  drcatpc /= 2.;
  rzcone   /= 2.;
  dzfc     /= 2.;
  zmax     /= 2.;
  xltpc    /= 2.;
*/
  acable    = 15.;  
  
  
  //     EQUAL DISTRIBUTION INTO THE 6 LAYERS 
  rstep = drcatpc / 6.;
  for (i = 0; i < 6; ++i) {
    drcac[i] = (i+1) * rstep;
  }

  //     NUMBER OF PHI SECTORS 
  
  nsec = 5;
  
  //     PACK IN PHI AS MUCH AS POSSIBLE 
  //     NOW PACK USING THICKNESS 
  
  for (i = 0; i < 6; ++i) {
    
//     PACKING FACTOR 
    fp = rl[5] / rl[i];
    
    //      PHI-PACKING NOT SUFFICIENT ? 
    
    if (dphi[i]/45 < fp) {
      drcac[i] = drcac[i] * fp * 45/dphi[i];
    }
  }
  
  
  // --- Define ghost volume containing the six layers and fill it with air 
  
  dgh[0] = 3.5;
  dgh[1] = 50.;
  dgh[2] = zmax;
  gMC->Gsvolu("ITSV", "TUBE", idtmed[275], dgh, 3);
  
  // --- Place the ghost volume in its mother volume (ALIC) and make it 
  //     invisible 
  
  gMC->Gspos("ITSV", 1, "ALIC", 0., 0., 0., 0, "ONLY");
  gMC->Gsatt("ITSV", "SEEN", 0);
  
  //     ITS LAYERS (SILICON) 
  
  dits[0] = rl[0];
  dits[1] = rl[0] + drl[0];
  dits[2] = dzl[0];
  gMC->Gsvolu("ITS1", "TUBE", idtmed[199], dits, 3);
  gMC->Gspos("ITS1", 1, "ITSV", 0., 0., 0., 0, "ONLY");
  
  dits[0] = rl[1];
  dits[1] = rl[1] + drl[1];
  dits[2] = dzl[1];
  gMC->Gsvolu("ITS2", "TUBE", idtmed[199], dits, 3);
  gMC->Gspos("ITS2", 1, "ITSV", 0., 0., 0., 0, "ONLY");
  
  dits[0] = rl[2];
  dits[1] = rl[2] + drl[2];
  dits[2] = dzl[2];
  gMC->Gsvolu("ITS3", "TUBE", idtmed[224], dits, 3);
  gMC->Gspos("ITS3", 1, "ITSV", 0., 0., 0., 0, "ONLY");
  
  dits[0] = rl[3];
  dits[1] = rl[3] + drl[3];
  dits[2] = dzl[3];
  gMC->Gsvolu("ITS4", "TUBE", idtmed[224], dits, 3);
  gMC->Gspos("ITS4", 1, "ITSV", 0., 0., 0., 0, "ONLY");
  
  dits[0] = rl[4];
  dits[1] = rl[4] + drl[4];
  dits[2] = dzl[4];
  gMC->Gsvolu("ITS5", "TUBE", idtmed[249], dits, 3);
  gMC->Gspos("ITS5", 1, "ITSV", 0., 0., 0., 0, "ONLY");
  
  dits[0] = rl[5];
  dits[1] = rl[5] + drl[5];
  dits[2] = dzl[5];
  gMC->Gsvolu("ITS6", "TUBE", idtmed[249], dits, 3);
  gMC->Gspos("ITS6", 1, "ITSV", 0., 0., 0., 0, "ONLY");
  
  //    ELECTRONICS BOXES 
  
  //     PCB (layer #3 and #4) 
  
  gMC->Gsvolu("IPCB", "TUBE", idtmed[233], dits, 0);
  for (i = 2; i < 4; ++i) {
    dits[0] = rl[i];
    dits[1] = dits[0] + drpcb[i];
    dits[2] = dzb[i] / 2.;
    zpos = dzl[i] + dits[2];
    gMC->Gsposp("IPCB", i-1, "ITSV", 0., 0., zpos, 0, "ONLY", dits, 3);
    gMC->Gsposp("IPCB", i+1, "ITSV", 0., 0.,-zpos, 0, "ONLY", dits, 3);
  }
  
  //     COPPER (layer #3 and #4) 
  
  gMC->Gsvolu("ICO2", "TUBE", idtmed[234], dits, 0);
  for (i = 2; i < 4; ++i) {
    dits[0] = rl[i] + drpcb[i];
    dits[1] = dits[0] + drcu[i];
    dits[2] = dzb[i] / 2.;
    zpos = dzl[i] + dits[2];
    gMC->Gsposp("ICO2", i-1, "ITSV", 0., 0., zpos, 0, "ONLY", dits, 3);
    gMC->Gsposp("ICO2", i+1, "ITSV", 0., 0.,-zpos, 0, "ONLY", dits, 3);
  }
  
  //     CERAMICS (layer #3 and #4) 
  
  gMC->Gsvolu("ICER", "TUBE", idtmed[235], dits, 0);
  for (i = 2; i < 4; ++i) {
    dits[0] = rl[i] + drpcb[i] + drcu[i];
    dits[1] = dits[0] + drcer[i];
    dits[2] = dzb[i] / 2.;
    zpos = dzl[i] + dits[2];
    gMC->Gsposp("ICER", i-1, "ITSV", 0., 0., zpos, 0, "ONLY", dits, 3);
    gMC->Gsposp("ICER", i+1, "ITSV", 0., 0.,-zpos, 0, "ONLY", dits, 3);
  }
  
  //     SILICON (layer #3 and #4) 
  
  gMC->Gsvolu("ISI2", "TUBE", idtmed[226], dits, 0);
  for (i = 2; i < 4; ++i) {
    dits[0] = rl[i] + drpcb[i] + drcu[i] + drcer[i];
    dits[1] = dits[0] + drsi[i];
    dits[2] = dzb[i] / 2.;
    zpos = dzl[i] + dits[2];
    gMC->Gsposp("ISI2", i-1, "ITSV", 0., 0., zpos, 0, "ONLY", dits, 3);
    gMC->Gsposp("ISI2", i+1, "ITSV", 0., 0.,-zpos, 0, "ONLY", dits, 3);
  }
  
  //     PLASTIC (G10FR4) (layer #5 and #6) 
  
  gMC->Gsvolu("IPLA", "TUBE", idtmed[262], dits, 0);
  for (i = 4; i < 6; ++i) {
    dits[0] = rl[i];
    dits[1] = dits[0] + drpla[i];
    dits[2] = dzb[i] / 2.;
    zpos = dzl[i] + dits[2];
    gMC->Gsposp("IPLA", i-1, "ITSV", 0., 0., zpos, 0, "ONLY", dits, 3);
    gMC->Gsposp("IPLA", i+1, "ITSV", 0., 0.,-zpos, 0, "ONLY", dits, 3);
  }
  
  //     COPPER (layer #5 and #6) 
  
  gMC->Gsvolu("ICO3", "TUBE", idtmed[259], dits, 0);
  for (i = 4; i < 6; ++i) {
    dits[0] = rl[i] + drpla[i];
    dits[1] = dits[0] + drcu[i];
    dits[2] = dzb[i] / 2.;
    zpos = dzl[i] + dits[2];
    gMC->Gsposp("ICO3", i-1, "ITSV", 0., 0., zpos, 0, "ONLY", dits, 3);
    gMC->Gsposp("ICO3", i+1, "ITSV", 0., 0.,-zpos, 0, "ONLY", dits, 3);
  }
  
  //     EPOXY (layer #5 and #6) 
  
  gMC->Gsvolu("IEPX", "TUBE", idtmed[262], dits, 0);
  for (i = 4; i < 6; ++i) {
    dits[0] = rl[i] + drpla[i] + drcu[i];
    dits[1] = dits[0] + drepx[i];
    dits[2] = dzb[i] / 2.;
    zpos = dzl[i] + dits[2];
    gMC->Gsposp("IEPX", i-1, "ITSV", 0., 0., zpos, 0, "ONLY", dits, 3);
    gMC->Gsposp("IEPX", i+1, "ITSV", 0., 0.,-zpos, 0, "ONLY", dits, 3);
  }
  
  //     SILICON (layer #5 and #6) 
  
  gMC->Gsvolu("ISI3", "TUBE", idtmed[251], dits, 0);
  for (i = 4; i < 6; ++i) {
    dits[0] = rl[i] + drpla[i] + drcu[i] + drepx[i];
    dits[1] = dits[0] + drsi[i];
    dits[2] = dzb[i] / 2.;
    zpos = dzl[i] + dits[2];
    gMC->Gsposp("ISI3", i-1, "ITSV", 0., 0., zpos, 0, "ONLY", dits, 3);
    gMC->Gsposp("ISI3", i+1, "ITSV", 0., 0.,-zpos, 0, "ONLY", dits, 3);
  }
  
  //    SUPPORT 
  
  gMC->Gsvolu("ISUP", "TUBE", idtmed[274], dits, 0);
  for (i = 0; i < 6; ++i) {
    dits[0] = rl[i];
    if (i < 5) dits[1] = rl[i+1];
    else       dits[1] = rlim;
    dits[2] = drsu / 2.;
    zpos = dzl[i] + dzb[i] + dits[2];
    gMC->Gsposp("ISUP", i+1, "ITSV", 0., 0., zpos, 0, "ONLY", dits, 3);
    gMC->Gsposp("ISUP", i+7, "ITSV", 0., 0.,-zpos, 0, "ONLY", dits, 3);
  }
  
  // CABLES (HORIZONTAL) 
  
  gMC->Gsvolu("ICHO", "TUBE", idtmed[278], dits, 0);
  for (i = 0; i < 6; ++i) {
    dits[0] = rl[i];
    dits[1] = dits[0] + drca;
    dits[2] = (rzcone + TMath::Tan(acone) * (rl[i] - rl[0]) - (dzl[i]+ dzb[i] + drsu)) / 2.;
    zpos = dzl[i - 1] + dzb[i] + drsu + dits[2];
    gMC->Gsposp("ICHO", i+1, "ITSV", 0., 0., zpos, 0, "ONLY", dits, 3);
    gMC->Gsposp("ICHO", i+7, "ITSV", 0., 0.,-zpos, 0, "ONLY", dits, 3);
  }
  //    DEFINE A CONICAL GHOST VOLUME FOR THE PHI SEGMENTATION 
  pcits[0] = 0.;
  pcits[1] = 360.;
  pcits[2] = 2.;
  pcits[3] = rzcone;
  pcits[4] = 3.5;
  pcits[5] = rl[0];
  pcits[6] = pcits[3] + TMath::Tan(acone) * (rlim - rl[0]);
  pcits[7] = rlim - rl[0] + 3.5;
  pcits[8] = rlim;
  gMC->Gsvolu("ICMO", "PCON", idtmed[275], pcits, 9);
  AliMatrix(idrotm[200], 90., 0., 90., 90., 180., 0.);
  gMC->Gspos("ICMO", 1, "ITSV", 0., 0., 0., 0, "ONLY");
  gMC->Gspos("ICMO", 2, "ITSV", 0., 0., 0., idrotm[200], "ONLY");
  
  //     DIVIDE INTO NSEC PHI-SECTIONS 
  
  gMC->Gsdvn("ICMD", "ICMO", nsec, 2);
  gMC->Gsatt("ICMO", "SEEN", 0);
  gMC->Gsatt("ICMD", "SEEN", 0);
  
  //     CONICAL CABLES 
  
  pcits[2] = 2.;
  gMC->Gsvolu("ICCO", "PCON", idtmed[278], pcits, 0);
  for (i = 1; i < 6; ++i) {
    pcits[0] = -dphi[i] / 2.;
    pcits[1] = dphi[i];
    if (i < 5) {
      dzco = TMath::Tan(acone) * (rl[i+1] - rl[i]);
    } else {
      dzco1 = zmax - (rzcone + TMath::Tan(acone) * (rl[5] - rl[0])) -2.;
      dzco2 = (rlim - rl[5]) * TMath::Tan(acone);
      if (rl[5] + dzco1 / TMath::Tan(acone) < rlim) {
	dzco = dzco1;
      } else {
	dzco = dzco2;
      }
    }
    pcits[3] = rzcone + TMath::Tan(acone) * (rl[i] - rl[0]);
    pcits[4] = rl[i] - drcac[i] / TMath::Sin(acone);
    pcits[5] = rl[i];
    pcits[6] = pcits[3] + dzco;
    pcits[7] = rl[i] + dzco / TMath::Tan(acone) - drcac[i] / TMath::Sin(acone);
    pcits[8] = rl[i] + dzco / TMath::Tan(acone);
    
    gMC->Gsposp("ICCO", i, "ICMD", 0., 0., 0., 0, "ONLY", pcits, 9);
    
  }
  zend = pcits[6];
  rend = pcits[8];
  
  //  CONICAL CABLES BELOW TPC 
  
  //    DEFINE A CONICAL GHOST VOLUME FOR THE PHI SEGMENTATION 
  pcits[0] = 0.;
  pcits[1] = 360.;
  pcits[2] = 2.;
  pcits[3] = zend;
  pcits[5] = rend;
  pcits[4] = pcits[5] - drcatpc;
  pcits[6] = xltpc;
  pcits[8] = pcits[4] + (pcits[6] - pcits[3]) * TMath::Tan(acable * kDegrad);
  pcits[7] = pcits[8] - drcatpc;
  AliMatrix(idrotm[200], 90., 0., 90., 90., 180., 0.);
  gMC->Gsvolu("ICCM", "PCON", idtmed[275], pcits, 9);
  gMC->Gspos("ICCM", 1, "ALIC", 0., 0., 0., 0, "ONLY");
  gMC->Gspos("ICCM", 2, "ALIC", 0., 0., 0., idrotm[200], "ONLY");
  gMC->Gsdvn("ITMD", "ICCM", nsec, 2);
  gMC->Gsatt("ITMD", "SEEN", 0);
  gMC->Gsatt("ICCM", "SEEN", 0);
  
  //     NOW PLACE SEGMENTS WITH DECREASING PHI SEGMENTS INTO THE 
  //     GHOST-VOLUME 
  
  pcits[2] = 2.;
  gMC->Gsvolu("ITTT", "PCON", idtmed[278], pcits, 0);
  r0 = rend;
  z0 = zend;
  dz = (xltpc - zend) / 9.;
  for (i = 0; i < 9; ++i) {
    zi = z0 + i*dz + dz / 2.;
    ri = r0 + (zi - z0) * TMath::Tan(acable * kDegrad);
    dphii = dphi[5] * r0 / ri;
    pcits[0] = -dphii / 2.;
    pcits[1] = dphii;
    pcits[3] = zi - dz / 2.;
    pcits[5] = r0 + (pcits[3] - z0) * TMath::Tan(acable * kDegrad);
    pcits[4] = pcits[5] - drcatpc;
    pcits[6] = zi + dz / 2.;
    pcits[8] = r0 + (pcits[6] - z0) * TMath::Tan(acable * kDegrad);
    pcits[7] = pcits[8] - drcatpc;
    
    gMC->Gsposp("ITTT", i+1, "ITMD", 0., 0., 0., 0, "ONLY", pcits, 9);
  }
  
  // --- Outputs the geometry tree in the EUCLID/CAD format 
  
  if (fEuclidOut) {
    gMC->WriteEuclid("ITSgeometry", "ITSV", 1, 5);
  }
}
//_____________________________________________________________________________
void AliITSv1::CreateMaterials(){
////////////////////////////////////////////////////////////////////////
  //
  // Create ITS materials
  //     This function defines the default materials used in the Geant
  // Monte Carlo simulations for the geometries AliITSv1 and AliITSv3.
  // In general it is automatically replaced by
  // the CreatMaterials routine defined in AliITSv?. Should the function
  // CreateMaterials not exist for the geometry version you are using this
  // one is used. See the definition found in AliITSv5 or the other routine
  // for a complete definition.
  //
  // Water H2O
  Float_t awat[2]  = { 1.00794,15.9994 };
  Float_t zwat[2]  = { 1.,8. };
  Float_t wwat[2]  = { 2.,1. };
  Float_t denswat  = 1.;
  // Freon
  Float_t afre[2]  = { 12.011,18.9984032 };
  Float_t zfre[2]  = { 6.,9. };
  Float_t wfre[2]  = { 5.,12. };
  Float_t densfre  = 1.5;
  // Ceramics
  //     94.4% Al2O3 , 2.8% SiO2 , 2.3% MnO , 0.5% Cr2O3 
  Float_t acer[5]  = { 26.981539,15.9994,28.0855,54.93805,51.9961 };
  Float_t zcer[5]  = { 13.,8.,14.,25.,	    24. };
  Float_t wcer[5]  = { .49976,1.01233,.01307,	    .01782,.00342 };
  Float_t denscer  = 3.6;
  //
  //     60% SiO2 , 40% G10FR4 
  // PC board
  Float_t apcb[3]  = { 28.0855,15.9994,17.749 };
  Float_t zpcb[3]  = { 14.,8.,8.875 };
  Float_t wpcb[3]  = { .28,.32,.4 };
  Float_t denspcb  = 1.8;
  // POLYETHYL
  Float_t apoly[2] = { 12.01,1. };
  Float_t zpoly[2] = { 6.,1. };
  Float_t wpoly[2] = { .33,.67 };
  // SERVICES
  Float_t zserv[4] = { 1.,6.,26.,29. };
  Float_t aserv[4] = { 1.,12.,55.8,63.5 };
  Float_t wserv[4] = { .014,.086,.42,.48 };
  
  Int_t  isxfld  = gAlice->Field()->Integ();
  Float_t sxmgmx = gAlice->Field()->Max();
  
  
  // --- Define the various materials for GEANT --- 
  
  //  200-224 --> Silicon Pixel Detectors (detectors, chips, buses, cooling,..)
  
  AliMaterial(0, "SPD Si$",      28.0855, 14., 2.33, 9.36, 999);
  AliMaterial(1, "SPD Si chip$", 28.0855, 14., 2.33, 9.36, 999);
  AliMaterial(2, "SPD Si bus$",  28.0855, 14., 2.33, 9.36, 999);
  AliMaterial(3, "SPD C$",       12.011,   6., 2.265,18.8, 999);
  // v. dens 
  AliMaterial(4, "SPD Air$",    14.61, 7.3, .001205, 30423., 999);
  AliMaterial(5, "SPD Vacuum$", 1e-16, 1e-16, 1e-16, 1e16, 1e16);
  AliMaterial(6, "SPD Al$",     26.981539, 13., 2.6989, 8.9, 999);
  AliMixture( 7, "SPD Water $", awat, zwat, denswat, -2, wwat);
  AliMixture( 8, "SPD Freon$",  afre, zfre, densfre, -2, wfre);
  // ** 
  AliMedium(0, "SPD Si$",      0, 1,isxfld,sxmgmx, 10., .01, .1, .003, .003);
  AliMedium(1, "SPD Si chip$", 1, 0,isxfld,sxmgmx, 10., .01, .1, .003, .003);
  AliMedium(2, "SPD Si bus$",  2, 0,isxfld,sxmgmx, 10., .01, .1, .003, .003);
  AliMedium(3, "SPD C$",       3, 0,isxfld,sxmgmx, 10., .01, .1, .003, .003);
  AliMedium(4, "SPD Air$",     4, 0,isxfld,sxmgmx, 10., .01, .1, .003, .003);
  AliMedium(5, "SPD Vacuum$",  5, 0,isxfld,sxmgmx, 10.,1.00, .1, .100,10.00);
  AliMedium(6, "SPD Al$",      6, 0,isxfld,sxmgmx, 10., .01, .1, .003, .003);
  AliMedium(7, "SPD Water $",  7, 0,isxfld,sxmgmx, 10., .01, .1, .003, .003);
  AliMedium(8, "SPD Freon$",   8, 0,isxfld,sxmgmx, 10., .01, .1, .003, .003);
  
  //  225-249 --> Silicon Drift Detectors (detectors, chips, buses, cooling,..)
  
  AliMaterial(25, "SDD Si$",      28.0855, 14., 2.33,  9.36, 999);
  AliMaterial(26, "SDD Si chip$", 28.0855, 14., 2.33,  9.36, 999);
  AliMaterial(27, "SDD Si bus$",  28.0855, 14., 2.33,  9.36, 999);
  AliMaterial(28, "SDD C$",       12.011,   6., 2.265,18.8,  999);
  // v. dens 
  AliMaterial(29, "SDD Air$",     14.61, 7.3, .001205, 30423., 999);
  AliMaterial(30, "SDD Vacuum$",  1e-16, 1e-16, 1e-16, 1e16,  1e16);
  AliMaterial(31, "SDD Al$",      26.981539, 13., 2.6989, 8.9, 999);
  // After a call with ratios by number (negative number of elements), 
  // the ratio array is changed to the ratio by weight, so all successive 
  // calls with the same array must specify the number of elements as 
  // positive 
  AliMixture(32, "SDD Water $", awat, zwat, denswat, 2, wwat);
  // After a call with ratios by number (negative number of elements), 
  // the ratio array is changed to the ratio by weight, so all successive 
  // calls with the same array must specify the number of elements as 
  // positive 
  AliMixture( 33, "SDD Freon$", afre, zfre, densfre, 2, wfre);
  AliMixture( 34, "SDD PCB$",   apcb, zpcb, denspcb, 3, wpcb);
  AliMaterial(35, "SDD Copper$", 63.546, 29., 8.96, 1.43, 999);
  AliMixture( 36, "SDD Ceramics$", acer, zcer, denscer, -5, wcer);
  AliMaterial(37, "SDD Kapton$", 12.011, 6., 1.3, 31.27, 999);
  // ** 
  // check A and Z 
  AliMedium(25, "SDD Si$",      25, 1,isxfld,sxmgmx, 10., .01, .1, .003, .003);
  AliMedium(26, "SDD Si chip$", 26, 0,isxfld,sxmgmx, 10., .01, .1, .003, .003);
  AliMedium(27, "SDD Si bus$",  27, 0,isxfld,sxmgmx, 10., .01, .1, .003, .003);
  AliMedium(28, "SDD C$",       28, 0,isxfld,sxmgmx, 10., .01, .1, .003, .003);
  AliMedium(29, "SDD Air$",     29, 0,isxfld,sxmgmx, 10., .01, .1, .003, .003);
  AliMedium(30, "SDD Vacuum$",  30, 0,isxfld,sxmgmx, 10.,1.00, .1, .100,10.00);
  AliMedium(31, "SDD Al$",      31, 0,isxfld,sxmgmx, 10., .01, .1, .003, .003);
  AliMedium(32, "SDD Water $",  32, 0,isxfld,sxmgmx, 10., .01, .1, .003, .003);
  AliMedium(33, "SDD Freon$",   33, 0,isxfld,sxmgmx, 10., .01, .1, .003, .003);
  AliMedium(34, "SDD PCB$",     34, 0,isxfld,sxmgmx, 10., .01, .1, .003, .003);
  AliMedium(35, "SDD Copper$",  35, 0,isxfld,sxmgmx, 10., .01, .1, .003, .003);
  AliMedium(36, "SDD Ceramics$",36, 0,isxfld,sxmgmx, 10., .01, .1, .003, .003);
  AliMedium(37, "SDD Kapton$",  37, 0,isxfld,sxmgmx, 10., .01, .1, .003, .003);
  
  //  250-274 --> Silicon Strip Detectors (detectors, chips, buses, cooling,..)
  
  AliMaterial(50, "SSD Si$",      28.0855, 14., 2.33, 9.36, 999.);
  AliMaterial(51, "SSD Si chip$", 28.0855, 14., 2.33, 9.36, 999.);
  AliMaterial(52, "SSD Si bus$",  28.0855, 14., 2.33, 9.36, 999.);
  AliMaterial(53, "SSD C$",       12.011,   6., 2.265,18.8, 999.);
  // v. dens 
  AliMaterial(54, "SSD Air$",     14.61, 7.3, .001205, 30423., 999);
  AliMaterial(55, "SSD Vacuum$",  1e-16, 1e-16, 1e-16, 1e16, 1e16);
  AliMaterial(56, "SSD Al$",      26.981539, 13., 2.6989, 8.9, 999);
  // After a call with ratios by number (negative number of elements), 
  // the ratio array is changed to the ratio by weight, so all successive 
  // calls with the same array must specify the number of elements as 
  // positive 
  AliMixture(57, "SSD Water $", awat, zwat, denswat, 2, wwat);
  // After a call with ratios by number (negative number of elements), 
  // the ratio array is changed to the ratio by weight, so all successive 
  // calls with the same array must specify the number of elements as 
  // positive 
  AliMixture(58, "SSD Freon$", afre, zfre, densfre, 2, wfre);
  AliMixture(59, "SSD PCB$",   apcb, zpcb, denspcb, 3, wpcb);
  AliMaterial(60, "SSD Copper$", 63.546, 29., 8.96, 1.43, 999.);
  // After a call with ratios by number (negative number of elements), 
  // the ratio array is changed to the ratio by weight, so all successive 
  // calls with the same array must specify the number of elements as 
  // positive 
  AliMixture( 61, "SSD Ceramics$", acer, zcer, denscer, 5, wcer);
  AliMaterial(62, "SSD Kapton$", 12.011, 6., 1.3, 31.27, 999.);
  // check A and Z 
  AliMaterial(63, "SDD G10FR4$", 17.749, 8.875, 1.8, 21.822, 999.);
  // ** 
  AliMedium(50, "SSD Si$",      50, 1,isxfld,sxmgmx, 10., .01, .1, .003, .003);
  AliMedium(51, "SSD Si chip$", 51, 0,isxfld,sxmgmx, 10., .01, .1, .003, .003);
  AliMedium(52, "SSD Si bus$",  52, 0,isxfld,sxmgmx, 10., .01, .1, .003, .003);
  AliMedium(53, "SSD C$",       53, 0,isxfld,sxmgmx, 10., .01, .1, .003, .003);
  AliMedium(54, "SSD Air$",     54, 0,isxfld,sxmgmx, 10., .01, .1, .003, .003);
  AliMedium(55, "SSD Vacuum$",  55, 0,isxfld,sxmgmx, 10.,1.00, .1, .100,10.00);
  AliMedium(56, "SSD Al$",      56, 0,isxfld,sxmgmx, 10., .01, .1, .003, .003);
  AliMedium(57, "SSD Water $",  57, 0,isxfld,sxmgmx, 10., .01, .1, .003, .003);
  AliMedium(58, "SSD Freon$",   58, 0,isxfld,sxmgmx, 10., .01, .1, .003, .003);
  AliMedium(59, "SSD PCB$",     59, 0,isxfld,sxmgmx, 10., .01, .1, .003, .003);
  AliMedium(60, "SSD Copper$",  60, 0,isxfld,sxmgmx, 10., .01, .1, .003, .003);
  AliMedium(61, "SSD Ceramics$",61, 0,isxfld,sxmgmx, 10., .01, .1, .003, .003);
  AliMedium(62, "SSD Kapton$",  62, 0,isxfld,sxmgmx, 10., .01, .1, .003, .003);
  AliMedium(63, "SSD G10FR4$",  63, 0,isxfld,sxmgmx, 10., .01, .1, .003, .003);
  
  //     275-299 --> General (end-caps, frames, cooling, cables, etc.) 
  
  AliMaterial(75, "GEN C$", 12.011, 6., 2.265, 18.8, 999.);
  // verify density 
  AliMaterial(76, "GEN Air$", 14.61, 7.3, .001205, 30423., 999);
  AliMaterial(77, "GEN Vacuum$", 1e-16, 1e-16, 1e-16, 1e16, 1e16);
  AliMixture( 78, "GEN POLYETHYL$", apoly, zpoly, .95, -2, wpoly);
  AliMixture( 79, "GEN SERVICES$",  aserv, zserv, 4.68, 4, wserv);
  AliMaterial(80, "GEN Copper$", 63.546, 29., 8.96, 1.43, 999.);
  // After a call with ratios by number (negative number of elements), 
  // the ratio array is changed to the ratio by weight, so all successive 
  // calls with the same array must specify the number of elements as 
  // positive 
  AliMixture(81, "GEN Water $", awat, zwat, denswat, 2, wwat);
  // ** 
  AliMedium(75,"GEN C$",        75, 0,isxfld,sxmgmx, 10., .01, .1, .003, .003);
  AliMedium(76,"GEN Air$",      76, 0,isxfld,sxmgmx, 10., .01, .1, .003, .003);
  AliMedium(77,"GEN Vacuum$",   77, 0,isxfld,sxmgmx, 10., .10, .1, .100,10.00);
  AliMedium(78,"GEN POLYETHYL$",78, 0,isxfld,sxmgmx, 10., .01, .1, .003, .003);
  AliMedium(79,"GEN SERVICES$", 79, 0,isxfld,sxmgmx, 10., .01, .1, .003, .003);
  AliMedium(80,"GEN Copper$",   80, 0,isxfld,sxmgmx, 10., .01, .1, .003, .003);
  AliMedium(81,"GEN Water $",   81, 0,isxfld,sxmgmx, 10., .01, .1, .003, .003);
}
//_____________________________________________________________________________
void AliITSv1::Init(){
////////////////////////////////////////////////////////////////////////
//     Initialise the ITS after it has been created.
////////////////////////////////////////////////////////////////////////

    //
    AliITS::Init();
    fMajorVersion = 1;
    fMinorVersion = 0;
}  
 
//_____________________________________________________________________________
void AliITSv1::DrawModule(){
////////////////////////////////////////////////////////////////////////
//     Draw a shaded view of the FMD version 1.
////////////////////////////////////////////////////////////////////////
  
  // Set everything unseen
  gMC->Gsatt("*", "seen", -1);
  // 
  // Set ALIC mother visible
  gMC->Gsatt("ALIC","SEEN",0);
  //
  // Set the volumes visible
  gMC->Gsatt("ITSV","SEEN",0);
  gMC->Gsatt("ITS1","SEEN",1);
  gMC->Gsatt("ITS2","SEEN",1);
  gMC->Gsatt("ITS3","SEEN",1);
  gMC->Gsatt("ITS4","SEEN",1);
  gMC->Gsatt("ITS5","SEEN",1);
  gMC->Gsatt("ITS6","SEEN",1);

  gMC->Gsatt("IPCB","SEEN",1);
  gMC->Gsatt("ICO2","SEEN",1);
  gMC->Gsatt("ICER","SEEN",0);
  gMC->Gsatt("ISI2","SEEN",0);
  gMC->Gsatt("IPLA","SEEN",0);
  gMC->Gsatt("ICO3","SEEN",0);
  gMC->Gsatt("IEPX","SEEN",0);
  gMC->Gsatt("ISI3","SEEN",1);
  gMC->Gsatt("ISUP","SEEN",0);
  gMC->Gsatt("ICHO","SEEN",0);
  gMC->Gsatt("ICMO","SEEN",0);
  gMC->Gsatt("ICMD","SEEN",0);
  gMC->Gsatt("ICCO","SEEN",1);
  gMC->Gsatt("ICCM","SEEN",0);
  gMC->Gsatt("ITMD","SEEN",0);
  gMC->Gsatt("ITTT","SEEN",1);

  //
  gMC->Gdopt("hide", "on");
  gMC->Gdopt("shad", "on");
  gMC->Gsatt("*", "fill", 7);
  gMC->SetClipBox(".");
  gMC->SetClipBox("*", 0, 300, -300, 300, -300, 300);
  gMC->DefaultRange();
  gMC->Gdraw("alic", 40, 30, 0, 11, 10, .07, .07);
  gMC->Gdhead(1111, "Inner Tracking System Version 1");
  gMC->Gdman(17, 6, "MAN");
}
//_____________________________________________________________________________
void AliITSv1::StepManager(){
////////////////////////////////////////////////////////////////////////
//    Called for every step in the ITS, then calles the AliITShit class
// creator with the information to be recoreded about that hit.
////////////////////////////////////////////////////////////////////////
/*
  Int_t         copy, id;
  Float_t       hits[8];
  Int_t         vol[4];
  TLorentzVector position, momentum;
  TClonesArray &lhits = *fHits;
  //
  // Track status
  vol[3] = 0;
  if(gMC->IsTrackInside())      vol[3] +=  1;
  if(gMC->IsTrackEntering())    vol[3] +=  2;
  if(gMC->IsTrackExiting())     vol[3] +=  4;
  if(gMC->IsTrackOut())         vol[3] +=  8;
  if(gMC->IsTrackDisappeared()) vol[3] += 16;
  if(gMC->IsTrackStop())        vol[3] += 32;
  if(gMC->IsTrackAlive())       vol[3] += 64;
  //
  // Fill hit structure.
  if( !(gMC->TrackCharge()) ) return;
    //
    // Only entering charged tracks
    if((id=gMC->CurrentVolID(copy))==fIdSens[0]) {  
      vol[0]=1;
      id=gMC->CurrentVolOffID(1,copy);      
      vol[1]=copy;
      id=gMC->CurrentVolOffID(2,copy);
      vol[2]=copy;                       
    } else if(id==fIdSens[1]) {
      vol[0]=2;
      id=gMC->CurrentVolOffID(1,copy);       
      vol[1]=copy;
      id=gMC->CurrentVolOffID(2,copy);
      vol[2]=copy;                    
    } else if(id==fIdSens[2]) {
      vol[0]=3;
      vol[1]=copy;
      id=gMC->CurrentVolOffID(1,copy);
      vol[2]=copy;             
    } else if(id==fIdSens[3]) {
      vol[0]=4;
      vol[1]=copy;
      id=gMC->CurrentVolOffID(1,copy);
      vol[2]=copy;                  
    } else if(id==fIdSens[4]) {
      vol[0]=5;
      vol[1]=copy;
      id=gMC->CurrentVolOffID(1,copy);
      vol[2]=copy;               
    } else if(id==fIdSens[5]) {
      vol[0]=6;
      vol[1]=copy;
      id=gMC->CurrentVolOffID(1,copy);
      vol[2]=copy;                      
    } else return;
    gMC->TrackPosition(position);
    gMC->TrackMomentum(momentum);
    hits[0]=position[0];
    hits[1]=position[1];
    hits[2]=position[2];          
    hits[3]=momentum[0];
    hits[4]=momentum[1];
    hits[5]=momentum[2];
    hits[6]=gMC->Edep();
    hits[7]=gMC->TrackTime();
    new(lhits[fNhits++]) AliITShit(fIshunt,gAlice->CurrentTrack(),vol,hits);
*/
}

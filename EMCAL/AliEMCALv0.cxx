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

//_________________________________________________________________________
// Implementation version v0 of EMCAL Manager class 
// An object of this class does not produce hits nor digits
// It is the one to use if you do not want to produce outputs in TREEH or TREED
// This class places a Geometry of the EMCAL in the ALICE Detector as defined in AliEMCALGeometry.cxx                 
//*-- Author: Yves Schutz (SUBATECH)
//*-- and   : Sahal Yacoob (LBL / UCT)
//          : Alexei Pavlinov (WSU)     SHASHLYK

// --- ROOT system ---
#include <cassert>

#include <TGeometry.h>
#include <TGeoPhysicalNode.h>
#include <TGeoManager.h>
#include <TGeoMatrix.h>
#include <TVirtualMC.h>
#include <TArrayI.h>
#include <TROOT.h>
#include <TList.h>
#include <TVector2.h>
#include <cassert>

//--- EMCAL system---
#include "AliEMCALShishKebabTrd1Module.h"

// --- Standard library ---

//#include <stdio.h>

// --- AliRoot header files ---

#include "AliEMCALv0.h"
#include "AliEMCALGeometry.h"
#include "AliRun.h"
#include "AliLog.h"
#include "AliGeomManager.h"
#include "AliEMCALSpaceFrame.h"

ClassImp(AliEMCALv0)

// EMCAL material: look to the AliEMCAL.cxx
enum
 {
  kIdAIR   = 1599, 
  kIdPB    = 1600, 
  kIdSC    = 1601, 
  kIdAL    = 1602, 
  kIdSTEEL = 1603,
  kIdPAPER = 1604
 };


//______________________________________________________________________
AliEMCALv0::AliEMCALv0()
  : AliEMCAL(),
    fShishKebabModules(),fEnvelop1(0),fIdRotm(0),fIdTmedArr(0),
    fSampleWidth(0),fSmodPar0(0),fSmodPar1(0),fSmodPar2(0),fCalFrame(0)
{
  //default ctor
  for(Int_t i = 0; i < 5 ; i++) fParEMOD[i]=0.0;
}

//______________________________________________________________________
AliEMCALv0::AliEMCALv0(const char *name, const char *title)
  : AliEMCAL(name,title),
    fShishKebabModules(),fEnvelop1(0),fIdRotm(0),fIdTmedArr(0),
    fSampleWidth(0),fSmodPar0(0),fSmodPar1(0),fSmodPar2(0),fCalFrame(0)
{
  // ctor : title is used to identify the layout
  // Apr 25, 2006
  // Nov 22, 2006 - case of 1X1  
  
  for(Int_t i = 0; i < 5 ; i++) fParEMOD[i]=0.0;
  TString ntmp(GetTitle());
  ntmp.ToUpper();

  AliEMCALGeometry *g = GetGeometry() ; 
  TString gn(g->GetName()); gn.ToUpper();
  fShishKebabModules = g->GetShishKebabTrd1Modules(); 
  fGeometry = g;
  fSampleWidth = double(g->GetECPbRadThick()+g->GetECScintThick());
  if(gn.Contains("V1")) fSampleWidth += 2.*g->GetTrd1BondPaperThick();
  printf("<I> AliEMCALv0::AliEMCALv : fGeometry %p : gMC %p : fSampleWidth %5.4f\n", 
	 fGeometry, gMC, fSampleWidth);
}

//______________________________________________________________________
void AliEMCALv0::CreateGeometry()
{
  // Create the EMCAL geometry for Geant
  // Geometry of a tower
  
  AliEMCALGeometry * geom = GetGeometry() ; 
  TString gn(geom->GetName());
  gn.ToUpper(); 
  
  if(!(geom->IsInitialized())){
    Error("CreateGeometry","EMCAL Geometry class has not been set up.");
  } // end if

  // Get pointer to the array containing media indices
  fIdTmedArr = fIdtmed->GetArray() - 1599 ;
  
  fIdRotm = 1;
  //  gMC->Matrix(nmat, theta1, phi1, theta2, phi2, theta3, phi3) - see AliModule
  AliMatrix(fIdRotm, 90.0, 0., 90.0, 90.0, 0.0, 0.0) ; 
  
  // Create the EMCAL Mother Volume (a polygone) within which to place the Detector and named XEN1 
  
  Float_t envelopA[10];
  if(gn.Contains("WSUC") ) { // TRD1 for WSUC facility
    // 17-may-05 - just BOX
    envelopA[0] = 26;
    envelopA[1] = 15;
    envelopA[2] = 30;
    gMC->Gsvolu("XEN1", "BOX", fIdTmedArr[kIdSC], envelopA, 3) ;
    fEnvelop1.Set(3);
    for(int i=0; i<3; i++) fEnvelop1[i] = envelopA[i]; // 23-may-05  
    // Position the EMCAL Mother Volume (XEN1) in WSUC  
    gMC->Gspos("XEN1", 1, "WSUC", 0.0, 0.0, 0.0, fIdRotm, "ONLY") ;
  } else { 
    envelopA[0] = geom->GetArm1PhiMin();                         // minimum phi angle
    envelopA[1] = geom->GetArm1PhiMax() - geom->GetArm1PhiMin(); // angular range in phi
    envelopA[2] = geom->GetNPhiSuperModule();                    // number of sections in phi
    envelopA[3] = 2;                                             // 2 z coordinates
    envelopA[4] = -geom->GetEnvelop(2)/2.;                       // zmin - includes padding
    envelopA[5] = geom->GetEnvelop(0) ;                          // rmin at z1 - includes padding
    envelopA[6] = geom->GetEnvelop(1) ;                          // rmax at z1 - includes padding
    envelopA[7] = geom->GetEnvelop(2)/2.;                        // zmax includes padding
    
    envelopA[8] = envelopA[5] ;                                  // radii are the same.
    envelopA[9] = envelopA[6] ;                                  // radii are the same.

    gMC->Gsvolu("XEN1", "PGON", fIdTmedArr[kIdAIR], envelopA, 10) ;   // Polygone filled with air 
    fEnvelop1.Set(10, envelopA);
    if (gDebug==2) {
      printf("CreateGeometry: XEN1 = %f, %f\n", envelopA[5], envelopA[6]); 
      printf("CreateGeometry: XU0 = %f, %f\n", envelopA[5], envelopA[6]); 
    }
    // Position the EMCAL Mother Volume (XEN1) in Alice (ALIC)  
    gMC->Gspos(geom->GetNameOfEMCALEnvelope(), 1, "ALIC", 0.0, 0.0, 0.0, fIdRotm, "ONLY") ;
  }

  // COMPACT, TRD1
  AliDebug(2,Form("Shish-Kebab geometry : %s", GetTitle())); 
  CreateShishKebabGeometry();

  //Space Frame
  AliDebug(2,"Creating EMCAL Space Frame");
  fCalFrame = new AliEMCALSpaceFrame();
  fCalFrame->CreateGeometry();

}

//______________________________________________________________________
void AliEMCALv0::Init(void)
{
    // Just prints an information message
  AliEMCAL::Init();
  if(AliLog::GetGlobalDebugLevel()>0) { 
    TString message("\n") ; 
    message += "*****************************************\n" ;
    
    // Here the EMCAL initialisation code (if any!)
    
    AliEMCALGeometry * geom = GetGeometry() ; 
    
    if (geom!=0) {   
      message += "AliEMCAL " ; 
      message += Version() ; 
      message += "EMCAL geometry initialized for " ; 
      message += geom->GetName()  ;
    }
    else {
      message += "AliEMCAL " ; 
      message += Version() ;  
      message += "EMCAL geometry initialization failed !" ; 
    }
    message += "\n*****************************************" ;
    printf("%s",message.Data() ) ; 
  }
}

// 24-aug-04 by PAI
//______________________________________________________________________
void AliEMCALv0::CreateShishKebabGeometry()
{  
  // Oct 26,2010
  // TRD1
  AliEMCALGeometry * g = GetGeometry(); 
  TString gn(g->GetName()); gn.ToUpper(); 
  Double_t trd1Angle = g->GetTrd1Angle()*TMath::DegToRad(), tanTrd1 = TMath::Tan(trd1Angle/2.);
  // see AliModule::fFIdTmedArr
  //  fIdTmedArr = fIdtmed->GetArray() - 1599 ; // see AliEMCAL::::CreateMaterials()
  //  int kIdAIR=1599, kIdPB = 1600, kIdSC = 1601, kIdSTEEL = 1603;
  //  idAL = 1602;
  Double_t par[10], xpos=0., ypos=0., zpos=0.;

  CreateSmod(g->GetNameOfEMCALEnvelope());

  CreateEmod("SMOD","EMOD"); // 18-may-05

  if(g->GetKey110DEG()) CreateEmod("SM10","EMOD"); // Nov 1,2006 

  // Sensitive SC  (2x2 tiles)
  double parSCM0[5]={0,0,0,0}, *dummy = 0, parTRAP[11];

  if(!gn.Contains("V1")) {
    double wallThickness = g->GetPhiModuleSize()/g->GetNPHIdiv() -  g->GetPhiTileSize();
    for(int i=0; i<3; i++) parSCM0[i] = fParEMOD[i] - wallThickness;
    parSCM0[3] = fParEMOD[3];
    gMC->Gsvolu("SCM0", "TRD1", fIdTmedArr[kIdAIR], parSCM0, 4);
    gMC->Gspos("SCM0", 1, "EMOD", 0., 0., 0., 0, "ONLY") ;
  } else {
    double wTh = g->GetLateralSteelStrip();
    parSCM0[0] = fParEMOD[0] - wTh + tanTrd1*g->GetTrd1AlFrontThick();
    parSCM0[1] = fParEMOD[1] - wTh;
    parSCM0[2] = fParEMOD[2] - wTh;
    parSCM0[3] = fParEMOD[3] - g->GetTrd1AlFrontThick()/2.;
    gMC->Gsvolu("SCM0", "TRD1", fIdTmedArr[kIdAIR], parSCM0, 4);
    double zshift = g->GetTrd1AlFrontThick()/2.;
    gMC->Gspos("SCM0", 1, "EMOD", 0., 0., zshift, 0, "ONLY");
    // 
    CreateAlFrontPlate("EMOD","ALFP");
  }

  if(g->GetNPHIdiv()==2 && g->GetNETAdiv()==2) {
    // Division to tile size - 1-oct-04
    AliDebug(2,Form(" Divide SCM0 on y-axis %i\n", g->GetNETAdiv()));
    gMC->Gsdvn("SCMY","SCM0", g->GetNETAdiv(), 2); // y-axis
    // Trapesoid 2x2
    parTRAP[0] = parSCM0[3];    // dz
    parTRAP[1] = TMath::ATan2((parSCM0[1]-parSCM0[0])/2.,2.*parSCM0[3])*180./TMath::Pi(); // theta
    parTRAP[2] = 0.;           // phi
    // bottom
    parTRAP[3] = parSCM0[2]/2.; // H1
    parTRAP[4] = parSCM0[0]/2.; // BL1
    parTRAP[5] = parTRAP[4];    // TL1
    parTRAP[6] = 0.0;           // ALP1
    // top
    parTRAP[7] = parSCM0[2]/2.; // H2
    parTRAP[8] = parSCM0[1]/2.; // BL2
    parTRAP[9] = parTRAP[8];    // TL2
    parTRAP[10]= 0.0;           // ALP2
    AliDebug(2,Form(" ** TRAP ** \n"));
    for(int i=0; i<11; i++) AliDebug(3, Form(" par[%2.2i] %9.4f\n", i, parTRAP[i]));
    
    gMC->Gsvolu("SCMX", "TRAP", fIdTmedArr[kIdSC], parTRAP, 11);
    xpos = +(parSCM0[1]+parSCM0[0])/4.;
    gMC->Gspos("SCMX", 1, "SCMY", xpos, 0.0, 0.0, 0, "ONLY") ;

    // Using rotation because SCMX should be the same due to Pb tiles
    xpos = -xpos; 
    AliMatrix(fIdRotm, 90.0,180., 90.0, 270.0, 0.0,0.0) ;
    gMC->Gspos("SCMX", 2, "SCMY", xpos, 0.0, 0.0, fIdRotm, "ONLY");
    // put LED to the SCM0 
    AliEMCALShishKebabTrd1Module *mod = (AliEMCALShishKebabTrd1Module*)fShishKebabModules->At(0);
    Double_t tanBetta = mod->GetTanBetta();

    int nr=0; 
    ypos = 0.0; 
    double xCenterSCMX =  (parTRAP[4] +  parTRAP[8])/2.;
    if(!gn.Contains("V1")) {
      par[1] = parSCM0[2]/2;            // y 
      par[2] = g->GetECPbRadThick()/2.; // z
      gMC->Gsvolu("PBTI", "BOX", fIdTmedArr[kIdPB], dummy, 0);
      zpos = -fSampleWidth*g->GetNECLayers()/2. + g->GetECPbRadThick()/2.;
      AliDebug(2,Form(" Pb tiles \n"));
      for(int iz=0; iz<g->GetNECLayers(); iz++){
        par[0] = (parSCM0[0] + tanBetta*fSampleWidth*iz)/2.;
        xpos   = par[0] - xCenterSCMX;
        gMC->Gsposp("PBTI", ++nr, "SCMX", xpos, ypos, zpos, 0, "ONLY", par, 3) ;
        AliDebug(3,Form(" %i xpos %f zpos %f par[0] %f \n", iz+1, xpos, zpos, par[0]));
        zpos += fSampleWidth;
      } 
      AliDebug(2,Form(" Number of Pb tiles in SCMX %i \n", nr));
    } else {
      // Oct 26, 2010
      // First sheet of paper
      par[1] = parSCM0[2]/2.;                 // y 
      par[2] = g->GetTrd1BondPaperThick()/2.; // z
      par[0] = parSCM0[0]/2.;                 // x 
      gMC->Gsvolu("PAP1", "BOX", fIdTmedArr[kIdPAPER], par, 3);
      xpos = par[0] - xCenterSCMX;
      zpos = -parSCM0[3] + g->GetTrd1BondPaperThick()/2.;
      gMC->Gspos("PAP1", 1, "SCMX", xpos, ypos, zpos, 0, "ONLY");
      for(int iz=0; iz<g->GetNECLayers()-1; iz++){
        nr = iz + 1;
        Double_t dz = g->GetECScintThick() + g->GetTrd1BondPaperThick() + fSampleWidth*iz;
	// PB + 2 paper sheets
        par[2] = g->GetECPbRadThick()/2. + g->GetTrd1BondPaperThick(); // z
        par[0] = (parSCM0[0] + tanBetta*dz)/2.;
        TString pa(Form("PA%2.2i",nr));
	gMC->Gsvolu(pa.Data(), "BOX", fIdTmedArr[kIdPAPER], par, 3);
        xpos   = par[0] - xCenterSCMX;
        zpos   = -parSCM0[3] + dz + par[2];
        gMC->Gspos(pa.Data(), 1, "SCMX", xpos, ypos, zpos, 0, "ONLY") ;
	// Pb
        TString pb(Form("PB%2.2i",nr));
        par[2] = g->GetECPbRadThick()/2.; // z
	gMC->Gsvolu(pb.Data(), "BOX", fIdTmedArr[kIdPB], par, 3);
        gMC->Gspos(pb.Data(), 1, pa.Data(), 0.0, 0.0, 0.0, 0, "ONLY") ;
      } 
    }
    
  } else if(g->GetNPHIdiv()==3 && g->GetNETAdiv()==3) {
    printf(" before AliEMCALv0::Trd1Tower3X3() : parSCM0");
    for(int i=0; i<4; i++) printf(" %7.4f ", parSCM0[i]);
    printf("\n"); 
    Trd1Tower3X3(parSCM0);
  } else if(g->GetNPHIdiv()==1 && g->GetNETAdiv()==1) {
    // no division in SCM0
    Trd1Tower1X1(parSCM0);
  } else if(g->GetNPHIdiv()==4 && g->GetNETAdiv()==4) {
    Trd1Tower4X4();
  }
  
}

//______________________________________________________________________
void AliEMCALv0::CreateSmod(const char* mother)
{ 
  // 18-may-05; mother="XEN1"; 
  // child="SMOD" from first to 10th, "SM10" (11th and 12th) (TRD1 case)
  AliEMCALGeometry * g = GetGeometry(); 
  TString gn(g->GetName()); gn.ToUpper();

  Double_t par[3], xpos=0., ypos=0., zpos=0., rpos=0., dphi=0., phi=0.0, phiRad=0.;
  Double_t par1C = 0.;
  //  ===== define Super Module from air - 14x30 module ==== ;
  AliDebug(2,Form("\n ## Super Module | fSampleWidth %5.3f ## %s \n", fSampleWidth, gn.Data()));
  par[0] = g->GetShellThickness()/2.;
  par[1] = g->GetPhiModuleSize()*g->GetNPhi()/2.; 
  par[2] = g->GetEtaModuleSize()*15.; 
  fIdRotm=0;
  int nphism = g->GetNumberOfSuperModules()/2; // 20-may-05
  if(nphism>0) {
    dphi = (g->GetArm1PhiMax() - g->GetArm1PhiMin())/nphism;
    //    if(g->GetKey110DEG()) dphi = (g->GetArm1PhiMax() - g->GetArm1PhiMin())/(nphism-1);
    rpos = (g->GetEnvelop(0) + g->GetEnvelop(1))/2.;
    AliDebug(2,Form(" rpos %8.2f : dphi %6.1f degree \n", rpos, dphi));
  }

  if(gn.Contains("WSUC")) {
    par[0] = g->GetPhiModuleSize()*g->GetNPhi()/2.; 
    par[1] = g->GetShellThickness()/2.;
    par[2] = g->GetEtaModuleSize()*g->GetNZ()/2. + 5; 

    gMC->Gsvolu("SMOD", "BOX", fIdTmedArr[kIdAIR], par, 3);

    AliDebug(2,Form("SMOD in WSUC : tmed %i | dx %7.2f dy %7.2f dz %7.2f (SMOD, BOX)\n", 
		    fIdTmedArr[kIdAIR], par[0],par[1],par[2]));
    fSmodPar0 = par[0]; 
    fSmodPar1 = par[1];
    fSmodPar2 = par[2];
    nphism   =  g->GetNumberOfSuperModules();
  } else {
    par[2]  = 350./2.; // 11-oct-04 - for 26 division
    AliDebug(2,Form(" par[0] %7.2f (old) \n",  par[0]));
    Float_t *parSM = g->GetSuperModulesPars(); 
    for(int i=0; i<3; i++) par[i] = parSM[i];
  }
  gMC->Gsvolu("SMOD", "BOX", fIdTmedArr[kIdAIR], par, 3);
  AliDebug(2,Form("tmed %i | dx %7.2f dy %7.2f dz %7.2f (SMOD, BOX)\n", 
		  fIdTmedArr[kIdAIR], par[0],par[1],par[2]));
  fSmodPar0 = par[0]; 
  fSmodPar2 = par[2];
  if(g->GetKey110DEG()) { // 12-oct-05
    par1C = par[1];
    par[1] /= 2.;
    gMC->Gsvolu("SM10", "BOX", fIdTmedArr[kIdAIR], par, 3);
    AliDebug(2,Form(" Super module with name \"SM10\" was created too par[1] = %f\n", par[1]));
    par[1] = par1C;
  }
  // Steel plate
  if(g->GetSteelFrontThickness() > 0.0) { // 28-mar-05
    par[0] = g->GetSteelFrontThickness()/2.;
    gMC->Gsvolu("STPL", "BOX", fIdTmedArr[kIdSTEEL], par, 3);
    printf("tmed %i | dx %7.2f dy %7.2f dz %7.2f (STPL) \n", fIdTmedArr[kIdSTEEL], par[0],par[1],par[2]);
    xpos = -(g->GetShellThickness() - g->GetSteelFrontThickness())/2.;
    gMC->Gspos("STPL", 1, "SMOD", xpos, 0.0, 0.0, 0, "ONLY") ;
  }

  int nr=0, nrsmod=0, i0=0;

  // Turn whole super module
  for(int i=i0; i<nphism; i++) {
    if(gn.Contains("WSUC")) {
      xpos = ypos = zpos = 0.0;
      fIdRotm = 0;
      gMC->Gspos("SMOD", 1, mother, xpos, ypos, zpos, fIdRotm, "ONLY") ;
      printf(" fIdRotm %3i phi %6.1f(%5.3f) xpos %7.2f ypos %7.2f zpos %7.2f \n", 
      fIdRotm, phi, phiRad, xpos, ypos, zpos);
      nr++;
    } else { // TRD1 
      TString smName("SMOD"); // 12-oct-05
      if(i==5 && g->GetKey110DEG()) {
        smName = "SM10";
        nrsmod = nr;
        nr     = 0;
      }
      phi    = g->GetArm1PhiMin() + dphi*(2*i+1)/2.; // phi= 70, 90, 110, 130, 150, 170
      phiRad = phi*TMath::Pi()/180.;

      AliMatrix(fIdRotm, 90.0, phi, 90.0, 90.0+phi, 0.0, 0.0);

      xpos = rpos * TMath::Cos(phiRad);
      ypos = rpos * TMath::Sin(phiRad);
      zpos = fSmodPar2; // 21-sep-04
      if(i==5 && g->GetKey110DEG()) {
        xpos += (par1C/2. * TMath::Sin(phiRad)); 
        ypos -= (par1C/2. * TMath::Cos(phiRad)); 
      }
      
      // 1th module in z-direction;
      gMC->Gspos(smName.Data(), ++nr, mother, xpos, ypos, zpos, fIdRotm, "ONLY") ;
      AliDebug(3, Form(" %s : %2i fIdRotm %3i phi %6.1f(%5.3f) xpos %7.2f ypos %7.2f zpos %7.2f : i %i \n", 
		       smName.Data(), nr, fIdRotm, phi, phiRad, xpos, ypos, zpos, i));
      // 2th module in z-direction;
      // turn arround X axis; 0<phi<360
      double phiy = 90. + phi + 180.;
      if(phiy>=360.) phiy -= 360.;
      
      AliMatrix(fIdRotm, 90.0, phi, 90.0, phiy, 180.0, 0.0);
      gMC->Gspos(smName.Data(), ++nr, mother, xpos, ypos, -zpos, fIdRotm, "ONLY");
      AliDebug(3, Form(" %s : %2i fIdRotm %3i phiy %6.1f  xpos %7.2f ypos %7.2f zpos %7.2f \n", 
		       smName.Data(), nr, fIdRotm, phiy, xpos, ypos, -zpos));
    }
  }
  AliDebug(2,Form(" Number of Super Modules %i \n", nr+nrsmod));
}

//______________________________________________________________________
void AliEMCALv0::CreateEmod(const char* mother, const char* child)
{ 
  // 17-may-05; mother="SMOD"; child="EMOD"
  // Oct 26,2010 
  AliEMCALGeometry * g = GetGeometry(); 
  TString gn(g->GetName()); gn.ToUpper(); 
  // Module definition
  Double_t xpos=0., ypos=0., zpos=0.;
  //Double_t trd1Angle = g->GetTrd1Angle()*TMath::DegToRad();tanTrd1 = TMath::Tan(trd1Angle/2.);

  if(strcmp(mother,"SMOD")==0) {
    fParEMOD[0] = g->GetEtaModuleSize()/2.;   // dx1
    fParEMOD[1] = g->Get2Trd1Dx2()/2.;        // dx2
    fParEMOD[2] = g->GetPhiModuleSize()/2.;;  // dy
    fParEMOD[3] = g->GetLongModuleSize()/2.;  // dz
    gMC->Gsvolu(child, "TRD1", fIdTmedArr[kIdSTEEL], fParEMOD, 4);
  }
  
  int nr=0;
  fIdRotm=0;
  // X->Z(0, 0); Y->Y(90, 90); Z->X(90, 0)
  AliEMCALShishKebabTrd1Module *mod=0; // current module

  for(int iz=0; iz<g->GetNZ(); iz++) {
    Double_t  angle=90., phiOK=0;
    mod = (AliEMCALShishKebabTrd1Module*)fShishKebabModules->At(iz);
    angle = mod->GetThetaInDegree();
    if(!gn.Contains("WSUC")) { // ALICE 
      AliMatrix(fIdRotm, 90.-angle,180., 90.0,90.0, angle, 0.);
      phiOK = mod->GetCenterOfModule().Phi()*180./TMath::Pi(); 
      //          printf(" %2i | angle | %6.3f - %6.3f = %6.3f(eta %5.3f)\n", 
      //iz+1, angle, phiOK, angle-phiOK, mod->GetEtaOfCenterOfModule());
      xpos = mod->GetPosXfromR() + g->GetSteelFrontThickness() - fSmodPar0;
      zpos = mod->GetPosZ() - fSmodPar2;
      
      int iyMax = g->GetNPhi();
      if(strcmp(mother,"SMOD") && g->GetKey110DEG()) {
	iyMax /= 2;
      }
      for(int iy=0; iy<iyMax; iy++) { // flat in phi
	ypos = g->GetPhiModuleSize()*(2*iy+1 - iyMax)/2.;
	gMC->Gspos(child, ++nr, mother, xpos, ypos, zpos, fIdRotm, "ONLY") ;
	//printf(" %2i xpos %7.2f ypos %7.2f zpos %7.2f fIdRotm %i\n", nr, xpos, ypos, zpos, fIdRotm);
	AliDebug(3,Form("%3.3i(%2.2i,%2.2i) ", nr,iy+1,iz+1));
      }
      //PH          printf("\n");
    } else { //WSUC
      if(iz==0) AliMatrix(fIdRotm, 0.,0., 90.,0., 90.,90.); // (x')z; y'(x); z'(y)
      else      AliMatrix(fIdRotm, 90-angle,270., 90.0,0.0, angle,90.);
      phiOK = mod->GetCenterOfModule().Phi()*180./TMath::Pi(); 
      printf(" %2i | angle -phiOK | %6.3f - %6.3f = %6.3f(eta %5.3f)\n", 
	     iz+1, angle, phiOK, angle-phiOK, mod->GetEtaOfCenterOfModule());
      zpos = mod->GetPosZ()      - fSmodPar2;
      ypos = mod->GetPosXfromR() - fSmodPar1;
      printf(" zpos %7.2f ypos %7.2f fIdRotm %i\n xpos ", zpos, xpos, fIdRotm);
      for(int ix=0; ix<g->GetNPhi(); ix++) { // flat in phi
	xpos = g->GetPhiModuleSize()*(2*ix+1 - g->GetNPhi())/2.;
	gMC->Gspos(child, ++nr, mother, xpos, ypos, zpos, fIdRotm, "ONLY") ;
	printf(" %7.2f ", xpos);
      }
      printf("\n");
    }
  }
  AliDebug(2,Form(" Number of modules in Super Module(%s) %i \n", mother, nr));
}

void AliEMCALv0::CreateAlFrontPlate(const char* mother, const char* child)
{
  // Oct 26,2010 : Al front plate : ALFP
  AliEMCALGeometry * g = GetGeometry(); 
  TString gn(g->GetName()); gn.ToUpper(); 
  Double_t trd1Angle = g->GetTrd1Angle()*TMath::DegToRad(), tanTrd1 = TMath::Tan(trd1Angle/2.);
  Double_t parALFP[5], zposALFP=0.;

  parALFP[0] = g->GetEtaModuleSize()/2. - g->GetLateralSteelStrip();    // dx1
  parALFP[1] = parALFP[0]               + tanTrd1*g->GetTrd1AlFrontThick(); // dx2
  parALFP[2] = g->GetPhiModuleSize()/2. - g->GetLateralSteelStrip();    // dy
  parALFP[3] = g->GetTrd1AlFrontThick()/2.; // dz
  gMC->Gsvolu(child, "TRD1", fIdTmedArr[kIdAL], parALFP, 4);
  zposALFP   = -fParEMOD[3] + g->GetTrd1AlFrontThick()/2.;
  gMC->Gspos (child, 1, mother, 0.0, 0.0, zposALFP, 0, "ONLY");
}

//______________________________________________________________________
void AliEMCALv0::Trd1Tower3X3(const double *parSCM0)
{
  // Started Dec 8,2004 by PAI
  // Fixed Nov 13,2006
  printf(" AliEMCALv0::Trd1Tower3X3() : parSCM0");
  for(int i=0; i<4; i++) printf(" %7.4f ", parSCM0[i]);
  printf("\n"); 
  // Nov 10, 2006 - different name of SCMX
  double parTRAP[11], *dummy=0;
  AliEMCALGeometry * g = GetGeometry(); 
  TString gn(g->GetName()), scmx; 
  gn.ToUpper(); 
 // Division to tile size 
  AliDebug(2,Form("Trd1Tower3X3() : Divide SCM0 on y-axis %i", g->GetNETAdiv()));
  gMC->Gsdvn("SCMY","SCM0", g->GetNETAdiv(), 2); // y-axis
  double dx1=parSCM0[0], dx2=parSCM0[1], dy=parSCM0[2], dz=parSCM0[3];
  double ndiv=3., xpos=0.0;
  // should be defined once
  gMC->Gsvolu("PBTI", "BOX", fIdTmedArr[kIdPB], dummy, 0);
  for(int ix=1; ix<=3; ix++) { // 3X3
    scmx = "SCX"; // Nov 10,2006 
    // ix=1
    parTRAP[0] = dz;
    double xCentBot = 2.*dx1/3.;
    double xCentTop = 2.*(dx2/4. + dx1/12.);
    parTRAP[1] = TMath::ATan2((xCentTop-xCentBot),2.*dz)*TMath::RadToDeg(); // theta
    parTRAP[2] = 0.;           // phi
    // bottom
    parTRAP[3] = dy/ndiv;      // H1
    parTRAP[4] = dx1/ndiv;     // BL1
    parTRAP[5] = parTRAP[4];   // TL1
    parTRAP[6] = 0.0;          // ALP1
    // top
    parTRAP[7] = dy/ndiv;      // H2
    parTRAP[8] = dx2/2 - dx1/6.;// BL2
    parTRAP[9] = parTRAP[8];   // TL2
    parTRAP[10]= 0.0;          // ALP2
    xpos = (xCentBot+xCentTop)/2.;

    if      (ix==3) {
      parTRAP[1] = -parTRAP[1];
      xpos = -xpos;
    } else if(ix==2) { // central part is box but we treat as trapesoid due to numbering
      parTRAP[1] = 0.;
      parTRAP[8] = dx1/ndiv;     // BL2
      parTRAP[9] = parTRAP[8];   // TL2
      xpos = 0.0;
    }
    AliDebug(2,Form(" ** TRAP ** xpos %9.3f\n", xpos));
    for(int i=0; i<11; i++) AliDebug(2,Form(" par[%2.2i] %9.4f\n", i, parTRAP[i]));

    scmx += ix;
    gMC->Gsvolu(scmx.Data(), "TRAP", fIdTmedArr[kIdSC], parTRAP, 11);
    gMC->Gspos(scmx.Data(), 1, "SCMY", xpos, 0.0, 0.0, 0, "ONLY") ;

    PbInTrap(parTRAP, scmx);
  }
  AliDebug(2,"Trd1Tower3X3 - Ver. 1.0 : was tested.");
}

// 8-dec-04 by PAI
//______________________________________________________________________
void AliEMCALv0::PbInTrap(const double parTRAP[11], TString n)
{
 // see for example CreateShishKebabGeometry(); just for case TRD1
  static int nr=0;
  AliDebug(2,Form(" Pb tiles : nrstart %i\n", nr));
  AliEMCALGeometry * g = GetGeometry(); 

  double par[3];
  double xpos = 0.0, ypos = 0.0;
  double zpos = -fSampleWidth*g->GetNECLayers()/2. + g->GetECPbRadThick()/2.;

  double coef = (parTRAP[8] -  parTRAP[4]) / (2.*parTRAP[0]);
  double xCenterSCMX =  (parTRAP[4] +  parTRAP[8])/2.; // ??
  //  double tan = TMath::Tan(parTRAP[1]*TMath::DegToRad());

  par[1] = parTRAP[3];              // y 
  par[2] = g->GetECPbRadThick()/2.; // z
  for(int iz=0; iz<g->GetNECLayers(); iz++){
    par[0] = parTRAP[4] + coef*fSampleWidth*iz;
    xpos   = par[0] - xCenterSCMX;
    if(parTRAP[1] < 0.) xpos = -xpos;
    gMC->Gsposp("PBTI", ++nr, n.Data(), xpos, ypos, zpos, 0, "ONLY", par, 3) ;
    AliDebug(2,Form(" %i xpos %9.3f zpos %9.3f par[0] %9.3f |", iz+1, xpos, zpos, par[0]));
    zpos += fSampleWidth;
    if(iz%2>0) printf("\n");
  } 
  AliDebug(2,Form(" Number of Pb tiles in SCMX %i coef %9.7f \n", nr, coef));
  AliDebug(2,Form(" par[1] %9.3f  par[2] %9.3f ypos %9.3f \n", par[1], par[2], ypos)); 
  AliDebug(2,Form(" PbInTrap Ver. 1.0 : was tested."));
}

// 8-dec-04 by PAI
//______________________________________________________________________
void AliEMCALv0::Trd1Tower4X4() const
{
 // Not ready yet
}

//______________________________________________________________________
void AliEMCALv0::Trd1Tower1X1(double *parSCM0)
{
  // Started Nov 22,2006 by PAI
  AliDebug(1," AliEMCALv0::Trd1Tower1X1() : parSCM0");
  for(int i=0; i<4; i++) printf(" %7.4f ", parSCM0[i]);
  printf("\n"); 

  // No division - keeping the same volume logic 
  // and as consequence the same abs is scheme
  AliDebug(2,"Trd1Tower1X1() : Create SCMX(SCMY) as SCM0");

  gMC->Gsvolu("SCMY", "TRD1", fIdTmedArr[kIdAIR], parSCM0, 4);
  gMC->Gspos("SCMY", 1, "SCM0", 0.0, 0.0, 0.0, 0, "ONLY");
  gMC->Gsvolu("SCMX", "TRD1", fIdTmedArr[kIdSC], parSCM0, 4);
  gMC->Gspos("SCMX", 1, "SCMY", 0.0, 0.0, 0.0, 0, "ONLY");

  // should be defined once
  double *dummy=0;
  gMC->Gsvolu("PBTI", "BOX", fIdTmedArr[kIdPB], dummy, 0);

  PbInTrd1(parSCM0, "SCMX");

  AliDebug(1,"Trd1Tower1X1() : Ver. 0.1 : was tested.");
}

//______________________________________________________________________
void AliEMCALv0::PbInTrd1(const double *parTrd1, TString n)
{
 // see PbInTrap(const double parTrd1[11], TString n)
  static int nr=0, ndeb=2;
  AliDebug(ndeb,Form(" Pb tiles : nrstart %i\n", nr));
  AliEMCALGeometry * g = GetGeometry(); 

  double par[3];
  double xpos = 0.0, ypos = 0.0;
  double zpos = -fSampleWidth*g->GetNECLayers()/2. + g->GetECPbRadThick()/2.;
  double coef = (parTrd1[1] -  parTrd1[0]) / (2.*parTrd1[3]);

  par[1] = parTrd1[2];              // y 
  par[2] = g->GetECPbRadThick()/2.; // z

  for(int iz=0; iz<g->GetNECLayers(); iz++){
    par[0] = parTrd1[0] + coef*fSampleWidth*iz;
    gMC->Gsposp("PBTI", ++nr, n.Data(), xpos, ypos, zpos, 0, "ONLY", par, 3) ;
    AliDebug(2,Form(" %i xpos %9.3f zpos %9.3f par[0] %9.3f |", iz+1, xpos, zpos, par[0]));
    zpos += fSampleWidth;
    if(iz%2>0) printf("\n");
  } 
  AliDebug(ndeb,Form(" Number of Pb tiles in SCMX %i coef %9.7f ", nr, coef));
  AliDebug(ndeb,Form(" PbInTrd1 Ver. 0.1 : was tested."));
}

// 3-feb-05
//______________________________________________________________________
void AliEMCALv0::Scm0InTrd2(const AliEMCALGeometry * g, const Double_t emodPar[5], Double_t parSCM0[5])
{
  // Passive material inside the detector
  double wallThickness = g->GetPhiModuleSize()/2. -  g->GetPhiTileSize(); //Need check
  AliDebug(2,Form(" wall thickness %7.5f \n", wallThickness));
  for(int i=0; i<4; i++) { // on pictures sometimes I can not see 0 -> be carefull!!
    parSCM0[i] = emodPar[i] - wallThickness;
    AliDebug(2,Form(" %i parSCMO %7.3f emodPar %7.3f : dif %7.3f \n", 
		    i, parSCM0[i],emodPar[i], parSCM0[i]-emodPar[i]));
  }
  parSCM0[4] = emodPar[4];
  gMC->Gsvolu("SCM0", "TRD2", fIdTmedArr[kIdSC], parSCM0, 5); // kIdAIR -> kIdSC
  gMC->Gspos("SCM0", 1, "EMOD", 0., 0., 0., 0, "ONLY") ;
  // Division 
  if(g->GetNPHIdiv()==2 && g->GetNETAdiv()==2) {
    Division2X2InScm0(g, parSCM0);
  } else {
    Info("Scm0InTrd2"," no division SCM0 in this geometry |%s|\n", g->GetName());
    assert(0);
  }
}

//______________________________________________________________________
void AliEMCALv0::Division2X2InScm0(const AliEMCALGeometry * g, const Double_t parSCM0[5])
{
  // Division 2X2
  Double_t parTRAP[11], xpos=0.,ypos=0., dx1=0.0,dx2=0.,dy1=0.0,dy2=0.,dz=0.
  ,dr1=0.0,dr2=0.;
  fIdRotm=0;

  Info("Division2X2InScm0","Divide SCM0 on y-axis %i\n", g->GetNETAdiv());
  TString n("SCMX"), overLapFlagSCMY("ONLY"), overLapFlagSCMX("ONLY");
  n = "SCM0"; // for testing - 14-mar-05
  if(n=="SCM0"){
    PbInTrapForTrd2(parSCM0, n);
    // overLapFlagSCMY=overLapFlagSCMX="MANY"; // do not work
    return;
  }

  dy1 = parSCM0[2] , dy2 = parSCM0[3], dz = parSCM0[4];

  parTRAP[0] = parSCM0[4];    // dz
  parTRAP[1] = TMath::ATan2((dy2-dy1)/2.,2.*dz)*TMath::RadToDeg();
  parTRAP[2] = 90.;           // phi
  // bottom
  parTRAP[3] = parSCM0[2]/2.; // H1
  parTRAP[4] = parSCM0[0];    // BL1
  parTRAP[5] = parTRAP[4];    // TL1
  parTRAP[6] = 0.0;           // ALP1
  // top
  parTRAP[7] = parSCM0[3]/2.; // H2
  parTRAP[8] = parSCM0[1];    // BL2
  parTRAP[9] = parTRAP[8];    // TL2
  parTRAP[10]= 0.0;           // ALP2
  AliDebug(2,Form(" ** SCMY ** \n"));
	   for(int i=0; i<11; i++) AliDebug(2,Form(" par[%2.2i] %9.4f\n", i, parTRAP[i]));

  fIdRotm=0;
  gMC->Gsvolu("SCMY", "TRAP", fIdTmedArr[kIdSC], parTRAP, 11); // kIdAIR -> kIdSC
  ypos = +(parTRAP[3]+parTRAP[7])/2.; //
  AliDebug(2,Form(" Y shift SCMY inside SCM0 : %7.3f : opt %s\n", ypos, overLapFlagSCMY.Data())); 
  gMC->Gspos("SCMY", 1, "SCM0", 0.0, ypos, 0.0, fIdRotm,  overLapFlagSCMY.Data()) ;
  // Rotation SCMY around z-axis on 180 degree; x'=-x; y'=-y and z=z
  AliMatrix(fIdRotm, 90.0,180., 90.0, 270.0, 0.0,0.0) ;
  // We may have problem with numeration due to rotation - 4-feb-05
  gMC->Gspos("SCMY", 2, "SCM0", 0.0, -ypos, 0.0, fIdRotm,  overLapFlagSCMY.Data()); 

  Info("Division2X2InScm0","Divide SCMY on x-axis %i\n", g->GetNPHIdiv());
  dx1 = parSCM0[0]; 
  dx2 = parSCM0[1]; 
  dr1=TMath::Sqrt(dx1*dx1+dy1*dy1);
  dr2=TMath::Sqrt(dx2*dx2+dy2*dy2);

  parTRAP[0] = parSCM0[4];    // dz
  parTRAP[1] = TMath::ATan2((dr2-dr1)/2.,2.*dz)*TMath::RadToDeg(); // 
  parTRAP[2] = 45.;           // phi
  // bottom
  parTRAP[3] = parSCM0[2]/2.; // H1
  parTRAP[4] = parSCM0[0]/2.; // BL1
  parTRAP[5] = parTRAP[4];    // TL1
  parTRAP[6] = 0.0;           // ALP1
  // top
  parTRAP[7] = parSCM0[3]/2.; // H2
  parTRAP[8] = parSCM0[1]/2;  // BL2
  parTRAP[9] = parTRAP[8];    // TL2
  parTRAP[10]= 0.0;           // ALP2
  AliDebug(2,Form(" ** SCMX ** \n"));
  for(int i=0; i<11; i++) AliDebug(2,Form(" par[%2.2i] %9.4f\n", i, parTRAP[i]));

  fIdRotm=0;
  gMC->Gsvolu("SCMX", "TRAP", fIdTmedArr[kIdSC], parTRAP, 11);
  xpos = (parTRAP[4]+parTRAP[8])/2.;
  AliDebug(2,Form(" X shift SCMX inside SCMX : %7.3f : opt %s\n", xpos, overLapFlagSCMX.Data())); 
  gMC->Gspos("SCMX", 1, "SCMY", xpos, 0.0, 0.0, fIdRotm,  overLapFlagSCMX.Data()) ;
  //  AliMatrix(fIdRotm, 90.0,270., 90.0, 0.0, 0.0,0.0); // x'=-y; y'=x; z'=z
  AliMatrix(fIdRotm, 90.0,90., 90.0, -180.0, 0.0,0.0);     // x'=y;  y'=-x; z'=z
  gMC->Gspos("SCMX", 2, "SCMY", -xpos, 0.0, 0.0, fIdRotm,  overLapFlagSCMX.Data()) ;
  // PB:
  if(n=="SCMX" && overLapFlagSCMY == "ONLY") {
    PbInTrapForTrd2(parTRAP, n);
  }
} 

// 4-feb-05 by PAI
//______________________________________________________________________
void AliEMCALv0::PbInTrapForTrd2(const double *parTRAP, TString name)
{
 // TRD2 cases
  Double_t *dummy=0;
  TString pbShape("BOX"), pbtiChonly("ONLY");
  if(name=="SCM0") {
    pbShape    = "TRD2";
    //    pbtiChonly = "MANY";
  }
  gMC->Gsvolu("PBTI", pbShape.Data(), fIdTmedArr[kIdPB], dummy, 0);

  int nr=0;
  Info("PbInTrapForTrd2"," Pb tiles inside %s: shape %s :pbtiChonly %s\n nrstart %i\n", 
  name.Data(), pbShape.Data(), pbtiChonly.Data(), nr);
  AliEMCALGeometry * g = GetGeometry(); 

  double par[5], parPB[5], parSC[5];
  double xpos = 0.0, ypos = 0.0;
  double zpos = -fSampleWidth*g->GetNECLayers()/2. + g->GetECPbRadThick()/2.;
  if(name == "SCMX") { // common trapezoid - 11 parameters
    double coef = (parTRAP[8] -  parTRAP[4]) / (2.*parTRAP[0]);
    double xCenterSCMX =  (parTRAP[4] +  parTRAP[8])/2.; // the same for y
    AliDebug(2,Form(" xCenterSCMX %8.5f : coef %8.7f \n", xCenterSCMX, coef));

    par[2] = g->GetECPbRadThick()/2.; // z
    for(int iz=0; iz<g->GetNECLayers(); iz++){
      par[0] = parTRAP[4] + coef*fSampleWidth*iz;
      par[1] = par[0];
      xpos   = ypos = par[0] - xCenterSCMX;
    //if(parTRAP[1] < 0.) xpos = -xpos;
      gMC->Gsposp("PBTI", ++nr, name.Data(), xpos, ypos, zpos, 0, "ONLY", par, 3) ;
      AliDebug(2,Form(" %2.2i xpos %8.5f zpos %6.3f par[0,1] %6.3f |", iz+1, xpos, zpos, par[0]));
      if(iz%2>0) AliDebug(2,Form("\n"));
      zpos += fSampleWidth;
    } 
    AliDebug(2,Form(" Number of Pb tiles in SCMX %i coef %9.7f \n", nr, coef));
    AliDebug(2,Form(" par[1] %9.5f  par[2] %9.5f ypos %9.5f \n", par[1], par[2], ypos)); 
  } else if(name == "SCM0") { // 1-mar-05 ; TRD2 - 5 parameters
    AliDebug(2,Form(" SCM0 par = "));
    for(int i=0; i<5; i++) AliDebug(2,Form(" %9.5f ", parTRAP[i]));
    AliDebug(2,Form("\n zpos %f \n",zpos));

    double tanx = (parTRAP[1] -  parTRAP[0]) / (2.*parTRAP[4]); //  tanx =  tany now
    double tany = (parTRAP[3] -  parTRAP[2]) / (2.*parTRAP[4]), ztmp=0.;
    parPB[4] = g->GetECPbRadThick()/2.;
    parSC[2] = g->GetECScintThick()/2.;
    for(int iz=0; iz<g->GetNECLayers(); iz++){
      ztmp     = fSampleWidth*double(iz);
      parPB[0] = parTRAP[0] + tanx*ztmp;
      parPB[1] = parPB[0]   + tanx*g->GetECPbRadThick();
      parPB[2] = parTRAP[2] + tany*ztmp;
      parPB[3] = parPB[2]   + tany*g->GetECPbRadThick();
      gMC->Gsposp("PBTI", ++nr, name.Data(), xpos, ypos, zpos, 0, pbtiChonly.Data(), parPB, 5) ;
      AliDebug(2,Form("\n PBTI %2i | zpos %6.3f | par = ", nr, zpos));
      /*
      for(int i=0; i<5; i++) printf(" %9.5f ", parPB[i]);
      // individual SC tile
      parSC[0] = parPB[0];
      parSC[1] = parPB[1];
      gMC->Gsposp("SCTI", nr, name.Data(), xpos, ypos, zpos+g->GetECScintThick(), 
      0, pbtiChonly.Data(), parSC, 3) ;
      printf("\n SCTI     zpos %6.3f | par = ", zpos+g->GetECScintThick());
      for(int i=0; i<3; i++) printf(" %9.5f ", parPB[i]);
      */
      zpos  += fSampleWidth;
    }
    AliDebug(2,Form("\n"));
  }
  Info("PbInTrapForTrd2", "Ver. 0.03 : was tested.");
}

// 15-mar-05
//______________________________________________________________________
void AliEMCALv0::PbmoInTrd2(const AliEMCALGeometry * g, const Double_t emodPar[5], Double_t parPBMO[5])
{
  // Pb inside Trd2
  Info("PbmoInTrd2"," started : geometry %s ", g->GetName());
  double wallThickness = g->GetPhiModuleSize()/2. -  g->GetPhiTileSize();
  AliDebug(2,Form(" wall thickness %7.5f \n", wallThickness));
  for(int i=0; i<4; i++) {
    parPBMO[i] = emodPar[i] - wallThickness;
    AliDebug(2,Form(" %i parPBMO %7.3f emodPar %7.3f : dif %7.3f \n", 
		    i, parPBMO[i],emodPar[i], parPBMO[i]-emodPar[i]));
  }
  parPBMO[4] = emodPar[4];
  gMC->Gsvolu("PBMO", "TRD2", fIdTmedArr[kIdPB], parPBMO, 5);
  gMC->Gspos("PBMO", 1, "EMOD", 0., 0., 0., 0, "ONLY") ;
  // Division 
  if(g->GetNPHIdiv()==2 && g->GetNETAdiv()==2) {
    Division2X2InPbmo(g, parPBMO);
    AliDebug(2,Form(" PBMO, division 2X2 | geometry |%s|\n", g->GetName()));
  } else {
    AliDebug(2,Form(" no division PBMO in this geometry |%s|\n", g->GetName()));
    assert(0);
  }
}

//______________________________________________________________________
void AliEMCALv0::Division2X2InPbmo(const AliEMCALGeometry * g, const Double_t parPBMO[5]) 
{
  // Division 2X2
  Info("Division2X2InPbmo"," started : geometry %s ", g->GetName());
  //Double_t *dummy=0;
  //  gMC->Gsvolu("SCTI", "BOX", fIdTmedArr[kIdSC], dummy, 0);

  double parSC[3];
  double xpos = 0.0, ypos = 0.0, zpos = 0.0, ztmp=0;;
  double tanx = (parPBMO[1] -  parPBMO[0]) / (2.*parPBMO[4]); //  tanx =  tany now
  double tany = (parPBMO[3] -  parPBMO[2]) / (2.*parPBMO[4]);
  const Int_t buffersize = 10;
  char name[buffersize], named[buffersize], named2[buffersize];

  AliDebug(2,Form(" PBMO par = "));
  for(int i=0; i<5; i++) AliDebug(2,Form(" %9.5f ", parPBMO[i]));
  AliDebug(2,Form("\n"));

  parSC[2] = g->GetECScintThick()/2.;
  zpos = -fSampleWidth*g->GetNECLayers()/2. + g->GetECPbRadThick() + g->GetECScintThick()/2.;
  AliDebug(2,Form(" parSC[2] %9.5f \n", parSC[2]));
  for(int iz=0; iz<g->GetNECLayers(); iz++){
    ztmp     = g->GetECPbRadThick() + fSampleWidth*double(iz); // Z for previous PB
    parSC[0] =  parPBMO[0] + tanx*ztmp;
    parSC[1] =  parPBMO[2] + tany*ztmp;

    snprintf(name,buffersize,"SC%2.2i", iz+1);
    gMC->Gsvolu(name, "BOX", fIdTmedArr[kIdSC], parSC, 3);
    gMC->Gspos(name, 1, "PBMO", xpos, ypos, zpos, 0, "ONLY") ;
    AliDebug(2,Form("%s | zpos %6.3f | parSC[0,1]=(%7.5f,%7.5f) -> ", 
		    name, zpos, parSC[0], parSC[1]));
    
    snprintf(named,buffersize,"SY%2.2i", iz+1);
    printf(" %s -> ", named);
    gMC->Gsdvn(named,name, 2, 2);

    snprintf(named2,buffersize,"SX%2.2i", iz+1);
    printf(" %s \n", named2);
    gMC->Gsdvn(named2,named, 2, 1);

    zpos    += fSampleWidth;
  }
}

//______________________________________________________________________
AliEMCALShishKebabTrd1Module* AliEMCALv0::GetShishKebabModule(Int_t neta)
{ 
  // 28-oct-05
  AliEMCALShishKebabTrd1Module* trd1=0;
  if(fShishKebabModules && neta>=0 && neta<fShishKebabModules->GetSize()) {
    trd1 = (AliEMCALShishKebabTrd1Module*)fShishKebabModules->At(neta);
  }
  return trd1;
}

//_____________________________________________________________________________
void AliEMCALv0::AddAlignableVolumes() const
{
  //Add volumes which are alignable (?)
  TString ntmp(GetTitle()); // name of EMCAL geometry

  if(ntmp.Contains("WSUC")) {
    AddAlignableVolumesInWSUC();  // WSUC case
  } else {
    AddAlignableVolumesInALICE(); // ALICE case
  }
}

//______________________________________________________________________
void AliEMCALv0::AddAlignableVolumesInALICE() const
{
  //
  // Create entries for alignable volumes associating the symbolic volume
  // name with the corresponding volume path. Needs to be synchronized with
  // eventual changes in the geometry.
  //

  Float_t * pars = GetGeometry()->GetSuperModulesPars();
  double rpos = (GetGeometry()->GetEnvelop(0) + GetGeometry()->GetEnvelop(1))/2.;
  double phi, phiRad, xpos, ypos, zpos;

  AliGeomManager::ELayerID idEMCAL = AliGeomManager::kEMCAL;
  Int_t modUID, modnum = 0;
  TString volpath, symname;

  Int_t nSMod = GetGeometry()->GetNumberOfSuperModules(); 
  for (Int_t smodnum=0; smodnum < nSMod; smodnum++) {
    modUID = AliGeomManager::LayerToVolUID(idEMCAL,modnum++);
    volpath = "ALIC_1/XEN1_1/SMOD_";
    volpath += (smodnum+1);
    symname = "EMCAL/FullSupermodule";
    symname += (smodnum+1);

    if(GetGeometry()->GetKey110DEG() && smodnum>=10) {
      volpath = "ALIC_1/XEN1_1/SM10_";
      volpath += (smodnum-10+1);
      symname = "EMCAL/HalfSupermodule";    
      symname += (smodnum-10+1);
    }

    if(!gGeoManager->SetAlignableEntry(symname.Data(),volpath.Data(),modUID))
      AliFatal("AliEMCALv0::Unable to set alignable entry!!");

    // Creates the Tracking to Local transformation matrix for EMCAL
    // modules                         
    TGeoPNEntry *alignableEntry = gGeoManager->GetAlignableEntryByUID(modUID) ;

    phiRad = GetGeometry()->GetPhiCenterOfSM(smodnum);  //comes in radians, not degrees
    phi = phiRad*180./TMath::Pi();             //need degrees for rot. matrix
    xpos = rpos * TMath::Cos(phiRad);
    ypos = rpos * TMath::Sin(phiRad);
    zpos = pars[2];
    if(GetGeometry()->GetKey110DEG() && smodnum >= 10) {
      xpos += (pars[1]/2. * TMath::Sin(phiRad));
      ypos -= (pars[1]/2. * TMath::Cos(phiRad));
    }

    TGeoHMatrix *matTtoL;
    TGeoHMatrix *globMatrix = alignableEntry->GetGlobalOrig();

    if(smodnum%2 == 0) {
      // pozitive z                                                        
      TGeoTranslation geoTran0(xpos,ypos, zpos);                        
      TGeoRotation geoRot0("geoRot0", 90.0, phi, 90.0, 90.0+phi, 0.0, 0.0);
      TGeoCombiTrans mat0(geoTran0, geoRot0);
      matTtoL = new TGeoHMatrix(mat0);

      matTtoL->MultiplyLeft(&(globMatrix->Inverse()));
      alignableEntry->SetMatrix(matTtoL);

    } else {
      // negative z                                                                                 
      double phiy = 90. + phi + 180.;
      if(phiy>=360.) phiy -= 360.;
      TGeoTranslation geoTran1(xpos,ypos,-zpos);
      TGeoRotation geoRot1("geoRot1", 90.0, phi, 90.0, phiy, 180.0, 0.0);
      TGeoCombiTrans mat1(geoTran1, geoRot1);
      matTtoL = new TGeoHMatrix(mat1);

      matTtoL->MultiplyLeft(&(globMatrix->Inverse()));
      alignableEntry->SetMatrix(matTtoL);

    }

  }

}

//______________________________________________________________________
void AliEMCALv0::AddAlignableVolumesInWSUC() const
{
  //
  // Create entries for alignable volumes associating the symbolic volume
  // name with the corresponding volume path. Needs to be synchronized with
  // eventual changes in the geometry.
  //

  TString vpstr1 = "WSUC_1/XEN1_1/SMOD_";
  TString snstr1 = "EMCAL/CosmicTestSupermodule";
  TString volpath, symname;

  // #SM is just one 
  for (Int_t smodnum=0; smodnum < 1; smodnum++) {
    symname = snstr1;
    symname += (smodnum+1);
    volpath = vpstr1;
    volpath += (smodnum+1);
    if(!gGeoManager->SetAlignableEntry(symname.Data(),volpath.Data()))
      AliFatal("Unable to set alignable entry!!");
  }
}

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
Revision 1.44  2003/01/28 14:38:18  cblume
Add track length to track references

Revision 1.43  2002/11/21 22:38:47  alibrary
Removing AliMC and AliMCProcess

Revision 1.42  2002/10/22 15:53:08  alibrary
Introducing Riostream.h

Revision 1.41  2002/10/14 14:57:43  hristov
Merging the VirtualMC branch to the main development branch (HEAD)

Revision 1.36.6.2  2002/07/24 10:09:30  alibrary
Updating VirtualMC

Revision 1.40  2002/06/13 08:11:56  cblume
Add the track references

Revision 1.39  2002/06/12 09:54:35  cblume
Update of tracking code provided by Sergei

Revision 1.38  2002/03/28 14:59:07  cblume
Coding conventions

Revision 1.37  2002/03/25 20:01:49  cblume
Introduce parameter class

Revision 1.36  2002/02/11 14:25:27  cblume
Geometry update, compressed hit structure

Revision 1.35  2001/11/14 12:08:44  cblume
Remove unneccessary header files

Revision 1.34  2001/11/14 10:50:45  cblume
Changes in digits IO. Add merging of summable digits

Revision 1.33  2001/11/06 17:19:41  cblume
Add detailed geometry and simple simulator

Revision 1.32  2001/10/08 06:57:33  hristov
Branches for  TRD digits are created only during the digitisation

Revision 1.31  2001/08/30 09:30:30  hristov
The split level of branches is set to 99

Revision 1.30  2001/05/28 17:07:58  hristov
Last minute changes; ExB correction in AliTRDclusterizerV1; taking into account of material in G10 TEC frames and material between TEC planes (C.Blume,S.Sedykh)

Revision 1.29  2001/05/21 16:45:47  hristov
Last minute changes (C.Blume)

Revision 1.28  2001/05/16 14:57:27  alibrary
New files for folders and Stack

Revision 1.27  2001/05/08 07:05:02  hristov
Loop variable declared once (HP, Sun)

Revision 1.26  2001/05/07 08:03:22  cblume
Generate also hits in the amplification region

Revision 1.25  2001/03/13 09:30:35  cblume
Update of digitization. Moved digit branch definition to AliTRD

Revision 1.24  2001/01/26 19:56:49  hristov
Major upgrade of AliRoot code

Revision 1.23  2000/11/01 14:53:20  cblume
Merge with TRD-develop

Revision 1.17.2.6  2000/10/15 23:29:08  cblume
Introduced more detailed geometry for the display

Revision 1.17.2.5  2000/10/06 16:49:46  cblume
Made Getters const

Revision 1.17.2.4  2000/10/04 16:34:57  cblume
Replace include files by forward declarations

Revision 1.17.2.3  2000/09/22 14:45:17  cblume
Included changes for the tracking

Revision 1.17.2.2  2000/09/18 13:25:13  cblume
Included LoadPoints() method to display the TR photons

Revision 1.22  2000/10/02 21:28:19  fca
Removal of useless dependecies via forward declarations

Revision 1.21  2000/06/09 11:10:07  cblume
Compiler warnings and coding conventions, next round

Revision 1.20  2000/06/08 18:32:57  cblume
Make code compliant to coding conventions

Revision 1.19  2000/06/07 16:25:37  cblume
Try to remove compiler warnings on Sun and HP

Revision 1.18  2000/05/08 16:17:27  cblume
Merge TRD-develop

Revision 1.21  2000/06/09 11:10:07  cblume
Compiler warnings and coding conventions, next round

Revision 1.20  2000/06/08 18:32:57  cblume
Make code compliant to coding conventions

Revision 1.19  2000/06/07 16:25:37  cblume
Try to remove compiler warnings on Sun and HP

Revision 1.18  2000/05/08 16:17:27  cblume
Merge TRD-develop

Revision 1.17.2.1  2000/05/08 14:28:59  cblume
Introduced SetPHOShole() and SetRICHhole(). AliTRDrecPoint container is now a TObjArray

Revision 1.17  2000/02/28 19:10:26  cblume
Include the new TRD classes

Revision 1.16.2.2  2000/02/28 17:53:24  cblume
Introduce TRD geometry classes

Revision 1.16.2.1  2000/02/28 17:04:19  cblume
Include functions and data members for AliTRDrecPoint

Revision 1.16  2000/01/19 17:17:35  fca
Introducing a list of lists of hits -- more hits allowed for detector now

Revision 1.15  1999/11/02 17:04:25  fca
Small syntax change for HP compiler

Revision 1.14  1999/11/02 16:57:02  fca
Avoid non ansi warnings on HP compilers

Revision 1.13  1999/11/02 16:35:56  fca
New version of TRD introduced

Revision 1.12  1999/11/01 20:41:51  fca
Added protections against using the wrong version of FRAME

Revision 1.11  1999/09/29 09:24:34  fca
Introduction of the Copyright and cvs Log

*/

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  Transition Radiation Detector                                            //
//  This class contains the basic functions for the Transition Radiation     //
//  Detector.                                                                //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <Riostream.h>

#include <TMath.h>
#include <TNode.h>
#include <TGeometry.h>
#include <TTree.h>                                                              
#include <TPGON.h> 
#include <TFile.h>
#include <TROOT.h>
#include <TParticle.h>
#include <TLorentzVector.h>

#include "AliRun.h"
#include "AliConst.h"
#include "AliDigit.h"
#include "AliMagF.h"

#include "AliTrackReference.h"
 
#include "AliTRD.h"
#include "AliTRDhit.h"
#include "AliTRDpoints.h"
#include "AliTRDdigit.h"
#include "AliTRDdigitizer.h"
#include "AliTRDclusterizer.h"
#include "AliTRDgeometryHole.h"
#include "AliTRDgeometryFull.h"
#include "AliTRDrecPoint.h"
#include "AliTRDcluster.h"
#include "AliTRDdigitsManager.h"
#include "AliTRDtrackHits.h"  

ClassImp(AliTRD)
 
//_____________________________________________________________________________
AliTRD::AliTRD()
{
  //
  // Default constructor
  //

  fIshunt        = 0;
  fGasMix        = 0;
  fHits          = 0;
  fDigits        = 0;

  fRecPoints     = 0;
  fNRecPoints    = 0;

  fGeometry      = 0;

  fGasDensity    = 0;
  fFoilDensity   = 0;

  fDrawTR        = 0;
  fDisplayType   = 0;
 
  fTrackHits     = 0; 
  fHitType       = 0; 

}
 
//_____________________________________________________________________________
AliTRD::AliTRD(const char *name, const char *title)
       : AliDetector(name,title)
{
  //
  // Standard constructor for the TRD
  //

  // Check that FRAME is there otherwise we have no place where to
  // put TRD
  AliModule* frame = gAlice->GetModule("FRAME");
  if (!frame) {
    Error("Ctor","TRD needs FRAME to be present\n");
    exit(1);
  } 

  // Define the TRD geometry according to the FRAME geometry
  if      (frame->IsVersion() == 0) {
    // Geometry with hole
    fGeometry = new AliTRDgeometryHole();
  }
  else if (frame->IsVersion() == 1) {
    // Geometry without hole
    fGeometry = new AliTRDgeometryFull();
  }
  else {
    Error("Ctor","Could not find valid FRAME version\n");
    exit(1);
  }

  // Allocate the hit array
  fHits           = new TClonesArray("AliTRDhit"     ,405);
  gAlice->AddHitList(fHits);

  // Allocate the digits array
  fDigits         = 0;

  // Allocate the rec point array
  fRecPoints     = new TObjArray(400);
  fNRecPoints    = 0;
   
  fIshunt        = 0;
  fGasMix        = 1;

  fGasDensity    = 0;
  fFoilDensity   = 0;

  fDrawTR        = 0;
  fDisplayType   = 0;

  fTrackHits     = 0;
  fHitType       = 2;

  SetMarkerColor(kWhite);   

}

//_____________________________________________________________________________
AliTRD::AliTRD(const AliTRD &trd)
{
  //
  // Copy constructor
  //

  ((AliTRD &) trd).Copy(*this);

}

//_____________________________________________________________________________
AliTRD::~AliTRD()
{
  //
  // TRD destructor
  //

  fIshunt = 0;

  if (fGeometry) {
    delete fGeometry;
    fGeometry  = 0;
  }
  if (fHits) {
    delete fHits;
    fHits      = 0;
  }
  if (fRecPoints) {
    delete fRecPoints;
    fRecPoints = 0;
  }
  if (fTrackHits) {
    delete fTrackHits;
    fTrackHits = 0;
  }

}

//_____________________________________________________________________________
void AliTRD::AddCluster(Float_t *pos, Int_t det, Float_t amp
                      , Int_t *tracks, Float_t *sig, Int_t iType)
{
  //
  // Add a cluster for the TRD
  //

  AliTRDcluster *c = new AliTRDcluster();

  c->SetDetector(det);
  c->AddTrackIndex(tracks);
  c->SetQ(amp);
  c->SetY(pos[0]);
  c->SetZ(pos[1]);
  c->SetSigmaY2(sig[0]);   
  c->SetSigmaZ2(sig[1]);
  c->SetLocalTimeBin(((Int_t) pos[2]));

  switch (iType) {
  case 0:
    c->Set2pad();
    break;
  case 1:
    c->Set3pad();
    break;
  case 2:
    c->Set4pad();
    break;
  case 3:
    c->Set5pad();
    break;
  case 4:
    c->SetLarge();
    break;
  };

  fRecPoints->Add(c);

}

//_____________________________________________________________________________
void  AliTRD::AddTrackReference(Int_t label, TVirtualMC *vMC)
{
  //
  // Add a trackrefernce to the list
  //

  if (!fTrackReferences) {
    Error("AddTrackReference","Container fTrackRefernce not active\n");
    return;
  }

  Int_t nref = fTrackReferences->GetEntriesFast();
  TClonesArray &lref = *fTrackReferences;
  new(lref[nref]) AliTrackReference(label, vMC);
}

//_____________________________________________________________________________
void AliTRD::Hits2Digits()
{
  //
  // Create digits
  //

  AliTRDdigitizer *digitizer = new AliTRDdigitizer("TRDdigitizer"
                                                  ,"TRD digitizer class");
  digitizer->SetDebug(GetDebug());
  digitizer->SetEvent(gAlice->GetEvNumber());

  // Initialization
  digitizer->InitDetector();
    
  // Create the digits
  digitizer->MakeDigits();
  
  // Write the digits into the input file
  if (digitizer->MakeBranch(fDigitsFile)) {

    digitizer->WriteDigits();

    // Save the digitizer class in the AliROOT 
    digitizer->Write();

  }

}

//_____________________________________________________________________________
void AliTRD::Hits2SDigits()
{
  //
  // Create summable digits
  //

  AliTRDdigitizer *digitizer = new AliTRDdigitizer("TRDdigitizer"
                                                  ,"TRD digitizer class");
  digitizer->SetDebug(GetDebug());

  // For the summable digits
  digitizer->SetSDigits(kTRUE);
  digitizer->SetEvent(gAlice->GetEvNumber());

  // Initialization
  digitizer->InitDetector();
    
  // Create the TRD s-digits branch
  digitizer->MakeDigits();
  
  // Write the digits into the input file
  if (digitizer->MakeBranch(fDigitsFile)) {

    digitizer->WriteDigits();

    // Save the digitizer class in the AliROOT 
    digitizer->Write();

  }

}

//_____________________________________________________________________________
void AliTRD::SDigits2Digits()
{
  //
  // Create final digits from summable digits
  //

   // Create the TRD digitizer
  AliTRDdigitizer *digitizer = new AliTRDdigitizer("TRDdigitizer"
                                                  ,"TRD digitizer class");  
  digitizer->SetDebug(GetDebug());

  // Set the parameter
  digitizer->SetEvent(gAlice->GetEvNumber());

  // Initialization
  digitizer->InitDetector();

  // Read the s-digits via digits manager
  AliTRDdigitsManager *sdigitsManager = new AliTRDdigitsManager();
  sdigitsManager->SetDebug(GetDebug());
  sdigitsManager->SetSDigits(kTRUE);
  if (fDigitsFile) {
    sdigitsManager->Open(fDigitsFile);
  }
  sdigitsManager->CreateArrays();
  sdigitsManager->ReadDigits();

  // Add the s-digits to the input list 
  digitizer->AddSDigitsManager(sdigitsManager);

  // Convert the s-digits to normal digits
  digitizer->SDigits2Digits();

  // Store the digits
  if (digitizer->MakeBranch(fDigitsFile)) {

    digitizer->WriteDigits();

  }

}

//_____________________________________________________________________________
void AliTRD::AddHit(Int_t track, Int_t det, Float_t *hits, Int_t q
                  , Bool_t inDrift)
{
  //
  // Add a hit for the TRD
  // 
  // The data structure is set according to fHitType:
  //   bit0: standard TClonesArray
  //   bit1: compressed trackHits structure
  //

  if (fHitType & 1) {
    TClonesArray &lhits = *fHits;
    new(lhits[fNhits++]) AliTRDhit(fIshunt,track,det,hits,q);
  }

  if (fHitType > 1) {
    AddHit2(track,det,hits,q,inDrift);
  }

}

//_____________________________________________________________________________
void AliTRD::BuildGeometry()
{
  //
  // Create the ROOT TNode geometry for the TRD
  //

  TNode *node, *top;
  TPGON *pgon;

  Float_t rmin, rmax;
  Float_t zmax1, zmax2;

  Int_t   iPlan;
 
  const Int_t kColorTRD = 46;
  
  // Find the top node alice
  top = gAlice->GetGeometry()->GetNode("alice");
  
  if      (fDisplayType == 0) {

    pgon = new TPGON("S_TRD","TRD","void",0,360,AliTRDgeometry::Nsect(),4);
    rmin = AliTRDgeometry::Rmin();
    rmax = AliTRDgeometry::Rmax();
    pgon->DefineSection(0,-AliTRDgeometry::Zmax1(),rmax,rmax);
    pgon->DefineSection(1,-AliTRDgeometry::Zmax2(),rmin,rmax);
    pgon->DefineSection(2, AliTRDgeometry::Zmax2(),rmin,rmax);
    pgon->DefineSection(3, AliTRDgeometry::Zmax1(),rmax,rmax);
    top->cd();
    node = new TNode("TRD","TRD","S_TRD",0,0,0,"");
    node->SetLineColor(kColorTRD);
    fNodes->Add(node);

  }
  else if (fDisplayType == 1) {

    Char_t name[7];

    Float_t slope = (AliTRDgeometry::Zmax1() - AliTRDgeometry::Zmax2())
                  / (AliTRDgeometry::Rmax()  - AliTRDgeometry::Rmin());

    rmin  = AliTRDgeometry::Rmin() + AliTRDgeometry::CraHght();
    rmax  = rmin                   + AliTRDgeometry::CdrHght();

    Float_t thickness = rmin - AliTRDgeometry::Rmin();
    zmax2 = AliTRDgeometry::Zmax2() + slope * thickness;
    zmax1 = zmax2 + slope * AliTRDgeometry::DrThick();

    for (iPlan = 0; iPlan < AliTRDgeometry::Nplan(); iPlan++) {

      sprintf(name,"S_TR1%d",iPlan);
      pgon  = new TPGON(name,"TRD","void",0,360,AliTRDgeometry::Nsect(),4);
      pgon->DefineSection(0,-zmax1,rmax,rmax);
      pgon->DefineSection(1,-zmax2,rmin,rmax);
      pgon->DefineSection(2, zmax2,rmin,rmax);
      pgon->DefineSection(3, zmax1,rmax,rmax);
      top->cd();
      node = new TNode("TRD","TRD",name,0,0,0,"");
      node->SetLineColor(kColorTRD);
      fNodes->Add(node);

      Float_t height = AliTRDgeometry::Cheight() + AliTRDgeometry::Cspace(); 
      rmin  = rmin  + height;
      rmax  = rmax  + height;
      zmax1 = zmax1 + slope * height;
      zmax2 = zmax2 + slope * height;

    }

    thickness += AliTRDgeometry::DrThick();
    rmin  = AliTRDgeometry::Rmin() + thickness;
    rmax  = rmin + AliTRDgeometry::AmThick();
    zmax2 = AliTRDgeometry::Zmax2() + slope * thickness;
    zmax1 = zmax2 + slope * AliTRDgeometry::AmThick();

    for (iPlan = 0; iPlan < AliTRDgeometry::Nplan(); iPlan++) {

      sprintf(name,"S_TR2%d",iPlan);
      pgon  = new TPGON(name,"TRD","void",0,360,AliTRDgeometry::Nsect(),4);
      pgon->DefineSection(0,-zmax1,rmax,rmax);
      pgon->DefineSection(1,-zmax2,rmin,rmax);
      pgon->DefineSection(2, zmax2,rmin,rmax);
      pgon->DefineSection(3, zmax1,rmax,rmax);
      top->cd();
      node = new TNode("TRD","TRD",name,0,0,0,"");
      node->SetLineColor(kColorTRD);
      fNodes->Add(node);

      Float_t height = AliTRDgeometry::Cheight() + AliTRDgeometry::Cspace(); 
      rmin  = rmin  + height;
      rmax  = rmax  + height;
      zmax1 = zmax1 + slope * height;
      zmax2 = zmax2 + slope * height;

    }

  }

}
 
//_____________________________________________________________________________
void AliTRD::Copy(TObject &trd)
{
  //
  // Copy function
  //

  ((AliTRD &) trd).fGasMix      = fGasMix;
  ((AliTRD &) trd).fGeometry    = fGeometry;       
  ((AliTRD &) trd).fRecPoints   = fRecPoints;
  ((AliTRD &) trd).fNRecPoints  = fNRecPoints;
  ((AliTRD &) trd).fGasDensity  = fGasDensity;
  ((AliTRD &) trd).fFoilDensity = fFoilDensity;
  ((AliTRD &) trd).fDrawTR      = fDrawTR;
  ((AliTRD &) trd).fDisplayType = fDisplayType;
  ((AliTRD &) trd).fHitType     = fHitType;

  //AliDetector::Copy(trd);

}

//_____________________________________________________________________________
void AliTRD::CreateGeometry()
{
  //
  // Creates the volumes for the TRD chambers
  //

  // Check that FRAME is there otherwise we have no place where to put the TRD
  AliModule* frame = gAlice->GetModule("FRAME");
  if (!frame) {
    printf(" The TRD needs the FRAME to be defined first\n");
    return;
  }

  fGeometry->CreateGeometry(fIdtmed->GetArray() - 1299);

}
 
//_____________________________________________________________________________
void AliTRD::CreateMaterials()
{
  //
  // Create the materials for the TRD
  // Origin Y.Foka
  //

  Int_t   isxfld = gAlice->Field()->Integ();
  Float_t sxmgmx = gAlice->Field()->Max();
  
  // For polyethilene (CH2) 
  Float_t ape[2] = { 12., 1. };
  Float_t zpe[2] = {  6., 1. };
  Float_t wpe[2] = {  1., 2. };
  Float_t dpe    = 0.95;

  // For mylar (C5H4O2) 
  Float_t amy[3] = { 12., 1., 16. };
  Float_t zmy[3] = {  6., 1.,  8. };
  Float_t wmy[3] = {  5., 4.,  2. };
  Float_t dmy    = 1.39;

  // For CO2 
  Float_t aco[2] = { 12., 16. };
  Float_t zco[2] = {  6.,  8. };
  Float_t wco[2] = {  1.,  2. };
  Float_t dco    = 0.001977;

  // For water
  Float_t awa[2] = {  1., 16. };
  Float_t zwa[2] = {  1.,  8. };
  Float_t wwa[2] = {  2.,  1. };
  Float_t dwa    = 1.0;

  // For isobutane (C4H10)
  Float_t ais[2] = { 12.,  1. };
  Float_t zis[2] = {  6.,  1. };
  Float_t wis[2] = {  4., 10. };
  Float_t dis    = 0.00267;

  // For plexiglas (C5H8O2)
  Float_t apg[3] = { 12.011 ,  1.0    , 15.9994 };
  Float_t zpg[3] = {  6.0   ,  1.0    ,  8.0    };
  Float_t wpg[3] = {  5.0   ,  8.0    ,  2.0    };
  Float_t dpg    = 1.18; 

  // For Xe/CO2-gas-mixture 
  // Xe-content of the Xe/CO2-mixture (85% / 15%) 
  Float_t fxc    = .85;
  // Xe-content of the Xe/Isobutane-mixture (97% / 3%) 
  Float_t fxi    = .97;
  Float_t dxe    = .005858;
  
  // General tracking parameter
  Float_t tmaxfd = -10.;
  Float_t stemax = -1e10;
  Float_t deemax = -0.1;
  Float_t epsil  =  1e-4;
  Float_t stmin  = -0.001;
  
  Float_t absl, radl, d, buf[1];
  Float_t agm[2], zgm[2], wgm[2];
  Float_t dgm1, dgm2;
  Int_t   nbuf;
  
  //////////////////////////////////////////////////////////////////////////
  //     Define Materials 
  //////////////////////////////////////////////////////////////////////////

  AliMaterial( 1, "Al"   ,  26.98, 13.0, 2.7     ,     8.9 ,    37.2);
  AliMaterial( 2, "Air"  ,  14.61,  7.3, 0.001205, 30420.0 , 67500.0);
  AliMaterial( 4, "Xe"   , 131.29, 54.0, dxe     ,  1447.59,     0.0);
  AliMaterial( 5, "Cu"   ,  63.54, 29.0, 8.96    ,     1.43,    14.8);
  AliMaterial( 6, "C"    ,  12.01,  6.0, 2.265   ,    18.8 ,    74.4);
  AliMaterial(12, "G10"  ,  20.00, 10.0, 1.7     ,    19.4 ,   999.0);
  AliMaterial(15, "Sn"   , 118.71, 50.0, 7.31    ,     1.21,    14.8);
  AliMaterial(16, "Si"   ,  28.09, 14.0, 2.33    ,     9.36,    37.2);
  AliMaterial(17, "Epoxy",  17.75,  8.9, 1.8     ,    21.82,   999.0);

  // Mixtures 
  AliMixture(3, "Polyethilene",   ape, zpe, dpe, -2, wpe);
  AliMixture(7, "Mylar",          amy, zmy, dmy, -3, wmy);
  AliMixture(8, "CO2",            aco, zco, dco, -2, wco);
  AliMixture(9, "Isobutane",      ais, zis, dis, -2, wis);
  AliMixture(13,"Water",          awa, zwa, dwa, -2, wwa);
  AliMixture(14,"Plexiglas",      apg, zpg, dpg, -3, wpg);

  // Gas mixtures
  Char_t namate[21];
  // Xe/CO2-mixture
  // Get properties of Xe 
  gMC->Gfmate((*fIdmate)[4], namate, agm[0], zgm[0], d, radl, absl, buf, nbuf);
  // Get properties of CO2 
  gMC->Gfmate((*fIdmate)[8], namate, agm[1], zgm[1], d, radl, absl, buf, nbuf);
  // Create gas mixture 
  wgm[0] = fxc;
  wgm[1] = 1. - fxc;
  dgm1   = wgm[0] * dxe + wgm[1] * dco;
  AliMixture(10, "Gas mixture 1", agm, zgm, dgm1,  2, wgm);
  // Xe/Isobutane-mixture
  // Get properties of Xe 
  gMC->Gfmate((*fIdmate)[4], namate, agm[0], zgm[0], d, radl, absl, buf, nbuf);
  // Get properties of Isobutane
  gMC->Gfmate((*fIdmate)[9], namate, agm[1], zgm[1], d, radl, absl, buf, nbuf);
  // Create gas mixture 
  wgm[0] = fxi;
  wgm[1] = 1. - fxi;
  dgm2   = wgm[0] * dxe + wgm[1] * dis;
  AliMixture(11, "Gas mixture 2", agm, zgm, dgm2,  2, wgm);
 
  //////////////////////////////////////////////////////////////////////////
  //     Tracking Media Parameters 
  //////////////////////////////////////////////////////////////////////////

  // Al Frame 
  AliMedium(1, "Al Frame",   1, 0, isxfld, sxmgmx
                , tmaxfd, stemax, deemax, epsil, stmin);
  // Air 
  AliMedium(2, "Air",        2, 0, isxfld, sxmgmx
                , tmaxfd, stemax, deemax, epsil, stmin);
  // Polyethilene 
  AliMedium(3, "Radiator",   3, 0, isxfld, sxmgmx
                , tmaxfd, stemax, deemax, epsil, stmin);
  // Xe 
  AliMedium(4, "Xe",         4, 1, isxfld, sxmgmx
                , tmaxfd, stemax, deemax, epsil, stmin);
  // Cu pads 
  AliMedium(5, "Padplane",   5, 1, isxfld, sxmgmx
                , tmaxfd, stemax, deemax, epsil, stmin);
  // Fee + cables 
  AliMedium(6, "Readout",    1, 0, isxfld, sxmgmx
                , tmaxfd, stemax, deemax, epsil, stmin);
  // C frame 
  AliMedium(7, "C Frame",    6, 0, isxfld, sxmgmx
                , tmaxfd, stemax, deemax, epsil, stmin);
  // Mylar foils 
  AliMedium(8, "Mylar",      7, 0, isxfld, sxmgmx
                , tmaxfd, stemax, deemax, epsil, stmin);
  if (fGasMix == 1) {
    // Gas-mixture (Xe/CO2) 
    AliMedium(9, "Gas-mix",   10, 1, isxfld, sxmgmx
                  , tmaxfd, stemax, deemax, epsil, stmin);
  }
  else {
    // Gas-mixture (Xe/Isobutane) 
    AliMedium(9, "Gas-mix",   11, 1, isxfld, sxmgmx
                  , tmaxfd, stemax, deemax, epsil, stmin);
  }
  // Nomex-honeycomb (use carbon for the time being) 
  AliMedium(10, "Nomex",      6, 0, isxfld, sxmgmx
                , tmaxfd, stemax, deemax, epsil, stmin);
  // Kapton foils (use Mylar for the time being) 
  AliMedium(11, "Kapton",     7, 0, isxfld, sxmgmx
                , tmaxfd, stemax, deemax, epsil, stmin);
  // Gas-filling of the radiator 
  AliMedium(12, "CO2",        8, 0, isxfld, sxmgmx
                , tmaxfd, stemax, deemax, epsil, stmin);
  // G10-plates
  AliMedium(13, "G10-plates",12, 0, isxfld, sxmgmx
                , tmaxfd, stemax, deemax, epsil, stmin);
  // Cooling water
  AliMedium(14, "Water",     13, 0, isxfld, sxmgmx
                , tmaxfd, stemax, deemax, epsil, stmin);
  // Rohacell (plexiglas) for the radiator
  AliMedium(15, "Rohacell",  14, 0, isxfld, sxmgmx
                , tmaxfd, stemax, deemax, epsil, stmin);
  // Al layer in MCMs
  AliMedium(16, "MCM-Al"  ,   1, 0, isxfld, sxmgmx
                , tmaxfd, stemax, deemax, epsil, stmin);
  // Sn layer in MCMs
  AliMedium(17, "MCM-Sn"  ,  15, 0, isxfld, sxmgmx
                , tmaxfd, stemax, deemax, epsil, stmin);
  // Cu layer in MCMs
  AliMedium(18, "MCM-Cu"  ,   5, 0, isxfld, sxmgmx
                , tmaxfd, stemax, deemax, epsil, stmin);
  // G10 layer in MCMs
  AliMedium(19, "MCM-G10" ,  12, 0, isxfld, sxmgmx
                , tmaxfd, stemax, deemax, epsil, stmin);
  // Si in readout chips
  AliMedium(20, "Chip-Si" ,  16, 0, isxfld, sxmgmx
                , tmaxfd, stemax, deemax, epsil, stmin);
  // Epoxy in readout chips
  AliMedium(21, "Chip-Ep" ,  17, 0, isxfld, sxmgmx
                , tmaxfd, stemax, deemax, epsil, stmin);
  // PE in connectors
  AliMedium(22, "Conn-PE" ,   3, 0, isxfld, sxmgmx
                , tmaxfd, stemax, deemax, epsil, stmin);
  // Cu in connectors
  AliMedium(23, "Chip-Cu" ,   5, 0, isxfld, sxmgmx
                , tmaxfd, stemax, deemax, epsil, stmin);
  // Al of cooling pipes
  AliMedium(24, "Cooling" ,   1, 0, isxfld, sxmgmx
                , tmaxfd, stemax, deemax, epsil, stmin);

  // Save the density values for the TRD absorbtion
  fFoilDensity = dmy;
  if (fGasMix == 1)
    fGasDensity = dgm1;
  else
    fGasDensity = dgm2;

}

//_____________________________________________________________________________
void AliTRD::DrawModule() const
{
  //
  // Draw a shaded view of the Transition Radiation Detector version 0
  //

  // Set everything unseen
  gMC->Gsatt("*"   ,"SEEN",-1);
  
  // Set ALIC mother transparent
  gMC->Gsatt("ALIC","SEEN", 0);
  
  // Set the volumes visible
  if (fGeometry->IsVersion() == 0) {
    gMC->Gsatt("B071","SEEN", 0);
    gMC->Gsatt("B074","SEEN", 0);
    gMC->Gsatt("B075","SEEN", 0);
    gMC->Gsatt("B077","SEEN", 0);
    gMC->Gsatt("BTR1","SEEN", 0);
    gMC->Gsatt("BTR2","SEEN", 0);
    gMC->Gsatt("BTR3","SEEN", 0);
    gMC->Gsatt("UTR1","SEEN", 0);
    gMC->Gsatt("UTR2","SEEN", 0);
    gMC->Gsatt("UTR3","SEEN", 0);
  }
  else {
    gMC->Gsatt("B071","SEEN", 0);
    gMC->Gsatt("B074","SEEN", 0);
    gMC->Gsatt("B075","SEEN", 0);
    gMC->Gsatt("B077","SEEN", 0);
    gMC->Gsatt("BTR1","SEEN", 0);
    gMC->Gsatt("BTR2","SEEN", 0);
    gMC->Gsatt("BTR3","SEEN", 0);
    gMC->Gsatt("UTR1","SEEN", 0);
    if (fGeometry->GetPHOShole())
      gMC->Gsatt("UTR2","SEEN", 0);
    if (fGeometry->GetRICHhole())
      gMC->Gsatt("UTR3","SEEN", 0);
  }
//   gMC->Gsatt("UCII","SEEN", 0);
//   gMC->Gsatt("UCIM","SEEN", 0);
//   gMC->Gsatt("UCIO","SEEN", 0);
//   gMC->Gsatt("UL02","SEEN", 1);
//   gMC->Gsatt("UL05","SEEN", 1);
//   gMC->Gsatt("UL06","SEEN", 1);
  
  gMC->Gdopt("hide", "on");
  gMC->Gdopt("shad", "on");
  gMC->Gsatt("*", "fill", 7);
  gMC->SetClipBox(".");
  gMC->SetClipBox("*", 0, 2000, -2000, 2000, -2000, 2000);
  gMC->DefaultRange();
  gMC->Gdraw("alic", 40, 30, 0, 12, 9.4, .021, .021);
  gMC->Gdhead(1111, "Transition Radiation Detector");
  gMC->Gdman(18, 4, "MAN");

}

//_____________________________________________________________________________
Int_t AliTRD::DistancetoPrimitive(Int_t , Int_t ) const
{
  //
  // Distance between the mouse and the TRD detector on the screen
  // Dummy routine
  
  return 9999;

}
 
//_____________________________________________________________________________
void AliTRD::Init()
{
  //
  // Initialize the TRD detector after the geometry has been created
  //

  Int_t i;

  if (fDebug) {
    printf("\n%s: ",ClassName());
    for (i = 0; i < 35; i++) printf("*");
    printf(" TRD_INIT ");
    for (i = 0; i < 35; i++) printf("*");
    printf("\n");
  }

  if      (fGeometry->IsVersion() == 0) {
    printf("%s: Geometry for spaceframe with holes initialized\n",ClassName());
  }
  else if (fGeometry->IsVersion() == 1) {
    printf("%s: Geometry for spaceframe without holes initialized\n",ClassName());
    if (fGeometry->GetPHOShole())
      printf("%s: Leave space in front of PHOS free\n",ClassName());
    if (fGeometry->GetRICHhole())
      printf("%s: Leave space in front of RICH free\n",ClassName());
  }
  
  if (fGasMix == 1) {
    printf("%s: Gas Mixture: 85%% Xe + 15%% CO2\n",ClassName());
  }
  else {
    printf("%s: Gas Mixture: 97%% Xe + 3%% Isobutane\n",ClassName());
  }

}

//_____________________________________________________________________________
void AliTRD::LoadPoints(Int_t track)
{
  //
  // Store x, y, z of all hits in memory.
  // Hit originating from TR photons are given a different color
  //

  //if (!fDrawTR) {
  //  AliDetector::LoadPoints(track);
  //  return;
  //}

  if ((fHits == 0) && (fTrackHits == 0)) return;

  Int_t nhits;
  if (fHitType < 2) {
    nhits = fHits->GetEntriesFast();
  }
  else {
    nhits = fTrackHits->GetEntriesFast();
  } 
  if (nhits == 0) return;

  Int_t tracks = gAlice->GetNtrack();
  if (fPoints == 0) fPoints = new TObjArray(tracks);

  AliTRDhit *ahit;
  
  Int_t    *ntrkE = new Int_t[tracks];
  Int_t    *ntrkT = new Int_t[tracks];
  Int_t    *limiE = new Int_t[tracks];
  Int_t    *limiT = new Int_t[tracks];
  Float_t **coorE = new Float_t*[tracks];
  Float_t **coorT = new Float_t*[tracks];
  for(Int_t i = 0; i < tracks; i++) {
    ntrkE[i] = 0;
    ntrkT[i] = 0;
    coorE[i] = 0;
    coorT[i] = 0;
    limiE[i] = 0;
    limiT[i] = 0;
  }
  
  AliTRDpoints *points = 0;
  Float_t      *fp     = 0;
  Int_t         trk;
  Int_t         chunk  = nhits / 4 + 1;

  // Loop over all the hits and store their position
  ahit = (AliTRDhit *) FirstHit(-1);
  while (ahit) {

    // dEdx hits
    if (ahit->GetCharge() >= 0) {

      trk = ahit->GetTrack();
      if (ntrkE[trk] == limiE[trk]) {
        // Initialise a new track
        fp = new Float_t[3*(limiE[trk]+chunk)];
        if (coorE[trk]) {
          memcpy(fp,coorE[trk],sizeof(Float_t)*3*limiE[trk]);
          delete [] coorE[trk];
        }
        limiE[trk] += chunk;
        coorE[trk]  = fp;
      } 
      else {
        fp = coorE[trk];
      }
      fp[3*ntrkE[trk]  ] = ahit->X();
      fp[3*ntrkE[trk]+1] = ahit->Y();
      fp[3*ntrkE[trk]+2] = ahit->Z();
      ntrkE[trk]++;

    }
    // TR photon hits
    else if ((ahit->GetCharge() < 0) && (fDrawTR)) {

      trk = ahit->GetTrack();
      if (ntrkT[trk] == limiT[trk]) {
        // Initialise a new track
        fp = new Float_t[3*(limiT[trk]+chunk)];
        if (coorT[trk]) {
          memcpy(fp,coorT[trk],sizeof(Float_t)*3*limiT[trk]);
          delete [] coorT[trk];
        }
        limiT[trk] += chunk;
        coorT[trk]  = fp;
      } 
      else {
        fp = coorT[trk];
      }
      fp[3*ntrkT[trk]  ] = ahit->X();
      fp[3*ntrkT[trk]+1] = ahit->Y();
      fp[3*ntrkT[trk]+2] = ahit->Z();
      ntrkT[trk]++;

    }

    ahit = (AliTRDhit *) NextHit();

  }

  for (trk = 0; trk < tracks; ++trk) {

    if (ntrkE[trk] || ntrkT[trk]) {

      points = new AliTRDpoints();
      points->SetDetector(this);
      points->SetParticle(trk);

      // Set the dEdx points
      if (ntrkE[trk]) {
        points->SetMarkerColor(GetMarkerColor());
        points->SetMarkerSize(GetMarkerSize());
        points->SetPolyMarker(ntrkE[trk],coorE[trk],GetMarkerStyle());
        delete [] coorE[trk];
        coorE[trk] = 0;
      }

      // Set the TR photon points
      if (ntrkT[trk]) {
        points->SetTRpoints(ntrkT[trk],coorT[trk]);
        delete [] coorT[trk];
        coorT[trk] = 0;
      }

      fPoints->AddAt(points,trk);

    }

  }

  delete [] coorE;
  delete [] coorT;
  delete [] ntrkE;
  delete [] ntrkT;
  delete [] limiE;
  delete [] limiT;

}

//_____________________________________________________________________________
void AliTRD::MakeBranch(Option_t* option, const char *file)
{
  //
  // Create Tree branches for the TRD digits.
  //

  Int_t  buffersize = 4000;
  Char_t branchname[15];
  sprintf(branchname,"%s",GetName());

  const char *cD = strstr(option,"D");

  AliDetector::MakeBranch(option,file);

  if (fDigits && gAlice->TreeD() && cD) {
    MakeBranchInTree(gAlice->TreeD(),branchname,&fDigits,buffersize,file);
  }	

  if (fHitType > 1) {
    MakeBranch2(option,file); 
  }

}

//_____________________________________________________________________________
void AliTRD::ResetDigits()
{
  //
  // Reset number of digits and the digits array for this detector
  //

  fNdigits = 0;
  if (fDigits) fDigits->Clear();

}

//_____________________________________________________________________________
void AliTRD::ResetRecPoints()
{
  //
  // Reset number of reconstructed points and the point array
  //

  if (fRecPoints) {
    fNRecPoints = 0;
    Int_t nentr = fRecPoints->GetEntriesFast();
    for (Int_t i = 0; i < nentr; i++) delete fRecPoints->RemoveAt(i);
  }

}

//_____________________________________________________________________________
void AliTRD::SetTreeAddress()
{
  //
  // Set the branch addresses for the trees.
  //

  Char_t branchname[15];

  AliDetector::SetTreeAddress();

  TBranch *branch;
  TTree   *treeR = gAlice->TreeR();

  if (treeR) {
    sprintf(branchname,"%scluster",GetName());
    if (fRecPoints) {
      branch = treeR->GetBranch(branchname);
      if (branch) {
        branch->SetAddress(&fRecPoints);
      }
    }
  }

  if (fHitType > 0) {
    SetTreeAddress2();    
  }

}

//_____________________________________________________________________________
void AliTRD::SetGasMix(Int_t imix)
{
  //
  // Defines the gas mixture (imix=0:  Xe/Isobutane imix=1: Xe/CO2)
  //
  
  if ((imix < 0) || (imix > 1)) {
    printf("Wrong input value: %d\n",imix);
    printf("Use standard setting\n");
    fGasMix = 1;
    return;
  }

  fGasMix = imix;

}

//_____________________________________________________________________________
void AliTRD::SetPHOShole()
{
  //
  // Selects a geometry with a hole in front of the PHOS
  //

  fGeometry->SetPHOShole();

}

//_____________________________________________________________________________
void AliTRD::SetRICHhole()
{
  //
  // Selects a geometry with a hole in front of the RICH
  //

  fGeometry->SetRICHhole();

}

//_____________________________________________________________________________
AliTRD &AliTRD::operator=(const AliTRD &trd)
{
  //
  // Assignment operator
  //

  if (this != &trd) ((AliTRD &) trd).Copy(*this);
  return *this;

} 

//_____________________________________________________________________________
void AliTRD::FinishPrimary()
{
  //
  // Store the hits in the containers after all primaries are finished
  //

  if (fTrackHits) { 
    fTrackHits->FlushHitStack();
  }

}

//_____________________________________________________________________________
void AliTRD::RemapTrackHitIDs(Int_t *map)
{
  //
  // Remap the track IDs
  //

  if (!fTrackHits) {
    return;
  }

  if (fTrackHits) {
    TClonesArray *arr = fTrackHits->GetArray();;
    for (Int_t i = 0; i < arr->GetEntriesFast(); i++){
      AliTrackHitsParamV2 *info = (AliTrackHitsParamV2 *) (arr->At(i));
      info->fTrackID = map[info->fTrackID];
    }
  }

}

//_____________________________________________________________________________
void AliTRD::ResetHits()
{
  //
  // Reset the hits
  //

  AliDetector::ResetHits();
  if (fTrackHits) {
    fTrackHits->Clear();
  }

}

//_____________________________________________________________________________
AliHit* AliTRD::FirstHit(Int_t track)
{
  //
  // Return the first hit of a track
  //

  if (fHitType > 1) {
    return FirstHit2(track);
  }

  return AliDetector::FirstHit(track);

}

//_____________________________________________________________________________
AliHit* AliTRD::NextHit()
{
  //
  // Returns the next hit of a track
  //

  if (fHitType > 1) {
    return NextHit2();
  }

  return AliDetector::NextHit();

}

//_____________________________________________________________________________
AliHit* AliTRD::FirstHit2(Int_t track) 
{
  //
  // Initializes the hit iterator.
  // Returns the address of the first hit of a track.
  // If <track> >= 0 the track is read from disk,
  // while if <track> < 0 the first hit of the current
  // track is returned.
  //

  if (track >= 0) {
    gAlice->ResetHits();
    gAlice->TreeH()->GetEvent(track);
  }
  
  if (fTrackHits) {
    fTrackHits->First();
    return (AliHit*) fTrackHits->GetHit();
  }
  else {
    return 0;
  }

}

//_____________________________________________________________________________
AliHit* AliTRD::NextHit2()
{
  //
  // Returns the next hit of the current track
  //

  if (fTrackHits) {
    fTrackHits->Next();
    return (AliHit *) fTrackHits->GetHit();
  }
  else {
    return 0;
  }

}

//_____________________________________________________________________________
void AliTRD::MakeBranch2(Option_t *option, const char *file)
{
  //
  // Create a new branch in the current Root tree.
  // The branch of fHits is automatically split.
  //

  if (fHitType < 2) {
    return;
  }

  char branchname[10];
  sprintf(branchname,"%s2",GetName());

  // Get the pointer to the header
  const char *cH = strstr(option,"H");
 
  if (!fTrackHits) {
    fTrackHits = new AliTRDtrackHits();
  }

  if (fTrackHits && gAlice->TreeH() && cH) {

    gAlice->TreeH()->Branch(branchname,"AliTRDtrackHits"
                                      ,&fTrackHits
                                      ,fBufferSize,99);

    if (GetDebug() > 1) {
      printf("<AliTRD::MakeBranch2> Making Branch %s for trackhits\n"
            ,branchname);
    }

    const char kFolder[] = "RunMC/Event/Data";

    if (GetDebug()) {
      printf("<AliTRD::MakeBranch2> %15s: Publishing %s to %s\n"
            ,ClassName(),branchname,kFolder);
    }

    Publish(kFolder,&fTrackHits,branchname);

    if (file) {
      TBranch *b = gAlice->TreeH()->GetBranch(branchname);
      TDirectory *wd = gDirectory;
      b->SetFile(file);
      TIter next(b->GetListOfBranches());
      while ((b = (TBranch*) next())) {
        b->SetFile(file);
      }
      wd->cd();
      if (GetDebug() > 1) {
        printf("<AliTRD::MakeBranch2> Diverting branch %s to file %s\n"
              ,branchname,file);
      }
    }

  }

}

//_____________________________________________________________________________
void AliTRD::SetTreeAddress2()
{
  //
  // Set the branch address for the trackHits tree
  //

  TBranch *branch;

  char branchname[20];

  sprintf(branchname,"%s2",GetName());
  
  // Branch address for hit tree
  TTree *treeH = gAlice->TreeH();
  if ((treeH) && (fHitType > 0)) {
    branch = treeH->GetBranch(branchname);
    if (branch) {
      branch->SetAddress(&fTrackHits);
    }
  }

}

//_____________________________________________________________________________
void AliTRD::AddHit2(Int_t track, Int_t det, Float_t *hits, Int_t q
                   , Bool_t inDrift)
{
  //
  // Add a hit to the list
  //

  Int_t rtrack;

  if (fIshunt) {
    Int_t primary = gAlice->GetPrimary(track);
    gAlice->Particle(primary)->SetBit(kKeepBit);
    rtrack = primary;
  } 
  else {
    rtrack = track;
    gAlice->FlagTrack(track);
  }

  if ((fTrackHits) && (fHitType > 0)) {
    fTrackHits->AddHitTRD(det,rtrack,hits[0],hits[1],hits[2],q,inDrift);
  }

}




















































































































































































































































































































































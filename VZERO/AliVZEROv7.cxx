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

///////////////////////////////////////////////////////////////////////
//                                                                   // 
//  (V-zero) detector  version 7 as designed by the Lyon and         //
//   Mexico groups and Carlos Perez Lara from Pontificia Universidad //
//   Catolica del Peru    				             // 
//   All comments should be sent to Brigitte CHEYNIS:                //
//                     b.cheynis@ipnl.in2p3.fr                       // 
//   Geometry of April 2006 done with ROOT geometrical modeler       //
//   V0R (now V0C) sits between Z values  -89.5 and  -84.8 cm        //
//   V0L (now V0A) sits between Z values +338.5 and +342.5 cm        // 
//   New coordinate system has been implemented in october 2003      //
//   Revision of the V0A part by Lizardo Valencia  in July 2008      //
//                                                                   //
/////////////////////////////////////////////////////////////////////// 

// --- Standard libraries ---
#include <Riostream.h>

// --- ROOT libraries ---
#include <TClonesArray.h>
#include <TMath.h>
#include <TVirtualMC.h>
#include <TParticle.h>

#include <TGeoManager.h>
#include <TGeoMatrix.h>
#include <TGeoMaterial.h>
#include <TGeoMedium.h>
#include <TGeoVolume.h>
#include "TGeoTube.h"
#include "TGeoArb8.h"
#include "TGeoCompositeShape.h"

// --- AliRoot header files ---
#include "AliRun.h"
#include "AliMC.h"
#include "AliMagF.h"
#include "AliVZEROLoader.h"
#include "AliVZEROdigit.h"
#include "AliVZEROhit.h"
#include "AliVZEROv7.h"
#include "AliLog.h"
 
ClassImp(AliVZEROv7)

//_____________________________________________________________________________
AliVZEROv7:: AliVZEROv7():AliVZERO(),
   fCellId(0),
   fTrackPosition(),
   fTrackMomentum(), 
   fV0CHeight1(2.5), 
   fV0CHeight2(4.4), 
   fV0CHeight3(7.4), 
   fV0CHeight4(12.5),
   fV0CRMin(4.6), 
   fV0CRBox(38.0),
   fV0CLidThickness(0.30),
   fV0CCellThickness(2.00),
   fV0CBoxThickness(4.70),
   fV0COffsetFibers(1.0),
   fV0CLightYield(93.75),
   fV0CLightAttenuation(0.05),
   fV0CnMeters(15.0),
   fV0CFibToPhot(0.3),
   fV0AR0(4.2), 
   fV0AR1(7.6), 
   fV0AR2(13.8), 
   fV0AR3(22.7),
   fV0AR4(41.3), 
   fV0AR5(43.3), 
   fV0AR6(72.6),
   fV0AR7(92.0), // Distance from origin to outtermost intersection sector7 and sector8 
   fV0ASciWd(2.5), 
   fV0APlaWd(0.5), 
   fV0APlaAl(0.06), 
   fV0AOctWd(0.75), 
   fV0AFraWd(0.2),
   fV0AOctH1(1.0), 
   fV0AOctH2(2.0), 
   fV0ABasHt(2.0),
   fV0AFibRd(0.1),
   fV0APlaEx(4.4),
   fV0APMBWd(24.6), 
   fV0APMBHt(22.0), 
   fV0APMBTh(7.1), 
   fV0APMBWdW(0.3), 
   fV0APMBHtW(1.0),
   fV0APMBAng(30.0), 
   fV0APMBThW(0.3), 
   fV0APMTR1(2.44), 
   fV0APMTR2(2.54), 
   fV0APMTR3(2.54),
   fV0APMTR4(2.70), 
   fV0APMTH(10.0), 
   fV0APMTB(1.0),
   fV0AnMeters(fV0AR6*0.01),
   fV0ALightYield(93.75),
   fV0ALightAttenuation(0.05),
   fV0AFibToPhot(0.3),
   fVersion(7)
{
// Standard default constructor 
}

//_____________________________________________________________________________
AliVZEROv7::AliVZEROv7(const char *name, const char *title):AliVZERO(name,title),
   fCellId(0),
   fTrackPosition(),
   fTrackMomentum(), 
   fV0CHeight1(2.5), 
   fV0CHeight2(4.4), 
   fV0CHeight3(7.4), 
   fV0CHeight4(12.5),
   fV0CRMin(4.6), 
   fV0CRBox(38.0),
   fV0CLidThickness(0.30),
   fV0CCellThickness(2.00),
   fV0CBoxThickness(4.70),
   fV0COffsetFibers(1.0),
   fV0CLightYield(93.75),
   fV0CLightAttenuation(0.05),
   fV0CnMeters(15.0),
   fV0CFibToPhot(0.3),
   fV0AR0(4.2), 
   fV0AR1(7.6), 
   fV0AR2(13.8), 
   fV0AR3(22.7),
   fV0AR4(41.3), 
   fV0AR5(43.3), 
   fV0AR6(72.6),
   fV0AR7(92.0), // Distance from origin to outtermost intersection of sector7 and sector8 
   fV0ASciWd(2.5), 
   fV0APlaWd(0.5), 
   fV0APlaAl(0.06), 
   fV0AOctWd(0.75), 
   fV0AFraWd(0.2),
   fV0AOctH1(1.0), 
   fV0AOctH2(2.0), 
   fV0ABasHt(2.0),
   fV0AFibRd(0.1),
   fV0APlaEx(4.4),
   fV0APMBWd(24.6), 
   fV0APMBHt(22.0), 
   fV0APMBTh(7.1), 
   fV0APMBWdW(0.3), 
   fV0APMBHtW(1.0),
   fV0APMBAng(30.0), 
   fV0APMBThW(0.3), 
   fV0APMTR1(2.44), 
   fV0APMTR2(2.54), 
   fV0APMTR3(2.54),
   fV0APMTR4(2.70), 
   fV0APMTH(10.0), 
   fV0APMTB(1.0),
   fV0AnMeters(fV0AR6*0.01),
   fV0ALightYield(93.75),
   fV0ALightAttenuation(0.05),
   fV0AFibToPhot(0.3),
   fVersion(7)


{
// Standard constructor for V-zero Detector  version 7

  AliDebug(2,"Create VZERO object ");

//  fVersion            =     7;  // version number

//   // V0C Parameters related to geometry: All in cm
//   fV0CHeight1         =    2.5; // height of cell 1
//   fV0CHeight2         =    4.4; // height of cell 2
//   fV0CHeight3         =    7.4; // height of cell 3
//   fV0CHeight4         =   12.5; // height of cell 4
//   fV0CRMin            =    4.6; // inner radius of box
//   fV0CRBox            =   38.0; // outer radius of box
//   fV0CLidThickness    =   0.30; // thickness of Carbon lid
//   fV0CCellThickness   =   2.00; // thickness of elementary cell
//   fV0CBoxThickness    =   4.70; // thickness of V0C Box
//   fV0COffsetFibers    =    1.0; // offset to output fibers
//   // V0C Parameters related to light output
//   fV0CLightYield         =  93.75; // Light yield in BC408 (93.75 eV per photon)
//   fV0CLightAttenuation   =   0.05; // Light attenuation in fiber (0.05 per meter)
//   fV0CnMeters            =   15.0; // Number of meters of clear fibers to PM
//   fV0CFibToPhot          =    0.3; // Attenuation at fiber-photocathode interface
// 
//   // V0A Parameters related to geometry: All in cm
//   fV0AR0     =  4.2;  // Radius of hole
//   fV0AR1     =  7.6;  // Maximun radius of 1st cell
//   fV0AR2     = 13.8; // Maximun radius of 2nd cell
//   fV0AR3     = 22.7; // Maximun radius of 3rd cell
//   fV0AR4     = 41.3; // Maximun radius of 4th cell
//   fV0AR5     = 43.3; // Radius circunscrite to innermost octagon
//   fV0AR6     = 68.0; // Radius circunscrite to outtermost octagon
//   fV0ASciWd  =  2.5;  // Scintillator thickness 
//   fV0APlaWd  =  0.5;  // Plates thinckness
//   fV0APlaAl  = 0.06; // Plates AlMg3 thinckness
//   fV0AOctWd  = 0.75; // Innermost octagon thickness
//   fV0AOctH1  =  1.0;  // Height of innermost octagon
//   fV0AOctH2  =  2.0;  // Height of outtermost octagon
//   fV0AFibRd  =  0.1;  // Radius of Fiber
//   fV0AFraWd  =  0.2;  // Support Frame thickness
//   fV0APMBWd  = 24.6;  // Width of PM Box
//   fV0APMBHt  = 22.0;  // Height of PM Box
//   fV0APMBTh  =  7.1;  // Thickness of PM Box
//   fV0APMBWdW =  0.3;  // Thickness of PM Box Side1 Wall
//   fV0APMBHtW =  1.0;  // Thickness of PM Box Side2 Wall
//   fV0APMBThW =  0.3;  // Thickness of PM Box Top Wall
//   fV0APMBAng = 30.0;  // Angle between PM Box and Support
//   fV0APMTR1  = 2.44;  // PMT Glass
//   fV0APMTR2  = 2.54;  // PMT Glass
//   fV0APMTR3  = 2.54;  // PMT Cover
//   fV0APMTR4  = 2.70;  // PMT Cover
//   fV0APMTH   = 10.0;  // PMT Height
//   fV0APMTB   =  1.0;  // PMT Basis
//   fV0APlaEx  =  4.4;  // Plates Extension height
//   fV0ABasHt  =  2.0;  // Basis Height
//   // V0A Parameters related to light output
//   fV0ALightYield         =  93.75;      // Light yield in BC404
//   fV0ALightAttenuation   =   0.05;      // Light attenuation in WLS fiber, per meter
//   fV0AnMeters            = fV0AR6*0.01; // Tentative value, in meters
//   fV0AFibToPhot          =    0.3;      // Attenuation at fiber-photocathode interface
}
//_____________________________________________________________________________

void AliVZEROv7::BuildGeometry()
{ 
}
            
//_____________________________________________________________________________
void AliVZEROv7::CreateGeometry()
{
// Constructs TGeo geometry 

  AliDebug(2,"VZERO ConstructGeometry");
  TGeoVolume *top = gGeoManager->GetVolume("ALIC");

  ///////////////////////////////////////////////////////////////////////////
  // Construct the geometry of V0C Detector. Brigitte CHEYNIS
  
    const int kColorVZERO  = kGreen;
    TGeoMedium *medV0CAlu = gGeoManager->GetMedium("VZERO_V0CAlu");
    TGeoMedium *medV0CCar = gGeoManager->GetMedium("VZERO_V0CCar");
    TGeoMedium *medV0CSci = gGeoManager->GetMedium("VZERO_V0CSci");
    TGeoVolume *v0RI = new TGeoVolumeAssembly("V0RI");
    Float_t heightRight, r4Right;
    Float_t zdet = 90.0 - 0.5 - fV0CBoxThickness/2.0;
    heightRight  = fV0CHeight1 + fV0CHeight2 + fV0CHeight3 + fV0CHeight4;
    r4Right      = fV0CRMin + heightRight + 3.0*0.2; // 3 spacings of 2mm between rings

    // Creation of  carbon lids (3.0 mm thick) to keep V0C box shut :
    Float_t   partube[3];
    partube[0] =   fV0CRMin;
    partube[1] =   fV0CRBox;
    partube[2] =   fV0CLidThickness/2.0;
    TGeoTube   *sV0CA = new TGeoTube("V0CA", partube[0], partube[1], partube[2]);
    TGeoVolume *v0CA  = new TGeoVolume("V0CA",sV0CA,medV0CCar);
    TGeoTranslation *tr2 = new TGeoTranslation(0.,0., fV0CBoxThickness/2.0-partube[2]);
    TGeoTranslation *tr3 = new TGeoTranslation(0.,0.,-fV0CBoxThickness/2.0+partube[2]);
    v0RI->AddNode(v0CA,1,tr2);
    v0RI->AddNode(v0CA,2,tr3);
    v0CA->SetLineColor(kYellow);

    // Creation of aluminum rings 3.0 mm thick to maintain the v0RI pieces : 
    partube[0] =   fV0CRMin - 0.3;
    partube[1] =   fV0CRMin;
    partube[2] =   fV0CBoxThickness/2.0;
    TGeoTube   *sV0IR = new TGeoTube("V0IR", partube[0], partube[1], partube[2]);
    TGeoVolume *v0IR  = new TGeoVolume("V0IR",sV0IR,medV0CAlu);
    v0RI->AddNode(v0IR,1,0);
    v0IR->SetLineColor(kYellow);
    partube[0] =   fV0CRBox;
    partube[1] =   fV0CRBox + 0.3; 
    partube[2] =   fV0CBoxThickness/2.0;
    TGeoTube   *sV0ER = new TGeoTube("V0ER", partube[0], partube[1], partube[2]);
    TGeoVolume *v0ER  = new TGeoVolume("V0ER",sV0ER,medV0CAlu);
    v0RI->AddNode(v0ER,1,0);
    v0ER->SetLineColor(kYellow);

    // Creation of assembly V0R0 of scintillator cells within one sector
    TGeoVolume *v0R0 = new TGeoVolumeAssembly("V0R0");  					  

    // Elementary cell of ring 1  - right part - :
    // (cells of ring 1 will be shifted by 2.0 cm backwards to output fibers)
    Float_t   r1Right =  fV0CRMin + fV0CHeight1;
    Float_t   offset  = fV0CBoxThickness/2.0 - fV0CLidThickness - fV0CCellThickness/2.0;   
    Float_t   partubs[5];   
    partubs[0]     =  fV0CRMin;
    partubs[1]     =  r1Right;
    partubs[2]     =  fV0CCellThickness/2.0;
    partubs[3]     =  90.0-22.5;
    partubs[4]     = 135.0-22.5;
    TGeoTubeSeg *sV0R1 = new TGeoTubeSeg("V0R1", partubs[0], partubs[1], partubs[2],
					 partubs[3], partubs[4]);
    TGeoVolume  *v0R1  = new TGeoVolume("V0R1",sV0R1,medV0CSci);				       
    TGeoTranslation *tr4 = new TGeoTranslation(0.,0.,-offset);
    v0R0->AddNode(v0R1,1,tr4);
    v0R1->SetLineColor(kColorVZERO);

    // Elementary cell of ring 2 - right part - :
    // (cells of ring 2 will be shifted by 1.0 cm backwards to output fibers)
    Float_t   r2Right  =  r1Right + fV0CHeight2;  
    partubs[0]     =  r1Right;  //  must be equal to 7.1
    partubs[1]     =  r2Right;  //  must be equal to 11.5
    TGeoTubeSeg *sV0R2 = new TGeoTubeSeg("V0R2", partubs[0], partubs[1], partubs[2],
					 partubs[3], partubs[4]);
    TGeoVolume  *v0R2  = new TGeoVolume("V0R2",sV0R2,medV0CSci);
    TGeoTranslation *tr5 = new TGeoTranslation(0.0,0.2,-offset + fV0COffsetFibers);
    v0R0->AddNode(v0R2,1,tr5);
    v0R2->SetLineColor(kColorVZERO);

    // Ring 3 - right part -  :
    r2Right  =  r2Right + 0.2;
    Float_t   r3Right  =  r2Right + fV0CHeight3;     
    partubs[0]     =  r2Right;  //  must be equal to 11.7
    partubs[1]     =  r3Right;  //  must be equal to 19.1
    partubs[3]     =  90.0-22.5;
    partubs[4]     = 112.5-22.5;
    TGeoTubeSeg *sV0R3 = new TGeoTubeSeg("V0R3", partubs[0], partubs[1], partubs[2],
					 partubs[3], partubs[4]);
    TGeoVolume  *v0R3  = new TGeoVolume("V0R3",sV0R3,medV0CSci);
    TGeoTranslation *tr6 = new TGeoTranslation(0.,0.2,-offset + 2.0*fV0COffsetFibers);
    v0R0->AddNode(v0R3,1,tr6);
    v0R3->SetLineColor(kColorVZERO);
    partubs[3]     = 112.5-22.5;
    partubs[4]     = 135.0-22.5;
    TGeoTubeSeg *sV0R4 = new TGeoTubeSeg("V0R4", partubs[0], partubs[1], partubs[2],
					 partubs[3], partubs[4]);
    TGeoVolume  *v0R4  = new TGeoVolume("V0R4",sV0R4,medV0CSci);
    v0R0->AddNode(v0R4,1,tr6);
    v0R4->SetLineColor(kColorVZERO);
  
    // Ring 4 - right part -  : 
    Float_t x = TMath::ATan(3.5/257.5) * ((180./TMath::Pi()));
    r3Right = r3Right + 0.2 + 0.2;   // + 0.2 because no shift in translation here !!
    partubs[0]     =  r3Right;  //  must be equal to 19.5
    partubs[1]     =  r4Right;  //  must be equal to 32.0
    partubs[3]     =  90.0-22.5+x;
    partubs[4]     = 112.5-22.5-x;
    TGeoTubeSeg *sV0R5 = new TGeoTubeSeg("V0R5", partubs[0], partubs[1], partubs[2],
					 partubs[3], partubs[4]);
    TGeoVolume  *v0R5  = new TGeoVolume("V0R5",sV0R5,medV0CSci);
    TGeoTranslation *tr7 = new TGeoTranslation(0.,0.0,-offset + 2.0*fV0COffsetFibers);					      
    v0R0->AddNode(v0R5,1,tr7);
    v0R5->SetLineColor(kColorVZERO);
    partubs[3]     = 112.5-22.5+x;
    partubs[4]     = 135.0-22.5-x;
    TGeoTubeSeg *sV0R6 = new TGeoTubeSeg("V0R6", partubs[0], partubs[1], partubs[2],
					 partubs[3], partubs[4]);
    TGeoVolume  *v0R6  = new TGeoVolume("V0R6",sV0R6,medV0CSci);
    v0R0->AddNode(v0R6,1,tr7);
    v0R6->SetLineColor(kColorVZERO);
    Float_t  phi;
    Float_t  phiDeg= 180./4.;
    Int_t    nsecR = 1;     // number of sectors in right part of V0
    for (phi = 22.5; phi < 360.0; phi = phi + phiDeg) {
      TGeoRotation  *rot1 = new TGeoRotation("rot1", 90.0, +phi, 90., 90.+phi, 0.0, 0.0 ); 
      v0RI->AddNode(v0R0,nsecR,rot1);    
      nsecR++;        
    }

  ///////////////////////////////////////////////////////////////////////////
  // Construct the geometry of V0A Detector. Carlos PEREZ, PUCP
  // Revision by Lizardo VALENCIA, UNAM Mexico in July 2008

    const int kV0AColorSci   = 5;
    const int kV0AColorPlaIn = 3;
    const int kV0AColorPlaOu = 41;
    const int kV0AColorOct   = 7;
    const int kV0AColorFra   = 6;
    const int kV0AColorFib   = 11;
    const int kV0AColorPMG   = 1;
    const int kV0AColorPMA   = 2;
    TGeoMedium *medV0ASci = gGeoManager->GetMedium("VZERO_V0ASci");
    TGeoMedium *medV0APlaIn = gGeoManager->GetMedium("VZERO_V0APlaIn");
    TGeoMedium *medV0APlaOu = gGeoManager->GetMedium("VZERO_V0APlaOu");
    TGeoMedium *medV0ASup = gGeoManager->GetMedium("VZERO_V0ALuc");
    TGeoMedium *medV0AFra = gGeoManager->GetMedium("VZERO_V0ALuc");
    TGeoMedium *medV0AFib = gGeoManager->GetMedium("VZERO_V0AFib");
    TGeoMedium *medV0APMGlass = gGeoManager->GetMedium("VZERO_V0APMG");
    TGeoMedium *medV0APMAlum = gGeoManager->GetMedium("VZERO_V0APMA");
    double pi = TMath::Pi();
    double sin225   = TMath::Sin(pi/8.);
    double cos225   = TMath::Cos(pi/8.);
    double sin45    = TMath::Sin(pi/4.); // lucky: Sin45=Cos45
    double cos45    = TMath::Cos(pi/4.); 
    double v0APts[16];
    //double sin6645   = TMath::Sin(1.16);
    //double cos6645   = TMath::Cos(1.16);
    double sin654   = TMath::Sin(1.14);
    double cos654   = TMath::Cos(1.14);
    double sin65   = TMath::Sin(1.13);//65
    double cos65   = TMath::Cos(1.13);
    double sin665   = TMath::Sin(1.16);
    double cos665   = TMath::Cos(1.16);//66.5

    ////////////////////////////
    /// Definition sector 1
    TGeoVolume *v0ASec = new TGeoVolumeAssembly("V0ASec");
    //TGeoCompositeShape *sV0AHole = new TGeoCompositeShape("sV0AHole");
    
    /// For boolean sustraction
    double preShape = 0.2;
    for (int i=0;i<2;i++) {
      v0APts[0+8*i] = fV0AR0-fV0AFraWd/2.-preShape;  v0APts[1+8*i] = -preShape;
      v0APts[2+8*i] = fV0AR0-fV0AFraWd/2.-preShape;  v0APts[3+8*i] = fV0AFraWd/2.;
      v0APts[4+8*i] = fV0AR4+fV0AFraWd/2.+preShape;  v0APts[5+8*i] = fV0AFraWd/2.;
      v0APts[6+8*i] = fV0AR4+fV0AFraWd/2.+preShape;  v0APts[7+8*i] = -preShape;
    }
    new TGeoArb8("sV0ACha1",fV0ASciWd/1.5,v0APts);
    for (int i=0;i<2;i++) {
      v0APts[0+8*i] = fV0AR0*sin45-preShape;
      v0APts[1+8*i] = (fV0AR0-fV0AFraWd)*sin45-preShape;
      v0APts[2+8*i] = (fV0AR0-fV0AFraWd/2.)*sin45-preShape;
      v0APts[3+8*i] = (fV0AR0-fV0AFraWd/2.)*sin45;
      v0APts[4+8*i] = (fV0AR4+fV0AFraWd/2.)*sin45+preShape;
      v0APts[5+8*i] = (fV0AR4+fV0AFraWd/2.)*sin45+2.*preShape;
      v0APts[6+8*i] = (fV0AR4+fV0AFraWd)*sin45+preShape;
      v0APts[7+8*i] = fV0AR4*sin45+preShape;
    }
    new TGeoArb8("sV0ACha2", fV0ASciWd/2.+2.*preShape, v0APts);
    new TGeoCompositeShape("sV0ACha12","sV0ACha1+sV0ACha2");
    new TGeoTube("sV0ANail1SciHole", 0.0, 0.4, 1.65);
    TGeoTranslation *pos1 = new TGeoTranslation("pos1", 42.9, 0.51, 0.0);
    pos1->RegisterYourself();
    new TGeoTube("sV0ANail2SciHole", 0.0, 0.4, 1.65);
    TGeoTranslation *pos2 = new TGeoTranslation("pos2", 30.8,30.04,0.0);
    pos2->RegisterYourself();
    new TGeoCompositeShape("sV0ANailsSciHoles","sV0ANail1SciHole:pos1+sV0ANail2SciHole:pos2");
    new TGeoCompositeShape("sV0ACha","sV0ACha12+sV0ANailsSciHoles");
    
    
    /// Frame
    TGeoVolume *v0AFra = new TGeoVolumeAssembly("V0AFra");
    for (int i=0;i<2;i++) {
      v0APts[0+8*i] = fV0AR0-fV0AFraWd/2.;  v0APts[1+8*i] = 0.;
      v0APts[2+8*i] = fV0AR0-fV0AFraWd/2.;  v0APts[3+8*i] = fV0AFraWd/2.;
      v0APts[4+8*i] = fV0AR4+fV0AFraWd/2.;  v0APts[5+8*i] = fV0AFraWd/2.;
      v0APts[6+8*i] = fV0AR4+fV0AFraWd/2.;  v0APts[7+8*i] = 0.;
    }
    TGeoArb8 *sV0AFraB1 = new TGeoArb8("sV0AFraB1",fV0ASciWd/2.,v0APts);
    TGeoVolume *v0AFraB1 = new TGeoVolume("V0AFraB1",sV0AFraB1,medV0AFra);
    for (int i=0;i<2;i++) {
      v0APts[0+8*i] = fV0AR0*sin45;
      v0APts[1+8*i] = (fV0AR0-fV0AFraWd)*sin45;
      v0APts[2+8*i] = (fV0AR0-fV0AFraWd/2.)*sin45;
      v0APts[3+8*i] = (fV0AR0-fV0AFraWd/2.)*sin45;
      v0APts[4+8*i] = (fV0AR4+fV0AFraWd/2.)*sin45;
      v0APts[5+8*i] = (fV0AR4+fV0AFraWd/2.)*sin45;
      v0APts[6+8*i] = (fV0AR4+fV0AFraWd)*sin45;
      v0APts[7+8*i] = fV0AR4*sin45;
    }
    TGeoArb8 *sV0AFraB2 = new TGeoArb8("sV0AFraB2", fV0ASciWd/2., v0APts);
    TGeoVolume *v0AFraB2 = new TGeoVolume("V0AFraB2",sV0AFraB2,medV0AFra);
    v0AFraB1->SetLineColor(kV0AColorFra); v0AFraB2->SetLineColor(kV0AColorFra);
    v0AFra->AddNode(v0AFraB1,1);
    v0AFra->AddNode(v0AFraB2,1);  // Prefer 2 GeoObjects insted of 3 GeoMovements
    new TGeoTubeSeg( "sV0AFraR1b", fV0AR0-fV0AFraWd/2.,
		     fV0AR0+fV0AFraWd/2., fV0ASciWd/2., 0, 45);
    new TGeoTubeSeg( "sV0AFraR2b", fV0AR1-fV0AFraWd/2.,
		     fV0AR1+fV0AFraWd/2., fV0ASciWd/2., 0, 45);
    new TGeoTubeSeg( "sV0AFraR3b", fV0AR2-fV0AFraWd/2.,
		     fV0AR2+fV0AFraWd/2., fV0ASciWd/2., 0, 45);
    new TGeoTubeSeg( "sV0AFraR4b", fV0AR3-fV0AFraWd/2.,
		     fV0AR3+fV0AFraWd/2., fV0ASciWd/2., 0, 45);
    new TGeoTubeSeg( "sV0AFraR5b", fV0AR4-fV0AFraWd/2.,
		     fV0AR4+fV0AFraWd/2., fV0ASciWd/2., 0, 45);
    TGeoCompositeShape *sV0AFraR1 = new TGeoCompositeShape("sV0AFraR1","sV0AFraR1b-sV0ACha");
    TGeoCompositeShape *sV0AFraR2 = new TGeoCompositeShape("sV0AFraR2","sV0AFraR2b-sV0ACha");
    TGeoCompositeShape *sV0AFraR3 = new TGeoCompositeShape("sV0AFraR3","sV0AFraR3b-sV0ACha");
    TGeoCompositeShape *sV0AFraR4 = new TGeoCompositeShape("sV0AFraR4","sV0AFraR4b-sV0ACha");
    TGeoCompositeShape *sV0AFraR5 = new TGeoCompositeShape("sV0AFraR5","sV0AFraR5b-sV0ACha");
    TGeoVolume *v0AFraR1 = new TGeoVolume("V0AFraR1",sV0AFraR1,medV0AFra);
    TGeoVolume *v0AFraR2 = new TGeoVolume("V0AFraR2",sV0AFraR2,medV0AFra);
    TGeoVolume *v0AFraR3 = new TGeoVolume("V0AFraR3",sV0AFraR3,medV0AFra);
    TGeoVolume *v0AFraR4 = new TGeoVolume("V0AFraR4",sV0AFraR4,medV0AFra);
    TGeoVolume *v0AFraR5 = new TGeoVolume("V0AFraR5",sV0AFraR5,medV0AFra);
    v0AFraR1->SetLineColor(kV0AColorFra); v0AFraR2->SetLineColor(kV0AColorFra);
    v0AFraR3->SetLineColor(kV0AColorFra); v0AFraR4->SetLineColor(kV0AColorFra);
    v0AFraR5->SetLineColor(kV0AColorFra);
    v0AFra->AddNode(v0AFraR1,1);
    v0AFra->AddNode(v0AFraR2,1);
    v0AFra->AddNode(v0AFraR3,1); 
    v0AFra->AddNode(v0AFraR4,1);
    v0AFra->AddNode(v0AFraR5,1);
    v0ASec->AddNode(v0AFra,1);

    /// Sensitive scintilator
    TGeoVolume *v0ASci = new TGeoVolumeAssembly("V0ASci");
    new TGeoTubeSeg( "sV0AR1b", fV0AR0+fV0AFraWd/2.,
		     fV0AR1-fV0AFraWd/2., fV0ASciWd/2., 0, 45);
    new TGeoTubeSeg( "sV0AR2b", fV0AR1+fV0AFraWd/2.,
		     fV0AR2-fV0AFraWd/2., fV0ASciWd/2., 0, 45);
    new TGeoTubeSeg( "sV0AR3b", fV0AR2+fV0AFraWd/2.,
		     fV0AR3-fV0AFraWd/2., fV0ASciWd/2., 0, 45);
    new TGeoTubeSeg( "sV0AR4b", fV0AR3+fV0AFraWd/2.,
		     fV0AR4-fV0AFraWd/2., fV0ASciWd/2., 0, 45);
    TGeoCompositeShape *sV0AR1 = new TGeoCompositeShape("sV0AR1","sV0AR1b-sV0ACha");
    TGeoCompositeShape *sV0AR2 = new TGeoCompositeShape("sV0AR2","sV0AR2b-sV0ACha");
    TGeoCompositeShape *sV0AR3 = new TGeoCompositeShape("sV0AR3","sV0AR3b-sV0ACha");
    TGeoCompositeShape *sV0AR4 = new TGeoCompositeShape("sV0AR4","sV0AR4b-sV0ACha");
    TGeoVolume *v0L1 = new TGeoVolume("V0L1",sV0AR1,medV0ASci);
    TGeoVolume *v0L2 = new TGeoVolume("V0L2",sV0AR2,medV0ASci);
    TGeoVolume *v0L3 = new TGeoVolume("V0L3",sV0AR3,medV0ASci);
    TGeoVolume *v0L4 = new TGeoVolume("V0L4",sV0AR4,medV0ASci);
    v0L1->SetLineColor(kV0AColorSci); v0L2->SetLineColor(kV0AColorSci);
    v0L3->SetLineColor(kV0AColorSci); v0L4->SetLineColor(kV0AColorSci);
    v0ASec->AddNode(v0L1,1);
    v0ASec->AddNode(v0L2,1);
    v0ASec->AddNode(v0L3,1);
    v0ASec->AddNode(v0L4,1);
    

    /// Non-sensitive scintilator
    new TGeoTubeSeg("sV0AR5S2", fV0AR4+fV0AFraWd/2., fV0AR4 + fV0AR0, fV0ASciWd/2.+2*preShape, 0, 45);
    TGeoCompositeShape *sV0AR5 = new TGeoCompositeShape("V0AR5","sV0AR5S2 - sV0ACha");
    TGeoVolume *v0AR5 = new TGeoVolume("V0AR5",sV0AR5,medV0ASci);
    v0AR5->SetLineColor(kV0AColorSci);
    v0ASci->AddNode(v0AR5,1);
    v0ASec->AddNode(v0ASci,1); 

    /// Segment of innermost octagon
    TGeoVolume *v0ASup = new TGeoVolumeAssembly("V0ASup");
    
    /// Segment of outtermost octagon
    for (int i=0;i<2;i++) {
    v0APts[0+8*i] =  fV0AR6-fV0AOctH2;	v0APts[1+8*i] = 0.;
      v0APts[2+8*i] = (fV0AR6-fV0AOctH2)*sin45;  v0APts[3+8*i] = (fV0AR6-fV0AOctH2)*sin45;
      v0APts[4+8*i] = fV0AR6*sin45;		v0APts[5+8*i] = fV0AR6*sin45;
      v0APts[6+8*i] = fV0AR6;			v0APts[7+8*i] = 0.;
    }
    TGeoArb8 *sV0AOct2 = new TGeoArb8("sV0AOct2", (fV0ASciWd+2*fV0AOctWd)/2., v0APts);
    TGeoVolume *v0AOct2 = new TGeoVolume("V0AOct2", sV0AOct2,medV0ASup);
    v0AOct2->SetLineColor(kV0AColorOct);
    v0ASup->AddNode(v0AOct2,1);
    v0ASec->AddNode(v0ASup,1);


    //Bunch of fibers
    v0APts[ 0] = v0APts[ 2] = -14.0;
    v0APts[ 1] = v0APts[ 7] = (fV0ASciWd+fV0AOctWd)/2.-0.01;
    v0APts[ 3] = v0APts[ 5] = (fV0ASciWd+fV0AOctWd)/2.+0.01;
    v0APts[ 4] = v0APts[ 6] = +14.0;
    v0APts[ 8] = v0APts[10] = -10.0;
    v0APts[ 9] = v0APts[15] = 0.;
    v0APts[11] = v0APts[13] = 0.25;
    v0APts[12] = v0APts[14] = +10.0;
    TGeoArb8 *sV0AFib = new TGeoArb8("sV0AFib", 9.0, v0APts);
    TGeoVolume *v0AFib1 = new TGeoVolume("V0AFib1",sV0AFib,medV0AFib);
    TGeoVolume *v0AFib = new TGeoVolumeAssembly("V0AFib");
    TGeoRotation *rot = new TGeoRotation("rot");
    rot->RotateX(-90);
    rot->RotateZ(-90.+22.5);
    v0AFib->AddNode(v0AFib1,1,rot);
    rot = new TGeoRotation("rot");
    rot->RotateX(-90);
    rot->RotateY(180);
    rot->RotateZ(-90.+22.5);
    v0AFib->SetLineColor(kV0AColorFib);
    v0AFib->AddNode(v0AFib1,2,rot);
    v0ASec->AddNode(v0AFib,1,new TGeoTranslation((fV0AR6-fV0AOctH2+fV0AR5)*cos225/2. - 1.0, (fV0AR6-fV0AOctH2+fV0AR5)*sin225/2., 0));
    
  
    /// Plates
    new TGeoTube("sV0ANail1PlaInHole", 0.0, 0.4, (fV0APlaWd-2*fV0APlaAl)/2.);
    new TGeoTube("sV0ANail2PlaInHole", 0.0, 0.4, (fV0APlaWd-2*fV0APlaAl)/2.);
    new TGeoCompositeShape("sV0ANailsPlaInHoles","sV0ANail1PlaInHole:pos1+sV0ANail2PlaInHole:pos2");
    new TGeoTube("sV0ANail1PlaOuHole", 0.0, 0.4, (fV0APlaAl)/2.);
    new TGeoTube("sV0ANail2PlaOuHole", 0.0, 0.4, (fV0APlaAl)/2.);
    new TGeoCompositeShape("sV0ANailsPlaOuHoles","sV0ANail1PlaOuHole:pos1+sV0ANail2PlaOuHole:pos2");
    for (int i=0;i<2;i++) {
      v0APts[0+8*i] = fV0AR0;			v0APts[1+8*i] = 0.;
      v0APts[2+8*i] = fV0AR0*sin45;		v0APts[3+8*i] = fV0AR0*sin45;
      v0APts[4+8*i] = fV0AR6 * sin45;	v0APts[5+8*i] = fV0AR6*sin45;
      v0APts[6+8*i] = fV0AR6;		v0APts[7+8*i] = 0.;
    }
    new TGeoArb8("sV0APlaIn", (fV0APlaWd-2*fV0APlaAl)/2., v0APts);
    TGeoCompositeShape *sV0APlaInNailsHoles = new TGeoCompositeShape("sV0APlaInNailsHoles","sV0APlaIn-sV0ANailsPlaInHoles");
    TGeoVolume *v0APlaInNailsHoles = new TGeoVolume("V0APlaInNailsHoles", sV0APlaInNailsHoles, medV0APlaIn);
    new TGeoArb8("sV0APlaOu", fV0APlaAl/2., v0APts);
    TGeoCompositeShape *sV0APlaOuNailsHoles = new TGeoCompositeShape("sV0APlaOuNailsHoles","sV0APlaOu-sV0ANailsPlaOuHoles"); 
    TGeoVolume *v0APlaOuNailsHoles = new TGeoVolume("V0APlaOuNailsHoles", sV0APlaOuNailsHoles, medV0APlaOu);
    v0APlaInNailsHoles->SetLineColor(kV0AColorPlaIn); v0APlaOuNailsHoles->SetLineColor(kV0AColorPlaOu);
    TGeoVolume *v0APla = new TGeoVolumeAssembly("V0APla");
    v0APla->AddNode(v0APlaInNailsHoles,1);
    v0APla->AddNode(v0APlaOuNailsHoles,1,new TGeoTranslation(0,0,(fV0APlaWd-fV0APlaAl)/2.));
    v0APla->AddNode(v0APlaOuNailsHoles,2,new TGeoTranslation(0,0,-(fV0APlaWd-fV0APlaAl)/2.));
    v0ASec->AddNode(v0APla,1,new TGeoTranslation(0,0,(fV0ASciWd+2*fV0AOctWd+fV0APlaWd)/2.));
    v0ASec->AddNode(v0APla,2,new TGeoTranslation(0,0,-(fV0ASciWd+2*fV0AOctWd+fV0APlaWd)/2.));

    /// PMBox
    TGeoVolume* v0APM = new TGeoVolumeAssembly("V0APM");
    new TGeoBBox("sV0APMB1", fV0APMBWd/2., fV0APMBHt/2., fV0APMBTh/2.);
    new TGeoBBox("sV0APMB2", fV0APMBWd/2.-fV0APMBWdW, fV0APMBHt/2.-fV0APMBHtW, fV0APMBTh/2.-fV0APMBThW);
    TGeoCompositeShape *sV0APMB = new TGeoCompositeShape("sV0APMB","sV0APMB1-sV0APMB2");
    TGeoVolume *v0APMB = new TGeoVolume("V0APMB",sV0APMB, medV0APMAlum);
    v0APMB->SetLineColor(kV0AColorPMA);
    v0APM->AddNode(v0APMB,1);

    /// PMTubes
    TGeoTube *sV0APMT1 = new TGeoTube("sV0APMT1", fV0APMTR1, fV0APMTR2, fV0APMTH/2.);
    TGeoVolume *v0APMT1 = new TGeoVolume("V0APMT1", sV0APMT1, medV0APMGlass);
    TGeoTube *sV0APMT2 = new TGeoTube("sV0APMT2", fV0APMTR3, fV0APMTR4, fV0APMTH/2.);
    TGeoVolume *v0APMT2 = new TGeoVolume("V0APMT2", sV0APMT2, medV0APMAlum);
    TGeoVolume *v0APMT = new TGeoVolumeAssembly("V0APMT");
    TGeoTube *sV0APMTT = new TGeoTube("sV0APMTT", 0., fV0APMTR4, fV0APMTB/2.);
    TGeoVolume *v0APMTT = new TGeoVolume("V0APMTT", sV0APMTT, medV0APMAlum);
    v0APMT1->SetLineColor(kV0AColorPMG);
    v0APMT2->SetLineColor(kV0AColorPMA);
    v0APMTT->SetLineColor(kV0AColorPMA);
    rot = new TGeoRotation("rot", 90, 0, 180, 0, 90, 90);
    v0APMT->AddNode(v0APMT1,1,rot);
    v0APMT->AddNode(v0APMT2,1,rot);
    v0APMT->AddNode(v0APMTT,1,new TGeoCombiTrans(0,-(fV0APMTH+fV0APMTB)/2.,0,rot));
    double autoShift = (fV0APMBWd-2*fV0APMBWdW)/4.;
    v0APM->AddNode(v0APMT, 1, new TGeoTranslation(-1.5*autoShift, 0, 0));
    v0APM->AddNode(v0APMT, 2, new TGeoTranslation(-0.5*autoShift, 0, 0));
    v0APM->AddNode(v0APMT, 3, new TGeoTranslation(+0.5*autoShift, 0, 0));
    v0APM->AddNode(v0APMT, 4, new TGeoTranslation(+1.5*autoShift, 0, 0));

    /// PM
    rot = new TGeoRotation("rot");
    rot->RotateX(90-fV0APMBAng);
    rot->RotateZ(-90.+22.5);
    double cosAngPMB = TMath::Cos(fV0APMBAng*TMath::DegToRad());
    double sinAngPMB = TMath::Sin(fV0APMBAng*TMath::DegToRad());
    double shiftZ = fV0APMBHt/2. * cosAngPMB
      -   ( fV0ASciWd + 2 * fV0AOctWd + 2 * fV0APlaWd )/2.   -   fV0APMBTh/2. * sinAngPMB;
    double shiftR = fV0AR6  +  fV0AOctH2 + fV0APlaAl;
    v0ASec->AddNode(v0APM,1, new TGeoCombiTrans( shiftR*cos225+1.07, shiftR*sin225, shiftZ, rot));
    
    // Aluminium nails 
    TGeoTube *sV0ANail1 = new TGeoTube("sV0ANail1", 0.0, 0.4, 5.09/2.);
    TGeoVolume *v0ANail1 = new TGeoVolume("V0ANail1", sV0ANail1, medV0APMAlum);
    v0ANail1->SetLineColor(kV0AColorPMA);// this is the color for aluminium
    v0ASec->AddNode(v0ANail1,1,new TGeoTranslation(42.9, 0.51, 0.0));
    TGeoTube *sV0ANail2 = new TGeoTube("sV0ANail2", 0.0, 0.4, 5.09/2.);
    TGeoVolume *v0ANail2 = new TGeoVolume("V0ANail2", sV0ANail2, medV0APMAlum);
    v0ANail2->SetLineColor(kV0AColorPMA);
    v0ASec->AddNode(v0ANail2,1,new TGeoTranslation(30.8,30.04,0.0));
    
    
    /// Replicate sectors
    TGeoVolume *v0LE = new TGeoVolumeAssembly("V0LE");
    for(int i=0; i<4; i++) {
      TGeoRotation *rot = new TGeoRotation("rot", 90., i*45., 90., 90.+i*45., 0., 0.);
      v0LE->AddNode(v0ASec,i+1,rot);  /// modificacion +1 anhadido
    }
   
    //Upper supports
    for (int i=0;i<2;i++){
    v0APts[0+8*i] = 0.2;	    v0APts[1+8*i] = 45.5;  
    v0APts[2+8*i] = 0.2;          v0APts[3+8*i] = 70.5;   //70.6
    v0APts[4+8*i] = 4.0;	    v0APts[5+8*i] = 68.9;
    v0APts[6+8*i] = 4.0;	    v0APts[7+8*i] = 45.5;  
    }
    TGeoArb8 *sV0ASuppur = new TGeoArb8("sV0ASuppur", 2.0, v0APts);    
    TGeoVolume *v0ASuppur = new TGeoVolume("V0ASuppur", sV0ASuppur, medV0ASup);
    v0ASuppur->SetLineColor(kV0AColorOct);
    v0LE->AddNode(v0ASuppur,1);
    for (int i=0;i<2;i++){
    v0APts[0+8*i] = -0.2;	    v0APts[1+8*i] = 45.3;
    v0APts[2+8*i] = -0.2;            v0APts[3+8*i] = 70.6;
    v0APts[4+8*i] = -4.0;	    v0APts[5+8*i] = 68.9;
    v0APts[6+8*i] = -4.0;	    v0APts[7+8*i] = 45.3;
    }
    TGeoArb8 *sV0ASuppul = new TGeoArb8("sV0ASuppul", 2.0, v0APts);    
    TGeoVolume *v0ASuppul = new TGeoVolume("V0ASuppul", sV0ASuppul, medV0ASup);
    v0ASuppul->SetLineColor(kV0AColorOct);
    v0LE->AddNode(v0ASuppul,1);
  

      //Definition of sector 5
   
       TGeoVolume *v0ASec5 = new TGeoVolumeAssembly("V0ASec5"); 

 /// For boolean sustraction
    double preShape5 = 0.2;
    for (int i=0;i<2;i++) {
      v0APts[0+8*i] = -fV0AR0+fV0AFraWd/2.-preShape5;  v0APts[1+8*i] = -preShape5;
      v0APts[2+8*i] = -fV0AR0+fV0AFraWd/2.-preShape5;  v0APts[3+8*i] = fV0AFraWd/2.;
      v0APts[4+8*i] = -fV0AR4-fV0AFraWd/2.+preShape5;  v0APts[5+8*i] = fV0AFraWd/2.;
      v0APts[6+8*i] = -fV0AR4-fV0AFraWd/2.+preShape5;  v0APts[7+8*i] = -preShape5;
    }
    new TGeoArb8("sV0ACha15",fV0ASciWd/1.5,v0APts);
    for (int i=0;i<2;i++) {
      v0APts[0+8*i] = -fV0AR0*cos65;
      v0APts[1+8*i] = -(fV0AR0-fV0AFraWd)*sin65;
      v0APts[2+8*i] = -(fV0AR0-fV0AFraWd/2.)*cos65;
      v0APts[3+8*i] = -(fV0AR0-fV0AFraWd/2.)*sin65;
      v0APts[4+8*i] = -(fV0AR4+fV0AFraWd/2.)*cos65;
      v0APts[5+8*i] = -(fV0AR4+fV0AFraWd/2.)*sin65;
      v0APts[6+8*i] = -(fV0AR4+fV0AFraWd)*cos65;
      v0APts[7+8*i] = -fV0AR4*sin65;
    }
    new TGeoArb8("sV0ACha25", fV0ASciWd/2.+2.*preShape5, v0APts);
    new TGeoCompositeShape("sV0ACha125","sV0ACha15+sV0ACha25");
    new TGeoTube("sV0ANail15Hole", 0.0, 0.4, 1.65);
    TGeoTranslation *pos15 = new TGeoTranslation("pos15", -42.9, -0.51, 0.0);
    pos15->RegisterYourself();
    new TGeoTube("sV0ANail25Hole", 0.0, 0.4, 1.65);
    TGeoTranslation *pos25 = new TGeoTranslation("pos25",-30.8,-30.04,0.0);
    pos25->RegisterYourself();
    new TGeoTube("sV0ANail35Hole", 0.0, 0.4, 1.65);
    TGeoTranslation *pos35 = new TGeoTranslation("pos35", -30.05,-30.79,0.0);
    pos35->RegisterYourself();
    new TGeoCompositeShape("sV0ANailsHoles5","sV0ANail15Hole:pos15+sV0ANail25Hole:pos25+sV0ANail35Hole:pos35");
    new TGeoCompositeShape("sV0ACha5","sV0ACha125+sV0ANailsHoles5");

    /// Frame
    TGeoVolume *v0AFra5 = new TGeoVolumeAssembly("V0AFra5");
    for (int i=0;i<2;i++) {
      v0APts[0+8*i] = -fV0AR0+fV0AFraWd/2.;  v0APts[1+8*i] = 0.;
      v0APts[2+8*i] = -fV0AR0+fV0AFraWd/2.;  v0APts[3+8*i] = fV0AFraWd/2.;
      v0APts[4+8*i] = -fV0AR4-fV0AFraWd/2.;  v0APts[5+8*i] = fV0AFraWd/2.;
      v0APts[6+8*i] = -fV0AR4-fV0AFraWd/2.;  v0APts[7+8*i] = 0.;
    }
    TGeoArb8 *sV0AFraB15 = new TGeoArb8("sV0AFraB15",fV0ASciWd/2.,v0APts);
    TGeoVolume *v0AFraB15 = new TGeoVolume("V0AFraB15",sV0AFraB15,medV0AFra);
    for (int i=0;i<2;i++) {
      v0APts[0+8*i] = -fV0AR0*cos65;
      v0APts[1+8*i] = -(fV0AR0-fV0AFraWd)*sin65;
      v0APts[2+8*i] = -(fV0AR0-fV0AFraWd/2.)*cos65;
      v0APts[3+8*i] = -(fV0AR0-fV0AFraWd/2.)*sin65;
      v0APts[4+8*i] = -(fV0AR4+fV0AFraWd/2.)*cos65;
      v0APts[5+8*i] = -(fV0AR4+fV0AFraWd/2.)*sin65;
      v0APts[6+8*i] = -(fV0AR4+fV0AFraWd)*cos65;
      v0APts[7+8*i] = -fV0AR4*sin65;
    }
    TGeoArb8 *sV0AFraB25 = new TGeoArb8("sV0AFraB25", fV0ASciWd/2., v0APts);
    TGeoVolume *v0AFraB25 = new TGeoVolume("V0AFraB25",sV0AFraB25,medV0AFra);
    v0AFraB15->SetLineColor(kV0AColorFra); v0AFraB25->SetLineColor(kV0AColorFra);
    v0AFra5->AddNode(v0AFraB15,1);
    v0AFra5->AddNode(v0AFraB25,1);  // Prefer 2 GeoObjects insted of 3 GeoMovements
    new TGeoTubeSeg( "sV0AFraR1b5", fV0AR0-fV0AFraWd/2.,
		     fV0AR0+fV0AFraWd/2., fV0ASciWd/2., 180.0, 245.4);
    new TGeoTubeSeg( "sV0AFraR2b5", fV0AR1-fV0AFraWd/2.,
		     fV0AR1+fV0AFraWd/2., fV0ASciWd/2., 180.0, 245.4);
    new TGeoTubeSeg( "sV0AFraR3b5", fV0AR2-fV0AFraWd/2.,
		     fV0AR2+fV0AFraWd/2., fV0ASciWd/2., 180.0, 245.4);
    new TGeoTubeSeg( "sV0AFraR4b5", fV0AR3-fV0AFraWd/2.,
		     fV0AR3+fV0AFraWd/2., fV0ASciWd/2., 180.0, 245.4);
    new TGeoTubeSeg( "sV0AFraR5b5", fV0AR4-fV0AFraWd/2.,
		     fV0AR4+fV0AFraWd/2., fV0ASciWd/2., 180.0, 245.4);
    TGeoCompositeShape *sV0AFraR15 = new TGeoCompositeShape("sV0AFraR15","sV0AFraR1b5-sV0ACha5");
    TGeoCompositeShape *sV0AFraR25 = new TGeoCompositeShape("sV0AFraR25","sV0AFraR2b5-sV0ACha5");
    TGeoCompositeShape *sV0AFraR35 = new TGeoCompositeShape("sV0AFraR35","sV0AFraR3b5-sV0ACha5");
    TGeoCompositeShape *sV0AFraR45 = new TGeoCompositeShape("sV0AFraR45","sV0AFraR4b5-sV0ACha5");
    TGeoCompositeShape *sV0AFraR55 = new TGeoCompositeShape("sV0AFraR55","sV0AFraR5b5-sV0ACha5");
    TGeoVolume *v0AFraR15 = new TGeoVolume("V0AFraR15",sV0AFraR15,medV0AFra);
    TGeoVolume *v0AFraR25 = new TGeoVolume("V0AFraR25",sV0AFraR25,medV0AFra);
    TGeoVolume *v0AFraR35 = new TGeoVolume("V0AFraR35",sV0AFraR35,medV0AFra);
    TGeoVolume *v0AFraR45 = new TGeoVolume("V0AFraR45",sV0AFraR45,medV0AFra);
    TGeoVolume *v0AFraR55 = new TGeoVolume("V0AFraR55",sV0AFraR55,medV0AFra);
    v0AFraR15->SetLineColor(kV0AColorFra); v0AFraR25->SetLineColor(kV0AColorFra);
    v0AFraR35->SetLineColor(kV0AColorFra); v0AFraR45->SetLineColor(kV0AColorFra);
    v0AFraR55->SetLineColor(kV0AColorFra);
    v0AFra5->AddNode(v0AFraR15,1);
    v0AFra5->AddNode(v0AFraR25,1);
    v0AFra5->AddNode(v0AFraR35,1);
    v0AFra5->AddNode(v0AFraR45,1);
    v0AFra5->AddNode(v0AFraR55,1);
    v0ASec5->AddNode(v0AFra5,1);

    /// Sensitive scintilator
    TGeoVolume *v0ASci5 = new TGeoVolumeAssembly("V0ASci5");
    new TGeoTubeSeg( "sV0AR1b5", fV0AR0+fV0AFraWd/2.,
		     fV0AR1-fV0AFraWd/2., fV0ASciWd/2., 180.0, 245.4);
    new TGeoTubeSeg( "sV0AR2b5", fV0AR1+fV0AFraWd/2.,
		     fV0AR2-fV0AFraWd/2., fV0ASciWd/2., 180.0, 245.4);
    new TGeoTubeSeg( "sV0AR3b5", fV0AR2+fV0AFraWd/2.,
		     fV0AR3-fV0AFraWd/2., fV0ASciWd/2., 180.0, 245.4);
    new TGeoTubeSeg( "sV0AR4b5", fV0AR3+fV0AFraWd/2.,
		     fV0AR4-fV0AFraWd/2., fV0ASciWd/2., 180.0, 245.4);
    TGeoCompositeShape *sV0AR15 = new TGeoCompositeShape("sV0AR15","sV0AR1b5-sV0ACha5");
    TGeoCompositeShape *sV0AR25 = new TGeoCompositeShape("sV0AR25","sV0AR2b5-sV0ACha5");
    TGeoCompositeShape *sV0AR35 = new TGeoCompositeShape("sV0AR35","sV0AR3b5-sV0ACha5");
    TGeoCompositeShape *sV0AR45 = new TGeoCompositeShape("sV0AR45","sV0AR4b5-sV0ACha5");
    TGeoVolume *v0L15 = new TGeoVolume("V0L15",sV0AR15,medV0ASci);
    TGeoVolume *v0L25 = new TGeoVolume("V0L25",sV0AR25,medV0ASci);
    TGeoVolume *v0L35 = new TGeoVolume("V0L35",sV0AR35,medV0ASci);
    TGeoVolume *v0L45 = new TGeoVolume("V0L45",sV0AR45,medV0ASci);
    v0L15->SetLineColor(kV0AColorSci); v0L25->SetLineColor(kV0AColorSci);
    v0L35->SetLineColor(kV0AColorSci); v0L45->SetLineColor(kV0AColorSci);
    v0ASci5->AddNode(v0L15,1);
    v0ASci5->AddNode(v0L25,1);
    v0ASci5->AddNode(v0L35,1);
    v0ASci5->AddNode(v0L45,1);

    /// Non-sensitive scintilator
    new TGeoTubeSeg("sV0AR5S25", fV0AR4+fV0AFraWd/2., fV0AR4 + fV0AR0, fV0ASciWd/2.+2*preShape, 180.0, 245.4);
    TGeoCompositeShape *sV0AR55 = new TGeoCompositeShape("V0AR55","sV0AR5S25 - sV0ACha5");
    TGeoVolume *v0AR55 = new TGeoVolume("V0AR55",sV0AR55,medV0ASci);
    v0AR55->SetLineColor(kV0AColorSci);
    v0ASci5->AddNode(v0AR55,1);
    v0ASec5->AddNode(v0ASci5,1);

    /// Segment of innermost octagon
    TGeoVolume *v0ASup5 = new TGeoVolumeAssembly("V0ASup5");

/// Segment of outtermost octagon   
     for (int i=0;i<2;i++) {
    v0APts[0+8*i] = -fV0AR6+fV0AOctH2;	         v0APts[1+8*i] = 0.;
    v0APts[2+8*i] = -(fV0AR7-fV0AOctH2)*cos654;   v0APts[3+8*i] = -(fV0AR7-fV0AOctH2)*sin654;
    v0APts[4+8*i] = -fV0AR7*cos654;	         v0APts[5+8*i] = -fV0AR7*sin654;
    v0APts[6+8*i] = -fV0AR6;			 v0APts[7+8*i] = 0.;
    }
    TGeoArb8 *sV0AOct25 = new TGeoArb8("sV0AOct25", (fV0ASciWd+2*fV0AOctWd)/2., v0APts);
    TGeoVolume *v0AOct25 = new TGeoVolume("V0AOct25", sV0AOct25,medV0ASup);
    v0AOct25->SetLineColor(kV0AColorOct);
    v0ASup5->AddNode(v0AOct25,1);
    v0ASec5->AddNode(v0ASup5,1);

    //Bunch of fibers
    v0APts[ 0] = v0APts[ 2] = -14.0;
    v0APts[ 1] = v0APts[ 7] = (fV0ASciWd+fV0AOctWd)/2.-0.01;
    v0APts[ 3] = v0APts[ 5] = (fV0ASciWd+fV0AOctWd)/2.+0.01;
    v0APts[ 4] = v0APts[ 6] = +14.0;
    v0APts[ 8] = v0APts[10] = -10.0;
    v0APts[ 9] = v0APts[15] = 0.;
    v0APts[11] = v0APts[13] = 0.25;
    v0APts[12] = v0APts[14] = +10.0;
    TGeoArb8 *sV0AFiba5 = new TGeoArb8("sV0AFiba5", 9.0, v0APts);
    TGeoVolume *v0AFiba15 = new TGeoVolume("V0AFiba15",sV0AFiba5,medV0AFib);
    TGeoVolume *v0AFiba5 = new TGeoVolumeAssembly("V0AFiba5");
    rot = new TGeoRotation("rot");
    rot->RotateX(-90);
    rot->RotateZ(90+22.5);
    v0AFiba5->AddNode(v0AFiba15,1,rot);
    rot = new TGeoRotation("rot");
    rot->RotateX(-90);
    rot->RotateY(180);
    rot->RotateZ(90+22.5);
    v0AFiba5->SetLineColor(kV0AColorFib);
    v0AFiba5->AddNode(v0AFiba15,2,rot);
    v0ASec5->AddNode(v0AFiba5,1,new TGeoTranslation(-(fV0AR6-fV0AOctH2+fV0AR5)*cos225/2. + 1.5, -(fV0AR6-fV0AOctH2+fV0AR5)*sin225/2. - 1.0, 0));
    for (int i=0;i<2;i++) {
    v0APts[0+8*i] = -33.47;	      v0APts[1+8*i] = -35.13;
    v0APts[2+8*i] = -50.48;            v0APts[3+8*i] = -46.97;
    v0APts[4+8*i] = -44.66;            v0APts[5+8*i] = -61.92;
    v0APts[6+8*i] = -7.9;	      v0APts[7+8*i] = -47.18;
    }
    TGeoArb8 *sV0AFibb5 = new TGeoArb8("sV0AFibb8", 1.25, v0APts);
    TGeoVolume *v0AFibb15 = new TGeoVolume("V0AFibb15",sV0AFibb5,medV0AFib);
    TGeoVolume *v0AFibb5 = new TGeoVolumeAssembly("V0AFibb5");
    v0AFibb5->AddNode(v0AFibb15,1);
    v0AFibb5->SetLineColor(kV0AColorFib);
    v0ASec5->AddNode(v0AFibb5,1);    

    /// Plates
    new TGeoTube("sV0ANail1PlaInHole5", 0.0, 0.4, (fV0APlaWd-2*fV0APlaAl)/2.);
    new TGeoTube("sV0ANail2PlaInHole5", 0.0, 0.4, (fV0APlaWd-2*fV0APlaAl)/2.);
    new TGeoTube("sV0ANail3PlaInHole5", 0.0, 0.4, (fV0APlaWd-2*fV0APlaAl)/2.);
    new TGeoCompositeShape("sV0ANailsPlaInHoles5","sV0ANail1PlaInHole5:pos15+sV0ANail2PlaInHole5:pos25+sV0ANail3PlaInHole5:pos35");
    new TGeoTube("sV0ANail1PlaOuHole5", 0.0, 0.4, (fV0APlaAl)/2.);
    new TGeoTube("sV0ANail2PlaOuHole5", 0.0, 0.4, (fV0APlaAl)/2.);
    new TGeoTube("sV0ANail3PlaOuHole5", 0.0, 0.4, (fV0APlaAl)/2.);
    new TGeoCompositeShape("sV0ANailsPlaOuHoles5","sV0ANail1PlaOuHole5:pos15+sV0ANail2PlaOuHole5:pos25+sV0ANail3PlaOuHole5:pos35");
    for (int i=0;i<2;i++) {
      v0APts[0+8*i] = -fV0AR0;			v0APts[1+8*i] = 0.;
      v0APts[2+8*i] = -fV0AR0*cos654;		v0APts[3+8*i] = -fV0AR0*sin654;
      v0APts[4+8*i] = -fV0AR7*cos654;    	v0APts[5+8*i] = -fV0AR7*sin654;
      v0APts[6+8*i] = -fV0AR6;    		v0APts[7+8*i] = 0.;
    }
    new TGeoArb8("sV0APlaIn5", (fV0APlaWd-2*fV0APlaAl)/2., v0APts);
    TGeoCompositeShape *sV0APlaInNailsHoles5 = new TGeoCompositeShape("sV0APlaInNailsHoles5","sV0APlaIn5-sV0ANailsPlaInHoles5");
    TGeoVolume *v0APlaInNailsHoles5 = new TGeoVolume("V0APlaInNailsHoles5", sV0APlaInNailsHoles5, medV0APlaIn);
    new TGeoArb8("sV0APlaOu5", fV0APlaAl/2., v0APts);
    TGeoCompositeShape *sV0APlaOuNailsHoles5 = new TGeoCompositeShape("sV0APlaOuNailsHoles5","sV0APlaOu5-sV0ANailsPlaOuHoles5"); 
    TGeoVolume *v0APlaOuNailsHoles5 = new TGeoVolume("V0APlaOuNailsHoles5", sV0APlaOuNailsHoles5, medV0APlaOu);
    v0APlaInNailsHoles5->SetLineColor(kV0AColorPlaIn); v0APlaOuNailsHoles5->SetLineColor(kV0AColorPlaOu);
    TGeoVolume *v0APla5 = new TGeoVolumeAssembly("V0APla5");
    v0APla5->AddNode(v0APlaInNailsHoles5,1);
    v0APla5->AddNode(v0APlaOuNailsHoles5,1,new TGeoTranslation(0,0,(fV0APlaWd-fV0APlaAl)/2.));
    v0APla5->AddNode(v0APlaOuNailsHoles5,2,new TGeoTranslation(0,0,-(fV0APlaWd-fV0APlaAl)/2.));
    v0ASec5->AddNode(v0APla5,1,new TGeoTranslation(0,0,(fV0ASciWd+2*fV0AOctWd+fV0APlaWd)/2.));
    v0ASec5->AddNode(v0APla5,2,new TGeoTranslation(0,0,-(fV0ASciWd+2*fV0AOctWd+fV0APlaWd)/2.));
    
    /// PMBox 1ero
    TGeoVolume* v0APM5 = new TGeoVolumeAssembly("V0APM5");
    new TGeoBBox("sV0APMB15", fV0APMBWd/2., fV0APMBHt/2., fV0APMBTh/2.);
    new TGeoBBox("sV0APMB25", fV0APMBWd/2.-fV0APMBWdW, fV0APMBHt/2.-fV0APMBHtW, fV0APMBTh/2.-fV0APMBThW);
    TGeoCompositeShape *sV0APMB5 = new TGeoCompositeShape("sV0APMB5","sV0APMB15-sV0APMB25");
    TGeoVolume *v0APMB5 = new TGeoVolume("V0APMB5",sV0APMB5, medV0APMAlum);
    v0APMB5->SetLineColor(kV0AColorPMA);
    v0APM5->AddNode(v0APMB5,1);

    /// PMTubes 1ero
    TGeoTube *sV0APMT15 = new TGeoTube("sV0APMT15", fV0APMTR1, fV0APMTR2, fV0APMTH/2.);
    TGeoVolume *v0APMT15 = new TGeoVolume("V0APMT15", sV0APMT15, medV0APMGlass);
    TGeoTube *sV0APMT25 = new TGeoTube("sV0APMT25", fV0APMTR3, fV0APMTR4, fV0APMTH/2.);
    TGeoVolume *v0APMT25 = new TGeoVolume("V0APMT25", sV0APMT25, medV0APMAlum);
    TGeoVolume *v0APMT5 = new TGeoVolumeAssembly("V0APMT5");
    TGeoTube *sV0APMTT5 = new TGeoTube("sV0APMTT5", 0., fV0APMTR4, fV0APMTB/2.);
    TGeoVolume *v0APMTT5 = new TGeoVolume("V0APMTT5", sV0APMTT5, medV0APMAlum);
    v0APMT5->SetLineColor(kV0AColorPMG);
    v0APMT25->SetLineColor(kV0AColorPMA);
    v0APMTT5->SetLineColor(kV0AColorPMA);
    rot = new TGeoRotation("rot", 90, 0, 180, 0, 90, 90);
    v0APMT5->AddNode(v0APMT15,1,rot);
    v0APMT5->AddNode(v0APMT25,1,rot);
    v0APMT5->AddNode(v0APMTT5,1,new TGeoCombiTrans(0,(fV0APMTH+fV0APMTB)/2.,0,rot));
    double autoShift5 = (fV0APMBWd-2*fV0APMBWdW)/4.;
    v0APM5->AddNode(v0APMT5, 1, new TGeoTranslation(-1.5*autoShift5, 0, 0));
    v0APM5->AddNode(v0APMT5, 2, new TGeoTranslation(-0.5*autoShift5, 0, 0));
    v0APM5->AddNode(v0APMT5, 3, new TGeoTranslation(+0.5*autoShift5, 0, 0));
    v0APM5->AddNode(v0APMT5, 4, new TGeoTranslation(+1.5*autoShift5, 0, 0));

    /// PM 1ero
    rot = new TGeoRotation("rot");
    rot->RotateX(-90+30);
    rot->RotateY(0); 
    rot->RotateZ(-65-3);
    double cosAngPMB5 = TMath::Cos(fV0APMBAng*TMath::DegToRad());
    double sinAngPMB5 = TMath::Sin(fV0APMBAng*TMath::DegToRad());
    double shiftZ5 = fV0APMBHt/2. * cosAngPMB5
      -   ( fV0ASciWd + 2 * fV0AOctWd + 2 * fV0APlaWd )/2.   -   fV0APMBTh/2. * sinAngPMB5;
    double shiftR5 = fV0AR6  +  fV0AOctH2 + fV0APlaAl;
    v0ASec5->AddNode(v0APM5,1, new TGeoCombiTrans( -shiftR5*cos225-1.3, -shiftR5*sin225, shiftZ5, rot));


    /// PMBox 2do 
    TGeoVolume* v0APM52 = new TGeoVolumeAssembly("V0APM52");
    new TGeoBBox("sV0APMB151", fV0APMBWd/2., fV0APMBHt/2., fV0APMBTh/2.);
    new TGeoBBox("sV0APMB252", fV0APMBWd/2.-fV0APMBWdW, fV0APMBHt/2.-fV0APMBHtW, fV0APMBTh/2.-fV0APMBThW);
    TGeoCompositeShape *sV0APMB52 = new TGeoCompositeShape("sV0APMB52","sV0APMB151-sV0APMB252");
    TGeoVolume *v0APMB52 = new TGeoVolume("V0APMB52",sV0APMB52, medV0APMAlum);
    v0APMB52->SetLineColor(kV0AColorPMA);
    v0APM52->AddNode(v0APMB52,1);

    /// PMTubes 2ndo
    TGeoTube *sV0APMT152 = new TGeoTube("sV0APMT152", fV0APMTR1, fV0APMTR2, fV0APMTH/2.);
    TGeoVolume *v0APMT152 = new TGeoVolume("V0APMT152", sV0APMT152, medV0APMGlass);
    TGeoTube *sV0APMT252 = new TGeoTube("sV0APMT252", fV0APMTR3, fV0APMTR4, fV0APMTH/2.);
    TGeoVolume *v0APMT252 = new TGeoVolume("V0APMT252", sV0APMT252, medV0APMAlum);
    TGeoVolume *v0APMT52 = new TGeoVolumeAssembly("V0APMT52"); // pk si no se confunde con la 752 o con la 794
    TGeoTube *sV0APMTT52 = new TGeoTube("sV0APMTT52", 0., fV0APMTR4, fV0APMTB/2.);
    TGeoVolume *v0APMTT52 = new TGeoVolume("V0APMT52", sV0APMTT52, medV0APMAlum);
    v0APMT52->SetLineColor(kV0AColorPMG);
    v0APMT252->SetLineColor(kV0AColorPMA);
    v0APMTT52->SetLineColor(kV0AColorPMA);
    rot = new TGeoRotation("rot", 90, 0, 180, 0, 90, 90);
    v0APMT52->AddNode(v0APMT152,1,rot);
    v0APMT52->AddNode(v0APMT252,1,rot);
    v0APMT52->AddNode(v0APMTT52,1,new TGeoCombiTrans(0,(fV0APMTH+fV0APMTB)/2.,0,rot));
    double autoShift52 = (fV0APMBWd-2*fV0APMBWdW)/4.;
    v0APM52->AddNode(v0APMT52, 1, new TGeoTranslation(-1.5*autoShift52, 0, 0));
    v0APM52->AddNode(v0APMT52, 2, new TGeoTranslation(-0.5*autoShift52, 0, 0));
    v0APM52->AddNode(v0APMT52, 3, new TGeoTranslation(+0.5*autoShift52, 0, 0));
    v0APM52->AddNode(v0APMT52, 4, new TGeoTranslation(+1.5*autoShift52, 0, 0));

    /// PM 2ndo
    rot = new TGeoRotation("rot");
    rot->RotateX(-90+30);
    rot->RotateY(0);
    rot->RotateZ(-65-3);
    double cosAngPMB52 = TMath::Cos(fV0APMBAng*TMath::DegToRad());
    double sinAngPMB52 = TMath::Sin(fV0APMBAng*TMath::DegToRad());
    double shiftZ52 = fV0APMBHt/2. * cosAngPMB52
      -   ( fV0ASciWd + 2 * fV0AOctWd + 2 * fV0APlaWd )/2.   -   fV0APMBTh/2. * sinAngPMB52;
    double shiftR52 = fV0AR6  + fV0AR1 + fV0AOctWd + fV0APlaAl/3.;
    v0ASec5->AddNode(v0APM52,1, new TGeoCombiTrans( -shiftR52*cos45-1.3, -shiftR52*sin45, shiftZ52, rot));    
    

    // Replicate sectors
    //for(int i=0; i<1; i++) {
    //TGeoRotation *rot = new TGeoRotation("rot", 90., i*45., 90., 90.+i*45., 0., 0.);
    v0LE->AddNode(v0ASec5, 1);   //i + 1,rot);
    //}


     //Definition of  sector 6
   
    TGeoVolume *v0ASec6 = new TGeoVolumeAssembly("V0ASec6");

    /// For boolean sustraction
    double preShape6 = 0.2;
    for (int i=0;i<2;i++) {
    v0APts[0+8*i] = -preShape6;                      v0APts[1+8*i] = -fV0AR0+fV0AFraWd/2.-preShape6;
    v0APts[2+8*i] = -fV0AFraWd/2.;                    v0APts[3+8*i] = -fV0AR0+fV0AFraWd/2.-preShape6;
    v0APts[4+8*i] = -fV0AFraWd/2.;                    v0APts[5+8*i] = -fV0AR4-fV0AFraWd/2.+preShape6;
    v0APts[6+8*i] = -preShape6;                      v0APts[7+8*i] = -fV0AR4-fV0AFraWd/2.+preShape6;
    }
    new TGeoArb8("sV0ACha16",fV0ASciWd/1.5,v0APts);
    for (int i=0;i<2;i++) {
    v0APts[0+8*i] = -fV0AR0*cos665;
    v0APts[1+8*i] = -(fV0AR0-fV0AFraWd)*sin665;
    v0APts[2+8*i] = -(fV0AR0-fV0AFraWd/2.)*cos665;
    v0APts[3+8*i] = -(fV0AR0-fV0AFraWd/2.)*sin665;
    v0APts[4+8*i] = -(fV0AR4+fV0AFraWd/2.)*cos665;
    v0APts[5+8*i] = -(fV0AR4+fV0AFraWd/2.)*sin665;
    v0APts[6+8*i] = -(fV0AR4+fV0AFraWd)*cos665;
    v0APts[7+8*i] = -fV0AR4*sin665;
    }
    new TGeoArb8("sV0ACha26", fV0ASciWd/2.+2.*preShape, v0APts);
    new TGeoCompositeShape("sV0ACha126","sV0ACha16+sV0ACha26");
    new TGeoTube("sV0ANail16Hole", 0.0, 0.4, 1.65);
    TGeoTranslation *pos16 = new TGeoTranslation("pos16",-0.51,-42.9,0.0);
    pos16->RegisterYourself();
    new TGeoCompositeShape("sV0ACha6","sV0ACha126+sV0ANail16Hole:pos16");

    /// Frame
    TGeoVolume *v0AFra6 = new TGeoVolumeAssembly("V0AFra6");
    for (int i=0;i<2;i++) {
    v0APts[0+8*i] = 0.;                              v0APts[1+8*i] = -fV0AR0+fV0AFraWd/2.;
    v0APts[2+8*i] = -fV0AFraWd/2.;                    v0APts[3+8*i] = -fV0AR0+fV0AFraWd/2.;
    v0APts[4+8*i] = -fV0AFraWd/2.;                    v0APts[5+8*i] = -fV0AR4-fV0AFraWd/2.;
    v0APts[6+8*i] = 0.;                              v0APts[7+8*i] = -fV0AR4-fV0AFraWd/2.;
    }
    TGeoArb8 *sV0AFraB16 = new TGeoArb8("sV0AFraB16",fV0ASciWd/2.,v0APts);
    TGeoVolume *v0AFraB16 = new TGeoVolume("V0AFraB16",sV0AFraB16,medV0AFra);
    for (int i=0;i<2;i++) {
    v0APts[0+8*i] = -fV0AR0*cos665;
    v0APts[1+8*i] = -(fV0AR0-fV0AFraWd)*sin665;
    v0APts[2+8*i] = -(fV0AR0-fV0AFraWd/2.)*cos665;
    v0APts[3+8*i] = -(fV0AR0-fV0AFraWd/2.)*sin665;
    v0APts[4+8*i] = -(fV0AR4+fV0AFraWd/2.)*cos665;
    v0APts[5+8*i] = -(fV0AR4+fV0AFraWd/2.)*sin665;
    v0APts[6+8*i] = -(fV0AR4+fV0AFraWd)*cos665;
    v0APts[7+8*i] = -fV0AR4*sin665;
    }
    TGeoArb8 *sV0AFraB26 = new TGeoArb8("sV0AFraB26", fV0ASciWd/2., v0APts);
    TGeoVolume *v0AFraB26 = new TGeoVolume("V0AFraB26",sV0AFraB26,medV0AFra);
    v0AFraB16->SetLineColor(kV0AColorFra); v0AFraB26->SetLineColor(kV0AColorFra);
    v0AFra6->AddNode(v0AFraB16,1);
    v0AFra6->AddNode(v0AFraB26,1);  // Prefer 2 GeoObjects insted of 3 GeoMovements
    new TGeoTubeSeg( "sV0AFraR1b6", fV0AR0-fV0AFraWd/2.,
    	     fV0AR0+fV0AFraWd/2., fV0ASciWd/2., 245.4, 270.0);
    new TGeoTubeSeg( "sV0AFraR2b6", fV0AR1-fV0AFraWd/2.,
    	     fV0AR1+fV0AFraWd/2., fV0ASciWd/2., 245.4, 270.0);
    new TGeoTubeSeg( "sV0AFraR3b6", fV0AR2-fV0AFraWd/2.,
    	     fV0AR2+fV0AFraWd/2., fV0ASciWd/2., 245.4, 270.0);
    new TGeoTubeSeg( "sV0AFraR4b6", fV0AR3-fV0AFraWd/2.,
    	     fV0AR3+fV0AFraWd/2., fV0ASciWd/2., 245.4, 270.0);
    new TGeoTubeSeg( "sV0AFraR5b6", fV0AR4-fV0AFraWd/2.,
    	     fV0AR4+fV0AFraWd/2., fV0ASciWd/2., 245.4, 270.0);
    TGeoCompositeShape *sV0AFraR16 = new TGeoCompositeShape("sV0AFraR16","sV0AFraR1b6-sV0ACha6");
    TGeoCompositeShape *sV0AFraR26 = new TGeoCompositeShape("sV0AFraR26","sV0AFraR2b6-sV0ACha6");
    TGeoCompositeShape *sV0AFraR36 = new TGeoCompositeShape("sV0AFraR36","sV0AFraR3b6-sV0ACha6");
    TGeoCompositeShape *sV0AFraR46 = new TGeoCompositeShape("sV0AFraR46","sV0AFraR4b6-sV0ACha6");
    TGeoCompositeShape *sV0AFraR56 = new TGeoCompositeShape("sV0AFraR56","sV0AFraR5b6-sV0ACha6");
    TGeoVolume *v0AFraR16 = new TGeoVolume("V0AFraR16",sV0AFraR16,medV0AFra);
    TGeoVolume *v0AFraR26 = new TGeoVolume("V0AFraR26",sV0AFraR26,medV0AFra);
    TGeoVolume *v0AFraR36 = new TGeoVolume("V0AFraR36",sV0AFraR36,medV0AFra);
    TGeoVolume *v0AFraR46 = new TGeoVolume("V0AFraR46",sV0AFraR46,medV0AFra);
    TGeoVolume *v0AFraR56 = new TGeoVolume("V0AFraR56",sV0AFraR56,medV0AFra);
    v0AFraR16->SetLineColor(kV0AColorFra); v0AFraR26->SetLineColor(kV0AColorFra);
    v0AFraR36->SetLineColor(kV0AColorFra); v0AFraR46->SetLineColor(kV0AColorFra);
    v0AFraR56->SetLineColor(kV0AColorFra);
    v0AFra6->AddNode(v0AFraR16,1);
    v0AFra6->AddNode(v0AFraR26,1);
    v0AFra6->AddNode(v0AFraR36,1);
    v0AFra6->AddNode(v0AFraR46,1);
    v0AFra6->AddNode(v0AFraR56,1);
    v0ASec6->AddNode(v0AFra6,1);

    /// Sensitive scintilator
    TGeoVolume *v0ASci6 = new TGeoVolumeAssembly("V0ASci6");
    new TGeoTubeSeg( "sV0AR1b6", fV0AR0+fV0AFraWd/2.,
    		     fV0AR1-fV0AFraWd/2., fV0ASciWd/2., 245.4, 270.0);
    new TGeoTubeSeg( "sV0AR2b6", fV0AR1+fV0AFraWd/2.,
    	     fV0AR2-fV0AFraWd/2., fV0ASciWd/2., 245.4, 270.0);
    new TGeoTubeSeg( "sV0AR3b6", fV0AR2+fV0AFraWd/2.,
    	     fV0AR3-fV0AFraWd/2., fV0ASciWd/2., 245.4, 270.0);
    new TGeoTubeSeg( "sV0AR4b6", fV0AR3+fV0AFraWd/2.,
    	     fV0AR4-fV0AFraWd/2., fV0ASciWd/2., 245.4, 270.0);
    TGeoCompositeShape *sV0AR16 = new TGeoCompositeShape("sV0AR16","sV0AR1b6-sV0ACha6");
    TGeoCompositeShape *sV0AR26 = new TGeoCompositeShape("sV0AR26","sV0AR2b6-sV0ACha6");
    TGeoCompositeShape *sV0AR36 = new TGeoCompositeShape("sV0AR36","sV0AR3b6-sV0ACha6");
    TGeoCompositeShape *sV0AR46 = new TGeoCompositeShape("sV0AR46","sV0AR4b6-sV0ACha6");
    TGeoVolume *v0L16 = new TGeoVolume("V0L16",sV0AR16,medV0ASci);
    TGeoVolume *v0L26 = new TGeoVolume("V0L26",sV0AR26,medV0ASci);
    TGeoVolume *v0L36 = new TGeoVolume("V0L36",sV0AR36,medV0ASci);
    TGeoVolume *v0L46 = new TGeoVolume("V0L46",sV0AR46,medV0ASci);
    v0L16->SetLineColor(kV0AColorSci); v0L26->SetLineColor(kV0AColorSci);
    v0L36->SetLineColor(kV0AColorSci); v0L46->SetLineColor(kV0AColorSci);
    v0ASci6->AddNode(v0L16,1);
    v0ASci6->AddNode(v0L26,1);
    v0ASci6->AddNode(v0L36,1);
    v0ASci6->AddNode(v0L46,1);

    /// Non-sensitive scintilator
    new TGeoTubeSeg("sV0AR5S26", fV0AR4+fV0AFraWd/2., fV0AR4 + fV0AR0, fV0ASciWd/2.+2*preShape, 245.4, 270.0);
    TGeoCompositeShape *sV0AR56 = new TGeoCompositeShape("V0AR56","sV0AR5S26 - sV0ACha6");
    TGeoVolume *v0AR56 = new TGeoVolume("V0AR56",sV0AR56,medV0ASci);
    v0AR56->SetLineColor(kV0AColorSci);
    v0ASci6->AddNode(v0AR56,1);
    v0ASec6->AddNode(v0ASci6,1);

    /// Segment of innermost octagon
    TGeoVolume *v0ASup6 = new TGeoVolumeAssembly("V0ASup6");

    /// Segment of outtermost octagon   
    for (int i=0;i<2;i++) {
    v0APts[0+8*i] = 0.;                 	 v0APts[1+8*i] = -(fV0AR7-fV0AOctH2)*sin654;
    v0APts[2+8*i] = 0.;                          v0APts[3+8*i] = -fV0AR7*sin654;
    v0APts[4+8*i] = -fV0AR7*cos654;     	 v0APts[5+8*i] = -fV0AR7*sin654;
    v0APts[6+8*i] = -(fV0AR7-fV0AOctH2)*cos654;	 v0APts[7+8*i] = -(fV0AR7-fV0AOctH2)*sin654;
    }
    TGeoArb8 *sV0AOct26 = new TGeoArb8("sV0AOct26", (fV0ASciWd+2*fV0AOctWd)/2., v0APts);
    TGeoVolume *v0AOct26 = new TGeoVolume("V0AOct26", sV0AOct26,medV0ASup);
    v0AOct26->SetLineColor(kV0AColorOct);
    v0ASup6->AddNode(v0AOct26,1);
    v0ASec6->AddNode(v0ASup6,1);


    /// Plates
    new TGeoTube("sV0ANail1PlaInHole6", 0.0, 0.4, (fV0APlaWd-2*fV0APlaAl)/2.);    
    new TGeoTube("sV0ANail1PlaOuHole6", 0.0, 0.4, (fV0APlaAl)/2.);
    for (int i=0;i<2;i++) {
    v0APts[0+8*i] = 0.;       		v0APts[1+8*i] = -fV0AR0;
    v0APts[2+8*i] = 0.;        		v0APts[3+8*i] = -fV0AR7*sin654;
    v0APts[4+8*i] = -fV0AR7*cos654;    	v0APts[5+8*i] = -fV0AR7*sin654;
    v0APts[6+8*i] = -fV0AR0*cos654;  		v0APts[7+8*i] = -fV0AR0*sin654;
    }
    new TGeoArb8("sV0APlaIn6", (fV0APlaWd-2*fV0APlaAl)/2., v0APts);
    TGeoCompositeShape *sV0APlaInNailsHoles6 = new TGeoCompositeShape("sV0APlaInNailsHoles6","sV0APlaIn6-sV0ANail1PlaInHole6:pos16");
    TGeoVolume *v0APlaInNailsHoles6 = new TGeoVolume("V0APlaInNailsHoles6", sV0APlaInNailsHoles6, medV0APlaIn);
    new TGeoArb8("sV0APlaOu6", fV0APlaAl/2., v0APts);
    TGeoCompositeShape *sV0APlaOuNailsHoles6 = new TGeoCompositeShape("sV0APlaOuNailsHoles6","sV0APlaOu6-sV0ANail1PlaOuHole6:pos16"); 
    TGeoVolume *v0APlaOuNailsHoles6 = new TGeoVolume("V0APlaOuNailsHoles6", sV0APlaOuNailsHoles6, medV0APlaOu);
    v0APlaInNailsHoles6->SetLineColor(kV0AColorPlaIn); v0APlaOuNailsHoles6->SetLineColor(kV0AColorPlaOu);
    TGeoVolume *v0APla6 = new TGeoVolumeAssembly("V0APla6");
    v0APla6->AddNode(v0APlaInNailsHoles6,1);
    v0APla6->AddNode(v0APlaOuNailsHoles6,1,new TGeoTranslation(0,0,(fV0APlaWd-fV0APlaAl)/2.));
    v0APla6->AddNode(v0APlaOuNailsHoles6,2,new TGeoTranslation(0,0,-(fV0APlaWd-fV0APlaAl)/2.));
    v0ASec6->AddNode(v0APla6,1,new TGeoTranslation(0,0,(fV0ASciWd+2*fV0AOctWd+fV0APlaWd)/2.));
    v0ASec6->AddNode(v0APla6,2,new TGeoTranslation(0,0,-(fV0ASciWd+2*fV0AOctWd+fV0APlaWd)/2.));
    
    /// Support
    TGeoBBox *sV0ASuppbl = new TGeoBBox("sV0ASuppbl", 2.0, 18.137, 2.0);       
    TGeoVolume *v0ASuppbl = new TGeoVolume("V0ASuppbl", sV0ASuppbl, medV0ASup);
    v0ASuppbl->SetLineColor(kV0AColorOct);
    v0ASec6->AddNode(v0ASuppbl,1,new TGeoTranslation(-2.0,-63.635,0.0));


    // Replicate sectors
    //for(int i=0; i<1; i++) {
    //TGeoRotation *rot = new TGeoRotation("rot", 90., i*45., 90., 90.+i*45., 0., 0.);
    v0LE->AddNode(v0ASec6, 1);   //i +1, rot);
    //}


    //Definition of sector 7
   
    TGeoVolume *v0ASec7 = new TGeoVolumeAssembly("V0ASec7");

    /// For boolean sustraction
    double preShape7 = 0.2;
    for (int i=0;i<2;i++) {
    v0APts[0+8*i] = 0.0;                      v0APts[1+8*i] = -fV0AR0+fV0AFraWd/2.-preShape7;
    v0APts[2+8*i] = fV0AFraWd/2.;                    v0APts[3+8*i] = -fV0AR0+fV0AFraWd/2.-preShape7;
    v0APts[4+8*i] = fV0AFraWd/2.;                    v0APts[5+8*i] = -fV0AR4-fV0AFraWd/2.+preShape7;
    v0APts[6+8*i] = 0.0;                      v0APts[7+8*i] = -fV0AR4-fV0AFraWd/2.+preShape7;
    }
    new TGeoArb8("sV0ACha17",fV0ASciWd/1.5,v0APts);
    for (int i=0;i<2;i++) {
    v0APts[0+8*i] = fV0AR0*cos654-preShape7;
    v0APts[1+8*i] = -(fV0AR0-fV0AFraWd)*sin654-preShape7;
    v0APts[2+8*i] = (fV0AR0-fV0AFraWd/2.)*cos654-preShape7;
    v0APts[3+8*i] = -(fV0AR0-fV0AFraWd/2.)*sin654;
    v0APts[4+8*i] = (fV0AR4+fV0AFraWd/2.)*cos654+preShape7;
    v0APts[5+8*i] = -(fV0AR4+fV0AFraWd/2.)*sin654+2.*preShape7;
    v0APts[6+8*i] = (fV0AR4+fV0AFraWd)*cos654+preShape7;
    v0APts[7+8*i] = -fV0AR4*sin654+preShape7;
    }
    new TGeoArb8("sV0ACha27", fV0ASciWd/2.+2.*preShape, v0APts);
    new TGeoCompositeShape("sV0ACha127","sV0ACha17+sV0ACha27");
    new TGeoTube("sV0ANail17Hole", 0.0, 0.4, 1.65);
    TGeoTranslation *pos17 = new TGeoTranslation("pos17",0.51,-42.9,0.0);
    pos17->RegisterYourself();
    new TGeoCompositeShape("sV0ACha7","sV0ACha127+sV0ANail17Hole:pos17");

    /// Frame
    TGeoVolume *v0AFra7 = new TGeoVolumeAssembly("V0AFra7");
    for (int i=0;i<2;i++) {
    v0APts[0+8*i] = 0.;                              v0APts[1+8*i] = -fV0AR0-fV0AFraWd/2.;
    v0APts[2+8*i] = fV0AFraWd/2.;                    v0APts[3+8*i] = -fV0AR0-fV0AFraWd/2.;
    v0APts[4+8*i] = fV0AFraWd/2.;                    v0APts[5+8*i] = -fV0AR4+fV0AFraWd/2.;
    v0APts[6+8*i] = 0.;                              v0APts[7+8*i] = -fV0AR4+fV0AFraWd/2.;
    }
    TGeoArb8 *sV0AFraB17 = new TGeoArb8("sV0AFraB17",fV0ASciWd/2.,v0APts);
    TGeoVolume *v0AFraB17 = new TGeoVolume("V0AFraB17",sV0AFraB17,medV0AFra);
    for (int i=0;i<2;i++) {
    v0APts[0+8*i] = fV0AR0*cos654;
    v0APts[1+8*i] = -(fV0AR0-fV0AFraWd)*sin654;
    v0APts[2+8*i] = (fV0AR0-fV0AFraWd/2.)*cos654;
    v0APts[3+8*i] = -(fV0AR0-fV0AFraWd/2.)*sin654;
    v0APts[4+8*i] = (fV0AR4+fV0AFraWd/2.)*cos654;
    v0APts[5+8*i] = -(fV0AR4+fV0AFraWd/2.)*sin654;
    v0APts[6+8*i] = (fV0AR4+fV0AFraWd)*cos654;
    v0APts[7+8*i] = -fV0AR4*sin654;
    }
    TGeoArb8 *sV0AFraB27 = new TGeoArb8("sV0AFraB27", fV0ASciWd/2., v0APts);
    TGeoVolume *v0AFraB27 = new TGeoVolume("V0AFraB27",sV0AFraB27,medV0AFra);
    v0AFraB17->SetLineColor(kV0AColorFra); v0AFraB27->SetLineColor(kV0AColorFra);
    v0AFra7->AddNode(v0AFraB17,1);
    v0AFra7->AddNode(v0AFraB27,1);  // Prefer 2 GeoObjects insted of 3 GeoMovements
    new TGeoTubeSeg( "sV0AFraR1b7", fV0AR0-fV0AFraWd/2.,
    	     fV0AR0+fV0AFraWd/2., fV0ASciWd/2., 270.0, 294.52);
    new TGeoTubeSeg( "sV0AFraR2b7", fV0AR1-fV0AFraWd/2.,
    	     fV0AR1+fV0AFraWd/2., fV0ASciWd/2., 270.0, 294.52);
    new TGeoTubeSeg( "sV0AFraR3b7", fV0AR2-fV0AFraWd/2.,
    	     fV0AR2+fV0AFraWd/2., fV0ASciWd/2., 270.0, 294.52);
    new TGeoTubeSeg( "sV0AFraR4b7", fV0AR3-fV0AFraWd/2.,
    	     fV0AR3+fV0AFraWd/2., fV0ASciWd/2., 270.0, 294.52);
    new TGeoTubeSeg( "sV0AFraR5b7", fV0AR4-fV0AFraWd/2.,
    	     fV0AR4+fV0AFraWd/2., fV0ASciWd/2., 270.0, 294.52);
    TGeoCompositeShape *sV0AFraR17 = new TGeoCompositeShape("sV0AFraR17","sV0AFraR1b7-sV0ACha7");
    TGeoCompositeShape *sV0AFraR27 = new TGeoCompositeShape("sV0AFraR27","sV0AFraR2b7-sV0ACha7");
    TGeoCompositeShape *sV0AFraR37 = new TGeoCompositeShape("sV0AFraR37","sV0AFraR3b7-sV0ACha7");
    TGeoCompositeShape *sV0AFraR47 = new TGeoCompositeShape("sV0AFraR47","sV0AFraR4b7-sV0ACha7");
    TGeoCompositeShape *sV0AFraR57 = new TGeoCompositeShape("sV0AFraR57","sV0AFraR5b7-sV0ACha7");
    TGeoVolume *v0AFraR17 = new TGeoVolume("V0AFraR17",sV0AFraR17,medV0AFra);
    TGeoVolume *v0AFraR27 = new TGeoVolume("V0AFraR27",sV0AFraR27,medV0AFra);
    TGeoVolume *v0AFraR37 = new TGeoVolume("V0AFraR37",sV0AFraR37,medV0AFra);
    TGeoVolume *v0AFraR47 = new TGeoVolume("V0AFraR47",sV0AFraR47,medV0AFra);
    TGeoVolume *v0AFraR57 = new TGeoVolume("V0AFraR57",sV0AFraR57,medV0AFra);
    v0AFraR17->SetLineColor(kV0AColorFra); v0AFraR27->SetLineColor(kV0AColorFra);
    v0AFraR37->SetLineColor(kV0AColorFra); v0AFraR47->SetLineColor(kV0AColorFra);
    v0AFraR57->SetLineColor(kV0AColorFra);
    v0AFra7->AddNode(v0AFraR17,1);
    v0AFra7->AddNode(v0AFraR27,1);
    v0AFra7->AddNode(v0AFraR37,1);
    v0AFra7->AddNode(v0AFraR47,1);
    v0AFra7->AddNode(v0AFraR57,1);
    v0ASec7->AddNode(v0AFra7,1);

    /// Sensitive scintilator
    TGeoVolume *v0ASci7 = new TGeoVolumeAssembly("V0ASci7");
    new TGeoTubeSeg( "sV0AR1b7", fV0AR0+fV0AFraWd/2.,
    	     fV0AR1-fV0AFraWd/2., fV0ASciWd/2., 270.0, 294.52);
    new TGeoTubeSeg( "sV0AR2b7", fV0AR1+fV0AFraWd/2.,
    	     fV0AR2-fV0AFraWd/2., fV0ASciWd/2., 270.0, 294.52);
    new TGeoTubeSeg( "sV0AR3b7", fV0AR2+fV0AFraWd/2.,
    	     fV0AR3-fV0AFraWd/2., fV0ASciWd/2., 270.0, 294.52);
    new TGeoTubeSeg( "sV0AR4b7", fV0AR3+fV0AFraWd/2.,
    	     fV0AR4-fV0AFraWd/2., fV0ASciWd/2., 270.0, 294.52);
    TGeoCompositeShape *sV0AR17 = new TGeoCompositeShape("sV0AR17","sV0AR1b7-sV0ACha7");
    TGeoCompositeShape *sV0AR27 = new TGeoCompositeShape("sV0AR27","sV0AR2b7-sV0ACha7");
    TGeoCompositeShape *sV0AR37 = new TGeoCompositeShape("sV0AR37","sV0AR3b7-sV0ACha7");
    TGeoCompositeShape *sV0AR47 = new TGeoCompositeShape("sV0AR47","sV0AR4b7-sV0ACha7");
    TGeoVolume *v0L17 = new TGeoVolume("V0L17",sV0AR17,medV0ASci);
    TGeoVolume *v0L27 = new TGeoVolume("V0L27",sV0AR27,medV0ASci);
    TGeoVolume *v0L37 = new TGeoVolume("V0L37",sV0AR37,medV0ASci);
    TGeoVolume *v0L47 = new TGeoVolume("V0L47",sV0AR47,medV0ASci);
    v0L17->SetLineColor(kV0AColorSci); v0L27->SetLineColor(kV0AColorSci);
    v0L37->SetLineColor(kV0AColorSci); v0L47->SetLineColor(kV0AColorSci);
    v0ASci7->AddNode(v0L17,1);
    v0ASci7->AddNode(v0L27,1);
    v0ASci7->AddNode(v0L37,1);
    v0ASci7->AddNode(v0L47,1);

    /// Non-sensitive scintilator
    new TGeoTubeSeg("sV0AR5S27", fV0AR4+fV0AFraWd/2., fV0AR4 + fV0AR0, fV0ASciWd/2.+2*preShape, 270.0, 294.52);
    TGeoCompositeShape *sV0AR57 = new TGeoCompositeShape("V0AR57","sV0AR5S27 - sV0ACha7");
    TGeoVolume *v0AR57 = new TGeoVolume("V0AR57",sV0AR57,medV0ASci);
    v0AR57->SetLineColor(kV0AColorSci);
    v0ASci7->AddNode(v0AR57,1);
    v0ASec7->AddNode(v0ASci7,1);

    /// Segment of innermost octagon
    TGeoVolume *v0ASup7 = new TGeoVolumeAssembly("V0ASup7");

    /// Segment of outtermost octagon   
    for (int i=0;i<2;i++) {
    v0APts[0+8*i] = 0.;                 	 v0APts[1+8*i] = -(fV0AR7-fV0AOctH2)*sin654;
    v0APts[2+8*i] = 0.;                          v0APts[3+8*i] = -fV0AR7*sin654;
    v0APts[4+8*i] = fV0AR7*cos654;      	 v0APts[5+8*i] = -fV0AR7*sin654;
    v0APts[6+8*i] = (fV0AR7-fV0AOctH2)*cos654;	 v0APts[7+8*i] = -(fV0AR7-fV0AOctH2)*sin654;
    }
    TGeoArb8 *sV0AOct27 = new TGeoArb8("sV0AOct27", (fV0ASciWd+2*fV0AOctWd)/2., v0APts);
    TGeoVolume *v0AOct27 = new TGeoVolume("V0AOct27", sV0AOct27,medV0ASup);
    v0AOct27->SetLineColor(kV0AColorOct);
    v0ASup7->AddNode(v0AOct27,1);
    v0ASec7->AddNode(v0ASup7,1);


    /// Plates
    new TGeoTube("sV0ANail1PlaInHole7", 0.0, 0.4, (fV0APlaWd-2*fV0APlaAl)/2.);    
    new TGeoTube("sV0ANail1PlaOuHole7", 0.0, 0.4, (fV0APlaAl)/2.);    
    for (int i=0;i<2;i++) {
    v0APts[0+8*i] = 0.;       		v0APts[1+8*i] = -fV0AR0;
    v0APts[2+8*i] = 0.;        		v0APts[3+8*i] = -fV0AR7*sin654;
    v0APts[4+8*i] = fV0AR7*cos654;    	v0APts[5+8*i] = -fV0AR7*sin654;
    v0APts[6+8*i] = fV0AR0*cos654;  		v0APts[7+8*i] = -fV0AR0*sin654;
    }
    new TGeoArb8("sV0APlaIn7", (fV0APlaWd-2*fV0APlaAl)/2., v0APts);
    TGeoCompositeShape *sV0APlaInNailsHoles7 = new TGeoCompositeShape("sV0APlaInNailsHoles7","sV0APlaIn7-sV0ANail1PlaInHole7:pos17");
    TGeoVolume *v0APlaInNailsHoles7 = new TGeoVolume("V0APlaInNailsHoles7", sV0APlaInNailsHoles7, medV0APlaIn);
    new TGeoArb8("sV0APlaOu7", fV0APlaAl/2., v0APts);
    TGeoCompositeShape *sV0APlaOuNailsHoles7 = new TGeoCompositeShape("sV0APlaOuNailsHoles7","sV0APlaOu7-sV0ANail1PlaOuHole7:pos17"); 
    TGeoVolume *v0APlaOuNailsHoles7 = new TGeoVolume("V0APlaOuNailsHoles7", sV0APlaOuNailsHoles7, medV0APlaOu);
    v0APlaInNailsHoles7->SetLineColor(kV0AColorPlaIn); v0APlaOuNailsHoles7->SetLineColor(kV0AColorPlaOu);
    TGeoVolume *v0APla7 = new TGeoVolumeAssembly("V0APla7");
    v0APla7->AddNode(v0APlaInNailsHoles7,1);
    v0APla7->AddNode(v0APlaOuNailsHoles7,1,new TGeoTranslation(0,0,(fV0APlaWd-fV0APlaAl)/2.));
    v0APla7->AddNode(v0APlaOuNailsHoles7,2,new TGeoTranslation(0,0,-(fV0APlaWd-fV0APlaAl)/2.));
    v0ASec7->AddNode(v0APla7,1,new TGeoTranslation(0,0,(fV0ASciWd+2*fV0AOctWd+fV0APlaWd)/2.));
    v0ASec7->AddNode(v0APla7,2,new TGeoTranslation(0,0,-(fV0ASciWd+2*fV0AOctWd+fV0APlaWd)/2.));

     /// Support
    TGeoBBox *sV0ASuppbr = new TGeoBBox("sV0ASuppbr", 2.0, 18.138, 2.0);      
    TGeoVolume *v0ASuppbr = new TGeoVolume("V0ASuppbr", sV0ASuppbr, medV0ASup);
    v0ASuppbr->SetLineColor(kV0AColorOct);
    v0ASec7->AddNode(v0ASuppbr,1,new TGeoTranslation(2.0,-63.639,0.0));

    // Replicate sectors
    for(int i=0; i<1; i++) {
    TGeoRotation *rot = new TGeoRotation("rot", 90., i*45., 90., 90.+i*45., 0., 0.);
    v0LE->AddNode(v0ASec7,i + 1,rot);
    }



    
    //Definition of sector 8
   
       TGeoVolume *v0ASec8 = new TGeoVolumeAssembly("V0ASec8");

 /// For boolean sustraction
      for (int i=0;i<2;i++) {
      v0APts[0+8*i] = fV0AR0-fV0AFraWd/2.-preShape;  v0APts[1+8*i] = -preShape;
      v0APts[2+8*i] = fV0AR0-fV0AFraWd/2.-preShape;  v0APts[3+8*i] = fV0AFraWd/2.;
      v0APts[4+8*i] = fV0AR4+fV0AFraWd/2.+preShape;  v0APts[5+8*i] = fV0AFraWd/2.;
      v0APts[6+8*i] = fV0AR4+fV0AFraWd/2.+preShape;  v0APts[7+8*i] = -preShape;
    }
    new TGeoArb8("sV0ACha18",fV0ASciWd/1.5,v0APts);
    for (int i=0;i<2;i++) {
      v0APts[0+8*i] = fV0AR0*cos654-preShape;
      v0APts[1+8*i] = -(fV0AR0-fV0AFraWd)*sin654-preShape;
      v0APts[2+8*i] = (fV0AR0-fV0AFraWd/2.)*cos654-preShape;
      v0APts[3+8*i] = -(fV0AR0-fV0AFraWd/2.)*sin654;
      v0APts[4+8*i] = (fV0AR4+fV0AFraWd/2.)*cos654+preShape;
      v0APts[5+8*i] = -(fV0AR4+fV0AFraWd/2.)*sin654+2.*preShape;
      v0APts[6+8*i] = (fV0AR4+fV0AFraWd)*cos654+preShape;
      v0APts[7+8*i] = -fV0AR4*sin654+preShape;
    }
    new TGeoArb8("sV0ACha28", fV0ASciWd/2.+2.*preShape, v0APts);
    new TGeoCompositeShape("sV0ACha128","sV0ACha18+sV0ACha28");
    new TGeoTube("sV0ANail18Hole", 0.0, 0.4, 1.65);
    TGeoTranslation *pos18 = new TGeoTranslation("pos18",42.9,-.51,0.0);
    pos18->RegisterYourself();
    new TGeoTube("sV0ANail28Hole", 0.0, 0.4, 1.65);
    TGeoTranslation *pos28 = new TGeoTranslation("pos28", 30.8,-30.04,0.0);
    pos28->RegisterYourself();
    new TGeoTube("sV0ANail38Hole", 0.0, 0.4, 1.65);
    TGeoTranslation *pos38 = new TGeoTranslation("pos38",29.8,-31.04,0.0); 
    pos38->RegisterYourself();
    new TGeoCompositeShape("sV0ANailsHoles8","sV0ANail18Hole:pos18+sV0ANail28Hole:pos28+sV0ANail38Hole:pos38");
    new TGeoCompositeShape("sV0ACha8","sV0ACha128+sV0ANailsHoles8");

    /// Frame
    TGeoVolume *v0AFra8 = new TGeoVolumeAssembly("V0AFra8");
    for (int i=0;i<2;i++) {
      v0APts[0+8*i] = fV0AR0-fV0AFraWd/2.;  v0APts[1+8*i] = preShape;
      v0APts[2+8*i] = fV0AR0-fV0AFraWd/2.;  v0APts[3+8*i] = fV0AFraWd/2.;
      v0APts[4+8*i] = fV0AR4+fV0AFraWd/2.;  v0APts[5+8*i] = fV0AFraWd/2.;
      v0APts[6+8*i] = fV0AR4+fV0AFraWd/2.;  v0APts[7+8*i] = preShape;
    }
    TGeoArb8 *sV0AFraB18 = new TGeoArb8("sV0AFraB18",fV0ASciWd/2.,v0APts);
    TGeoVolume *v0AFraB18 = new TGeoVolume("V0AFraB18",sV0AFraB18,medV0AFra);
    for (int i=0;i<2;i++) {
      v0APts[0+8*i] = fV0AR0*cos654;
      v0APts[1+8*i] = -(fV0AR0-fV0AFraWd)*sin654;
      v0APts[2+8*i] = (fV0AR0-fV0AFraWd/2.)*cos654;
      v0APts[3+8*i] = -(fV0AR0-fV0AFraWd/2.)*sin654;
      v0APts[4+8*i] = (fV0AR4+fV0AFraWd/2.)*cos654;
      v0APts[5+8*i] = -(fV0AR4+fV0AFraWd/2.)*sin654;
      v0APts[6+8*i] = (fV0AR4+fV0AFraWd)*cos654;
      v0APts[7+8*i] = -fV0AR4*sin654;
    }
    TGeoArb8 *sV0AFraB28 = new TGeoArb8("sV0AFraB28", fV0ASciWd/2., v0APts);
    TGeoVolume *v0AFraB28 = new TGeoVolume("V0AFraB28",sV0AFraB28,medV0AFra);
    v0AFraB18->SetLineColor(kV0AColorFra); v0AFraB28->SetLineColor(kV0AColorFra);
    v0AFra8->AddNode(v0AFraB18,1);
    v0AFra8->AddNode(v0AFraB28,1);  // Prefer 2 GeoObjects insted of 3 GeoMovements
    new TGeoTubeSeg( "sV0AFraR1b8", fV0AR0-fV0AFraWd/2.,
		     fV0AR0+fV0AFraWd/2., fV0ASciWd/2., 294.52, 359.0);
    new TGeoTubeSeg( "sV0AFraR2b8", fV0AR1-fV0AFraWd/2.,
		     fV0AR1+fV0AFraWd/2., fV0ASciWd/2., 294.52, 359.0);
    new TGeoTubeSeg( "sV0AFraR3b8", fV0AR2-fV0AFraWd/2.,
		     fV0AR2+fV0AFraWd/2., fV0ASciWd/2., 294.52, 359.0);
    new TGeoTubeSeg( "sV0AFraR4b8", fV0AR3-fV0AFraWd/2.,
		     fV0AR3+fV0AFraWd/2., fV0ASciWd/2., 294.52, 359.0);
    new TGeoTubeSeg( "sV0AFraR5b8", fV0AR4-fV0AFraWd/2.,
		     fV0AR4+fV0AFraWd/2., fV0ASciWd/2., 294.52, 359.0);
    TGeoCompositeShape *sV0AFraR18 = new TGeoCompositeShape("sV0AFraR18","sV0AFraR1b8-sV0ACha8");
    TGeoCompositeShape *sV0AFraR28 = new TGeoCompositeShape("sV0AFraR28","sV0AFraR2b8-sV0ACha8");
    TGeoCompositeShape *sV0AFraR38 = new TGeoCompositeShape("sV0AFraR38","sV0AFraR3b8-sV0ACha8");
    TGeoCompositeShape *sV0AFraR48 = new TGeoCompositeShape("sV0AFraR48","sV0AFraR4b8-sV0ACha8");
    TGeoCompositeShape *sV0AFraR58 = new TGeoCompositeShape("sV0AFraR58","sV0AFraR5b8-sV0ACha8");
    TGeoVolume *v0AFraR18 = new TGeoVolume("V0AFraR18",sV0AFraR18,medV0AFra);
    TGeoVolume *v0AFraR28 = new TGeoVolume("V0AFraR28",sV0AFraR28,medV0AFra);
    TGeoVolume *v0AFraR38 = new TGeoVolume("V0AFraR38",sV0AFraR38,medV0AFra);
    TGeoVolume *v0AFraR48 = new TGeoVolume("V0AFraR48",sV0AFraR48,medV0AFra);
    TGeoVolume *v0AFraR58 = new TGeoVolume("V0AFraR58",sV0AFraR58,medV0AFra);
    v0AFraR18->SetLineColor(kV0AColorFra); v0AFraR28->SetLineColor(kV0AColorFra);
    v0AFraR38->SetLineColor(kV0AColorFra); v0AFraR48->SetLineColor(kV0AColorFra);
    v0AFraR58->SetLineColor(kV0AColorFra);
    v0AFra8->AddNode(v0AFraR18,1);
    v0AFra8->AddNode(v0AFraR28,1);
    v0AFra8->AddNode(v0AFraR38,1);
    v0AFra8->AddNode(v0AFraR48,1);
    v0AFra8->AddNode(v0AFraR58,1);
    v0ASec8->AddNode(v0AFra8,1);

    /// Sensitive scintilator
    TGeoVolume *v0ASci8 = new TGeoVolumeAssembly("V0ASci8");
    new TGeoTubeSeg( "sV0AR1b8", fV0AR0+fV0AFraWd/2.,
		     fV0AR1-fV0AFraWd/2., fV0ASciWd/2., 294.52, 359.0);
    new TGeoTubeSeg( "sV0AR2b8", fV0AR1+fV0AFraWd/2.,
		     fV0AR2-fV0AFraWd/2., fV0ASciWd/2., 294.52, 359.0);
    new TGeoTubeSeg( "sV0AR3b8", fV0AR2+fV0AFraWd/2.,
		     fV0AR3-fV0AFraWd/2., fV0ASciWd/2., 294.52, 359.0);
    new TGeoTubeSeg( "sV0AR4b8", fV0AR3+fV0AFraWd/2.,
		     fV0AR4-fV0AFraWd/2., fV0ASciWd/2., 294.52, 359.0);
    TGeoCompositeShape *sV0AR18 = new TGeoCompositeShape("sV0AR18","sV0AR1b8-sV0ACha8");
    TGeoCompositeShape *sV0AR28 = new TGeoCompositeShape("sV0AR28","sV0AR2b8-sV0ACha8");
    TGeoCompositeShape *sV0AR38 = new TGeoCompositeShape("sV0AR38","sV0AR3b8-sV0ACha8");
    TGeoCompositeShape *sV0AR48 = new TGeoCompositeShape("sV0AR48","sV0AR4b8-sV0ACha8");
    TGeoVolume *v0L18 = new TGeoVolume("V0L18",sV0AR18,medV0ASci);
    TGeoVolume *v0L28 = new TGeoVolume("V0L28",sV0AR28,medV0ASci);
    TGeoVolume *v0L38 = new TGeoVolume("V0L38",sV0AR38,medV0ASci);
    TGeoVolume *v0L48 = new TGeoVolume("V0L48",sV0AR48,medV0ASci);
    v0L18->SetLineColor(kV0AColorSci); v0L28->SetLineColor(kV0AColorSci);
    v0L38->SetLineColor(kV0AColorSci); v0L48->SetLineColor(kV0AColorSci);
    v0ASci8->AddNode(v0L18,1);
    v0ASci8->AddNode(v0L28,1);
    v0ASci8->AddNode(v0L38,1);
    v0ASci8->AddNode(v0L48,1);

    /// Non-sensitive scintilator
    new TGeoTubeSeg("sV0AR5S28", fV0AR4+fV0AFraWd/2., fV0AR4 + fV0AR0, fV0ASciWd/2.+2*preShape, 294.52, 359.0);
    TGeoCompositeShape *sV0AR58 = new TGeoCompositeShape("V0AR58","sV0AR5S28 - sV0ACha8");
    TGeoVolume *v0AR58 = new TGeoVolume("V0AR58",sV0AR58,medV0ASci);
    v0AR58->SetLineColor(kV0AColorSci);
    v0ASci8->AddNode(v0AR58,1);
    v0ASec8->AddNode(v0ASci8,1);

    /// Segment of innermost octagon
    TGeoVolume *v0ASup8 = new TGeoVolumeAssembly("V0ASup8");

/// Segment of outtermost octagon   
     for (int i=0;i<2;i++) {
    v0APts[0+8*i] = fV0AR6-fV0AOctH2;	         v0APts[1+8*i] = 0.;
    v0APts[2+8*i] = (fV0AR7-fV0AOctH2)*cos654;   v0APts[3+8*i] = -(fV0AR7-fV0AOctH2)*sin654;
    v0APts[4+8*i] = fV0AR7*cos654;	         v0APts[5+8*i] = -fV0AR7*sin654;
    v0APts[6+8*i] = fV0AR6;			 v0APts[7+8*i] = 0.;
    }
    TGeoArb8 *sV0AOct28 = new TGeoArb8("sV0AOct28", (fV0ASciWd+2*fV0AOctWd)/2., v0APts);
    TGeoVolume *v0AOct28 = new TGeoVolume("V0AOct28", sV0AOct28,medV0ASup);
    v0AOct28->SetLineColor(kV0AColorOct);
    v0ASup8->AddNode(v0AOct28,1);
    v0ASec8->AddNode(v0ASup8,1);

    //Bunch of fibers
    v0APts[ 0] = v0APts[ 2] = -14.0;
    v0APts[ 1] = v0APts[ 7] = (fV0ASciWd+fV0AOctWd)/2.-0.01;
    v0APts[ 3] = v0APts[ 5] = (fV0ASciWd+fV0AOctWd)/2.+0.01;
    v0APts[ 4] = v0APts[ 6] = +14.0;
    v0APts[ 8] = v0APts[10] = -10.0;
    v0APts[ 9] = v0APts[15] = 0.;
    v0APts[11] = v0APts[13] = 0.25;
    v0APts[12] = v0APts[14] = +10.0;
    TGeoArb8 *sV0AFiba8 = new TGeoArb8("sV0AFiba8", 9.0, v0APts);
    TGeoVolume *v0AFiba18 = new TGeoVolume("V0AFiba18",sV0AFiba8,medV0AFib);
    TGeoVolume *v0AFiba8 = new TGeoVolumeAssembly("V0AFiba8");
    rot = new TGeoRotation("rot");
    rot->RotateX(-90);
    rot->RotateZ(-90-22.5);
    v0AFiba8->AddNode(v0AFiba18,1,rot);
    rot = new TGeoRotation("rot");
    rot->RotateX(-90);
    rot->RotateY(180);
    rot->RotateZ(-90-22.5);
    v0AFiba8->SetLineColor(kV0AColorFib);
    v0AFiba8->AddNode(v0AFiba18,2,rot);
    v0ASec8->AddNode(v0AFiba8,1,new TGeoTranslation((fV0AR6-fV0AOctH2+fV0AR5)*cos225/2. - 1.5, -(fV0AR6-fV0AOctH2+fV0AR5)*sin225/2. - 1.0, 0));
    for (int i=0;i<2;i++) {
    v0APts[0+8*i] = 33.47;	      v0APts[1+8*i] = -35.13;
    v0APts[2+8*i] = 50.48;            v0APts[3+8*i] = -46.97;
    v0APts[4+8*i] = 44.66;            v0APts[5+8*i] = -61.92;
    v0APts[6+8*i] = 7.9;	      v0APts[7+8*i] = -47.18;
    }
    TGeoArb8 *sV0AFibb8 = new TGeoArb8("sV0AFibb8", 1.25, v0APts);
    TGeoVolume *v0AFibb18 = new TGeoVolume("V0AFibb18",sV0AFibb8,medV0AFib);
    TGeoVolume *v0AFibb8 = new TGeoVolumeAssembly("V0AFibb8");
    //rot = new TGeoRotation("rot");
    //rot->RotateX(-90);
    //rot->RotateZ(-90-40);
    v0AFibb8->AddNode(v0AFibb18,1);//,rot);
    //rot = new TGeoRotation("rot");
    //rot->RotateX(-90);
    //rot->RotateY(180);
    //rot->RotateZ(-90-40);
    v0AFibb8->SetLineColor(kV0AColorFib);
    //v0AFibb8->AddNode(v0AFibb18,2,rot);
    v0ASec8->AddNode(v0AFibb8,1);//,new TGeoTranslation((fV0AR6-fV0AOctH2+fV0AR5)*cos225/2. - 17.0, -(fV0AR6-fV0AOctH2+fV0AR5)*sin225/2. - 30.0, 0));
    

    /// Plates
    new TGeoTube("sV0ANail1PlaInHole8", 0.0, 0.4, (fV0APlaWd-2*fV0APlaAl)/2.);
    new TGeoTube("sV0ANail2PlaInHole8", 0.0, 0.4, (fV0APlaWd-2*fV0APlaAl)/2.);
    new TGeoTube("sV0ANail3PlaInHole8", 0.0, 0.4, (fV0APlaWd-2*fV0APlaAl)/2.);
    new TGeoCompositeShape("sV0ANailsPlaInHoles8","sV0ANail1PlaInHole8:pos18+sV0ANail2PlaInHole8:pos28+sV0ANail3PlaInHole8:pos38");
    new TGeoTube("sV0ANail1PlaOuHole8", 0.0, 0.4, (fV0APlaAl)/2.);
    new TGeoTube("sV0ANail2PlaOuHole8", 0.0, 0.4, (fV0APlaAl)/2.);
    new TGeoTube("sV0ANail3PlaOuHole8", 0.0, 0.4, (fV0APlaAl)/2.);
    new TGeoCompositeShape("sV0ANailsPlaOuHoles8","sV0ANail1PlaOuHole8:pos18+sV0ANail2PlaOuHole8:pos28+sV0ANail3PlaInHole8:pos38");
    for (int i=0;i<2;i++) {
      v0APts[0+8*i] = fV0AR0;			v0APts[1+8*i] = 0.;
      v0APts[2+8*i] = fV0AR0*cos654;		v0APts[3+8*i] = -fV0AR0*sin654;
      v0APts[4+8*i] = fV0AR7*cos654;    	v0APts[5+8*i] = -fV0AR7*sin654;
      v0APts[6+8*i] = fV0AR6;    		v0APts[7+8*i] = 0.;
    }
    new TGeoArb8("sV0APlaIn8", (fV0APlaWd-2*fV0APlaAl)/2., v0APts);
    TGeoCompositeShape *sV0APlaInNailsHoles8 = new TGeoCompositeShape("sV0APlaInNailsHoles8","sV0APlaIn8-sV0ANailsPlaInHoles8");
    TGeoVolume *v0APlaInNailsHoles8 = new TGeoVolume("V0APlaInNailsHoles8", sV0APlaInNailsHoles8, medV0APlaIn);
    new TGeoArb8("sV0APlaOu8", fV0APlaAl/2., v0APts);
    TGeoCompositeShape *sV0APlaOuNailsHoles8 = new TGeoCompositeShape("sV0APlaOuNailsHoles8","sV0APlaOu8-sV0ANailsPlaOuHoles8"); 
    TGeoVolume *v0APlaOuNailsHoles8 = new TGeoVolume("V0APlaOuNailsHoles8", sV0APlaOuNailsHoles8, medV0APlaOu);
    v0APlaInNailsHoles8->SetLineColor(kV0AColorPlaIn); v0APlaOuNailsHoles8->SetLineColor(kV0AColorPlaOu);
    TGeoVolume *v0APla8 = new TGeoVolumeAssembly("V0APla8");
    v0APla8->AddNode(v0APlaInNailsHoles8,1);
    v0APla8->AddNode(v0APlaOuNailsHoles8,1,new TGeoTranslation(0,0,(fV0APlaWd-fV0APlaAl)/2.));
    v0APla8->AddNode(v0APlaOuNailsHoles8,2,new TGeoTranslation(0,0,-(fV0APlaWd-fV0APlaAl)/2.));
    v0ASec8->AddNode(v0APla8,1,new TGeoTranslation(0,0,(fV0ASciWd+2*fV0AOctWd+fV0APlaWd)/2.));
    v0ASec8->AddNode(v0APla8,2,new TGeoTranslation(0,0,-(fV0ASciWd+2*fV0AOctWd+fV0APlaWd)/2.));

    /// PMBox 1ero
    TGeoVolume* v0APM8 = new TGeoVolumeAssembly("V0APM1");
    new TGeoBBox("sV0APMB18", fV0APMBWd/2., fV0APMBHt/2., fV0APMBTh/2.);
    new TGeoBBox("sV0APMB28", fV0APMBWd/2.-fV0APMBWdW, fV0APMBHt/2.-fV0APMBHtW, fV0APMBTh/2.-fV0APMBThW);
    TGeoCompositeShape *sV0APMB8 = new TGeoCompositeShape("sV0APMB8","sV0APMB18-sV0APMB28");
    TGeoVolume *v0APMB8 = new TGeoVolume("V0APMB8",sV0APMB8, medV0APMAlum);
    v0APMB8->SetLineColor(kV0AColorPMA);
    v0APM8->AddNode(v0APMB8,1);

    /// PMTubes 1ero
    TGeoTube *sV0APMT18 = new TGeoTube("sV0APMT18", fV0APMTR1, fV0APMTR2, fV0APMTH/2.);
    TGeoVolume *v0APMT18 = new TGeoVolume("V0APMT18", sV0APMT18, medV0APMGlass);
    TGeoTube *sV0APMT28 = new TGeoTube("sV0APMT28", fV0APMTR3, fV0APMTR4, fV0APMTH/2.);
    TGeoVolume *v0APMT28 = new TGeoVolume("V0APMT28", sV0APMT28, medV0APMAlum);
    TGeoVolume *v0APMT8 = new TGeoVolumeAssembly("V0APMT8");
    TGeoTube *sV0APMTT8 = new TGeoTube("sV0APMTT8", 0., fV0APMTR4, fV0APMTB/2.);
    TGeoVolume *v0APMTT8 = new TGeoVolume("V0APMTT8", sV0APMTT8, medV0APMAlum);
    v0APMT8->SetLineColor(kV0AColorPMG);
    v0APMT28->SetLineColor(kV0AColorPMA);
    v0APMTT8->SetLineColor(kV0AColorPMA);
    rot = new TGeoRotation("rot", 90, 0, 180, 0, 90, 90);
    v0APMT8->AddNode(v0APMT18,1,rot);
    v0APMT8->AddNode(v0APMT28,1,rot);
    v0APMT8->AddNode(v0APMTT8,1,new TGeoCombiTrans(0,(fV0APMTH+fV0APMTB)/2.,0,rot));
    double autoShift8 = (fV0APMBWd-2*fV0APMBWdW)/4.;
    v0APM8->AddNode(v0APMT8, 1, new TGeoTranslation(-1.5*autoShift8, 0, 0));
    v0APM8->AddNode(v0APMT8, 2, new TGeoTranslation(-0.5*autoShift8, 0, 0));
    v0APM8->AddNode(v0APMT8, 3, new TGeoTranslation(+0.5*autoShift8, 0, 0));
    v0APM8->AddNode(v0APMT8, 4, new TGeoTranslation(+1.5*autoShift8, 0, 0));

    /// PM 1ero
    rot = new TGeoRotation("rot");
    rot->RotateX(-90+30);
    rot->RotateY(0);
    rot->RotateZ(65+3);
    double shiftZ8 = fV0APMBHt/2. * cosAngPMB
      -   ( fV0ASciWd + 2 * fV0AOctWd + 2 * fV0APlaWd )/2.   -   fV0APMBTh/2. * sinAngPMB;
    double shiftR8 = fV0AR6  +  fV0AOctH2 + fV0APlaAl;
    v0ASec8->AddNode(v0APM8,1, new TGeoCombiTrans( shiftR8*cos225, -shiftR8*sin225, shiftZ8, rot));

    /// PMBox 2do 
    TGeoVolume* v0APM82 = new TGeoVolumeAssembly("V0APM82");
    new TGeoBBox("sV0APMB82", fV0APMBWd/2., fV0APMBHt/2., fV0APMBTh/2.);
    new TGeoBBox("sV0APMB82", fV0APMBWd/2.-fV0APMBWdW, fV0APMBHt/2.-fV0APMBHtW, fV0APMBTh/2.-fV0APMBThW);
    TGeoCompositeShape *sV0APMB82 = new TGeoCompositeShape("sV0APMB82","sV0APMB82-sV0APMB82");
    TGeoVolume *v0APMB82 = new TGeoVolume("V0APMB82",sV0APMB82, medV0APMAlum);
    v0APMB82->SetLineColor(kV0AColorPMA);
    v0APM82->AddNode(v0APMB82,1);

    /// PMTubes 2ndo
    TGeoTube *sV0APMT182 = new TGeoTube("sV0APMT182", fV0APMTR1, fV0APMTR2, fV0APMTH/2.);
    TGeoVolume *v0APMT182 = new TGeoVolume("V0APMT182", sV0APMT182, medV0APMGlass);
    TGeoTube *sV0APMT282 = new TGeoTube("sV0APMT282", fV0APMTR3, fV0APMTR4, fV0APMTH/2.);
    TGeoVolume *v0APMT282 = new TGeoVolume("V0APMT282", sV0APMT282, medV0APMAlum);
    TGeoVolume *v0APMT82 = new TGeoVolumeAssembly("V0APMT82"); // pk si no choca con la 752 o con la 794
    TGeoTube *sV0APMTT82 = new TGeoTube("sV0APMTT82", 0., fV0APMTR4, fV0APMTB/2.);
    TGeoVolume *v0APMTT82 = new TGeoVolume("V0APMT82", sV0APMTT82, medV0APMAlum);
    v0APMT82->SetLineColor(kV0AColorPMG);
    v0APMT282->SetLineColor(kV0AColorPMA);
    v0APMTT82->SetLineColor(kV0AColorPMA);
    rot = new TGeoRotation("rot", 90, 0, 180, 0, 90, 90);
    v0APMT82->AddNode(v0APMT182,1,rot);
    v0APMT82->AddNode(v0APMT282,1,rot);
    v0APMT82->AddNode(v0APMTT82,1,new TGeoCombiTrans(0,(fV0APMTH+fV0APMTB)/2.,0,rot));
    v0APM82->AddNode(v0APMT82, 1, new TGeoTranslation(-1.5*autoShift, 0, 0));
    v0APM82->AddNode(v0APMT82, 2, new TGeoTranslation(-0.5*autoShift, 0, 0));
    v0APM82->AddNode(v0APMT82, 3, new TGeoTranslation(+0.5*autoShift, 0, 0));
    v0APM82->AddNode(v0APMT82, 4, new TGeoTranslation(+1.5*autoShift, 0, 0));

    /// PM 2ndo
    rot = new TGeoRotation("rot");
    rot->RotateX(-90+30);
    rot->RotateY(0);
    rot->RotateZ(65+3);
    double shiftZ82 = fV0APMBHt/2. * cosAngPMB
      -   ( fV0ASciWd + 2 * fV0AOctWd + 2 * fV0APlaWd )/2.   -   fV0APMBTh/2. * sinAngPMB;
    double shiftR82 = fV0AR6  + fV0AR1 + fV0AOctWd + fV0APlaAl/3.;
    v0ASec8->AddNode(v0APM82,1, new TGeoCombiTrans( shiftR82*cos45, -shiftR82*sin45, shiftZ82, rot)); 
   
    // Replicate sectors
    for(int i=0; i<1; i++) {
    TGeoRotation *rot = new TGeoRotation("rot", 90., i*45., 90., 90.+i*45., 0., 0.);    
    v0LE->AddNode(v0ASec8,i + 1,rot);
    }
    
    
    ///Aluminium nails for sectors 5, 6, 7 and 8
    TGeoTube *sV0ANail51 = new TGeoTube("sV0ANail51", 0.0, 0.4, 5.09/2.);
    TGeoVolume *v0ANail51 = new TGeoVolume("V0ANail51", sV0ANail51, medV0APMAlum);
    v0ANail51->SetLineColor(kV0AColorPMA);// this is the color for aluminium
    v0LE->AddNode(v0ANail51,1,new TGeoTranslation(-42.9,-0.51,0.0));
    TGeoTube *sV0ANail52 = new TGeoTube("sV0ANail52", 0.0, 0.4, 5.09/2.);
    TGeoVolume *v0ANail52 = new TGeoVolume("V0ANail52", sV0ANail52, medV0APMAlum);
    v0ANail52->SetLineColor(kV0AColorPMA);
    v0LE->AddNode(v0ANail52,1,new TGeoTranslation(-30.8,-30.04,0.0)); 
    TGeoTube *sV0ANail61 = new TGeoTube("sV0ANail61", 0.0, 0.4, 5.09/2.);
    TGeoVolume *v0ANail61 = new TGeoVolume("V0ANail61", sV0ANail61, medV0APMAlum);
    v0ANail61->SetLineColor(kV0AColorPMA);// this is the color for aluminium
    v0LE->AddNode(v0ANail61,1,new TGeoTranslation(-0.51,-42.9,0.0));  
    TGeoTube *sV0ANail53 = new TGeoTube("sV0ANail53", 0.0, 0.4, 5.09/2.);
    TGeoVolume *v0ANail53 = new TGeoVolume("V0ANail53", sV0ANail53, medV0APMAlum);
    v0ANail53->SetLineColor(kV0AColorPMA);
    v0LE->AddNode(v0ANail53,1,new TGeoTranslation(-30.05,-30.79,0.0)); //     -29.8,-31.04,0.0
    TGeoTube *sV0ANail71 = new TGeoTube("sV0ANail71", 0.0, 0.4, 5.09/2.);
    TGeoVolume *v0ANail71 = new TGeoVolume("V0ANail71", sV0ANail71, medV0APMAlum);
    v0ANail71->SetLineColor(kV0AColorPMA);// this is the color for aluminium
    v0LE->AddNode(v0ANail71,1,new TGeoTranslation(0.51,-42.9,0.0));
    TGeoTube *sV0ANail83 = new TGeoTube("sV0ANail83", 0.0, 0.4, 5.09/2.);
    TGeoVolume *v0ANail83 = new TGeoVolume("V0ANail83", sV0ANail83, medV0APMAlum);
    v0ANail83->SetLineColor(kV0AColorPMA);
    v0LE->AddNode(v0ANail83,1,new TGeoTranslation(29.8,-31.04,0.0)); 
    TGeoTube *sV0ANail81 = new TGeoTube("sV0ANail81", 0.0, 0.4, 5.09/2.);
    TGeoVolume *v0ANail81 = new TGeoVolume("V0ANail81", sV0ANail81, medV0APMAlum);
    v0ANail81->SetLineColor(kV0AColorPMA);// this is the color for aluminium
    v0LE->AddNode(v0ANail81,1,new TGeoTranslation(42.9,-.51,0.0));
    TGeoTube *sV0ANail82 = new TGeoTube("sV0ANail82", 0.0, 0.4, 5.09/2.);
    TGeoVolume *v0ANail82 = new TGeoVolume("V0ANail82", sV0ANail82, medV0APMAlum); 
    v0ANail82->SetLineColor(kV0AColorPMA);
    v0LE->AddNode(v0ANail82,1,new TGeoTranslation(30.8,-30.04,0.0)); 



   /* /// Basis Construction
    rot = new TGeoRotation("rot"); rot->RotateX(90-fV0APMBAng); rot->RotateZ(-22.5);
    TGeoCombiTrans *pos1 = new TGeoCombiTrans("pos1", shiftR*sin225, shiftR*cos225, shiftZ, rot);
    pos1->RegisterYourself();
    for (int i=0;i<2;i++) {
    v0APts[0+8*i] = fV0AR6/cos225*sin45;  v0APts[1+8*i] = fV0AR6/cos225*sin45;
    v0APts[2+8*i] = 0;		    v0APts[3+8*i] = fV0AR6/cos225;
    v0APts[4+8*i] = 0;		    v0APts[5+8*i] = fV0AR6/cos225+fV0APlaEx;
    v0APts[6+8*i] = fV0AR6/cos225-(fV0AR6/cos225+fV0APlaEx)/ctg225;
    v0APts[7+8*i] = fV0AR6/cos225+fV0APlaEx;
    }
    new TGeoArb8("sV0APlaExIn1", (fV0APlaWd-2*fV0APlaAl)/2., v0APts);
    new TGeoArb8("sV0APlaExOu1", fV0APlaAl/2., v0APts);
    TGeoCompositeShape *sV0APlaExIn = new TGeoCompositeShape("sV0APlaExIn","sV0APlaExIn1-sV0APMB1:pos1");
    TGeoVolume *v0APlaExIn = new TGeoVolume("V0APlaExIn", sV0APlaExIn, medV0APlaIn);
    TGeoCompositeShape *sV0APlaExOu = new TGeoCompositeShape("sV0APlaExOu","sV0APlaExOu1-sV0APMB1:pos1");
    TGeoVolume *v0APlaExOu = new TGeoVolume("V0APlaExOu", sV0APlaExOu, medV0APlaOu);
    v0APlaExIn->SetLineColor(kV0AColorPlaIn); v0APlaExOu->SetLineColor(kV0AColorPlaOu);
    TGeoVolume *v0APlaEx = new TGeoVolumeAssembly("V0APlaEx");
    v0APlaEx->AddNode(v0APlaExIn,1);
    v0APlaEx->AddNode(v0APlaExOu,1,new TGeoTranslation(0,0,(fV0APlaWd-fV0APlaAl)/2.));
    v0APlaEx->AddNode(v0APlaExOu,2,new TGeoTranslation(0,0,-(fV0APlaWd-fV0APlaAl)/2.));
    for (int i=0;i<2;i++) {
    v0APts[0+8*i] = fV0AR6/cos225-(fV0AR6/cos225+fV0APlaEx)/ctg225-fV0ABasHt*sin45;
    v0APts[1+8*i] = fV0AR6/cos225+fV0APlaEx-fV0ABasHt*sin45;
    v0APts[2+8*i] = 0;  v0APts[3+8*i] = fV0AR6/cos225+fV0APlaEx-fV0ABasHt;
    v0APts[4+8*i] = 0;  v0APts[5+8*i] = fV0AR6/cos225+fV0APlaEx;
    v0APts[6+8*i] = fV0AR6/cos225-(fV0AR6/cos225+fV0APlaEx)/ctg225;
    v0APts[7+8*i] = fV0AR6/cos225+fV0APlaEx;
    }
    new TGeoArb8("sV0ABas1", (fV0ASciWd+2*fV0AOctWd)/2., v0APts);
    TGeoCompositeShape *sV0ABas = new TGeoCompositeShape("sV0ABas","sV0ABas1-sV0APMB1:pos1");
    TGeoVolume *v0ABas = new TGeoVolume("V0ABas", sV0ABas, medV0ABas);
    v0ABas->SetLineColor(kV0AColorBas);
    TGeoVolume *v0ABasis = new TGeoVolumeAssembly("V0ABasis");
    rot = new TGeoRotation("rot",90.,180.,90.,90.,0.,0.);
    v0ABasis->AddNode(v0APlaEx,1, new TGeoTranslation(0,0,(fV0ASciWd+2*fV0AOctWd+fV0APlaWd)/2.));
    v0ABasis->AddNode(v0APlaEx,2, new TGeoTranslation(0,0,-(fV0ASciWd+2*fV0AOctWd+fV0APlaWd)/2.));
    v0ABasis->AddNode(v0APlaEx,3, new TGeoCombiTrans(0,0,(fV0ASciWd+2*fV0AOctWd+fV0APlaWd)/2.,rot));
    v0ABasis->AddNode(v0APlaEx,4, new TGeoCombiTrans(0,0,-(fV0ASciWd+2*fV0AOctWd+fV0APlaWd)/2.,rot));
    v0ABasis->AddNode(v0ABas,1);
    v0ABasis->AddNode(v0ABas,2,rot);
    rot = new TGeoRotation("rot");
    rot->RotateZ(180);
    v0LE->AddNode(v0ABasis,1,rot); */

    // Adding detectors to top volume
    TGeoVolume *vZERO = new TGeoVolumeAssembly("VZERO");
    vZERO->AddNode(v0RI,1,new TGeoTranslation(0, 0, -zdet));
    // V0A position according to TB decision 13/12/2005 
    //rot=new TGeoRotation("rot");
    //rot->RotateX(90);
    //rot->RotateY(180);
    //rot->RotateZ(270);
    vZERO->AddNode(v0LE,1,new TGeoTranslation(0, 0, +327.5));//,rot));
    top->AddNode(vZERO,1);
}

//_____________________________________________________________________________
void AliVZEROv7::AddAlignableVolumes() const
{
  //
  // Create entries for alignable volumes associating the symbolic volume
  // name with the corresponding volume path. Needs to be syncronized with
  // eventual changes in the geometry.
  // 
  TString vpC = "/ALIC_1/VZERO_1/V0RI_1";
  TString vpA = "/ALIC_1/VZERO_1/V0LE_1";
  TString snC = "VZERO/V0C";
  TString snA = "VZERO/V0A";
  
  if(!gGeoManager->SetAlignableEntry(snC.Data(),vpC.Data()))
    AliFatal(Form("Alignable entry %s not created. Volume path %s not valid", snC.Data(),vpC.Data()));
  if(!gGeoManager->SetAlignableEntry(snA.Data(),vpA.Data()))
    AliFatal(Form("Alignable entry %s not created. Volume path %s not valid", snA.Data(),vpA.Data()));

} 

//_____________________________________________________________________________
void AliVZEROv7::CreateMaterials()
{

// Creates materials used for geometry 

  AliDebug(2,"Create materials");
  // Parameters for simulation scope
  Int_t     fieldType       = gAlice->Field()->Integ();     // Field type 
  Double_t  maxField        = gAlice->Field()->Max();       // Field max.
  Double_t  maxBending      = 10;    // Max Angle
  Double_t  maxStepSize     = 0.01;  // Max step size 
  Double_t  maxEnergyLoss   = 1;     // Max Delta E
  Double_t  precision       = 0.003; // Precision
  Double_t  minStepSize     = 0.003; // Minimum step size 

  Int_t    id;
  Double_t a, z, radLength, absLength;
  Float_t density, as[4], zs[4], ws[4];

// Parameters  for V0CPrePlates: Aluminium
   a = 26.98; 
   z = 13.00;
   density     = 2.7;
   radLength   = 8.9;
   absLength   = 37.2;
   id = 2;
   AliMaterial( id, "V0CAlu", a, z, density, radLength, absLength, 0, 0);
   AliMedium(id, "V0CAlu", id, 1, fieldType, maxField, maxBending, maxStepSize,
	     maxEnergyLoss, precision, minStepSize);
		    
// Parameters  for V0CPlates: Carbon 
   a = 12.01; 
   z =  6.00;
   density   = 2.265;
   radLength = 18.8;
   absLength = 49.9;
   id = 3;
   AliMaterial(id, "V0CCar",  a, z, density, radLength, absLength, 0, 0);
   AliMedium(id, "V0CCar", id, 1, fieldType, maxField, maxBending, maxStepSize,
	     maxEnergyLoss, precision, minStepSize);
	    
// Parameters  for V0Cscintillator: BC408
   as[0] = 1.00794;	as[1] = 12.011;
   zs[0] = 1.;		zs[1] = 6.;
   ws[0] = 1.;	        ws[1] = 1.;
   density      = 1.032;
   id           = 4;
   AliMixture(id, "V0CSci", as, zs, density, -2, ws);
   AliMedium(id,"V0CSci", id, 1, fieldType, maxField, maxBending, maxStepSize,
	     maxEnergyLoss, precision, minStepSize);

// Parameters for V0Ascintilator: BC404
   as[0] = 1.00794;	as[1] = 12.011;
   zs[0] = 1.;		zs[1] = 6.;
   ws[0] = 5.21;	ws[1] = 4.74;
   density      = 1.032;
   id           = 5;
   AliMixture(id, "V0ASci", as, zs, density, -2, ws);
   AliMedium(id,  "V0ASci", id, 1, fieldType, maxField, maxBending, maxStepSize,
	     maxEnergyLoss, precision, minStepSize);

// Parameters for V0ALuc: Lucita but for the simulation BC404
   as[0] = 1.00794;	as[1] = 12.011;
   zs[0] = 1.;		zs[1] = 6.;
   ws[0] = 5.21;	ws[1] = 4.74;
   density      = 1.032;
   id           = 6;
   AliMixture(id, "V0ALuc", as, zs, density, -2, ws);
   AliMedium(id, "V0ALuc", id, 1, fieldType, maxField, maxBending, maxStepSize,
	     maxEnergyLoss, precision, minStepSize);

// Parameters for V0Aplate: EuroComposite - EC-PI 626 PS - AlMg3
   as[0] = 26.982;	as[1] = 24.305;
   zs[0] = 13.;		zs[1] = 12.;
   ws[0] = 1.;		ws[1] = 3.;
   density      = 3.034;
   id           = 7;
   AliMixture(id, "V0APlaOu", as, zs, density, -2, ws);
   AliMedium(id, "V0APlaOu", id, 1, fieldType, maxField, maxBending, maxStepSize,
	     maxEnergyLoss, precision, minStepSize);

// Parameters for V0Aplate: EuroComposite - EC-PI 626 PS - EC-PI 6.4-42
   as[0] = 1.00794;	as[1] = 12.011;
   zs[0] = 1.;		zs[1] = 6.;
   ws[0] = 5.21;	ws[1] = 4.74;
   density      = 0.042;
   id           = 8;
   AliMixture(id, "V0APlaIn", as, zs, density, -2, ws);
   AliMedium(id, "V0APlaIn", id, 1, fieldType, maxField, maxBending, maxStepSize,
	     maxEnergyLoss, precision, minStepSize);

// Parameters for V0Afiber: BC9929AMC Plastic Scintillating Fiber from Saint-Gobain
   as[0] = 1.00794;	as[1] = 12.011;
   zs[0] = 1.;		zs[1] = 6.;
   ws[0] = 4.82;	ws[1] = 4.85;
   density      = 1.05;
   id           = 9;
   AliMixture(id, "V0AFib", as, zs, density, -2, ws);
   AliMedium(id, "V0AFib", id, 1, fieldType, maxField, maxBending, maxStepSize,
	     maxEnergyLoss, precision, minStepSize);

// Parameters for V0APMA: Aluminium
   a = 26.98; 
   z = 13.00;
   density     = 2.7;
   radLength   = 8.9;
   absLength   = 37.2;
   id = 10;
   AliMaterial(id, "V0APMA",  a, z, density, radLength, absLength, 0, 0);
   AliMedium(id, "V0APMA", id, 1, fieldType, maxField, maxBending, maxStepSize,
	     maxEnergyLoss, precision, minStepSize);

// Parameters for V0APMG: Glass for the simulation Aluminium
   a = 26.98; 
   z = 13.00;
   density   = 2.7;
   radLength = 8.9;
   absLength = 37.2;
   id = 11;
   AliMaterial(id, "V0APMG",  a, z, density, radLength, absLength, 0, 0);
   AliMedium(id, "V0APMG", id, 1, fieldType, maxField, maxBending, maxStepSize,
	     maxEnergyLoss, precision, minStepSize);
}

//_____________________________________________________________________________
void AliVZEROv7::DrawModule() const
{
//  Drawing is done in DrawVZERO.C

   AliDebug(2,"DrawModule");
}


//_____________________________________________________________________________
void AliVZEROv7::DrawGeometry() 
{
//  Drawing of V0 geometry done in DrawV0.C

   AliDebug(2,"DrawGeometry");
}

//_____________________________________________________________________________
void AliVZEROv7::Init()
{
// Initialises version of the VZERO Detector given in Config
// Just prints an information message

//   AliInfo(Form("VZERO version %d initialized \n",IsVersion()));
   
   AliDebug(1,"VZERO version 7 initialized");
   AliVZERO::Init();  
}

//_____________________________________________________________________________
void AliVZEROv7::StepManager()
{
// Step Manager, called at each step  

  Int_t     copy;
  static    Int_t   vol[4];
  static    Float_t hits[21];
  static    Float_t eloss, tlength;
  static    Int_t   nPhotonsInStep = 0;
  static    Int_t   nPhotons = 0; 
  static    Int_t   numStep = 0;
  Int_t     ringNumber;
  Float_t   destep, step;
  numStep += 1; 

  //   We keep only charged tracks : 
  if ( !gMC->TrackCharge() || !gMC->IsTrackAlive() ) return;

  vol[0]    = gMC->CurrentVolOffID(1, vol[1]);
  vol[2]    = gMC->CurrentVolID(copy);
  vol[3]    = copy;
  static Int_t idV0R1 = gMC->VolId("V0R1");
  static Int_t idV0L1 = gMC->VolId("V0L1");
  static Int_t idV0L15 = gMC->VolId("V0L15");
  static Int_t idV0L16 = gMC->VolId("V0L16");
  static Int_t idV0L17 = gMC->VolId("V0L17");
  static Int_t idV0L18 = gMC->VolId("V0L18");  
  static Int_t idV0R2 = gMC->VolId("V0R2");
  static Int_t idV0L2 = gMC->VolId("V0L2");
  static Int_t idV0L25 = gMC->VolId("V0L25");
  static Int_t idV0L26 = gMC->VolId("V0L26");
  static Int_t idV0L27 = gMC->VolId("V0L27");
  static Int_t idV0L28 = gMC->VolId("V0L28");
  static Int_t idV0R3 = gMC->VolId("V0R3");
  static Int_t idV0L3 = gMC->VolId("V0L3");
  static Int_t idV0L35 = gMC->VolId("V0L35");
  static Int_t idV0L36 = gMC->VolId("V0L36");
  static Int_t idV0L37 = gMC->VolId("V0L37");
  static Int_t idV0L38 = gMC->VolId("V0L38");
  static Int_t idV0R4 = gMC->VolId("V0R4");
  static Int_t idV0L4 = gMC->VolId("V0L4");
  static Int_t idV0L45 = gMC->VolId("V0L45");
  static Int_t idV0L46 = gMC->VolId("V0L46");
  static Int_t idV0L47 = gMC->VolId("V0L47");
  static Int_t idV0L48 = gMC->VolId("V0L48");
  static Int_t idV0R5 = gMC->VolId("V0R5");
  static Int_t idV0R6 = gMC->VolId("V0R6");
  bool   hitOnV0C = true;
  double lightYield;
  double lightAttenuation;
  double nMeters; 
  double fibToPhot;
  if      ( gMC->CurrentVolID(copy) == idV0R1 || gMC->CurrentVolID(copy) == idV0L1 || gMC->CurrentVolID(copy) == idV0L15 ||  gMC->CurrentVolID(copy) == idV0L16  || gMC->CurrentVolID(copy) == idV0L17  || gMC->CurrentVolID(copy) == idV0L18  )
    ringNumber = 1;
  else if ( gMC->CurrentVolID(copy) == idV0R2 || gMC->CurrentVolID(copy) == idV0L2 || gMC->CurrentVolID(copy) == idV0L25 || gMC->CurrentVolID(copy) == idV0L26 || gMC->CurrentVolID(copy) == idV0L27 || gMC->CurrentVolID(copy) == idV0L28 )
    ringNumber = 2;  
  else if ( gMC->CurrentVolID(copy) == idV0R3 || gMC->CurrentVolID(copy) == idV0R4
	    || gMC->CurrentVolID(copy) == idV0L3 || gMC->CurrentVolID(copy) == idV0L35 || gMC->CurrentVolID(copy) == idV0L36 || gMC->CurrentVolID(copy) == idV0L37 || gMC->CurrentVolID(copy) == idV0L38 ) ringNumber = 3;
  else if ( gMC->CurrentVolID(copy) == idV0R5 || gMC->CurrentVolID(copy) == idV0R6
	    || gMC->CurrentVolID(copy) == idV0L4 || gMC->CurrentVolID(copy) == idV0L45 || gMC->CurrentVolID(copy) == idV0L46 || gMC->CurrentVolID(copy) == idV0L47 || gMC->CurrentVolID(copy) == idV0L48 ) ringNumber = 4;	       
  else ringNumber = 0;
  if  (ringNumber) {
    if (gMC->CurrentVolID(copy) == idV0L1 || gMC->CurrentVolID(copy) == idV0L15  || gMC->CurrentVolID(copy) == idV0L16  || gMC->CurrentVolID(copy) == idV0L17 || gMC->CurrentVolID(copy) == idV0L18 || gMC->CurrentVolID(copy) == idV0L2 || gMC->CurrentVolID(copy) == idV0L25 || gMC->CurrentVolID(copy) == idV0L26  || gMC->CurrentVolID(copy) == idV0L27  || gMC->CurrentVolID(copy) == idV0L28 || gMC->CurrentVolID(copy) == idV0L3 || gMC->CurrentVolID(copy) == idV0L35 || gMC->CurrentVolID(copy) == idV0L36 || gMC->CurrentVolID(copy) == idV0L37 || gMC->CurrentVolID(copy) == idV0L38 || gMC->CurrentVolID(copy) == idV0L4 || gMC->CurrentVolID(copy) == idV0L45 || gMC->CurrentVolID(copy) == idV0L46 || gMC->CurrentVolID(copy) == idV0L47 || gMC->CurrentVolID(copy) == idV0L48)
      hitOnV0C = false;
    destep = gMC->Edep();
    step   = gMC->TrackStep();
    if (hitOnV0C) {
      lightYield = fV0CLightYield;
      lightAttenuation = fV0CLightAttenuation;
      nMeters = fV0CnMeters;
      fibToPhot = fV0CFibToPhot;
    } else {
      lightYield = fV0ALightYield;
      lightAttenuation = fV0ALightAttenuation;
      nMeters = fV0AnMeters;
      fibToPhot = fV0AFibToPhot;
    }
    nPhotonsInStep  = Int_t(destep / (lightYield *1e-9) );	
    nPhotonsInStep  = gRandom->Poisson(nPhotonsInStep);
    eloss    += destep;
    tlength  += step; 	 
    if ( gMC->IsTrackEntering() ) { 
      nPhotons  =  nPhotonsInStep;
      gMC->TrackPosition(fTrackPosition);
      gMC->TrackMomentum(fTrackMomentum);
      Float_t pt  = TMath::Sqrt( fTrackMomentum.Px() * fTrackMomentum.Px()
				 + fTrackMomentum.Py() * fTrackMomentum.Py() );
      TParticle *par = gAlice->GetMCApp()->Particle(gAlice->GetMCApp()->GetCurrentTrackNumber());
      hits[0]  = fTrackPosition.X();
      hits[1]  = fTrackPosition.Y();
      hits[2]  = fTrackPosition.Z();	 	 
      hits[3]  = Float_t (gMC->TrackPid()); 
      hits[4]  = gMC->TrackTime();
      hits[5]  = gMC->TrackCharge();
      hits[6]  = fTrackMomentum.Theta()*TMath::RadToDeg();
      hits[7]  = fTrackMomentum.Phi()*TMath::RadToDeg();
      hits[8]  = ringNumber;
      hits[9]  = pt;
      hits[10] = fTrackMomentum.P();
      hits[11] = fTrackMomentum.Px();
      hits[12] = fTrackMomentum.Py();
      hits[13] = fTrackMomentum.Pz();
      hits[14] = par->Vx();
      hits[15] = par->Vy();
      hits[16] = par->Vz();
      tlength  = 0.0;
      eloss    = 0.0;	    

      //////////////////////////
      ///// Display V0A geometry
      //      if (!hitOnV0C) {
      //      	FILE *of;
      //      	of = fopen("V0A.out", "a");
      //      	// x, y, z, ringnumber, cellid
      //      	fprintf( of, "%f %f %f %f %d \n",  hits[0], hits[1], hits[2], hits[8], GetCellId (vol, hits) );
      //      	fclose(of);
      //      }
      //////////////////////////
    }
    nPhotons  = nPhotons + nPhotonsInStep;
    if( gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared()){
      nPhotons = nPhotons - Int_t((Float_t(nPhotons) * lightAttenuation * nMeters));
      nPhotons = nPhotons - Int_t( Float_t(nPhotons) * fibToPhot);
      hits[17] = eloss;
      hits[18] = tlength;
      hits[19] = nPhotons;
      hits[20] = GetCellId (vol, hits);
      AddHit(gAlice->GetMCApp()->GetCurrentTrackNumber(), vol, hits);
      tlength         = 0.0;
      eloss           = 0.0; 
      nPhotons        = 0;
      nPhotonsInStep  = 0;
      numStep         = 0;  
    }
  }
}

//_____________________________________________________________________________
void AliVZEROv7::AddHit(Int_t track, Int_t *vol, Float_t *hits)
{
//  Adds a VZERO hit

  TClonesArray &lhits = *fHits;
  new(lhits[fNhits++]) AliVZEROhit(fIshunt,track,vol,hits);
}

//_____________________________________________________________________________
void AliVZEROv7::AddDigits(Int_t *tracks, Int_t* digits) 
{
//  Adds a VZERO digit

   TClonesArray  &ldigits = *fDigits;
   new(ldigits[fNdigits++]) AliVZEROdigit(tracks, digits);
}

//_____________________________________________________________________________
void AliVZEROv7::MakeBranch(Option_t *option)
{
// Creates new branches in the current Root Tree
    
  char branchname[10];
  sprintf(branchname,"%s",GetName());
  AliDebug(2,Form("fBufferSize = %d",fBufferSize));
  const char *cH = strstr(option,"H");
  if (fHits   && TreeH() && cH) {
    TreeH()->Branch(branchname,&fHits, fBufferSize);
    AliDebug(2,Form("Making Branch %s for hits",branchname));
  }     
  const char *cD = strstr(option,"D");
  if (fDigits   && fLoader->TreeD() && cD) {
    fLoader->TreeD()->Branch(branchname,&fDigits, fBufferSize);
    AliDebug(2,Form("Making Branch %s for digits",branchname));
  }  
}

//_____________________________________________________________________________
Int_t AliVZEROv7::GetCellId(Int_t *vol, Float_t *hits) 
{
  //   Returns Id of scintillator cell
  //   Right side from  0 to 47 
  //   Left  side from 48 to 79
  //   hits[8] = ring number (1 to 4)
  //   vol[1]  = copy number (1 to 8)

  Int_t index      = vol[1];
  Int_t ringNumber = Int_t(hits[8]);
  fCellId          = 0;

  Float_t phi = Float_t(TMath::ATan2(Double_t(hits[1]),Double_t(hits[0])) ); 
  Float_t kRaddeg = 180.0/TMath::Pi();
  phi = kRaddeg * phi;

  if (index < 7) index = index + 8;

  if (hits[2] < 0.0) {
    if(ringNumber < 3) {
      index = (index - 7) + ( ( ringNumber - 1 ) * 8);
    } else if (ringNumber >= 3) { 
      if ( gMC->CurrentVolID(vol[1]) == gMC->VolId("V0R3") || gMC->CurrentVolID(vol[1])
	   == gMC->VolId("V0R5") )  index = (index*2-14)+((ringNumber-2)*16);
      if ( gMC->CurrentVolID(vol[1]) == gMC->VolId("V0R4") || gMC->CurrentVolID(vol[1])
	   == gMC->VolId("V0R6") )  index = (index*2-13)+((ringNumber-2)*16);
    }
    fCellId   = index;           
  } else if (hits[2] > 0.0) {
    index = (index - 7 + 48) + ( ( ringNumber - 1 ) * 8);
    fCellId   = index;
  }

  return fCellId;
}

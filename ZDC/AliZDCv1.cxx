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
//  		AliZDCv1 --- ZDC geometry as designed in TDR (obsolete!)     //
//  			with the EM ZDC at 116 m from IP		     //
//  Just one set of ZDC is inserted, on the same side of the dimuon arm      //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

// --- Standard libraries
#include "stdio.h"

// --- ROOT system
#include <TBRIK.h>
#include <TLorentzVector.h>
#include <TMath.h>
#include <TNode.h>
#include <TRandom.h>
#include <TSystem.h>
#include <TTree.h>
#include <TVirtualMC.h>

// --- AliRoot classes
#include "AliConst.h"
#include "AliDetector.h"
#include "AliMagF.h"
#include "AliPDG.h"
#include "AliRun.h"
#include "AliZDCHit.h"
#include "AliZDCv1.h"
 
 
ClassImp(AliZDCv1)
 
//_____________________________________________________________________________
AliZDCv1::AliZDCv1() : AliZDC()
{
  //
  // Default constructor for Zero Degree Calorimeter
  //
  
  fMedSensF1  = 0;
  fMedSensF2  = 0;
  fMedSensZN  = 0;
  fMedSensZP  = 0;
  fMedSensZEM = 0;
  fMedSensGR  = 0;
//  fMedSensPI  = 0;
//  fMedSensTDI = 0;
}
 
//_____________________________________________________________________________
AliZDCv1::AliZDCv1(const char *name, const char *title)
  : AliZDC(name,title)
{
  //
  // Standard constructor for Zero Degree Calorimeter 
  //
  //
  // Check that DIPO, ABSO, DIPO and SHIL is there (otherwise tracking is wrong!!!)
  
  AliModule *PIPE=gAlice->GetModule("PIPE");
  AliModule *ABSO=gAlice->GetModule("ABSO");
  AliModule *DIPO=gAlice->GetModule("DIPO");
  AliModule *SHIL=gAlice->GetModule("SHIL");
  if((!PIPE) || (!ABSO) || (!DIPO) || (!SHIL)) {
    Error("Constructor","ZDC needs PIPE, ABSO, DIPO and SHIL!!!\n");
    exit(1);
  } 

  fMedSensF1  = 0;
  fMedSensF2  = 0;
  fMedSensZN  = 0;
  fMedSensZP  = 0;
  fMedSensZEM = 0;
  fMedSensGR  = 0;
//  fMedSensPI  = 0;
//  fMedSensTDI = 0;

  
  // Parameters for light tables
  fNalfan = 90;       // Number of Alfa (neutrons)
  fNalfap = 90;       // Number of Alfa (protons)
  fNben = 18;         // Number of beta (neutrons)
  fNbep = 28;         // Number of beta (protons)
  Int_t ip,jp,kp;
  for(ip=0; ip<4; ip++){
     for(kp=0; kp<fNalfap; kp++){
        for(jp=0; jp<fNbep; jp++){
           fTablep[ip][kp][jp] = 0;
        } 
     }
  }
  Int_t in,jn,kn;
  for(in=0; in<4; in++){
     for(kn=0; kn<fNalfan; kn++){
        for(jn=0; jn<fNben; jn++){
           fTablen[in][kn][jn] = 0;
        } 
     }
  }

  // Parameters for hadronic calorimeters geometry
  fDimZP[0] = 11.2;
  fDimZP[1] = 6.;
  fDimZP[2] = 75.;    
  fPosZN[0] = 0.;
  fPosZN[1] = 1.2;
  fPosZN[2] = 11650.;
  fPosZP[0] = -24.;
  fPosZP[1] = 0.;
  fPosZP[2] = 11600.;
  fFibZN[0] = 0.;
  fFibZN[1] = 0.01825;
  fFibZN[2] = 50.;
  fFibZP[0] = 0.;
  fFibZP[1] = 0.0275;
  fFibZP[2] = 75.;
  
  // Parameters for EM calorimeter geometry
  fPosZEM[0] = 0.;
  fPosZEM[1] = 5.8;
  fPosZEM[2] = 11600.;
  
}
 
//_____________________________________________________________________________
void AliZDCv1::CreateGeometry()
{
  //
  // Create the geometry for the Zero Degree Calorimeter version 1
  //* Initialize COMMON block ZDC_CGEOM
  //*

  CreateBeamLine();
  CreateZDC();
}
  
//_____________________________________________________________________________
void AliZDCv1::CreateBeamLine()
{
  
  Float_t zq, zd1, zd2;
  Float_t conpar[9], tubpar[3], tubspar[5], boxpar[3];
  Int_t im1, im2;
  
  Int_t *idtmed = fIdtmed->GetArray();
  
  // -- Mother of the ZDCs (Vacuum PCON)
  
  conpar[0] = 0.;
  conpar[1] = 360.;
  conpar[2] = 2.;
  conpar[3] = 2000.;
  conpar[4] = 0.;
  conpar[5] = 55.;
  conpar[6] = 13060.;
  conpar[7] = 0.;
  conpar[8] = 55.;
  gMC->Gsvolu("ZDC ", "PCON", idtmed[11], conpar, 9);
  gMC->Gspos("ZDC ", 1, "ALIC", 0., 0., 0., 0, "ONLY");

  // -- FIRST SECTION OF THE BEAM PIPE (from compensator dipole to 
  //    	the beginning of D1) 
  
  zd1 = 2000.;
  
  tubpar[0] = 6.3/2.;
  tubpar[1] = 6.7/2.;
  tubpar[2] = 3838.3/2.;
  gMC->Gsvolu("QT01", "TUBE", idtmed[7], tubpar, 3);
  gMC->Gspos("QT01", 1, "ZDC ", 0., 0., tubpar[2] + zd1, 0, "ONLY");
  
  //-- SECOND SECTION OF THE BEAM PIPE (from the end of D1 to the
  //    	beginning of D2) 
  
  //-- FROM MAGNETIC BEGINNING OF D1 TO MAGNETIC END OF D1 + 13.5 cm
  //-- 	Cylindrical pipe (r = 3.47) + conical flare
  
  // -> Beginning of D1
  zd1 += 2.*tubpar[2];
  
  tubpar[0] = 3.47;
  tubpar[1] = 3.47+0.2;
  tubpar[2] = 958.5/2.;
  gMC->Gsvolu("QT02", "TUBE", idtmed[7], tubpar, 3);
  gMC->Gspos("QT02", 1, "ZDC ", 0., 0., tubpar[2] + zd1, 0, "ONLY");

  zd1 += 2.*tubpar[2];
  
  conpar[0] = 25./2.;
  conpar[1] = 6.44/2.;
  conpar[2] = 6.84/2.;
  conpar[3] = 10./2.;
  conpar[4] = 10.4/2.;
  gMC->Gsvolu("QC01", "CONE", idtmed[7], conpar, 5);
  gMC->Gspos("QC01", 1, "ZDC ", 0., 0., conpar[0] + zd1, 0, "ONLY");

  zd1 += 2.*conpar[0];
  
  tubpar[0] = 10./2.;
  tubpar[1] = 10.4/2.;
  tubpar[2] = 50./2.;
  gMC->Gsvolu("QT03", "TUBE", idtmed[7], tubpar, 3);
  gMC->Gspos("QT03", 1, "ZDC ", 0., 0., tubpar[2] + zd1, 0, "ONLY");
  
  zd1 += tubpar[2]*2.;
  
  tubpar[0] = 10./2.;
  tubpar[1] = 10.4/2.;
  tubpar[2] = 10./2.;
  gMC->Gsvolu("QT04", "TUBE", idtmed[7], tubpar, 3);
  gMC->Gspos("QT04", 1, "ZDC ", 0., 0., tubpar[2] + zd1, 0, "ONLY");
  
  zd1 += tubpar[2] * 2.;
  
  tubpar[0] = 10./2.;
  tubpar[1] = 10.4/2.;
  tubpar[2] = 3.16/2.;
  gMC->Gsvolu("QT05", "TUBE", idtmed[7], tubpar, 3);
  gMC->Gspos("QT05", 1, "ZDC ", 0., 0., tubpar[0] + zd1, 0, "ONLY");
  
  zd1 += tubpar[2] * 2.;
  
  tubpar[0] = 10.0/2.;
  tubpar[1] = 10.4/2;
  tubpar[2] = 190./2.;
  gMC->Gsvolu("QT06", "TUBE", idtmed[7], tubpar, 3);
  gMC->Gspos("QT06", 1, "ZDC ", 0., 0., tubpar[2] + zd1, 0, "ONLY");
  
  zd1 += tubpar[2] * 2.;
  
  conpar[0] = 30./2.;
  conpar[1] = 10./2.;
  conpar[2] = 10.4/2.;
  conpar[3] = 20.6/2.;
  conpar[4] = 21./2.;
  gMC->Gsvolu("QC02", "CONE", idtmed[7], conpar, 5);
  gMC->Gspos("QC02", 1, "ZDC ", 0., 0., conpar[0] + zd1, 0, "ONLY");
  
  zd1 += conpar[0] * 2.;
  
  tubpar[0] = 20.6/2.;
  tubpar[1] = 21./2.;
  tubpar[2] = 450./2.;
  gMC->Gsvolu("QT07", "TUBE", idtmed[7], tubpar, 3);
  gMC->Gspos("QT07", 1, "ZDC ", 0., 0., tubpar[2] + zd1, 0, "ONLY");
  
  zd1 += tubpar[2] * 2.;
  
  conpar[0] = 13.6/2.;
  conpar[1] = 20.6/2.;
  conpar[2] = 21./2.;
  conpar[3] = 25.4/2.;
  conpar[4] = 25.8/2.;
  gMC->Gsvolu("QC03", "CONE", idtmed[7], conpar, 5);
  gMC->Gspos("QC03", 1, "ZDC ", 0., 0., conpar[0] + zd1, 0, "ONLY");
  
  zd1 += conpar[0] * 2.;
  
  tubpar[0] = 25.4/2.;
  tubpar[1] = 25.8/2.;
  tubpar[2] = 205.8/2.;
  gMC->Gsvolu("QT08", "TUBE", idtmed[7], tubpar, 3);
  gMC->Gspos("QT08", 1, "ZDC ", 0., 0., tubpar[2] + zd1, 0, "ONLY");
  
  zd1 += tubpar[2] * 2.;
  
  tubpar[0] = 50./2.;
  tubpar[1] = 50.4/2.;
  // QT09 is 10 cm longer to accomodate TDI
  tubpar[2] = 515.4/2.;
  gMC->Gsvolu("QT09", "TUBE", idtmed[7], tubpar, 3);
  gMC->Gspos("QT09", 1, "ZDC ", 0., 0., tubpar[2] + zd1, 0, "ONLY"); 
  
  // --- Insert TDI (inside ZDC volume)
  
  boxpar[0] = 5.6;
  boxpar[1] = 5.6;
  boxpar[2] = 400./2.;
  gMC->Gsvolu("QTD1", "BOX ", idtmed[7], boxpar, 3);
  gMC->Gspos("QTD1", 1, "ZDC ", 3., 10.6, tubpar[2] + zd1 + 56.3, 0, "ONLY");
  gMC->Gspos("QTD1", 2, "ZDC ", 3., -10.6, tubpar[2] + zd1 + 56.3, 0, "ONLY");
  
  boxpar[0] = 0.2/2.;
  boxpar[1] = 5.6;
  boxpar[2] = 400./2.;
  gMC->Gsvolu("QTD2", "BOX ", idtmed[6], boxpar, 3);
  gMC->Gspos("QTD2", 1, "ZDC ", 8.6+boxpar[0], 0., tubpar[2] + zd1 + 56.3, 0, "ONLY");
  
//  tubspar[0] = 6.2;	// R = 6.2 cm----------------------------------------
//  tubspar[1] = 6.4;
//  tubspar[2] = 400./2.;
//  tubspar[3] = 180.-62.5;
//  tubspar[4] = 180.+62.5;
  tubspar[0] = 10.5;	// R = 10.5 cm------------------------------------------
  tubspar[1] = 10.7;
  tubspar[2] = 400./2.;
  tubspar[3] = 180.-75.5;
  tubspar[4] = 180.+75.5;
  gMC->Gsvolu("QTD3", "TUBS", idtmed[6], tubspar, 5);
  gMC->Gspos("QTD3", 1, "ZDC ", 0., 0., tubpar[2] + zd1 + 56.3, 0, "ONLY");

  zd1 += tubpar[2] * 2.;
  
  tubpar[0] = 50./2.;
  tubpar[1] = 50.4/2.;
  // QT10 is 10 cm shorter
  tubpar[2] = 690./2.;
  gMC->Gsvolu("QT10", "TUBE", idtmed[7], tubpar, 3);
  gMC->Gspos("QT10", 1, "ZDC ", 0., 0., tubpar[2] + zd1, 0, "ONLY");
  
  zd1 += tubpar[2] * 2.;
  
  tubpar[0] = 50./2.;
  tubpar[1] = 50.4/2.;
  tubpar[2] = 778.5/2.;
  gMC->Gsvolu("QT11", "TUBE", idtmed[7], tubpar, 3);
  gMC->Gspos("QT11", 1, "ZDC ", 0., 0., tubpar[2] + zd1, 0, "ONLY");
  
  zd1 += tubpar[2] * 2.;
  
  conpar[0] = 14.18/2.;
  conpar[1] = 50./2.;
  conpar[2] = 50.4/2.;
  conpar[3] = 55./2.;
  conpar[4] = 55.4/2.;
  gMC->Gsvolu("QC04", "CONE", idtmed[7], conpar, 5);
  gMC->Gspos("QC04", 1, "ZDC ", 0., 0., conpar[0] + zd1, 0, "ONLY");
  
  zd1 += conpar[0] * 2.;
  
  tubpar[0] = 55./2.;
  tubpar[1] = 55.4/2.;
  tubpar[2] = 730./2.;
  gMC->Gsvolu("QT12", "TUBE", idtmed[7], tubpar, 3);
  gMC->Gspos("QT12", 1, "ZDC ", 0., 0., tubpar[2] + zd1, 0, "ONLY");
  
  zd1 += tubpar[2] * 2.;
  
  conpar[0] = 36.86/2.;
  conpar[1] = 55./2.;
  conpar[2] = 55.4/2.;
  conpar[3] = 68./2.;
  conpar[4] = 68.4/2.;
  gMC->Gsvolu("QC05", "CONE", idtmed[7], conpar, 5);
  gMC->Gspos("QC05", 1, "ZDC ", 0., 0., conpar[0] + zd1, 0, "ONLY");
  
  zd1 += conpar[0] * 2.;
  
  tubpar[0] = 68./2.;
  tubpar[1] = 68.4/2.;
  tubpar[2] = 927.3/2.;
  gMC->Gsvolu("QT13", "TUBE", idtmed[7], tubpar, 3);
  gMC->Gspos("QT13", 1, "ZDC ", 0., 0., tubpar[2] + zd1, 0, "ONLY");
  
  zd1 += tubpar[2] * 2.;
  
  tubpar[0] = 0./2.;
  tubpar[1] = 68.4/2.;
  tubpar[2] = 0.2/2.;
  gMC->Gsvolu("QT14", "TUBE", idtmed[8], tubpar, 3);
  gMC->Gspos("QT14", 1, "ZDC ", 0., 0., tubpar[2] + zd1, 0, "ONLY");
  
  zd1 += tubpar[2] * 2.;
  
  tubpar[0] = 0./2.;
  tubpar[1] = 6.4/2.;
  tubpar[2] = 0.2/2.;
  gMC->Gsvolu("QT15", "TUBE", idtmed[11], tubpar, 3);
  
  //-- Position QT15 inside QT14
  gMC->Gspos("QT15", 1, "QT14", -7.7, 0., 0., 0, "ONLY");
  
  tubpar[0] = 0./2.;
  tubpar[1] = 6.4/2.;
  tubpar[2] = 0.2/2.;
  gMC->Gsvolu("QT16", "TUBE", idtmed[11], tubpar, 3);
  
  //-- Position QT16 inside QT14
  gMC->Gspos("QT16", 1, "QT14", 7.7, 0., 0., 0, "ONLY");
  
  
  //-- BEAM PIPE BETWEEN END OF CONICAL PIPE AND BEGINNING OF D2
  
  tubpar[0] = 6.4/2.;
  tubpar[1] = 6.8/2.;
  tubpar[2] = 680.8/2.;
  gMC->Gsvolu("QT17", "TUBE", idtmed[7], tubpar, 3);

  tubpar[0] = 6.4/2.;
  tubpar[1] = 6.8/2.;
  tubpar[2] = 680.8/2.;
  gMC->Gsvolu("QT18", "TUBE", idtmed[7], tubpar, 3);
  
  // -- ROTATE PIPES 

  Float_t angle = 0.143*kDegrad;
  
  AliMatrix(im1, 90.-0.143, 0., 90., 90., 0.143, 180.);
  gMC->Gspos("QT17", 1, "ZDC ", TMath::Sin(angle) * 680.8/ 2. - 9.4, 
             0., tubpar[2] + zd1, im1, "ONLY");
	     
  AliMatrix(im2, 90.+0.143, 0., 90., 90., 0.143, 0.);
  gMC->Gspos("QT18", 1, "ZDC ", 9.7 - TMath::Sin(angle) * 680.8 / 2., 
             0., tubpar[2] + zd1, im2, "ONLY");
	     
  
  // --  END OF BEAM PIPE VOLUME DEFINITION.  
  // ----------------------------------------------------------------
   
  // --  MAGNET DEFINITION  -> LHC OPTICS 6.2 (preliminary version) 
  
  // ----------------------------------------------------------------
  // 			Replaced by the muon dipole
  // ----------------------------------------------------------------
  // -- COMPENSATOR DIPOLE (MBXW) 
  //     GAP (VACUUM WITH MAGNETIC FIELD) 
  
//  tubpar[0] = 0.;
//  tubpar[1] = 4.5;
//  tubpar[2] = 340./2.;
//  gMC->Gsvolu("MBXW", "TUBE", idtmed[11], tubpar, 3);
//  gMC->Gspos("MBXW", 1, "ZDC ", 0., 0., tubpar[2] + 805., 0, "ONLY");
  
  // --  YOKE (IRON WITHOUT MAGNETIC FIELD) 
  
//  tubpar[0] = 4.5;
//  tubpar[1] = 55.;
//  tubpar[2] = 340./2.;
//  gMC->Gsvolu("YMBX", "TUBE", idtmed[7], tubpar, 3);
//  gMC->Gspos("YMBX", 1, "ZDC ", 0., 0., tubpar[2] + 805., 0, "ONLY");
  
  // ----------------------------------------------------------------
  // 		      Replaced by the second dipole
  // ----------------------------------------------------------------
  // -- COMPENSATOR DIPOLE (MCBWA) 
  //     GAP (VACUUM WITH MAGNETIC FIELD) 
  
//  tubpar[0] = 0.;
//  tubpar[1] = 4.5;
//  tubpar[2] = 170./2.;
//  gMC->Gsvolu("MCBW", "TUBE", idtmed[11], tubpar, 3);
//  gMC->Gspos("MCBW", 1, "ZDC ", 0., 0., tubpar[2] + 1921.6, 0, "ONLY");
  
  // --  YOKE (IRON WITHOUT MAGNETIC FIELD) 
  
//  tubpar[0] = 4.5;
//  tubpar[1] = 55.;
//  tubpar[2] = 170./2.;
//  gMC->Gsvolu("YMCB", "TUBE", idtmed[7], tubpar, 3);
//  gMC->Gspos("YMCB", 1, "ZDC ", 0., 0., tubpar[2] + 1921.6, 0, "ONLY");
  
  // -- INNER TRIPLET 
  
  zq = 2296.5;
  
  // -- DEFINE MQXL AND MQX QUADRUPOLE ELEMENT 
  
  //     MQXL 
  // --  GAP (VACUUM WITH MAGNETIC FIELD) 
  
  tubpar[0] = 0.;
  tubpar[1] = 3.5;
  tubpar[2] = 637./2.;
  gMC->Gsvolu("MQXL", "TUBE", idtmed[11], tubpar, 3);
  
  // --  YOKE 
  
  tubpar[0] = 3.5;
  tubpar[1] = 22.;
  tubpar[2] = 637./2.;
  gMC->Gsvolu("YMQL", "TUBE", idtmed[7], tubpar, 3);
  
  gMC->Gspos("MQXL", 1, "ZDC ", 0., 0., tubpar[2] + zq, 0, "ONLY");
  gMC->Gspos("YMQL", 1, "ZDC ", 0., 0., tubpar[2] + zq, 0, "ONLY");
  
  gMC->Gspos("MQXL", 2, "ZDC ", 0., 0., tubpar[2] + zq + 2430., 0, "ONLY");
  gMC->Gspos("YMQL", 2, "ZDC ", 0., 0., tubpar[2] + zq + 2430., 0, "ONLY");
  
  // --  MQX 
  // --  GAP (VACUUM WITH MAGNETIC FIELD) 
  
  tubpar[0] = 0.;
  tubpar[1] = 3.5;
  tubpar[2] = 550./2.;
  gMC->Gsvolu("MQX ", "TUBE", idtmed[11], tubpar, 3);
  
  // --  YOKE 
  
  tubpar[0] = 3.5;
  tubpar[1] = 22.;
  tubpar[2] = 550./2.;
  gMC->Gsvolu("YMQ ", "TUBE", idtmed[7], tubpar, 3);
  
  gMC->Gspos("MQX ", 1, "ZDC ", 0., 0., tubpar[2] + zq + 883.5,  0, "ONLY");
  gMC->Gspos("YMQ ", 1, "ZDC ", 0., 0., tubpar[2] + zq + 883.5,  0, "ONLY");
  
  gMC->Gspos("MQX ", 2, "ZDC ", 0., 0., tubpar[2] + zq + 1533.5, 0, "ONLY");
  gMC->Gspos("YMQ ", 2, "ZDC ", 0., 0., tubpar[2] + zq + 1533.5, 0, "ONLY");
  
  // -- SEPARATOR DIPOLE D1 
  
  zd1 = 5838.3;
  
  // --  GAP (VACUUM WITH MAGNETIC FIELD) 
  
  tubpar[0] = 0.;
  tubpar[1] = 6.94/2.;
  tubpar[2] = 945./2.;
  gMC->Gsvolu("MD1 ", "TUBE", idtmed[11], tubpar, 3);
  
  // --  Insert horizontal Cu plates inside D1 
  // --   (to simulate the vacuum chamber)
  
  boxpar[0] = TMath::Sqrt(tubpar[1]*tubpar[1]-(2.98+0.2)*(2.98+0.2));
  boxpar[1] = 0.2/2.;
  boxpar[2] =945./2.;
  gMC->Gsvolu("MD1V", "BOX ", idtmed[6], boxpar, 3);
  gMC->Gspos("MD1V", 1, "MD1 ", 0., 2.98+boxpar[1], 0., 0, "ONLY");
  gMC->Gspos("MD1V", 2, "MD1 ", 0., -2.98-boxpar[1], 0., 0, "ONLY");
    
  // --  YOKE 
  
  tubpar[0] = 0.;
  tubpar[1] = 110./2;
  tubpar[2] = 945./2.;
  gMC->Gsvolu("YD1 ", "TUBE", idtmed[7], tubpar, 3);
  
  gMC->Gspos("YD1 ", 1, "ZDC ", 0., 0., tubpar[2] + zd1, 0, "ONLY");
  gMC->Gspos("MD1 ", 1, "YD1 ", 0., 0., 0., 0, "ONLY");
  
  // -- DIPOLE D2 
  
  zd2 = 12147.6;
  
  // --  GAP (VACUUM WITH MAGNETIC FIELD) 
  
  tubpar[0] = 0.;
  tubpar[1] = 7.5/2.;
  tubpar[2] = 945./2.;
  gMC->Gsvolu("MD2 ", "TUBE", idtmed[11], tubpar, 3);
  
  // --  YOKE 
  
  tubpar[0] = 0.;
  tubpar[1] = 55.;
  tubpar[2] = 945./2.;
  gMC->Gsvolu("YD2 ", "TUBE", idtmed[7], tubpar, 3);
  
  gMC->Gspos("YD2 ", 1, "ZDC ", 0., 0., tubpar[2] + zd2, 0, "ONLY");
  
  gMC->Gspos("MD2 ", 1, "YD2 ", -9.4, 0., 0., 0, "ONLY");
  gMC->Gspos("MD2 ", 2, "YD2 ",  9.4, 0., 0., 0, "ONLY");
  
  // -- END OF MAGNET DEFINITION 
}
  
//_____________________________________________________________________________
void AliZDCv1::CreateZDC()
{
  
  Float_t DimPb[6], DimVoid[6];
  
  Int_t *idtmed = fIdtmed->GetArray();

  // Parameters for hadronic calorimeters geometry
  // NB -> parameters used ONLY in CreateZDC()
  Float_t fDimZN[3] = {3.52, 3.52, 50.};  // Dimensions of neutron detector
  Float_t fGrvZN[3] = {0.03, 0.03, 50.};  // Grooves for neutron detector
  Float_t fGrvZP[3] = {0.04, 0.04, 75.};  // Grooves for proton detector
  Int_t   fDivZN[3] = {11, 11, 0};  	  // Division for neutron detector
  Int_t   fDivZP[3] = {7, 15, 0};  	  // Division for proton detector
  Int_t   fTowZN[2] = {2, 2};  		  // Tower for neutron detector
  Int_t   fTowZP[2] = {4, 1};  		  // Tower for proton detector

  // Parameters for EM calorimeter geometry
  // NB -> parameters used ONLY in CreateZDC()
  Float_t fDimZEMPb  = 0.15*(TMath::Sqrt(2.));  // z-dimension of the Pb slice
  Float_t fDimZEMAir = 0.001; 			// scotch
  Float_t fFibRadZEM = 0.0315; 			// External fiber radius (including cladding)
  Int_t   fDivZEM[3] = {92, 0, 20}; 		// Divisions for EM detector
  Float_t fDimZEM0 = 2*fDivZEM[2]*(fDimZEMPb+fDimZEMAir+fFibRadZEM*(TMath::Sqrt(2.)));
  Float_t fDimZEM[6] = {fDimZEM0, 3.5, 3.5, 45., 0., 0.}; // Dimensions of EM detector
  Float_t fFibZEM2 = fDimZEM[2]/TMath::Sin(fDimZEM[3]*kDegrad)-fFibRadZEM;
  Float_t fFibZEM[3] = {0., 0.0275, fFibZEM2};  // Fibers for EM calorimeter

  
  //-- Create calorimeters geometry
  
  // -------------------------------------------------------------------------------
  //--> Neutron calorimeter (ZN) 
  
  gMC->Gsvolu("ZNEU", "BOX ", idtmed[1], fDimZN, 3); // Passive material  
  gMC->Gsvolu("ZNF1", "TUBE", idtmed[3], fFibZN, 3); // Active material
  gMC->Gsvolu("ZNF2", "TUBE", idtmed[4], fFibZN, 3); 
  gMC->Gsvolu("ZNF3", "TUBE", idtmed[4], fFibZN, 3); 
  gMC->Gsvolu("ZNF4", "TUBE", idtmed[3], fFibZN, 3); 
  gMC->Gsvolu("ZNG1", "BOX ", idtmed[12], fGrvZN, 3); // Empty grooves 
  gMC->Gsvolu("ZNG2", "BOX ", idtmed[12], fGrvZN, 3); 
  gMC->Gsvolu("ZNG3", "BOX ", idtmed[12], fGrvZN, 3); 
  gMC->Gsvolu("ZNG4", "BOX ", idtmed[12], fGrvZN, 3); 
  
  // Divide ZNEU in towers (for hits purposes) 
  
  gMC->Gsdvn("ZNTX", "ZNEU", fTowZN[0], 1); // x-tower 
  gMC->Gsdvn("ZN1 ", "ZNTX", fTowZN[1], 2); // y-tower
  
  //-- Divide ZN1 in minitowers 
  //  fDivZN[0]= NUMBER OF FIBERS PER TOWER ALONG X-AXIS, 
  //  fDivZN[1]= NUMBER OF FIBERS PER TOWER ALONG Y-AXIS
  //  (4 fibres per minitower) 
  
  gMC->Gsdvn("ZNSL", "ZN1 ", fDivZN[1], 2); // Slices 
  gMC->Gsdvn("ZNST", "ZNSL", fDivZN[0], 1); // Sticks
  
  // --- Position the empty grooves in the sticks (4 grooves per stick)
  Float_t dx = fDimZN[0] / fDivZN[0] / 4.;
  Float_t dy = fDimZN[1] / fDivZN[1] / 4.;
  
  gMC->Gspos("ZNG1", 1, "ZNST", 0.-dx, 0.+dy, 0., 0, "ONLY");
  gMC->Gspos("ZNG2", 1, "ZNST", 0.+dx, 0.+dy, 0., 0, "ONLY");
  gMC->Gspos("ZNG3", 1, "ZNST", 0.-dx, 0.-dy, 0., 0, "ONLY");
  gMC->Gspos("ZNG4", 1, "ZNST", 0.+dx, 0.-dy, 0., 0, "ONLY");
  
  // --- Position the fibers in the grooves 
  gMC->Gspos("ZNF1", 1, "ZNG1", 0., 0., 0., 0, "ONLY");
  gMC->Gspos("ZNF2", 1, "ZNG2", 0., 0., 0., 0, "ONLY");
  gMC->Gspos("ZNF3", 1, "ZNG3", 0., 0., 0., 0, "ONLY");
  gMC->Gspos("ZNF4", 1, "ZNG4", 0., 0., 0., 0, "ONLY");
  
  // --- Position the neutron calorimeter in ZDC 
  gMC->Gspos("ZNEU", 1, "ZDC ", fPosZN[0], fPosZN[1], fPosZN[2] + fDimZN[2], 0, "ONLY");
  

  // -------------------------------------------------------------------------------
  //--> Proton calorimeter (ZP)  
  
  gMC->Gsvolu("ZPRO", "BOX ", idtmed[2], fDimZP, 3); // Passive material
  gMC->Gsvolu("ZPF1", "TUBE", idtmed[3], fFibZP, 3); // Active material
  gMC->Gsvolu("ZPF2", "TUBE", idtmed[4], fFibZP, 3); 
  gMC->Gsvolu("ZPF3", "TUBE", idtmed[4], fFibZP, 3); 
  gMC->Gsvolu("ZPF4", "TUBE", idtmed[3], fFibZP, 3); 
  gMC->Gsvolu("ZPG1", "BOX ", idtmed[12], fGrvZP, 3); // Empty grooves 
  gMC->Gsvolu("ZPG2", "BOX ", idtmed[12], fGrvZP, 3); 
  gMC->Gsvolu("ZPG3", "BOX ", idtmed[12], fGrvZP, 3); 
  gMC->Gsvolu("ZPG4", "BOX ", idtmed[12], fGrvZP, 3); 
    
  //-- Divide ZPRO in towers(for hits purposes) 
  
  gMC->Gsdvn("ZPTX", "ZPRO", fTowZP[0], 1); // x-tower 
  gMC->Gsdvn("ZP1 ", "ZPTX", fTowZP[1], 2); // y-tower
  
  
  //-- Divide ZP1 in minitowers 
  //  fDivZP[0]= NUMBER OF FIBERS ALONG X-AXIS PER MINITOWER, 
  //  fDivZP[1]= NUMBER OF FIBERS ALONG Y-AXIS PER MINITOWER
  //  (4 fiber per minitower) 
  
  gMC->Gsdvn("ZPSL", "ZP1 ", fDivZP[1], 2); // Slices 
  gMC->Gsdvn("ZPST", "ZPSL", fDivZP[0], 1); // Sticks
  
  // --- Position the empty grooves in the sticks (4 grooves per stick)
  dx = fDimZP[0] / fTowZP[0] / fDivZP[0] / 2.;
  dy = fDimZP[1] / fTowZP[1] / fDivZP[1] / 2.;
  
  gMC->Gspos("ZPG1", 1, "ZPST", 0.-dx, 0.+dy, 0., 0, "ONLY");
  gMC->Gspos("ZPG2", 1, "ZPST", 0.+dx, 0.+dy, 0., 0, "ONLY");
  gMC->Gspos("ZPG3", 1, "ZPST", 0.-dx, 0.-dy, 0., 0, "ONLY");
  gMC->Gspos("ZPG4", 1, "ZPST", 0.+dx, 0.-dy, 0., 0, "ONLY");
  
  // --- Position the fibers in the grooves 
  gMC->Gspos("ZPF1", 1, "ZPG1", 0., 0., 0., 0, "ONLY");
  gMC->Gspos("ZPF2", 1, "ZPG2", 0., 0., 0., 0, "ONLY");
  gMC->Gspos("ZPF3", 1, "ZPG3", 0., 0., 0., 0, "ONLY");
  gMC->Gspos("ZPF4", 1, "ZPG4", 0., 0., 0., 0, "ONLY");
  

  // --- Position the proton calorimeter in ZDC 
  gMC->Gspos("ZPRO", 1, "ZDC ", fPosZP[0], fPosZP[1], fPosZP[2] + fDimZP[2], 0, "ONLY");
    
  
  // -------------------------------------------------------------------------------
  // -> EM calorimeter (ZEM)  
  
  gMC->Gsvolu("ZEM ", "PARA", idtmed[10], fDimZEM, 6);

  Int_t irot1, irot2;
  
  gMC->Matrix(irot1,0.,0.,90.,90.,90.,180.); 		        // Rotation matrix 1  
  gMC->Matrix(irot2,180.,0.,90.,fDimZEM[3]+90.,90.,fDimZEM[3]); // Rotation matrix 2
//  printf("irot1 = %d, irot2 = %d \n", irot1, irot2);
  
  gMC->Gsvolu("ZEMF", "TUBE", idtmed[3], fFibZEM, 3); // Active material

  gMC->Gsdvn("ZETR", "ZEM ", fDivZEM[2], 1); 	     // Tranches 
  
  DimPb[0] = fDimZEMPb;			// Lead slices 
  DimPb[1] = fDimZEM[2];
  DimPb[2] = fDimZEM[1];
  DimPb[3] = 90.-fDimZEM[3];
  DimPb[4] = 0.;
  DimPb[5] = 0.;
  gMC->Gsvolu("ZEL0", "PARA", idtmed[5], DimPb, 6);
  gMC->Gsvolu("ZEL1", "PARA", idtmed[5], DimPb, 6);
  gMC->Gsvolu("ZEL2", "PARA", idtmed[5], DimPb, 6);
  
  // --- Position the lead slices in the tranche 
  Float_t zTran = fDimZEM[0]/fDivZEM[2]; 
  Float_t zTrPb = -zTran+fDimZEMPb;
  gMC->Gspos("ZEL0", 1, "ZETR", zTrPb, 0., 0., 0, "ONLY");
  gMC->Gspos("ZEL1", 1, "ZETR", fDimZEMPb, 0., 0., 0, "ONLY");
  
  // --- Vacuum zone (to be filled with fibres)
  DimVoid[0] = (zTran-2*fDimZEMPb)/2.;
  DimVoid[1] = fDimZEM[2];
  DimVoid[2] = fDimZEM[1];
  DimVoid[3] = 90.-fDimZEM[3];
  DimVoid[4] = 0.;
  DimVoid[5] = 0.;
  gMC->Gsvolu("ZEV0", "PARA", idtmed[10], DimVoid,6);
  gMC->Gsvolu("ZEV1", "PARA", idtmed[10], DimVoid,6);
  
  // --- Divide the vacuum slice into sticks along x axis
  gMC->Gsdvn("ZES0", "ZEV0", fDivZEM[0], 3); 
  gMC->Gsdvn("ZES1", "ZEV1", fDivZEM[0], 3); 
  
  // --- Positioning the fibers into the sticks
  gMC->Gspos("ZEMF", 1,"ZES0", 0., 0., 0., irot2, "ONLY");
  gMC->Gspos("ZEMF", 1,"ZES1", 0., 0., 0., irot2, "ONLY");
  
  // --- Positioning the vacuum slice into the tranche
  Float_t DisplFib = fDimZEM[1]/fDivZEM[0];
  gMC->Gspos("ZEV0", 1,"ZETR", -DimVoid[0], 0., 0., 0, "ONLY");
  gMC->Gspos("ZEV1", 1,"ZETR", -DimVoid[0]+zTran, 0., DisplFib, 0, "ONLY");

  // --- Positioning the ZEM into the ZDC - rotation for 90 degrees  
  gMC->Gspos("ZEM ", 1,"ZDC ", fPosZEM[0], fPosZEM[1], fPosZEM[2], irot1, "ONLY");
  
  // --- Adding last slice at the end of the EM calorimeter 
  Float_t zLastSlice = fPosZEM[2]+fDimZEMPb+fDimZEM[0];
  gMC->Gspos("ZEL2", 1,"ZDC ", fPosZEM[0], fPosZEM[1], zLastSlice, irot1, "ONLY");
  
}
 
//_____________________________________________________________________________
void AliZDCv1::DrawModule()
{
  //
  // Draw a shaded view of the Zero Degree Calorimeter version 1
  //

  // Set everything unseen
  gMC->Gsatt("*", "seen", -1);
  // 
  // Set ALIC mother transparent
  gMC->Gsatt("ALIC","SEEN",0);
  //
  // Set the volumes visible
  gMC->Gsatt("ZDC ","SEEN",0);
  gMC->Gsatt("QT01","SEEN",1);
  gMC->Gsatt("QT02","SEEN",1);
  gMC->Gsatt("QT03","SEEN",1);
  gMC->Gsatt("QT04","SEEN",1);
  gMC->Gsatt("QT05","SEEN",1);
  gMC->Gsatt("QT06","SEEN",1);
  gMC->Gsatt("QT07","SEEN",1);
  gMC->Gsatt("QT08","SEEN",1);
  gMC->Gsatt("QT09","SEEN",1);
  gMC->Gsatt("QT10","SEEN",1);
  gMC->Gsatt("QT11","SEEN",1);
  gMC->Gsatt("QT12","SEEN",1);
  gMC->Gsatt("QT13","SEEN",1);
  gMC->Gsatt("QT14","SEEN",1);
  gMC->Gsatt("QT15","SEEN",1);
  gMC->Gsatt("QT16","SEEN",1);
  gMC->Gsatt("QT17","SEEN",1);
  gMC->Gsatt("QT18","SEEN",1);
  gMC->Gsatt("QC01","SEEN",1);
  gMC->Gsatt("QC02","SEEN",1);
  gMC->Gsatt("QC03","SEEN",1);
  gMC->Gsatt("QC04","SEEN",1);
  gMC->Gsatt("QC05","SEEN",1);
  gMC->Gsatt("QTD1","SEEN",1);
  gMC->Gsatt("QTD2","SEEN",1);
  gMC->Gsatt("QTD3","SEEN",1);
  gMC->Gsatt("MQXL","SEEN",1);
  gMC->Gsatt("YMQL","SEEN",1);
  gMC->Gsatt("MQX ","SEEN",1);
  gMC->Gsatt("YMQ ","SEEN",1);
  gMC->Gsatt("ZQYX","SEEN",1);
  gMC->Gsatt("MD1 ","SEEN",1);
  gMC->Gsatt("MD1V","SEEN",1);
  gMC->Gsatt("YD1 ","SEEN",1);
  gMC->Gsatt("MD2 ","SEEN",1);
  gMC->Gsatt("YD2 ","SEEN",1);
  gMC->Gsatt("ZNEU","SEEN",0);
  gMC->Gsatt("ZNF1","SEEN",0);
  gMC->Gsatt("ZNF2","SEEN",0);
  gMC->Gsatt("ZNF3","SEEN",0);
  gMC->Gsatt("ZNF4","SEEN",0);
  gMC->Gsatt("ZNG1","SEEN",0);
  gMC->Gsatt("ZNG2","SEEN",0);
  gMC->Gsatt("ZNG3","SEEN",0);
  gMC->Gsatt("ZNG4","SEEN",0);
  gMC->Gsatt("ZNTX","SEEN",0);
  gMC->Gsatt("ZN1 ","COLO",4); 
  gMC->Gsatt("ZN1 ","SEEN",1);
  gMC->Gsatt("ZNSL","SEEN",0);
  gMC->Gsatt("ZNST","SEEN",0);
  gMC->Gsatt("ZPRO","SEEN",0);
  gMC->Gsatt("ZPF1","SEEN",0);
  gMC->Gsatt("ZPF2","SEEN",0);
  gMC->Gsatt("ZPF3","SEEN",0);
  gMC->Gsatt("ZPF4","SEEN",0);
  gMC->Gsatt("ZPG1","SEEN",0);
  gMC->Gsatt("ZPG2","SEEN",0);
  gMC->Gsatt("ZPG3","SEEN",0);
  gMC->Gsatt("ZPG4","SEEN",0);
  gMC->Gsatt("ZPTX","SEEN",0);
  gMC->Gsatt("ZP1 ","COLO",6); 
  gMC->Gsatt("ZP1 ","SEEN",1);
  gMC->Gsatt("ZPSL","SEEN",0);
  gMC->Gsatt("ZPST","SEEN",0);
  gMC->Gsatt("ZEM ","COLO",7); 
  gMC->Gsatt("ZEM ","SEEN",1);
  gMC->Gsatt("ZEMF","SEEN",0);
  gMC->Gsatt("ZETR","SEEN",0);
  gMC->Gsatt("ZEL0","SEEN",0);
  gMC->Gsatt("ZEL1","SEEN",0);
  gMC->Gsatt("ZEL2","SEEN",0);
  gMC->Gsatt("ZEV0","SEEN",0);
  gMC->Gsatt("ZEV1","SEEN",0);
  gMC->Gsatt("ZES0","SEEN",0);
  gMC->Gsatt("ZES1","SEEN",0);
  
  //
  gMC->Gdopt("hide", "on");
  gMC->Gdopt("shad", "on");
  gMC->Gsatt("*", "fill", 7);
  gMC->SetClipBox(".");
  gMC->SetClipBox("*", 0, 100, -100, 100, 12000, 16000);
  gMC->DefaultRange();
  gMC->Gdraw("alic", 40, 30, 0, 488, 220, .07, .07);
  gMC->Gdhead(1111, "Zero Degree Calorimeter Version 1");
  gMC->Gdman(18, 4, "MAN");
}

//_____________________________________________________________________________
void AliZDCv1::CreateMaterials()
{
  //
  // Create Materials for the Zero Degree Calorimeter
  //
  
  Int_t *idtmed = fIdtmed->GetArray();
  
  Float_t dens, ubuf[1], wmat[2], a[2], z[2], deemax = -1;
  Int_t i;
  
  // --- Store in UBUF r0 for nuclear radius calculation R=r0*A**1/3 

  // --- Tantalum -> ZN passive material
  ubuf[0] = 1.1;
  AliMaterial(1, "TANT", 180.95, 73., 16.65, .4, 11.9, ubuf, 1);
    
  // --- Tungsten 
//  ubuf[0] = 1.11;
//  AliMaterial(1, "TUNG", 183.85, 74., 19.3, .35, 10.3, ubuf, 1);
  
  // --- Brass (CuZn)  -> ZP passive material
  dens = 8.48;
  a[0] = 63.546;
  a[1] = 65.39;
  z[0] = 29.;
  z[1] = 30.;
  wmat[0] = .63;
  wmat[1] = .37;
  AliMixture(2, "BRASS               ", a, z, dens, 2, wmat);
  
  // --- SiO2 
  dens = 2.64;
  a[0] = 28.086;
  a[1] = 15.9994;
  z[0] = 14.;
  z[1] = 8.;
  wmat[0] = 1.;
  wmat[1] = 2.;
  AliMixture(3, "SIO2                ", a, z, dens, -2, wmat);  
  
  // --- Lead 
  ubuf[0] = 1.12;
  AliMaterial(5, "LEAD", 207.19, 82., 11.35, .56, 18.5, ubuf, 1);

  // --- Copper 
  ubuf[0] = 1.10;
  AliMaterial(6, "COPP", 63.54, 29., 8.96, 1.4, 0., ubuf, 1);
  
  // --- Iron (energy loss taken into account)
  ubuf[0] = 1.1;
  AliMaterial(7, "IRON", 55.85, 26., 7.87, 1.76, 0., ubuf, 1);
  
  // --- Iron (no energy loss)
  ubuf[0] = 1.1;
  AliMaterial(8, "IRON", 55.85, 26., 7.87, 1.76, 0., ubuf, 1);
  
  // --- Vacuum (no magnetic field) 
  AliMaterial(10, "VOID", 1e-16, 1e-16, 1e-16, 1e16, 1e16, ubuf,0);
  
  // --- Vacuum (with magnetic field) 
  AliMaterial(11, "VOIM", 1e-16, 1e-16, 1e-16, 1e16, 1e16, ubuf,0);
  
  // --- Air (no magnetic field)
  AliMaterial(12, "Air    $", 14.61, 7.3, .001205, 30420., 67500., ubuf, 0);
  
  // ---  Definition of tracking media: 
  
  // --- Tantalum = 1 ; 
  // --- Brass = 2 ; 
  // --- Fibers (SiO2) = 3 ; 
  // --- Fibers (SiO2) = 4 ; 
  // --- Lead = 5 ; 
  // --- Copper = 6 ; 
  // --- Iron (with energy loss) = 7 ; 
  // --- Iron (without energy loss) = 8 ; 
  // --- Vacuum (no field) = 10 
  // --- Vacuum (with field) = 11 
  // --- Air (no field) = 12 
  
  
  // --- Tracking media parameters 
  Float_t epsil  = .01, stmin=0.01, stemax = 1.;
//  Int_t   isxfld = gAlice->Field()->Integ();
//  Float_t fieldm = gAlice->Field()->Max();
  Float_t fieldm = 0., tmaxfd = 0.;
  Int_t   ifield = 0, isvolActive = 1, isvol = 0, inofld = 0;
  
  AliMedium(1, "ZTANT", 1, isvolActive, inofld, fieldm, tmaxfd, stemax, deemax, epsil, stmin);
//  AliMedium(1, "ZW", 1, isvolActive, inofld, fieldm, tmaxfd, stemax, deemax, epsil, stmin);
  AliMedium(2, "ZBRASS",2, isvolActive, inofld, fieldm, tmaxfd, stemax, deemax, epsil, stmin);
  AliMedium(3, "ZSIO2", 3, isvolActive, inofld, fieldm, tmaxfd, stemax, deemax, epsil, stmin);
  AliMedium(4, "ZQUAR", 3, isvolActive, inofld, fieldm, tmaxfd, stemax, deemax, epsil, stmin);
  AliMedium(5, "ZLEAD", 5, isvolActive, inofld, fieldm, tmaxfd, stemax, deemax, epsil, stmin);
//  AliMedium(6, "ZCOPP", 6, isvolActive, inofld, fieldm, tmaxfd, stemax, deemax, epsil, stmin);
//  AliMedium(7, "ZIRON", 7, isvolActive, inofld, fieldm, tmaxfd, stemax, deemax, epsil, stmin);
  AliMedium(6, "ZCOPP", 6, isvol, inofld, fieldm, tmaxfd, stemax, deemax, epsil, stmin);
  AliMedium(7, "ZIRON", 7, isvol, inofld, fieldm, tmaxfd, stemax, deemax, epsil, stmin);
  AliMedium(8, "ZIRONN",8, isvol, inofld, fieldm, tmaxfd, stemax, deemax, epsil, stmin);
  AliMedium(10,"ZVOID",10, isvol, inofld, fieldm, tmaxfd, stemax, deemax, epsil, stmin);
  AliMedium(12,"ZAIR", 12, 0, inofld, fieldm, tmaxfd, stemax,deemax, epsil, stmin);
  
  ifield =2;
  fieldm = 45.;
  AliMedium(11, "ZVOIM", 11, isvol, ifield, fieldm, tmaxfd, stemax, deemax, epsil, stmin);
  
  // Thresholds for showering in the ZDCs 
  i = 1; //tantalum
  gMC->Gstpar(idtmed[i], "CUTGAM", .001);
  gMC->Gstpar(idtmed[i], "CUTELE", .001);
  gMC->Gstpar(idtmed[i], "CUTNEU", .01);
  gMC->Gstpar(idtmed[i], "CUTHAD", .01);
  i = 2; //brass
  gMC->Gstpar(idtmed[i], "CUTGAM", .001);
  gMC->Gstpar(idtmed[i], "CUTELE", .001);
  gMC->Gstpar(idtmed[i], "CUTNEU", .01);
  gMC->Gstpar(idtmed[i], "CUTHAD", .01);
  i = 5; //lead
  gMC->Gstpar(idtmed[i], "CUTGAM", .001);
  gMC->Gstpar(idtmed[i], "CUTELE", .001);
  gMC->Gstpar(idtmed[i], "CUTNEU", .01);
  gMC->Gstpar(idtmed[i], "CUTHAD", .01);
  
  // Avoid too detailed showering in TDI 
  i = 6; //copper
  gMC->Gstpar(idtmed[i], "CUTGAM", .1);
  gMC->Gstpar(idtmed[i], "CUTELE", .1);
  gMC->Gstpar(idtmed[i], "CUTNEU", 1.);
  gMC->Gstpar(idtmed[i], "CUTHAD", 1.);
  
  // Avoid too detailed showering along the beam line 
  i = 7; //iron with energy loss (ZIRON)
  gMC->Gstpar(idtmed[i], "CUTGAM", .1);
  gMC->Gstpar(idtmed[i], "CUTELE", .1);
  gMC->Gstpar(idtmed[i], "CUTNEU", 1.);
  gMC->Gstpar(idtmed[i], "CUTHAD", 1.);
  
  // Avoid too detailed showering along the beam line 
  i = 8; //iron with energy loss (ZIRONN)
  gMC->Gstpar(idtmed[i], "CUTGAM", .1);
  gMC->Gstpar(idtmed[i], "CUTELE", .1);
  gMC->Gstpar(idtmed[i], "CUTNEU", 1.);
  gMC->Gstpar(idtmed[i], "CUTHAD", 1.);
  
  // Avoid interaction in fibers (only energy loss allowed) 
  i = 3; //fibers (ZSI02)
  gMC->Gstpar(idtmed[i], "DCAY", 0.);
  gMC->Gstpar(idtmed[i], "MULS", 0.);
  gMC->Gstpar(idtmed[i], "PFIS", 0.);
  gMC->Gstpar(idtmed[i], "MUNU", 0.);
  gMC->Gstpar(idtmed[i], "LOSS", 1.);
  gMC->Gstpar(idtmed[i], "PHOT", 0.);
  gMC->Gstpar(idtmed[i], "COMP", 0.);
  gMC->Gstpar(idtmed[i], "PAIR", 0.);
  gMC->Gstpar(idtmed[i], "BREM", 0.);
  gMC->Gstpar(idtmed[i], "DRAY", 0.);
  gMC->Gstpar(idtmed[i], "ANNI", 0.);
  gMC->Gstpar(idtmed[i], "HADR", 0.);
  i = 4; //fibers (ZQUAR)
  gMC->Gstpar(idtmed[i], "DCAY", 0.);
  gMC->Gstpar(idtmed[i], "MULS", 0.);
  gMC->Gstpar(idtmed[i], "PFIS", 0.);
  gMC->Gstpar(idtmed[i], "MUNU", 0.);
  gMC->Gstpar(idtmed[i], "LOSS", 1.);
  gMC->Gstpar(idtmed[i], "PHOT", 0.);
  gMC->Gstpar(idtmed[i], "COMP", 0.);
  gMC->Gstpar(idtmed[i], "PAIR", 0.);
  gMC->Gstpar(idtmed[i], "BREM", 0.);
  gMC->Gstpar(idtmed[i], "DRAY", 0.);
  gMC->Gstpar(idtmed[i], "ANNI", 0.);
  gMC->Gstpar(idtmed[i], "HADR", 0.);
  
  // Avoid interaction in void 
  i = 11; //void with field
  gMC->Gstpar(idtmed[i], "DCAY", 0.);
  gMC->Gstpar(idtmed[i], "MULS", 0.);
  gMC->Gstpar(idtmed[i], "PFIS", 0.);
  gMC->Gstpar(idtmed[i], "MUNU", 0.);
  gMC->Gstpar(idtmed[i], "LOSS", 0.);
  gMC->Gstpar(idtmed[i], "PHOT", 0.);
  gMC->Gstpar(idtmed[i], "COMP", 0.);
  gMC->Gstpar(idtmed[i], "PAIR", 0.);
  gMC->Gstpar(idtmed[i], "BREM", 0.);
  gMC->Gstpar(idtmed[i], "DRAY", 0.);
  gMC->Gstpar(idtmed[i], "ANNI", 0.);
  gMC->Gstpar(idtmed[i], "HADR", 0.);

  //
  fMedSensZN  = idtmed[1];  // Sensitive volume: ZN passive material
  fMedSensZP  = idtmed[2];  // Sensitive volume: ZP passive material
  fMedSensF1  = idtmed[3];  // Sensitive volume: fibres type 1
  fMedSensF2  = idtmed[4];  // Sensitive volume: fibres type 2
  fMedSensZEM = idtmed[5];  // Sensitive volume: ZEM passive material
//  fMedSensTDI = idtmed[6];  // Sensitive volume: TDI Cu shield
//  fMedSensPI  = idtmed[7];  // Sensitive volume: beam pipes
  fMedSensGR  = idtmed[12]; // Sensitive volume: air into the grooves
} 

//_____________________________________________________________________________
void AliZDCv1::Init()
{
 InitTables();
}

//_____________________________________________________________________________
void AliZDCv1::InitTables()
{
  Int_t k, j;

  char *lightfName1,*lightfName2,*lightfName3,*lightfName4,
       *lightfName5,*lightfName6,*lightfName7,*lightfName8;
  FILE *fp1, *fp2, *fp3, *fp4, *fp5, *fp6, *fp7, *fp8;

  //  --- Reading light tables for ZN 
  lightfName1 = gSystem->ExpandPathName("$ALICE/$ALICE_LEVEL/ZDC/light22620362207s");
  if((fp1 = fopen(lightfName1,"r")) == NULL){
     printf("Cannot open file fp1 \n");
     return;
  }
  lightfName2 = gSystem->ExpandPathName("$ALICE/$ALICE_LEVEL/ZDC/light22620362208s");
  if((fp2 = fopen(lightfName2,"r")) == NULL){
     printf("Cannot open file fp2 \n");
     return;
  }  
  lightfName3 = gSystem->ExpandPathName("$ALICE/$ALICE_LEVEL/ZDC/light22620362209s");
  if((fp3 = fopen(lightfName3,"r")) == NULL){
     printf("Cannot open file fp3 \n");
     return;
  }
  lightfName4 = gSystem->ExpandPathName("$ALICE/$ALICE_LEVEL/ZDC/light22620362210s");
  if((fp4 = fopen(lightfName4,"r")) == NULL){
     printf("Cannot open file fp4 \n");
     return;
  }
  
  for(k=0; k<fNalfan; k++){
     for(j=0; j<fNben; j++){
       fscanf(fp1,"%f",&fTablen[0][k][j]);
       fscanf(fp2,"%f",&fTablen[1][k][j]);
       fscanf(fp3,"%f",&fTablen[2][k][j]);
       fscanf(fp4,"%f",&fTablen[3][k][j]);
     } 
  }
  fclose(fp1);
  fclose(fp2);
  fclose(fp3);
  fclose(fp4);
  
  //  --- Reading light tables for ZP and ZEM
  lightfName5 = gSystem->ExpandPathName("$ALICE/$ALICE_LEVEL/ZDC/light22620552207s");
  if((fp5 = fopen(lightfName5,"r")) == NULL){
     printf("Cannot open file fp5 \n");
     return;
  }
  lightfName6 = gSystem->ExpandPathName("$ALICE/$ALICE_LEVEL/ZDC/light22620552208s");
  if((fp6 = fopen(lightfName6,"r")) == NULL){
     printf("Cannot open file fp6 \n");
     return;
  }
  lightfName7 = gSystem->ExpandPathName("$ALICE/$ALICE_LEVEL/ZDC/light22620552209s");
  if((fp7 = fopen(lightfName7,"r")) == NULL){
     printf("Cannot open file fp7 \n");
     return;
  }
  lightfName8 = gSystem->ExpandPathName("$ALICE/$ALICE_LEVEL/ZDC/light22620552210s");
  if((fp8 = fopen(lightfName8,"r")) == NULL){
     printf("Cannot open file fp8 \n");
     return;
  }
  
  for(k=0; k<fNalfap; k++){
     for(j=0; j<fNbep; j++){
       fscanf(fp5,"%f",&fTablep[0][k][j]);
       fscanf(fp6,"%f",&fTablep[1][k][j]);
       fscanf(fp7,"%f",&fTablep[2][k][j]);
       fscanf(fp8,"%f",&fTablep[3][k][j]);
     } 
  }
  fclose(fp5);
  fclose(fp6);
  fclose(fp7);
  fclose(fp8);
}
//_____________________________________________________________________________
void AliZDCv1::StepManager()
{
  //
  // Routine called at every step in the Zero Degree Calorimeters
  //

  Int_t j, vol[2], ibeta=0, ialfa, ibe, nphe;
  Float_t x[3], xdet[3], destep, hits[10], m, ekin, um[3], ud[3], be, radius, out;
  Float_t xalic[3], z, GuiEff, GuiPar[4]={0.31,-0.0004,0.0197,0.7958};
  TLorentzVector s, p;
  const char *knamed;

  for (j=0;j<10;j++) hits[j]=0;

  if((gMC->GetMedium() == fMedSensZN) || (gMC->GetMedium() == fMedSensZP) ||
     (gMC->GetMedium() == fMedSensGR) || (gMC->GetMedium() == fMedSensF1) ||
     (gMC->GetMedium() == fMedSensF2) || (gMC->GetMedium() == fMedSensZEM)){

//   --- This part is for no shower developement in beam pipe and TDI
//     (gMC->GetMedium() == fMedSensPI) || (gMC->GetMedium() == fMedSensTDI)){
       
  // If particle interacts with beam pipe -> return
//    if((gMC->GetMedium() == fMedSensPI) || (gMC->GetMedium() == fMedSensTDI)){ 
      // If option NoShower is set -> StopTrack
//      if(fNoShower==1) {
//	if(gMC->GetMedium() == fMedSensPI) {
//          knamed = gMC->CurrentVolName();
//          if((!strncmp(knamed,"MQ",2)) || (!strncmp(knamed,"YM",2)))  fpLostIT += 1;
//          if((!strncmp(knamed,"MD1",3))|| (!strncmp(knamed,"YD1",2))) fpLostD1 += 1;
//	}
//	if(gMC->GetMedium() == fMedSensTDI) fpLostTDI += 1;
//        gMC->StopTrack();
//	printf("\n	# of p lost in Inner Triplet = %d\n",fpLostIT);
//	printf("\n	# of p lost in D1  = %d\n",fpLostD1);
//	printf("\n	# of p lost in TDI = %d\n",fpLostTDI);
//      }
//      return;
//    }
  
  //Particle coordinates 
    gMC->TrackPosition(s);
    for(j=0; j<=2; j++){
       x[j] = s[j];
    }
    hits[0] = x[0];
    hits[1] = x[1];
    hits[2] = x[2];

  // Determine in which ZDC the particle is
    knamed = gMC->CurrentVolName();
    if(!strncmp(knamed,"ZN",2)){
      vol[0]=1;
    }
    else if(!strncmp(knamed,"ZP",2)){
      vol[0]=2;
    }
    else if(!strncmp(knamed,"ZE",2)){
      vol[0]=3;
    }
  
  // Determine in which quadrant the particle is
       
    if(vol[0]==1){	//Quadrant in ZN
      xdet[0] = x[0]-fPosZN[0];
      xdet[1] = x[1]-fPosZN[1];
      if((xdet[0]<=0.) && (xdet[1]>=0.))  vol[1]=1;
      if((xdet[0]>0.)  && (xdet[1]>0.))   vol[1]=2;
      if((xdet[0]<0.)  && (xdet[1]<0.))   vol[1]=3;
      if((xdet[0]>0.)  && (xdet[1]<0.))   vol[1]=4;
    }
    else if(vol[0]==2){	//Quadrant in ZP
      xdet[0] = x[0]-fPosZP[0];
      xdet[1] = x[1]-fPosZP[1];
      if(xdet[0]>fDimZP[0])xdet[0]=fDimZP[0]-0.01;
      if(xdet[0]<-fDimZP[0])xdet[0]=-fDimZP[0]+0.01;
      Float_t xqZP = xdet[0]/(fDimZP[0]/2);
      for(int i=1; i<=4; i++){
         if(xqZP>=(i-3) && xqZP<(i-2)){
 	   vol[1] = i;
 	   break;
 	 }
      }
    }
    else if(vol[0] == 3){	//ZEM has only 1 quadrant
      vol[1] = 1;
      xdet[0] = x[0]-fPosZEM[0];
      xdet[1] = x[1]-fPosZEM[1];
    }

  // Store impact point and kinetic energy of the ENTERING particle
    
//    if(Curtrack==Prim){
      if(gMC->IsTrackEntering()){
        //Particle energy
        gMC->TrackMomentum(p);
        hits[3] = p[3];
        // Impact point on ZDC  
        hits[4] = xdet[0];
        hits[5] = xdet[1];
	hits[6] = 0;
        hits[7] = 0;
        hits[8] = 0;
        hits[9] = 0;

//	  Int_t PcID = gMC->TrackPid();
//	  printf("Pc ID -> %d\n",PcID);
	AddHit(gAlice->CurrentTrack(), vol, hits);
	
	if(fNoShower==1){
//	  fpDetected += 1;
	  gMC->StopTrack();
//	  printf("\n	# of detected p = %d\n",fpDetected);
	  return;
	}
      }
//    } // Curtrack IF
             
      // Charged particles -> Energy loss
      if((destep=gMC->Edep())){
         if(gMC->IsTrackStop()){
           gMC->TrackMomentum(p);
	   m = gMC->TrackMass();
	   ekin = p[3]-m;
	   hits[9] = ekin;
	   hits[7] = 0.;
	   hits[8] = 0.;
	   AddHit(gAlice->CurrentTrack(), vol, hits);
	   }
	 else{
	   hits[9] = destep;
	   hits[7] = 0.;
	   hits[8] = 0.;
	   AddHit(gAlice->CurrentTrack(), vol, hits);
	   }
//	 printf(" Dep. E = %f \n",hits[9]);
      }
  }// NB -> Questa parentesi (chiude il primo IF) io la sposterei al fondo!???


  // *** Light production in fibres 
  if((gMC->GetMedium() == fMedSensF1) || (gMC->GetMedium() == fMedSensF2)){

     //Select charged particles
     if((destep=gMC->Edep())){

       // Particle velocity
       gMC->TrackMomentum(p);
       Float_t ptot=TMath::Sqrt(p[0]*p[0]+p[1]*p[1]+p[2]*p[2]);
       Float_t beta =  ptot/p[3];
       if(beta<0.67){
         return;
       }
       else if((beta>=0.67) && (beta<=0.75)){
         ibeta = 0;
       }
       if((beta>0.75)  && (beta<=0.85)){
         ibeta = 1;
       }
       if((beta>0.85)  && (beta<=0.95)){
         ibeta = 2;
       }
       if(beta>0.95){
         ibeta = 3;
       }
 
       // Angle between particle trajectory and fibre axis
       // 1 -> Momentum directions
       um[0] = p[0]/ptot;
       um[1] = p[1]/ptot;
       um[2] = p[2]/ptot;
       gMC->Gmtod(um,ud,2);
       // 2 -> Angle < limit angle
       Double_t alfar = TMath::ACos(ud[2]);
       Double_t alfa = alfar*kRaddeg;
       if(alfa>=110.) return;
       ialfa = Int_t(1.+alfa/2.);
 
       // Distance between particle trajectory and fibre axis
       gMC->TrackPosition(s);
       for(j=0; j<=2; j++){
   	  x[j] = s[j];
       }
       gMC->Gmtod(x,xdet,1);
       if(TMath::Abs(ud[0])>0.00001){
         Float_t dcoeff = ud[1]/ud[0];
         be = TMath::Abs((xdet[1]-dcoeff*xdet[0])/TMath::Sqrt(dcoeff*dcoeff+1.));
       }
       else{
         be = TMath::Abs(ud[0]);
       }
 
       if((vol[0]==1)){
         radius = fFibZN[1];
       }
       else if((vol[0]==2)){
         radius = fFibZP[1];
       }
       ibe = Int_t(be*1000.+1);
 
       //Looking into the light tables 
       Float_t charge = gMC->TrackCharge();
       
       if((vol[0]==1)) {	// (1)  ZN fibres
         if(ibe>fNben) ibe=fNben;
         out =  charge*charge*fTablen[ibeta][ialfa][ibe];
	 nphe = gRandom->Poisson(out);
//	 printf("ZN --- ibeta = %d, ialfa = %d, ibe = %d"
//	        "	-> out = %f, nphe = %d\n", ibeta, ialfa, ibe, out, nphe);
	 if(gMC->GetMedium() == fMedSensF1){
	   hits[7] = nphe;  	//fLightPMQ
	   hits[8] = 0;
	   hits[9] = 0;
	   AddHit(gAlice->CurrentTrack(), vol, hits);
	 }
	 else{
	   hits[7] = 0;
	   hits[8] = nphe;	//fLightPMC
	   hits[9] = 0;
	   AddHit(gAlice->CurrentTrack(), vol, hits);
	 }
       } 
       else if((vol[0]==2)) {	// (2) ZP fibres
         if(ibe>fNbep) ibe=fNbep;
         out =  charge*charge*fTablep[ibeta][ialfa][ibe];
	 nphe = gRandom->Poisson(out);
//	 printf("ZP --- ibeta = %d, ialfa = %d, ibe = %d"
//	        "	-> out = %f, nphe = %d\n", ibeta, ialfa, ibe, out, nphe);
	 if(gMC->GetMedium() == fMedSensF1){
	   hits[7] = nphe;  	//fLightPMQ
	   hits[8] = 0;
	   hits[9] = 0;
	   AddHit(gAlice->CurrentTrack(), vol, hits);
	 }
	 else{
	   hits[7] = 0;
	   hits[8] = nphe;	//fLightPMC
	   hits[9] = 0;
	   AddHit(gAlice->CurrentTrack(), vol, hits);
	 }
       } 
       else if((vol[0]==3)) {	// (3) ZEM fibres
         if(ibe>fNbep) ibe=fNbep;
         out =  charge*charge*fTablep[ibeta][ialfa][ibe];
	 gMC->TrackPosition(s);
         for(j=0; j<=2; j++){
            xalic[j] = s[j];
         }
	 // z-coordinate from ZEM front face 
	 // NB-> fPosZEM[2]+fZEMLength = -1000.+2*10.3 = 979.69 cm
	 z = -xalic[2]+fPosZEM[2]+2*fZEMLength-xalic[1];
//	 z = xalic[2]-fPosZEM[2]-fZEMLength-xalic[1]*(TMath::Tan(45.*kDegrad));
//         printf("\n	fPosZEM[2]+2*fZEMLength = %f", fPosZEM[2]+2*fZEMLength);
	 GuiEff = GuiPar[0]*(GuiPar[1]*z*z+GuiPar[2]*z+GuiPar[3]);
//         printf("\n	xalic[0] = %f	xalic[1] = %f	xalic[2] = %f	z = %f	\n",
//	        xalic[0],xalic[1],xalic[2],z);
	 out = out*GuiEff;
	 nphe = gRandom->Poisson(out);
//         printf("	out*GuiEff = %f	nphe = %d", out, nphe);
//	 printf("ZEM --- ibeta = %d, ialfa = %d, ibe = %d"
//	        "	-> out = %f, nphe = %d\n", ibeta, ialfa, ibe, out, nphe);
	 hits[7] = 0;  	
	 hits[8] = nphe;	//fLightPMC
	 hits[9] = 0;
	 AddHit(gAlice->CurrentTrack(), vol, hits);
       }
     }
   }
}

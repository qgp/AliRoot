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
//  TRD geometry for the spaceframe without holes                            //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include <TVirtualMC.h>

#include "AliTRDgeometryFull.h"

ClassImp(AliTRDgeometryFull)

//_____________________________________________________________________________
AliTRDgeometryFull::AliTRDgeometryFull():AliTRDgeometry()
{
  //
  // AliTRDgeometryFull default constructor
  //

  Init();

}

//_____________________________________________________________________________
AliTRDgeometryFull::~AliTRDgeometryFull()
{
  //
  // AliTRDgeometryFull destructor
  //

}

//_____________________________________________________________________________
void AliTRDgeometryFull::Init()
{
  //
  // Initializes the geometry parameter
  //

  Int_t icham;
  Int_t iplan;

  fPHOShole = kFALSE;
  fRICHhole = kFALSE;

  // The outer lengths of the chambers for the sectors with holes for the PHOS
  Float_t lengthPH[kNplan][kNcham] = { { 124.0, 117.0,   0.0, 117.0, 124.0 }
				     , { 131.0, 124.0,   0.0, 124.0, 131.0 }
				     , { 138.0, 131.0,   0.0, 131.0, 138.0 }
				     , { 145.0, 138.0,   0.0, 138.0, 145.0 }
				     , { 147.0, 140.0,   0.0, 140.0, 147.0 }
				     , { 147.0, 140.0,   0.0, 140.0, 147.0 } };

  // The outer lengths of the chambers for the sectors with holes for the RICH
  Float_t lengthRH[kNplan][kNcham] = { {  87.5,   0.0,   0.0,   0.0,  87.5 }
				     , { 101.5,   0.0,   0.0,   0.0, 101.5 }
				     , { 115.5,   0.0,   0.0,   0.0, 115.5 }
				     , { 129.5,   0.0,   0.0,   0.0, 129.5 }
				     , { 133.5,   0.0,   0.0,   0.0, 133.5 }
				     , { 133.5,   0.0,   0.0,   0.0, 133.5 } };

  for (icham = 0; icham < kNcham; icham++) {
    for (iplan = 0; iplan < kNplan; iplan++) {
      fClengthPH[iplan][icham] = lengthPH[iplan][icham];
      fClengthRH[iplan][icham] = lengthRH[iplan][icham];
    }
  }

}

//_____________________________________________________________________________
void AliTRDgeometryFull::CreateGeometry(Int_t *idtmed)
{
  //
  // Create the TRD geometry without hole
  //
  //
  // Names of the TRD volumina (xx = detector number):
  //
  //      Lower part of the readout chambers (gas volume + radiator)
  //
  //        UAxx    Aluminum frames             (Al)
  //        UBxx    G10 frames                  (C)
  //        UCxx    Inner volumes               (Air)
  //
  //      Upper part of the readout chambers (readout plane + fee)
  //
  //        UDxx    G10 frames                  (C)
  //        UExx    Inner volumes of the G10    (Air)
  //        UFxx    Aluminum frames             (Al)
  //        UGxx    Inner volumes of the Al     (Air)
  //
  //      Inner material layers
  //
  //        UHxx    Radiator                    (Rohacell)
  //        UIxx    Entrance window             (Mylar)
  //        UJxx    Drift volume                (Xe/CO2)
  //        UKxx    Amplification volume        (Xe/CO2)
  //        ULxx    Pad plane                   (Cu)
  //        UMxx    Support structure           (Rohacell)
  //        UNxx    FEE + signal lines          (Cu)
  //        UOxx    Cooling device              (Al)
  //        UPxx    Cooling device              (Water)
  //

  const Int_t kNdet    = kNplan * kNcham;

  const Int_t kNparTrd = 4;
  const Int_t kNparCha = 3;

  Float_t xpos, ypos, zpos;

  Float_t parTrd[kNparTrd];
  Float_t parCha[kNparCha];

  Char_t  cTagV[5];
  Char_t  cTagM[5];

  AliTRDgeometry::CreateGeometry(idtmed);

  // The TRD mother volume for one sector (Air), full length in z-direction
  parTrd[0] = fgkSwidth1/2.;
  parTrd[1] = fgkSwidth2/2.;
  parTrd[2] = fgkSlenTR1/2.;
  parTrd[3] = fgkSheight/2.;
  gMC->Gsvolu("UTR1","TRD1",idtmed[1302-1],parTrd,kNparTrd);

  // The TRD mother volume for one sector (Air), leaving hole for PHOS
  if (fPHOShole) {
    gMC->Gsvolu("UTR2","TRD1",idtmed[1302-1],parTrd,kNparTrd);
  }

  // The TRD mother volume for one sector (Air), leaving hole for RICH
  if (fRICHhole) {
    gMC->Gsvolu("UTR3","TRD1",idtmed[1302-1],parTrd,kNparTrd);
  }  

  for (Int_t icham = 0; icham < kNcham; icham++) {
    for (Int_t iplan = 0; iplan < kNplan; iplan++) {  

      Int_t iDet = GetDetectorSec(iplan,icham);

      // The lower part of the readout chambers (gas volume + radiator) 
      // The aluminum frames 
      sprintf(cTagV,"UA%02d",iDet);
      parCha[0] = fCwidth[iplan]/2.;
      parCha[1] = fClength[iplan][icham]/2. - fgkHspace/2.;
      parCha[2] = fgkCraH/2. + fgkCdrH/2.;
      gMC->Gsvolu(cTagV,"BOX ",idtmed[1301-1],parCha,kNparCha);
      // The G10 frames 
      sprintf(cTagV,"UB%02d",iDet);
      parCha[0] = fCwidth[iplan]/2. - fgkCalT; 
      parCha[1] = -1.;
      parCha[2] = -1.;
      gMC->Gsvolu(cTagV,"BOX ",idtmed[1307-1],parCha,kNparCha);
      // The inner part (air)
      sprintf(cTagV,"UC%02d",iDet);
      parCha[0] = fCwidth[iplan]/2. - fgkCalT - fgkCclsT; 
      parCha[1] = fClength[iplan][icham]/2. - fgkHspace/2.- fgkCclfT;
      parCha[2] = -1.;
      gMC->Gsvolu(cTagV,"BOX ",idtmed[1302-1],parCha,kNparCha);
      if (fPHOShole) {
        if (fClengthPH[iplan][icham] > 0.0) {
          // The aluminum frames 
          sprintf(cTagV,"UA%02d",iDet+kNdet);
          parCha[0] = fCwidth[iplan]/2.;
          parCha[1] = fClengthPH[iplan][icham]/2. - fgkHspace/2.;
          parCha[2] = fgkCraH/2. + fgkCdrH/2.;
          gMC->Gsvolu(cTagV,"BOX ",idtmed[1301-1],parCha,kNparCha);
          // The G10 frames 
          sprintf(cTagV,"UB%02d",iDet+kNdet);
          parCha[0] = fCwidth[iplan]/2. - fgkCalT; 
          parCha[1] = -1.;
          parCha[2] = -1.;
          gMC->Gsvolu(cTagV,"BOX ",idtmed[1307-1],parCha,kNparCha);
          // The inner part (air)
          sprintf(cTagV,"UC%02d",iDet+kNdet);
          parCha[0] = fCwidth[iplan]/2. - fgkCalT - fgkCclsT; 
          parCha[1] = fClengthPH[iplan][icham]/2. - fgkHspace/2.- fgkCclfT;
          parCha[2] = -1.;
          gMC->Gsvolu(cTagV,"BOX ",idtmed[1302-1],parCha,kNparCha);
	}
      }
      if (fRICHhole) {
        if (fClengthRH[iplan][icham] > 0.0) {
          // The aluminum frames 
          sprintf(cTagV,"UA%02d",iDet+2*kNdet);
          parCha[0] = fCwidth[iplan]/2.;
          parCha[1] = fClengthRH[iplan][icham]/2. - fgkHspace/2.;
          parCha[2] = fgkCraH/2. + fgkCdrH/2.;
          gMC->Gsvolu(cTagV,"BOX ",idtmed[1301-1],parCha,kNparCha);
          // The G10 frames 
          sprintf(cTagV,"UB%02d",iDet+2*kNdet);
          parCha[0] = fCwidth[iplan]/2. - fgkCalT; 
          parCha[1] = -1.;
          parCha[2] = -1.;
          gMC->Gsvolu(cTagV,"BOX ",idtmed[1307-1],parCha,kNparCha);
          // The inner part (air)
          sprintf(cTagV,"UC%02d",iDet+2*kNdet);
          parCha[0] = fCwidth[iplan]/2. - fgkCalT - fgkCclsT; 
          parCha[1] = fClengthRH[iplan][icham]/2. - fgkHspace/2.- fgkCclfT;
          parCha[2] = -1.;
          gMC->Gsvolu(cTagV,"BOX ",idtmed[1302-1],parCha,kNparCha);
	}
      }

      // The upper part of the readout chambers (readout plane + fee)
      // The G10 frames
      sprintf(cTagV,"UD%02d",iDet);
      parCha[0] = fCwidth[iplan]/2. + fgkCroW;
      parCha[1] = fClength[iplan][icham]/2. - fgkHspace/2.;
      parCha[2] = fgkCamH/2.;
      gMC->Gsvolu(cTagV,"BOX ",idtmed[1307-1],parCha,kNparCha);
      // The inner part of the G10 frame (air)
      sprintf(cTagV,"UE%02d",iDet);
      parCha[0] = fCwidth[iplan]/2. + fgkCroW - fgkCcuT; 
      parCha[1] = fClength[iplan][icham]/2. - fgkHspace/2.- fgkCcuT;
      parCha[2] = -1.;
      gMC->Gsvolu(cTagV,"BOX ",idtmed[1302-1],parCha,kNparCha);
      // The aluminum frames
      sprintf(cTagV,"UF%02d",iDet);
      parCha[0] = fCwidth[iplan]/2. + fgkCroW;
      parCha[1] = fClength[iplan][icham]/2. - fgkHspace/2.;
      parCha[2] = fgkCroH/2.;
      gMC->Gsvolu(cTagV,"BOX ",idtmed[1301-1],parCha,kNparCha);
      // The inner part of the aluminum frames
      sprintf(cTagV,"UG%02d",iDet);
      parCha[0] = fCwidth[iplan]/2. + fgkCroW - fgkCauT; 
      parCha[1] = fClength[iplan][icham]/2. - fgkHspace/2.- fgkCauT;
      parCha[2] = -1.;
      gMC->Gsvolu(cTagV,"BOX ",idtmed[1302-1],parCha,kNparCha);
      if (fPHOShole) {
        if (fClengthPH[iplan][icham] > 0.0) {
          sprintf(cTagV,"UD%02d",iDet+kNdet);
          parCha[0] = fCwidth[iplan]/2. + fgkCroW;
          parCha[1] = fClengthPH[iplan][icham]/2. - fgkHspace/2.;
          parCha[2] = fgkCamH/2.;
          gMC->Gsvolu(cTagV,"BOX ",idtmed[1307-1],parCha,kNparCha);
          // The inner part of the G10 frame (air)
          sprintf(cTagV,"UE%02d",iDet+kNdet);
          parCha[0] = fCwidth[iplan]/2. + fgkCroW - fgkCcuT; 
          parCha[1] = fClengthPH[iplan][icham]/2. - fgkHspace/2.- fgkCcuT;
          parCha[2] = -1.;
          gMC->Gsvolu(cTagV,"BOX ",idtmed[1302-1],parCha,kNparCha);
          // The aluminum frames
          sprintf(cTagV,"UF%02d",iDet+kNdet);
          parCha[0] = fCwidth[iplan]/2. + fgkCroW;
          parCha[1] = fClengthPH[iplan][icham]/2. - fgkHspace/2.;
          parCha[2] = fgkCroH/2.;
          gMC->Gsvolu(cTagV,"BOX ",idtmed[1301-1],parCha,kNparCha);
          // The inner part of the aluminum frames
          sprintf(cTagV,"UG%02d",iDet+kNdet);
          parCha[0] = fCwidth[iplan]/2. + fgkCroW - fgkCauT; 
          parCha[1] = fClengthPH[iplan][icham]/2. - fgkHspace/2.- fgkCauT;
          parCha[2] = -1.;
          gMC->Gsvolu(cTagV,"BOX ",idtmed[1302-1],parCha,kNparCha);
	}
      }
      if (fRICHhole) {
        if (fClengthRH[iplan][icham] > 0.0) {
          sprintf(cTagV,"UD%02d",iDet+2*kNdet);
          parCha[0] = fCwidth[iplan]/2. + fgkCroW;
          parCha[1] = fClengthRH[iplan][icham]/2. - fgkHspace/2.;
          parCha[2] = fgkCamH/2.;
          gMC->Gsvolu(cTagV,"BOX ",idtmed[1307-1],parCha,kNparCha);
          // The inner part of the G10 frame (air)
          sprintf(cTagV,"UE%02d",iDet+2*kNdet);
          parCha[0] = fCwidth[iplan]/2. + fgkCroW - fgkCcuT; 
          parCha[1] = fClengthRH[iplan][icham]/2. - fgkHspace/2.- fgkCcuT;
          parCha[2] = -1.;
          gMC->Gsvolu(cTagV,"BOX ",idtmed[1302-1],parCha,kNparCha);
          // The aluminum frames
          sprintf(cTagV,"UF%02d",iDet+2*kNdet);
          parCha[0] = fCwidth[iplan]/2. + fgkCroW;
          parCha[1] = fClengthRH[iplan][icham]/2. - fgkHspace/2.;
          parCha[2] = fgkCroH/2.;
          gMC->Gsvolu(cTagV,"BOX ",idtmed[1301-1],parCha,kNparCha);
          // The inner part of the aluminum frames
          sprintf(cTagV,"UG%02d",iDet+2*kNdet);
          parCha[0] = fCwidth[iplan]/2. + fgkCroW - fgkCauT; 
          parCha[1] = fClengthRH[iplan][icham]/2. - fgkHspace/2.- fgkCauT;
          parCha[2] = -1.;
          gMC->Gsvolu(cTagV,"BOX ",idtmed[1302-1],parCha,kNparCha);
	}
      }

      // The material layers inside the chambers
      parCha[0] = -1.;
      parCha[1] = -1.;
      // Rohacell layer (radiator)
      parCha[2] = fgkRaThick/2;
      sprintf(cTagV,"UH%02d",iDet);
      gMC->Gsvolu(cTagV,"BOX ",idtmed[1315-1],parCha,kNparCha);
      // Mylar layer (entrance window + HV cathode) 
      parCha[2] = fgkMyThick/2;
      sprintf(cTagV,"UI%02d",iDet);
      gMC->Gsvolu(cTagV,"BOX ",idtmed[1308-1],parCha,kNparCha);
      // Xe/Isobutane layer (drift volume) 
      parCha[2] = fgkDrThick/2.;
      sprintf(cTagV,"UJ%02d",iDet);
      gMC->Gsvolu(cTagV,"BOX ",idtmed[1309-1],parCha,kNparCha);
      // Xe/Isobutane layer (amplification volume)
      parCha[2] = fgkAmThick/2.;
      sprintf(cTagV,"UK%02d",iDet);
      gMC->Gsvolu(cTagV,"BOX ",idtmed[1309-1],parCha,kNparCha);  
      // Cu layer (pad plane)
      parCha[2] = fgkCuThick/2;
      sprintf(cTagV,"UL%02d",iDet);
      gMC->Gsvolu(cTagV,"BOX ",idtmed[1305-1],parCha,kNparCha);
      // G10 layer (support structure / honeycomb)
      parCha[2] = fgkSuThick/2;
      sprintf(cTagV,"UM%02d",iDet);
      gMC->Gsvolu(cTagV,"BOX ",idtmed[1313-1],parCha,kNparCha);
      // Cu layer (FEE + signal lines)
      parCha[2] = fgkFeThick/2;
      sprintf(cTagV,"UN%02d",iDet);
      gMC->Gsvolu(cTagV,"BOX ",idtmed[1305-1],parCha,kNparCha);
      // Al layer (cooling devices)
      parCha[2] = fgkCoThick/2;
      sprintf(cTagV,"UO%02d",iDet);
      gMC->Gsvolu(cTagV,"BOX ",idtmed[1301-1],parCha,kNparCha);
      // Water layer (cooling)
      parCha[2] = fgkWaThick/2;
      sprintf(cTagV,"UP%02d",iDet);
      gMC->Gsvolu(cTagV,"BOX ",idtmed[1314-1],parCha,kNparCha);
      if (fPHOShole) {
        if (fClengthPH[iplan][icham] > 0.0) {
          // Rohacell layer (radiator)
          parCha[2] = fgkRaThick/2;
          sprintf(cTagV,"UH%02d",iDet+kNdet);
          gMC->Gsvolu(cTagV,"BOX ",idtmed[1315-1],parCha,kNparCha);
          // Mylar layer (entrance window + HV cathode) 
          parCha[2] = fgkMyThick/2;
          sprintf(cTagV,"UI%02d",iDet+kNdet);
          gMC->Gsvolu(cTagV,"BOX ",idtmed[1308-1],parCha,kNparCha);
          // Xe/Isobutane layer (drift volume) 
          parCha[2] = fgkDrThick/2.;
          sprintf(cTagV,"UJ%02d",iDet+kNdet);
          gMC->Gsvolu(cTagV,"BOX ",idtmed[1309-1],parCha,kNparCha);
          // Xe/Isobutane layer (amplification volume)
          parCha[2] = fgkAmThick/2.;
          sprintf(cTagV,"UK%02d",iDet+kNdet);
          gMC->Gsvolu(cTagV,"BOX ",idtmed[1309-1],parCha,kNparCha);  
          // Cu layer (pad plane)
          parCha[2] = fgkCuThick/2;
          sprintf(cTagV,"UL%02d",iDet+kNdet);
          gMC->Gsvolu(cTagV,"BOX ",idtmed[1305-1],parCha,kNparCha);
          // G10 layer (support structure / honeycomb)
          parCha[2] = fgkSuThick/2;
          sprintf(cTagV,"UM%02d",iDet+kNdet);
          gMC->Gsvolu(cTagV,"BOX ",idtmed[1313-1],parCha,kNparCha);
          // Cu layer (FEE + signal lines)
          parCha[2] = fgkFeThick/2;
          sprintf(cTagV,"UN%02d",iDet+kNdet);
          gMC->Gsvolu(cTagV,"BOX ",idtmed[1305-1],parCha,kNparCha);
          // Al layer (cooling devices)
          parCha[2] = fgkCoThick/2;
          sprintf(cTagV,"UO%02d",iDet+kNdet);
          gMC->Gsvolu(cTagV,"BOX ",idtmed[1301-1],parCha,kNparCha);
          // Water layer (cooling)
          parCha[2] = fgkWaThick/2;
          sprintf(cTagV,"UP%02d",iDet+kNdet);
          gMC->Gsvolu(cTagV,"BOX ",idtmed[1314-1],parCha,kNparCha);
	}
      }
      if (fRICHhole) {
        if (fClengthRH[iplan][icham] > 0.0) {
          // Rohacell layer (radiator)
          parCha[2] = fgkRaThick/2;
          sprintf(cTagV,"UH%02d",iDet+2*kNdet);
          gMC->Gsvolu(cTagV,"BOX ",idtmed[1315-1],parCha,kNparCha);
          // Mylar layer (entrance window + HV cathode) 
          parCha[2] = fgkMyThick/2;
          sprintf(cTagV,"UI%02d",iDet+2*kNdet);
          gMC->Gsvolu(cTagV,"BOX ",idtmed[1308-1],parCha,kNparCha);
          // Xe/Isobutane layer (drift volume) 
          parCha[2] = fgkDrThick/2.;
          sprintf(cTagV,"UJ%02d",iDet+2*kNdet);
          gMC->Gsvolu(cTagV,"BOX ",idtmed[1309-1],parCha,kNparCha);
          // Xe/Isobutane layer (amplification volume)
          parCha[2] = fgkAmThick/2.;
          sprintf(cTagV,"UK%02d",iDet+2*kNdet);
          gMC->Gsvolu(cTagV,"BOX ",idtmed[1309-1],parCha,kNparCha);  
          // Cu layer (pad plane)
          parCha[2] = fgkCuThick/2;
          sprintf(cTagV,"UL%02d",iDet+2*kNdet);
          gMC->Gsvolu(cTagV,"BOX ",idtmed[1305-1],parCha,kNparCha);
          // G10 layer (support structure / honeycomb)
          parCha[2] = fgkSuThick/2;
          sprintf(cTagV,"UM%02d",iDet+2*kNdet);
          gMC->Gsvolu(cTagV,"BOX ",idtmed[1313-1],parCha,kNparCha);
          // Cu layer (FEE + signal lines)
          parCha[2] = fgkFeThick/2;
          sprintf(cTagV,"UN%02d",iDet+2*kNdet);
          gMC->Gsvolu(cTagV,"BOX ",idtmed[1305-1],parCha,kNparCha);
          // Al layer (cooling devices)
          parCha[2] = fgkCoThick/2;
          sprintf(cTagV,"UO%02d",iDet+2*kNdet);
          gMC->Gsvolu(cTagV,"BOX ",idtmed[1301-1],parCha,kNparCha);
          // Water layer (cooling)
          parCha[2] = fgkWaThick/2;
          sprintf(cTagV,"UP%02d",iDet+2*kNdet);
          gMC->Gsvolu(cTagV,"BOX ",idtmed[1314-1],parCha,kNparCha);
	}
      }

      // Position the layers in the chambers
      xpos = 0;
      ypos = 0;
      // Lower part
      // Rohacell layer (radiator)
      zpos = fgkRaZpos;
      sprintf(cTagV,"UH%02d",iDet);
      sprintf(cTagM,"UC%02d",iDet);
      gMC->Gspos(cTagV,1,cTagM,xpos,ypos,zpos,0,"ONLY");
      // Mylar layer (entrance window + HV cathode)   
      zpos = fgkMyZpos;
      sprintf(cTagV,"UI%02d",iDet);
      sprintf(cTagM,"UC%02d",iDet);
      gMC->Gspos(cTagV,1,cTagM,xpos,ypos,zpos,0,"ONLY");
      // Xe/Isobutane layer (drift volume) 
      zpos = fgkDrZpos;
      sprintf(cTagV,"UJ%02d",iDet);
      sprintf(cTagM,"UC%02d",iDet);
      gMC->Gspos(cTagV,1,cTagM,xpos,ypos,zpos,0,"ONLY");
      // Upper part
      // Xe/Isobutane layer (amplification volume)
      zpos = fgkAmZpos;
      sprintf(cTagV,"UK%02d",iDet);
      sprintf(cTagM,"UE%02d",iDet);
      gMC->Gspos(cTagV,1,cTagM,xpos,ypos,zpos,0,"ONLY");
      // Readout part
      // Cu layer (pad plane)
      zpos = fgkCuZpos; 
      sprintf(cTagV,"UL%02d",iDet);
      sprintf(cTagM,"UG%02d",iDet);
      gMC->Gspos(cTagV,1,cTagM,xpos,ypos,zpos,0,"ONLY");
      // G10 layer (support structure)
      zpos = fgkSuZpos;
      sprintf(cTagV,"UM%02d",iDet);
      sprintf(cTagM,"UG%02d",iDet);
      gMC->Gspos(cTagV,1,cTagM,xpos,ypos,zpos,0,"ONLY");
      // Cu layer (FEE + signal lines)
      zpos = fgkFeZpos; 
      sprintf(cTagV,"UN%02d",iDet);
      sprintf(cTagM,"UG%02d",iDet);
      gMC->Gspos(cTagV,1,cTagM,xpos,ypos,zpos,0,"ONLY");
      // Al layer (cooling devices)
      zpos = fgkCoZpos;
      sprintf(cTagV,"UO%02d",iDet);
      sprintf(cTagM,"UG%02d",iDet);
      gMC->Gspos(cTagV,1,cTagM,xpos,ypos,zpos,0,"ONLY");
      // Water layer (cooling)
      zpos = fgkWaZpos;
      sprintf(cTagV,"UP%02d",iDet);
      sprintf(cTagM,"UG%02d",iDet);
      gMC->Gspos(cTagV,1,cTagM,xpos,ypos,zpos,0,"ONLY");
      if (fPHOShole) {
        if (fClengthPH[iplan][icham] > 0.0) {
          // Lower part
          // Rohacell layer (radiator)
          zpos = fgkRaZpos;
          sprintf(cTagV,"UH%02d",iDet+kNdet);
          sprintf(cTagM,"UC%02d",iDet+kNdet);
          gMC->Gspos(cTagV,1,cTagM,xpos,ypos,zpos,0,"ONLY");
          // Mylar layer (entrance window + HV cathode)   
          zpos = fgkMyZpos;
          sprintf(cTagV,"UI%02d",iDet+kNdet);
          sprintf(cTagM,"UC%02d",iDet+kNdet);
          gMC->Gspos(cTagV,1,cTagM,xpos,ypos,zpos,0,"ONLY");
          // Xe/Isobutane layer (drift volume) 
          zpos = fgkDrZpos;
          sprintf(cTagV,"UJ%02d",iDet+kNdet);
          sprintf(cTagM,"UC%02d",iDet+kNdet);
          gMC->Gspos(cTagV,1,cTagM,xpos,ypos,zpos,0,"ONLY");
          // Upper part
          // Xe/Isobutane layer (amplification volume)
          zpos = fgkAmZpos;
          sprintf(cTagV,"UK%02d",iDet+kNdet);
          sprintf(cTagM,"UE%02d",iDet+kNdet);
          gMC->Gspos(cTagV,1,cTagM,xpos,ypos,zpos,0,"ONLY");
          // Readout part
          // Cu layer (pad plane)
          zpos = fgkCuZpos; 
          sprintf(cTagV,"UL%02d",iDet+kNdet);
          sprintf(cTagM,"UG%02d",iDet+kNdet);
          gMC->Gspos(cTagV,1,cTagM,xpos,ypos,zpos,0,"ONLY");
          // G10 layer (support structure)
          zpos = fgkSuZpos;
          sprintf(cTagV,"UM%02d",iDet+kNdet);
          sprintf(cTagM,"UG%02d",iDet+kNdet);
          gMC->Gspos(cTagV,1,cTagM,xpos,ypos,zpos,0,"ONLY");
          // Cu layer (FEE + signal lines)
          zpos = fgkFeZpos; 
          sprintf(cTagV,"UN%02d",iDet+kNdet);
          sprintf(cTagM,"UG%02d",iDet+kNdet);
          gMC->Gspos(cTagV,1,cTagM,xpos,ypos,zpos,0,"ONLY");
          // Al layer (cooling devices)
          zpos = fgkCoZpos;
          sprintf(cTagV,"UO%02d",iDet+kNdet);
          sprintf(cTagM,"UG%02d",iDet+kNdet);
          gMC->Gspos(cTagV,1,cTagM,xpos,ypos,zpos,0,"ONLY");
          // Water layer (cooling)
          zpos = fgkWaZpos;
          sprintf(cTagV,"UP%02d",iDet+kNdet);
          sprintf(cTagM,"UG%02d",iDet+kNdet);
          gMC->Gspos(cTagV,1,cTagM,xpos,ypos,zpos,0,"ONLY");
	}
      }
      if (fRICHhole) {
        if (fClengthRH[iplan][icham] > 0.0) {
          // Lower part
          // Rohacell layer (radiator)
          zpos = fgkRaZpos;
          sprintf(cTagV,"UH%02d",iDet+2*kNdet);
          sprintf(cTagM,"UC%02d",iDet+2*kNdet);
          gMC->Gspos(cTagV,1,cTagM,xpos,ypos,zpos,0,"ONLY");
          // Mylar layer (entrance window + HV cathode)   
          zpos = fgkMyZpos;
          sprintf(cTagV,"UI%02d",iDet+2*kNdet);
          sprintf(cTagM,"UC%02d",iDet+2*kNdet);
          gMC->Gspos(cTagV,1,cTagM,xpos,ypos,zpos,0,"ONLY");
          // Xe/Isobutane layer (drift volume) 
          zpos = fgkDrZpos;
          sprintf(cTagV,"UJ%02d",iDet+2*kNdet);
          sprintf(cTagM,"UC%02d",iDet+2*kNdet);
          gMC->Gspos(cTagV,1,cTagM,xpos,ypos,zpos,0,"ONLY");
          // Upper part
          // Xe/Isobutane layer (amplification volume)
          zpos = fgkAmZpos;
          sprintf(cTagV,"UK%02d",iDet+2*kNdet);
          sprintf(cTagM,"UE%02d",iDet+2*kNdet);
          gMC->Gspos(cTagV,1,cTagM,xpos,ypos,zpos,0,"ONLY");
          // Readout part
          // Cu layer (pad plane)
          zpos = fgkCuZpos; 
          sprintf(cTagV,"UL%02d",iDet+2*kNdet);
          sprintf(cTagM,"UG%02d",iDet+2*kNdet);
          gMC->Gspos(cTagV,1,cTagM,xpos,ypos,zpos,0,"ONLY");
          // G10 layer (support structure)
          zpos = fgkSuZpos;
          sprintf(cTagV,"UM%02d",iDet+2*kNdet);
          sprintf(cTagM,"UG%02d",iDet+2*kNdet);
          gMC->Gspos(cTagV,1,cTagM,xpos,ypos,zpos,0,"ONLY");
          // Cu layer (FEE + signal lines)
          zpos = fgkFeZpos; 
          sprintf(cTagV,"UN%02d",iDet+2*kNdet);
          sprintf(cTagM,"UG%02d",iDet+2*kNdet);
          gMC->Gspos(cTagV,1,cTagM,xpos,ypos,zpos,0,"ONLY");
          // Al layer (cooling devices)
          zpos = fgkCoZpos;
          sprintf(cTagV,"UO%02d",iDet+2*kNdet);
          sprintf(cTagM,"UG%02d",iDet+2*kNdet);
          gMC->Gspos(cTagV,1,cTagM,xpos,ypos,zpos,0,"ONLY");
          // Water layer (cooling)
          zpos = fgkWaZpos;
          sprintf(cTagV,"UP%02d",iDet+2*kNdet);
          sprintf(cTagM,"UG%02d",iDet+2*kNdet);
          gMC->Gspos(cTagV,1,cTagM,xpos,ypos,zpos,0,"ONLY");
	}
      }

      // Position the inner volumes of the chambers in the frames
      xpos      = 0.0;
      ypos      = 0.0;
      zpos      = 0.0;
      // The inside of the lower G10 frame
      sprintf(cTagV,"UC%02d",iDet);
      sprintf(cTagM,"UB%02d",iDet);
      gMC->Gspos(cTagV,1,cTagM,xpos,ypos,zpos,0,"ONLY");
      // The lower G10 frame inside the aluminum frame
      sprintf(cTagV,"UB%02d",iDet);
      sprintf(cTagM,"UA%02d",iDet);
      gMC->Gspos(cTagV,1,cTagM,xpos,ypos,zpos,0,"ONLY");
      // The inside of the upper G10 frame
      sprintf(cTagV,"UE%02d",iDet);
      sprintf(cTagM,"UD%02d",iDet);
      gMC->Gspos(cTagV,1,cTagM,xpos,ypos,zpos,0,"ONLY");
      // The inside of the upper aluminum frame
      sprintf(cTagV,"UG%02d",iDet);
      sprintf(cTagM,"UF%02d",iDet);
      gMC->Gspos(cTagV,1,cTagM,xpos,ypos,zpos,0,"ONLY");      
      if (fPHOShole) {
        if (fClengthPH[iplan][icham] > 0.0) {
          // The inside of the lower G10 frame
          sprintf(cTagV,"UC%02d",iDet+kNdet);
          sprintf(cTagM,"UB%02d",iDet+kNdet);
          gMC->Gspos(cTagV,1,cTagM,xpos,ypos,zpos,0,"ONLY");
          // The lower G10 frame inside the aluminum frame
          sprintf(cTagV,"UB%02d",iDet+kNdet);
          sprintf(cTagM,"UA%02d",iDet+kNdet);
          gMC->Gspos(cTagV,1,cTagM,xpos,ypos,zpos,0,"ONLY");
          // The inside of the upper G10 frame
          sprintf(cTagV,"UE%02d",iDet+kNdet);
          sprintf(cTagM,"UD%02d",iDet+kNdet);
          gMC->Gspos(cTagV,1,cTagM,xpos,ypos,zpos,0,"ONLY");
          // The inside of the upper aluminum frame
          sprintf(cTagV,"UG%02d",iDet+kNdet);
          sprintf(cTagM,"UF%02d",iDet+kNdet);
          gMC->Gspos(cTagV,1,cTagM,xpos,ypos,zpos,0,"ONLY");      
	}
      }
      if (fRICHhole) {
        if (fClengthRH[iplan][icham] > 0.0) {
          // The inside of the lower G10 frame
          sprintf(cTagV,"UC%02d",iDet+2*kNdet);
          sprintf(cTagM,"UB%02d",iDet+2*kNdet);
          gMC->Gspos(cTagV,1,cTagM,xpos,ypos,zpos,0,"ONLY");
          // The lower G10 frame inside the aluminum frame
          sprintf(cTagV,"UB%02d",iDet+2*kNdet);
          sprintf(cTagM,"UA%02d",iDet+2*kNdet);
          gMC->Gspos(cTagV,1,cTagM,xpos,ypos,zpos,0,"ONLY");
          // The inside of the upper G10 frame
          sprintf(cTagV,"UE%02d",iDet+2*kNdet);
          sprintf(cTagM,"UD%02d",iDet+2*kNdet);
          gMC->Gspos(cTagV,1,cTagM,xpos,ypos,zpos,0,"ONLY");
          // The inside of the upper aluminum frame
          sprintf(cTagV,"UG%02d",iDet+2*kNdet);
          sprintf(cTagM,"UF%02d",iDet+2*kNdet);
          gMC->Gspos(cTagV,1,cTagM,xpos,ypos,zpos,0,"ONLY");      
	}
      }

      // Position the frames of the chambers in the TRD mother volume
      xpos  = 0.;
      ypos  = - fClength[iplan][0] - fClength[iplan][1] - fClength[iplan][2]/2.;
      for (Int_t ic = 0; ic < icham; ic++) {
        ypos += fClength[iplan][ic];        
      }
      ypos += fClength[iplan][icham]/2.;
      zpos  = fgkCraH/2. + fgkCdrH/2. - fgkSheight/2. + iplan * (fgkCH + fgkVspace);
      // The lower aluminum frame, radiator + drift region
      sprintf(cTagV,"UA%02d",iDet);
      gMC->Gspos(cTagV,1,"UTR1",xpos,ypos,zpos,0,"ONLY");
      // The upper G10 frame, amplification region
      sprintf(cTagV,"UD%02d",iDet);
      zpos += fgkCamH/2. + fgkCraH/2. + fgkCdrH/2.;
      gMC->Gspos(cTagV,1,"UTR1",xpos,ypos,zpos,0,"ONLY");
      // The upper aluminum frame
      sprintf(cTagV,"UF%02d",iDet);
      zpos += fgkCroH/2. + fgkCamH/2.;
      gMC->Gspos(cTagV,1,"UTR1",xpos,ypos,zpos,0,"ONLY");
      if (fPHOShole) {
        if (fClengthPH[iplan][icham] > 0.0) {
          xpos  = 0.;
          ypos  = - fClength[iplan][0] - fClength[iplan][1] - fClength[iplan][2]/2.;
          for (Int_t ic = 0; ic < icham; ic++) {
            ypos += fClength[iplan][ic];        
          }
          if (icham > 2) {
            ypos += fClength[iplan][icham];
            ypos -= fClengthPH[iplan][icham]/2.;
	  }
          else {
            ypos += fClengthPH[iplan][icham]/2.;
	  }
          zpos  = fgkCraH/2. + fgkCdrH/2. - fgkSheight/2. + iplan * (fgkCH + fgkVspace);
          // The lower aluminum frame, radiator + drift region
          sprintf(cTagV,"UA%02d",iDet+kNdet);
          gMC->Gspos(cTagV,1,"UTR2",xpos,ypos,zpos,0,"ONLY");
          // The upper G10 frame, amplification region
          sprintf(cTagV,"UD%02d",iDet+kNdet);
          zpos += fgkCamH/2. + fgkCraH/2. + fgkCdrH/2.;
          gMC->Gspos(cTagV,1,"UTR2",xpos,ypos,zpos,0,"ONLY");
          // The upper aluminum frame
          sprintf(cTagV,"UF%02d",iDet+kNdet);
          zpos += fgkCroH/2. + fgkCamH/2.;
          gMC->Gspos(cTagV,1,"UTR2",xpos,ypos,zpos,0,"ONLY");
	}
      }
      if (fRICHhole) {
        if (fClengthRH[iplan][icham] > 0.0) {
          xpos  = 0.;
          ypos  = - fClength[iplan][0] - fClength[iplan][1] - fClength[iplan][2]/2.;
          for (Int_t ic = 0; ic < icham; ic++) {
            ypos += fClength[iplan][ic];        
          }
          if (icham > 2) {
            ypos += fClength[iplan][icham];
            ypos -= fClengthRH[iplan][icham]/2.;
	  }
          else {
            ypos += fClengthRH[iplan][icham]/2.;
	  }
          zpos  = fgkCraH/2. + fgkCdrH/2. - fgkSheight/2. + iplan * (fgkCH + fgkVspace);
          // The lower aluminum frame, radiator + drift region
          sprintf(cTagV,"UA%02d",iDet+2*kNdet);
          gMC->Gspos(cTagV,1,"UTR3",xpos,ypos,zpos,0,"ONLY");
          // The upper G10 frame, amplification region
          sprintf(cTagV,"UD%02d",iDet+2*kNdet);
          zpos += fgkCamH/2. + fgkCraH/2. + fgkCdrH/2.;
          gMC->Gspos(cTagV,1,"UTR3",xpos,ypos,zpos,0,"ONLY");
          // The upper aluminum frame
          sprintf(cTagV,"UF%02d",iDet+2*kNdet);
          zpos += fgkCroH/2. + fgkCamH/2.;
          gMC->Gspos(cTagV,1,"UTR3",xpos,ypos,zpos,0,"ONLY");
	}
      }

    }
  }

  xpos = 0.;
  ypos = 0.;
  zpos = 0.;
  gMC->Gspos("UTR1",1,"BTR1",xpos,ypos,zpos,0,"ONLY");
  if (fPHOShole) {
    gMC->Gspos("UTR2",2,"BTR2",xpos,ypos,zpos,0,"ONLY");
  }
  else {
    gMC->Gspos("UTR1",2,"BTR2",xpos,ypos,zpos,0,"ONLY");
  }
  if (fRICHhole) {
    gMC->Gspos("UTR3",3,"BTR3",xpos,ypos,zpos,0,"ONLY");
  }
  else {
    gMC->Gspos("UTR1",3,"BTR3",xpos,ypos,zpos,0,"ONLY");
  }

}

//_____________________________________________________________________________
void AliTRDgeometryFull::SetOldGeometry()
{
  //
  // Use the old chamber lengths
  //

  Int_t icham;
  Int_t iplan;

  AliTRDgeometry::SetOldGeometry();

  Float_t lengthPH[kNplan][kNcham] = { { 123.5, 116.5,   0.0, 116.5, 123.5 }
				     , { 131.0, 124.0,   0.0, 124.0, 131.0 }
				     , { 134.5, 131.5,   0.0, 131.5, 134.5 }
				     , { 142.0, 139.0,   0.0, 139.0, 142.0 }
				     , { 142.0, 146.0,   0.0, 146.0, 142.0 }
                                     , { 134.5, 153.5,   0.0, 153.5, 134.5 } };

  Float_t lengthRH[kNplan][kNcham] = { {  86.5,   0.0,   0.0,   0.0,  86.5 }
				     , { 101.5,   0.0,   0.0,   0.0, 101.5 }
				     , { 112.5,   0.0,   0.0,   0.0, 112.5 }
				     , { 127.5,   0.0,   0.0,   0.0, 127.5 }
				     , { 134.5,   0.0,   0.0,   0.0, 134.5 }
                                     , { 134.5,   0.0,   0.0,   0.0, 134.5 } };

  for (icham = 0; icham < kNcham; icham++) {
    for (iplan = 0; iplan < kNplan; iplan++) {
      fClengthPH[iplan][icham] = lengthPH[iplan][icham];
      fClengthRH[iplan][icham] = lengthRH[iplan][icham];
    }
  }

}

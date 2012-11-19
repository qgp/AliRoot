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
Revision 1.6  2006/04/20 22:30:49  hristov
Coding conventions (Annalisa)

Revision 1.5  2006/04/16 22:29:05  hristov
Coding conventions (Annalisa)

Revision 1.4  2006/04/05 08:35:38  hristov
Coding conventions (S.Arcelli, C.Zampolli)

Revision 1.3  2006/03/28 14:57:56  arcelli
updates to handle new V5 geometry & some re-arrangements

Revision 1.2  2006/02/13 17:22:26  arcelli
just Fixing Log info

Revision 1.1  2006/02/13 16:10:48  arcelli
Add classes for TOF Calibration (C.Zampolli)

author: Chiara Zampolli, zampolli@bo.infn.it
*/  

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// class for TOF calibration : PlateC                                        //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "TBrowser.h"

#include "AliLog.h"

#include "AliTOFCalPlateC.h"
#include "AliTOFCalStrip.h"
#include "AliTOFChannel.h"
#include "AliTOFGeometryV5.h"

ClassImp(AliTOFCalPlateC)

//________________________________________________________________

AliTOFCalPlateC::AliTOFCalPlateC():
  fNStripC(0),
  fNpadZ(0),
  fNpadX(0),
  fGeom(0x0), 
  fCh(0x0)
{
  //main ctor
}
//________________________________________________________________

AliTOFCalPlateC::AliTOFCalPlateC(AliTOFChannel *ch) : 
  fNStripC(0),
  fNpadZ(0),
  fNpadX(0),
  fGeom(0x0), 
  fCh(ch)
{
  //ctor with channel
}
//________________________________________________________________

AliTOFCalPlateC::AliTOFCalPlateC(AliTOFGeometry *geom):
  fNStripC(0),
  fNpadZ(0),
  fNpadX(0),
  fGeom(geom), 
  fCh(0x0)
{
  //ctor with geom
  fNStripC = fGeom->NStripC();
  fNpadZ = fGeom->NpadZ();
  fNpadX = fGeom->NpadX();
}
//________________________________________________________________

AliTOFCalPlateC::AliTOFCalPlateC(AliTOFGeometry *geom, AliTOFChannel *ch): 
  fNStripC(0),
  fNpadZ(0),
  fNpadX(0),
  fGeom(geom), 
  fCh(ch)
{
  //ctor with channel and geom
  fNStripC = fGeom->NStripC();
  fNpadZ = fGeom->NpadZ();
  fNpadX = fGeom->NpadX();
}

//________________________________________________________________

AliTOFCalPlateC& AliTOFCalPlateC::operator=(const AliTOFCalPlateC& pl)
  {
  //assignment operator
    this->fCh = pl.fCh;
    this->fNStripC = pl.fNStripC;
    this->fNpadZ = pl.fNpadZ;
    this->fNpadX = pl.fNpadX;
    this->fGeom = pl.fGeom;
    return *this;

  }
//________________________________________________________________

AliTOFCalPlateC::~AliTOFCalPlateC()
{
  //dtor
  delete[] fCh;
}

//________________________________________________________________

AliTOFCalPlateC::AliTOFCalPlateC(const AliTOFCalPlateC& pl):
  TObject(pl),
  fNStripC(0),
  fNpadZ(0),
  fNpadX(0),
  fGeom(0x0), 
  fCh(0x0)
  {
  //copy ctor 
    fCh = pl.fCh;
    fNStripC = pl.fNStripC;
    fNpadZ = pl.fNpadZ;
    fNpadX = pl.fNpadX;
    fGeom = pl.fGeom;

  }
//________________________________________________________________

void AliTOFCalPlateC::Browse(TBrowser *b){
  //add cal obj to list of browsables

  if(fGeom==0x0){
    AliTOFGeometry *geom = new AliTOFGeometryV5(); 
    AliInfo("V5 TOF Geometry is taken as the default");
    fNStripC = geom->NStripC();
    fNpadZ = geom->NpadZ();
    fNpadX = geom->NpadX();
    delete geom;
  }
  char name[10];
  for(Int_t i=0; i<fNStripC; ++i) {
    snprintf(name,sizeof(name),"Strip %2.2d",i);
    b->Add(new AliTOFCalStrip(&fCh[i*fNpadZ*fNpadX]),name);
  }
}

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

Revision 1.3  2006/03/28 14:57:30  arcelli
updates to handle new V5 geometry & some re-arrangements

Revision 1.2  2006/02/13 17:22:26  arcelli
just Fixing Log info

Revision 1.1  2006/02/13 16:10:48  arcelli
Add classes for TOF Calibration (C.Zampolli)

author: Chiara Zampolli, zampolli@bo.infn.it
*/  

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// class for TOF calibration : PadZ                                          //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "TBrowser.h"

#include "AliLog.h"

#include "AliTOFCalPadZ.h"
#include "AliTOFChannel.h"
#include "AliTOFGeometryV5.h"

ClassImp(AliTOFCalPadZ)

//________________________________________________________________

  AliTOFCalPadZ::AliTOFCalPadZ():
  fNpadX(0),
  fGeom(0x0),
  fCh(0)
{
  //fCh = 0;
  //fNpadX=0;
}
//________________________________________________________________

AliTOFCalPadZ::AliTOFCalPadZ(AliTOFChannel *ch):
  fNpadX(0),
  fGeom(0x0),
  fCh(ch)
{  
  //fNpadX = 0;
}
//________________________________________________________________

AliTOFCalPadZ::AliTOFCalPadZ(AliTOFGeometry *geom):
  fNpadX(0),
  fGeom(geom),
  fCh(0)
{
  //ctor with TOF geometry
  //fCh = 0;
  //fGeom = geom;
  fNpadX = fGeom->NpadX();
}
//________________________________________________________________

AliTOFCalPadZ::AliTOFCalPadZ(AliTOFGeometry *geom,AliTOFChannel *ch):
  fNpadX(0),
  fGeom(geom),
  fCh(ch)
{  
  //ctor with TOF geometry and cal channel
  fGeom = geom;
  fNpadX = fGeom->NpadX();
}

//________________________________________________________________
AliTOFCalPadZ::AliTOFCalPadZ(const AliTOFCalPadZ &source):
  TObject(),
  fNpadX(0),
  fGeom(0x0),
  fCh(0)
{
  // copy constructor

  this->fGeom=source.fGeom;

}

//_____________________________________________________________________________
AliTOFCalPadZ& AliTOFCalPadZ::operator=(const AliTOFCalPadZ &source)
{
  // assignment operator

  this->fGeom=source.fGeom;
  return *this;

}

//________________________________________________________________

AliTOFCalPadZ::~AliTOFCalPadZ()
  {
    delete[] fCh;
  }
//________________________________________________________________

void AliTOFCalPadZ::Browse(TBrowser *b)
{
  //Add cal object to browsable list
  if(fGeom==0x0){
    AliInfo("V5 TOF Geometry is taken as the default");
    AliTOFGeometry *geom = new AliTOFGeometryV5();
    fNpadX = geom->NpadX();
    delete geom;
  }
  char name[10];
  for(Int_t i=0; i<fNpadX; ++i) {
    snprintf(name,sizeof(name),"PadX %2.2d",i);
    b->Add(new AliTOFChannel(fCh[i]),name);
  }
}

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

/* History of cvs commits:
 *
 * $Log$
 */

#include "AliSurveyPoint.h"
#include "AliSurveyObj.h"

#include "AliPHOSEMCAGeometry.h"
#include "AliPHOSGeometry.h"
#include "AliPHOSSurvey1.h"
#include "AliLog.h"

ClassImp(AliPHOSSurvey1)

//____________________________________________________________________________
AliPHOSSurvey1::AliPHOSSurvey1(const TString &fileName, const TString &namePrefix)
{
  const AliPHOSGeometry *phosGeom = AliPHOSGeometry::GetInstance("IHEP", "IHEP");
  if (!phosGeom) {
    AliError("Cannot obtain AliPHOSGeometry instance.");
    return;
  }

  AliSurveyObj survey;
  survey.FillFromLocalFile(fileName);

  AliPHOSEMCAGeometry * emcaGeom = phosGeom->GetEMCAGeometry();
  fStrNum = emcaGeom->GetNStripX() * emcaGeom->GetNStripZ();

  TObjArray *points = survey.GetData();
  Int_t goodPoints = 0;
  Int_t start = -1;
  for (Int_t i = 0, e = points->GetEntries(); i < e; ++i) {
    AliSurveyPoint *stripPoint = static_cast<AliSurveyPoint *>(points->At(i));
    if (stripPoint->GetPointName().BeginsWith(namePrefix)) {
      ++goodPoints;
      if (start == -1)
        start = i;
    }
  }

  if (goodPoints != kNumberOfPoints) {
    AliError("Wrong number of points with prefix" + namePrefix);
    return;
  }

  Double_t *xReal = new Double_t[fStrNum * 2];//1
  Double_t *zReal = new Double_t[fStrNum * 2];//2

  for (Int_t i = 0; i < fStrNum * 2; ++i) {
    AliSurveyPoint *stripPoint = static_cast<AliSurveyPoint *>(points->At(start + kStartingPoint + i));
    xReal[i] = stripPoint->GetX() * 0.1;
    zReal[i] = stripPoint->GetZ() * 0.1;
  }

  InitStripData(xReal, zReal);

  delete [] zReal;//2
  delete [] xReal;//1
}

//____________________________________________________________________________
AliPHOSSurvey1::~AliPHOSSurvey1()
{
}

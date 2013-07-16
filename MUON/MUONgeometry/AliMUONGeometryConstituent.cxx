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

// $Id$

//-----------------------------------------------------------------------------
// Class AliMUONGeometryConstituent
// -----------------------------
// Helper class for definititon of an assembly of volumes.
// Author: Ivana Hrivnacova, IPN Orsay
// 23/01/2004
//-----------------------------------------------------------------------------

#include <TGeoMatrix.h>

#include "AliMUONGeometryConstituent.h"
#include "AliLog.h"

/// \cond CLASSIMP
ClassImp(AliMUONGeometryConstituent)
/// \endcond

//______________________________________________________________________________
AliMUONGeometryConstituent::AliMUONGeometryConstituent(const TString& name, 
                                   Int_t copyNo, Int_t npar, Double_t* param)
  : TNamed(name, name),
    fCopyNo(copyNo),
    fNpar(npar),
    fParam(0), 
    fTransformation(0)				   
{				    
/// Standard constructor for a constituent without translation & rotation

  // fTransformation = new TGeoCombiTrans(name);
           // would be nice to be so simple 

  // Create the constituent transformation
  fTransformation = new TGeoCombiTrans("");

  // Volume parameters
  if (npar > 0) {
    fParam = new Double_t[npar];
    for (Int_t i=0; i<npar; i++) fParam[i] = param[i];
  }  
}

//______________________________________________________________________________
AliMUONGeometryConstituent::AliMUONGeometryConstituent(const TString& name, 
                                   Int_t copyNo, const TGeoTranslation& translation,
	  		           Int_t npar, Double_t* param)
  : TNamed(name, name),
    fCopyNo(copyNo),
    fNpar(npar),
    fParam(0),				   
    fTransformation(0) 
{
/// Standard constructor for a constituent with translation

  // Create the constituent transformation
  fTransformation = new TGeoCombiTrans(translation, TGeoRotation());

  // Volume parameters
  if (npar > 0) {
    fParam = new Double_t[npar];
    for (Int_t i=0; i<npar; i++) fParam[i] = param[i];
  }  
}

			 
//______________________________________________________________________________
AliMUONGeometryConstituent::AliMUONGeometryConstituent(const TString& name, 
                                   Int_t copyNo, const TGeoTranslation& translation, 
	  	                   const TGeoRotation& rotation, 
				   Int_t npar, Double_t* param)
				   
  : TNamed(name, name),
    fCopyNo(copyNo),
    fNpar(npar),
    fParam(0),				   
    fTransformation(0) 
{
/// Standard constructor for a constituent with translation and rotation

  // Create the constituent transformation
  fTransformation = new TGeoCombiTrans(translation, rotation);

  // Volume parameters
  if (npar > 0) {
    fParam = new Double_t[npar];
    for (Int_t i=0; i<npar; i++) fParam[i] = param[i];
  }  
}

//______________________________________________________________________________
AliMUONGeometryConstituent::AliMUONGeometryConstituent(const TString& name, 
                                   Int_t copyNo, 
				   const TGeoCombiTrans& transform, 
				   Int_t npar, Double_t* param)
				   
  : TNamed(name, name),
    fCopyNo(copyNo),
    fNpar(npar),
    fParam(0),				   
    fTransformation(0) 
{
/// Standard constructor for a constituent with translation and rotation
/// defined via TGeoCombiTrans

  // Create the constituent transformation
  fTransformation = new TGeoCombiTrans(transform);

  // Volume parameters
  if (npar > 0) {
    fParam = new Double_t[npar];
    for (Int_t i=0; i<npar; i++) fParam[i] = param[i];
  }  
}

//______________________________________________________________________________
AliMUONGeometryConstituent::AliMUONGeometryConstituent()
  : TNamed(),
    fCopyNo(0),
    fNpar(0),
    fParam(0),				   
    fTransformation(0) 
{
/// Default constructor
}

//______________________________________________________________________________
AliMUONGeometryConstituent::~AliMUONGeometryConstituent() 
{
/// Destructor

  delete fTransformation;
  delete [] fParam;
}

// -*- mode: C++ -*- 
#ifndef ALIESDUTILS_H
#define ALIESDUTILS_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */


/* $Id$ */

//-------------------------------------------------------------------------
//   AliESDUtils - This is a namespace that temporary provides general 
//                 purpose ESD utilities to avoid unnecessary dependencies
//                 between PWG libraries. To be removed/replaced by AODB
//                 framework.
//      
//-------------------------------------------------------------------------
// Author: Andrei Gheata

#ifndef ROOT_Rtypes
#include "Rtypes.h"
#endif
class AliESDEvent;

namespace AliESDUtils {

  /********************************/
  /* Centrality-related corrections */
  /********************************/

  Float_t GetCorrV0(const AliESDEvent* esd, Float_t &v0CorrResc, Float_t *v0multChCorr = NULL);
  Float_t GetCorrSPD2(Float_t spd2raw,Float_t zv);
}  

#endif

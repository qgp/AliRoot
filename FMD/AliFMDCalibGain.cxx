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

//____________________________________________________________________
//                                                                          
//
//
#include "AliFMDCalibGain.h"	// ALIFMDCALIBGAIN_H
//____________________________________________________________________
ClassImp(AliFMDCalibGain)
#if 0
  ; // This is here to keep Emacs for indenting the next line
#endif

//____________________________________________________________________
AliFMDCalibGain::AliFMDCalibGain()
{
  fValue.Reset(-1.);
  fThreshold = -1.;
}

//____________________________________________________________________
AliFMDCalibGain::AliFMDCalibGain(const AliFMDCalibGain& o)
  : TObject(o), fValue(o.fValue), fThreshold(o.fThreshold)
{}

//____________________________________________________________________
AliFMDCalibGain&
AliFMDCalibGain::operator=(const AliFMDCalibGain& o)
{
  fValue     = o.fValue;
  fThreshold = o.fThreshold;
  return (*this);
}

//____________________________________________________________________
void
AliFMDCalibGain::Set(UShort_t det, Char_t ring, UShort_t sec, 
		     UShort_t str, Float_t val)
{
  if (fValue.CheckIndex(det, ring, sec, str) < 0) return;
  fValue(det, ring, sec, str) = val;
}

//____________________________________________________________________
Float_t
AliFMDCalibGain::Value(UShort_t det, Char_t ring, UShort_t sec, 
		       UShort_t str)
{
  return fValue(det, ring, sec, str);
}

//____________________________________________________________________
//
// EOF
//

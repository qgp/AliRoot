#ifndef ALIFMDCALIBSAMPLERATE_H
#define ALIFMDCALIBSAMPLERATE_H
/* Copyright(c) 1998-2000, ALICE Experiment at CERN, All rights
 * reserved. 
 *
 * See cxx source for full Copyright notice                               
 */
/** @file    AliFMDCalibSampleRate.h
    @author  Christian Holm Christensen <cholm@nbi.dk>
    @date    Sun Mar 26 18:32:14 2006
    @brief   Per digitizer card pulser calibration
    
*/
#ifndef ROOT_TObject
# include <TObject.h>
#endif
#ifndef ALIFMDUSHORTMAP_H
# include "AliFMDUShortMap.h"
#endif
#ifndef ROOT_TArrayI
# include <TArrayI.h>
#endif
//____________________________________________________________________
/** @brief Per digitizer card pulser calibration
    @ingroup FMD_base
*/
class AliFMDCalibSampleRate : public TObject
{
public:
  /** CTOR */
  AliFMDCalibSampleRate();
  /** Copy CTOR
      @param o Object to copy from  */
  AliFMDCalibSampleRate(const AliFMDCalibSampleRate& o);
  /** Assignment operator 
      @param o Object to assign from 
      @return Reference to assign from  */
  AliFMDCalibSampleRate& operator=(const AliFMDCalibSampleRate& o);
  /** Set sample for a DDL
      @param det   Detector #
      @param ring  Ring ID 
      @param sec   Sector # 
      @param str   Strip number (not used)
      @param rate  Sample rate */
  void Set(UShort_t det, Char_t ring, UShort_t sec, UShort_t str, 
	   UShort_t rate);
  /** Get sample rate for a detector 
      @param det  Detector #
      @param ring Ring ID 
      @param sec  Sector # 
      @param str  Strip number (not used)
      @return Sample rate */
  UShort_t Rate(UShort_t det, Char_t ring, UShort_t sec, UShort_t str=0) const;
protected:
  // TArrayI fRates; // Sample rates 
  AliFMDUShortMap fRates;
  ClassDef(AliFMDCalibSampleRate,2); // Sample rates 
};

#endif
//____________________________________________________________________
//
// Local Variables:
//   mode: C++
// End:
//



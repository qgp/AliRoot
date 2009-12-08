//-*- Mode: C++ -*-
// $Id$

#ifndef ALIHLTQADATAMAKERREC_H
#define ALIHLTQADATAMAKERREC_H
//* This file is property of and copyright by the ALICE HLT Project        * 
//* ALICE Experiment at CERN, All rights reserved.                         *
//* See cxx source for full Copyright notice                               *

/** @file   AliHLTQADataMakerRec.h
    @author Zhongbao Yin, Matthias Richter
    @date   2009-05-14
    @brief  Container for the HLT offline QA
*/

#include "AliQADataMakerRec.h"

class AliHLTQADataMakerRec: public AliQADataMakerRec {

public:

  enum HESDsType_t {kMultiplicity=0, kMultiplicityFired, kNCls, 
		    kNClsFired, kPHLT, kPOffline, kPRatio, 
		    kPHLTFired, kPOfflineFired, kPRatioFired,
		    kPtHLT, kPtOffline, 
		    kPtHLTFired, kPtOfflineFired, 
		    kNClsPerTrkHLT, kNClsPerTrkOffline, 
		    kNClsPerTrkHLTFired, kNClsPerTrkOfflineFired, 
		    kPhiHLT, kPhiOffline,
		    kPhiHLTFired, kPhiOfflineFired,
		    kEtaHLT, kEtaOffline,
		    kEtaHLTFired, kEtaOfflineFired};  
  
  AliHLTQADataMakerRec();
  virtual ~AliHLTQADataMakerRec();

private:
  /** copy constructor prohibited */
  AliHLTQADataMakerRec(const AliHLTQADataMakerRec&);   
  /** assignment operator prohibited */
  AliHLTQADataMakerRec& operator = (const AliHLTQADataMakerRec&);

  virtual void Exec(AliQAv1::TASKINDEX_t task, TObject * data);
  virtual void StartOfDetectorCycle();
  virtual void EndOfDetectorCycle(AliQAv1::TASKINDEX_t, TObjArray** list);
  virtual void MakeRaws(AliRawReader * rawReader);
  virtual void InitESDs();
  virtual void MakeESDs(AliESDEvent * esd);
  virtual void MakeESDs(AliESDEvent * esd, AliESDEvent* hltesd);

  ClassDef(AliHLTQADataMakerRec,0)  // HLT Quality Assurance Data Maker for reconstruction
};

#endif // ALIHLTQADATAMAKERREC_H

// -*- mode: C++ -*-
//
// $Id$
//
#ifndef ALIFMD2_H
#define ALIFMD2_H

#ifndef ALIFMDSUBDETECTOR_H
# include "AliFMDSubDetector.h"
#endif

class AliFMD2 : public AliFMDSubDetector 
{
private:
  Int_t    fVolumeId;
  Double_t fDz;
public:
  AliFMD2();
  virtual ~AliFMD2();
  virtual void   SetupGeometry(Int_t airId, Int_t kaptionId);  
  virtual void   Geometry(const char* mother, Int_t pbRotId, 
			  Int_t idRotId, Double_t z=0);
  ClassDef(AliFMD2,1); // Geometry of FMD2 
};

#endif
//
// EOF
//

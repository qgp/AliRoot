#ifndef ALIMUONRECONSTRUCTOR_H
#define ALIMUONRECONSTRUCTOR_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */
// Revision of includes 07/05/2004

/// \ingroup rec
/// \class AliMUONReconstructor
/// \brief Class for the MUON reconstruction

#include "AliReconstructor.h"

class AliMUONCalibrationData;
class AliMUONData;
class TTask;

class AliMUONReconstructor: public AliReconstructor 
{
  public:
    AliMUONReconstructor();
    virtual ~AliMUONReconstructor();

    virtual void         Init(AliRunLoader* runLoader);

    virtual void         Reconstruct(TTree* /*digitsTree*/, 
				     TTree* /*clustersTree*/) const {return;}
    virtual void         Reconstruct(AliRawReader* /*rawReader*/, 
				     TTree* /*clustersTree*/) const {return;}
    virtual void         Reconstruct(AliRunLoader* runLoader) const;
    virtual void         Reconstruct(AliRunLoader* runLoader, 
                                   AliRawReader* rawReader) const;

    virtual void         FillESD(TTree* /*digitsTree*/, TTree* /*clustersTree*/, 
				 AliESD* /*esd*/) const {return;}
    virtual void         FillESD(AliRawReader* /*rawReader*/, TTree* /*clustersTree*/, 
				 AliESD* /*esd*/) const {return;}
    virtual void         FillESD(AliRunLoader* runLoader, AliESD* esd) const;
    virtual void         FillESD(AliRunLoader* runLoader, 
				 AliRawReader* /*rawReader*/, AliESD* esd) const;

protected:
    AliMUONReconstructor(const AliMUONReconstructor& right);
    AliMUONReconstructor&  operator = (const AliMUONReconstructor& right);
     
private:

    TTask* GetCalibrationTask(AliMUONData* data) const;

private:
    AliRunLoader* fRunLoader; //!
    mutable AliMUONCalibrationData* fCalibrationData; //!
    
  ClassDef(AliMUONReconstructor, 0)   // class for the MUON reconstruction
};

#endif

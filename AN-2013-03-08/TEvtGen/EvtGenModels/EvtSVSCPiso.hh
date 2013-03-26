//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information: See EvtGen/COPYRIGHT
//      Copyright (C) 1998      Caltech, UCSB
//
// Module: EvtGen/EvtSVSCPiso.hh
//
// Description:
//
// Modification history:
//
//    DJL/RYD     August 11, 1998         Module created
//
//------------------------------------------------------------------------

#ifndef EVTSVSCPISO_HH
#define EVTSVSCPISO_HH

#include "EvtGenBase/EvtDecayAmp.hh"

class EvtParticle;

class EvtSVSCPiso:public  EvtDecayAmp  {

public:

  EvtSVSCPiso() {}
  virtual ~EvtSVSCPiso();

  std::string getName();
  EvtDecayBase* clone();

  void init();
  void initProbMax();  

  void decay(EvtParticle *p); 

};

#endif

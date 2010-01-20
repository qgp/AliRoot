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
// Module: EvtGen/EvtAbsLineShape.hh
//
// Description: Class to keep the particle properties for
//              one particle
//
// Modification history:
//
//    Lange     March 10, 2001         Module created
//
//------------------------------------------------------------------------

#ifndef EVTFLATLINESHAPE_HH
#define EVTFLATLINESHAPE_HH

#include "EvtGenBase/EvtAbsLineShape.hh"

class EvtFlatLineShape :public EvtAbsLineShape {

public:

  EvtFlatLineShape(); 
  EvtFlatLineShape(double mass, double width, double maxRange, EvtSpinType::spintype sp);
    //figure the m1 and l on the fly
    //			       double mDaug1, double mDaug2, int l); 
  ~EvtFlatLineShape();
  EvtFlatLineShape& operator=(const EvtFlatLineShape& x);
  EvtFlatLineShape(const EvtFlatLineShape& x); 

  EvtAbsLineShape* clone();

  double getMassProb(double mass, double massPar, int nDaug, double *massDau);
  // othDaugId is the other daughter of the parent in the case of a two body decay (only!)
  // ie B->rho K rho->pipi, othDaugId = K
   double getRandMass(EvtId *parId, int nDaug, EvtId *dauId, EvtId *othDaugId,double maxMass, double *dauMasses);


protected:
}; 

#endif


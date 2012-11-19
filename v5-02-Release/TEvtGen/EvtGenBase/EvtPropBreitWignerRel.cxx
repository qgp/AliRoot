#include "EvtGenBase/EvtPatches.hh"
/*******************************************************************************
 * Project: BaBar detector at the SLAC PEP-II B-factory
 * Package: EvtGenBase
 *    File: $Id: EvtPropBreitWignerRel.cc,v 1.5 2004/12/21 19:58:47 ryd Exp $
 *  Author: Alexei Dvoretskii, dvoretsk@slac.stanford.edu, 2001-2002
 *
 * Copyright (C) 2002 Caltech
 *******************************************************************************/

#include <math.h>
#include "EvtGenBase/EvtPropBreitWignerRel.hh"


EvtPropBreitWignerRel::EvtPropBreitWignerRel(double m0, double g0) 
  : EvtPropagator(m0,g0) 
{}


EvtPropBreitWignerRel::EvtPropBreitWignerRel(const EvtPropBreitWignerRel& other) 
  : EvtPropagator(other)
{}


EvtPropBreitWignerRel::~EvtPropBreitWignerRel() 
{}
  

EvtAmplitude<EvtPoint1D>* EvtPropBreitWignerRel::clone() const
{ 
  return new EvtPropBreitWignerRel(*this); 
}


EvtComplex EvtPropBreitWignerRel::amplitude(const EvtPoint1D& x) const
{
  double m = x.value();
  return 1./(_m0*_m0-m*m-EvtComplex(0.,_m0*_g0));   
}

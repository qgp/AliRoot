// $Header$

#include "NLTBases.h"

using namespace Reve;

//______________________________________________________________________
// NLTProjectable
//

ClassImp(NLTProjectable)

NLTProjectable::NLTProjectable()
{

}


//______________________________________________________________________
// NLTProjected
//

ClassImp(NLTProjected)

NLTProjected::NLTProjected() :
  fProjector   (0),
  fProjectable (0)
{

}

void NLTProjected::SetProjection(NLTProjector* proj, NLTProjectable* model)
{
  fProjector   = proj;
  fProjectable = model;
}

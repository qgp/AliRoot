// $Header$

#include "NLTBases.h"
#include "Reve/NLTPolygonSet.h"

using namespace Reve;

//______________________________________________________________________
// NLTProjectable
//

ClassImp(NLTProjectable)

NLTProjectable::NLTProjectable()
{

}

//______________________________________________________________________
// NLTGeoProjectable
//

ClassImp(NLTGeoProjectable)

NLTGeoProjectable::NLTGeoProjectable()
{}

TClass* NLTGeoProjectable::ProjectedClass() const
{
   return NLTPolygonSet::Class();
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

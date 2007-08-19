// $Header$

#ifndef REVE_NLTBases_H
#define REVE_NLTBases_H

#include <Reve/Reve.h>

#include <TObject.h>

namespace Reve {

class NLTProjector;

class NLTProjectable
{
private:
  NLTProjectable(const NLTProjectable&);            // Not implemented
  NLTProjectable& operator=(const NLTProjectable&); // Not implemented

protected:
  // Eventually, references to all projected instances.

public:
  NLTProjectable();
  virtual ~NLTProjectable() {}

  virtual TClass* ProjectedClass() const = 0;

  ClassDef(NLTProjectable, 0);
}; // endclass NLTProjectable


class NLTProjected
{
private:
  NLTProjected(const NLTProjected&);            // Not implemented
  NLTProjected& operator=(const NLTProjected&); // Not implemented

protected:
  NLTProjector   *fProjector;
  NLTProjectable *fProjectable;

public:
  NLTProjected();
  virtual ~NLTProjected() {}

  virtual void SetProjection(NLTProjector* proj, NLTProjectable* model);

  virtual void UpdateProjection() = 0;

  ClassDef(NLTProjected, 0);
}; // endclass NLTProjected

}

#endif

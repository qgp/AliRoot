// $Header$

#ifndef REVE_NLTBases_H
#define REVE_NLTBases_H

#include <Reve/Reve.h>

class TBuffer3D;

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


class NLTGeoProjectable : public NLTProjectable
{
private:
  NLTGeoProjectable(const NLTGeoProjectable&);            // Not implemented
  NLTGeoProjectable& operator=(const NLTGeoProjectable&); // Not implemented

public:
  NLTGeoProjectable();
  virtual ~NLTGeoProjectable() {}

  virtual TBuffer3D*     MakeBuffer3D() = 0;
  virtual TClass*        ProjectedClass() const;

  ClassDef(NLTGeoProjectable, 0);
};

/**************************************************************************/

class NLTProjected
{
private:
  NLTProjected(const NLTProjected&);            // Not implemented
  NLTProjected& operator=(const NLTProjected&); // Not implemented

protected:
  NLTProjector   *fProjector;
  NLTProjectable *fProjectable;

  Float_t         fDepth;

public:
  NLTProjected();
  virtual ~NLTProjected() {}

  virtual void SetProjection(NLTProjector* proj, NLTProjectable* model);
  virtual void SetDepth(Float_t d) { fDepth = d; }

  virtual void UpdateProjection() = 0;

  ClassDef(NLTProjected, 0);
}; // endclass NLTProjected

}

#endif

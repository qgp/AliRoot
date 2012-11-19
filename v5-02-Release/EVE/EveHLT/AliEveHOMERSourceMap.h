// $Id$
// Main authors: Matevz Tadel & Alja Mrak-Tadel: 2006, 2007

/**************************************************************************
 * Copyright(c) 1998-2008, ALICE Experiment at CERN, all rights reserved. *
 * See http://aliceinfo.cern.ch/Offline/AliRoot/License.html for          *
 * full copyright notice.                                                 *
 **************************************************************************/

#ifndef AliEveAliEveHOMERSourceMap_H
#define AliEveAliEveHOMERSourceMap_H

#include "AliEveHOMERSource.h"

#include <TNamed.h>

#include <map>

//______________________________________________________________________________
//
// AliEveHOMERSourceMap is an abstract container for HLT HOMER sources.
// The concrete implementations AliEveHOMERSourceMapByDet and
// AliEveHOMERSourceMapByType allow retrieval of HOMER sources in proper
// order as required for their display in EVE object browser.


class AliEveHOMERSourceMap : public TNamed
{
protected:
  // --- Inner structs ---

  struct iterator_imp_base
  {
    virtual ~iterator_imp_base() {}

    virtual const AliEveHOMERSource::SourceId&    id()    const = 0;
    virtual const AliEveHOMERSource::SourceState& state() const = 0;
    virtual       AliEveHOMERSource::SourceState& state() = 0;

    virtual iterator_imp_base* clone() const = 0;

    virtual iterator_imp_base& operator++() = 0;
    virtual bool operator!=(const iterator_imp_base& o) const = 0;

    virtual TString description() const = 0;
  };

public:
  // --- Inner structs ---

  struct iterator
  {
    iterator_imp_base* m_imp;

    iterator()                       :  m_imp(0)   {}
    iterator(const iterator& it)     :  m_imp(it.m_imp->clone())  {}
    iterator(iterator_imp_base* imp) :  m_imp(imp) {}
    ~iterator() { delete m_imp; }

    iterator& operator= (const iterator& o) { delete m_imp; m_imp = o.m_imp->clone(); return *this; }
    bool      operator!=(const iterator& o) { return m_imp->operator!=(*o.m_imp); }

    const AliEveHOMERSource::SourceId&    id()    { return m_imp->id(); }
          AliEveHOMERSource::SourceState& state() { return m_imp->state(); }

    Int_t   level();
    TString description() { return m_imp->description(); }

    iterator& operator++() { m_imp->operator++(); return *this; }
    iterator& skip_children()
    {
      Int_t lvl = level();
      do operator++(); while (level() > lvl);
      return *this;
    }
  };

  iterator begin() { return iterator(iterator_imp_begin()); }
  iterator end()   { return iterator(iterator_imp_end()); }

  enum ESourceGrouping_e { kSG_ByDet, kSG_ByType };

  // --- Interface ---

  AliEveHOMERSourceMap(ESourceGrouping_e grouping);
  virtual ~AliEveHOMERSourceMap() {}

  static AliEveHOMERSourceMap* Create(ESourceGrouping_e grouping);

  virtual void FillMap(const TList* handles, Bool_t def_state) = 0;

  void PrintXXX();


protected:
  ESourceGrouping_e fGrouping; // Not used so far ...

  virtual iterator_imp_base* iterator_imp_new()   = 0; // Not used so far ...
  virtual iterator_imp_base* iterator_imp_begin() = 0;
  virtual iterator_imp_base* iterator_imp_end()   = 0;

  ClassDef(AliEveHOMERSourceMap, 0); // A map of HOMER sources.
};

#endif

// $Id$
// Main authors: Matevz Tadel & Alja Mrak-Tadel: 2006, 2007

/**************************************************************************
 * Copyright(c) 1998-2008, ALICE Experiment at CERN, all rights reserved. *
 * See http://aliceinfo.cern.ch/Offline/AliRoot/License.html for          *
 * full copyright notice.                                                 *
 **************************************************************************/

#ifndef ALIEVE_TPCSectorViz_H
#define ALIEVE_TPCSectorViz_H

#include <TEveElement.h>
#include <TEveTrans.h>

#include <TNamed.h>
#include <TAtt3D.h>
#include <TAttBBox.h>


class AliEveTPCData; class AliEveTPCSectorData;

class AliEveTPCSectorVizEditor;
class AliEveTPCSector2D;  class AliEveTPCSector2DEditor;  class AliEveTPCSector2DGL;
class AliEveTPCSector3D;  class AliEveTPCSector3DEditor;  class AliEveTPCSector3DGL;

class AliEveTPCSectorViz : public TEveElement,
			   public TNamed,
			   public TAtt3D,
			   public TAttBBox
{
  friend class AliEveTPCSectorVizEditor;
  friend class AliEveTPCSector2D;
  friend class AliEveTPCSector2DEditor;
  friend class AliEveTPCSector2DGL;
  friend class AliEveTPCSector3D;
  friend class AliEveTPCSector3DEditor;
  friend class AliEveTPCSector3DGL;

  AliEveTPCSectorViz(const AliEveTPCSectorViz&);            // Not implemented
  AliEveTPCSectorViz& operator=(const AliEveTPCSectorViz&); // Not implemented

protected:
  AliEveTPCData    *fTPCData;
  Int_t             fSectorID;

  Int_t             fMinTime;
  Int_t             fMaxTime;
  Short_t           fThreshold;
  Int_t             fMaxVal;

  Bool_t            fRnrInn;
  Bool_t            fRnrOut1;
  Bool_t            fRnrOut2;

  Color_t           fFrameColor;
  Bool_t            fRnrFrame;
  TEveTrans         fHMTrans;
  Bool_t            fAutoTrans;
  UInt_t            fRTS;       //! Rendering TimeStamp

  mutable UChar_t  *fColorArray;

  void SetupColor(Int_t val, UChar_t* pix) const;
  void ClearColorArray();
  void SetupColorArray() const;
  UChar_t* ColorFromArray(Int_t val) const;
  void     ColorFromArray(Int_t val, UChar_t* pix) const;

public:
  AliEveTPCSectorViz(const Text_t* n="AliEveTPCSectorViz", const Text_t* t=0);
  virtual ~AliEveTPCSectorViz();

  virtual void CopyVizParams(const AliEveTPCSectorViz& v);

  virtual UInt_t IncRTS()           { return ++fRTS; }
  virtual Bool_t CanEditMainColor() { return kTRUE; }

  void SetDataSource(AliEveTPCData* data);
  void SetSectorID(Int_t id);

  AliEveTPCData*       GetData()     const { return fTPCData; }
  Int_t          GetSectorID() const { return fSectorID; }
  AliEveTPCSectorData* GetSectorData() const;

  Int_t GetMinTime() const { return fMinTime; }
  Int_t GetMaxTime() const { return fMaxTime; }
  void SetMinTime(Int_t mt)    { fMinTime   = mt; IncRTS(); }
  void SetMaxTime(Int_t mt)    { fMaxTime   = mt; IncRTS(); }
  void SetThreshold(Short_t t);
  void SetMaxVal(Int_t mv);

  void SetRnrInn(Bool_t r)     { fRnrInn  = r; IncRTS(); }
  void SetRnrOut1(Bool_t r)    { fRnrOut1 = r; IncRTS(); }
  void SetRnrOut2(Bool_t r)    { fRnrOut2 = r; IncRTS(); }

  void SetFrameColor(Color_t col)     { fFrameColor = col; IncRTS(); }
  virtual void SetRnrFrame(Bool_t rf) { fRnrFrame = rf;  IncRTS(); }
  void SetAutoTrans(Bool_t t);

  TEveTrans& RefHMTrans() { return fHMTrans; }
  void SetUseTrans(Bool_t t) { fHMTrans.SetUseTrans(t); }

  ClassDef(AliEveTPCSectorViz, 1); // Base-class for TPC raw-data visualization
}; // endclass AliEveTPCSectorViz


inline UChar_t* AliEveTPCSectorViz::ColorFromArray(Int_t val) const
{
  if(val < fThreshold) val = fThreshold;
  if(val > fMaxVal)    val = fMaxVal;
  return fColorArray + 4 * (val - fThreshold);
}

inline void AliEveTPCSectorViz::ColorFromArray(Int_t val, UChar_t* pix) const
{
  UChar_t* c = ColorFromArray(val);
  pix[0] = c[0]; pix[1] = c[1]; pix[2] = c[2]; pix[3] = c[3];
}

#endif

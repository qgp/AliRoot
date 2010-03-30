#ifndef ALITRDRECOTASK_H
#define ALITRDRECOTASK_H

///////////////////////////////////////////////////////
//
// Basic class for Performance/Calibration TRD tasks
//
// Author: Alexandru Bercuci, 10/09/2008
//
//////////////////////////////////////////////////

#ifndef ALIANALYSISTASK_H
#include "AliAnalysisTask.h"
#endif

#ifndef ALITRDTRACKINFO_H
#include "info/AliTRDtrackInfo.h"
#endif

class TH1;
class TF1;
class TList;
class TObjArray;
class TTreeSRedirector;
class AliTRDtrackV1;
class AliTRDtrackInfo;
class AliTRDrecoTask : public AliAnalysisTask 
{
public:
  enum AliTRDrecoSteeringBits{
    kMCdata       = BIT(20)
    ,kFriends     = BIT(21)
    ,kPostProcess = BIT(22)
  };
  AliTRDrecoTask(const char *name, const char *title);
  virtual ~AliTRDrecoTask();
  
  
  void           ConnectInputData(Option_t *);
  virtual void   CreateOutputObjects() = 0;
  virtual void   Exec(Option_t *opt);

  Int_t          GetNRefFigures() const  { return fNRefFigures; } 
  TList*         GetPlotFunctors() const { return fPlotFuncList;}
  virtual Bool_t GetRefFigure(Int_t ifig);

  Bool_t         HasFriends() const      { return TestBit(kFriends);};
  Bool_t         HasMCdata() const       { return TestBit(kMCdata);};
  Bool_t         HasPostProcess() const  { return TestBit(kPostProcess);};
  virtual TObjArray* Histos()            { return fContainer;}

  virtual Bool_t Load(const Char_t *filename = "TRD.Performance.root");
  virtual Bool_t Save(TObjArray * const res);
  virtual Bool_t PostProcess();
  virtual Bool_t PutTrendValue(const Char_t *name, Double_t val);
  virtual void   SetDebugLevel(Int_t level);
  virtual void   SetFriends(Bool_t fr = kTRUE) {SetBit(kFriends, fr);}
  virtual void   SetMCdata(Bool_t mc = kTRUE) {SetBit(kMCdata, mc);}
  virtual void   SetPostProcess(Bool_t pp = kTRUE) {SetBit(kPostProcess, pp);}
  virtual void   Terminate(Option_t *);

protected:
  Int_t          DebugLevel() const      { return fDebugLevel;}
  static TTreeSRedirector* DebugStream() { return fgDebugStream;}
  void           InitFunctorList();
  void           Adjust(TF1 *f, TH1 * const h);

  UChar_t   fNRefFigures;  //! no of reference figures reported by task
  TObjArray *fContainer;   //! container to store results
  TObjArray *fTracks;      //! Array of tracks
  const AliTRDtrackV1    *fkTrack;         //! current track
  const AliTRDtrackInfo::AliMCinfo  *fkMC; //! MC info
  const AliTRDtrackInfo::AliESDinfo *fkESD;//! ESD info

private:
  AliTRDrecoTask(const AliTRDrecoTask&);
  AliTRDrecoTask& operator=(const AliTRDrecoTask&);

  UChar_t   fDebugLevel;   //! Debug level 
  TList     *fPlotFuncList;//! plot functors list
  static TList     *fgTrendPoint;          //! trend point
  static TTreeSRedirector *fgDebugStream;  //! Debug stream 

  ClassDef(AliTRDrecoTask, 1) // base TRD reconstruction task
};

#endif


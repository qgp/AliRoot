///////////////////////////////////////////////////////////////////////////
//                                                                       //
// AliFemtoESDTrackCut: A basic track cut that used information from     //
// ALICE ESD to accept or reject the track.                              //  
// Enables the selection on charge, transverse momentum, rapidity,       //
// pid probabilities, number of ITS and TPC clusters                     //
// Author: Marek Chojnacki (WUT), mchojnacki@knf.pw.edu.pl               //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

#ifndef ALIFEMTOESDTRACKCUT_H
#define ALIFEMTOESDTRACKCUT_H

//#ifndef StMaker_H
//#include "StMaker.h"
//#endif

#include "AliFemtoTrackCut.h"

class AliFemtoESDTrackCut : public AliFemtoTrackCut 
{

 public:
  AliFemtoESDTrackCut();
  //~AliFemtoESDTrackCut();

  virtual bool Pass(const AliFemtoTrack* aTrack);

  virtual AliFemtoString Report();
  virtual TList *ListSettings();

  void SetPt(const float& lo, const float& hi);
  void SetRapidity(const float& lo, const float& hi);
  void SetCharge(const int&);
  void SetPidProbElectron(const float& lo, const float& hi);
  void SetPidProbPion(const float& lo, const float& hi);
  void SetPidProbKaon(const float& lo, const float& hi);
  void SetPidProbProton(const float& lo, const float& hi);
  void SetPidProbMuon(const float& lo, const float& hi);
  void SetLabel(const bool& flag);
  void SetStatus(const long& w);
  void SetminTPCclsF(const short& s);
  void SetminITScls(const int& s);
  void SetRemoveKinks(const bool& flag);

 private:   // here are the quantities I want to cut on...

  int               fCharge;             // particle charge
  float             fPt[2];              // bounds for transverse momentum
  float             fRapidity[2];        // bounds for rapidity
  float             fPidProbElectron[2]; // bounds for electron probability
  float             fPidProbPion[2];     // bounds for pion probability
  float             fPidProbKaon[2];     // bounds for kaon probability
  float             fPidProbProton[2];   // bounds for proton probability
  float             fPidProbMuon[2];     // bounds for muon probability 
  bool              fLabel;              // if true label<0 will not pass throught 
  long              fStatus;             // staus flag
  short             fminTPCclsF;         // min number of findable clusters in the TPC
  int               fminITScls;          // min number of clusters assigned in the ITS 
  long              fNTracksPassed;      // passed tracks count
  long              fNTracksFailed;      // failed tracks count
  bool              fRemoveKinks;        // if true particles with any kink label will not pass

#ifdef __ROOT__ 
  ClassDef(AliFemtoESDTrackCut, 1)
#endif
    };


inline void AliFemtoESDTrackCut::SetPt(const float& lo, const float& hi){fPt[0]=lo; fPt[1]=hi;}
inline void AliFemtoESDTrackCut::SetRapidity(const float& lo,const float& hi){fRapidity[0]=lo; fRapidity[1]=hi;}
inline void AliFemtoESDTrackCut::SetCharge(const int& ch){fCharge = ch;}
inline void AliFemtoESDTrackCut::SetPidProbElectron(const float& lo,const float& hi){fPidProbElectron[0]=lo; fPidProbElectron[1]=hi;}
inline void AliFemtoESDTrackCut::SetPidProbPion(const float& lo,const float& hi){fPidProbPion[0]=lo; fPidProbPion[1]=hi;}
inline void AliFemtoESDTrackCut::SetPidProbKaon(const float& lo,const float& hi){fPidProbKaon[0]=lo; fPidProbKaon[1]=hi;}
inline void AliFemtoESDTrackCut::SetPidProbProton(const float& lo,const float& hi){fPidProbProton[0]=lo; fPidProbProton[1]=hi;}
inline void AliFemtoESDTrackCut::SetPidProbMuon(const float& lo,const float& hi){fPidProbMuon[0]=lo; fPidProbMuon[1]=hi;}
inline void AliFemtoESDTrackCut::SetLabel(const bool& flag){fLabel=flag;}
inline void AliFemtoESDTrackCut::SetStatus(const long& status){fStatus=status;}
inline void AliFemtoESDTrackCut::SetminTPCclsF(const short& minTPCclsF){fminTPCclsF=minTPCclsF;}
inline void AliFemtoESDTrackCut::SetminITScls(const int& minITScls){fminITScls=minITScls;}

#endif


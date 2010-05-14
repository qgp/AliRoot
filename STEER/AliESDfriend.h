#ifndef ALIESDFRIEND_H
#define ALIESDFRIEND_H

//-------------------------------------------------------------------------
//                     Class AliESDfriend
//               This class contains ESD additions
//       Origin: Iouri Belikov, CERN, Jouri.Belikov@cern.ch 
//-------------------------------------------------------------------------

#include <TObject.h>
#include <TClonesArray.h>

#include "AliESDfriendTrack.h"

class AliESDVZEROfriend;
class AliESDTZEROfriend;

//_____________________________________________________________________________
class AliESDfriend : public TObject {
public:
  AliESDfriend();
  AliESDfriend(const AliESDfriend &);
  AliESDfriend& operator=(const AliESDfriend& esd);  
  virtual ~AliESDfriend();

  Int_t GetNumberOfTracks() const {return fTracks.GetEntriesFast();}
  AliESDfriendTrack *GetTrack(Int_t i) const {
     return (AliESDfriendTrack *)fTracks.UncheckedAt(i);
  }
  Int_t GetEntriesInTracks() const {return fTracks.GetEntries();}
  void AddTrack(const AliESDfriendTrack *t) {
     new(fTracks[fTracks.GetEntriesFast()]) AliESDfriendTrack(*t);
  }

  void AddTrackAt(const AliESDfriendTrack *t, Int_t i) {
     new(fTracks[i]) AliESDfriendTrack(*t);
  }

  void SetVZEROfriend(AliESDVZEROfriend * obj);
  AliESDVZEROfriend *GetVZEROfriend(){ return fESDVZEROfriend; }
  void SetTZEROfriend(AliESDTZEROfriend * obj);
  AliESDTZEROfriend *GetTZEROfriend(){ return fESDTZEROfriend; }

  void Ls(){
	  return fTracks.ls();
  }

protected:
  TClonesArray fTracks;    // ESD friend tracks
  AliESDVZEROfriend *fESDVZEROfriend; // VZERO object containing complete raw data
  AliESDTZEROfriend *fESDTZEROfriend; // TZERO calibration object

  ClassDef(AliESDfriend,3) // ESD friend
};

#endif



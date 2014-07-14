#ifndef ALIVVFRIENDEVENT_H
#define ALIVVFRIENDEVENT_H

#include "Rtypes.h"
class AliVVfriendTrack;

//_____________________________________________________________________________
class AliVVfriendEvent {
public:
  AliVVfriendEvent() {}
  virtual ~AliVVfriendEvent() {}

  virtual Int_t GetNclustersTPC(UInt_t /*sector*/) const {return 0;}
  virtual Int_t GetNclustersTPCused(UInt_t /*sector*/) const {return 0;}

  //used in calibration
  virtual Bool_t TestSkipBit() {return kFALSE;}
  virtual Int_t GetNumberOfTracks() const {return 0;}
  virtual const AliVVfriendTrack *GetTrack(Int_t /*i*/) const {return NULL;}

private: 
  AliVVfriendEvent(const AliVVfriendEvent &);
  AliVVfriendEvent& operator=(const AliVVfriendEvent& esd);  

  ClassDef(AliVVfriendEvent,1);
};

#endif



#ifndef ALIVFRIENDTRACK_H
#define ALIVFRIENDTRACK_H

//_____________________________________________________________________________
#include "AliVMisc.h"

#include "Rtypes.h"
class AliTPCseed;
class AliVVtrackPointArray;

//_____________________________________________________________________________
class AliVfriendTrack {
public:

  AliVfriendTrack(){}
  // constructor for reinitialisation of vtable
  AliVfriendTrack( AliVConstructorReinitialisationFlag ){}
  virtual ~AliVfriendTrack(){}

  //used in calibration
  
  virtual Int_t GetTPCseed( AliTPCseed &) const = 0;
  
  /*
  Int_t GetTrackParamTPCOut( AliExternalTrackParam &p ) const { return GetExternalTrackParam( p, 0x0  ); }
  Int_t GetTrackParamITSOut( AliExternalTrackParam &p ) const { return GetExternalTrackParam( p, 0x0  ); }
  Int_t GetTrackParamTRDIn( AliExternalTrackParam &p ) const { return GetExternalTrackParam( p, 0x0  ); }
  */

  //virtual const AliVtrackPointArray *GetTrackPointArray() const {return NULL;}

  // bit manipulation for filtering
  virtual void SetSkipBit(Bool_t skip) = 0;
  virtual Bool_t TestSkipBit() const = 0;

private: 
  AliVfriendTrack(const AliVfriendTrack &);
  AliVfriendTrack& operator=(const AliVfriendTrack& esd);  

};

#endif


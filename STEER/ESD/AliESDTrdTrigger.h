#ifndef ALIESDTRDTRIGGER_H
#define ALIESDTRDTRIGGER_H

#include "TObject.h"

class AliESDTrdTrigger : public TObject
{
 public:
  AliESDTrdTrigger();
  AliESDTrdTrigger(const AliESDTrdTrigger &rhs);
  AliESDTrdTrigger& operator=(const AliESDTrdTrigger &rhs);
  ~AliESDTrdTrigger();

  UInt_t GetFlags(const Int_t sector) const { return fFlags[sector]; }

  void SetFlags(const Int_t sector, const UInt_t flags) { fFlags[sector] = flags; }

  void SetTiming(const Int_t stack, const UInt_t timing) { fTiming[stack] = timing; }
  void SetLME(const Int_t stack, const UInt_t lmeFlags) { fLME[stack] = lmeFlags; }
  void SetTrackletEndmarker(const Int_t stack, const UShort_t trklEndm) { fTrackletEndmarker[stack] = trklEndm; }

  UInt_t   GetTiming(const Int_t stack) { return fTiming[stack]; }
  UInt_t   GetLME(const Int_t stack) { return fLME[stack]; }
  UShort_t GetTrackletEndmarker(const Int_t stack) { return fTrackletEndmarker[stack]; }

 protected:
  static const Int_t fgkNsectors = 18;	  // number of sectors
  static const Int_t fgkNstacks  = 90;	  // number of stacks

  UInt_t fFlags[fgkNsectors];	          // trigger flags for every sector

  UInt_t   fTiming[fgkNstacks];
  UInt_t   fLME[fgkNstacks];
  UShort_t fTrackletEndmarker[fgkNstacks];

  ClassDef(AliESDTrdTrigger, 3);
};

#endif

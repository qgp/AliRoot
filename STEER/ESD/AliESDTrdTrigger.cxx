#include "TObject.h"

#include "AliESDTrdTrigger.h"

AliESDTrdTrigger::AliESDTrdTrigger() : 
  TObject()
{
  // default ctor

  for (Int_t iSector = 0; iSector < fgkNsectors; iSector++) {
    for (Int_t iStack = 0; iStack < 5; iStack++) {
      fTiming[iSector*5 + iStack] = 0;
      fLME[iSector*5 + iStack] = 0;
      fTrackletEndmarker[iSector*5 + iStack] = 0;
    }
    fFlags[iSector] = 0x0;
  }
}

AliESDTrdTrigger::AliESDTrdTrigger(const AliESDTrdTrigger &rhs) :
  TObject(rhs)
{
  // copy ctor

  for (Int_t iSector = 0; iSector < fgkNsectors; iSector++) {
    fFlags[iSector] = rhs.fFlags[iSector];
    for (Int_t iStack = 0; iStack < 5; iStack++) {
      fTiming[iSector*5 + iStack] = rhs.fTiming[iSector*5 + iStack];
      fLME[iSector*5 + iStack] = rhs.fLME[iSector*5 + iStack];
      fTrackletEndmarker[iSector*5 + iStack] = rhs.fTrackletEndmarker[iSector*5 + iStack];
    }
  }
}

AliESDTrdTrigger& AliESDTrdTrigger::operator=(const AliESDTrdTrigger &rhs)
{
  // assignment operator
  if (&rhs != this) {
    TObject::operator=(rhs);
    for (Int_t iSector = 0; iSector < fgkNsectors; iSector++) {
      fFlags[iSector] = rhs.fFlags[iSector];
      for (Int_t iStack = 0; iStack < 5; iStack++) {
	fTiming[iSector*5 + iStack] = rhs.fTiming[iSector*5 + iStack];
	fLME[iSector*5 + iStack] = rhs.fLME[iSector*5 + iStack];
	fTrackletEndmarker[iSector*5 + iStack] = rhs.fTrackletEndmarker[iSector*5 + iStack];
      }
    }
  }

  return *this;
}

AliESDTrdTrigger::~AliESDTrdTrigger()
{
  // dtor

}

#ifndef ALI_ITS_ONLINESPDSCANMEANTH_H
#define ALI_ITS_ONLINESPDSCANMEANTH_H  

////////////////////////////////////////////////////////////
// Author: Henrik Tydesjo                                 //
// Interface class to the containers of an online mean    //
// threshold scan.                                        //
////////////////////////////////////////////////////////////

#include "AliITSOnlineSPDscanMultiple.h"

class AliITSOnlineSPDscanMeanTh :  public AliITSOnlineSPDscanMultiple {

 public:
  AliITSOnlineSPDscanMeanTh(){}
  AliITSOnlineSPDscanMeanTh(const Char_t *fileName, Bool_t readFromGridFile=kFALSE);
  AliITSOnlineSPDscanMeanTh(const AliITSOnlineSPDscanMeanTh& scan);
  virtual ~AliITSOnlineSPDscanMeanTh();
  AliITSOnlineSPDscanMeanTh& operator=(const AliITSOnlineSPDscanMeanTh& scan);

  virtual UInt_t AddScanStep();

  void     SetDacLow(UInt_t nsi, UInt_t hs, Int_t val);
  void     SetDacHigh(UInt_t nsi, UInt_t hs, Int_t val);
  void     SetTPAmp(UInt_t nsi, UInt_t hs, Int_t val);

  Int_t    GetDacLow(UInt_t nsi, UInt_t hs);
  Int_t    GetDacHigh(UInt_t nsi, UInt_t hs);
  Int_t    GetTPAmp(UInt_t nsi, UInt_t hs);

};
    
#endif

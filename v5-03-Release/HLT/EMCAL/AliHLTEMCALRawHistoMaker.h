/**************************************************************************
 * This file is property of and copyright by the ALICE HLT Project        *
 * All rights reserved.                                                   *
 * INFN Laboratori Nazionali di Frascati                                  *
 * Primary Authors: Federico Ronchetti                                    *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/
#ifndef ALIHLTEMCALRAWHISTOMAKER_H
#define ALIHLTEMCALRAWHISTOMAKER_H

/**
 * Class makes histograms from information from raw data
 *
 * @file   AliHLTEMCALRawHistoMaker.h
 * @author Federico Ronchetti
 * @date
 * @brief  Histo maker for EMCAL HLT
 */


#include "AliHLTCaloConstantsHandler.h"

#include "AliHLTCaloClusterDataStruct.h"
#include "AliHLTCaloChannelDataStruct.h"
#include "AliHLTCaloClusterReader.h"

#include "AliHLTDataTypes.h"


// Includes for RAW data 

//#include "AliAltroRawStreamV3.h"
//#include "AliRawReaderMemory.h"
//#include "AliAltroRawStreamV3.h"
//#include "AliCaloRawStreamV3.h"
//#include "AliEMCALTriggerSTURawStream.h"


// include root stuff
#include "TFile.h"
#include "TProfile2D.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH2I.h"
#include "TObjArray.h"
#include "TString.h"



class AliHLTEMCALConstants;
class AliHLTEMCALMapper;

class AliHLTCaloSharedMemoryInterfacev2;

class AliHLTCaloChannelDataHeaderStruct;
class AliHLTCaloClusterHeaderStruct;

class AliHLTCaloClusterReader;

//class AliHLTCaloRecPointHeaderStruct;

//class AliRawReaderMemory;
//class AliAltroRawStreamV3;
//class AliEMCALTriggerSTURawStream;
//class AliCaloRawStreamV3;
//class AliCaloRawAnalyzer;

class TString;
class TH2F;
class TH2I;


class AliHLTEMCALRawHistoMaker : AliHLTCaloConstantsHandler
{

public:
  /** Constructor */
  AliHLTEMCALRawHistoMaker();

  /** Destructor */
  virtual ~AliHLTEMCALRawHistoMaker();


  /**
   * Make the digits for one event.
   * @param channelDataHeader is the data header from the AliHLTCaloRawAnalyzer
   * @return the number of digits found
   */

  Int_t MakeHisto(AliHLTCaloChannelDataHeaderStruct* channelDataHeader,  AliHLTCaloClusterHeaderStruct *caloClusterHeaderPtr, int beverbose);

  TObjArray * GetHistograms();

private:

  TProfile2D **fAmp;
  TProfile2D **fTime;
  TH2F **fAT;

  TH2F **fCellVsEne;
  

  TObjArray *hList;

  /** Pointer to shared memory interface */
  AliHLTCaloSharedMemoryInterfacev2* fShmPtr;                    //! transient

  /** Pointer to the raw data reader which reads from memory */
  //   AliRawReaderMemory* fRawCounterMemoryPtr;            //!transient

   /** Pointer to the raw stream */
  //   AliAltroRawStreamV3* fAltroRawStreamPtr;               //!transient

   /** Pointer to the calo raw stream */
  // AliCaloRawStreamV3* fRawStreamPtr;              //!transient

   /** Pointer to the STU raw stream */
  // AliEMCALTriggerSTURawStream* fSTURawStreamPtr;

  /** Mapper */
  AliHLTEMCALMapper* fMapperPtr;                                  //COMMENT
 
  /** Constants class */
  AliHLTEMCALConstants* fEMCALConstants;

  AliHLTCaloClusterReader* fClusterReaderPtr; // !transient The reader

  AliHLTEMCALRawHistoMaker(const AliHLTEMCALRawHistoMaker &);
  AliHLTEMCALRawHistoMaker & operator = (const AliHLTEMCALRawHistoMaker &);
  
  ClassDef(AliHLTEMCALRawHistoMaker, 0); 

};


#endif
 

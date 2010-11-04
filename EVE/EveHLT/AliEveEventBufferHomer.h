//-*- Mode: C++ -*-

// $Id$


/* This file is property of and copyright by the ALICE HLT Project        * 
 * ALICE Experiment at CERN, All rights reserved.                         *
 * See cxx source for full Copyright notice     
 */

/** @file   AliEveEventBufferHomer.h
    @author Svein Lindal
    @date
    @brief  Manager for HOMER in aliroot
*/

#define BUFFERSIZE 15


#ifndef ALIEVEEVENTBUFFERHOMER_H
#define ALIEVEEVENTBUFFERHOMER_H

class AliEveHOMERManager;
#include "Rtypes.h"
#include "AliEveEventBuffer.h"

class TFile;
class TTree;
class AliESDEvent;
class TString;

class AliEveEventBufferHomer : public AliEveEventBuffer {

public:
  
  /** default constructor */
  AliEveEventBufferHomer();
  /** destructor */
  virtual ~AliEveEventBufferHomer();

  void ConnectToSource();
  
  void Initialize();

  AliEveHOMERManager * GetHomerManager() const { return fHomer;}

  TList * GetAList() { return fAsyncList;}

  void WriteToFile();


private:


  /** copy constructor prohibited */
  AliEveEventBufferHomer(const AliEveEventBufferHomer&);

  /** assignment operator prohibited */
  AliEveEventBufferHomer& operator=(const AliEveEventBufferHomer&);

  ///Inherited from AliEveEventBuffer
  TObject * GetEventFromSource();
  
  ULong64_t GetEventIdFromSource();


  ///Inherited form AliEveEventBuffer
  void AddToBuffer(TObject * event);
  AliEveHOMERManager * fHomer;
  Int_t fEventNo;
  TList * fAsyncList;



  ClassDef(AliEveEventBufferHomer, 0); 
};

#endif

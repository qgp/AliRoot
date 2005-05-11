#ifndef ALIITSBEAMTESTDIG_H
#define ALIITSBEAMTESTDIG_H

////////////////////////////////////////////////////
//  Class to define                               //
//  beam test raw 2 dig conv.                     //
//  Origin: E. Crescio crescio@to.infn.it         //
//  November 2004                                 //
////////////////////////////////////////////////////

#include "TTask.h"

class AliITS;
class AliRawReader;
class TTree;
class AliITSEventHeader;

class AliITSBeamTestDig: public TTask {
 
 public:

 
  AliITSBeamTestDig();
  AliITSBeamTestDig(const Text_t* name, const Text_t* title);
  AliITSBeamTestDig(const AliITSBeamTestDig& bt);
  AliITSBeamTestDig& operator=(const AliITSBeamTestDig &source);
  virtual ~AliITSBeamTestDig() {}
 
  void SetRawReader(AliRawReader* rd) {fReader=rd;}
  void SetTree(TTree* treedig) {fTreeD=treedig;}
  void SetITSEventHeader(AliITSEventHeader* header){fITSHeader = header;}

  void SetBeamTest(AliITS* bt) {fBt=bt;}


 protected:      
  
  AliITSEventHeader* fITSHeader;     // its event header
  AliRawReader* fReader;             // !reader ;
  TTree* fTreeD;                     // tree of digits
 
  AliITS* fBt;               // !beam test object

  ClassDef(AliITSBeamTestDig,3)   // its beam test digitization 

 };



#endif

    

#ifndef ALITAGANALYSIS_H
#define ALITAGANALYSIS_H
/*  See cxx source for full Copyright notice */


/* $Id$ */

//-------------------------------------------------------------------------
//                          Class AliTagAnalysis
//   This is the AliTagAnalysis class for the tag analysis
//
//    Origin: Panos Christakoglou, UOA-CERN, Panos.Christakoglou@cern.ch
//-------------------------------------------------------------------------



//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                        AliTagAnalysis                                //
//                                                                      //
//           Implementation of the tag analysis mechanism.              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


//ROOT
#include <TObject.h>

class AliEventTag;
class TChain;
class AliEventTagCuts;
class TGridResult;

class AliTagAnalysis : public TObject {
 public:
  AliTagAnalysis();
  ~AliTagAnalysis(); 

  void ChainLocalTags(const char *dirname);
  void ChainGridTags(TGridResult *result);
  TChain *QueryTags(AliEventTagCuts *EvTagCuts);
   
 protected:
  TGridResult *ftagresult; //the results from the tag grid query     
  TString fTagDirName; //the location of the locally stored tags
    
  static TChain *fgChain; //tag chain 
  TChain *fChain; //tag chain 
       
  ClassDef(AliTagAnalysis,0)  
};

#endif


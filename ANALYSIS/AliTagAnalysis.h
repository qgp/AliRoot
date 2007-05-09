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
class AliRunTagCuts;
class TGridResult;
class TTreeFormula;

//____________________________________________________//
class AliTagAnalysis : public TObject {
 public:
  AliTagAnalysis();
  ~AliTagAnalysis(); 
  
  Bool_t AddTagsFile(const char *alienUrl);
  void ChainLocalTags(const char *dirname);
  void ChainGridTags(TGridResult *result);
  
  TChain *QueryTags(AliRunTagCuts *RunTagCuts, AliEventTagCuts *EvTagCuts);
  TChain *QueryTags(const char *fRunCut, const char *fEventCut);  

  Bool_t CreateXMLCollection(const char* name, AliRunTagCuts *RunTagCuts, AliEventTagCuts *EvTagCuts);
  Bool_t CreateXMLCollection(const char* name, const char *fRunCut, const char *fEventCut);

  TChain *GetInputChain(const char* system, const char *wn);
  TChain *GetChainFromCollection(const char* collectionname, const char* treename);
  
  //____________________________________________________//
 protected:
  TGridResult *ftagresult; //the results from the tag grid query     
  TString fTagDirName; //the location of the locally stored tags
  
  TChain *fChain; //tag chain 
  
  //____________________________________________________//
 private:
  AliTagAnalysis(const AliTagAnalysis & source);
  AliTagAnalysis & operator=(const AliTagAnalysis & source);
       
  ClassDef(AliTagAnalysis,0)  
};

#endif


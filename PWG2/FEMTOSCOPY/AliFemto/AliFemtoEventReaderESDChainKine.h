////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// AliFemtoEventReaderESDChainKine - the reader class for the Alice ESD and   //
// the model Kinematics information tailored for the Task framework and the   //
// Reads in AliESDfriend to create shared hit/quality information             //
// Authors: Adam Kisiel kisiel@mps.ohio-state.edu                             //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
#ifndef ALIFEMTOEVENTREADERESDCHAINKINE_H
#define ALIFEMTOEVENTREADERESDCHAINKINE_H

#include "AliFemtoEventReader.h"
#include "AliFemtoEnumeration.h"

#include <string>
#include <vector>
#include <TTree.h>
#include <AliESDEvent.h>
#include <AliESDfriend.h>
#include <AliStack.h>
#include <list>

class AliFemtoEvent;

class AliFemtoEventReaderESDChainKine : public AliFemtoEventReader 
{
 public:
  AliFemtoEventReaderESDChainKine();
  AliFemtoEventReaderESDChainKine(const AliFemtoEventReaderESDChainKine& aReader);
  ~AliFemtoEventReaderESDChainKine();

  AliFemtoEventReaderESDChainKine& operator=(const AliFemtoEventReaderESDChainKine& aReader);

  AliFemtoEvent* ReturnHbtEvent();
  AliFemtoString Report();
  void SetConstrained(const bool constrained);
  bool GetConstrained() const;

  void SetESDSource(AliESDEvent *aESD);
  void SetStackSource(AliStack *aStack);

 protected:

 private:
  string         fFileName;      // name of current ESD file
  bool           fConstrained;   // flag to set which momentum from ESD file will be use
  int            fNumberofEvent; // number of Events in ESD file
  int            fCurEvent;      // number of current event
  unsigned int   fCurFile;       // number of current file
  AliESDEvent*   fEvent;         // ESD event
  AliStack      *fStack;         // Kinematics stack pointer

#ifdef __ROOT__
  ClassDef(AliFemtoEventReaderESDChainKine, 1)
#endif

    };
  
#endif



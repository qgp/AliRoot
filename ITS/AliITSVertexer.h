#ifndef ALIITSVERTEXER_H
#define ALIITSVERTEXER_H

#include <TTree.h>
#include <TFile.h>
#include <AliITSVertex.h>
#include <AliRun.h>


///////////////////////////////////////////////////////////////////
//                                                               //
// Base class for primary vertex reconstruction                  //
//                                                               //
///////////////////////////////////////////////////////////////////



class AliITSVertexer : public TObject {

 public:
    // default constructor
    AliITSVertexer();   
    // standard constructor     
    AliITSVertexer(TString filename); 
    // destructor
    virtual ~AliITSVertexer(); 
    // computes the vertex for the current event
    virtual AliITSVertex* FindVertexForCurrentEvent(Int_t evnumb)=0; 
    // computes the vetex for each event and stores it on file
    virtual void FindVertices()= 0;
    virtual void PrintStatus() const = 0;
    virtual void SetDebug(Int_t debug = 0){fDebug = debug;}
    virtual void SetFirstEvent(Int_t ev){fFirstEvent = ev;}
    virtual void SetLastEvent(Int_t ev){fLastEvent = ev;}
    virtual void WriteCurrentVertex();

 
 protected:
    AliITSVertex *fCurrentVertex;  //! pointer to the vertex of the current
                                   //  event
    Int_t fFirstEvent;          // First event to be processed by FindVertices
    Int_t fLastEvent;           // Last event to be processed by FindVertices 
    Int_t fDebug;               //! debug flag - verbose printing if >0

  ClassDef(AliITSVertexer,1);
};

#endif

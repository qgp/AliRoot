#include <AliITSVertex.h>
#include <AliITSVertexer.h>
#include <AliRunLoader.h>
#include <AliITSLoader.h>

ClassImp(AliITSVertexer)

//////////////////////////////////////////////////////////////////////
// Base class for primary vertex reconstruction                     //
// AliITSVertex is a class for full 3D primary vertex finding       //
// derived classes: AliITSVertexerIons AliITSvertexerPPZ            //
//                  AliITSVertexerTracks                            //
//////////////////////////////////////////////////////////////////////

//______________________________________________________________________
AliITSVertexer::AliITSVertexer() {
  // Default Constructor

    fCurrentVertex  = 0;
    SetDebug();
    SetFirstEvent(0);
    SetLastEvent(0);
}

AliITSVertexer::AliITSVertexer(TString filename) {
  // Standard constructor
  AliRunLoader *rl = AliRunLoader::GetRunLoader();
  if(!rl){
    Fatal("AliITSVertexer","Run Loader not found");
  }
  if(rl->LoadgAlice()){
    Fatal("AliITSVertexer","The AliRun object is not available - nothing done");
  }
  fCurrentVertex  = 0;   
  SetDebug();
  SetFirstEvent(0);
  SetLastEvent(0);
  rl->LoadHeader();
  AliITSLoader* itsLoader =  (AliITSLoader*) rl->GetLoader("ITSLoader");
  if(!filename.Contains("default"))itsLoader->SetVerticesFileName(filename);
  itsLoader->LoadVertices("recreate");
  itsLoader->LoadRecPoints();
  Int_t lst;
  if(rl->TreeE()){
    lst = static_cast<Int_t>(rl->TreeE()->GetEntries());
    SetLastEvent(lst-1);
  }
}

//______________________________________________________________________
AliITSVertexer::AliITSVertexer(const AliITSVertexer &vtxr) : TObject(vtxr) {
  // Copy constructor
  // Copies are not allowed. The method is protected to avoid misuse.
  Error("AliITSVertexer","Copy constructor not allowed\n");
}

//______________________________________________________________________
AliITSVertexer& AliITSVertexer::operator=(const AliITSVertexer& /* vtxr */){
  // Assignment operator
  // Assignment is not allowed. The method is protected to avoid misuse.
  Error("= operator","Assignment operator not allowed\n");
  return *this;
}

//______________________________________________________________________
AliITSVertexer::~AliITSVertexer() {
  // Default Destructor
  // The objects pointed by the following pointers are not owned
  // by this class and are not deleted

    fCurrentVertex  = 0;
}

//______________________________________________________________________
void AliITSVertexer::WriteCurrentVertex(){
  // Write the current AliVertex object to file fOutFile
  AliRunLoader *rl = AliRunLoader::GetRunLoader();
  AliITSLoader* itsLoader =  (AliITSLoader*) rl->GetLoader("ITSLoader");
  fCurrentVertex->SetName("Vertex");
  //  const char * name = fCurrentVertex->GetName();
  //  itsLoader->SetVerticesContName(name);
  Int_t rc = itsLoader->PostVertex(fCurrentVertex);
  rc = itsLoader->WriteVertices();
}

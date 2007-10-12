////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// AliFemtoBasicEventCut - the basic cut for events.                          //
// Only cuts on event multiplicity and z-vertex position                      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "AliFemtoBasicEventCut.h"
//#include <cstdio>

#ifdef __ROOT__
ClassImp(AliFemtoBasicEventCut)
#endif

AliFemtoBasicEventCut::AliFemtoBasicEventCut() :
  fNEventsPassed(0), fNEventsFailed(0)
{
  // Default constructor
} 
//------------------------------
AliFemtoBasicEventCut::~AliFemtoBasicEventCut(){
  // Default destructor
}
//------------------------------
bool AliFemtoBasicEventCut::Pass(const AliFemtoEvent* event){
  // Pass events if they fall within the multiplicity and z-vertex
  // position range. Fail otherwise
  int mult =  event->NumberOfTracks();
  double vertexZPos = event->PrimVertPos().z();
  cout << "AliFemtoBasicEventCut:: mult:       " << fEventMult[0] << " < " << mult << " < " << fEventMult[1] << endl;
  cout << "AliFemtoBasicEventCut:: VertexZPos: " << fVertZPos[0] << " < " << vertexZPos << " < " << fVertZPos[1] << endl;
  bool goodEvent =
    ((mult > fEventMult[0]) && 
     (mult < fEventMult[1]) && 
     (vertexZPos > fVertZPos[0]) &&
     (vertexZPos < fVertZPos[1]));
  goodEvent ? fNEventsPassed++ : fNEventsFailed++ ;
  cout << "AliFemtoBasicEventCut:: return : " << goodEvent << endl;
  return (goodEvent);
}
//------------------------------
AliFemtoString AliFemtoBasicEventCut::Report(){
  // Prepare report
  string stemp;
  char ctemp[100];
  sprintf(ctemp,"\nMultiplicity:\t %d-%d",fEventMult[0],fEventMult[1]);
  stemp = ctemp;
  sprintf(ctemp,"\nVertex Z-position:\t %E-%E",fVertZPos[0],fVertZPos[1]);
  stemp += ctemp;
  sprintf(ctemp,"\nNumber of events which passed:\t%ld  Number which failed:\t%ld",fNEventsPassed,fNEventsFailed);
  stemp += ctemp;
  AliFemtoString returnThis = stemp;
  return returnThis;
}

////////////////////////////////////////////////////////////////////////////////
///                                                                          ///
/// AliFemtoCutMonitorParticleTrue - the cut monitor for particles to study  ///
/// the difference between reconstructed and true momentum    ///
///                                                                          ///
////////////////////////////////////////////////////////////////////////////////
#ifndef AliFemtoCutMonitorParticleTrue_hh
#define AliFemtoCutMonitorParticleTrue_hh

class AliFemtoEvent;
class AliFemtoTrack;
class AliFemtoV0;
class AliFemtoKink;
class AliFemtoPair; // Gael 12/04/02
class TH1D;
#include "AliFemtoString.h"
#include "AliFemtoParticleCollection.h"
#include "AliFemtoCutMonitor.h"

class AliFemtoCutMonitorParticleTrue : public AliFemtoCutMonitor{
  
public:
  AliFemtoCutMonitorParticleTrue();
  AliFemtoCutMonitorParticleTrue(const char *aName);
  AliFemtoCutMonitorParticleTrue(const AliFemtoCutMonitorParticleTrue &aCut);
  virtual ~AliFemtoCutMonitorParticleTrue();

  AliFemtoCutMonitorParticleTrue& operator=(const AliFemtoCutMonitorParticleTrue& aCut);

  virtual AliFemtoString Report();
  virtual void Fill(const AliFemtoTrack* aTrack);
  void Write();

private:
  TH1D *fMomValue;             // Histogram in total momentum difference
  TH1D *fMomX;                 // Histogram in x-component difference
  TH1D *fMomY;                 // Histogram in y-component difference
  TH1D *fMomZ;                 // Histogram in z-component difference
};

#endif

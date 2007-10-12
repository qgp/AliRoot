////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// AliFemtoCutMonitorParticleTrue - the cut monitor for particles to study    //
// the difference between reconstructed and true momentum                     //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
#include <TH1D.h>
#include "AliFemtoCutMonitorParticleTrue.h"
#include "AliFemtoModelHiddenInfo.h"

AliFemtoCutMonitorParticleTrue::AliFemtoCutMonitorParticleTrue()
{
  // Default constructor
  fMomValue = new TH1D("MomValue", "Total momentum difference", 200, -0.5, 0.5);
  fMomX = new TH1D("MomX", "X-component momentum difference", 200, -0.2, 0.2);             
  fMomY = new TH1D("MomY", "Y-component momentum difference", 200, -0.2, 0.2);             
  fMomZ = new TH1D("MomZ", "Z-component momentum difference", 200, -0.2, 0.2);              

}

AliFemtoCutMonitorParticleTrue::AliFemtoCutMonitorParticleTrue(const char *aName)
{
  // Normal constructor
  char name[200];
  snprintf(name, 200, "MomValue%s", aName);
  fMomValue = new TH1D(name, "Total momentum difference", 200, -0.5, 0.5);
  snprintf(name, 200, "MomX%s", aName);
  fMomX = new TH1D(name, "X-component momentum difference", 200, -0.2, 0.2);             
  snprintf(name, 200, "MomY%s", aName);
  fMomY = new TH1D(name, "Y-component momentum difference", 200, -0.2, 0.2);             
  snprintf(name, 200, "MomZ%s", aName);
  fMomZ = new TH1D(name, "Z-component momentum difference", 200, -0.2, 0.2);              
  
}

AliFemtoCutMonitorParticleTrue::AliFemtoCutMonitorParticleTrue(const AliFemtoCutMonitorParticleTrue &aCut)
{
  // copy constructor
  if (fMomValue) delete fMomValue;
  fMomValue = new TH1D(*aCut.fMomValue);
  if (fMomX) delete fMomX;
  fMomX = new TH1D(*aCut.fMomX);
  if (fMomY) delete fMomY;
  fMomY = new TH1D(*aCut.fMomY);
  if (fMomZ) delete fMomZ;
  fMomZ = new TH1D(*aCut.fMomZ);
}

AliFemtoCutMonitorParticleTrue::~AliFemtoCutMonitorParticleTrue()
{
  // Destructor
  delete fMomValue;
  delete fMomX;
  delete fMomY;
  delete fMomZ;
}

AliFemtoCutMonitorParticleTrue& AliFemtoCutMonitorParticleTrue::operator=(const AliFemtoCutMonitorParticleTrue& aCut)
{
  // assignment operator
  if (this == &aCut) 
    return *this;

  if (fMomValue) delete fMomValue;
  fMomValue = new TH1D(*aCut.fMomValue);
  if (fMomX) delete fMomX;
  fMomX = new TH1D(*aCut.fMomX);
  if (fMomY) delete fMomY;
  fMomY = new TH1D(*aCut.fMomY);
  if (fMomZ) delete fMomZ;
  fMomZ = new TH1D(*aCut.fMomZ);
  
  return *this;
}

AliFemtoString AliFemtoCutMonitorParticleTrue::Report(){ 
  // Prepare report from the execution
  string stemp = "*** AliFemtoCutMonitorParticleTrue report"; 
  AliFemtoString returnThis = stemp;
  return returnThis; 
}

void AliFemtoCutMonitorParticleTrue::Fill(const AliFemtoTrack* aTrack)
{
  // Fill in the monitor histograms with the values from the current track
  AliFemtoModelHiddenInfo *tInfo = (AliFemtoModelHiddenInfo *) aTrack->GetHiddenInfo();
  if (tInfo) {
    double tMomFull = sqrt(aTrack->P().x()*aTrack->P().x() +
			   aTrack->P().y()*aTrack->P().y() +
			   aTrack->P().z()*aTrack->P().z());
    double tTrueFull = sqrt(tInfo->GetTrueMomentum()->x()*tInfo->GetTrueMomentum()->x() +
			    tInfo->GetTrueMomentum()->y()*tInfo->GetTrueMomentum()->y() +
			    tInfo->GetTrueMomentum()->z()*tInfo->GetTrueMomentum()->z());
    fMomValue->Fill((tTrueFull-tMomFull)/tTrueFull);
    fMomX->Fill(tInfo->GetTrueMomentum()->x()-aTrack->P().x());
    fMomY->Fill(tInfo->GetTrueMomentum()->y()-aTrack->P().y());
    fMomZ->Fill(tInfo->GetTrueMomentum()->z()-aTrack->P().z());
  }
    
}

void AliFemtoCutMonitorParticleTrue::Write()
{
  // Write out the relevant histograms
  fMomValue->Write();
  fMomX->Write();
  fMomY->Write();
  fMomZ->Write();
}


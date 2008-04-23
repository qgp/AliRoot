// ALICE event generator based on the THERMINATOR model
// It reads the test output of the model and puts it onto
// the stack
// It has an option to use the Lhyquid3D input freeze-out
// hypersurface
// Author: Adam.Kisiel@cern.ch

#include <iostream>
#include <fstream>
#include <sstream>
#include <TClonesArray.h>
#include <TMCProcess.h>
#include <TDatabasePDG.h>
#include <TParticle.h>

#include "AliConst.h"
#include "AliDecayer.h"
#include "AliGenEventHeader.h"
#include "AliGenHijingEventHeader.h"
#include "AliGenTherminator.h"
#include "AliLog.h"
#include "AliRun.h"

ClassImp(AliGenTherminator)

using namespace std;

AliGenTherminator::AliGenTherminator():
  AliGenMC(),
  fNt(0),
  fEventNumber(0),
  fFileName(""),
  fFreezeOutModel(""),
  fTemperature(0.1656),
  fMiuI(-0.0009),
  fMiuS(0.0),
  fMiuB(0.0008),
  fAlfaRange(6.0),
  fRapRange(4.0),
  fRhoMax(8.0),
  fTau(8.0),
  fBWA(0.0),
  fBWVt(1.41),
  fBWDelay(0.0)
{
  // Default constructor
  fParticles = new TClonesArray("TParticle",20000);    
}
AliGenTherminator::AliGenTherminator(Int_t npart):
  AliGenMC(npart),
  fNt(0),
  fEventNumber(0),
  fFileName(""),
  fFreezeOutModel(""),
  fTemperature(0.1656),
  fMiuI(-0.0009),
  fMiuS(0.0),
  fMiuB(0.0008),
  fAlfaRange(6.0),
  fRapRange(4.0),
  fRhoMax(8.0),
  fTau(8.0),
  fBWA(0.0),
  fBWVt(1.41),
  fBWDelay(0.0)
{
  // Constructor specifying the size of the particle table
  fParticles = new TClonesArray("TParticle",20000);    
  fNprimaries = 0;
}

AliGenTherminator::~AliGenTherminator()
{
  //  AliGenMC::~AliGenMC();
  //  if (fTherminator) delete fTherminator;
}

void AliGenTherminator::Generate()
{
  // Run single event generation with the Therminator model
  AliWarning("Generating event from AliGenTherminator");

  Float_t polar[3]    =   {0,0,0};
  Float_t origin[3]   =   {0,0,0};
  Float_t origin0[3]  =   {0,0,0};
  Float_t p[3];
  Float_t mass, energy;

  Int_t nt  = 0;
  Int_t j, kf, ks, imo;
  kf = 0;

  Vertex();
  for (j=0; j < 3; j++) origin0[j] = fVertex[j];

  // Generate one event

  ((TTherminator *) fMCEvGen)->GenerateEvent();
  AliWarning("Generated");
  ((TTherminator *) fMCEvGen)->ImportParticles(fParticles);

  Int_t np = fParticles->GetEntriesFast();
  AliWarning(Form("Imported %d particles", np));

  TParticle *iparticle;
  Double_t evrot = gRandom->Rndm()*TMath::Pi();
  
  for (int i = 0; i < np; i++) {
    iparticle = (TParticle *) fParticles->At(i);
    Bool_t  hasMother   = (iparticle->GetFirstMother()     >=0);
    Bool_t  hasDaughter = (iparticle->GetFirstDaughter()   >=0);
    
    if (hasDaughter) {
      // This particle has decayed
      // It will not be tracked
      // Add it only once with coorduinates not
      // smeared with primary vertex position

      kf   = iparticle->GetPdgCode();
      ks   = iparticle->GetStatusCode();
      Double_t aphi = TMath::ATan2(iparticle->Py(), iparticle->Px());
      Double_t arho = TMath::Hypot(iparticle->Px(), iparticle->Py());
      p[0] = arho*TMath::Cos(aphi + evrot);
      p[1] = arho*TMath::Sin(aphi + evrot);
      //     p[0] = iparticle->Px();
      //     p[1] = iparticle->Py();
      p[2] = iparticle->Pz();
      mass = TDatabasePDG::Instance()->GetParticle(kf)->Mass();
      energy = sqrt(mass*mass + p[0]*p[0] + p[1]*p[1] + p[2]*p[2]);
      
      Double_t vphi = TMath::ATan2(iparticle->Vy(), iparticle->Vx());
      Double_t vrho = TMath::Hypot(iparticle->Vx(), iparticle->Vy());
      origin[0] = origin0[0]+vrho*TMath::Cos(vphi + evrot);
      origin[1] = origin0[1]+vrho*TMath::Sin(vphi + evrot);
      origin[2] = origin0[2]+iparticle->Vz();
      
      imo = -1;
      TParticle* mother = 0;
      if (hasMother) {
	imo = iparticle->GetFirstMother();
	mother = (TParticle *) fParticles->At(imo);
      } // if has mother   
      Bool_t tFlag = (!hasDaughter);
      
      printf("Pushing Track %d with status %d mother %d\n", kf, tFlag, imo);
      PushTrack(tFlag,imo,kf,
		p[0],p[1],p[2],energy,
		origin[0],origin[1],origin[2],iparticle->T()*0.197327*1e-13/300000000,
		polar[0],polar[1],polar[2],
		hasMother ? kPDecay:kPNoProcess,nt);
      fNprimaries++;
      KeepTrack(nt);
    }
    else {
      // This is a final state particle
      // It will be tracked
      // Add it TWICE to the stack !!!
      // First time with event-wide coordicates (for femtoscopy) - 
      //   this one will not be tracked
      // Second time with event-wide ccordiantes and vertex smearing
      //   this one will be tracked
      
      kf   = iparticle->GetPdgCode();
      ks   = iparticle->GetStatusCode();
      Double_t aphi = TMath::ATan2(iparticle->Py(), iparticle->Px());
      Double_t arho = TMath::Hypot(iparticle->Px(), iparticle->Py());
      p[0] = arho*TMath::Cos(aphi + evrot);
      p[1] = arho*TMath::Sin(aphi + evrot);
      //     p[0] = iparticle->Px();
      //     p[1] = iparticle->Py();
      p[2] = iparticle->Pz();
      mass = TDatabasePDG::Instance()->GetParticle(kf)->Mass();
      energy = sqrt(mass*mass + p[0]*p[0] + p[1]*p[1] + p[2]*p[2]);
      
      Double_t vphi = TMath::ATan2(iparticle->Vy(), iparticle->Vx());
      Double_t vrho = TMath::Hypot(iparticle->Vx(), iparticle->Vy());
      origin[0] = vrho*TMath::Cos(vphi + evrot);
      origin[1] = vrho*TMath::Sin(vphi + evrot);
      origin[2] = iparticle->Vz();
      
      imo = -1;
      TParticle* mother = 0;
      if (hasMother) {
	imo = iparticle->GetFirstMother();
	mother = (TParticle *) fParticles->At(imo);
      } // if has mother   
      Bool_t tFlag = (hasDaughter);
      
      printf("Pushing Track %d with status %d mother %d\n", kf, tFlag, imo);
      PushTrack(tFlag,imo,kf,
		p[0],p[1],p[2],energy,
		origin[0],origin[1],origin[2],iparticle->T()*0.197327*1e-13/300000000,
		polar[0],polar[1],polar[2],
		hasMother ? kPDecay:kPNoProcess,nt);
      fNprimaries++;
      KeepTrack(nt);

      origin[0] = origin0[0]+vrho*TMath::Cos(vphi + evrot);
      origin[1] = origin0[1]+vrho*TMath::Sin(vphi + evrot);
      origin[2] = origin0[2]+iparticle->Vz();

      imo = nt;
      mother = (TParticle *) fParticles->At(nt);
      tFlag = (!hasDaughter);
      
      printf("Pushing Track %d with status %d mother %d\n", kf, tFlag, imo);
      PushTrack(tFlag,imo,kf,
		p[0],p[1],p[2],energy,
		origin[0],origin[1],origin[2],iparticle->T()*0.197327*1e-13/300000000,
		polar[0],polar[1],polar[2],
		hasMother ? kPDecay:kPNoProcess,nt);
      fNprimaries++;
      KeepTrack(nt);
    }
  }

  SetHighWaterMark(fNprimaries);

  TArrayF eventVertex;
  eventVertex.Set(3);
  eventVertex[0] = origin0[0];
  eventVertex[1] = origin0[1];
  eventVertex[2] = origin0[2];

// Builds the event header, to be called after each event
  AliGenEventHeader* header = new AliGenHijingEventHeader("Therminator");

  // Header
  //  AliGenEventHeader* header = new AliGenEventHeader("Therminator");
  // Event Vertex
//   header->SetPrimaryVertex(eventVertex);
//   header->SetNProduced(fNprimaries);

  ((AliGenHijingEventHeader*) header)->SetNProduced(fNprimaries);
  ((AliGenHijingEventHeader*) header)->SetPrimaryVertex(eventVertex);
  ((AliGenHijingEventHeader*) header)->SetImpactParameter(0.0);
  ((AliGenHijingEventHeader*) header)->SetTotalEnergy(0.0);
  ((AliGenHijingEventHeader*) header)->SetHardScatters(0);
  ((AliGenHijingEventHeader*) header)->SetParticipants(0, 0);
  ((AliGenHijingEventHeader*) header)->SetCollisions(0, 0, 0, 0);
  ((AliGenHijingEventHeader*) header)->SetSpectators(0, 0, 0, 0);
  ((AliGenHijingEventHeader*) header)->SetReactionPlaneAngle(evrot);


// 4-momentum vectors of the triggered jets.
//
// Before final state gluon radiation.
//     TLorentzVector* jet1 = new TLorentzVector(fHijing->GetHINT1(21), 
// 					      fHijing->GetHINT1(22),
// 					      fHijing->GetHINT1(23),
// 					      fHijing->GetHINT1(24));

//     TLorentzVector* jet2 = new TLorentzVector(fHijing->GetHINT1(31), 
// 					      fHijing->GetHINT1(32),
// 					      fHijing->GetHINT1(33),
// 					      fHijing->GetHINT1(34));
// // After final state gluon radiation.
//     TLorentzVector* jet3 = new TLorentzVector(fHijing->GetHINT1(26), 
// 					      fHijing->GetHINT1(27),
// 					      fHijing->GetHINT1(28),
// 					      fHijing->GetHINT1(29));

//     TLorentzVector* jet4 = new TLorentzVector(fHijing->GetHINT1(36), 
// 					      fHijing->GetHINT1(37),
// 					      fHijing->GetHINT1(38),
// 					      fHijing->GetHINT1(39));
//     ((AliGenHijingEventHeader*) header)->SetJets(jet1, jet2, jet3, jet4);
// Bookkeeping for kinematic bias
//     ((AliGenHijingEventHeader*) header)->SetTrials(fTrials);
// Event Vertex
  header->SetPrimaryVertex(fVertex);
  AddHeader(header);
  fCollisionGeometry = (AliGenHijingEventHeader*)  header;

  //  gAlice->SetGenEventHeader(header); 
}

void AliGenTherminator::Init()
{
  // Initialize global variables and
  // particle and decay tables
  if (fFileName.Length() == 0)
    fFileName = "event.out";
  ReadShareParticleTable();

  SetMC(new TTherminator());
  
  AliGenMC::Init();
  ((TTherminator *) fMCEvGen)->Initialize();
}
void AliGenTherminator::SetFileName(const char *infilename)
{
  // Set parameter filename
  fFileName = infilename;
}
void AliGenTherminator::SetEventNumberInFile(int evnum)
{
  // Set number of events to generate - default: 1
  fEventNumber = evnum;
}

void AliGenTherminator::ReadShareParticleTable()
{
  // Read in particle table from share
  // and add missing particle type to TDatabasePDG

  char str[50];
  char str1[200];
    
  TDatabasePDG *tInstance = TDatabasePDG::Instance();
  TParticlePDG *tParticleType;

  AliWarning(Form("Reading particle types from particles.data"));

  TString aroot = gSystem->Getenv("ALICE_ROOT");
  ifstream in((aroot+"/TTherminator/data/SHARE/particles.data").Data());
  //  ifstream in("particles.data");
  
  int charge;
    
  int number=0;
  if ((in) && (in.is_open()))
    {
      //START OF HEAD-LINE
      in.ignore(200,'\n');
      in.ignore(200,'\n');
      in.ignore(200,'\n');
      //END OF HEAD-LINE
      
      while (in>>str)
	{
	  if (/*(*str == '#')||*/(*str<65)||(*str>122))
	    {
	      in.getline(str1,200);
	      continue;
	    }
	  double mass, gamma, spin, tI3, tI, q, s, aq, as, c, ac, mc;
	  
	  in>>mass>>gamma>>spin>>tI>>tI3>>q>>s>>aq>>as>>c>>ac>>mc;
	  number++;
	  tParticleType = tInstance->GetParticle((int) mc);
	  if (!tParticleType) {
	    charge = 0;
	    if (strstr(str, "plu")) charge = 1;
	    if (strstr(str, "min")) charge = -1;
	    if (strstr(str, "plb")) charge = -1;
	    if (strstr(str, "mnb")) charge = 1;
	    if (strstr(str, "plp")) charge = 2;
	    if (strstr(str, "ppb")) charge = -2;
	    tInstance->AddParticle(str, str, mass, gamma == 0.0 ? 1:0, gamma, charge , "meson", (int) mc);
	    AliWarning(Form("Added particle %s with PDG PID %d charge %d", str, (int) mc, charge));
	    //	    AliWarning(Form("Quantum numbers q s c aq as ac tI3 %lf %lf %lf %lf %lf %lf %lf", q, s, c, aq, as, ac, tI3));

	  }
	}
      in.close();
    }
  CreateTherminatorInputFile();
}

void AliGenTherminator::CreateTherminatorInputFile()
{
  // Create Therminator input file
  const char *aroot = gSystem->Getenv("ALICE_ROOT");
  ofstream *ostr = new ofstream("therminator.in");
  (*ostr) << "NumberOfEvents = 1" << endl;
  (*ostr) << "Randomize = 1" << endl;
  (*ostr) << "TableType = SHARE" << endl;
  (*ostr) << "InputDirSHARE = "<< aroot << "/TTherminator/data/SHARE" << endl;
  (*ostr) << "EventOutputFile = " << fFileName.Data() << endl;
  (*ostr) << "FOHSLocation = " << fFOHSlocation.Data() << endl;
  (*ostr) << "FreezeOutModel = " << fFreezeOutModel.Data() << endl;
  (*ostr) << "BWVt = " << fBWVt << endl;
  (*ostr) << "Tau = " << fTau << endl;
  (*ostr) << "RhoMax = " << fRhoMax << endl;
  (*ostr) << "Temperature = " << fTemperature << endl;
  (*ostr) << "MiuI = " << fMiuI << endl;
  (*ostr) << "MiuS = " << fMiuS << endl;
  (*ostr) << "MiuB = " << fMiuB << endl;
  (*ostr) << "AlphaRange = " << fAlfaRange << endl;
  (*ostr) << "RapidityRange = " << fRapRange << endl;
  (*ostr) << "NumberOfIntegrateSamples = 1000000" << endl;
}

void AliGenTherminator::SetModel(const char *model)
{
  // Set the freeze-out model to use
  fFreezeOutModel = model;
}

void AliGenTherminator::SetLhyquidSet(const char *set)
{
  // Select one of pregenerated Lhyquid hypersurfaces
  const char *aroot = gSystem->Getenv("ALICE_ROOT");
  if (strstr(set, "LHC500C0005")) {
    AliWarning(Form("AliGenTherminator: Selected default Lhyquid hypersurface"));
    AliWarning(Form("  Pb-Pb collisions, centrality 0-5%"));
    AliWarning(Form("  initial temperature at tau=1 fm in the center Ti=500 MeV"));
    AliWarning(Form("  freeze-out criteria Tf=145 MeV"));
    AliWarning(Form("  for details see $(ALICE_ROOT)/TTherminator/data/LHC500C0005/FO.txt"));
    fFOHSlocation = aroot;
    fFOHSlocation += "/TTherminator/data/LHC500C0005";
  }
  else if (strstr(set, "LHC500C2030")) {
    AliWarning(Form("AliGenTherminator: Selected default Lhyquid hypersurface"));
    AliWarning(Form("  Pb-Pb collisions, centrality 20-30%"));
    AliWarning(Form("  initial temperature at tau=1 fm in the center Ti=500 MeV"));
    AliWarning(Form("  freeze-out criteria Tf=145 MeV"));
    AliWarning(Form("  for details see $(ALICE_ROOT)/TTherminator/data/LHC500C2030/FO.txt"));
    fFOHSlocation = aroot;
    fFOHSlocation += "/TTherminator/data/LHC500C2030";
  }
  else {
    AliWarning(Form("Did not find Lhyquid set %s", set));
    AliWarning(Form("Reverting to default: current directory"));
    fFOHSlocation = "";
  }
}

void AliGenTherminator::SetLhyquidInputDir(const char *inputdir)
{
  // Select Your own Lhyquid hypersurface
  fFOHSlocation = inputdir;
}

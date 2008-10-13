//
// Class AliRsnDaughter
//
// Light-weight AOD object which contains all required track details
// which are used for resonance analysis.
// Provides converters from all kinds of input track type: ESD, AOD and MC.
//
// authors: A. Pulvirenti (alberto.pulvirenti@ct.infn.it)
//          M. Vala (martin.vala@cern.ch)
//

#include <Riostream.h>

#include <TParticle.h>
#include <TString.h>

#include "AliLog.h"
#include "AliESDtrack.h"
#include "AliAODTrack.h"
#include "AliMCParticle.h"

#include "AliRsnMCInfo.h"
#include "AliRsnDaughter.h"

ClassImp(AliRsnDaughter)

AliRsnDaughter::EPIDMethod AliRsnDaughter::fgPIDMethod = AliRsnDaughter::kNoPID;

//_____________________________________________________________________________
AliRsnDaughter::AliRsnDaughter() :
    AliVParticle(),
    fIndex(-1),
    fLabel(-1),
    fCharge(0),
    fFlags(0),
    fKink(0),
    fMass(0.0),
    fChi2(0.0),
    fNSigmaToVertex(-1.0),
    fITSnum(0),
    fTPCnum(0),
    fRealisticPID(AliRsnPID::kUnknown),
    fMCInfo(0x0)
{
//
// Default constructor.
// Initializes all data-members with meaningless values.
//

  Int_t i;
  for (i = 0; i < AliRsnPID::kSpecies; i++)
  {
    if (i < 3)
    {
      fP[i] = 0.0;
      fV[i] = 0.0;
    }
    fPIDWeight[i] = 0.0;
    fPIDProb[i] = 0.0;
  }
}

//_____________________________________________________________________________
AliRsnDaughter::AliRsnDaughter(const AliRsnDaughter &copy) :
    AliVParticle(copy),
    fIndex(copy.fIndex),
    fLabel(copy.fLabel),
    fCharge(copy.fCharge),
    fFlags(copy.fFlags),
    fKink(copy.fKink),
    fMass(copy.fMass),
    fChi2(copy.fChi2),
    fNSigmaToVertex(copy.fNSigmaToVertex),
    fITSnum(copy.fITSnum),
    fTPCnum(copy.fTPCnum),
    fRealisticPID(copy.fRealisticPID),
    fMCInfo(0x0)
{
//
// Copy constructor.
//

  Int_t i;
  for (i = 0; i < AliRsnPID::kSpecies; i++)
  {
    if (i < 3)
    {
      fP[i] = copy.fP[i];
      fV[i] = copy.fV[i];
    }
    fPIDWeight[i] = copy.fPIDWeight[i];
    fPIDProb[i] = copy.fPIDProb[i];
  }

  // initialize particle object
  // only if it is present in the template object
  if (copy.fMCInfo) fMCInfo = new AliRsnMCInfo(*(copy.fMCInfo));
}

//_____________________________________________________________________________
AliRsnDaughter::AliRsnDaughter(AliESDtrack *track, Bool_t useTPC) :
    AliVParticle(),
    fIndex(-1),
    fLabel(-1),
    fCharge(0),
    fFlags(0),
    fKink(0),
    fMass(0.0),
    fChi2(0.0),
    fNSigmaToVertex(-1.0),
    fITSnum(0),
    fTPCnum(0),
    fRealisticPID(AliRsnPID::kUnknown),
    fMCInfo(0x0)
{
//
// Constructor to get data from an ESD track.
//

  Int_t i;
  for (i = 0; i < AliRsnPID::kSpecies; i++) fPIDProb[i] = 0.0;
  Adopt(track, kEsd,0.0,useTPC);
}

//_____________________________________________________________________________
AliRsnDaughter::AliRsnDaughter(AliAODTrack *track) :
    AliVParticle(),
    fIndex(-1),
    fLabel(-1),
    fCharge(0),
    fFlags(0),
    fKink(0),
    fMass(0.0),
    fChi2(0.0),
    fNSigmaToVertex(-1.0),
    fITSnum(0),
    fTPCnum(0),
    fRealisticPID(AliRsnPID::kUnknown),
    fMCInfo(0x0)
{
//
// Constructor to get data from an AOD track.
//

  Int_t i;
  for (i = 0; i < AliRsnPID::kSpecies; i++) fPIDProb[i] = 0.0;
  Adopt(track);
}

//_____________________________________________________________________________
AliRsnDaughter::AliRsnDaughter(AliMCParticle *track) :
    AliVParticle(),
    fIndex(-1),
    fLabel(-1),
    fCharge(0),
    fFlags(0),
    fKink(0),
    fMass(0.0),
    fChi2(0.0),
    fNSigmaToVertex(-1.0),
    fITSnum(0),
    fTPCnum(0),
    fRealisticPID(AliRsnPID::kUnknown),
    fMCInfo(0x0)
{
//
// Constructor to get data from an MC track.
//

  Int_t i;
  for (i = 0; i < AliRsnPID::kSpecies; i++) fPIDProb[i] = 0.0;
  Adopt(track);
}

//_____________________________________________________________________________
AliRsnDaughter& AliRsnDaughter::operator=(const AliRsnDaughter &copy)
{
//
// Assignment operator.
// Works like the copy constructor and returns a reference
// to the initialized object for which it is called.
//

  fIndex  = copy.fIndex;
  fLabel  = copy.fLabel;
  fCharge = copy.fCharge;
  fFlags  = copy.fFlags;
  fKink   = copy.fKink;
  fChi2   = copy.fChi2;
  fNSigmaToVertex = copy.fNSigmaToVertex;
  fITSnum = copy.fITSnum;
  fTPCnum = copy.fTPCnum;

  Int_t i;
  for (i = 0; i < AliRsnPID::kSpecies; i++)
  {
    if (i < 3)
    {
      fP[i] = copy.fP[i];
      fV[i] = copy.fV[i];
    }
    fPIDWeight[i] = copy.fPIDWeight[i];
    fPIDProb[i] = copy.fPIDProb[i];
  }

  fMass = copy.fMass;
  fRealisticPID = copy.fRealisticPID;

  // initialize particle object
  // only if it is present in the template object;
  // otherwise, it is just cleared and not replaced with anything
  if (fMCInfo)
  {
    delete fMCInfo;
    fMCInfo = 0x0;
  }
  if (copy.fMCInfo) fMCInfo = new AliRsnMCInfo(*(copy.fMCInfo));

  return (*this);
}

//_____________________________________________________________________________
AliRsnDaughter::~AliRsnDaughter()
{
//
// Destructor
//

  if (fMCInfo)
  {
    delete fMCInfo;
    fMCInfo = 0;
  }
}

//_____________________________________________________________________________
void AliRsnDaughter::RotateP(Double_t angle)
{
//
// Rotate the momentum along Z axis by an angle
//

  Double_t s = TMath::Sin(angle);
  Double_t c = TMath::Cos(angle);
  Double_t xx = fP[0];
  fP[0] = c*xx - s*fP[1];
  fP[1] = s*xx + c*fP[1];
}

//_____________________________________________________________________________
void AliRsnDaughter::SetPIDWeight(Int_t i, Double_t value)
{
//
// I the argument 'i' is in the correct range,
// sets the i-th PID weight to 'value'
//

  if (i >= 0 && i < AliRsnPID::kSpecies) fPIDWeight[i] = value;
  else
  {
    AliError(Form("Cannot set a weight related to slot %d", i));
  }
}

//_____________________________________________________________________________
void  AliRsnDaughter::AssignRealisticPID()
{
//
// Assign realistic PID from largest probability
//

  Int_t i, imax = 0;
  Double_t pmax = fPIDProb[0];

  // search for maximum
  for (i = 1; i < AliRsnPID::kSpecies; i++)
  {
    if (fPIDProb[i] > pmax)
    {
      imax = i;
      pmax = fPIDProb[i];
    }
  }

  fRealisticPID = (AliRsnPID::EType)imax;
}

//_____________________________________________________________________________
AliRsnPID::EType AliRsnDaughter::PIDType(Double_t &prob) const
{
//
// Return the PID type according to the selected method
// in the argument passed by reference, the probability is stored.
// It will be realistic for realistic PID and 1 for perfect PID.
//

  switch (fgPIDMethod)
  {
    case kNoPID:
      AliWarning("Requested a PIDtype call in NoPID mode");
      return AliRsnPID::kUnknown;
    case kPerfect:
      if (fMCInfo) return AliRsnPID::InternalType(fMCInfo->PDG());
      else return AliRsnPID::kUnknown;
    default:
      if (fRealisticPID > 0 && fRealisticPID < AliRsnPID::kSpecies)
      {
        prob = fPIDProb[fRealisticPID];
        return fRealisticPID;
      }
      else
      {
        prob = 1.0;
        return AliRsnPID::kUnknown;
      }
  }
}

//_____________________________________________________________________________
void AliRsnDaughter::SetPIDProb(Int_t i, Double_t value)
{
//
// I the argument 'i' is in the correct range,
// sets the i-th PID probability to 'value'
//

  if (i >= 0 && i < AliRsnPID::kSpecies) fPIDProb[i] = value;
  else
  {
    AliError(Form("Cannot set a weight related to slot %d", i));
  }
}

//_____________________________________________________________________________
Bool_t AliRsnDaughter::Adopt(AliESDtrack* esdTrack,EPIDType pidType ,Double_t divValue, Bool_t useTPCInnerParam)
{
//
// Copies data from an AliESDtrack into "this":
//  - charge sign
//  - momentum
//  - point of closest approach to primary vertex
//  - ESD pid weights
// In case of errors returns kFALSE, otherwise kTRUE.
//

  if (!esdTrack)
  {
    AliError("Passed NULL object: nothing done");
    return kFALSE;
  }

  // copy momentum, vertex, chi2 and number of clusters
  if (!useTPCInnerParam)
  {
    esdTrack->GetPxPyPz(fP);
    esdTrack->GetXYZ(fV);
    fChi2 = esdTrack->GetConstrainedChi2();
    fITSnum = esdTrack->GetITSclusters(0x0);
    fTPCnum = esdTrack->GetTPCclusters(0x0);
  }
  else
  {
    if (!esdTrack->GetTPCInnerParam()) return kFALSE;
    esdTrack->GetTPCInnerParam()->GetPxPyPz(fP);
    esdTrack->GetTPCInnerParam()->GetXYZ(fV);
    fChi2 = esdTrack->GetTPCchi2();
    fITSnum = 0;
    fTPCnum = esdTrack->GetTPCclusters(0x0);
  }

  // define the kink index:
  //  0 = no kink
  //  1 = kink daughter
  // -1 = kink mother
  Int_t i, ik[3];
  for (i = 0; i < 3; i++) ik[i] = esdTrack->GetKinkIndex(i);
  if (ik[0] < 0 || ik[1] < 0 || ik[2] < 0) fKink = -1;
  else if (ik[0] > 0 || ik[1] > 0 || ik[2] > 0) fKink = 1;
  else fKink = 0;

  Double_t p = esdTrack->P();
  // copy PID weights
  Double_t pid[5];
  if (!useTPCInnerParam)
  {
    GetESDPID(esdTrack,pid,pidType,divValue,p);
  }
  else
  {
//     fCurrentESDPID = kTPC;
    GetESDPID(esdTrack,pid,kTPC,0.0,p);
  }
  for (i = 0; i < 5; i++) fPIDWeight[i] = pid[i];

  // copy flags
  fFlags = esdTrack->GetStatus();

  // copy charge sign
  fCharge = (Short_t)esdTrack->Charge();

  // calculate N sigma to vertex
  SetNSigmaToVertex(fESDTrackCuts.GetSigmaToVertex(esdTrack));


  return kTRUE;
}


//_____________________________________________________________________________
Bool_t AliRsnDaughter::Adopt(AliAODTrack* aodTrack)
{
//
// Copies data from an AliAODtrack into "this":
//  - charge sign
//  - momentum
//  - point of closest approach to primary vertex
//  - ESD pid weights
// In case of errors returns kFALSE, otherwise kTRUE.
//

  if (!aodTrack)
  {
    AliError("Passed NULL object: nothing done");
    return kFALSE;
  }

  // copy momentum  and vertex
  fP[0] = aodTrack->Px();
  fP[1] = aodTrack->Py();
  fP[2] = aodTrack->Pz();
  fV[0] = aodTrack->Xv();
  fV[1] = aodTrack->Yv();
  fV[2] = aodTrack->Zv();

  // chi2
  fChi2 = aodTrack->Chi2perNDF();

  // copy PID weights
  Int_t i;
  for (i = 0; i < 5; i++) fPIDWeight[i] = aodTrack->PID()[i];

  // copy flags
  fFlags = aodTrack->GetStatus();

  // copy sign
  fCharge = aodTrack->Charge();

  return kTRUE;
}


//_____________________________________________________________________________
Bool_t AliRsnDaughter::Adopt(AliMCParticle *mcParticle)
{
//
// Copies data from a MCParticle into "this":
//  - charge sign
//  - momentum
//  - point of closest approach to primary vertex
//  - ESD pid weights
//  - true PDG code
//  - mother
// In case of errors returns kFALSE, otherwise kTRUE.
//

  if (!mcParticle)
  {
    AliError("Passed NULL object: nothing done");
    return kFALSE;
  }

  // retrieve the TParticle object from the argument
  TParticle *particle = mcParticle->Particle();
  if (!particle)
  {
    AliError("AliMCParticle::Particle() returned NULL");
    return kFALSE;
  }

  // copy momentum  and vertex
  fP[0] = particle->Px();
  fP[1] = particle->Py();
  fP[2] = particle->Pz();
  fV[0] = particle->Vx();
  fV[1] = particle->Vy();
  fV[2] = particle->Vz();

  // recognize charge sign from PDG code sign
  Int_t pdg = particle->GetPdgCode();
  Int_t absPDG = TMath::Abs(pdg);
  if (absPDG == 11 || absPDG == 13)
  {
    if (pdg > 0) fCharge = -1; else fCharge = 1;
  }
  else if (absPDG == 211 || absPDG == 321 || absPDG == 2212)
  {
    if (pdg > 0) fCharge = 1; else fCharge = -1;
  }
  else
  {
    // when trying to "adopt" a neutral track (photon, neutron, etc.)
    // for the moment a "failed" message is returned
    fCharge = 0;
    return kFALSE;
  }

  // flags and PID weights make no sense with MC tracks
  fFlags = 0;
  for (pdg = 0; pdg < AliRsnPID::kSpecies; pdg++) fPIDWeight[pdg] = 0.0;
  fPIDWeight[AliRsnPID::InternalType(absPDG)] = 1.0;

  // copy other MC info (mother PDG code cannot be retrieved here)
  InitMCInfo(particle);

  return kTRUE;
}

//_____________________________________________________________________________
void AliRsnDaughter::Print(Option_t *option) const
{
//
// Prints the values of data members, using the options:
// - P --> momentum
// - V --> DCA vertex
// - C --> electric charge
// - F --> flags
// - I --> identification (PID, probability and mass)
// - W --> PID weights
// - M --> Montecarlo (from AliRsnMCInfo)
// - L --> index & label
// - A --> angles
// - ALL --> All oprions switched on
//
// Index and label are printed by default.
//

  TString opt(option);
  opt.ToUpper();

  if (opt.Contains("L") || opt.Contains("ALL"))
  {
    cout << ".......Index            : " << fIndex << endl;
    cout << ".......Label            : " << fLabel << endl;
  }
  if (opt.Contains("P") || opt.Contains("ALL"))
  {
    cout << ".......Px, Py, Pz, Pt   : " << Px() << ' ' << Py() << ' ' << Pz() << ' ' << Pt() << endl;
  }
  if (opt.Contains("A") || opt.Contains("ALL"))
  {
    cout << ".......Phi, Theta       : " << Phi() << ' ' << Theta() << endl;
  }
  if (opt.Contains("V") || opt.Contains("ALL"))
  {
    cout << ".......Vx, Vy, Vz       : " << Xv() << ' ' << Yv() << ' ' << Zv() << endl;
  }
  if (opt.Contains("I") || opt.Contains("ALL"))
  {
    AliRsnPID::EType type;
    Double_t prob;
    type = PIDType(prob);
    cout << ".......PID & prob       : " << AliRsnPID::ParticleName(type) << ' ' << prob << endl;
  }
  if (opt.Contains("C") || opt.Contains("ALL"))
  {
    cout << ".......Charge           : " << fCharge << endl;
  }
  if (opt.Contains("F") || opt.Contains("ALL"))
  {
    cout << ".......Flags            : " << fFlags << endl;
  }
  if (opt.Contains("W") || opt.Contains("ALL"))
  {
    cout << ".......Weights          : ";
    Int_t i;
    for (i = 0; i < AliRsnPID::kSpecies; i++) cout << fPIDWeight[i] << ' ';
    cout << endl;
  }
  if (opt.Contains("M") || opt.Contains("ALL"))
  {
    if (fMCInfo)
    {
      cout << ".......PDG code         : " << fMCInfo->PDG() << endl;
      cout << ".......Mother (label)   : " << fMCInfo->Mother() << endl;
      cout << ".......Mother (PDG code): " << fMCInfo->MotherPDG() << endl;
    }
    else
    {
      cout << ".......MC info not present" << endl;
    }
  }
}

//_____________________________________________________________________________
void AliRsnDaughter::InitMCInfo()
{
//
// Initializes the particle object with default constructor.
//

  fMCInfo = new AliRsnMCInfo;
}

//_____________________________________________________________________________
Bool_t AliRsnDaughter::InitMCInfo(TParticle *particle)
{
//
// Copies data from an MC particle into the object which
// contains all MC details taken from kinematics info.
// If requested by second argument, momentum and vertex
// of the Particle are copied into the 'fP' and 'fV'
// data members, to simulate a perfect reconstruction.
// If something goes wrong, returns kFALSE,
// otherwise returns kTRUE.
//

  // retrieve the TParticle object pointed by this MC track
  if (!particle)
  {
    AliError("Passed NULL particle object");
    return kFALSE;
  }

  // initialize object if not initialized yet
  if (fMCInfo) delete fMCInfo;
  fMCInfo = new AliRsnMCInfo;
  fMCInfo->Adopt(particle);

  return kTRUE;
}

//_____________________________________________________________________________
Bool_t AliRsnDaughter::InitMCInfo(AliMCParticle *mcParticle)
{
//
// Copies data from an MC particle into the object which
// contains all MC details taken from kinematics info.
// If requested by second argument, momentum and vertex
// of the Particle are copied into the 'fP' and 'fV'
// data members, to simulate a perfect reconstruction.
// If something goes wrong, returns kFALSE,
// otherwise returns kTRUE.
//

  // retrieve the TParticle object pointed by this MC track
  TParticle *particle = mcParticle->Particle();
  return InitMCInfo(particle);
}

//_____________________________________________________________________________
Int_t AliRsnDaughter::Compare(const TObject* obj) const
{
//
// Compare two tracks with respect to their transverse momentum.
// Citation from ROOT reference:
// "Must return -1 if this is smaller than obj, 0 if objects are equal
//  and 1 if this is larger than obj".
//

  AliRsnDaughter *that = (AliRsnDaughter*)obj;
  if (Pt() < that->Pt()) return 1;
  else if (Pt() > that->Pt()) return -1;
  else return 0;
}

//_____________________________________________________________________________
void AliRsnDaughter::GetESDPID
(AliESDtrack * track, Double_t * pid, EPIDType pidType, Double_t divValue, Double_t val)
{
//
// A simple method to define PID weights by combining the ones
// from single detectors by multiplication.
// In the case when 'kITS_TPC_TOF_SP' option is chosen, a separation value
// defines if the TOF signal must be included or not.
// This value is usually the transverse momentum, but here it is left undefined.
//

  Int_t i;
  Double_t ctmp[AliRsnPID::kSpecies];

  switch (pidType)
  {
    case kEsd :
      track->GetESDpid(pid);
      break;
    case kITS :
      track->GetITSpid(pid);
      break;
    case kTPC :
      track->GetTPCpid(pid);
      break;
    case kTOF :
      track->GetTOFpid(pid);
      break;
    case kITS_TPC :
      track->GetITSpid(pid);
      track->GetTPCpid(ctmp);
      for (i = 0; i < 5; i++) pid[i] *= ctmp[i];
      break;
    case kITS_TOF :
      track->GetITSpid(pid);
      track->GetTOFpid(ctmp);
      for (i = 0; i < AliRsnPID::kSpecies; i++) pid[i] *= ctmp[i];
      break;
    case kTPC_TOF :
      track->GetTPCpid(pid);
      track->GetTOFpid(ctmp);
      for (i = 0; i < AliRsnPID::kSpecies; i++) pid[i] *= ctmp[i];
      break;
    case kITS_TPC_TOF :
      track->GetITSpid(pid);
      track->GetTPCpid(ctmp);
      for (i = 0; i < AliRsnPID::kSpecies; i++) pid[i] *= ctmp[i];
      track->GetTOFpid(ctmp);
      for (i = 0; i < AliRsnPID::kSpecies; i++) pid[i] *= ctmp[i];
      break;
    case kITS_TPC_TOF_SP :
      if (val < divValue)
      {
        track->GetITSpid(pid);
        track->GetTPCpid(ctmp);
        for (i = 0; i < AliRsnPID::kSpecies; i++) pid[i] *= ctmp[i];
      }
      else
      {
        track->GetTOFpid(pid);
      }
      break;
    default:
      AliLog::Message(AliLog::kError,
                      "Unrecognized value of EPIDType argument",
                      "", "AliRsnDaughter", "GetESDPID", __FILE__, __LINE__);
      for (i = 0; i < AliRsnPID::kSpecies; i++) pid[i] = 1.0;
      break;
  }
}

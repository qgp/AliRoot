//
// Class AliRsnCutTrackQuality
//
// General implementation of a single cut strategy, which can be:
// - a value contained in a given interval  [--> IsBetween()   ]
// - a value equal to a given reference     [--> MatchesValue()]
//
// In all cases, the reference value(s) is (are) given as data members
// and each kind of cut requires a given value type (Int, UInt, Double),
// but the cut check procedure is then automatized and chosen thanks to
// an enumeration of the implemented cut types.
// At the end, the user (or any other point which uses this object) has
// to use the method IsSelected() to check if this cut has been passed.
//
// authors: Martin Vala (martin.vala@cern.ch)
//          Alberto Pulvirenti (alberto.pulvirenti@ct.infn.it)
//

#include <Riostream.h>
#include <TFormula.h>
#include <TBits.h>

#include "AliLog.h"
#include "AliESDtrack.h"
#include "AliAODTrack.h"
#include "AliESDtrackCuts.h"

#include "AliRsnEvent.h"
#include "AliRsnDaughter.h"
#include "AliRsnCutTrackQuality.h"

ClassImp(AliRsnCutTrackQuality)

//_________________________________________________________________________________________________
AliRsnCutTrackQuality::AliRsnCutTrackQuality(const char *name) :
  AliRsnCut(name, AliRsnCut::kDaughter, 0.0, 0.0),
  fFlagsOn(0xFFFFFFFF),
  fFlagsOff(0x0),
  fRejectKinkDaughter(kFALSE),
  fDCARfixed(kTRUE),
  fDCARptFormula(""),
  fDCARmax(fgkVeryBig),
  fDCAZfixed(kTRUE),
  fDCAZptFormula(""),
  fDCAZmax(fgkVeryBig),
  fSPDminNClusters(0),
  fITSminNClusters(0),
  fITSmaxChi2(fgkVeryBig),
  fTPCminNClusters(0),
  fTPCmaxChi2(fgkVeryBig)
{
//
// Default constructor.
// Initializes all cuts in such a way that all of them are disabled.
//

  SetPtRange(0.0, fgkVeryBig);
  SetEtaRange(-fgkVeryBig, fgkVeryBig);
}

//_________________________________________________________________________________________________
AliRsnCutTrackQuality::AliRsnCutTrackQuality(const AliRsnCutTrackQuality &copy) :
  AliRsnCut(copy),
  fFlagsOn(copy.fFlagsOn),
  fFlagsOff(copy.fFlagsOff),
  fRejectKinkDaughter(copy.fRejectKinkDaughter),
  fDCARfixed(copy.fDCARfixed),
  fDCARptFormula(copy.fDCARptFormula),
  fDCARmax(copy.fDCARmax),
  fDCAZfixed(copy.fDCAZfixed),
  fDCAZptFormula(copy.fDCAZptFormula),
  fDCAZmax(copy.fDCAZmax),
  fSPDminNClusters(copy.fSPDminNClusters),
  fITSminNClusters(copy.fITSminNClusters),
  fITSmaxChi2(copy.fITSmaxChi2),
  fTPCminNClusters(copy.fTPCminNClusters),
  fTPCmaxChi2(copy.fTPCmaxChi2)
{
//
// Copy constructor.
// Just copy all data member values.
//

  SetPtRange(copy.fPt[0], copy.fPt[1]);
  SetEtaRange(copy.fEta[0], copy.fEta[1]);
}

//_________________________________________________________________________________________________
AliRsnCutTrackQuality& AliRsnCutTrackQuality::operator=(const AliRsnCutTrackQuality &copy)
{
//
// Assignment operator.
// Just copy all data member values.
//

  
  fFlagsOn = copy.fFlagsOn;
  fFlagsOff = copy.fFlagsOff;
  fRejectKinkDaughter = copy.fRejectKinkDaughter;
  fDCARfixed = copy.fDCARfixed;
  fDCARptFormula = copy.fDCARptFormula;
  fDCARmax = copy.fDCARmax;
  fDCAZfixed = copy.fDCAZfixed;
  fDCAZptFormula = copy.fDCAZptFormula;
  fDCAZmax = copy.fDCAZmax;
  fSPDminNClusters = copy.fSPDminNClusters;
  fITSminNClusters = copy.fITSminNClusters;
  fITSmaxChi2 = copy.fITSmaxChi2;
  fTPCminNClusters = copy.fTPCminNClusters;
  fTPCmaxChi2 = copy.fTPCmaxChi2;

  SetPtRange(copy.fPt[0], copy.fPt[1]);
  SetEtaRange(copy.fEta[0], copy.fEta[1]);
  
  return (*this);
}

//_________________________________________________________________________________________________
void AliRsnCutTrackQuality::DisableAll()
{
//
// Disable all cuts
//
  
  fFlagsOn = 0xFFFFFFFF;
  fFlagsOff = 0x0;
  fRejectKinkDaughter = kFALSE;
  fDCARfixed = kTRUE;
  fDCARptFormula = "";
  fDCARmax = fgkVeryBig;
  fDCAZfixed = kTRUE;
  fDCAZptFormula = "";
  fDCAZmax = fgkVeryBig;
  fSPDminNClusters = 0;
  fITSminNClusters = 0;
  fITSmaxChi2 = fgkVeryBig;
  fTPCminNClusters = 0;
  fTPCmaxChi2 = fgkVeryBig;

  SetPtRange(0.0, fgkVeryBig);
  SetEtaRange(-fgkVeryBig, fgkVeryBig);
}

//_________________________________________________________________________________________________
Bool_t AliRsnCutTrackQuality::IsSelected(TObject *object)
{
//
// Cut checker.
// Checks the type of object being evaluated
// and then calls the appropriate sub-function (for ESD or AOD)
//

  // coherence check
  if (!TargetOK(object)) return kFALSE;
  
  // status is checked in the same way for all tracks
  AliVTrack *vtrack = dynamic_cast<AliVTrack*>(fDaughter->GetRef());
  if (!vtrack)
  {
    AliDebug(AliLog::kDebug + 2, Form("This object is not either an ESD nor AOD track, it is an %s", fDaughter->GetRef()->ClassName()));
    return kFALSE;
  }
  ULong_t status   = (ULong_t)vtrack->GetStatus();
  ULong_t checkOn  = status & fFlagsOn;
  ULong_t checkOff = status & fFlagsOff;
  if (checkOn == 0)
  {
    AliDebug(AliLog::kDebug + 2, Form("Not all required flags are present: required %lx, track has %lx", fFlagsOn, status));
    return kFALSE;
  }
  if (checkOff != 0)
  {
    AliDebug(AliLog::kDebug + 2, Form("Some forbidden flags are present: required %lx, track has %lx", fFlagsOff, status));
    return kFALSE;
  }
  
  // retrieve real object type
  AliESDtrack *esdTrack = fDaughter->GetRefESDtrack();
  AliAODTrack *aodTrack = fDaughter->GetRefAODtrack();
  if (esdTrack) 
  {
    AliDebug(AliLog::kDebug + 2, "Checking an ESD track");
    return CheckESD(esdTrack);
  }
  else if (aodTrack)
  {
    AliDebug(AliLog::kDebug + 2, "Checking an AOD track");
    return CheckAOD(aodTrack);
  }
  else
  {
    AliDebug(AliLog::kDebug + 2, Form("This object is not either an ESD nor AOD track, it is an %s", fDaughter->GetRef()->ClassName()));
    return kFALSE;
  }
}

//_________________________________________________________________________________________________
Bool_t AliRsnCutTrackQuality::CheckESD(AliESDtrack *track)
{
//
// Check an ESD track.
// This is done using the default track checker for ESD.
// It is declared static, not to recreate it every time.
//

  static AliESDtrackCuts cuts;
  
  // general acceptance/pt cuts
  cuts.SetPtRange (fPt[0], fPt[1]);
  cuts.SetEtaRange(fEta[0], fEta[1]);
  
  // transverse DCA cuts
  if (fDCARfixed)
    cuts.SetMaxDCAToVertexXY(fDCARmax);
  else
    cuts.SetMaxDCAToVertexXYPtDep(fDCARptFormula.Data());
    
  // longitudinal DCA cuts
  if (fDCAZfixed)
    cuts.SetMaxDCAToVertexZ(fDCARmax);
  else
    cuts.SetMaxDCAToVertexZPtDep(fDCAZptFormula.Data());
    
  // these options are always disabled in currend version
  cuts.SetDCAToVertex2D(kFALSE);
  cuts.SetRequireSigmaToVertex(kFALSE);
  
  // TPC related cuts for TPC+ITS tracks
  cuts.SetMinNClustersTPC(fTPCminNClusters);
  cuts.SetMaxChi2PerClusterTPC(fTPCmaxChi2);
  cuts.SetAcceptKinkDaughters(!fRejectKinkDaughter);
  
  // ITS related cuts for TPC+ITS tracks
  if (fSPDminNClusters > 0)
    cuts.SetClusterRequirementITS(AliESDtrackCuts::kSPD, AliESDtrackCuts::kAny);
  
  // now that all is initialized, do the check
  return cuts.IsSelected(track);
}

//_________________________________________________________________________________________________
Bool_t AliRsnCutTrackQuality::CheckAOD(AliAODTrack *track)
{
//
// Check an AOD track.
// This is done doing directly all checks, since there is not
// an equivalend checker for AOD tracks
//
  
  // step #0: check SPD and ITS clusters
  Int_t nSPD = 0;
  nSPD  = TESTBIT(track->GetITSClusterMap(), 0);
  nSPD += TESTBIT(track->GetITSClusterMap(), 1);
  if (nSPD < fSPDminNClusters)
  {
    AliDebug(AliLog::kDebug + 2, "Not enough SPD clusters in this track. Rejected");
    return kFALSE;
  }

  // step #1: check number of clusters in TPC
  if (track->GetTPCNcls() < fTPCminNClusters)
  {
    AliDebug(AliLog::kDebug + 2, "Too few TPC clusters. Rejected");
    return kFALSE;
  }
  if (track->GetITSNcls() < fITSminNClusters)
  {
    AliDebug(AliLog::kDebug + 2, "Too few ITS clusters. Rejected");
    return kFALSE;
  }
  
  // step #2: check chi square
  if (track->Chi2perNDF() > fTPCmaxChi2)
  {
    AliDebug(AliLog::kDebug + 2, "Bad chi2. Rejected");
    return kFALSE;
  }
  if (track->Chi2perNDF() > fITSmaxChi2)
  {
    AliDebug(AliLog::kDebug + 2, "Bad chi2. Rejected");
    return kFALSE;
  }
  
  // step #3: reject kink daughters
  AliAODVertex *vertex = track->GetProdVertex();
  if (vertex && fRejectKinkDaughter)
  {
    if (vertex->GetType() == AliAODVertex::kKink)
    {
      AliDebug(AliLog::kDebug + 2, "Kink daughter. Rejected");
      return kFALSE;
    }
  }
  
  // step #4: DCA cut (transverse)
  Double_t b[2], cov[3];
  vertex = AliRsnTarget::GetCurrentEvent()->GetRefAOD()->GetPrimaryVertex();
  if (!vertex)
  {
    AliDebug(AliLog::kDebug + 2, "NULL vertex");
    return kFALSE;
  }
  if (!track->PropagateToDCA(vertex, AliRsnTarget::GetCurrentEvent()->GetRefAOD()->GetMagneticField(), kVeryBig, b, cov))
  {
    AliDebug(AliLog::kDebug + 2, "Failed propagation to vertex");
    return kFALSE;
  }
  // if the DCA cut is not fixed, compute current value
  if (!fDCARfixed)
  {
    static TString str(fDCARptFormula);
    str.ReplaceAll("pt", "x");
    static const TFormula dcaXY(Form("%s_dcaXY", GetName()), str.Data());
    fDCARmax = dcaXY.Eval(track->Pt());
  }
  // check the cut
  if (TMath::Abs(b[0]) > fDCARmax)
  {
    AliDebug(AliLog::kDebug + 2, "Too large transverse DCA");
    return kFALSE;
  }
  
  // step #5: DCA cut (longitudinal)
  // the DCA has already been computed above
  // if the DCA cut is not fixed, compute current value
  if (!fDCAZfixed)
  {
    static TString str(fDCAZptFormula);
    str.ReplaceAll("pt", "x");
    static const TFormula dcaZ(Form("%s_dcaXY", GetName()), str.Data());
    fDCAZmax = dcaZ.Eval(track->Pt());
  }
  // check the cut
  if (TMath::Abs(b[1]) > fDCAZmax)
  {
    AliDebug(AliLog::kDebug + 2, "Too large longitudinal DCA");
    return kFALSE;
  }
  
  // step #6: check eta/pt range
  if (track->Eta() < fEta[0] || track->Eta() > fEta[1])
  {
    AliDebug(AliLog::kDebug + 2, "Outside ETA acceptance");
    return kFALSE;
  }
  if (track->Pt() < fPt[0] || track->Pt() > fPt[1])
  {
    AliDebug(AliLog::kDebug + 2, "Outside PT acceptance");
    return kFALSE;
  }
  
  // if we are here, all cuts were passed and no exit point was got
  return kTRUE;
}

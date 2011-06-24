#include "KiddiePoolClasses.h"

// ===================================================================
//                             KiddiePool
// ===================================================================

ClassImp(KiddiePool)

KiddiePool::~KiddiePool()
{
  while (!fEvents.empty()) {
    MiniEvent* fe= fEvents.front();
    delete fe;
    fe = 0;
    fEvents.pop_front();
  }
}

void 
KiddiePool::PrintInfo() const
{
  cout << Form("%20s: %d events", "Pool capacity", fMixDepth) << endl;
  cout << Form("%20s: %d events, %d tracks", "Current size", 
	       GetCurrentNEvents(), NTracksInPool()) << endl;
  cout << Form("%20s: %.1f to %.1f", "Sub-event mult.", fMultMin, fMultMax) << endl;
  cout << Form("%20s: %.1f to %.1f", "Z-vtx range", fZvtxMin, fZvtxMax) << endl;

  return;
}

Bool_t 
KiddiePool::EventMatchesBin(Int_t mult, Double_t zvtx) const
{
  return EventMatchesBin((Double_t) mult, zvtx);
}

Bool_t 
KiddiePool::EventMatchesBin(Double_t mult, Double_t zvtx) const
{
  // Lower bin limit included; upper limit excluded.

  Bool_t multOK = (mult >= fMultMin && mult < fMultMax);
  Bool_t zvtxOK = (zvtx >= fZvtxMin && zvtx < fZvtxMax);
  return (multOK && zvtxOK);
}

Int_t 
KiddiePool::NTracksInPool() const
{
  // Number of tracks for this cent, zvtx bin; possibly includes many events.

  Int_t ntrk=0;
  for (Int_t i=0; i<(Int_t)fEvents.size(); ++i) {
    ntrk += fNTracksInEvent.at(i);
  }
  return ntrk;
}

Int_t
KiddiePool::SetEventMultRange(Int_t multMin, Int_t multMax)
{
  fMultMin = (Double_t)multMin;
  fMultMax = (Double_t)multMax;
  return 0;
}

Int_t
KiddiePool::SetEventMultRange(Double_t multMin, Double_t multMax)
{
  fMultMin = multMin;
  fMultMax = multMax;
  return 0;
}

Int_t
KiddiePool::SetEventZvtxRange(Double_t zvtxMin, Double_t zvtxMax)
{
  fZvtxMin = zvtxMin;
  fZvtxMax = zvtxMax;
  return 0;
}

Int_t
KiddiePool::GlobalEventIndex(Int_t j) const
{
  // Index returned from passing local pool event index.

  if (j < 0 || j >= (Int_t)fEventIndex.size()) {
    cout << "ERROR in KiddiePool::GlobalEventIndex(): "
	 << " Invalid index " << j << endl;
    return -99;
  }
  return fEventIndex.at(j);
}

Int_t
KiddiePool::UpdatePool(MiniEvent* miniEvt)
{
  // A rolling buffer (a double-ended queue) is updated by removing
  // the oldest event, and appending the newest.

  static Int_t iEvent = -1; 
  iEvent++;

  Int_t mult = miniEvt->size(); // # tracks in this (mini) event
  Int_t nTrk = NTracksInPool();

  if (nTrk < fTargetTrackDepth && ((nTrk + mult) >= fTargetTrackDepth)) 
    fNTimes++;

  // remove 0th element before appending this event
  Bool_t removeFirstEvent = 0;
  if (nTrk>fTargetTrackDepth) {
    Int_t nTrksFirstEvent= fNTracksInEvent.front();
    Int_t diff = nTrk - nTrksFirstEvent + mult;
    if (diff>fTargetTrackDepth)
      removeFirstEvent = 1;
  }
  if (removeFirstEvent) {
    MiniEvent* oldestEvent = fEvents.front();
    delete oldestEvent;
    fEvents.pop_front();         // remove first track array 
    fNTracksInEvent.pop_front(); // remove first int
    fEventIndex.pop_front();
  }

  fNTracksInEvent.push_back(mult);
  fEvents.push_back(miniEvt);
  fEventIndex.push_back(iEvent);

  if (fNTimes==1) {
    fFirstFilled = kTRUE;
    if (KiddiePool::fDebug) {
      cout << "\nPool " << MultBinIndex() << ", " << ZvtxBinIndex() 
           << " ready at event "<< iEvent;
      PrintInfo();
      cout << endl;
    }
    fNTimes++; // See this message exactly once/pool
  } else {
    fFirstFilled = kFALSE;
  }

  fWasUpdated = true;

  if (KiddiePool::fDebug) {
    cout << " Event " << fEventIndex.back();
    cout << " PoolDepth = " << GetCurrentNEvents(); 
    cout << " NTracksInCurrentEvent = " << " " << mult << endl;
  }

  return fEvents.size();
}

MiniEvent* 
KiddiePool::GetEvent(Int_t i) const
{
  if (i<0 || i>=(Int_t)fEvents.size()) {
    cout << "KiddiePool::GetEvent(" 
	 << i << "): Invalid index" << endl;
    return 0x0;
  }

  MiniEvent* ev = fEvents.at(i);
  return ev;
}

Int_t
KiddiePool::NTracksInEvent(Int_t iEvent) const
{
  // Return number of tracks in iEvent, which is the local pool index.

  Int_t n = -1;
  Int_t curEvent = fEventIndex.back();
  Int_t offset = curEvent - iEvent;
  Int_t pos = fEventIndex.size() - offset - 1;

  if (offset==0)
    n = fNTracksInEvent.back();
  else if (offset < 0 || iEvent < 0) {
    n = 0;
  }
  else if (offset > 0 && offset <= (int)fEventIndex.size()) {
    n = fNTracksInEvent.at(pos);
  }
  else
    cout << "Event info no longer in memory" << endl;
  return n;
}

// ===================================================================
//                          KiddiePoolManager
// ===================================================================


ClassImp(KiddiePoolManager)

KiddiePoolManager::KiddiePoolManager(Int_t depth,     Int_t minNTracks,
				     Int_t nMultBins, Double_t *multbins,
				     Int_t nZvtxBins, Double_t *zvtxbins) 
: fDebug(0), fNMultBins(0), fNZvtxBins(0), fEvPool(0), fTargetTrackDepth(minNTracks) 
{
  // Constructor.

  InitEventPools(depth, nMultBins, multbins, nZvtxBins, zvtxbins);
  cout << "KiddiePoolManager initialized." << endl;
}

KiddiePoolManager::~KiddiePoolManager()
{
  for (Int_t iM=0; iM<fNMultBins; iM++) {
    for (Int_t iZ=0; iZ<fNZvtxBins; iZ++) {
      KiddiePool* pool = fEvPool.at(iM).at(iZ);
      delete pool;
    }
  }
}

Int_t KiddiePoolManager::InitEventPools(Int_t depth, 
                                       Int_t nMultBins, Double_t *multbin, 
                                       Int_t nZvtxBins, Double_t *zvtxbin)
{
  // Assign KiddiePoolManager members.

  fNMultBins = nMultBins;
  fNZvtxBins = nZvtxBins;

  for (Int_t iM=0; iM<fNMultBins; iM++) {
    std::vector<KiddiePool*> evp;
    for (Int_t iZ=0; iZ<fNZvtxBins; iZ++) {
      evp.push_back(new KiddiePool(depth, 
				   multbin[iM], multbin[iM+1], 
				   zvtxbin[iZ], zvtxbin[iZ+1] ));
    }
    fEvPool.push_back(evp);
  }
  
  for (Int_t iM=0; iM<nMultBins; iM++) {
    for (Int_t iZ=0; iZ<nZvtxBins; iZ++) {
      fEvPool.at(iM).at(iZ)->SetMultBinIndex(iM);
      fEvPool.at(iM).at(iZ)->SetZvtxBinIndex(iZ);
      fEvPool.at(iM).at(iZ)->SetTargetTrackDepth(fTargetTrackDepth);
      fEvPool.at(iM).at(iZ)->SetDebug(fDebug);
    }
  }
  
  if (fDebug) {
    cout << "fEvPool outer size: " << fEvPool.size() << endl;
    for (Int_t iM=0; iM<nMultBins; iM++) {
      for (Int_t iZ=0; iZ<nZvtxBins; iZ++) {
	if(fEvPool.at(iM).at(iZ)) {
	  cout << "multiplicity bin: " << iM;
	  cout << ", z-vertex bin: " << iZ;
	  fEvPool.at(iM).at(iZ)->PrintInfo();
	}
      }
    }
  }
  
  return fEvPool.size();
}

KiddiePool*
KiddiePoolManager::GetEventPool(Int_t iMult, Int_t iZvtx) const
{
  if (iMult < 0 || iMult >= fNMultBins) 
    return 0x0;
  if (iZvtx < 0 || iZvtx >= fNZvtxBins) 
    return 0x0;
  return fEvPool.at(iMult).at(iZvtx);
}

KiddiePool*
KiddiePoolManager::GetEventPool(Int_t centVal, Double_t zVtxVal) const
{
  return GetEventPool((Double_t)centVal, zVtxVal);
}

KiddiePool*
KiddiePoolManager::GetEventPool(Double_t centVal, Double_t zVtxVal) const
{
  // Return appropriate pool for this centrality and z-vertex value.

  for (Int_t iM=0; iM<fNMultBins; iM++) {
    for (Int_t iZ=0; iZ<fNZvtxBins; iZ++) {
      KiddiePool* pool = GetEventPool(iM, iZ);
      if (pool->EventMatchesBin(centVal, zVtxVal))
	return pool;
    }
  }
  return 0x0;
}

Int_t
KiddiePoolManager::UpdatePools(MiniEvent* miniEvt)
{
  // Call UpdatePool for all bins.

  for (Int_t iM=0; iM<fNMultBins; iM++) {
    for (Int_t iZ=0; iZ<fNZvtxBins; iZ++) {
      if (fEvPool.at(iM).at(iZ)->UpdatePool(miniEvt) > -1)
        break;
    }
  }
  return 0;
}

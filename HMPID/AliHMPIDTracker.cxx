#include "AliHMPIDTracker.h"     //class header
#include "AliHMPIDtrack.h"     //class header
#include "AliHMPIDCluster.h"     //GetTrackPoint(),PropagateBack() 
#include "AliHMPIDParam.h"       //GetTrackPoint(),PropagateBack()
#include "AliHMPIDRecon.h"       //Recon()
#include "AliHMPIDReconHTA.h"    //ReconHTA()
#include <AliESDEvent.h>         //PropagateBack(),Recon()  
#include <AliESDtrack.h>         //Intersect()  
#include <AliRun.h>              //GetTrackPoint(),PropagateBack()  
#include <AliTrackPointArray.h>  //GetTrackPoint()
#include <AliAlignObj.h>         //GetTrackPoint()
#include <AliCDBManager.h>       //PropageteBack()
#include <AliCDBEntry.h>         //PropageteBack()
//.
// HMPID base class fo tracking
//.
//.
//.
ClassImp(AliHMPIDTracker)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
AliHMPIDTracker::AliHMPIDTracker():
  AliTracker(),
  fClu(new TObjArray(AliHMPIDParam::kMaxCh+1))  
{
// ctor. Create TObjArray of TClonesArray of AliHMPIDCluster  
// 
//  
  fClu->SetOwner(kTRUE);
  for(int i=AliHMPIDParam::kMinCh;i<=AliHMPIDParam::kMaxCh;i++) fClu->AddAt(new TClonesArray("AliHMPIDCluster"),i);
}//ctor
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++    
Bool_t AliHMPIDTracker::GetTrackPoint(Int_t idx, AliTrackPoint& point) const
{
// Interface callback methode invoked from AliReconstruction::WriteAlignmentData() to get position of MIP cluster in MARS associated to a current track.
// MIP cluster is reffered by index which is stored in AliESDtrack  ???????
// Arguments: idx- cluster index which is stored by HMPID in AliESDtrack
//            point- reference to the object where to store the point     
//   Returns: status of operation  if FALSE then AliReconstruction::WriteAlignmentData() do not store this point to array of points for current track. 
  if(idx<0) return kFALSE; //no MIP cluster assigned to this track in PropagateBack()
  Int_t iCham=idx/1000000; Int_t iClu=idx%1000000;
  point.SetVolumeID(AliGeomManager::LayerToVolUID(AliGeomManager::kHMPID,iCham-1));//layer and chamber number
  TClonesArray *pArr=(TClonesArray*)(*fClu)[iCham];
  AliHMPIDCluster *pClu=(AliHMPIDCluster*)pArr->UncheckedAt(iClu);//get pointer to cluster
  Double_t mars[3];
  AliHMPIDParam::Instance()->Lors2Mars(iCham,pClu->X(),pClu->Y(),mars);
  point.SetXYZ(mars[0],mars[1],mars[2]);
  return kTRUE;
}//GetTrackPoint()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Int_t AliHMPIDTracker::IntTrkCha(AliESDtrack *pTrk,Float_t &xPc,Float_t &yPc,Float_t &xRa,Float_t &yRa,Float_t &theta,Float_t &phi)
{
// Static method to find intersection in between given track and HMPID chambers
// Arguments: pTrk- ESD track; xPc,yPc- track intersection with PC in LORS [cm]
//   Returns: intersected chamber ID or -1
  AliHMPIDParam *pParam=AliHMPIDParam::Instance();
  for(Int_t i=AliHMPIDParam::kMinCh;i<=AliHMPIDParam::kMaxCh;i++){                              //chambers loop
    Double_t p1[3],n1[3]; pParam->Norm(i,n1); pParam->Point(i,p1,AliHMPIDParam::kRad);          //point & norm  for middle of radiator plane
    Double_t p2[3],n2[3]; pParam->Norm(i,n2); pParam->Point(i,p2,AliHMPIDParam::kPc);           //point & norm  for entrance to PC plane
    if(pTrk->Intersect(p1,n1,-GetBz())==kFALSE) continue;                                       //try to intersect track with the middle of radiator
    if(pTrk->Intersect(p2,n2,-GetBz())==kFALSE) continue;                                       //try to intersect track with PC
    pParam->Mars2LorsVec(i,n1,theta,phi);                                                       //track angles at RAD
    pParam->Mars2Lors   (i,p1,xRa,yRa);                                                         //TRKxRAD position
    pParam->Mars2Lors   (i,p2,xPc,yPc);                                                         //TRKxPC position
    if(AliHMPIDParam::IsInside(xPc,yPc,pParam->DistCut())==kTRUE) return i;                     //return intersected chamber  
  }                                                                                             //chambers loop
  return -1;                                                                                    //no intersection with HMPID chambers
}//IntTrkCha()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Int_t AliHMPIDTracker::IntTrkCha(Int_t ch,AliHMPIDtrack *pTrk,Float_t &xPc,Float_t &yPc,Float_t &xRa,Float_t &yRa,Float_t &theta,Float_t &phi)
{
// Static method to find intersection in between given track and HMPID chambers
// Arguments: pTrk- HMPID track; xPc,yPc- track intersection with PC in LORS [cm]
//   Returns: intersected chamber ID or -1
    AliHMPIDParam *pParam=AliHMPIDParam::Instance();
    Double_t p1[3],n1[3]; 
    pParam->Norm(ch,n1); 
    pParam->Point(ch,p1,AliHMPIDParam::kRad);                                                    //point & norm  for middle of radiator plane
    Double_t p2[3],n2[3]; 
    pParam->Norm(ch,n2); 
    pParam->Point(ch,p2,AliHMPIDParam::kPc);                                                     //point & norm  for entrance to PC plane
    if(pTrk->Intersect(pTrk,p1,n1)==kFALSE) return -1;                                           //try to intersect track with the middle of radiator
    if(pTrk->Intersect(pTrk,p2,n2)==kFALSE) return -1;   
    pParam->Mars2LorsVec(ch,n1,theta,phi);                                                       //track angles at RAD
    pParam->Mars2Lors   (ch,p1,xRa,yRa);                                                         //TRKxRAD position
    pParam->Mars2Lors   (ch,p2,xPc,yPc);                                                         //TRKxPC position
    if(AliHMPIDParam::IsInside(xPc,yPc,pParam->DistCut())==kTRUE) return ch;                     //return intersected chamber  
  return -1;                                                                                     //no intersection with HMPID chambers
}//IntTrkCha()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Int_t AliHMPIDTracker::LoadClusters(TTree *pCluTree)
{
// Interface callback methode invoked from AliReconstruction::RunTracking() to load HMPID clusters before PropagateBack() gets control. Done once per event.
// Arguments: pCluTree- pointer to clusters tree got by AliHMPIDLoader::LoadRecPoints("read") then AliHMPIDLoader::TreeR()
//   Returns: error code (currently ignored in AliReconstruction::RunTraking())    
  for(int i=AliHMPIDParam::kMinCh;i<=AliHMPIDParam::kMaxCh;i++) pCluTree->SetBranchAddress(Form("HMPID%d",i),&((*fClu)[i]));
  pCluTree->GetEntry(0);
  return 0;  
}//LoadClusters()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Int_t AliHMPIDTracker::PropagateBack(AliESDEvent *pEsd)
{
// Interface pure virtual in AliTracker. Invoked from AliReconstruction::RunTracking() after invocation of AliTracker::LoadClusters() once per event
// Agruments: pEsd - pointer to ESD
//   Returns: error code    
  AliCDBEntry *pNmeanEnt =AliCDBManager::Instance()->Get("HMPID/Calib/Nmean"); //contains TObjArray of 42 TF1 + 1 EPhotMean
  AliCDBEntry *pQthreEnt =AliCDBManager::Instance()->Get("HMPID/Calib/Qthre"); //contains TObjArray of 42 (7ch * 6sec) TF1
  if(!pNmeanEnt) AliFatal("No Nmean C6F14 ");
  if(!pQthreEnt) AliFatal("No Qthre");
    
  return Recon(pEsd,fClu,(TObjArray*)pNmeanEnt->GetObject(),(TObjArray*)pQthreEnt->GetObject());  
}//PropagateBack()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Int_t AliHMPIDTracker::Recon(AliESDEvent *pEsd,TObjArray *pClus,TObjArray *pNmean, TObjArray *pQthre)
{
// Static method to reconstruct Theta Ckov for all valid tracks of a given event.
// Arguments: pEsd- pointer ESD; pClu- pointer to clusters for all chambers; pNmean - pointer to all function Nmean=f(time)
//   Returns: error code, 0 if no errors   
  
  AliHMPIDRecon recon;                                                                           //instance of reconstruction class, nothing important in ctor
  Float_t xPc,yPc,xRa,yRa,theta,phi;
  Double_t cluLORS[2]={0},cluMARS[3]={0},trkMARS[3]={0};
//  Double_t bestcluMARS[3]={0,0,0};
  Double_t radClu,radInitTrk;   
  Int_t nMipClusTot=0;
  Double_t d3d=0,dmin=999999,bz=0;
  Bool_t isMatched=kFALSE;
  Int_t cluSiz=0;
  Double_t qthre = 0;   Double_t nmean=0; Int_t cham=0; Int_t hvsec=0;
  Int_t index=0;                                                                                //index of the "best" matching cluster
  Double_t bestChi2=-1;                                                                         //Chi2 of the "best" matching cluster
  Double_t chi2=0;   
  Int_t nClusCh[AliHMPIDParam::kMaxCh+1];
  Bool_t isOkQcut=kFALSE;
  Bool_t isOkDcut=kFALSE;
  
  AliHMPIDParam *pParam = AliHMPIDParam::Instance();                                             //Instance of AliHMPIDParam
  
  for(Int_t iTrk=0;iTrk<pEsd->GetNumberOfTracks();iTrk++){                                        //loop on the ESD tracks in the event
    isMatched=kFALSE;dmin=999999;bestChi2=99999;chi2=99999;cluSiz=0;                              //init. track matching params
    isOkQcut = kFALSE;
    AliHMPIDCluster *bestHmpCluster=0x0;                                                          //the best matching cluster
    AliESDtrack *pTrk = pEsd->GetTrack(iTrk);                                                     //get reconstructed track    
    AliHMPIDtrack *hmpTrk = new AliHMPIDtrack(*pTrk);                                             //create a hmpid track to be used for propagation and matching 
    bz=AliTracker::GetBz();  
    
    Int_t ipCh=IntTrkCha(pTrk,xPc,yPc,xRa,yRa,theta,phi);                                        //find the intersected chamber for this track 
    if(ipCh<0) {                                                                                 //no intersection at all, go after next track
      pTrk->SetHMPIDtrk(0,0,0,0);                                                                //no intersection found
      pTrk->SetHMPIDcluIdx   (99,99999);                                                         //chamber not found, mip not yet considered
      pTrk->SetHMPIDsignal(AliHMPIDRecon::kNotPerformed);                                        //ring reconstruction not yet performed
      continue;                                                                         
    }
    
// track intersects the chamber ipCh: find the MIP          
    
    TClonesArray *pMipCluLst=(TClonesArray *)pClus->At(ipCh);                                   //get the list of clusters
    nMipClusTot = pMipCluLst->GetEntries();                                                     //total number of clusters in the given chamber
    nClusCh[ipCh] = nMipClusTot;
    
    for (Int_t iClu=0; iClu<nMipClusTot;iClu++) {                                               //clusters loop
      
      AliHMPIDCluster *pClu=(AliHMPIDCluster*)pMipCluLst->UncheckedAt(iClu);                    //get the cluster
// evaluate qThre
      if(pQthre->GetEntriesFast()==pParam->kMaxCh+1) {                                             // just for backward compatibility
        qthre=((TF1*)pQthre->At(pClu->Ch()))->Eval(pEsd->GetTimeStamp());                          //
      } else {                                                                                     // in the past just 1 qthre
        hvsec = pParam->InHVSector(pClu->Y());                                              //  per chamber
        if(hvsec>=0)
	  qthre=((TF1*)pQthre->At(6*cham+hvsec))->Eval(pEsd->GetTimeStamp());                      //
      }                                                                                            //
//
      if(pClu->Q()<qthre) continue;                                                                      //charge compartible with MIP clusters      
      isOkQcut = kTRUE;

      cluLORS[0]=pClu->X(); cluLORS[1]=pClu->Y();                                            //get the LORS coordinates of the cluster
      pParam->Lors2Mars(ipCh,cluLORS[0],cluLORS[1],cluMARS);              //convert cluster coors. from LORS to MARS
      radClu=TMath::Sqrt(cluMARS[0]*cluMARS[0]+cluMARS[1]*cluMARS[1]);                       //radial distance of candidate cluster in MARS                                          
      Double_t trkx0[3]; 
      hmpTrk->GetXYZ(trkx0);                                                                 //get track position in MARS
      radInitTrk=TMath::Sqrt(trkx0[0]*trkx0[0]+trkx0[1]*trkx0[1]);
      hmpTrk->PropagateToR(radClu,10);
      hmpTrk->GetXYZ(trkx0);                                                                   //get track position in MARS
      hmpTrk->GetXYZAt(radClu,bz,trkMARS);                                                     //get the track coordinates at the rad distance after prop. 
      d3d=TMath::Sqrt((cluMARS[0]-trkMARS[0])*(cluMARS[0]-trkMARS[0])+(cluMARS[1]-trkMARS[1])*(cluMARS[1]-trkMARS[1])+(cluMARS[2]-trkMARS[2])*(cluMARS[2]-trkMARS[2]));
      chi2=hmpTrk->GetPredictedChi2(pClu);
      if(dmin > d3d ) {                                                                         //to be saved for the moment...
        cluSiz = pClu->Size();
        dmin=d3d;
        bestHmpCluster=pClu;
        index=iClu;
        bestChi2=chi2;
        cluLORS[0]=pClu->X(); cluLORS[1]=pClu->Y();
//        pParam->Lors2Mars(ipCh,cluLORS[0],cluLORS[1],bestcluMARS); 
      }//global dmin cut 
    }//clus loop

    pTrk->SetHMPIDmip(0,0,0,0);                                                                //store mip info in any case 
   
    if(!isOkQcut) {
      pTrk->SetHMPIDcluIdx(ipCh,9999);                                                          
      pTrk->SetHMPIDsignal(pParam->kMipQdcCut);
      continue;                                                                     
    }
    
    if(dmin < pParam->DistCut()) {
      isOkDcut = kTRUE;
    }

    if(!isOkDcut) {
      pTrk->SetHMPIDmip(bestHmpCluster->X(),bestHmpCluster->Y(),(Int_t)bestHmpCluster->Q(),0);  //store mip info in any case 
      pTrk->SetHMPIDcluIdx(ipCh,index+1000*cluSiz);                                             //set chamber, index of cluster + cluster size
      pTrk->SetHMPIDsignal(pParam->kMipDistCut);                                                //closest cluster with enough charge is still too far from intersection
    }
    
    if(isOkQcut*isOkDcut) isMatched = kTRUE;                                                    // MIP-Track matched !!    
    
    if(!isMatched) continue;                                                                    // If matched continue...
    
    Int_t indexAll = 0;
    for(Int_t iC=0;iC<ipCh;iC++) indexAll+=nClusCh[iC]; indexAll+=index;                        //to be verified...

    Bool_t isOk = hmpTrk->Update(bestHmpCluster,bestChi2,indexAll);
    if(!isOk) continue;
    pTrk->SetOuterParam(hmpTrk,AliESDtrack::kHMPIDout);                 

//    cham=IntTrkCha(ipCh,hmpTrk,xPc,yPc,xRa,yRa,theta,phi);
    cham=IntTrkCha(pTrk,xPc,yPc,xRa,yRa,theta,phi);
    if(cham<0) {                                                                                  //no intersection at all, go after next track
      pTrk->SetHMPIDtrk(0,0,0,0);                                                                //no intersection found
      pTrk->SetHMPIDcluIdx   (99,99999);                                                         //chamber not found, mip not yet considered
      pTrk->SetHMPIDsignal(AliHMPIDRecon::kNotPerformed);                                        //ring reconstruction not yet performed
      continue;                                                                         
    }

    pTrk->SetHMPIDtrk(xRa,yRa,theta,phi);                                                        //store initial infos
    //evaluate nMean
    if(pNmean->GetEntries()==21) {                                                              //for backward compatibility
      nmean=((TF1*)pNmean->At(3*cham))->Eval(pEsd->GetTimeStamp());                             //C6F14 Nmean for this chamber
    } else {
      Int_t iRad     = pParam->Radiator(yRa);                                                   //evaluate the radiator involved
      Double_t tLow  = ((TF1*)pNmean->At(6*cham+2*iRad  ))->Eval(pEsd->GetTimeStamp());         //C6F14 low  temp for this chamber
      Double_t tHigh = ((TF1*)pNmean->At(6*cham+2*iRad+1))->Eval(pEsd->GetTimeStamp());         //C6F14 high temp for this chamber
      Double_t tExp  = pParam->FindTemp(tLow,tHigh,yRa);                                        //estimated temp for that chamber at that y
      nmean = pParam->NIdxRad(AliHMPIDParam::Instance()->GetEPhotMean(),tExp);                  //mean ref idx @ a given temp
      if(nmean < 0){                                                                            //track didn' t pass through the radiator
         pTrk->SetHMPIDsignal(AliHMPIDRecon::kNoRad);                                           //set the appropriate flag
         pTrk->SetHMPIDcluIdx(ipCh,index+1000*cluSiz);                                          //set index of cluster
         continue;
      }
    }
    //
    recon.SetImpPC(xPc,yPc);                                                                     //store track impact to PC
    recon.CkovAngle(pTrk,(TClonesArray *)pClus->At(cham),index,nmean);                           //search for Cerenkov angle of this track
  }//iTrk

  return 0; // error code: 0=no error;
}//Recon()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Int_t AliHMPIDTracker::ReconHiddenTrk(Int_t iCh,Int_t iHVsec,AliESDtrack *pTrk,TClonesArray *pCluLst,TObjArray *pNmean,TObjArray *pQthre)
{
// Static method to reconstruct Theta Ckov for all valid tracks of a given event.
// Arguments: pEsd- pointer ESD; pClu- pointer to clusters for all chambers; pNmean - pointer to all function Nmean=f(time), pQthre - pointer to all function Qthre=f(time)
//   Returns: error code, 0 if no errors
  AliHMPIDReconHTA reconHTA;                                                                          //instance of reconstruction class, nothing important in ctor
  Double_t nmean=((TF1*)pNmean->At(3*iCh))->Eval(0);                                            //C6F14 Nmean for this chamber
  Double_t qthre = 0;
  if(pQthre->GetEntriesFast()==AliHMPIDParam::kMaxCh+1)                                         //
    qthre=((TF1*)pQthre->At(iCh))->Eval(0);                                                     //just for backward compatibi
  else  qthre=((TF1*)pQthre->At(6*iCh+iHVsec))->Eval(0);                                        //
  if(pCluLst->GetEntriesFast()<4) return 1;                                                     //min 4 clusters (3 + 1 mip) to find a ring! 
  if(reconHTA.CkovHiddenTrk(pTrk,pCluLst,nmean,qthre)) return 0;                                   //search for track parameters and Cerenkov angle of this track
  else return 1;                                                                                // error code: 0=no error,1=fit not performed;
}//Recon()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void AliHMPIDTracker::FillClusterArray(TObjArray* array) const {
  
 // Publishes all pointers to clusters known to the tracker into the
  // passed object array.
  // The ownership is not transfered - the caller is not expected to delete
  // the clusters
 
  for(Int_t iCh=AliHMPIDParam::kMinCh;iCh<=AliHMPIDParam::kMaxCh;iCh++){    
    TClonesArray *pCluArr=(TClonesArray*)(*fClu)[iCh];
    for (Int_t iClu=0; iClu<pCluArr->GetEntriesFast();iClu++){
      AliHMPIDCluster *pClu=(AliHMPIDCluster*)pCluArr->UncheckedAt(iClu);    
      array->AddLast(pClu);
    }//cluster loop in iCh
    pCluArr->Delete();
  }//Ch loop
    
  return;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#include "AliReaderESD.h"
//____________________________________________________________________
//////////////////////////////////////////////////////////////////////
//                                                                  //
// class AliReaderESD                                               //
//                                                                  //
// reader for ALICE Event Summary Data (ESD).                       //
//                                                                  //
// Piotr.Skowronski@cern.ch                                         //
//                                                                  //
//////////////////////////////////////////////////////////////////////

#include <TPDGCode.h>
#include <TString.h>
#include <TObjString.h>
#include <TTree.h>
#include <TFile.h>
#include <TKey.h>
#include <TParticle.h>
#include <TH1.h>

#include <AliRun.h>
#include <AliRunLoader.h>
#include <AliStack.h>
#include <AliESDtrack.h>
#include <AliESD.h>

#include "AliAnalysis.h"
#include "AliAODRun.h"
#include "AliAOD.h"
#include "AliAODParticle.h"
#include "AliAODParticleCut.h"
#include "AliTrackPoints.h"
#include "AliClusterMap.h"

ClassImp(AliReaderESD)

AliReaderESD::AliReaderESD(const Char_t* esdfilename, const Char_t* galfilename):
 fESDFileName(esdfilename),
 fGAlFileName(galfilename),
 fFile(0x0),
 fRunLoader(0x0),
 fKeyIterator(0x0),
 fReadSim(kFALSE),
 fCheckParticlePID(kFALSE),
 fReadMostProbableOnly(kFALSE),
 fNTrackPoints(0),
 fdR(0.0),
 fClusterMap(kFALSE),
 fNTPCClustMin(0),
 fNTPCClustMax(150),
 fTPCChi2PerClustMin(0.0),
 fTPCChi2PerClustMax(10e5),
 fChi2Min(0.0),
 fChi2Max(10e5),
 fC00Min(0.0),
 fC00Max(10e5),
 fC11Min(0.0),
 fC11Max(10e5),
 fC22Min(0.0),
 fC22Max(10e5),
 fC33Min(0.0),
 fC33Max(10e5),
 fC44Min(0.0),
 fC44Max(10e5),
 fTPCC00Min(0.0),
 fTPCC00Max(10e5),
 fTPCC11Min(0.0),
 fTPCC11Max(10e5),
 fTPCC22Min(0.0),
 fTPCC22Max(10e5),
 fTPCC33Min(0.0),
 fTPCC33Max(10e5),
 fTPCC44Min(0.0),
 fTPCC44Max(10e5)

{
  //cosntructor
  if ( ((Int_t)kNSpecies) != ((Int_t)AliESDtrack::kSPECIES))
    Fatal("AliReaderESD","ESD defintions probobly changed. Ask Youra.");
}
/********************************************************************/
  
AliReaderESD::AliReaderESD(TObjArray* dirs,const Char_t* esdfilename, const Char_t* galfilename):
 AliReader(dirs), 
 fESDFileName(esdfilename),
 fGAlFileName(galfilename),
 fFile(0x0),
 fRunLoader(0x0),
 fKeyIterator(0x0),
 fReadSim(kFALSE),
 fCheckParticlePID(kFALSE),
 fReadMostProbableOnly(kFALSE),
 fNTrackPoints(0),
 fdR(0.0),
 fClusterMap(kFALSE),
 fNTPCClustMin(0),
 fNTPCClustMax(150),
 fTPCChi2PerClustMin(0.0),
 fTPCChi2PerClustMax(10e5),
 fChi2Min(0.0),
 fChi2Max(10e5),
 fC00Min(0.0),
 fC00Max(10e5),
 fC11Min(0.0),
 fC11Max(10e5),
 fC22Min(0.0),
 fC22Max(10e5),
 fC33Min(0.0),
 fC33Max(10e5),
 fC44Min(0.0),
 fC44Max(10e5),
 fTPCC00Min(0.0),
 fTPCC00Max(10e5),
 fTPCC11Min(0.0),
 fTPCC11Max(10e5),
 fTPCC22Min(0.0),
 fTPCC22Max(10e5),
 fTPCC33Min(0.0),
 fTPCC33Max(10e5),
 fTPCC44Min(0.0),
 fTPCC44Max(10e5)
{
  //cosntructor
  if ( ((Int_t)kNSpecies) != ((Int_t)AliESDtrack::kSPECIES))
    Fatal("AliReaderESD","ESD defintions probobly changed. Ask Youra.");
}
/********************************************************************/

AliReaderESD::~AliReaderESD()
{
 //desctructor
  delete fRunLoader;
  delete fKeyIterator;
  delete fFile;
}
/**********************************************************/
Int_t AliReaderESD::ReadNext()
{
//reads next event from fFile
  //fRunLoader is for reading Kine
  
  if (AliVAODParticle::GetDebug())
    Info("ReadNext","Entered");
    
  if (fEventSim == 0x0)  fEventSim = new AliAOD();
  if (fEventRec == 0x0)  fEventRec = new AliAOD();
  
  fEventSim->Reset();
  fEventRec->Reset();
        
  do  //do{}while; is OK even if 0 dirs specified. In that case we try to read from "./"
   {
     if (fFile == 0x0)
      {
       fFile = OpenFile(fCurrentDir);//rl is opened here
       if (fFile == 0x0)
         {
           Error("ReadNext","Cannot get fFile for dir no. %d",fCurrentDir);
           fCurrentDir++;
           continue;
         }
       fCurrentEvent = 0;
       fKeyIterator = new TIter(fFile->GetListOfKeys());  
//       fFile->Dump();
//       fFile->GetListOfKeys()->Print();
      } 
     TKey* key = (TKey*)fKeyIterator->Next();
     if (key == 0x0)
      {
        if (AliVAODParticle::GetDebug() > 2 )
          {
            Info("ReadNext","No more keys.");
          }
        fCurrentDir++;
        delete fKeyIterator;
        fKeyIterator = 0x0;
        delete fFile;//we have to assume there is no more ESD objects in the fFile
        fFile = 0x0;
        delete fRunLoader;
        fRunLoader = 0x0;
        continue;
      }
     //try to read
     
     
//     TObject* esdobj = key->ReadObj();
//     if (esdobj == 0x0)
//      {
//        if (AliVAODParticle::GetDebug() > 2 )
//          {
//            Info("ReadNext","Key read NULL. Key Name is %s",key->GetName());
//            key->Dump();
//          }
//        continue;
//      }
//     esdobj->Dump();
//     AliESD* esd = dynamic_cast<AliESD*>(esdobj);
     
     TString esdname = "ESD";
     esdname+=fCurrentEvent;
     AliESD* esd = dynamic_cast<AliESD*>(fFile->Get(esdname));
     if (esd == 0x0)
      {
//        if (AliVAODParticle::GetDebug() > 2 )
//          {
//            Info("ReadNext","This key is not an AliESD object %s",key->GetName());
//          }
        if (AliVAODParticle::GetDebug() > 2 )
          {
            Info("ReadNext","Can not find AliESD object named %s",esdname.Data());
          }
        fCurrentDir++;
        delete fKeyIterator;
        fKeyIterator = 0x0;
        delete fFile;//we have to assume there is no more ESD objects in the fFile
        fFile = 0x0;
        delete fRunLoader;
        fRunLoader = 0x0;
        continue;
      }
     
     ReadESD(esd);
      
     fCurrentEvent++;
     fNEventsRead++;
     delete esd;
     return 0;//success -> read one event
   }while(fCurrentDir < GetNumberOfDirs());//end of loop over directories specified in fDirs Obj Array  
   
  return 1; //no more directories to read
}
/**********************************************************/

Int_t AliReaderESD::ReadESD(AliESD* esd)
{
  //****** Tentative particle type "concentrations"
  static const Double_t concentr[5]={0.05, 0., 0.85, 0.10, 0.05};
  
  Double_t pidtable[kNSpecies];//array used for reading pid probabilities from ESD track
  Double_t w[kNSpecies];
  Double_t mom[3];//momentum
  Double_t pos[3];//position
  Double_t vertexpos[3];//vertex position
  //Reads one ESD
  if (esd == 0x0)
   {
     Error("ReadESD","ESD is NULL");
     return 1;
   }

  TDatabasePDG* pdgdb = TDatabasePDG::Instance();
  if (pdgdb == 0x0)
   {
     Error("ReadESD","Can not get PDG Database Instance.");
     return 1;
   }
   
  Float_t mf = esd->GetMagneticField(); 

  if ( (mf == 0.0) && (fNTrackPoints > 0) )
   {
      Error("ReadESD","Magnetic Field is 0 and Track Points Demended. Skipping to next event.");
      return 1;
   }

  AliStack* stack = 0x0;
  if (fReadSim && fRunLoader)
   {
     fRunLoader->GetEvent(fCurrentEvent);
     stack = fRunLoader->Stack();
   }

  const AliESDVertex* vertex = esd->GetVertex();
  if (vertex == 0x0)
   {
     Info("ReadESD","ESD returned NULL pointer to vertex - assuming (0.0,0.0,0.0)");
     vertexpos[0] = 0.0;
     vertexpos[1] = 0.0;
     vertexpos[2] = 0.0;
   }
  else
   {
     vertex->GetXYZ(vertexpos);
   }
   
  if (AliVAODParticle::GetDebug() > 0)
   {
     Info("ReadESD","Primary Vertex is (%f,%f,%f)",vertexpos[0],vertexpos[1],vertexpos[2]);
   }
   
  Info("ReadESD","Reading Event %d",fCurrentEvent);

  Int_t ntr = esd->GetNumberOfTracks();
  Info("ReadESD","Found %d tracks.",ntr);
  for (Int_t i = 0;i<ntr; i++)
   {
     AliESDtrack *esdtrack = esd->GetTrack(i);
     if (esdtrack == 0x0)
      {
        Error("Next","Can not get track %d", i);
        continue;
      }

     //if (esdtrack->HasVertexParameters() == kFALSE) 
     if ((esdtrack->GetStatus() & AliESDtrack::kITSrefit) == kFALSE)
      {
        if (AliVAODParticle::GetDebug() > 2) 
          Info("ReadNext","Particle skipped: Data at vertex not available.");
        continue;
      }

     if ((esdtrack->GetStatus() & AliESDtrack::kESDpid) == kFALSE) 
      {
        if (AliVAODParticle::GetDebug() > 2) 
          Info("ReadNext","Particle skipped: PID BIT is not set.");
        continue;
      }


     Double_t extx;
     Double_t extp[5];
     esdtrack->GetConstrainedExternalParameters(extx,extp);
     if (extp[4] == 0.0)
      {
        if (AliVAODParticle::GetDebug() > 2) 
          Info("ReadNext","Track has 0 contrianed curvature -> Probobly parameters never updated. Skipping.");
        continue;
      } 
     esdtrack->GetESDpid(pidtable);
     esdtrack->GetConstrainedPxPyPz(mom);
     esdtrack->GetConstrainedXYZ(pos);
     pos[0] -= vertexpos[0];//we are interested only in relative position to Primary vertex at this point
     pos[1] -= vertexpos[1];
     pos[2] -= vertexpos[2];

     Int_t charge = (extp[4] > 0)?1:-1;//if curvature=charg/Pt is positive charge is positive

     //Particle from kinematics
     AliAODParticle* particle = 0;
     Bool_t keeppart = kFALSE;
     if ( fReadSim && stack  )
      {
        if (esdtrack->GetLabel() < 0) continue;//this is fake -  we are not able to match any track 
        TParticle *p = stack->Particle(esdtrack->GetLabel());
        if (p==0x0) 
         {
           Error("ReadNext","Can not find track with such label.");
           continue;
         }
        if(Pass(p->GetPdgCode())) 
         {
           if ( AliVAODParticle::GetDebug() > 5 )
             Info("ReadNext","Simulated Particle PID (%d) did not pass the cut.",p->GetPdgCode());
           continue; //check if we are intersted with particles of this type 
         }
//           if(p->GetPdgCode()<0) charge = -1;
        particle = new AliAODParticle(*p,i);

      }
      
     if(CheckTrack(esdtrack)) continue;
      
     //Here we apply Bayes' formula
     Double_t rc=0.;
     for (Int_t s=0; s<AliESDtrack::kSPECIES; s++) rc+=concentr[s]*pidtable[s];
     if (rc==0.0) 
      {
        if (AliVAODParticle::GetDebug() > 2) 
          Info("ReadNext","Particle rejected since total bayessian PID probab. is zero.");
        continue;
      }

     for (Int_t s=0; s<AliESDtrack::kSPECIES; s++) w[s]=concentr[s]*pidtable[s]/rc;

     if (AliVAODParticle::GetDebug() > 4)
      { 
        Info("ReadNext","###########################################################################");
        Info("ReadNext","Momentum: %f %f %f",mom[0],mom[1],mom[2]);
        Info("ReadNext","Position: %f %f %f",pos[0],pos[1],pos[2]);
        TString msg("Pid list got from track:");
        for (Int_t s = 0;s<kNSpecies;s++)
         {
           msg+="\n    ";
           msg+=s;
           msg+="(";
           msg+=charge*GetSpeciesPdgCode((ESpecies)s);
           msg+="): ";
           msg+=w[s];
           msg+=" (";
           msg+=pidtable[s];
           msg+=")";
         }
        Info("ReadNext","%s",msg.Data());
      }//if (AliVAODParticle::GetDebug()>4)

      AliTrackPoints* tpts = 0x0;
      if (fNTrackPoints > 0) 
       {
         tpts = new AliTrackPoints(fNTrackPoints,esdtrack,mf,fdR);
       }

      AliClusterMap* cmap = 0x0; 
      if (  fClusterMap ) 
       {
         cmap = new AliClusterMap(esdtrack);
       }
     
     //If this flag fReadMostProbableOnly is false the 
     //loop over species (see "LOOPPIDS") is over all possible PIDs
     //in other case the most probablle PID is searched
     //and the loop is limited to that PID only

     Int_t firstspecie = 0;
     Int_t lastspecie = kNSpecies;
     
     if (fReadMostProbableOnly)
      { 
        //find the most probable PID
        Int_t spec = 0;
        Float_t maxprob = w[0];
        for (Int_t s=1; s<AliESDtrack::kSPECIES; s++) 
         {
           if (w[s]>maxprob)
            {
              maxprob = w[s];
              spec = s;
            }
         }
        firstspecie = spec;
        lastspecie = spec + 1;
      }
     
     for (Int_t s = firstspecie; s<lastspecie; s++)//LOOPPIDS
      {
        Int_t pdgcode = charge*GetSpeciesPdgCode((ESpecies)s);
        Float_t pp = w[s];
        if (pp == 0.0) 
         {
           if ( AliVAODParticle::GetDebug() > 5 )
             Info("ReadNext","Probability of being PID %d is zero. Continuing.",pdgcode);
           continue;
         }

        if(Pass(pdgcode)) 
         {
           if ( AliVAODParticle::GetDebug() > 5 )
             Info("ReadNext","PID (%d) did not pass the cut.",pdgcode);
           continue; //check if we are intersted with particles of this type 
         }

        Double_t mass = pdgdb->GetParticle(pdgcode)->Mass();
        Double_t tEtot = TMath::Sqrt( mom[0]*mom[0] + mom[1]*mom[1] + mom[2]*mom[2] + mass*mass);//total energy of the track

        AliAODParticle* track = new AliAODParticle(pdgcode, w[s],i, 
                                                   mom[0], mom[1], mom[2], tEtot,
                                                   pos[0], pos[1], pos[2], 0.);
        //copy probabilitis of other species (if not zero)
        for (Int_t k = 0; k<kNSpecies; k++)
         {
           if (k == s) continue;
           if (w[k] == 0.0) continue;
           track->SetPIDprobability(charge*GetSpeciesPdgCode( (ESpecies)k ),w[k]);
         }

        if(Pass(track))//check if meets all criteria of any of our cuts
                       //if it does not delete it and take next good track
         { 
           if ( AliVAODParticle::GetDebug() > 4 )
             Info("ReadNext","Track did not pass the cut");
           delete track;
           continue;
         }

        //Single Particle cuts on cluster map and track points rather do not have sense
        if (tpts)
         {
           track->SetTrackPoints(tpts); 
         }

        if (cmap) 
         { 
           track->SetClusterMap(cmap);
         }

        fEventRec->AddParticle(track);
        if (particle) fEventSim->AddParticle(particle);
        keeppart = kTRUE;

        if (AliVAODParticle::GetDebug() > 4 )
         {
           Info("ReadNext","\n\nAdding Particle with incarnation %d",pdgcode);
           track->Print();
           if (particle) particle->Print();
           Info("ReadNext","\n----------------------------------------------\n");
         }
      }//for (Int_t s = 0; s<kNSpecies; s++)

     if (keeppart == kFALSE) 
      {
        delete particle;//particle was not stored in event
        delete tpts;
        delete cmap;
      }

   }//for (Int_t i = 0;i<ntr; i++)  -- loop over tracks

  Info("ReadNext","Read %d tracks and %d particles from event %d (event %d in dir %d).",
         fEventRec->GetNumberOfParticles(), fEventSim->GetNumberOfParticles(),
         fNEventsRead,fCurrentEvent,fCurrentDir);
  fTrackCounter->Fill(fEventRec->GetNumberOfParticles());
  return 0;
}

/**********************************************************/

void AliReaderESD::Rewind()
{
  //rewinds reading 
  delete fKeyIterator;
  delete fFile;
  fFile = 0x0;
  fKeyIterator = 0x0;
  delete fRunLoader;
  fRunLoader = 0x0;
  fCurrentDir = 0;
  fNEventsRead = 0;
  if (fTrackCounter) fTrackCounter->Reset();
}
/**********************************************************/

TFile* AliReaderESD::OpenFile(Int_t n)
{
//opens fFile with kine tree

 const TString& dirname = GetDirName(n);
 if (dirname == "")
  {
   Error("OpenFiles","Can not get directory name");
   return 0x0;
  }
 TString filename = dirname +"/"+ fESDFileName;
 TFile *ret = TFile::Open(filename.Data()); 

 if ( ret == 0x0)
  {
    Error("OpenFiles","Can't open fFile %s",filename.Data());
    return 0x0;
  }
 if (!ret->IsOpen())
  {
    Error("OpenFiles","Can't open fFile  %s",filename.Data());
    return 0x0;
  }
 
 if (fReadSim )
  {
   fRunLoader = AliRunLoader::Open(dirname +"/"+ fGAlFileName);
   if (fRunLoader == 0x0)
    {
      Error("OpenFiles","Can't get RunLoader for directory %s",dirname.Data());
      delete ret;
      return 0x0;
    }
    
   fRunLoader->LoadHeader();
   if (fRunLoader->LoadKinematics())
    {
      Error("Next","Error occured while loading kinematics.");
      delete fRunLoader;
      delete ret;
      return 0x0;
    }
  }
   
 return ret;
}
/**********************************************************/

Int_t AliReaderESD::GetSpeciesPdgCode(ESpecies spec)//skowron
{
  //returns pdg code from the PID index
  //ask jura about charge
  switch (spec)
   {
     case kESDElectron:
       return kPositron;
       break;
     case kESDMuon:
       return kMuonPlus;
       break;
     case kESDPion:
       return kPiPlus;
       break;
     case kESDKaon:
       return kKPlus;
       break;
     case kESDProton:
       return kProton;
       break;
     default:
       ::Warning("GetSpeciesPdgCode","Specie with number %d is not defined.",(Int_t)spec);
       break;
   }
  return 0;
}
/********************************************************************/
Bool_t AliReaderESD::CheckTrack(AliESDtrack* t) const
{
  //Performs check of the track
  
  if ( (t->GetConstrainedChi2() < fChi2Min) || (t->GetConstrainedChi2() > fChi2Max) ) return kTRUE;
  
  if ( (t->GetTPCclusters(0x0) < fNTPCClustMin) || (t->GetTPCclusters(0x0) > fNTPCClustMax) ) return kTRUE;

  if (t->GetTPCclusters(0x0) > 0)
   {
     Float_t chisqpercl = t->GetTPCchi2()/((Double_t)t->GetTPCclusters(0x0));
     if ( (chisqpercl < fTPCChi2PerClustMin) || (chisqpercl > fTPCChi2PerClustMax) ) return kTRUE;
   }

  Double_t cc[15];
  t->GetConstrainedExternalCovariance(cc);

  if ( (cc[0]  < fC00Min) || (cc[0]  > fC00Max) ) return kTRUE;
  if ( (cc[2]  < fC11Min) || (cc[2]  > fC11Max) ) return kTRUE;
  if ( (cc[5]  < fC22Min) || (cc[5]  > fC22Max) ) return kTRUE;
  if ( (cc[9]  < fC33Min) || (cc[9]  > fC33Max) ) return kTRUE;
  if ( (cc[14] < fC44Min) || (cc[14] > fC44Max) ) return kTRUE;


  t->GetInnerExternalCovariance(cc);

  if ( (cc[0]  < fTPCC00Min) || (cc[0]  > fTPCC00Max) ) return kTRUE;
  if ( (cc[2]  < fTPCC11Min) || (cc[2]  > fTPCC11Max) ) return kTRUE;
  if ( (cc[5]  < fTPCC22Min) || (cc[5]  > fTPCC22Max) ) return kTRUE;
  if ( (cc[9]  < fTPCC33Min) || (cc[9]  > fTPCC33Max) ) return kTRUE;
  if ( (cc[14] < fTPCC44Min) || (cc[14] > fTPCC44Max) ) return kTRUE;

  return kFALSE;

}
/********************************************************************/

void AliReaderESD::SetChi2Range(Float_t min, Float_t max)
{
  //sets range of Chi2 per Cluster
  fChi2Min = min;
  fChi2Max = max;
}
/********************************************************************/

void AliReaderESD::SetTPCNClustersRange(Int_t min,Int_t max)
{
 //sets range of Number Of Clusters that tracks have to have
 fNTPCClustMin = min;
 fNTPCClustMax = max;
}
/********************************************************************/

void AliReaderESD::SetTPCChi2PerCluserRange(Float_t min, Float_t max)
{
  //sets range of Chi2 per Cluster
  fTPCChi2PerClustMin = min;
  fTPCChi2PerClustMax = max;
}
/********************************************************************/

void AliReaderESD::SetC00Range(Float_t min, Float_t max)
{
 //Sets range of C00 parameter of covariance matrix of the track
 //it defines uncertainty of the momentum
  fC00Min = min;
  fC00Max = max;
}
/********************************************************************/

void AliReaderESD::SetC11Range(Float_t min, Float_t max)
{
 //Sets range of C11 parameter of covariance matrix of the track
 //it defines uncertainty of the momentum
  fC11Min = min;
  fC11Max = max;
}
/********************************************************************/

void AliReaderESD::SetC22Range(Float_t min, Float_t max)
{
 //Sets range of C22 parameter of covariance matrix of the track
 //it defines uncertainty of the momentum
  fC22Min = min;
  fC22Max = max;
}
/********************************************************************/

void AliReaderESD::SetC33Range(Float_t min, Float_t max)
{
 //Sets range of C33 parameter of covariance matrix of the track
 //it defines uncertainty of the momentum
  fC33Min = min;
  fC33Max = max;
}
/********************************************************************/

void AliReaderESD::SetC44Range(Float_t min, Float_t max)
{
 //Sets range of C44 parameter of covariance matrix of the track
 //it defines uncertainty of the momentum
  fC44Min = min;
  fC44Max = max;
}

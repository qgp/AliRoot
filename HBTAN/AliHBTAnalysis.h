#ifndef ALIHBTANALYSIS_H
#define ALIHBTANALYSIS_H

#include <TObject.h>



class AliHBTParticleCut;
class AliHBTCut;
class AliHBTPairCut;
class AliHBTPair;

class AliHBTRun;
class AliHBTEvent;
class AliHBTReader;
class AliHBTOnePairFctn;      
class AliHBTTwoPairFctn;

class AliHBTMonOneParticleFctn;
class AliHBTMonTwoParticleFctn;

class TList;

class AliHBTAnalysis: public TObject
 {
   public:
     AliHBTAnalysis();

     virtual ~AliHBTAnalysis();

     virtual void Process(Option_t* option = "TracksAndParticles");
     

     void SetGlobalPairCut(AliHBTPairCut* cut);
     
     void AddTrackFunction(AliHBTOnePairFctn* f);
     void AddParticleFunction(AliHBTOnePairFctn* f);
     void AddParticleAndTrackFunction(AliHBTTwoPairFctn* f);
     
     void AddParticleMonitorFunction(AliHBTMonOneParticleFctn* f);    //z.ch.
     void AddTrackMonitorFunction(AliHBTMonOneParticleFctn* f);    //z.ch.
     void AddParticleAndTrackMonitorFunction(AliHBTMonTwoParticleFctn* f);//z.ch.

     void AddResolutionFunction(AliHBTTwoPairFctn* f){AddParticleAndTrackFunction(f);}
     
     void SetReader(AliHBTReader* r){fReader = r;}
     
     void WriteFunctions();
     
     void SetBufferSize(Int_t buffsize){fBufferSize=buffsize;}
     
     Bool_t IsNonIdentAnalysis();
   protected:
     
     Bool_t RunCoherencyCheck();
     
     void FilterOut(AliHBTEvent* outpart1, AliHBTEvent* outpart2, AliHBTEvent* inpart,
                    AliHBTEvent* outtrack1, AliHBTEvent* outtrack2, AliHBTEvent* intrack);
     void FilterOut(AliHBTEvent* out1, AliHBTEvent* out2, AliHBTEvent* in);
     
     AliHBTReader* fReader;//!
     
     virtual void ProcessTracks();
     virtual void ProcessParticles();
     virtual void ProcessTracksAndParticles();
     
     virtual void ProcessTracksAndParticlesNonIdentAnal();
     virtual void ProcessParticlesNonIdentAnal();
     virtual void ProcessTracksNonIdentAnal();
     
     AliHBTOnePairFctn**  fTrackFunctions; //!array of pointers to functions that analyze rekonstructed tracks
     AliHBTOnePairFctn**  fParticleFunctions; //!array of pointers to functions that analyze generated particles
     AliHBTTwoPairFctn**  fParticleAndTrackFunctions; //!array of pointers to functions that analyze both 
                                        //reconstructed tracks and generated particles
		//i.e. - resolution analyzers

     AliHBTMonOneParticleFctn**  fParticleMonitorFunctions; // array of pointers to monitoring functions
     AliHBTMonOneParticleFctn**  fTrackMonitorFunctions; // which are used for single particle analysis,
     AliHBTMonTwoParticleFctn**  fParticleAndTrackMonitorFunctions;  // cut monitoring, etc.

     UInt_t fNTrackFunctions; //!
     UInt_t fNParticleFunctions; //!
     UInt_t fNParticleAndTrackFunctions; //!
		
     UInt_t fNParticleMonitorFunctions; //! 
     UInt_t fNTrackMonitorFunctions; //! 
     UInt_t fNParticleAndTrackMonitorFunctions; //! 

     /**********************************************/
     /* Control parameters  */

      AliHBTPairCut *fPairCut;//!
      
      Int_t fBufferSize; //defines the size of buffer for mixed events; -1==MIX All
     /**********************************************/
     
     
   private:
     static const Int_t fgkHbtAnalyzeAll;//!
     static const UInt_t fgkFctnArraySize;//!
/*********************************************/   
   public:
     ClassDef(AliHBTAnalysis,0)
 };




#endif

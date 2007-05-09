/***************************************************************************
 *
 * $Id$
 *
 * Author: Mike Lisa, Ohio State, lisa@mps.ohio-state.edu
 ***************************************************************************
 *
 * Description: part of STAR HBT Framework: AliFemtoMaker package
 *  PicoEvents are last-step ultra-compressed "events" just containing
 *  bare information about the particles of interest.  They have already
 *  gone through Event and Track cuts, so only Pair cuts are left.
 *  PicoEvents are *internal* to the code, and are stored in the
 *  Event-mixing buffers.
 *           
 *
 ***************************************************************************
 *
 * $Log$
 * Revision 1.1.1.1  2007/03/07 10:14:49  mchojnacki
 * First version on CVS
 *
 * Revision 1.4  2000/07/16 21:38:23  laue
 * AliFemtoCoulomb.cxx AliFemtoSectoredAnalysis.cxx : updated for standalone version
 * AliFemtoV0.cc AliFemtoV0.h : some cast to prevent compiling warnings
 * AliFemtoParticle.cc AliFemtoParticle.h : pointers mTrack,mV0 initialized to 0
 * AliFemtoIOBinary.cc : some printouts in #ifdef STHBTDEBUG
 * AliFemtoEvent.cc : B-Field set to 0.25Tesla, we have to think about a better
 *                 solution
 *
 * Revision 1.3  2000/06/01 20:40:13  laue
 * AliFemtoIO.cc: updated for new V0s
 * AliFemtoPicoEvent.cc: collections especially cleared
 * franks1DHistoD.h, include changed from  <stdio> to <cstdio>
 * franks1DHistoD.cc, cout statement deleted
 *
 * Revision 1.2  2000/03/17 17:23:05  laue
 * Roberts new three particle correlations implemented.
 *
 * Revision 1.1.1.1  1999/06/29 16:02:57  lisa
 * Installation of AliFemtoMaker
 *
 **************************************************************************/

#include "Infrastructure/AliFemtoPicoEvent.h"

//________________
AliFemtoPicoEvent::AliFemtoPicoEvent(){
  fFirstParticleCollection = new AliFemtoParticleCollection;
  fSecondParticleCollection = new AliFemtoParticleCollection;
  fThirdParticleCollection = new AliFemtoParticleCollection;
}
//_________________
AliFemtoPicoEvent::~AliFemtoPicoEvent(){
  AliFemtoParticleIterator iter;


  if (fFirstParticleCollection){
      for (iter=fFirstParticleCollection->begin();iter!=fFirstParticleCollection->end();iter++){
	delete *iter;
      }
      fFirstParticleCollection->clear();
      delete fFirstParticleCollection;
      fFirstParticleCollection = 0;
  }

  if (fSecondParticleCollection){
    for (iter=fSecondParticleCollection->begin();iter!=fSecondParticleCollection->end();iter++){
      delete *iter;
    }
    fSecondParticleCollection->clear();
    delete fSecondParticleCollection;
    fSecondParticleCollection = 0;
  }

  if (fThirdParticleCollection){
    if (fThirdParticleCollection->size() != 0 ) {
      for (iter=fThirdParticleCollection->begin();iter!=fThirdParticleCollection->end();iter++){
	delete *iter;
      }
    }
    fThirdParticleCollection->clear();
    delete fThirdParticleCollection;
    fThirdParticleCollection = 0;
  }
}
//_________________

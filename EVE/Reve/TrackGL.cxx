// $Header$

#include "TrackGL.h"
#include <Reve/Track.h>
#include <Reve/GLUtilNS.h>

#include <TGLSelectRecord.h>

#include <TGLIncludes.h>

using namespace Reve;

//______________________________________________________________________
// TrackGL
//

ClassImp(TrackGL)

TrackGL::TrackGL() : LineGL()
{
  // fDLCache = false; // Disable display list.
}

TrackGL::~TrackGL()
{}

/**************************************************************************/

Bool_t TrackGL::SetModel(TObject* obj, const Option_t* /*opt*/)
{
  if(LineGL::SetModel(obj) == kFALSE) return kFALSE;
  if(SetModelCheckClass(obj, Track::Class())) {
    fTrack = dynamic_cast<Track*>(obj);
    return kTRUE;
  }
  return kFALSE;
}
/**************************************************************************/

void TrackGL::ProcessSelection(TGLRnrCtx & /*rnrCtx*/, TGLSelectRecord & rec)
{
  // Processes secondary selection from TGLViewer.
  // Calls TPointSet3D::PointSelected(Int_t) with index of selected
  // point as an argument.

  printf("TrackGL::ProcessSelection %d names on the stack (z1=%g, z2=%g).\n",
	 rec.GetN(), rec.GetMinZ(), rec.GetMaxZ());
  printf("  Names: ");
  for (Int_t j=0; j<rec.GetN(); ++j) printf ("%d ", rec.GetItem(j));
  printf("\n");

  ((Track*)fM)->CtrlClicked((Track*)fM);
}

/**************************************************************************/
void TrackGL::DirectDraw(TGLRnrCtx & rnrCtx) const
{
  // track 
  LineGL::DirectDraw(rnrCtx);

  // path-marks
  TrackRnrStyle& RS = *fTrack->GetRnrStyle();
  Int_t N =  fTrack->fVisPathMarks.size();   
  if ( N && RS.fPMSize > 0.)
  { Float_t* pnts = new Float_t[3*N];
    for(Int_t i=0; i<N; i++) 
    {
      pnts[i*3]   = fTrack->fVisPathMarks[i].x;
      pnts[i*3+1] = fTrack->fVisPathMarks[i].y;
      pnts[i*3+2] = fTrack->fVisPathMarks[i].z;
    }
    TAttMarker ms(RS.fPMColor, RS.fPMStyle, RS.fPMSize);
    GLUtilNS::RenderPolyMarkers(ms, pnts, N);
    delete [] pnts;
  }
}

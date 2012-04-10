#include "AliITSSumTP.h"
#include "AliTrackPointArray.h"

///////////////////////////////////////////////////////////////////
//                                                               //
// Class for ITS trackpoints summary + some aux. info  )         //
// Author: Ruben Shahoian                                        //
//                                                               //
///////////////////////////////////////////////////////////////////

/* $Id$ */

ClassImp(AliITSSumTP)

//__________________________________________
AliITSSumTP::AliITSSumTP(const AliITSSumTP& src) : 
		   TObject(src), fTracks(src.fTracks.GetEntriesFast()), fVertex(src.GetVertex()), 
		   fNVars(src.fNVars), fCrvVars(0)
{
  // copy c-tor
  fCrvVars = new Double32_t[fNVars];
  TObjArray& arrSrc = src.GetTracks();
  for (int i=fNVars;i--;) fCrvVars[i] = src.fCrvVars[i];
  for (int i=arrSrc.GetEntriesFast();i--;) fTracks.AddAtAndExpand(arrSrc.UncheckedAt(i),i);
}

//__________________________________________
AliITSSumTP& AliITSSumTP::operator=(const AliITSSumTP& src)
{
  // assignment op-r
  if (this == &src) return *this;
  Reset();
  TObject::operator=(src);
  fVertex = src.GetVertex();
  fNVars = src.fNVars;
  fCrvVars = new Double32_t[fNVars];
  TObjArray& arrSrc = src.GetTracks();
  for (int i=fNVars;i--;) fCrvVars[i] = src.fCrvVars[i];
  for (int i=arrSrc.GetEntriesFast();i--;) fTracks.AddAtAndExpand(arrSrc.UncheckedAt(i),i);
  return *this;
}

//__________________________________________
void AliITSSumTP::BookNTracks(Int_t n)
{
  // book space for tracks info
  delete[] fCrvVars; fCrvVars = 0;
  fNVars = n*kNVarPerTrack; 
  if (fNVars>0) {
    fCrvVars = new Double32_t[fNVars];
    for (int i=fNVars;i--;) fCrvVars[i]=0; 
  }
}

//__________________________________________
void AliITSSumTP::Reset()
{
  // reset object
  fTracks.Delete();
  delete[] fCrvVars; 
  fCrvVars = 0;
  fNVars = 0;
  SetUniqueID(0);
}

//__________________________________________
void AliITSSumTP::Print(Option_t *) const
{
  // reset object
  printf("Vertex: "); fVertex.Print();
  int ntr = GetNTracks();
  printf("Number of tracks: %d\n",ntr);
  for (int itr=0;itr<ntr;itr++) {
    AliTrackPointArray* tr = GetTrack(itr);
    printf("#%2d : %d hits CrvGlo: %+.2e/%+.2e CrvTPC: %+.2e/%+.2e\n",itr,tr->GetNPoints(),
	   GetCrvGlo(itr),GetCrvGloErr(itr),
	   GetCrvTPC(itr),GetCrvTPCErr(itr));
  }
  //
}


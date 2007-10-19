/***************************************************************************
 *
 * $Id$
 *
 * Author: Adam Kisiel, Ohio State, kisiel@mps.ohio-state.edu
 ***************************************************************************
 *
 * Description: part of STAR HBT Framework: AliFemtoMaker package
 *   a cut to remove "shared" and "split" pairs
 *
 ***************************************************************************
 *
 *
 **************************************************************************/

#include "AliFemtoShareQualityKTPairCut.h"
#include <string>
#include <cstdio>

#ifdef __ROOT__
ClassImp(AliFemtoShareQualityKTPairCut)
#endif

//__________________
AliFemtoShareQualityKTPairCut::AliFemtoShareQualityKTPairCut():
  AliFemtoShareQualityPairCut(),
  fKTMin(0),
  fKTMax(1.0e6)
{
}
//__________________
AliFemtoShareQualityKTPairCut::AliFemtoShareQualityKTPairCut(const AliFemtoShareQualityKTPairCut& c) : 
  AliFemtoShareQualityPairCut(c),
  fKTMin(0),
  fKTMax(1.0e6)
{ 
  fKTMin = c.fKTMin;
  fKTMax = c.fKTMax;
}

//__________________
AliFemtoShareQualityKTPairCut::~AliFemtoShareQualityKTPairCut(){
  /* no-op */
}
//__________________
bool AliFemtoShareQualityKTPairCut::Pass(const AliFemtoPair* pair){
  bool temp = true;
  
  if (pair->KT() < fKTMin)
    temp = false;

  if (pair->KT() > fKTMax)
    temp = false;

  if (temp) {
    temp = AliFemtoShareQualityPairCut::Pass(pair);
  }
  else
    fNPairsFailed++;

  return temp;
}
//__________________
AliFemtoString AliFemtoShareQualityKTPairCut::Report(){
  string Stemp = "AliFemtoShareQuality Pair Cut - remove shared and split pairs\n";  char Ctemp[100];
  sprintf(Ctemp,"Accept pair with kT in range %lf , %lf",fKTMin,fKTMax);
  Stemp += Ctemp;
  sprintf(Ctemp,"Number of pairs which passed:\t%ld  Number which failed:\t%ld\n",fNPairsPassed,fNPairsFailed);
  Stemp += Ctemp;
  AliFemtoString returnThis = Stemp;
  return returnThis;}
//__________________

TList *AliFemtoShareQualityKTPairCut::ListSettings()
{
  // return a list of settings in a writable form
  TList *tListSetttings =  AliFemtoShareQualityPairCut::ListSettings();
  char buf[200];
  snprintf(buf, 200, "AliFemtoShareQualityKTPairCut.ktmax=%lf", fKTMax);
  tListSetttings->AddLast(new TObjString(buf));
  snprintf(buf, 200, "AliFemtoShareQualityKTPairCut.ktmin=%lf", fKTMin);
  tListSetttings->AddLast(new TObjString(buf));

  return tListSetttings;
}

void AliFemtoShareQualityKTPairCut::SetKTRange(double ktmin, double ktmax)
{
  fKTMin = ktmin;
  fKTMax = ktmax;
}

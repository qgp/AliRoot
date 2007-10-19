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

#include "AliFemtoKTPairCut.h"
#include <string>
#include <cstdio>

#ifdef __ROOT__
ClassImp(AliFemtoKTPairCut)
#endif

//__________________
AliFemtoKTPairCut::AliFemtoKTPairCut():
  AliFemtoPairCut(),
  fKTMin(0),
  fKTMax(1.0e6)
{
}
//__________________
AliFemtoKTPairCut::AliFemtoKTPairCut(const AliFemtoKTPairCut& c) : 
  AliFemtoPairCut(c),
  fKTMin(0),
  fKTMax(1.0e6)
{ 
  fKTMin = c.fKTMin;
  fKTMax = c.fKTMax;
}

//__________________
AliFemtoKTPairCut::~AliFemtoKTPairCut(){
  /* no-op */
}
//__________________
bool AliFemtoKTPairCut::Pass(const AliFemtoPair* pair){
  bool temp = true;
  
  if (pair->KT() < fKTMin)
    temp = false;

  if (pair->KT() > fKTMax)
    temp = false;

  return temp;
}
//__________________
AliFemtoString AliFemtoKTPairCut::Report(){
  string Stemp = "AliFemtoKT Pair Cut \n";  char Ctemp[100];
  sprintf(Ctemp,"Accept pair with kT in range %lf , %lf",fKTMin,fKTMax);
  Stemp += Ctemp;
  AliFemtoString returnThis = Stemp;
  return returnThis;}
//__________________

TList *AliFemtoKTPairCut::ListSettings()
{
  // return a list of settings in a writable form
  TList *tListSetttings =  new TList();
  char buf[200];
  snprintf(buf, 200, "AliFemtoKTPairCut.ktmax=%lf", fKTMax);
  tListSetttings->AddLast(new TObjString(buf));
  snprintf(buf, 200, "AliFemtoKTPairCut.ktmin=%lf", fKTMin);
  tListSetttings->AddLast(new TObjString(buf));

  return tListSetttings;
}

void AliFemtoKTPairCut::SetKTRange(double ktmin, double ktmax)
{
  fKTMin = ktmin;
  fKTMax = ktmax;
}

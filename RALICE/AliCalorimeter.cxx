/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

// $Id$

///////////////////////////////////////////////////////////////////////////
// Class AliCalorimeter
// Description of a modular calorimeter system.
// A matrix geometry is used in which a module is identified by (row,col).
// Note : First module is identified as (1,1).
//
// This is the way to define and enter signals into a calorimeter :
//
//   AliCalorimeter cal(10,15);  // Calorimeter of 10x15 modules
//                               // All module signals set to 0.
//   cal.AddSignal(5,7,85.4);
//   cal.AddSignal(5,7,25.9);
//   cal.AddSignal(3,5,1000);
//   cal.SetSignal(5,7,10.3);
//   cal.Reset(3,5);             // Reset module (3,5) as being 'not fired'
//                               // All module data are re-initialised.
//   cal.SetEdgeOn(1,1);         // Declare module (1,1) as an 'edge module'
//   cal.SetDead(8,3);
//   cal.SetGain(2,8,3.2);
//
//   Float_t vec[3]={6,1,20};
//   cal.SetPosition(2,8,vec,"car");
//
//   AliSignal s;
//   Float_t loc[3]={-1,12,3};
//   s.SetPosition(loc,"car");
//   s.SetSignal(328);
//   cal.AddVetoSignal(s); // Associate (extrapolated) signal as a veto
//
//   cal.Group(2);      // Group 'fired' modules into clusters
//                      // Perform grouping over 2 rings around the center
//   cal.Reset();       // Reset the complete calorimeter
//                      // Normally to prepare for the next event data
//                      // Note : Module gain, edge and dead flags remain
//
//--- Author: Nick van Eijndhoven 13-jun-1997 UU-SAP Utrecht
//- Modified: NvE $Date$ UU-SAP Utrecht
///////////////////////////////////////////////////////////////////////////

#include "AliCalorimeter.h"

ClassImp(AliCalorimeter) // Class implementation to enable ROOT I/O
 
AliCalorimeter::AliCalorimeter()
{
// Default constructor, all parameters set to 0
 fNrows=0;
 fNcolumns=0;
 fNsignals=0;
 fNclusters=0;
 fMatrix=0;
 fClusters=0;
 fModules=0;
 fHmodules=0;
 fHclusters=0;
 fNvetos=0;
 fVetos=0;
 fAttributes=0;
 fGains=0;
 fPositions=0;
 fName=" ";
}
///////////////////////////////////////////////////////////////////////////
AliCalorimeter::~AliCalorimeter()
{
// Destructor to delete memory allocated to the various arrays and matrix
 if (fModules)
 {
  delete fModules;
  fModules=0;
 }
 if (fClusters)
 {
  delete fClusters;
  fClusters=0;
 }
 if (fVetos)
 {
  delete fVetos;
  fVetos=0;
 }
 if (fHmodules)
 {
  delete fHmodules;
  fHmodules=0;
 }
 if (fHclusters)
 {
  delete fHclusters;
  fHclusters=0;
 }
 if (fMatrix || fPositions)
 {
  for (Int_t i=0; i<fNrows; i++)
  {
   for (Int_t j=0; j<fNcolumns; j++)
   {
    fMatrix[i][j]=0;
    if (fPositions[i][j]) delete fPositions[i][j];
   }
  }
  if (fMatrix)
  {
   delete [] fMatrix;
   fMatrix=0;
  }
  if (fPositions)
  {
   delete [] fPositions;
   fPositions=0;
  }
 }
 if (fGains)
 {
  delete fGains;
  fGains=0;
 }
 if (fAttributes)
 {
  delete fAttributes;
  fAttributes=0;
 }
}
///////////////////////////////////////////////////////////////////////////
AliCalorimeter::AliCalorimeter(Int_t nrow,Int_t ncol)
{
// Create a calorimeter module matrix
 fNrows=nrow;
 fNcolumns=ncol;
 fNsignals=0;
 fModules=0;
 fNclusters=0;
 fClusters=0;
 fAttributes=new TMatrix(nrow,ncol);
 fGains=new TMatrix(nrow,ncol);
 fMatrix=new AliCalmodule**[nrow];
 fPositions=new AliPosition**[nrow];
 for (Int_t row=0; row<nrow; row++)
 {
  fMatrix[row]=new AliCalmodule*[ncol];
  fPositions[row]=new AliPosition*[ncol];
  // Initialise the various matrices
  for (Int_t col=0; col<ncol; col++)
  {
   fMatrix[row][col]=0;
   fPositions[row][col]=0;
   (*fGains)(row,col)=1;
   (*fAttributes)(row,col)=0;
  }
 }

 // Mark the edge modules
 for (Int_t j=0; j<ncol; j++)
 {
  (*fAttributes)(0,j)=10;
  (*fAttributes)(nrow-1,j)=10;
 }
 for (Int_t i=0; i<nrow; i++)
 {
  (*fAttributes)(i,0)=10;
  (*fAttributes)(i,ncol-1)=10;
 }
 
 fHmodules=0;
 fHclusters=0;

 fNvetos=0;
 fVetos=0;

 fName=" ";
}
///////////////////////////////////////////////////////////////////////////
Int_t AliCalorimeter::GetNrows()
{
// Provide the number of rows for the calorimeter module matrix
 return fNrows;
}
///////////////////////////////////////////////////////////////////////////
Int_t AliCalorimeter::GetNcolumns()
{
// Provide the number of columns for the calorimeter module matrix
 return fNcolumns;
}
///////////////////////////////////////////////////////////////////////////
void AliCalorimeter::SetSignal(Int_t row,Int_t col,Float_t sig)
{
// Set the signal for a certain calorimeter module
 
 if (!fMatrix) LoadMatrix(); // Restore matrix data in case of reading input
 
 if (row>0 && row<=fNrows && col>0 && col<=fNcolumns)
 {
  AliCalmodule* m=fMatrix[row-1][col-1];
  if (!m) // only count new modules
  {
   if (!fModules)
   {
    fModules=new TObjArray();  // Default size, expanded automatically
    fModules->SetOwner();
   }
   fNsignals++;
   m=new AliCalmodule();
   AliPosition* r=fPositions[row-1][col-1];
   if (r) m->SetPosition(*r);
   fModules->Add(m);
   fMatrix[row-1][col-1]=m;
  }
  m->SetSignal(row,col,sig);
 }
 else
 {
  cout << " *AliCalorimeter::SetSignal* row,col : " << row << "," << col
       << " out of range." << endl;
  cout << " Nrows,Ncols = " << fNrows << "," << fNcolumns << endl;
 }
}
///////////////////////////////////////////////////////////////////////////
void AliCalorimeter::AddSignal(Int_t row, Int_t col, Float_t sig)
{
// Add the signal to a certain calorimeter module
 
 if (!fMatrix) LoadMatrix(); // Restore matrix data in case of reading input
 
 if (row>0 && row<=fNrows && col>0 && col<=fNcolumns)
 {
  AliCalmodule* m=fMatrix[row-1][col-1];
  if (!m) // only count new modules
  {
   SetSignal(row,col,sig);
  }
  else
  {
   m->AddSignal(row,col,sig);
  }
 }
 else
 {
  cout << " *AliCalorimeter::AddSignal* row,col : " << row << "," << col
       << " out of range." << endl;
  cout << " Nrows,Ncols = " << fNrows << "," << fNcolumns << endl;
 }
}
///////////////////////////////////////////////////////////////////////////
void AliCalorimeter::AddSignal(AliCalmodule* mod)
{
// Add the signal of module mod to the current calorimeter data.
// This enables mixing of calorimeter data of various events.
 
 if (!fMatrix) LoadMatrix(); // Restore matrix data in case of reading input
 
 Int_t row=mod->GetRow();
 Int_t col=mod->GetColumn();
 Float_t sig=mod->GetSignal();
 AliPosition r=mod->GetPosition();

 if (row>0 && row<=fNrows && col>0 && col<=fNcolumns)
 {
  AliCalmodule* m=fMatrix[row-1][col-1];
  if (!m) // No module existed yet at this position
  {
   if (!fModules)
   {
    fModules=new TObjArray();  // Default size, expanded automatically
    fModules->SetOwner();
   }
   fNsignals++;
   m=new AliCalmodule;
   fModules->Add(m);
   fMatrix[row-1][col-1]=m;
   m->SetPosition(r);
  }
  m->AddSignal(row,col,sig);
  if (!fPositions[row-1][col-1]) fPositions[row-1][col-1]=new AliPosition(r);
 }
 else
 {
  cout << " *AliCalorimeter::AddSignal* row,col : " << row << "," << col
       << " out of range." << endl;
  cout << " Nrows,Ncols = " << fNrows << "," << fNcolumns << endl;
 }
}
///////////////////////////////////////////////////////////////////////////
void AliCalorimeter::Reset(Int_t row,Int_t col)
{
// Reset the signal for a certain calorimeter module
 
 if (!fMatrix) LoadMatrix(); // Restore matrix data in case of reading input
 
 if (row>0 && row<=fNrows && col>0 && col<=fNcolumns)
 {
  AliCalmodule* m=fMatrix[row-1][col-1];
  if (m)
  {
   fModules->Remove(m);
   fNsignals--;
   fModules->Compress();
   delete m;
   fMatrix[row-1][col-1]=0;
  }
 }
 else
 {
  cout << " *AliCalorimeter::Reset* row,col : " << row << "," << col
       << " out of range." << endl;
  cout << " Nrows,Ncols = " << fNrows << "," << fNcolumns << endl;
 }
}
///////////////////////////////////////////////////////////////////////////
void AliCalorimeter::Reset()
{
// Reset the signals for the complete calorimeter
// Normally this is done to prepare for the data of the next event
// Note : Module gains, edge and dead flags remain unchanged
 
 if (!fMatrix) LoadMatrix(); // Restore matrix data in case of reading input

 fNsignals=0;
 if (fModules)
 {
  delete fModules;
  fModules=0;
 }
 for (Int_t i=0; i<fNrows; i++)
 {
  for (Int_t j=0; j<fNcolumns; j++)
  {
   fMatrix[i][j]=0;
  }
 }

 fNclusters=0;
 if (fClusters)
 {
  delete fClusters;
  fClusters=0;
 }

 fNvetos=0;
 if (fVetos)
 {
  delete fVetos;
  fVetos=0;
 }
}
///////////////////////////////////////////////////////////////////////////
Float_t AliCalorimeter::GetSignal(Int_t row,Int_t col)
{
// Provide the signal of a certain calorimeter module.
// In case the module was marked dead, 0 is returned.
 
 if (!fMatrix) LoadMatrix(); // Restore matrix data in case of reading input
 
 if (row>0 && row<=fNrows && col>0 && col<=fNcolumns)
 {
  Float_t signal=0;
  AliCalmodule* m=fMatrix[row-1][col-1];
  if (m)
  {
   Int_t dead=m->GetDeadValue();
   if (!dead) signal=m->GetSignal();
  }
  return signal;
 }
 else
 {
  cout << " *AliCalorimeter::GetSignal* row,col : " << row << "," << col
       << " out of range." << endl;
  cout << " Nrows,Ncols = " << fNrows << "," << fNcolumns << endl;
  return 0;
 }
}
///////////////////////////////////////////////////////////////////////////
void AliCalorimeter::SetEdgeOn(Int_t row,Int_t col)
{
// Indicate a certain calorimeter module as 'edge module'
 
 if (!fMatrix) LoadMatrix(); // Restore matrix data in case of reading input
 
 if (row>0 && row<=fNrows && col>0 && col<=fNcolumns)
 {
  Float_t word=(*fAttributes)(row-1,col-1);
  Int_t iword=int(word+0.1);
  Int_t dead=iword%10;
  Int_t edge=1;
  (*fAttributes)(row-1,col-1)=float(dead+10*edge); 
 }
 else
 {
  cout << " *AliCalorimeter::SetEdgeOn* row,col : " << row << "," << col
       << " out of range." << endl;
  cout << " Nrows,Ncols = " << fNrows << "," << fNcolumns << endl;
 }
}
///////////////////////////////////////////////////////////////////////////
void AliCalorimeter::SetEdgeOff(Int_t row,Int_t col)
{
// Indicate a certain calorimeter module as 'non-edge module'
 
 if (!fMatrix) LoadMatrix(); // Restore matrix data in case of reading input
 
 if (row>0 && row<=fNrows && col>0 && col<=fNcolumns)
 {
  Float_t word=(*fAttributes)(row-1,col-1);
  Int_t iword=int(word+0.1);
  Int_t dead=iword%10;
  Int_t edge=0;
  (*fAttributes)(row-1,col-1)=float(dead+10*edge); 
 }
 else
 {
  cout << " *AliCalorimeter::SetEdgeOff* row,col : " << row << "," << col
       << " out of range." << endl;
  cout << " Nrows,Ncols = " << fNrows << "," << fNcolumns << endl;
 }
}
///////////////////////////////////////////////////////////////////////////
void AliCalorimeter::SetDead(Int_t row,Int_t col)
{
// Indicate a certain calorimeter module as 'dead module'
 
 if (!fMatrix) LoadMatrix(); // Restore matrix data in case of reading input
 
 if (row>0 && row<=fNrows && col>0 && col<=fNcolumns)
 {
  Float_t word=(*fAttributes)(row-1,col-1);
  Int_t iword=int(word+0.1);
  Int_t edge=iword/10;
  Int_t dead=1;
  (*fAttributes)(row-1,col-1)=float(dead+10*edge);
  if (fMatrix[row-1][col-1]) (fMatrix[row-1][col-1])->SetDead();
 
  // Increase the 'edge value' of surrounding modules
  Int_t rlow=row-1;
  Int_t rup=row+1;
  Int_t clow=col-1;
  Int_t cup=col+1;
 
  if (rlow < 1) rlow=row;
  if (rup > fNrows) rup=fNrows;
  if (clow < 1) clow=col;
  if (cup > fNcolumns) cup=fNcolumns;
 
  for (Int_t i=rlow; i<=rup; i++)
  {
   for (Int_t j=clow; j<=cup; j++)
   {
    if (i!=row || j!=col) // No increase of edge value for the dead module itself
    {
     word=(*fAttributes)(i-1,j-1);
     iword=int(word+0.1);
     edge=iword/10;
     dead=iword%10;
     edge++;
     (*fAttributes)(i-1,j-1)=float(dead+10*edge);
    }
   }
  }
 }
 else
 {
  cout << " *AliCalorimeter::SetDead* row,col : " << row << "," << col
       << " out of range." << endl;
  cout << " Nrows,Ncols = " << fNrows << "," << fNcolumns << endl;
 }
}
///////////////////////////////////////////////////////////////////////////
void AliCalorimeter::SetAlive(Int_t row,Int_t col)
{
// Indicate a certain calorimeter module as 'active module'
 
 if (!fMatrix) LoadMatrix(); // Restore matrix data in case of reading input
 
 if (row>0 && row<=fNrows && col>0 && col<=fNcolumns)
 {
  Float_t word=(*fAttributes)(row-1,col-1);
  Int_t iword=int(word+0.1);
  Int_t edge=iword/10;
  Int_t dead=0;
  (*fAttributes)(row-1,col-1)=float(dead+10*edge);
  if (fMatrix[row-1][col-1]) (fMatrix[row-1][col-1])->SetAlive();
 
  // Decrease the 'edge value' of surrounding modules
  Int_t rlow=row-1;
  Int_t rup=row+1;
  Int_t clow=col-1;
  Int_t cup=col+1;
 
  if (rlow < 1) rlow=row;
  if (rup > fNrows) rup=fNrows;
  if (clow < 1) clow=col;
  if (cup > fNcolumns) cup=fNcolumns;
 
  for (Int_t i=rlow; i<=rup; i++)
  {
   for (Int_t j=clow; j<=cup; j++)
   {
    if (i!=row || j!=col) // No decrease of edge value for the dead module itself
    {
     word=(*fAttributes)(i-1,j-1);
     iword=int(word+0.1);
     edge=iword/10;
     dead=iword%10;
     if (edge>0) edge--;
     (*fAttributes)(i-1,j-1)=float(dead+10*edge);
    }
   }
  }
 }
 else
 {
  cout << " *AliCalorimeter::SetAlive* row,col : " << row << "," << col
       << " out of range." << endl;
  cout << " Nrows,Ncols = " << fNrows << "," << fNcolumns << endl;
 }
}
///////////////////////////////////////////////////////////////////////////
void AliCalorimeter::SetGain(Int_t row,Int_t col,Float_t gain)
{
// Set the gain value for a certain calorimeter module
 
 if (!fMatrix) LoadMatrix(); // Restore matrix data in case of reading input
 
 if (row>0 && row<=fNrows && col>0 && col<=fNcolumns)
 {
  (*fGains)(row-1,col-1)=gain;
  if (fMatrix[row-1][col-1]) (fMatrix[row-1][col-1])->SetGain(gain);
 }
 else
 {
  cout << " *AliCalorimeter::SetGain* row,col : " << row << "," << col
       << " out of range." << endl;
  cout << " Nrows,Ncols = " << fNrows << "," << fNcolumns << endl;
 }
}
///////////////////////////////////////////////////////////////////////////
void AliCalorimeter::SetPosition(Int_t row,Int_t col,Float_t* vec,TString f)
{
// Set the position in user coordinates for a certain calorimeter module
 
 if (!fMatrix) LoadMatrix(); // Restore matrix data in case of reading input
 
 if (row>0 && row<=fNrows && col>0 && col<=fNcolumns)
 {
  if (!fPositions[row-1][col-1]) fPositions[row-1][col-1]=new AliPosition;
  (fPositions[row-1][col-1])->SetVector(vec,f);
  if (fMatrix[row-1][col-1]) (fMatrix[row-1][col-1])->SetPosition(vec,f);
 }
 else
 {
  cout << " *AliCalorimeter::SetPosition* row,col : " << row << "," << col
       << " out of range." << endl;
  cout << " Nrows,Ncols = " << fNrows << "," << fNcolumns << endl;
 }
}
///////////////////////////////////////////////////////////////////////////
void AliCalorimeter::SetPosition(Int_t row,Int_t col,Ali3Vector& r)
{
// Set the position for a certain calorimeter module
 
 if (!fMatrix) LoadMatrix(); // Restore matrix data in case of reading input
 
 if (row>0 && row<=fNrows && col>0 && col<=fNcolumns)
 {
  if (!fPositions[row-1][col-1]) fPositions[row-1][col-1]=new AliPosition;
  (fPositions[row-1][col-1])->SetPosition(r);
  if (fMatrix[row-1][col-1]) (fMatrix[row-1][col-1])->SetPosition(r);
 }
 else
 {
  cout << " *AliCalorimeter::SetPosition* row,col : " << row << "," << col
       << " out of range." << endl;
  cout << " Nrows,Ncols = " << fNrows << "," << fNcolumns << endl;
 }
}
///////////////////////////////////////////////////////////////////////////
Int_t AliCalorimeter::GetEdgeValue(Int_t row,Int_t col)
{
// Provide the value of the edge flag of a certain module
 
 if (row>0 && row<=fNrows && col>0 && col<=fNcolumns)
 {
  Float_t word=(*fAttributes)(row-1,col-1);
  Int_t iword=int(word+0.1);
  Int_t edge=iword/10;
  return edge;
 }
 else
 {
  cout << " *AliCalorimeter::GetEdgeValue* row,col : " << row << "," << col
       << " out of range." << endl;
  cout << " Nrows,Ncols = " << fNrows << "," << fNcolumns << endl;
  return 0;
 }
}
///////////////////////////////////////////////////////////////////////////
Int_t AliCalorimeter::GetDeadValue(Int_t row,Int_t col)
{
// Provide the value of the dead flag of a certain module
 
 if (row>0 && row<=fNrows && col>0 && col<=fNcolumns)
 {
  Float_t word=(*fAttributes)(row-1,col-1);
  Int_t iword=int(word+0.1);
  Int_t dead=iword%10;
  return dead;
 }
 else
 {
  cout << " *AliCalorimeter::GetDeadValue* row,col : " << row << "," << col
       << " out of range." << endl;
  cout << " Nrows,Ncols = " << fNrows << "," << fNcolumns << endl;
  return 0;
 }
}
///////////////////////////////////////////////////////////////////////////
Float_t AliCalorimeter::GetGain(Int_t row,Int_t col)
{
// Provide the gain value of a certain module
 
 if (row>0 && row<=fNrows && col>0 && col<=fNcolumns)
 {
   return (*fGains)(row-1,col-1);
 }
 else
 {
  cout << " *AliCalorimeter::GetGain* row,col : " << row << "," << col
       << " out of range." << endl;
  cout << " Nrows,Ncols = " << fNrows << "," << fNcolumns << endl;
  return 0;
 }
}
///////////////////////////////////////////////////////////////////////////
void AliCalorimeter::GetPosition(Int_t row,Int_t col,Float_t* vec,TString f)
{
// Return the position in user coordinates for a certain calorimeter module
 
 if (!fMatrix) LoadMatrix(); // Restore matrix data in case of reading input
 
 if (row>0 && row<=fNrows && col>0 && col<=fNcolumns)
 {
//  if (fMatrix[row-1][col-1]) (fMatrix[row-1][col-1])->GetPosition(vec,f);
  if (fPositions[row-1][col-1]) (fPositions[row-1][col-1])->GetVector(vec,f);
 }
 else
 {
  cout << " *AliCalorimeter::GetPosition* row,col : " << row << "," << col
       << " out of range." << endl;
  cout << " Nrows,Ncols = " << fNrows << "," << fNcolumns << endl;
 }
}
///////////////////////////////////////////////////////////////////////////
AliPosition* AliCalorimeter::GetPosition(Int_t row,Int_t col)
{
// Access to the position of a certain calorimeter module
 
 if (!fMatrix) LoadMatrix(); // Restore matrix data in case of reading input
 
 if (row>0 && row<=fNrows && col>0 && col<=fNcolumns)
 {
  return fPositions[row-1][col-1];
 }
 else
 {
  cout << " *AliCalorimeter::GetPosition* row,col : " << row << "," << col
       << " out of range." << endl;
  cout << " Nrows,Ncols = " << fNrows << "," << fNcolumns << endl;
  return 0;
 }
}
///////////////////////////////////////////////////////////////////////////
Float_t AliCalorimeter::GetClusteredSignal(Int_t row,Int_t col)
{
// Provide the module signal after clustering
 
 if (!fMatrix) LoadMatrix(); // Restore matrix data in case of reading input
 
 if (row>0 && row<=fNrows && col>0 && col<=fNcolumns)
 {
  if (fMatrix[row-1][col-1])
  {
   return (fMatrix[row-1][col-1])->GetClusteredSignal();
  }
  else
  {
   return 0;
  }
 }
 else
 {
  cout << " *AliCalorimeter::GetClusteredSignal* row,col : " << row << "," << col
       << " out of range." << endl;
  cout << " Nrows,Ncols = " << fNrows << "," << fNcolumns << endl;
  return 0;
 }
}
///////////////////////////////////////////////////////////////////////////
Int_t AliCalorimeter::GetNsignals()
{
// Provide the number of modules that contain a signal
// Note : The number of modules marked 'dead' but which had a signal
//        are included.
 return fNsignals;
}
///////////////////////////////////////////////////////////////////////////
void AliCalorimeter::Group(Int_t n)
{
// Group the individual modules into clusters
// Module signals of n rings around the central module will be grouped
 
 if (fNsignals > 0) // Directly return if no modules fired
 {
  if (!fMatrix) LoadMatrix(); // Restore matrix data in case of reading input
 
  if (fNclusters > 0) Ungroup(); // Restore unclustered situation if needed
 
  // Order the modules with decreasing signal
  AliCalmodule** ordered=new AliCalmodule*[fNsignals]; // temp. array for ordered modules
  Int_t nord=0;
  Sortm(ordered,nord);
 
  // Clustering of modules. Start with the highest signal.
  if (fClusters)
  {
   delete fClusters;
   fClusters=0;
  }
  fClusters=new TObjArray();
  fClusters->SetOwner();
  fNclusters=0;
  Int_t row=0;
  Int_t col=0;
  AliCalcluster* c=0;
  AliCalmodule* m=0;
  for (Int_t i=0; i<nord; i++)
  {
   row=ordered[i]->GetRow();    // row number of cluster center
   col=ordered[i]->GetColumn(); // column number of cluster center
   if (row>0 && row<=fNrows && col>0 && col<=fNcolumns)
   {
    m=fMatrix[row-1][col-1];
    if (!m) continue;

    // only use modules not yet used in a cluster
    if (m->GetClusteredSignal() > 0.)
    {
     Int_t edge=GetEdgeValue(row,col);
     c=new AliCalcluster;
     if (!edge) c->Start(*m);   // module to start the cluster if not on edge
     if (c->GetNmodules() > 0)  // cluster started successfully (no edge)
     {
      fClusters->Add(c);
      fNclusters++;       // update cluster counter
      AddRing(row,col,n); // add signals of n rings around the center
     }
     else
     {
      if (c) delete c;
      c=0;
     }
    }
   }
  }
 
  // Delete the temp. array
  if (ordered)
  { 
   for (Int_t j=0; j<nord; j++)
   {
    ordered[j]=0;
   }
   delete [] ordered;
  }
 }
}
///////////////////////////////////////////////////////////////////////////
void AliCalorimeter::Sortm(AliCalmodule** ordered,Int_t& nord)
{
// Order the modules with decreasing signal
 
 nord=0;
 for (Int_t i=0; i<fNrows; i++) // loop over all modules of the matrix
 {
  for (Int_t ii=0; ii<fNcolumns; ii++)
  {
   if (GetSignal(i+1,ii+1) <= 0.) continue; // only take alive modules with a signal
 
   if (nord == 0) // store the first module with a signal at the first ordered position
   {
    nord++;
    ordered[nord-1]=fMatrix[i][ii];
    continue;
   }
 
   for (Int_t j=0; j<=nord; j++) // put module in the right ordered position
   {
    if (j == nord) // module has smallest signal seen so far
    {
     nord++;
     ordered[j]=fMatrix[i][ii]; // add module at the end
     break; // go for next matrix module
    }
 
    if (GetSignal(i+1,ii+1) < ordered[j]->GetSignal()) continue;
 
    nord++;
    for (Int_t k=nord-1; k>j; k--) // create empty position
    {
     ordered[k]=ordered[k-1];
    }
    ordered[j]=fMatrix[i][ii]; // put module at empty position
    break; // go for next matrix module
   }
  }
 }
}
///////////////////////////////////////////////////////////////////////////
void AliCalorimeter::AddRing(Int_t row, Int_t col, Int_t n)
{
// Add module signals of 1 ring around (row,col) to current cluster
// n denotes the maximum number of rings around cluster center
// Note : This function is used recursively
 
 if (n >= 1) // Check if any rings left for recursive calls
 {
  Float_t signal=GetSignal(row,col); // signal of (row,col) module
 
  Int_t lrow=row-1; if (lrow < 1) lrow=1;                 // row lowerbound for ring
  Int_t urow=row+1; if (urow > fNrows) urow=fNrows;       // row upperbound for ring
  Int_t lcol=col-1; if (lcol < 1) lcol=1;                 // col lowerbound for ring
  Int_t ucol=col+1; if (ucol > fNcolumns) ucol=fNcolumns; // row upperbound for ring
 
  for (Int_t i=lrow; i<=urow; i++)
  {
   for (Int_t j=lcol; j<=ucol; j++)
   {
    // add module(i,j) to cluster if the signal <= signal(row,col)
    if (GetSignal(i,j) <= signal)
    {
     AliCalmodule* m=fMatrix[i-1][j-1];
     if (m) ((AliCalcluster*)fClusters->At(fNclusters-1))->Add(*m);
    }
    AddRing(i,j,n-1); // Go for ring of modules around this (i,j) one
   }
  }
 }
}
///////////////////////////////////////////////////////////////////////////
Int_t AliCalorimeter::GetNclusters()
{
// Provide the number of clusters
 return fNclusters;
}
///////////////////////////////////////////////////////////////////////////
AliCalcluster* AliCalorimeter::GetCluster(Int_t j)
{
// Provide cluster number j
// Note : j=1 denotes the first cluster
 if ((j >= 1) && (j <= fNclusters))
 {
  return (AliCalcluster*)fClusters->At(j-1);
 }
 else
 {
  cout << " *AliCalorimeter::GetCluster* cluster number : " << j
       << " out of range." << endl;
  cout << " -- Cluster number 1 (if any) returned " << endl;
  return (AliCalcluster*)fClusters->At(0);
 }
}
///////////////////////////////////////////////////////////////////////////
AliCalmodule* AliCalorimeter::GetModule(Int_t j)
{
// Provide 'fired' module number j
// Note : j=1 denotes the first 'fired' module
 if ((j >= 1) && (j <= fNsignals))
 {
  return (AliCalmodule*)fModules->At(j-1);
 }
 else
 {
  cout << " *AliCalorimeter::GetModule* module number : " << j
       << " out of range." << endl;
  cout << " -- Fired module number 1 (if any) returned " << endl;
  return (AliCalmodule*)fModules->At(0);
 }
}
///////////////////////////////////////////////////////////////////////////
AliCalmodule* AliCalorimeter::GetModule(Int_t row,Int_t col)
{
// Provide access to module (row,col).
// Note : first module is at (1,1).

 if (!fMatrix) LoadMatrix(); // Restore matrix data in case of reading input

 if (row>=1 && row<=fNrows && col>=1 && col<=fNcolumns)
 {
  return fMatrix[row-1][col-1];
 }
 else
 {
  cout << " *AliCalorimeter::GetModule* row,col : " << row << ", " << col
       << " out of range." << endl;
  return 0;
 }
}
///////////////////////////////////////////////////////////////////////////
TH2F* AliCalorimeter::DrawModules()
{
// Provide a lego plot of the module signals
 
 if (fHmodules)
 {
  fHmodules->Reset();
 }
 else
 {
  fHmodules=new TH2F("fHmodules","Module signals",
            fNcolumns,0.5,float(fNcolumns)+0.5,fNrows,0.5,float(fNrows)+0.5);
 
  fHmodules->SetDirectory(0); // Suppress global character of histo pointer
 }
 
 AliCalmodule* m;
 Float_t row,col,signal;
 Int_t dead;
 for (Int_t i=0; i<fNsignals; i++)
 {
  m=(AliCalmodule*)fModules->At(i);
  if (m)
  {
   row=float(m->GetRow());
   col=float(m->GetColumn());
   dead=m->GetDeadValue();
   signal=0;
   if (!dead) signal=m->GetSignal();
   if (signal>0.) fHmodules->Fill(col,row,signal);
  }
 }
 
 fHmodules->Draw("lego");
 return fHmodules;
}
///////////////////////////////////////////////////////////////////////////
TH2F* AliCalorimeter::DrawClusters()
{
// Provide a lego plot of the cluster signals
 
 if (fHclusters)
 {
  fHclusters->Reset();
 }
 else
 {
  fHclusters=new TH2F("fHclusters","Cluster signals",
            fNcolumns,0.5,float(fNcolumns)+0.5,fNrows,0.5,float(fNrows)+0.5);
 
  fHclusters->SetDirectory(0); // Suppress global character of histo pointer
 }
 
 AliCalcluster* c;
 Float_t row,col,signal;
 for (Int_t i=0; i<fNclusters; i++)
 {
  c=(AliCalcluster*)fClusters->At(i);
  if (c)
  {
   row=float(c->GetRow());
   col=float(c->GetColumn());
   signal=c->GetSignal();
   if (signal>0.) fHclusters->Fill(col,row,signal);
  }
 }
 
 fHclusters->Draw("lego");
 return fHclusters;
}
///////////////////////////////////////////////////////////////////////////
void AliCalorimeter::LoadMatrix()
{
// Load the Calorimeter module matrix data back from the TObjArray

 // Initialise the module matrix
 if (!fMatrix)
 {
  fMatrix=new AliCalmodule**[fNrows];
  for (Int_t i=0; i<fNrows; i++)
  {
   fMatrix[i]=new AliCalmodule*[fNcolumns];
  }
 }

 // Initialise the position matrix
 if (!fPositions)
 {
  fPositions=new AliPosition**[fNrows];
  for (Int_t j=0; j<fNrows; j++)
  {
   fPositions[j]=new AliPosition*[fNcolumns];
  }
 }

 for (Int_t jrow=0; jrow<fNrows; jrow++)
 {
  for (Int_t jcol=0; jcol<fNcolumns; jcol++)
  {
   fMatrix[jrow][jcol]=0;
   fPositions[jrow][jcol]=0;
  }
 }
 
 // Copy the module pointers back into the matrix
 AliCalmodule* m=0;
 Int_t row=0;
 Int_t col=0;
 Int_t nsig=0;
 if (fModules) nsig=fModules->GetEntries();
 for (Int_t j=0; j<nsig; j++)
 {
  m=(AliCalmodule*)fModules->At(j);
  if (m)
  {
   row=m->GetRow();
   col=m->GetColumn();
   AliPosition r=m->GetPosition();
   fMatrix[row-1][col-1]=m;
   fPositions[row-1][col-1]=new AliPosition(r);
  }
 }
}
///////////////////////////////////////////////////////////////////////////
void AliCalorimeter::Ungroup()
{
// Set the module signals back to the non-clustered situation
 
 if (!fMatrix) LoadMatrix(); // Restore matrix data in case of reading input
 
 Int_t nsig=0;
 if (fModules) nsig=fModules->GetEntries();

 Float_t signal=0;
 AliCalmodule* m=0;
 for (Int_t j=0; j<nsig; j++)
 {
  m=(AliCalmodule*)fModules->At(j);
  if (m)
  {
   signal=m->GetSignal();
   m->SetClusteredSignal(signal);
  }
 }
}
///////////////////////////////////////////////////////////////////////////
void AliCalorimeter::AddVetoSignal(AliSignal& s)
{
// Associate an (extrapolated) AliSignal as veto to the calorimeter.
 if (!fVetos)
 {
  fNvetos=0;
  fVetos=new TObjArray();
  fVetos->SetOwner();
 } 

 Int_t nvalues=s.GetNvalues();
 AliSignal* sx=new AliSignal(nvalues);
 sx->SetName(s.GetName());
 
 sx->SetPosition((Ali3Vector&)s);

 Double_t sig,err;
 for (Int_t i=1; i<=nvalues; i++)
 {
  sig=s.GetSignal(i);
  err=s.GetSignalError(i);
  sx->SetSignal(sig,i);
  sx->SetSignalError(err,i);
 } 

 fVetos->Add(sx);
 fNvetos++;
}
///////////////////////////////////////////////////////////////////////////
Int_t AliCalorimeter::GetNvetos()
{
// Provide the number of veto signals associated to the calorimeter
 return fNvetos;
}
///////////////////////////////////////////////////////////////////////////
AliSignal* AliCalorimeter::GetVetoSignal(Int_t i)
{
// Provide access to the i-th veto signal of this calorimeter
// Note : The first hit corresponds to i=1

 if (i>0 && i<=fNvetos)
 {
  return (AliSignal*)fVetos->At(i-1);
 }
 else
 {
  cout << " *AliCalorimeter::GetVetoSignal* Signal number " << i
       << " out of range." << endl;
  cout << " --- First signal (if any) returned." << endl;
  return (AliSignal*)fVetos->At(0);
 }
}
///////////////////////////////////////////////////////////////////////////
void AliCalorimeter::SetName(TString name)
{
// Set the name of the calorimeter system.
 fName=name;
}
///////////////////////////////////////////////////////////////////////////
TString AliCalorimeter::GetName()
{
// Provide the name of the calorimeter system.
 return fName;
}
///////////////////////////////////////////////////////////////////////////

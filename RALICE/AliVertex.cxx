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
// Class AliVertex
// Creation and investigation of an AliVertex.
// An AliVertex can be constructed by adding AliTracks and/or AliJets.
//
// Note : Also (secondary) vertices can be added to a vertex.
//
// To provide maximal flexibility to the user, two modes of vertex storage
// are provided by means of the memberfunction SetVertexCopy().
// The same holds for the storage of jets via SetJetCopy().
//
// a) SetVertexCopy(0) (which is the default).
//    Only the pointers of the 'added' vertices are stored.
//    This mode is typically used by making vertex studies based on a fixed list
//    of vertices which stays under user control or is contained for instance
//    in an AliEvent.  
//    In this way the AliVertex just represents a 'logical structure' for the
//    physics analysis which can be embedded in e.g. an AliEvent or AliVertex.
//
//    Note :
//    Modifications made to the original vertices also affect the AliVertex objects
//    which are stored.
// 
// b) SetVertexCopy(1).
//    Of every 'added' vertex a private copy will be made of which the pointer
//    will be stored.
//    In this way the AliVertex represents an entity on its own and modifications
//    made to the original vertices do not affect the AliVertex objects which are
//    stored. 
//    This mode will allow 'adding' many different AliVertex objects by
//    creating only one AliVertex instance in the main programme and using the
//    AliVertex::Reset, AliVertex::AddTrack and parameter setting memberfunctions.
//
// See also the documentation provided for the memberfunction SetOwner(). 
//
// Coding example to make 3 vertices v1, v2 and v3.
// ------------------------------------------------
// v1 contains the tracks 1,2,3 and 4
// v2 contains many different tracks
// v3 contains the jets 1 and 2
//
//        AliTrack t1,t2,t3,t4;
//         ...
//         ... // code to fill the track data
//         ...
//
//        AliJet j1,j2;
//         ...
//         ... // code to fill the jet data
//         ...
//
//        AliVertex v1;
//        v1.SetVertexCopy(1);
//
//        v1.AddTrack(t1);
//        v1.AddTrack(t2);
//        v1.AddTrack(t3);
//        v1.AddTrack(t4);
//
//        Float_t r1[3]={2.4,0.1,-8.5};
//        v1.SetPosition(r1,"car");
//
//        AliVertex v2;
//        v2.SetTrackCopy(1);
//
//        AliTrack* tx=new AliTrack();
//        for (Int_t i=0; i<10; i++)
//        {
//         ...
//         ... // code to fill the track data
//         ...
//         v2.AddTrack(tx);
//         tx->Reset(); 
//        }
//
//        Float_t r2[3]={1.6,-3.2,5.7};
//        v2.SetPosition(r2,"car");
//
//        AliVertex v3;
//
//        v3.AddJet(j1);
//        v3.AddJet(j2);
//
//        Float_t r3[3]={6.2,4.8,1.3};
//        v3.SetPosition(r3,"car");
//
//        v1.Info("sph");
//        v2.ListAll();
//        v3.List("cyl");
//
//        Float_t e1=v1.GetEnergy();
//        Ali3Vector p1=v1.Get3Momentum();
//        Float_t loc[3];
//        v1.GetPosition(loc,"sph");
//        AliPosition r=v2.GetPosition();
//        r.Info(); 
//        Int_t nt=v2.GetNtracks();
//        AliTrack* tv=v2.GetTrack(1); // Access track number 1 of Vertex v2
//
// Specify the vertices v2 and v3 as secondary vertices of v1
//
//        v1.AddVertex(v2);
//        v1.AddVertex(v3);
//
//        v1.List();
//
//        Int_t nv=v1.GetNvtx();
//        AliVertex* vx=v1.GetVertex(1); // Access 1st secondary vertex of v1
//        Float_t e=vx->GetEnergy();
//
//        Float_t M=v1.GetInvmass(); 
//
// Reconstruct Vertex v1 from scratch
//
//        v1.Reset();
//        v1.SetNvmax(25); // Increase initial no. of sec. vertices
//        v1.AddTrack(t3);
//        v1.AddTrack(t4);
//        v1.AddJet(j2);
//        Float_t pos[3]={7,9,4};
//        v1.SetPosition(pos,"car");
//
// Note : All quantities are in GeV, GeV/c or GeV/c**2
//
//--- Author: Nick van Eijndhoven 04-apr-1998 UU-SAP Utrecht
//- Modified: NvE $Date$ UU-SAP Utrecht
///////////////////////////////////////////////////////////////////////////

#include "AliVertex.h"
 
ClassImp(AliVertex) // Class implementation to enable ROOT I/O
 
AliVertex::AliVertex()
{
// Default constructor.
// All variables initialised to 0.
// Initial maximum number of tracks is set to the default value.
// Initial maximum number of sec. vertices is set to the default value.
 Init();
 Reset();
 SetNtinit();
 SetNvmax();
 SetNjmax();
}
///////////////////////////////////////////////////////////////////////////
void AliVertex::Init()
{
// Initialisation of pointers etc...
 AliJet::Init();
 fNvmax=0;
 fVertices=0;
 fConnects=0;
 fVertexCopy=0;
 fNjmax=0;
 fJets=0;
 fJetTracks=0;
 fJetCopy=0;
 fLines=0;
}
///////////////////////////////////////////////////////////////////////////
AliVertex::AliVertex(Int_t n)
{
// Create a vertex to hold initially a maximum of n tracks
// All variables initialised to 0
 Init();
 Reset();
 if (n > 0)
 {
  SetNtinit(n);
 }
 else
 {
  cout << endl;
  cout << " *AliVertex* Initial max. number of tracks entered : " << n << endl;
  cout << " This is invalid. Default initial maximum will be used." << endl;
  cout << endl;
  SetNtinit();
 }
 SetNvmax();
 SetNjmax();
}
///////////////////////////////////////////////////////////////////////////
AliVertex::~AliVertex()
{
// Default destructor
 if (fVertices)
 {
  delete fVertices;
  fVertices=0;
 }
 if (fConnects)
 {
  delete fConnects;
  fConnects=0;
 }
 if (fJets)
 {
  delete fJets;
  fJets=0;
 }
 if (fJetTracks)
 {
  delete fJetTracks;
  fJetTracks=0;
 }
 if (fLines)
 {
  delete fLines;
  fLines=0;
 }
}
///////////////////////////////////////////////////////////////////////////
void AliVertex::SetOwner(Bool_t own)
{
// Set ownership of all added objects. 
// The default parameter is own=kTRUE.
//
// Invokation of this memberfunction also sets all the copy modes
// (e.g. TrackCopy & co.) according to the value of own.
//
// This function (with own=kTRUE) is particularly useful when reading data
// from a tree/file, since Reset() will then actually remove all the
// added objects from memory irrespective of the copy mode settings
// during the tree/file creation process. In this way it provides a nice way
// of preventing possible memory leaks in the reading/analysis process.
//
// In addition this memberfunction can also be used as a shortcut to set all
// copy modes in one go during a tree/file creation process.
// However, in this case the user has to take care to only set/change the
// ownership (and copy mode) for empty objects (e.g. newly created objects
// or after invokation of the Reset() memberfunction) otherwise it will
// very likely result in inconsistent destructor behaviour.

 Int_t mode=1;
 if (!own) mode=0;
 if (fVertices) fVertices->SetOwner(own);
 fVertexCopy=mode;
 if (fJets) fJets->SetOwner(own);
 fJetCopy=mode;

 AliJet::SetOwner(own);
}
///////////////////////////////////////////////////////////////////////////
AliVertex::AliVertex(AliVertex& v)
{
// Copy constructor
 Init();
 Reset();
 SetNtinit();
 SetNvmax();
 SetNjmax();
 SetTrackCopy(v.GetTrackCopy());
 SetVertexCopy(v.GetVertexCopy());
 SetJetCopy(v.GetJetCopy());
 SetId(v.GetId());
 SetPosition(v.GetPosition());

 // Copy all tracks except the ones coming from jets
 AliTrack* tx=0;
 Int_t jetflag=0,connect=0;
 AliTrack* tx2=0;
 for (Int_t it=1; it<=v.GetNtracks(); it++)
 {
  tx=v.GetTrack(it);
  if (tx)
  {
   jetflag=v.IsJetTrack(tx);
   connect=v.IsConnectTrack(tx);

   if (!jetflag && !connect) AddTrack(tx);

   if (connect)
   {
    if (!fConnects)
    {
     fConnects=new TObjArray(fNvmax);
     if (!fTrackCopy) fConnects->SetOwner();
    }
    tx2=new AliTrack(*tx);
    fConnects->Add(tx2);
    AddTrack(tx2,0);
   } 
  }
 }

 // Copy all the (secondary) vertices without re-creating connecting tracks
 // The connecting tracks have already been copied above
 AliVertex* vx=0;
 for (Int_t iv=1; iv<=v.GetNvertices(); iv++)
 {
  vx=v.GetVertex(iv);
  if (vx) AddVertex(vx,0); 
 }

 // Copy all the jets including the jet tracks for these jets for which
 // this was also the case in the original vertex
 AliJet* jx=0;
 for (Int_t ij=1; ij<=v.GetNjets(); ij++)
 {
  jx=v.GetJet(ij);
  if (jx)
  {
   jetflag=0;
   if (jx->GetNtracks())
   {
    tx=jx->GetTrack(1);
    if (tx)
    {
     jetflag=v.IsJetTrack(tx);
    }
   }
   AddJet(jx,jetflag);
  } 
 }
}
///////////////////////////////////////////////////////////////////////////
void AliVertex::SetNvmax(Int_t n)
{
// Set the initial maximum number of (secondary) vertices
 if (n > 0)
 {
  fNvmax=n;
 }
 else
 {
  fNvmax=1;
 }
 if (fVertices)
 {
  delete fVertices;
  fVertices=0;
 }
}
///////////////////////////////////////////////////////////////////////////
void AliVertex::SetNjmax(Int_t n)
{
// Set the initial maximum number of jets
 if (n > 0)
 {
  fNjmax=n;
 }
 else
 {
  fNjmax=1;
 }
 if (fJets)
 {
  delete fJets;
  fJets=0;
 }
}
///////////////////////////////////////////////////////////////////////////
void AliVertex::Reset()
{
// Reset all variables to 0 and reset all stored vertex and jet lists.
// The max. number of tracks is set to the initial value again
// The max. number of vertices is set to the default value again
// The max. number of jets is set to the default value again

 AliJet::Reset();

 Double_t a[3]={0,0,0};
 SetPosition(a,"sph");
 SetPositionErrors(a,"car");

 fNvtx=0;
 if (fNvmax>0) SetNvmax(fNvmax);
 if (fConnects)
 {
  delete fConnects;
  fConnects=0;
 }

 fNjets=0;
 if (fNjmax>0) SetNjmax(fNjmax);
 if (fJetTracks)
 {
  delete fJetTracks;
  fJetTracks=0;
 }
 
 if (fLines)
 {
  delete fLines;
  fLines=0;
 }
}
///////////////////////////////////////////////////////////////////////////
void AliVertex::ResetVertices()
{
// Reset the stored vertex list and delete all connecting tracks which
// were generated automatically via connect=1 in AddVertex().
// The max. number of vertices is set to the default value again.
// All physics quantities are updated according to the removal of the
// connecting tracks.
 AliTrack* t;
 if (fConnects)
 {
  for (Int_t i=0; i<=fConnects->GetLast(); i++)
  {
   t=(AliTrack*)fConnects->At(i);
   AliTrack* test=(AliTrack*)fTracks->Remove(t);
   if (test)
   {
    fNtrk--;
    (Ali4Vector&)(*this)-=(Ali4Vector&)(*t);
    fQ-=t->GetCharge();
    if (fTrackCopy) delete t;
   }
  }
  fTracks->Compress();
 }

 fNvtx=0;
 if (fNvmax>0) SetNvmax(fNvmax);
 if (fConnects)
 {
  delete fConnects;
  fConnects=0;
 }
}
///////////////////////////////////////////////////////////////////////////
void AliVertex::AddJet(AliJet& j,Int_t tracks)
{
// Add a jet (and its tracks) to the vertex
// In case the maximum number of jets has been reached,
// the array space will be extended automatically
//
// Note : By default the tracks of the jet are added to the current (primary)
//        vertex.
//        The automatic addition of the tracks of the jet can be suppressed
//        by specifying tracks=0. In this case only the AliJet object will
//        be stored according to the mode specified by SetJetCopy().
//        The latter will enable jet studies based on a fixed list of tracks
//        as contained e.g. in an AliVertex or AliEvent. 
 if (!fJets)
 {
  fJets=new TObjArray(fNjmax);
  if (fJetCopy) fJets->SetOwner();
 }
 if (fNjets == fNjmax) // Check if maximum jet number is reached
 {
  fNjmax++;
  fJets->Expand(fNjmax);
 }

 // Add the jet to the list 
 AliJet* jx=&j;
 if (fJetCopy) jx=new AliJet(j);

 if (jx)
 {
  fNjets++;
  fJets->Add(jx); 
 }

 // Add the tracks of the jet to this vertex
 if (tracks)
 {
  if (!fJetTracks)
  {
   fJetTracks=new TObjArray();
  }
  Int_t copy=1-(jx->GetTrackCopy());
  AliTrack* tj;
  for (Int_t i=1; i<=jx->GetNtracks(); i++)
  {
   tj=jx->GetTrack(i);
   if (tj)
   {
    AddTrack(tj,copy);
    fJetTracks->Add(tj);
   }
  }
 }
}
///////////////////////////////////////////////////////////////////////////
void AliVertex::AddVertex(AliVertex& v,Int_t connect)
{
// Add a (secondary) vertex to the current vertex.
// In case the maximum number of (secondary) vertices has been reached,
// the array space will be extended automatically
//
// Note : By default the 4-momentum and charge of the current (primary) vertex
//        are updated by automatically creating the track connecting
//        both vertices. The track parameters are taken from the
//        4-momentum and charge of the secondary vertex.
//        The automatic creation of the connecting track and updating
//        of the (primary) vertex 4-momentum and charge can be suppressed
//        by specifying connect=0. In this case, however, the user
//        has to introduce the connecting track lateron by hand
//        explicitly in order to match the kinematics and charge.
//
 if (!fVertices)
 {
  fVertices=new TObjArray(fNvmax);
  if (fVertexCopy) fVertices->SetOwner();
 }
 if (fNvtx == fNvmax) // Check if maximum vertex number is reached
 {
  fNvmax++;
  fVertices->Expand(fNvmax);
 }

 // Add the linked (secondary) vertex to the list 
 AliVertex* vx=&v;
 if (fVertexCopy) vx=new AliVertex(v);

 if (vx)
 {
  fNvtx++;
  fVertices->Add(vx);
 } 

 // Create connecting track and update 4-momentum and charge for current vertex
 if (connect)
 {
  AliTrack* t=new AliTrack();
  t->SetBeginPoint(GetPosition());
  t->SetEndPoint(v.GetPosition());
  t->SetCharge(v.GetCharge());
  t->Set4Momentum((Ali4Vector&)v);

  AddTrack(t,0);

  if (!fConnects)
  {
   fConnects=new TObjArray(fNvmax);
   if (!fTrackCopy) fConnects->SetOwner();
  }
  fConnects->Add(t);
 }
}
///////////////////////////////////////////////////////////////////////////
void AliVertex::Info(TString f)
{
// Provide vertex information within the coordinate frame f
 cout << " *AliVertex::Info* Id : " << fUserId << " Invmass : " << GetInvmass()
      << " Charge : " << GetCharge() << " Momentum : " << GetMomentum()
      << " Ntracks : " << GetNtracks() << " Nvertices : " << fNvtx 
      << " Njets : " << fNjets << endl;
 cout << " ";
 Ali4Vector::Info(f);
 cout << "  Position";
 AliPosition::Info(f); 
} 
///////////////////////////////////////////////////////////////////////////
void AliVertex::List(TString f)
{
// Provide primary track and sec. vertex information within the coordinate frame f

 Info(f); // Information of the current vertex

 // The tracks of this vertex
 AliTrack* t; 
 for (Int_t it=1; it<=GetNtracks(); it++)
 {
  t=GetTrack(it);
  if (t)
  {
   cout << "  ---Track no. " << it << endl;
   cout << " ";
   t->Info(f); 
  }
  else
  {
   cout << " *AliVertex::List* Error : No track present." << endl; 
  }
 }

 // The secondary vertices of this vertex
 AliVertex* v; 
 for (Int_t iv=1; iv<=GetNvertices(); iv++)
 {
  v=GetVertex(iv);
  if (v)
  {
   cout << "  ---Level 1 sec. vertex no. " << iv << endl;
   cout << " ";
   v->Info(f); 
  }
  else
  {
   cout << " *AliVertex::List* Error : No sec. vertex present." << endl; 
  }
 }
} 
///////////////////////////////////////////////////////////////////////////
void AliVertex::ListAll(TString f)
{
// Provide complete (sec) vertex and (decay) track info within the coordinate frame f

 Info(f); // Information of the current vertex

 // The tracks of this vertex
 AliTrack* t; 
 for (Int_t it=1; it<=GetNtracks(); it++)
 {
  t=GetTrack(it);
  if (t)
  {
   cout << "  ---Track no. " << it << endl;
   cout << " ";
   t->ListAll(f); 
  }
  else
  {
   cout << " *AliVertex::ListAll* Error : No track present." << endl; 
  }
 }

 AliVertex* v=this;
 Dump(v,1,f); // Information of all sec. vertices
}
//////////////////////////////////////////////////////////////////////////
void AliVertex::Dump(AliVertex* v,Int_t n,TString f)
{
// Recursively provide the info of all secondary vertices of this vertex
 AliVertex* vs; 
 for (Int_t iv=1; iv<=v->GetNvertices(); iv++)
 {
  vs=v->GetVertex(iv);
  if (vs)
  {
   cout << "  ---Level " << n << " sec. vertex no. " << iv << endl;
   cout << " ";
   vs->Info(f); 

   // The tracks of this vertex
   AliTrack* t; 
   for (Int_t it=1; it<=vs->GetNtracks(); it++)
   {
    t=vs->GetTrack(it);
    if (t)
    {
     cout << "  ---Track no. " << it << endl;
     cout << " ";
     t->ListAll(f); 
    }
    else
    {
     cout << " *AliVertex::Dump* Error : No track present." << endl; 
    }
   }

   // Go for next sec. vertex level of this sec. vertex recursively
   Dump(vs,n+1,f);
  }
  else
  {
   cout << " *AliVertex::Dump* Error : No sec. vertex present." << endl; 
  }
 }
} 
//////////////////////////////////////////////////////////////////////////
Int_t AliVertex::GetNvertices()
{
// Return the current number of (secondary) vertices
 return fNvtx;
}
///////////////////////////////////////////////////////////////////////////
AliVertex* AliVertex::GetVertex(Int_t i)
{
// Return the i-th (secondary) vertex of the current vertex
 if (!fVertices)
 {
  cout << " *AliVertex*::GetVertex* No (secondary) vertices present." << endl;
  return 0;
 }
 else
 {
  if (i<=0 || i>fNvtx)
  {
   cout << " *AliVertex*::GetVertex* Invalid argument i : " << i
        << " Nvtx = " << fNvtx << endl;
   return 0;
  }
  else
  {
   return (AliVertex*)fVertices->At(i-1);
  }
 }
}
///////////////////////////////////////////////////////////////////////////
AliVertex* AliVertex::GetIdVertex(Int_t id)
{
// Return the (sec.) vertex with user identifier "id"
 AliVertex* vx=0;
 AliVertex* v=0;
 if (!fVertices)
 {
  cout << " *AliVertex*::GetIdVertex* No (secondary) vertices present." << endl;
  return 0;
 }
 else
 {
  for (Int_t i=0; i<fNvtx; i++)
  {
   vx=(AliVertex*)fVertices->At(i);
   if (id == vx->GetId()) v=vx;
  }
  return v;
 }
}
///////////////////////////////////////////////////////////////////////////
void AliVertex::SetVertexCopy(Int_t j)
{
// (De)activate the creation of private copies of the added vertices.
// j=0 ==> No private copies are made; pointers of original vertices are stored.
// j=1 ==> Private copies of the vertices are made and these pointers are stored.
//
// Note : Once the storage contains pointer(s) to AliVertex objects one cannot
//        change the VertexCopy mode anymore.
//        To change the VertexCopy mode for an existing AliVertex containing
//        vertices one first has to invoke Reset().
 if (!fVertices)
 {
  if (j==0 || j==1)
  {
   fVertexCopy=j;
  }
  else
  {
   cout << "*AliVertex::SetVertexCopy* Invalid argument : " << j << endl;
  }
 }
 else
 {
  cout << "*AliVertex::SetVertexCopy* Storage already contained vertices."
       << "  ==> VertexCopy mode not changed." << endl; 
 }
}
///////////////////////////////////////////////////////////////////////////
Int_t AliVertex::GetVertexCopy()
{
// Provide value of the VertexCopy mode.
// 0 ==> No private copies are made; pointers of original vertices are stored.
// 1 ==> Private copies of the vertices are made and these pointers are stored.
 return fVertexCopy;
}
///////////////////////////////////////////////////////////////////////////
Int_t AliVertex::GetNjets()
{
// Return the current number of jets
 return fNjets;
}
///////////////////////////////////////////////////////////////////////////
AliJet* AliVertex::GetJet(Int_t i)
{
// Return the i-th jet of the current vertex
 if (!fJets)
 {
  cout << " *AliVertex*::GetJet* No jets present." << endl;
  return 0;
 }
 else
 {
  if (i<=0 || i>fNjets)
  {
   cout << " *AliVertex*::GetJet* Invalid argument i : " << i
        << " Njets = " << fNjets << endl;
   return 0;
  }
  else
  {
   return (AliJet*)fJets->At(i-1);
  }
 }
}
///////////////////////////////////////////////////////////////////////////
AliJet* AliVertex::GetIdJet(Int_t id)
{
// Return the jet with user identifier "id"
 AliJet* jx=0;
 AliJet* j=0;
 if (!fJets)
 {
  cout << " *AliVertex*::GetIdJet* No jets present." << endl;
  return 0;
 }
 else
 {
  for (Int_t i=0; i<fNjets; i++)
  {
   jx=(AliJet*)fJets->At(i);
   if (id == jx->GetId()) j=jx;
  }
  return j;
 }
}
///////////////////////////////////////////////////////////////////////////
void AliVertex::SetJetCopy(Int_t j)
{
// (De)activate the creation of private copies of the added jets.
// j=0 ==> No private copies are made; pointers of original jets are stored.
// j=1 ==> Private copies of the jets are made and these pointers are stored.
//
// Note : Once the storage contains pointer(s) to AliJet objects one cannot
//        change the JetCopy mode anymore.
//        To change the JetCopy mode for an existing AliVertex containing
//        jets one first has to invoke Reset().
 if (!fJets)
 {
  if (j==0 || j==1)
  {
   fJetCopy=j;
  }
  else
  {
   cout << "*AliVertex::SetJetCopy* Invalid argument : " << j << endl;
  }
 }
 else
 {
  cout << "*AliVertex::SetJetCopy* Storage already contained jets."
       << "  ==> JetCopy mode not changed." << endl; 
 }
}
///////////////////////////////////////////////////////////////////////////
Int_t AliVertex::GetJetCopy()
{
// Provide value of the JetCopy mode.
// 0 ==> No private copies are made; pointers of original jets are stored.
// 1 ==> Private copies of the jets are made and these pointers are stored.
 return fJetCopy;
}
///////////////////////////////////////////////////////////////////////////
Int_t AliVertex::IsConnectTrack(AliTrack* t)
{
// Indicate whether a track from the tracklist was created via the
// connection of a (secondary) vertex or not.
// In case the track was the result of (secondary) vertex addition the
// return value is 1, otherwise the value 0 will be returned.
 Int_t connect=0;
 if (fConnects)
 {
  if (fConnects->FindObject(t)) connect=1;
 }
 return connect;
}
///////////////////////////////////////////////////////////////////////////
Int_t AliVertex::IsJetTrack(AliTrack* t)
{
// Indicate whether a track from the tracklist was created via the
// addition of a jet or not.
// In case the track was the result of jet addition the return value is 1,
// otherwise the value 0 will be returned.
 Int_t jetflag=0;
 if (fJetTracks)
 {
  if (fJetTracks->FindObject(t)) jetflag=1;
 }
 return jetflag;
}
///////////////////////////////////////////////////////////////////////////
void AliVertex::Draw(Int_t secs,Int_t cons,Int_t jets)
{
// 3-Dimensional visualisation of an AliVertex with its attributes.
// The displayed tracklength is proportional to the momentum of the track.
//
// Color conventions :
// -------------------
// positive track : red
// neutral  track : green
// negative track : blue
// jet-track      : magenta (if explicit marking selected)
//
// secs = 1 --> Draw secondary vertices.
//        0 --> Don't draw secondary vertices.
//
// cons = 1 --> Draw (auto generated) connecting tracks. 
//        0 --> Don't draw (auto generated) connecting tracks.
//                  
// jets = 1 --> Mark tracks belonging to jets.
//        0 --> Don't mark jet-tracks.
//
// Notes :
// -------
// Auto generated connecting tracks will be drawn as thin lines.
// Tracks belonging to jets will be marked as somewhat thinner magenta lines.
// This memberfunction is used recursively.
//
 Double_t vec[3]={0,0,0};
 AliTrack* tx=0;
 AliVertex* vx=0;
 AliPosition r;
 Ali3Vector p;
 Int_t charge;

 if (fLines) delete fLines;
 fLines=new TObjArray();
 fLines->SetOwner();

 Int_t ntk=GetNtracks();
 for (Int_t jtk=1; jtk<=ntk; jtk++)
 {
  tx=GetTrack(jtk);

  if (!tx) continue;

  charge=static_cast<int>(tx->GetCharge());

  TPolyLine3D* line=new TPolyLine3D();
  fLines->Add(line);

  if (IsConnectTrack(tx))
  {
   if (cons==1)
   {
    r=tx->GetBeginPoint();
    r.GetPosition(vec,"car");
    line->SetNextPoint(vec[0],vec[1],vec[2]);
    r=tx->GetEndPoint();
    r.GetPosition(vec,"car");
    line->SetNextPoint(vec[0],vec[1],vec[2]);
    line->SetLineWidth(1);
   }
  }
  else
  {
   r=tx->GetClosestPoint();
   r.GetPosition(vec,"car");
   line->SetNextPoint(vec[0],vec[1],vec[2]);
   p=tx->Get3Momentum();
   p=p+r;
   p.GetVector(vec,"car");
   line->SetNextPoint(vec[0],vec[1],vec[2]);
   line->SetLineWidth(3);
  }

  if (charge>0) line->SetLineColor(kRed);   // Positive track
  if (!charge)  line->SetLineColor(kGreen); // Neutral track
  if (charge<0) line->SetLineColor(kBlue);  // Negative track
 
  // Mark tracks belonging to jets
  if (IsJetTrack(tx))
  {
   if (jets==1)
   {
    line->SetLineWidth(2);
    line->SetLineColor(kMagenta);
   }
  }
    
  line->Draw();
 }

 // Go for secondary vertices if selected
 if (secs==1)
 {
  Int_t nvtx=GetNvertices();
  for (Int_t jvtx=1; jvtx<=nvtx; jvtx++)
  {
   vx=GetVertex(jvtx);
   if (vx) vx->Draw(secs,cons,jets);
  }
 }
}
///////////////////////////////////////////////////////////////////////////

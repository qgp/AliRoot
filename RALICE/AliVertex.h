#ifndef ALIVERTEX_H
#define ALIVERTEX_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

// $Id$

#include <iostream.h>
#include <math.h>
 
#include "TObject.h"
#include "TObjArray.h"
#include "TPolyLine3D.h"
 
#include "AliJet.h"
#include "AliPosition.h"
 
class AliVertex : public AliJet,public AliPosition
{
 public:
  AliVertex();                            // Default constructor
  AliVertex(Int_t n);                     // Create a vertex to hold initially n tracks
  ~AliVertex();                           // Default destructor
  virtual void SetOwner(Bool_t own=kTRUE);// Set ownership of all added objects
  AliVertex(AliVertex& v);                // Copy constructor
  void Reset();                           // Reset all values and stored vertex and jet lists
  void ResetVertices();                   // Reset stored vertex list
  void AddJet(AliJet& j,Int_t tracks=1);  // Add a jet (and its tracks) to the vertex
  void AddVertex(AliVertex& v,Int_t connect=1);// Add (and connect) a (sec.) vertex to the current vertex
  void AddJet(AliJet* j,Int_t tracks=1)    { AddJet(*j,tracks); }
  void AddVertex(AliVertex* v,Int_t connect=1) { AddVertex(*v,connect); }
  void Info(TString f="car");             // Print the vertex info within coordinate frame f
  void List(TString f="car");             // Print vertex prim. track information for coord. frame f
  void ListAll(TString f="car");          // Print prim. + sec. vertex full track info for coord. frame f
  Int_t GetNvertices();                   // Return the number of (secondary) vertices
  AliVertex* GetVertex(Int_t i);          // Provide i-th (secondary) vertex
  AliVertex* GetIdVertex(Int_t id);       // Provide the vertex with user identifier "id"
  void SetNvmax(Int_t n=2);               // Set the initial max. number of (secondary) vertices
  void SetVertexCopy(Int_t j);            // (De)activate creation of private copies in fVertices
  Int_t GetVertexCopy();                  // Provide VertexCopy flag value      
  Int_t GetNjets();                       // Return the number of jets
  AliJet* GetJet(Int_t i);                // Provide i-th jet
  AliJet* GetIdJet(Int_t id);             // Provide the jet with user identifier "id"
  void SetNjmax(Int_t n=2);               // Set the initial max. number of jets
  void SetJetCopy(Int_t j);               // (De)activate creation of private copies in fJets
  Int_t GetJetCopy();                     // Provide JetCopy flag value      
  Int_t IsConnectTrack(AliTrack* t);      // Indicate if track is created by vertex connection
  Int_t IsJetTrack(AliTrack* t);          // Indicate if track is resulting from jet addition
  void Draw(Int_t secs=1,Int_t cons=1,Int_t jets=0); // Draw the vertex in an event display

 protected:
  void Init();          // Initialisation of pointers etc... 
  Int_t fNvmax;         // The maximum number of (secondary) vertices
  Int_t fNvtx;          // The number of (secondary) vertices
  TObjArray* fVertices; // Array to hold the pointers to the (secondary) vertices
  TObjArray* fConnects; // Array to hold the pointers to the auto-generated connecting tracks
  Int_t fVertexCopy;    // Flag to denote creation of private copies in fVertices
  Int_t fNjmax;         // The maximum number of jets
  Int_t fNjets;         // The number of jets
  TObjArray* fJets;     // Array to hold the pointers to the jets
  TObjArray* fJetTracks;// Array to hold the pointers to tracks introduced by jet addition
  Int_t fJetCopy;       // Flag to denote creation of private copies in fJets
  TObjArray* fLines;    //! Array to (temporarily) store the 3D lines for the event display 

 private:
  void Dump(AliVertex* v,Int_t n,TString f); // Recursively print all sec. vertices
 
 ClassDef(AliVertex,3) // Creation and investigation of an AliVertex.
};
#endif

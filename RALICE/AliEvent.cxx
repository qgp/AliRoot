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

// $Id: AliEvent.cxx,v 1.17 2003/12/18 09:28:06 nick Exp $

///////////////////////////////////////////////////////////////////////////
// Class AliEvent
// Creation and investigation of an Alice physics event.
// An AliEvent can be constructed by adding AliTracks, Alivertices, AliJets
// and/or devices like AliCalorimeters.
// All objects which are derived from TObject can be regarded as a device.
//
// The basic functionality of AliEvent is identical to the one of AliVertex.
// So, an AliEvent may be used as the primary vertex with some additional
// functionality compared to AliVertex.
//
// To provide maximal flexibility to the user, the two modes of track/jet/vertex
// storage as described in AliJet and AliVertex can be used.
// In addition an identical structure is provided for the storage of devices like
// AliCalorimeter objects, which can be selected by means of the memberfunction
// SetDevCopy().
//
// a) SetDevCopy(0) (which is the default).
//    Only the pointers of the 'added' devices are stored.
//    This mode is typically used by making studies based on a fixed set
//    of devices which stays under user control or is kept on an external
//    file/tree. 
//    In this way the AliEvent just represents a 'logical structure' for the
//    physics analysis.
//
//    Note :
//    Modifications made to the original devices also affect the device
//    objects which are stored in the AliEvent. 
//
// b) SetDevCopy(1).
//    Of every 'added' device a private copy will be made of which the pointer
//    will be stored.
//    In this way the AliEvent represents an entity on its own and modifications
//    made to the original calorimeters do not affect the AliCalorimeter objects
//    which are stored in the AliEvent. 
//    This mode will allow 'adding' many different devices into an AliEvent by
//    creating only one device instance in the main programme and using the
//    Reset() and parameter setting memberfunctions of the object representing the device.
//
//    Note :
//    The copy is made using the Clone() memberfunction.
//    All devices (i.e. classes derived from TObject) have the default TObject::Clone() 
//    memberfunction.
//    However, devices generally contain an internal (signal) data structure
//    which may include pointers to other objects. Therefore it is recommended to provide
//    for all devices a specific copy constructor and override the default Clone()
//    memberfunction using this copy constructor.
//    An example for this may be seen from AliCalorimeter.   
//
// See also the documentation provided for the memberfunction SetOwner(). 
//
// Coding example to make an event consisting of a primary vertex,
// 2 secondary vertices and a calorimeter.
// --------------------------------------------------------------
// vp contains the tracks 1,2,3 and 4 (primary vertex)
// v1 contains the tracks 5,6 and 7   (sec. vertex)
// v2 contains the jets 1 and 2       (sec. vertex)
//
//        AliEvent evt;
//
// Specify the event object as the repository of all objects
// for the event building and physics analysis.
// 
//        evt.SetDevCopy(1);
//        evt.SetTrackCopy(1);
//
// Fill the event structure with the basic objects
// 
//        AliCalorimeter emcal;
//         ...
//         ... // code to fill the calorimeter data
//         ...
//
//        evt.AddDevice(emcal);
//
//        AliTrack* tx=new AliTrack();
//        for (Int_t i=0; i<10; i++)
//        {
//         ...
//         ... // code to fill the track data
//         ...
//         evt.AddTrack(tx);
//         tx->Reset(); 
//        }
//
//        if (tx)
//        {
//         delete tx;
//         tx=0;
//        }
//
// Build the event structure (vertices, jets, ...) for physics analysis
// based on the basic objects from the event repository.
//
//        AliJet j1,j2;
//        for (Int_t i=0; i<evt.GetNtracks(); i++)
//        {
//         tx=evt.GetTrack(i);
//         ...
//         ... // code to fill the jet data
//         ...
//        }
//
//        AliVertex vp;
//        tx=evt.GetTrack(1);
//        vp.AddTrack(tx);
//        tx=evt.GetTrack(2);
//        vp.AddTrack(tx);
//        tx=evt.GetTrack(3);
//        vp.AddTrack(tx);
//        tx=evt.GetTrack(4);
//        vp.AddTrack(tx);
//
//        Float_t rp[3]={2.4,0.1,-8.5};
//        vp.SetPosition(rp,"car");
//
//        AliVertex v1;
//        tx=evt.GetTrack(5);
//        v1.AddTrack(tx);
//        tx=evt.GetTrack(6);
//        v1.AddTrack(tx);
//        tx=evt.GetTrack(7);
//        v1.AddTrack(tx);
//
//        Float_t r1[3]={1.6,-3.2,5.7};
//        v1.SetPosition(r1,"car");
//
//
//        AliVertex v2;
//        v2.SetJetCopy(1);
//        v2.AddJet(j1);
//        v2.AddJet(j2);
//
//        Float_t r2[3]={6.2,4.8,1.3};
//        v2.SetPosition(r2,"car");
//
// Specify the vertices v1 and v2 as secondary vertices of the primary
//
//        vp.SetVertexCopy(1);
//        vp.AddVertex(v1);
//        vp.AddVertex(v2);
//
// Enter the physics structures into the event
//        evt.SetVertexCopy(1);
//        evt.AddVertex(vp,0);
//
// The jets j1 and j2 are already available via sec. vertex v2,
// but can be made available also from the event itself if desired.
//        AliJet* jx;
//        jx=v2.GetJet(1);
//        evt.AddJet(jx,0); 
//        jx=v2.GetJet(2);
//        evt.AddJet(jx,0); 
// 
//        evt.Data("sph");
//        v1.ListAll();
//        v2.List("cyl");
//
//        Float_t etot=evt.GetEnergy();
//        Ali3Vector ptot=evt.Get3Momentum();
//        Float_t loc[3];
//        evt.GetPosition(loc,"sph");
//        AliPosition r=v1.GetPosition();
//        r.Data(); 
//        Int_t nt=v2.GetNtracks();
//        AliTrack* tv=v2.GetTrack(1); // Access track number 1 of Vertex v2
//
//        evt.List();
//
//        Int_t nv=evt.GetNvtx();
//        AliVertex* vx=evt.GetVertex(1); // Access primary vertex
//        Float_t e=vx->GetEnergy();
//
//        Float_t M=evt.GetInvmass(); 
//
// Reconstruct the event from scratch
//
//        evt.Reset();
//        evt.SetNvmax(25); // Increase initial no. of sec. vertices
//        ...
//        ... // code to create tracks etc... 
//        ...
//
// Note : All quantities are in GeV, GeV/c or GeV/c**2
//
//--- Author: Nick van Eijndhoven 27-may-2001 UU-SAP Utrecht
//- Modified: NvE $Date: 2003/12/18 09:28:06 $ UU-SAP Utrecht
///////////////////////////////////////////////////////////////////////////

#include "AliEvent.h"
#include "Riostream.h"
 
ClassImp(AliEvent) // Class implementation to enable ROOT I/O
 
AliEvent::AliEvent() : AliVertex()
{
// Default constructor.
// All variables initialised to default values.
 TTimeStamp tx;
 fDaytime=tx;
 fRun=0;
 fEvent=0;
 fAproj=0;
 fZproj=0;
 fPnucProj=0;
 fIdProj=0;
 fAtarg=0;
 fZtarg=0;
 fPnucTarg=0;
 fIdTarg=0;
 fDevices=0;
 fDevCopy=0;
}
///////////////////////////////////////////////////////////////////////////
AliEvent::AliEvent(Int_t n) : AliVertex(n)
{
// Create an event to hold initially a maximum of n tracks
// All variables initialised to default values
 if (n<=0)
 {
  cout << " *** This AliVertex initialisation was invoked via the AliEvent ctor." << endl;
 }
 TTimeStamp tx;
 fDaytime=tx;
 fRun=0;
 fEvent=0;
 fAproj=0;
 fZproj=0;
 fPnucProj=0;
 fIdProj=0;
 fAtarg=0;
 fZtarg=0;
 fPnucTarg=0;
 fIdTarg=0;
 fDevices=0;
 fDevCopy=0;
}
///////////////////////////////////////////////////////////////////////////
AliEvent::~AliEvent()
{
// Default destructor
 if (fDevices)
 {
  delete fDevices;
  fDevices=0;
 }
}
///////////////////////////////////////////////////////////////////////////
AliEvent::AliEvent(AliEvent& evt) : AliVertex(evt)
{
// Copy constructor.
 fDaytime=evt.fDaytime;
 fRun=evt.fRun;
 fEvent=evt.fEvent;
 fAproj=evt.fAproj;
 fZproj=evt.fZproj;
 fPnucProj=evt.fPnucProj;
 fIdProj=evt.fIdProj;
 fAtarg=evt.fAtarg;
 fZtarg=evt.fZtarg;
 fPnucTarg=evt.fPnucTarg;
 fIdTarg=evt.fIdTarg;
 fDevCopy=evt.fDevCopy;

 fDevices=0;
 Int_t ndevs=evt.GetNdevices();
 if (ndevs)
 {
  fDevices=new TObjArray(ndevs);
  if (fDevCopy) fDevices->SetOwner();
  for (Int_t i=1; i<=ndevs; i++)
  {
   TObject* dev=evt.GetDevice(i);
   if (dev)
   {
    if (fDevCopy)
    {
     fDevices->Add(dev->Clone());
    }
    else
    {
     fDevices->Add(dev);
    }
   }
  }
 }
}
///////////////////////////////////////////////////////////////////////////
void AliEvent::Reset()
{
// Reset all variables to default values
// The max. number of tracks is set to the initial value again
// The max. number of vertices is set to the default value again
// Note : The CalCopy mode is maintained as it was set by the user before.

 AliVertex::Reset();

 TTimeStamp tx;
 fDaytime=tx;
 fRun=0;
 fEvent=0;
 fAproj=0;
 fZproj=0;
 fPnucProj=0;
 fIdProj=0;
 fAtarg=0;
 fZtarg=0;
 fPnucTarg=0;
 fIdTarg=0;

 if (fDevices)
 {
  delete fDevices;
  fDevices=0;
 }
}
///////////////////////////////////////////////////////////////////////////
void AliEvent::SetOwner(Bool_t own)
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
 if (fDevices) fDevices->SetOwner(own);
 fDevCopy=mode;

 AliVertex::SetOwner(own);
}
///////////////////////////////////////////////////////////////////////////
void AliEvent::SetDayTime(TTimeStamp& stamp)
{
// Set the date and time stamp for this event.
// An exact copy of the entered date/time stamp will be saved with an
// accuracy of 1 nanosecond.
 fDaytime=stamp;
}
///////////////////////////////////////////////////////////////////////////
void AliEvent::SetDayTime(TDatime& stamp)
{
// Set the date and time stamp for this event.
// The entered date/time will be interpreted as being the local date/time
// and the accuracy is 1 second.
// This function with the TDatime argument is mainly kept for backward
// compatibility reasons. It is recommended to use the corresponding
// function with the TTimeStamp argument.

 TTimeStamp ts(stamp.GetDate(),stamp.GetTime(),0,kFALSE);
 fDaytime=ts;
}
///////////////////////////////////////////////////////////////////////////
void AliEvent::SetRunNumber(Int_t run)
{
// Set the run number for this event
 fRun=run;
}
///////////////////////////////////////////////////////////////////////////
void AliEvent::SetEventNumber(Int_t evt)
{
// Set the event number for this event
 fEvent=evt;
}
///////////////////////////////////////////////////////////////////////////
TTimeStamp AliEvent::GetDayTime()
{
// Provide the date and time stamp for this event
 return fDaytime;
}
///////////////////////////////////////////////////////////////////////////
Int_t AliEvent::GetRunNumber()
{
// Provide the run number for this event
 return fRun;
}
///////////////////////////////////////////////////////////////////////////
Int_t AliEvent::GetEventNumber()
{
// Provide the event number for this event
 return fEvent;
}
///////////////////////////////////////////////////////////////////////////
void AliEvent::SetProjectile(Int_t a,Int_t z,Double_t pnuc,Int_t id)
{
// Set the projectile A, Z, momentum per nucleon and user defined particle ID.
// By default the particle ID is set to zero.
 fAproj=a;
 fZproj=z;
 fPnucProj=pnuc;
 fIdProj=id;
}
///////////////////////////////////////////////////////////////////////////
Int_t AliEvent::GetProjectileA()
{
// Provide the projectile A value.
 return fAproj;
}
///////////////////////////////////////////////////////////////////////////
Int_t AliEvent::GetProjectileZ()
{
// Provide the projectile Z value.
 return fZproj;
}
///////////////////////////////////////////////////////////////////////////
Double_t AliEvent::GetProjectilePnuc()
{
// Provide the projectile momentum value per nucleon.
 return fPnucProj;
}
///////////////////////////////////////////////////////////////////////////
Int_t AliEvent::GetProjectileId()
{
// Provide the user defined particle ID of the projectile.
 return fIdProj;
}
///////////////////////////////////////////////////////////////////////////
void AliEvent::SetTarget(Int_t a,Int_t z,Double_t pnuc,Int_t id)
{
// Set the target A, Z, momentum per nucleon and user defined particle ID.
// By default the particle ID is set to zero.
 fAtarg=a;
 fZtarg=z;
 fPnucTarg=pnuc;
 fIdTarg=id;
}
///////////////////////////////////////////////////////////////////////////
Int_t AliEvent::GetTargetA()
{
// Provide the target A value.
 return fAtarg;
}
///////////////////////////////////////////////////////////////////////////
Int_t AliEvent::GetTargetZ()
{
// Provide the target Z value.
 return fZtarg;
}
///////////////////////////////////////////////////////////////////////////
Double_t AliEvent::GetTargetPnuc()
{
// Provide the target momentum value per nucleon.
 return fPnucTarg;
}
///////////////////////////////////////////////////////////////////////////
Int_t AliEvent::GetTargetId()
{
// Provide the user defined particle ID of the target.
 return fIdTarg;
}
///////////////////////////////////////////////////////////////////////////
void AliEvent::HeaderData()
{
// Provide event header information
 const char* name=GetName();
 const char* title=GetTitle();
 Int_t ndevs=GetNdevices();
 cout << " *" << ClassName() << "::Data*";
 if (strlen(name))  cout << " Name : " << GetName();
 if (strlen(title)) cout << " Title : " << GetTitle();
 cout << endl;
 cout << "  " << fDaytime.AsString() << endl;
 cout << "  Run : " << fRun << " Event : " << fEvent
      << " Number of devices : " << ndevs << endl;

 if (ndevs) ShowDevices();
}
///////////////////////////////////////////////////////////////////////////
void AliEvent::Data(TString f)
{
// Provide event information within the coordinate frame f
 HeaderData();
 AliVertex::Data(f);
} 
///////////////////////////////////////////////////////////////////////////
Int_t AliEvent::GetNdevices()
{
// Provide the number of stored devices
 Int_t ndevs=0;
 if (fDevices) ndevs=fDevices->GetEntries();
 return ndevs;
} 
///////////////////////////////////////////////////////////////////////////
void AliEvent::AddDevice(TObject& d)
{
// Add a device to the event.
//
// Note :
// In case a private copy is made, this is performed via the Clone() memberfunction.
// All devices (i.e. classes derived from TObject) have the default TObject::Clone() 
// memberfunction.
// However, devices generally contain an internal (signal) data structure
// which may include pointers to other objects. Therefore it is recommended to provide
// for all devices a specific copy constructor and override the default Clone()
// memberfunction using this copy constructor.
// An example for this may be seen from AliCalorimeter.   

 if (!fDevices)
 {
  fDevices=new TObjArray();
  if (fDevCopy) fDevices->SetOwner();
 }
 
 // Add the device to this event
 if (fDevCopy)
 {
  fDevices->Add(d.Clone());
 }
 else
 {
  fDevices->Add(&d);
 }
}
///////////////////////////////////////////////////////////////////////////
void AliEvent::SetDevCopy(Int_t j)
{
// (De)activate the creation of private copies of the added devices.
// j=0 ==> No private copies are made; pointers of original devices are stored.
// j=1 ==> Private copies of the devices are made and these pointers are stored.
//
//
// Notes :
//  In case a private copy is made, this is performed via the Clone() memberfunction.
//  All devices (i.e. classes derived from TObject) have the default TObject::Clone() 
//  memberfunction.
//  However, devices generally contain an internal (signal) data structure
//  which may include pointers to other objects. Therefore it is recommended to provide
//  for all devices a specific copy constructor and override the default Clone()
//  memberfunction using this copy constructor.
//  An example for this may be seen from AliCalorimeter.   
//
//  Once the storage contains pointer(s) to device(s) one cannot
//  change the DevCopy mode anymore.
//  To change the DevCopy mode for an existing AliEvent containing
//  devices one first has to invoke Reset().

 if (!fDevices)
 {
  if (j==0 || j==1)
  {
   fDevCopy=j;
  }
  else
  {
   cout << " *" << ClassName() << "::SetDevCopy* Invalid argument : " << j << endl;
  }
 }
 else
 {
  cout << " *" << ClassName() << "::SetDevCopy* Storage already contained devices."
       << "  ==> DevCopy mode not changed." << endl; 
 }
}
///////////////////////////////////////////////////////////////////////////
Int_t AliEvent::GetDevCopy()
{
// Provide value of the DevCopy mode.
// 0 ==> No private copies are made; pointers of original devices are stored.
// 1 ==> Private copies of the devices are made and these pointers are stored.
//
// Note :
// In case a private copy is made, this is performed via the Clone() memberfunction.
// All devices (i.e. classes derived from TObject) have the default TObject::Clone() 
// memberfunction.
// However, devices generally contain an internal (signal) data structure
// which may include pointers to other objects. Therefore it is recommended to provide
// for all devices a specific copy constructor and override the default Clone()
// memberfunction using this copy constructor.
// An example for this may be seen from AliCalorimeter.   

 return fDevCopy;
}
///////////////////////////////////////////////////////////////////////////
TObject* AliEvent::GetDevice(Int_t i)
{
// Return the i-th device of this event.
// The first device corresponds to i=1.

 if (!fDevices)
 {
  return 0;
 }
 else
 {
  Int_t ndevs=GetNdevices();
  if (i<=0 || i>ndevs)
  {
   cout << " *" << ClassName() << "::GetDevice* Invalid argument i : " << i
        << " ndevs = " << ndevs << endl;
   return 0;
  }
  else
  {
   return fDevices->At(i-1);
  }
 }
}
///////////////////////////////////////////////////////////////////////////
TObject* AliEvent::GetDevice(TString name)
{
// Return the device with name tag "name"
 if (!fDevices)
 {
  return 0;
 }
 else
 {
  TString s;
  Int_t ndevs=GetNdevices();
  for (Int_t i=0; i<ndevs; i++)
  {
   TObject* dev=fDevices->At(i);
   if (dev)
   {
    s=dev->GetName();
    if (s == name) return dev;
   }
  }

  return 0; // No matching name found
 }
}
///////////////////////////////////////////////////////////////////////////
void AliEvent::ShowDevices()
{
// Provide an overview of the available devices.
 Int_t ndevs=GetNdevices();
 if (ndevs)
 {
  cout << " The following " << ndevs << " devices are available :" << endl; 
  for (Int_t i=1; i<=ndevs; i++)
  {
   TObject* dev=GetDevice(i);
   if (dev)
   {
    cout << " Device number : " << i
         << " Class : " << dev->ClassName()
         << " Name : " << dev->GetName() << endl;
   }
  }
 }
 else
 {
  cout << " No devices present for this event." << endl;
 }
}
///////////////////////////////////////////////////////////////////////////
TObject* AliEvent::Clone(char* name)
{
// Make a deep copy of the current object and provide the pointer to the copy.
// This memberfunction enables automatic creation of new objects of the
// correct type depending on the object type, a feature which may be very useful
// for containers when adding objects in case the container owns the objects.
// This feature allows to store either AliEvent objects or objects derived from
// AliEvent via some generic AddEvent memberfunction, provided these derived
// classes also have a proper Clone memberfunction. 

 AliEvent* evt=new AliEvent(*this);
 if (name)
 {
  if (strlen(name)) evt->SetName(name);
 }
 return evt;
}
///////////////////////////////////////////////////////////////////////////


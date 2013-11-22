#ifndef ALIITSUINITGEOMETRY_H
#define ALIITSUINITGEOMETRY_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/*
$Id: AliITSUInitGeometry.h $
*/

/////////////////////////////////////////////////////////////////////
// Class to inilize AliITSgeom and the like for both simulation
//  and reconstruction.
/////////////////////////////////////////////////////////////////////

#include <TObject.h>
#include <TString.h>
#include "AliITSgeom.h"

class TArrayD;
class TGeoHMatrix;
class TDatime;

class AliITSUInitGeometry : public TObject{
 public:

    AliITSUInitGeometry();//Default Constructor
    AliITSUInitGeometry(AliITSVersion_t version,Int_t minorversion=2);//Standard Constructor
    //virtual ~AliITSUInitGeometry(); // Destructor
    //
    // Create and initialize geometry from TGeo
    AliITSgeom* CreateAliITSgeom();
    AliITSgeom* CreateAliITSgeom(Int_t major,Int_t minor); 
    Bool_t InitAliITSgeom(AliITSgeom *geom);//Initilize geometry from gGeoManager
    TString GetGeometryName()const {return fName;}// Return geometry name
    void    SetGeometryName(const Char_t *name){fName = name;}// Set Geometry name
    Bool_t  GetTiming()const{return fTiming;} // return routine timing flag
    void    SetTiming(Bool_t time=kTRUE){fTiming=time;}// Set routine timing (on)
    void    SetDebug(Int_t debug=0){fDebug=debug;};
    // Retrun debug value
    Int_t   GetDebug()const{return fDebug;};
    // Decode module number into old layer, ladder, and detector numbers
    void DecodeDetectorLayers(Int_t mod,Int_t &lay,Int_t &lad,Int_t &det);
    // find module number by layer, and copy numbers
    void  DecodeDetector(Int_t &mod,Int_t lay,Int_t cpn0,
			 Int_t cpn1,Int_t cpn2) const;
    // Given module number, find copy numbers.
    void  RecodeDetector(Int_t mod,Int_t &cpn0,Int_t &cpn1,Int_t &cpn2);
    Int_t GetNumberOfLayers();
    Int_t GetNumberOfLadders(const Int_t lay) const;
    Int_t GetNumberOfModules(const Int_t lay) const;
    Int_t GetLayerDetTypeID(const Int_t lay) const;
    //
 private:
    // Decode module number into old layer, ladder, and detector numbers
    void DecodeDetectorLayersv11(Int_t mod,Int_t &lay,
				 Int_t &lad,Int_t &det);
    // find module number by layer, and copy numbers
    void DecodeDetectorv11(Int_t &mod,Int_t lay,Int_t cpn0,Int_t cpn1,
			   Int_t cpn2)const;
    // Given module number, find copy numbers.
    void RecodeDetectorv11(Int_t mod,Int_t &cpn0,Int_t &cpn1,
			   Int_t &cpn2);
    // Decode module number into old layer, ladder, and detector numbers
    void DecodeDetectorvUpgrade(Int_t &mod,Int_t lay,Int_t cpn0,Int_t cpn1,
                                    Int_t cpn2)const;
    // find module number by layer, and copy numbers
    void RecodeDetectorvUpgrade(Int_t mod,Int_t &cpn0,Int_t &cpn1,
                                    Int_t &cpn2);
    // Given module number, find copy numbers.
    void DecodeDetectorLayersvUpgrade(Int_t mod,Int_t &lay,Int_t &lad,
                                    Int_t &det);
		   
    // Virtual MC code 
    Bool_t InitAliITSgeomV11(AliITSgeom *geom);
    Bool_t InitAliITSgeomVUpgrade(AliITSgeom *geom);
    Bool_t GetTransformation(const TString &volumePath,TGeoHMatrix &mat);
    Bool_t GetShape(const TString &volumePath,TString &shapeType,TArrayD &par);
    void TransposeTGeoHMatrix(TGeoHMatrix *m) const;

    TString         fName;         // Geometry name
    Bool_t          fTiming;       // Flag to start inilization timing
    Int_t           fDebug;        // Debug flag

    ClassDef(AliITSUInitGeometry,0) // create/Init AliITSgeom
    // 0 in ClassDef indicates that this class will not be "saved" in a file.
};

#endif


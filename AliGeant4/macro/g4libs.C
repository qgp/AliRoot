// $Id$
//
// Root macro for loading Geant4, geant4_mc and AliGeant4 libraries.

#include <iostream>

static Bool_t isGeant4 = false;
static Bool_t isSteer = false;
static Bool_t isDetector = false;

void g4libs()
{
  // g4libs_global();
  g4libs_granular();
}   

void g4libs_global()
{
// Loads G4 global libraries, 
// external packages: CLHEP, graphics drivers, .. used by G4
// and Alice G4 libraries: AliGeant4, TGeant4
// ---
  if (!isGeant4) {

    cout << "Loading Geant4 global libraries ..." << endl;
 
    // CLHEP
    gSystem->Load("libCLHEP");

    // Geant4
    gSystem->Load("libG4global");
    gSystem->Load("libG4graphics_reps");
    gSystem->Load("libG4intercoms");
    gSystem->Load("libG4materials");
    gSystem->Load("libG4geometry");
    gSystem->Load("libG4particles");
    gSystem->Load("libG4track");
    gSystem->Load("libG4processes");
    gSystem->Load("libG4tracking");
    gSystem->Load("libG4digits+hits");
    gSystem->Load("libG4event");
    gSystem->Load("libG4readout");
    gSystem->Load("libG4run");
    gSystem->Load("libG3toG4");

    // Geant4 interfaces
    //gSystem->Load("/usr/X11R6/lib/libXt");
    //gSystem->Load("/usr/local/lib/libXm");
    gSystem->Load("libG4UIcommon");
    gSystem->Load("libG4UIbasic");
    gSystem->Load("libG4UIGAG");

    // Geant4 visualization
    gSystem->Load("libG4vis_management");
    gSystem->Load("libG4modeling");
    gSystem->Load("libG4FR");
//    gSystem->Load("libG4OpenGL");
    gSystem->Load("libG4VRML");
    
    // geant4mc, AliGeant4
    gSystem->Load("libgeant4vmc");
    gSystem->Load("libAliGeant4");
    
    isGeant4 = true;
    cout << "Loading Geant4 global libraries ... finished" << endl;
  }    
}

void g4libs_granular()
{
// Loads G4 granular libraries, 
// external packages: CLHEP, graphics drivers, .. used by G4
// and Alice G4 libraries: AliGeant4, TGeant4
// ---
  if (!isGeant4) {

    cout << "Loading Geant4 granular libraries ..." << endl;
    
    // CLHEP
    gSystem->Load("libCLHEP");

    // G4 categories

    // global
    gSystem->Load("libG4globman");                      
    gSystem->Load("libG4hepnumerics");

    // graphics_reps
    gSystem->Load("libG4graphics_reps");       

    // intercoms
    gSystem->Load("libG4intercoms");

    // materials
    gSystem->Load("libG4materials");

    // geometry
    gSystem->Load("libG4geomver");
    gSystem->Load("libG4volumes");
    gSystem->Load("libG4magneticfield");
    gSystem->Load("libG4geometrymng");    
    gSystem->Load("libG4geomBoolean");    
    gSystem->Load("libG4csg");                  
    gSystem->Load("libG4step");
    gSystem->Load("libG4brep"); 
    gSystem->Load("libG4specsolids"); 
    gSystem->Load("libG4stepinterface");
  
    // particles          
    gSystem->Load("libG4partman");
    gSystem->Load("libG4bosons");             
    gSystem->Load("libG4baryons");              
    gSystem->Load("libG4ions");
    gSystem->Load("libG4mesons");
    gSystem->Load("libG4leptons");
    gSystem->Load("libG4shortlived");

    // track
    gSystem->Load("libG4track");

    // processes
    gSystem->Load("libG4procman");
    gSystem->Load("libG4parameterisation");
    gSystem->Load("libG4decay");                  
    gSystem->Load("libG4emutils");              
    gSystem->Load("libG4emstandard");           
    gSystem->Load("libG4emlowenergy");          
    gSystem->Load("libG4muons");
    gSystem->Load("libG4xrays");
    gSystem->Load("libG4hadronic_xsect");
    gSystem->Load("libG4hadronic_mgt");         
    gSystem->Load("libG4hadronic_proc");
    gSystem->Load("libG4hadronic_util");
    gSystem->Load("libG4hadronic_man_gen");     
    gSystem->Load("libG4hadronic_util_gen");
    gSystem->Load("libG4hadronic_string_common");
    gSystem->Load("libG4hadronic_diffstring");  
    gSystem->Load("libG4hadronic_stringfrag");
    gSystem->Load("libG4hadronic_HE_gen");      
    gSystem->Load("libG4hadronic_kinetic");     
    gSystem->Load("libG4hadronic_qgstring");
    gSystem->Load("libG4hadronic_HE");          
    gSystem->Load("libG4hadronic_LE");          
    gSystem->Load("libG4hadronic_deex");
    gSystem->Load("libG4hadronic_preequ");      
    gSystem->Load("libG4hadronic_stop");
    gSystem->Load("libG4hadronic_neu");         
    gSystem->Load("libG4hadronic_iso");         
    gSystem->Load("libG4optical");
    gSystem->Load("libG4photolepton_hadron");
    gSystem->Load("libG4transportation");

    // tracking
    gSystem->Load("libG4tracking");

    // digits+hits  
    gSystem->Load("libG4hits");
    gSystem->Load("libG4digits");               
    gSystem->Load("libG4detector");             

    // event
    gSystem->Load("libG4event");                

    // readout
    gSystem->Load("libG4readout");
  
    // run
    gSystem->Load("libG4run");
  
    // g3tog4
    gSystem->Load("libG3toG4");                     

    // interfaces
    //gSystem->Load("/usr/X11R6/lib/libXt");
    //gSystem->Load("/usr/X11R6/lib/libXm");
    gSystem->Load("libG4UIcommon");             
    gSystem->Load("libG4UIbasic");              
    gSystem->Load("libG4UIGAG");                

    // visualisation
    gSystem->Load("libG4modeling");
    gSystem->Load("libG4vis_management");
    gSystem->Load("libG4FR");                   
    //gSystem->Load("libG4OpenGL");               
    gSystem->Load("libG4VRML");                 
    gSystem->Load("libG4RayTracer");                 
  
    // geant4mc, AliGeant4
    gSystem->Load("libgeant4vmc");
    gSystem->Load("libAliGeant4");

    isGeant4 = true;
    cout << "Loading Geant4 granular libraries ... finished" << endl;
  }
}

void steerlibs() {
// Loads AliRoot steer libraries
// ---
  if (!isSteer) {
  
    gSystem->Load("libSTEER");
    gSystem->Load("libdummypythia6");
    gSystem->Load("$(ROOTSYS)/lib/libEGPythia6");
    gSystem->Load("libEVGEN");

    isSteer = true;
    cout << "AliRoot steer libraries have been loaded." << endl;
  }  
}  

void detlibs() {
// Load AliRoot modules libraries
// ---
  if (!isDetector) {

    gSystem->Load("libmicrocern");
    // microcern required by MUON
    gSystem->Load("libSTRUCT");
    gSystem->Load("libFMD");
    gSystem->Load("libMUON");
    gSystem->Load("libPHOS");
    gSystem->Load("libPMD");
    gSystem->Load("libRICH");
    gSystem->Load("libTOF");
    gSystem->Load("libTPC");
    gSystem->Load("libTRD");
    gSystem->Load("libZDC");
    //gSystem->Load("libITS");
    gSystem->Load("libSTART");
    
    isDetector = true;
    cout << "AliRoot detectors libraries have been loaded." << endl;
  }  
}  

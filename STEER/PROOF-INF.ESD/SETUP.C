void SETUP()
{
   // Load some ROOT libraries
   gSystem->Load("libEG");
   gSystem->Load("libGeom");
   gSystem->Load("libTree"); // from ROOT v5.15.04

   // Load the ESD library
   gSystem->Load("libESD");

   // Set the Inlucde paths
   gSystem->SetIncludePath("-I$ROOTSYS/include -IESD");
   gROOT->ProcessLine(".include ESD");

   // Set our location, so that other packages can find us
   gSystem->Setenv("ESD_INCLUDE", "ESD");
}

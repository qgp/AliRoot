// $Id$

/*! 

\page README_mchda Tracking DA
 
The detector algorithms are implemented for the Muon Tracking in the AliRoot framework.
We currently have 3 DAs for MCH :

- MUONTRKPEDda.cxx for PEDESTAL runs, running at the end of data taking on each LDC.
- MUONTRKGAINda.cxx for CALIBRATION runs, running at the end of data taking on each LDC.
- MUONTRKOCCda.cxx for PHYSICS runs, running during data taking on each LDC.

\section da_s1 The Muon Tracking Calibration

The Muon tracking chambers needs three types of calibration in order to work properly 
(to be more precise pedestals are required, gains are needed to get the best charge measurement possible, and the occupancy
 is needed in order not to spend all the reconstruction time in hot-spots). 

\subsection da_ss1 Pedestals

The front-end electronics performs an online zero suppression using a threshold level.
Those threshold levels for all channels (~ 1 million) have to be computed in a dedicated
PEDESTALS runs. During this runs the zero suppression is OFF and the pedestal level and the noise is obtained for each channel. The threshold for the FEE is obtained adding the pedestal level to 3 sigmas of the noise. 

The typical ECS sequence for pedestals is :

- Switch ON the electronics LV
- Boot the CROCUS 
- Configuration 
- Saving Configuration in an ascii file then transferring in the File eXchange Server (FXS)
- Zero suppression OFF
- Data taking (typically 400 events)
- The DA computes the mean and sigma (it runs in each LDC)
- The DA writes one ASCII file per LDC with the results in the File Exchange Server

Then the SHUTTLE process the ASCII files and store the result on the OCDB (Keyword=PEDESTALS)
Only configuration files corresponding to a change of the Muon Tracker configuration are written in the FXS (Keyword=CONFIG).


\subsection da_ss2 Electronics gain

In order to perform the required spatial resolution or the tracking chambers (~ 100 microns),
we need to calibrate the gain of each channel. The gain is computed using dedicated runs where
a signal (DAC) is send to the chambers FEE. 

The typical ECS sequence for calibration is :

- Switch ON the electronics LV
- Boot the CROCUS 
- Configuration 
- Zero suppression OFF
- Loop of 11 data taking (typically 400 events) each with a different signal (DAC=0-200- 400-800-1200-1600-2000-2500-3000-3500-4000)
- The DA computes the mean and sigma (it runs in each LDC) for each run
- At the end of the last run, the DA computes, by using a fitting procedure, linear and parabolic gain parameters, and writes results in one ASCII file per LDC. Every ascii files are transferred in the FXS.

Then the SHUTTLE process the ASCII files and store the result on the OCDB (KEYWORD=GAINS)

\subsection da_ss3 Occupancy

For PHYSICS (or STANDALONE) runs, the MUONTRKOCCda, which is a monitoring DA, keep track of how many times
 each channel has been hit during the run. The output is an ASCII file containing the needed information to 
 compute the occupancy values. This file is written to the DAQ FXS so the SHUTTLE can transfer it to the OCDB.

\section da_s2 Using the DA Online

\subsection da_ss1 Pedestals

The syntax is: MUONTRKPEDda.exe "raw data file"

Two input files located in the DAQ Detector database (DetDB) are needed:

- muontrkpedvalues is built in flight in CONFIGURATION_PED.sh (ECS script) and contains two parameters "config" and "writeconfig"
  config = 1 if configuration file has to be used (OnLine case)
  config = 0 if not (OffLine case for the time being)
  writeconfig=0 if configuration is unchanged
  writeconfig=1 if configuration is changed , then new config. file is written in DetDB

- config_ldc-MTRK-S3-0 : configuration file name corresponding to MuonTracker Station 3 if (for example) DA is running on ldc-MTRK-S3-0

- DA validation: see Header of MUONTRKPEDda.cxx for reference run, and corresponding input mutrkpedvalues and configuration files are located in path=/afs/cern.ch/user/j/jcharvet/public/DA_validation

\subsection da_ss2 Electonics gain

The syntax is: MUONTRKGAINda.exe "raw data file"

Two input files located in the DAQ Detector database (DetDB) are needed:

- muontrkcalibvalues: which attributes to each the run index (1->11) its corrresponding DAC value. The other parameters are used to tune the fit procedure (for expert). The last parameter indicates the number of events to be read: if "0" all events in the run are read, if not the parameter indicates the maximum number of events to be read. 
Default values are listed below

\verbatim
1 0			
2 200
3 400
4 800
5 1200
6 1600
7 2000
8 2500
9 3000
10 3500
11 4000
1						
6
0
1
1
0
\endverbatim

 - config_ldc-MTRK-S3-0 : configuration file name corresponding to MuonTracker station 3 if (for example) DA is running on ldc-MTRK-S3-0 

- DA validation: Header of MUONTRKGAINda.cxx shows the list of the 11 reference runs, and corresponding input mutrkcalibvalues and configuration files are located in path=/afs/cern.ch/user/j/jcharvet/public/DA_validation

\section da_s3 Using the DA Offline
 
The DAs normally runs with a RAW data DATE format as input
The development of an Offline version is under way.

Nevertheless, Pedestal runs can be analysed locally, but without detector configuration file.  If you get a file in root format (e.g. from alien), you can de-rootify it using the
  "deroot" program which is part of aliroot. 
Note that PED and GAIN DAs work with ROOT input files as well.

You have a line command help. To have it just type :

\verbatim
> MUONTRKPEDda.exe -h

******************* ./MUONTRKPEDda.exe usage **********************
Online (called from ECS) : ./MUONTRKPEDda.exe <raw data file> (no inline options)

./MUONTRKPEDda.exe can be used locally only with options (without DiMuon configuration file)
./MUONTRKPEDda.exe -options, the available options are :
-h help                    (this screen)

 Input
-f <raw data file>         (default = )

 Output
-a <Flat ASCII file>       (default = MUONTRKPEDda.ped)

 Options
-m <max date events>       (default = 1000000)
-s <skip events>           (default = 0)
-n <max events>            (default = 1000000)

\endverbatim

  
\section da_s4 In case of trouble

Please contact :

Jean-Luc Charvet : jean-luc.charvet@cern.ch 
or
Alberto Baldisseri : a.baldisseri@cea.fr
or
Laurent Aphecetche : laurent.aphecetche@subatech.in2p3.fr (for OCC DA)

This chapter is defined in the READMEmchda.txt file.
*/


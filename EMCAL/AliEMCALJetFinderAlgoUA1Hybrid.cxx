//SARAH'S HYBRID PERSONAL COPY!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//THIS IS SARAH'S REVISED UA1 CODE WITH CHANGES FOR ETA/PHI ITERATION INCLUDED!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//THIS Also includes summing ALL cells in the jetcone towards the jet energy NOT just those above threshold!!!!!


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

/*
 
$Log$
Revision 1.3  2003/06/18 17:00:25  horner
Cleaned up more cout statements - Mark

Revision 1.2  2003/06/18 00:20:07  horner
Removed cout statement - Mark


*/


//*--Author: Sarah Blyth (LBL)
//*--Based on UA1 jet algorithm from LUND JETSET called from EMC-erj

#include "TTask.h"
#include "AliEMCALJetFinderInput.h"
#include "AliEMCALJetFinderOutput.h"
#include "AliEMCALJetFinderAlgo.h"
#include "AliEMCALJetFinderAlgoUA1Hybrid.h"
#include "AliEMCALJetFinderAlgoUA1Unit.h"
#include "AliEMCALGeometry.h"
#include "AliEMCAL.h"
#include "AliEMCALDigit.h"
#include "TParticle.h"
#include "AliRun.h"
#include "AliEMCALJet.h"
#include "TMath.h"


ClassImp(AliEMCALJetFinderAlgoUA1Hybrid)

  AliEMCALJetFinderAlgoUA1Hybrid::AliEMCALJetFinderAlgoUA1Hybrid()
{
  //Default constructor
if (fDebug>0) Info("AliEMCALJetFinderAlgoUA1Hybrid","Beginning Default Constructor");

  fNumIter           = 0;
  fNumUnits          = 13824;     //Number of towers in EMCAL
  fESeed             = 5.0;       //Default value
  fConeRad           = 0.5;       //Default value
  fJetEMin           = 10.0;      //Default value
  fEtMin             = 0.28;      //Default value
  fMinMove           = 0.05;      //From original UA1 JetFinder
  fMaxMove           = 0.15;      //From original UA1 JetFinder
  fBGMaxMove         = 0.035;     //From original UA1 JetFinder
  fPtCut             = 0;         
  fHadCorr           = 0;       
  fEBGTotal          = 1.0;       //Set to 1 so that no div by zero in first FindJets() loop
  fEBGTotalOld       = 0.0;
  fEBGAve            = 0.0;
  fEnergy            = 0.0;
  fJetEta            = 0.0;
  fJetPhi            = 0.0;
  fEtaInit           = 0.0;
  fPhiInit           = 0.0;
  fEtaB              = 0.0;
  fPhiB              = 0.0;
  fJetESum           = 0.0;
  fJetEtaSum         = 0.0;
  fJetPhiSum         = 0.0;
  fDEta              = 0.0;
  fDPhi              = 0.0;
  fDistP             = 0.0;
  fDistI             = 0.0;
  fTempE             = 0.0;
  fRad               = 2.0;      //Set to 2 to start 
  fNumInCone         = 0;
  fNumJets           = 0;
  fArrayInitialised  = 0;        //Set to FALSE to start
}

 AliEMCALJetFinderAlgoUA1Hybrid::~AliEMCALJetFinderAlgoUA1Hybrid()
   {
     //Destructor
     if (fDebug>0) Info("AliEMCALJetFinderAlgoUA1Revised","Beginning Destructor");
     delete[] fUnit;
   }

 void AliEMCALJetFinderAlgoUA1Hybrid::SetJetFindingParameters
                               (Int_t numUnits, Float_t eSeed, Float_t coneRad, Float_t jetEMin, Float_t etMin, 
                               Float_t minMove, Float_t maxMove, Float_t bgMaxMove)
   {
     //Sets parameters for the JetFinding algorithm
     if (fDebug>1) Info("SetJetFindingParameters","Setting parameters for JetFinding");

     SetNumUnits(numUnits);
     SetJetESeed(eSeed);
     SetConeRad(coneRad);
     SetJetEMin(jetEMin);
     SetEtMin(etMin);
     SetMinMove(minMove);
     SetMaxMove(maxMove);
     SetBGMaxMove(bgMaxMove);
   }

 void AliEMCALJetFinderAlgoUA1Hybrid::SetJetFindingParameters
                               (Int_t numUnits, Float_t eSeed, Float_t coneRad, Float_t jetEMin, Float_t etMin)
   {
     //Sets fewer parameters for the JetFinding algorithm
     if (fDebug>1) Info("SetJetFindingParameters","Setting parameters for JetFinding");

     SetNumUnits(numUnits);
     SetJetESeed(eSeed);
     SetConeRad(coneRad);
     SetJetEMin(jetEMin);
     SetEtMin(etMin);
     SetMinMove(fMinMove);
     SetMaxMove(fMaxMove);
     SetBGMaxMove(fBGMaxMove);
   }

 void AliEMCALJetFinderAlgoUA1Hybrid::InitUnitArray()
   {
     //Initialises unit array
     if(fArrayInitialised) delete[] fUnit;
     fUnit = new AliEMCALJetFinderAlgoUA1Unit[fNumUnits];
     fArrayInitialised = 1;
   }

 void AliEMCALJetFinderAlgoUA1Hybrid::FillUnitArray(AliEMCALJetFinderAlgoUA1FillUnitFlagType_t flag)
   {
     if (fDebug>1) Info("FillUnitArray","Beginning FillUnitArray");
     AliEMCAL* pEMCAL = (AliEMCAL*) gAlice->GetModule("EMCAL");

         //   if (pEMCAL){ 
         //	     AliEMCALGeometry* geom =  AliEMCALGeometry::GetInstance(pEMCAL->GetTitle(), "");
         //     }else
         //    {
     AliEMCALGeometry* geom =  AliEMCALGeometry::GetInstance("EMCAL_5655_21", "");
        //    }
         
     AliEMCALJetFinderAlgoUA1FillUnitFlagType_t option = flag;
     Int_t         numTracks, numDigits;
    
     //Loops over all elements in the AliEMCALJetFinderAlgoUA1Unit array and 
     //fills the objects with relevant values from the Data Input object
     if (fDebug>1) Info("FillUnitArray","Filling array with Unit objects");
     if (fDebug>1) Info("FillUnitArray","NTracks %i NDigits %i",fInputPointer->GetNTracks(),fInputPointer->GetNDigits());
	 numTracks = fInputPointer->GetNTracks();
	 numDigits = fInputPointer->GetNDigits();
	 TParticle         *myPart;
	 AliEMCALDigit     *myDigit;

	 //Fill units with Track info if appropriate
	 if(option==kFillTracksOnly || option ==kFillAll) 
	   {          
	    for(Int_t j=0; j<numTracks; j++)
	    {
	     myPart = fInputPointer->GetTrack(j);
	     Float_t eta = myPart->Eta();
	     Float_t  phi = myPart->Phi();
	     Int_t towerID = geom->TowerIndexFromEtaPhi(eta,180.0/TMath::Pi()*phi);
	     Float_t  pT = myPart->Pt();
	     Float_t unitEnergy = fUnit[towerID-1].GetUnitEnergy(); 

   	     //Do Hadron Correction
              if(fHadCorr != 0)
	       {
		 Double_t   fullP = myPart->P();
		 Double_t   hCEnergy = fHadCorr->GetEnergy(fullP, (Double_t)eta);
		 unitEnergy -= hCEnergy*TMath::Sin(myPart->Theta());
		 fUnit[towerID-1].SetUnitEnergy(unitEnergy);
	       } //end Hadron Correction loop 
	     
	     //Do Pt cut on tracks
	     if(fPtCut != 0 && pT < fPtCut) continue;

	     fUnit[towerID-1].SetUnitEnergy(unitEnergy+pT);

             }//end tracks loop
	   }//end Tracks condition


	 //Fill units with Digit info if appropriate
	 if(option ==kFillDigitsOnly || option ==kFillAll)
	   {
            for(Int_t k=0; k<numDigits; k++)
	    {
	     myDigit = fInputPointer->GetDigit(k);
             if (fDebug>1) Info("FillUnitArray","getting digits %i %i numdigits",k,numDigits );
	     Int_t towerID = myDigit->GetId();
	     Int_t amplitude = myDigit->GetAmp();     //Gets the integer valued amplitude of the digit
	     Float_t amp = (Float_t)amplitude;        //Need to typecast to Float_t before doing real energy conversion
	     Float_t digitEnergy = amp/10000000.0;    //Factor of 10 million needed to convert!
	     Float_t unitEnergy = fUnit[towerID-1].GetUnitEnergy() + digitEnergy;
	     fUnit[towerID-1].SetUnitEnergy(unitEnergy);

	    }//end digits loop
	   }//end digits condition

	 //Set all unit flags, Eta, Phi
	 for(Int_t i=0; i<fNumUnits; i++)
	   {
             if (fDebug>1) Info("FillUnitArray","Setting all units outside jets");
	     fUnit[i].SetUnitFlag(kOutJet);           //Set all units to be outside a jet initially
	     fUnit[i].SetUnitID(i+1);
	     Float_t eta;
	     Float_t phi;
	     geom->EtaPhiFromIndex(fUnit[i].GetUnitID(), eta, phi);
	     fUnit[i].SetUnitEta(eta);
	     fUnit[i].SetUnitPhi(phi*TMath::Pi()/180.0);
	     //	     if(i>13000) cout<<"!!!!!!!!!!!!!!!!!For unit0, eta="<<eta<<" and phi="<<phi*TMath::Pi()/180.0<<" and ID="<<fUnit[i].GetUnitID()<<endl;
	     //  if(fUnit[i].GetUnitEnergy()>0) cout<<"Unit ID "<<fUnit[i].GetUnitID() <<"with eta="<<eta<<" and phi="<<phi*TMath::Pi()/180.0<<" has energy="<<fUnit[i].GetUnitEnergy()<<endl;
	   }//end loop over all units in array (same as all towers in EMCAL)
   }


 void AliEMCALJetFinderAlgoUA1Hybrid::Sort(AliEMCALJetFinderAlgoUA1Unit *unit, Int_t length)
 {
   //Calls the recursive quicksort method to sort unit objects in decending order of Energy
   if (fDebug>1) Info("Sort","Sorting Unit objects");
   QS(unit, 0, length-1);
 }
  

 void AliEMCALJetFinderAlgoUA1Hybrid::QS(AliEMCALJetFinderAlgoUA1Unit *unit, Int_t left, Int_t right)
 {
  //Sorts the AliEMCALJetFinderAlgoUA1Unit objects in decending order of Energy
   if (fDebug>111) Info("QS","QuickSorting Unit objects");   

   Int_t    i;
   Int_t    j;
   AliEMCALJetFinderAlgoUA1Unit  unitFirst;
   AliEMCALJetFinderAlgoUA1Unit  unitSecond;

   i = left;
   j = right;
   unitFirst = unit[(left+right)/2];

 do
  {
    while( (unit[i].GetUnitEnergy() > unitFirst.GetUnitEnergy()) && (i < right)) i++;
    while( (unitFirst.GetUnitEnergy() > unit[j].GetUnitEnergy()) && (j > left)) j--;

    if(i <= j)
      {
	unitSecond = unit[i];
	unit[i] = unit[j];
	unit[j] = unitSecond;
	i++;
	j--;
      }//end if
  }while(i <= j);

 if(left < j) QS(unit, left, j);
 if(i < right) QS(unit, i, right);
 }


 void AliEMCALJetFinderAlgoUA1Hybrid::FindBG()
   {
     //Finds the background energy for the iteration
     if (fDebug>1) Info("FindBG","Finding Average Background"); 

     fEBGTotal          = 0.0;
     Int_t numCone      = 0;

     //Loop over all unit objects in the array and sum the energy of those not in a jet
     for(Int_t i=0; i<fNumUnits; i++)
       {
	 if(fUnit[i].GetUnitFlag() != kInJet)
	   fEBGTotal += fUnit[i].GetUnitEnergy();
	 else numCone++;
       }//end for

     fEBGTotalOld = fEBGTotal;
     fEBGAve = fEBGTotal / (fNumUnits - numCone);
     if (fDebug>5) Info("FindBG","Average BG is %f: ",fEBGAve);      

     for(Int_t count=0; count<fNumUnits;count++)
       {
	 fUnit[count].SetUnitFlag(kOutJet);
       }//end for
   }


 void AliEMCALJetFinderAlgoUA1Hybrid::FindJetEtaPhi(Int_t counter)
   {
     //Finds the eta and phi of the jet axis
     if (fDebug>1) Info("FindJetEtaPhi","Finding Jet Eta and Phi");

     fDEta = fUnit[counter].GetUnitEta() - fEtaInit;
     fDPhi = fUnit[counter].GetUnitPhi() - fPhiInit;

     fEnergy = fUnit[counter].GetUnitEnergy() - fEBGAve;
     fJetEtaSum += fEnergy * fDEta;
     fJetPhiSum += fEnergy * fDPhi;
     fJetESum += fEnergy;
     fJetEta = fEtaInit + (fJetEtaSum / fJetESum);
     fJetPhi = fPhiInit + (fJetPhiSum / fJetESum);
   }


 void AliEMCALJetFinderAlgoUA1Hybrid::FindJetEnergy()
   {
     //Finds the energy of the jet after the final axis has been found
     if (fDebug>1) Info("FindJetEnergy","Finding Jet Energy");

     for(Int_t i=0; i<fNumUnits; i++)
       {
	 //Loop over all unit objects in the array and find if within cone radius
	 Float_t dEta = fUnit[i].GetUnitEta() - fJetEta;
	 Float_t dPhi = fUnit[i].GetUnitPhi() - fJetPhi;
	 Float_t rad = TMath::Sqrt( (dEta*dEta) + (dPhi*dPhi) );

	 if(fUnit[i].GetUnitFlag()==kOutJet && rad<= fConeRad)
	   {
	     fUnit[i].SetUnitFlag(kInCurrentJet);
	     Float_t energy = fUnit[i].GetUnitEnergy() - fEBGAve;
	     fJetESum += energy;                             
	     fJetEtaSum += energy * dEta;
	     fJetPhiSum += energy * dPhi;
	     fNumInCone++;                     //Increment the number of cells in the jet cone
	   }//end if
       }//end for
   }


 void AliEMCALJetFinderAlgoUA1Hybrid::StoreJetInfo()
   {
     //Stores the resulting jet information in appropriate storage structure (TO BE DECIDED!!!!)
     if (fDebug>1) Info("StoreJetInfo","Storing Jet Information");
    
     //Store:
     //fJetESum is the final jet energy (background has been subtracted)
     //fJetEta is the final jet Eta
     //fJetPhi is the final jet Phi
     //fNumInCone is the final number of cells included in the jet cone
     //fEtaInit is the eta of the initiator cell
     //fPhiInit is the phi of the initiator cell
     fJet.SetEnergy(fJetESum);
     fJet.SetEta(fJetEta);
     fJet.SetPhi(fJetPhi);

      cout<<"For iteration "<<fNumIter <<" and Jet number " <<fNumJets <<endl;
      cout<<"The jet energy is: " <<fJetESum <<endl;
      cout<<"The jet eta is ---->" <<fJetEta <<endl;
      cout<<"The jet phi is ---->" <<fJetPhi <<endl;

     Int_t             numberTracks = fInputPointer->GetNTracks();
     TParticle         *myP;
     Int_t             numTracksInCone = 0;

     for(Int_t counter=0; counter<numberTracks; counter++)
       {
	myP = fInputPointer->GetTrack(counter);
	Float_t eta = myP->Eta();
	Float_t phi = myP->Phi(); 
	Float_t deta = fJetEta-eta;
	Float_t dphi = fJetPhi -phi;
	Float_t rad = TMath::Sqrt( (deta*deta) + (dphi*dphi));
	if(rad<=fConeRad) numTracksInCone++;
       }//end for

     Float_t    *pTArray = new Float_t[numTracksInCone];
     Float_t    *etaArray = new Float_t[numTracksInCone];
     Float_t    *phiArray = new Float_t[numTracksInCone];
     Int_t      *pdgArray = new Int_t[numTracksInCone];
     Int_t             index = 0;

     for(Int_t counter2=0; counter2<numberTracks; counter2++)
       {
	 myP = fInputPointer->GetTrack(counter2);
 	 Float_t eta = myP->Eta();
         Float_t phi = myP->Phi(); 
	 Float_t deta = fJetEta-eta;
	 Float_t dphi = fJetPhi -phi;
	 Float_t rad = TMath::Sqrt( (deta*deta) + (dphi*dphi));
	 if(rad<=fConeRad)
	   {
	     pTArray[index] = myP->Pt();
	     etaArray[index] = eta;
	     phiArray[index] = phi;
	     pdgArray[index] = myP->GetPdgCode();
	     index++;
	   }//end if
       }//end for

     fJet.SetTrackList(numTracksInCone,pTArray, etaArray, phiArray, pdgArray);
     fOutputObject.AddJet(&fJet);
     delete[] pTArray;
     delete[] etaArray;
     delete[] phiArray;
     delete[] pdgArray;
   }


 void AliEMCALJetFinderAlgoUA1Hybrid::FindJets()
   {
     //Runs the complete UA1 JetFinding algorithm to find jets!
     if (fDebug>1) Info("FindJets","Starting Jet Finding!!!");

     //If the array of JetFinderUnit objects has not been initialised then initialise with default settings
     if(!fArrayInitialised) 
      {
       InitUnitArray();
       FillUnitArray(kFillAll);
      }//end if
     if (fDebug>1) Info("FindJets","Unit array filled");

     //Step 1. Sort the array in descending order of Energy
     Sort(fUnit,fNumUnits);

     //Step 2. Set the number of iterations and Number of jets found to zero to start
     fNumIter = 0;
     fNumJets = 0;

     //Step 3. Begin the iteration loop to find jets
     //Need to iterate the algorithm while number of iterations<2 OR number of iterations<10 AND 
     //the value of the average background has changed more than specified amount
     //Min iterations = 2, Max iterations = 10
     //while(fNumIter<2 || (fNumIter <10 && ( (fEBGTotal-fEBGTotalOld)/fEBGTotal) > fBGMaxMove) )

     while(fNumIter<2 || (fNumIter <10 && ( fEBGTotal-fEBGTotalOld) > fEBGTotal*fBGMaxMove) )
       {
        if (fDebug>1) Info("FindJets","Starting BIG iteration ---> %i",fNumIter);

         //Step 4. Find the value of the average background energy
	 FindBG();
	 fOutputObject.Reset(kResetJets); //Reset output object to store info for new iteration
	 fNumJets=0;

	 //Loop over the array of unit objects and flag those with energy below MinCellEt
         Int_t numbelow = 0;
	 for(Int_t j=0; j<fNumUnits; j++)
	   {
	     if( (fUnit[j].GetUnitEnergy()-fEBGAve) < fEtMin)
        	{       
		  //          fUnit[j].SetUnitFlag(kBelowMinEt);    TAKING OUT kBelow flag
                  numbelow++;
                }//end if
	   }//end for
	 //cout<<"THERE WERE "<<numbelow<<" units with E <EtMin!!!!!!!!!!!!!!!"<<endl;

	 //Do quick check if there are no jets upfront
	 // if(fUnit[0].GetUnitFlag() == kBelowMinEt)
	 if( (fUnit[0].GetUnitEnergy()-fEBGAve) < fEtMin)
	   {
            cout <<"There are no jets for this event!" <<endl;
	    break;
	   }//end if

         //Step 5. Begin with the first jet candidate cell (JET SEED LOOP)
         if (fDebug>5) Info("FindJets","Beginning JET SEED LOOP");
	 for(Int_t count=0; count<fNumUnits; count++)
	   {

//CHECK CONDITION HERE _ NOT SURE IF SHOULD MAYBE BE: GetUnitEnergy()-fEBGAve >fESeed?????????????????????????????
	     if(fUnit[count].GetUnitEnergy()>=fESeed && fUnit[count].GetUnitFlag()==kOutJet)
	       {
		 fEnergy = fUnit[count].GetUnitEnergy() - fEBGAve;
		 fJetEta = fUnit[count].GetUnitEta();
		 fJetPhi = fUnit[count].GetUnitPhi();
		 Int_t seedID = fUnit[count].GetUnitID();
                 if (fDebug>5) Info("FindJets","Inside first candidate jet seed loop for time : %i", count);
                 if (fDebug>5) Info("FindJets","Found candidate energy %f ",fEnergy);
                 if (fDebug>5) Info("FindJets","Found candidate eta %f ", fJetEta);
                 if (fDebug>5) Info("FindJets","Found candidate phi %f ", fJetPhi);
		 if (fDebug>5) Info("FindJets","Found candidate ID %i", seedID);

		 fEtaInit = fJetEta;
		 fPhiInit = fJetPhi;
		 fEtaB = fJetEta;
		 fPhiB = fJetPhi;
		 fJetESum = 0.0;
		 fJetEtaSum = 0.0;
		 fJetPhiSum = 0.0;
       
         //Step 6. Find Jet Eta and Phi
		 //Loop over all units in the array to find the ones in the jet cone and determine contrib to Jet eta, phi
		 do
		   {
		     for(Int_t count1=0; count1<fNumUnits; count1++)		   
		       {
			 if(fUnit[count1].GetUnitID() == seedID) continue;   //skip unit if the jetseed to avoid doublecounting
			 if(fUnit[count1].GetUnitFlag() == kOutJet)
			   {
			     fDEta = fUnit[count1].GetUnitEta() - fJetEta;
			     fDPhi = fUnit[count1].GetUnitPhi() - fJetPhi;
			     fRad = TMath::Sqrt( (fDEta*fDEta) + (fDPhi*fDPhi) );
			     if(fRad <= fConeRad)
			       {
				 FindJetEtaPhi(count1); 
			       }//end if
			   }//end if
		       }//end for (Jet Eta, Phi LOOP)
			     
		      //Find the distance cone centre moved from previous cone centre
		      if (fDebug>10) Info("FindJets","Checking if cone move small enough");
		      fDistP = TMath::Sqrt( ((fJetEta-fEtaB)*(fJetEta-fEtaB)) + ((fJetPhi-fPhiB)*(fJetPhi-fPhiB)) );
		      //     if(fDistP <= fMinMove) break;
			     

		      //Find the distance cone centre is from initiator cell
		      if (fDebug>10) Info("FindJets","Checking if cone move too large");
		      fDistI = TMath::Sqrt( ((fJetEtaSum/fJetESum)*(fJetEtaSum/fJetESum)) + ((fJetPhiSum/fJetESum)*
												    (fJetPhiSum/fJetESum)));

		      if(fDistP>fMinMove && fDistI<fMaxMove)
			{
			  fEtaB = fJetEta;
			  fPhiB = fJetPhi;
			}//end if
		 
		   }while(fDistP>fMinMove && fDistI<fMaxMove);
			  
		 fJetEta = fEtaB;
		 fJetPhi = fPhiB;


       //Step 7. Find the Jet Energy
                 if (fDebug>1) Info("FindJets","Looking for Jet energy");
		 fJetESum = 0.0;
		 fJetEtaSum = 0.0;
		 fJetPhiSum = 0.0;
		 fNumInCone = 0;
		 FindJetEnergy();

		 //cout<<"Number of cells in jet cone is: "<<fNumInCone<<endl;

       //Step 8. Check if the jet is a valid jet
		 //Check if cluster energy is above Min allowed to be a jet
//DID NOT DO THE COSH COMPARISON HERE -> NEED TO CHECK WHICH COMPARISON IS BEST!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                 if (fDebug>5) Info("FindJets","Checking cluster is valid jet");
		 if(fJetESum < fJetEMin)
		   {
		     for(Int_t count2=0; count2<fNumUnits; count2++)
		       {
			 if(fUnit[count2].GetUnitFlag()==kInCurrentJet || fUnit[count2].GetUnitFlag()==kOutJet)
			   fUnit[count2].SetUnitFlag(kOutJet);
		       }//end for
                   if (fDebug>10) Info("FindJets","NOT a valid jet cell");
		  }else
		    {
		     for(Int_t count2=0; count2<fNumUnits; count2++)
		       {
			 if(fUnit[count2].GetUnitFlag()==kInCurrentJet)
			   {
			     //	     cout<<"Setting unit #"<<count2 <<" to be officially in a jet!"<<endl;
                           fUnit[count2].SetUnitFlag(kInJet);
			   }
		       }//end for			

 //NEED TO CHECK FINAL WEIRD ITERATION OF ETA AND PHI CHANGES!!!!!!!!!
		     //	 fJetPhi += fJetPhiSum/fJetESum;        //CHECK!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		     //  fJetEta += fJetEtaSum/fJetESum;        //CHECK!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		     fNumJets++;              //Incrementing number of jets found
		     StoreJetInfo();          //Storing jet info

		 }//end if (check cluster above Min Jet Energy)
	       }//end if (Jet Seed condition)
	   }//end (JET SEED LOOP)

if (fDebug>5) Info("FindJets","End of BIG iteration number %i",fNumIter);
// this->Dump();
	 fNumIter++;
       }//end 10 iteration WHILE LOOP
 }












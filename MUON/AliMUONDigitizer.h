#ifndef ALIMUONDIGITIZER_H
#define ALIMUONDIGITIZER_H
/* Copyright(c) 1998-2001, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */
#include "AliDigitizer.h"
#include "AliRunLoader.h"
#include "AliMUONLoader.h"
#include "AliMUONTransientDigit.h"
#include "AliMUON.h"
#include "AliMUONData.h"

class AliRunDigitizer;
class AliMUONHitMapA1;


class AliMUONDigitizer : public AliDigitizer
{
public:

	/* Default constructor initializes all the internal pointers to NULL.
	 */
	AliMUONDigitizer();
	
	/* Constructor initialises all the internal pointers to NULL and
	   sets this digitizers manager to the one specified.
	 */
	AliMUONDigitizer(AliRunDigitizer * manager);

	// The Init method does nothing.
	// All initialization happens in Exec at the moment.  
	virtual Bool_t Init();

	// Override the TTask Exec method.
	virtual void Exec(Option_t* option = 0);

	Int_t GetDebug() const {return fDebug;}       // Get debug level.
	void SetDebug(Int_t level){fDebug = level;}   // Set debug level.    

protected:

	// Parses the option string given to the Exec method.
	virtual void ParseOptions(Option_t* options);

	/* Digitizers inheriting from AliMUONDigitizer should implement this abstract method 
	   so that TransientDigit objects are generated and put onto the fTDList.
	   The method would be implemented as some loop over the input stream. The data can be
	   fetched from the muondata pointer. To add to the fTDList once should use code similar
	   to the following:
	   
	     TObject* source_object;  // Assume the object from which the transient digit
	                              // is created is already initialized at this point.
	     AliMUONTransientDigit* td = new AliMUONTransientDigit();
	     // Initialize the transient digit.
	     // ...
	     
	     // The following line of code is required to have working digitisers that want
	     // to store information about which digits were created from which hits. 
	     OnCreateTransientDigit(td, source_object);
	     
	     AddOrUpdateTransientDigit(td);  // Adds to the fTDList preventing duplicates.
	 */
	virtual void GenerateTransientDigits() = 0;
	
	// Loops over the fTDList of transient digits to write them to the output stream.
	virtual void CreateDigits();

	/* Inheriting digitizers should implement this method to prepare the muondata
	   object before GenerateTransientDigits() is called. 
	   If the initialization was successful then kTRUE should be returned otherwise
	   kFALSE should be returned. 
	 */
	virtual Bool_t InitInputData(AliMUONLoader* muonloader) = 0;
	
	/* This method should be overridden to undo/cleanup what was done in the 
	   InitInputData method call.
	 */
	virtual void CleanupInputData(AliMUONLoader* muonloader) = 0;

	/* Inheriting digitizers should implement this method to prepare the muondata
	   object before CreateDigits() is called.
	   If the initialization was successful then kTRUE should be returned otherwise
	   kFALSE should be returned. 
	 */
	virtual Bool_t InitOutputData(AliMUONLoader* muonloader) = 0;

	/* When all the data is added to the muondata object and the trees need to be
	   filled then this method is called by CreateDigits(). 
	   Thus code like
	       muondata->Fill("D")
	   should go into this method.
	 */
	virtual void FillOutputData() = 0;
	
	/* This method should be overridden to undo/cleanup what was done in the 
	   InitOutputData method call.
	 */
	virtual void CleanupOutputData(AliMUONLoader* muonloader) = 0;

	/* This is called by CreateDigits when it wants the Signal value that will be written
	   to the output stream. Inheriting digitizers can override this to apply some kind 
	   of detector response.
	 */
	virtual Int_t GetSignalFrom(AliMUONTransientDigit* td) = 0;

	/* Should be overridden by inheriting digitizers such that this method adds the digits
	   to the correct tree. 
	 */
	virtual void AddDigit(Int_t chamber, Int_t tracks[kMAXTRACKS], Int_t charges[kMAXTRACKS], Int_t digits[6]) = 0;

	/* Should be called by GenerateTransientDigits() when a new transient digit is generated
	   form a source object from the input stream. The source object could be an AliMUONHit
	   or AliMUONSDigit for example.
	 */ 
	virtual void OnCreateTransientDigit(AliMUONTransientDigit* /*digit*/, TObject* /*source_object*/);

	/* Called by AddDigit(AliMUONTransientDigit*, Int_t) when transient digit is added to the 
	   muondata object ready for writing to the data trees.
	 */ 
	virtual void OnWriteTransientDigit(AliMUONTransientDigit* digit);
	
	// Wrapper method for AddDigit(Int_t, Int_t[kMAXTRACKS], Int_t[kMAXTRACKS], Int_t[6])
	void AddDigit(AliMUONTransientDigit* td, Int_t response_charge);

	// Creates a new fTDList object, and creates and fills the fHitMap arrays.
	// Note: this method assumes the array pointers are NULL when calling this method.
	void InitArrays();
	
	// Frees the memory allocated for fTDList and the fHitMap arrays.
	void CleanupArrays();

	/* Gets the run loader and muon loader pointers from the given folder name. If an error 
	   occurred then kFALSE is returned else kTRUE on success. 
	 */
	Bool_t FetchLoaders(const char* foldername, AliRunLoader*& runloader, AliMUONLoader*& muonloader);

	/* Gets the gAlice, and MUON module pointers from the specified run loader. If an error 
	   occurred then kFALSE is returned else kTRUE on success. 
	 */
	Bool_t FetchGlobalPointers(AliRunLoader* runloader);

	// Adds the transient digit uniquely to the fTDList.
	void AddOrUpdateTransientDigit(AliMUONTransientDigit* mTD);
	
	// Updates a TransientDigit in fTDList
	void UpdateTransientDigit(AliMUONTransientDigit * mTD);
	
	// Adds the new TransientDigit to fTDList
	void AddTransientDigit(AliMUONTransientDigit * mTD);

	// Verify that a TransientDigit already exists.
	Bool_t ExistTransientDigit(AliMUONTransientDigit * mTD); 

	// Sorts the 3 most significant tracks.    
	void SortTracks(Int_t *tracks, Int_t *charges, Int_t ntr);


	AliRunLoader* runloader;        //! Global run loader.
	AliMUONLoader* gime;            //! MUON specific loader.
	AliMUON* pMUON;                 //! Pointer to MUON module.
	AliMUONData* muondata;          //! muon data interface

	AliMUONHitMapA1 **fHitMap;      //! pointer to array of pointers to hitmaps
	TObjArray *fTDList;             //! list of AliMUONTransientDigits
	Int_t fTDCounter;               //! nr. of AliMUONTransientDigit
	Int_t fMask;                    //! mask dependent on input file
	Bool_t fSignal;                 //! kTRUE if signal file is processed 

private:

	Int_t fDebug;                   //! Debug level.

	ClassDef(AliMUONDigitizer, 1)   // MUON merging/digitization
};    
#endif

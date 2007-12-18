#ifndef ALIMUONRAWSTREAMTRACKERHP_H
#define ALIMUONRAWSTREAMTRACKERHP_H
/* This file is property of and copyright by the ALICE HLT Project        *
 * ALICE Experiment at CERN, All rights reserved.                         *
 * See cxx source for full Copyright notice                               */

/* $Id$*/

///
/// \file   AliMUONRawStreamTrackerHP.h
/// \author Artur Szostak <artursz@iafrica.com>
/// \date   29-11-2007
/// \brief  Declaration of the high performance decoder for muon trigger chamber raw streams.
///

#include "AliMUONVRawStreamTracker.h"
#include "AliMUONTrackerDDLDecoder.h"

class AliMUONRawStreamTrackerHP : public AliMUONVRawStreamTracker
{
public:
	class AliDspHeader;
	class AliBusPatch;

	/// Default constructor.
	AliMUONRawStreamTrackerHP();
	
	/// Constructor for setting the raw reader.
	AliMUONRawStreamTrackerHP(AliRawReader* rawReader);
	
	/// Default destructor.
	virtual ~AliMUONRawStreamTrackerHP();
	
	// The following public methods are all inherited from AliMUONVRawStreamTracker:
	
	/// Initialize iterator
	virtual void First();
	
	/// DDL iterator 
	virtual Bool_t NextDDL();
	
	/// Whether the iteration is finished or not
	virtual Bool_t IsDone() const;
	
	/// Nothing is actually done in the AddErrorMessage method because we log
	/// the error messages as we find them in AliDecoderEventHandler::OnError().
	virtual void AddErrorMessage() { };
	
	/// Advance one step in the iteration. Returns false if finished.
	virtual Bool_t Next(Int_t& busPatchId,
				UShort_t& manuId, UChar_t& manuChannel,
				UShort_t& adc);
	
	/// Returns the next batch of decoded channel data.
	const AliBusPatch* Next()
	{
		do {
			if (fCurrentBusPatch != fDecoder.GetHandler().EndOfBusPatch()) return fCurrentBusPatch++;
		} while (NextDDL());
		return NULL;
	}
	
	/// Return maximum number of blocks per DDL allowed.
	virtual Int_t GetMaxBlock() const { return (Int_t) fDecoder.MaxBlocks(); }
	/// Return maximum number of Dsp per block allowed.
	virtual Int_t GetMaxDsp() const { return (Int_t) fDecoder.MaxDSPs(); }
	/// Return maximum number of Buspatch per Dsp allowed.
	virtual Int_t GetMaxBus() const { return (Int_t) fDecoder.MaxBusPatches(); }
	
	/// Set maximum number of blocks per DDL allowed.
	virtual void SetMaxBlock(Int_t blk);
	/// Set maximum number of Dsp per block allowed.
	virtual void SetMaxDsp(Int_t dsp);
	/// Set maximum number of Buspatch per Dsp allowed.
	virtual void SetMaxBus(Int_t bus);
	
	/// Return number of the current DDL.
	virtual Int_t GetDDL() const { return fDDL - 1; }
	
	/// check error/Warning presence
	virtual Bool_t IsErrorMessage() const { return fHadError; }
	
	/// Get number of parity errors
	Int_t   GetParityErrors() const
	{
		return (Int_t) fDecoder.GetHandler().ParityErrorCount();
	}

	/// Get number of glitch errors
	Int_t   GetGlitchErrors() const
	{
		return (Int_t) fDecoder.GetHandler().GlitchErrorCount();
	}

	/// Get number of padding word errors
	Int_t   GetPaddingErrors() const
	{
		return (Int_t) fDecoder.GetHandler().PaddingErrorCount();
	}

	/// Set warnings flag to disable warnings on data errors.
	void DisableWarnings() { fDecoder.GetHandler().Warnings(kFALSE); }
	/// Set warnings flag to enable warnings on data errors.
	void EnableWarnings() { fDecoder.GetHandler().Warnings(kTRUE); }

	/// Returns the "try to recover from errors" flag.
	Bool_t TryRecover() const { return Bool_t(fDecoder.TryRecover()); }
	
	/// Sets the "try to recover from errors" flag.
	/// i.e. should the decoder try to recover from errors found in the
	/// payload headers.
	void TryRecover(Bool_t value) { fDecoder.TryRecover(bool(value)); }
	
	/// Light weight interface class to the block header data.
	class AliBlockHeader
	{
	public:
		/// Default constructor.
		AliBlockHeader(
				AliDspHeader* dspArray = NULL,
				const AliMUONBlockHeaderStruct* header = NULL
			)
			: fNext(NULL), fDspCount(0), fFirstDsp(dspArray), fHeader(header)
		{
		}
		
		/// Implement shallow copying in the copy constructor.
		AliBlockHeader(const AliBlockHeader& o) :
			fNext(o.fNext), fDspCount(o.fDspCount), fFirstDsp(o.fFirstDsp), fHeader(o.fHeader)
		{
		}
		
		/// Implement shallow copying in the assignment operator.
		AliBlockHeader& operator = (const AliBlockHeader& object)
		{
			memcpy(this, &object, sizeof(AliBlockHeader));
			return *this;
		}
	
		/// Default destructor.
		~AliBlockHeader() {};
	
		/// Return data key word for CRT header
		Int_t   GetDataKey()        const {assert(fHeader != NULL); return fHeader->fDataKey;}
		/// Return total length of block structure (w/o padding word)
		Int_t   GetTotalLength()    const {assert(fHeader != NULL); return fHeader->fTotalLength;}
		/// Return length of raw data
		Int_t   GetLength()         const {assert(fHeader != NULL); return fHeader->fLength;}
		/// Return Dsp id
		Int_t   GetDspId()          const {assert(fHeader != NULL); return fHeader->fDSPId;}
		/// Return L0 trigger word
		Int_t   GetL0Trigger()      const {assert(fHeader != NULL); return fHeader->fL0Trigger;}
		/// Return Bunch Crossing for mini-event id (see TDR chapter 8)
		Int_t   GetMiniEventId()    const {assert(fHeader != NULL); return fHeader->fMiniEventId;}
		/// Return Event Id in bunch crossing
		Int_t   GetEventId1()       const {assert(fHeader != NULL); return fHeader->fEventId1;}
		/// Return Event Id in orbit number
		Int_t   GetEventId2()       const {assert(fHeader != NULL); return fHeader->fEventId2;}
	
		/// Return the header's raw data.
		const AliMUONBlockHeaderStruct* GetHeader() {return fHeader;}
		
		/// Return the next block header.
		const AliBlockHeader* Next() const { return fNext; }
		
		/// Returns the first AliDspHeader class in this block.
		const AliDspHeader* GetFirstDspHeader() const { return fFirstDsp; }
		
		/// Returns the number of DSPs within this block.
		UInt_t GetDspCount() const { return fDspCount; }
	
		/// Return the i'th DSP in this block.
		const AliDspHeader* GetDspHeader(UInt_t i) const
		{
			return i < fDspCount ? GetFirstDspHeader() + i : NULL;
		}
		
		/// Sets the next block header.
		void SetNext(const AliBlockHeader* next) { fNext = next; }

		/// Increments the DSP count.
		void IncDspCount() { fDspCount++; };
		
		/// Print the contents of the header to screen.
		void Print() const;
	
	private:
	
		const AliBlockHeader* fNext;  ///< Pointer to next block.
		UInt_t fDspCount;    ///< The number of AliDspHeader objects found in the array pointed to by fFirstDsp.
		const AliDspHeader* fFirstDsp;  ///< The first DSP associated with this block.
		const AliMUONBlockHeaderStruct*  fHeader;  ///< Pointer to header in DDL payload.
	};
	
	/// Light weight interface class to the DSP header data.
	class AliDspHeader
	{
	public:
		/// Default constructor.
		AliDspHeader(
				const AliBlockHeader* block = NULL,
				const AliBusPatch* busPatchArray = NULL,
				const AliMUONDSPHeaderStruct* header = NULL
			) :
			fBlock(block), fNext(NULL), fBusPatchCount(0),
			fFirstBusPatch(busPatchArray), fHeader(header)
		{
		}
		
		/// Implement shallow copying in the copy constructor.
		AliDspHeader(const AliDspHeader& o) :
			fBlock(o.fBlock), fNext(o.fNext), fBusPatchCount(o.fBusPatchCount),
			fFirstBusPatch(o.fFirstBusPatch), fHeader(o.fHeader)
		{
		}
		
		/// Implement shallow copying in the assignment operator.
		AliDspHeader& operator = (const AliDspHeader& object)
		{
			memcpy(this, &object, sizeof(AliDspHeader));
			return *this;
		}
	
		/// Default destructor.
		~AliDspHeader() {};
	
		/// Return Data key word for FRT header
		Int_t   GetDataKey()        const {assert(fHeader != NULL); return fHeader->fDataKey;}
		/// Return total length of block structure
		Int_t   GetTotalLength()    const {assert(fHeader != NULL); return fHeader->fTotalLength;}
		/// Return length of raw data
		Int_t   GetLength()         const {assert(fHeader != NULL); return fHeader->fLength;}
		/// Return Dsp id
		Int_t   GetDspId()          const {assert(fHeader != NULL); return fHeader->fDSPId;}
		/// Return L1 accept in Block Structure (CRT)
		Int_t   GetBlkL1ATrigger()  const {assert(fHeader != NULL); return fHeader->fBlkL1ATrigger;}
		/// Return Mini Event Id in bunch crossing
		Int_t   GetMiniEventId()    const {assert(fHeader != NULL); return fHeader->fMiniEventId;}
		/// Return Number of L1 accept in DSP Structure (FRT)
		Int_t   GetL1ATrigger()     const {assert(fHeader != NULL); return fHeader->fL1ATrigger;}
		/// Return Number of L1 reject in DSP Structure (FRT)
		Int_t   GetL1RTrigger()     const {assert(fHeader != NULL); return fHeader->fL1RTrigger;}
		/// Return padding dummy word for 64 bits transfer
		UInt_t  GetPaddingWord()    const {assert(fHeader != NULL); return fHeader->fPaddingWord;}
		/// Return Error word
		Int_t   GetErrorWord()      const {assert(fHeader != NULL); return fHeader->fErrorWord;}
	
		/// Return raw data of header
		const AliMUONDSPHeaderStruct* GetHeader() {return fHeader;}
		
		/// Return the parent block header.
		const AliBlockHeader* GetBlockHeader() const { return fBlock; }
		
		/// Return the next DSP header.
		const AliDspHeader* Next() const { return fNext; }
		
		/// Returns the first AliBusPatch class in this DSP.
		const AliBusPatch* GetFirstBusPatch() const { return fFirstBusPatch; }
		
		/// Returns the number of bus patches within this DSP.
		UInt_t GetBusPatchCount() const { return fBusPatchCount; }
	
		/// Return the i'th bus patch in this DSP.
		const AliBusPatch* GetBusPatch(UInt_t i) const
		{
			return i < fBusPatchCount ? GetFirstBusPatch() + i : NULL;
		}
	
		/// Sets the next DSP header.
		void SetNext(const AliDspHeader* next) { fNext = next; }

		/// Increments the bus patch count.
		void IncBusPatchCount() { fBusPatchCount++; };
		
		/// Print the contents of the header to screen.
		void Print() const;
	
	private:
	
		const AliBlockHeader* fBlock;  ///< Pointer to parent block structure.
		const AliDspHeader* fNext;  ///< Pointer to next DSP.
		UInt_t fBusPatchCount;    ///< The number of AliDspHeader objects found in the array pointed to by fFirstBusPatch
		const AliBusPatch* fFirstBusPatch;  ///< The first bus patch of this DSP.
		const AliMUONDSPHeaderStruct*  fHeader;  ///< Pointer to header in DDL payload.
	};
	
	/// Light weight interface class to the bus patch data.
	class AliBusPatch
	{
	public:
		/// Default constructor.
		AliBusPatch(
				const AliDspHeader* dsp = NULL,
				const AliMUONBusPatchHeaderStruct* header = NULL,
				const UInt_t* data = NULL,
				const Bool_t* parityOk = NULL
			) :
			fDSP(dsp),
			fNext(NULL),
			fHeader(header),
			fData(data),
			fParityOk(parityOk)
		{
		}
		
		/// Implement shallow copying in the copy constructor.
		AliBusPatch(const AliBusPatch& o) :
			fDSP(o.fDSP),
			fNext(o.fNext),
			fHeader(o.fHeader),
			fData(o.fData),
			fParityOk(o.fParityOk)
		{
		}
		
		/// Implement shallow copying in the assignment operator.
		AliBusPatch& operator = (const AliBusPatch& object)
		{
			memcpy(this, &object, sizeof(AliBusPatch));
			return *this;
		}
	
		/// Default destructor.
		~AliBusPatch() {};
		
		/// Return Data key word for bus patch header.
		Int_t   GetDataKey()     const {assert(fHeader != NULL); return fHeader->fDataKey;}
		/// Return total length of buspatch structure
		Int_t   GetTotalLength() const {assert(fHeader != NULL); return fHeader->fTotalLength;}
		/// Return length of raw data
		Int_t   GetLength()      const {assert(fHeader != NULL); return fHeader->fLength;}
		/// Return bus patch id
		Int_t   GetBusPatchId()  const {assert(fHeader != NULL); return fHeader->fBusPatchId;}

		/// Return raw data of header
		const AliMUONBusPatchHeaderStruct* GetHeader() {return fHeader;}
		/// Return raw digit data
		const UInt_t* GetData()  const {return fData;}
		/// Returns the number of raw data words within this bus patch.
		UInt_t GetDataCount() const { return (UInt_t)GetLength(); }

		/// Returns the parity bit of the n'th raw data word.
		Char_t GetParity(UInt_t n) const
		{
			assert( fHeader != NULL and n < fHeader->fLength );
			return (Char_t)(fData[n] >> 31) &  0x1;
		}
		
		/// Returns the MANU ID of the n'th raw data word.
		UShort_t GetManuId(UInt_t n) const
		{
			assert( fHeader != NULL and n < fHeader->fLength );
			return (UShort_t)(fData[n] >> 18) &  0x7FF;
		}
		
		/// Returns the channel ID of the n'th raw data word.
		UChar_t GetChannelId(UInt_t n) const
		{
			assert( fHeader != NULL and n < fHeader->fLength );
			return (Char_t)(fData[n] >> 12) & 0x3F;
		}
		
		/// Returns the charge/signal of the n'th raw data word.
		UShort_t GetCharge(UInt_t n) const
		{
			assert( fHeader != NULL and n < fHeader->fLength );
			return (UShort_t)(fData[n] & 0xFFF);
		}
		
		/// Returns the n'th raw data word.
		UInt_t GetData(UInt_t n) const
		{
			assert( fHeader != NULL and n < fHeader->fLength );
			return fData[n];
		}
		
		/// Returns kTRUE if the parity of the n'th raw data word is OK
		/// and kFALSE otherwise.
		Bool_t IsParityOk(UInt_t n) const
		{
			assert( fHeader != NULL and n < fHeader->fLength );
			return fParityOk[n];
		}
		
		/// Unpacks and returns the fields of the n'th raw data word. kTRUE
		/// is returned if the data word's parity was OK and kFALSE otherwise.
		Bool_t GetData(UInt_t n, UShort_t& manuId, UChar_t& channelId, UShort_t& adc) const
		{
			assert( fHeader != NULL and n < fHeader->fLength );
			AliMUONTrackerDDLDecoderEventHandler::UnpackADC(fData[n], manuId, channelId, adc);
			return fParityOk[n];
		}
		
		/// Return the parent block header.
		const AliDspHeader* GetDspHeader() const { return fDSP; }
		
		/// Return the next bus patch header.
		const AliBusPatch* Next() const { return fNext; }
		
		/// Sets the next bus patch.
		void SetNext(const AliBusPatch* next) { fNext = next; }
		
		/// Print the contents of the bus patch to screen.
		void Print(const Option_t* opt = "") const;
	
	private:
	
		const AliDspHeader* fDSP;   ///< The DSP this bus patch belongs to.
		const AliBusPatch* fNext;  ///< Next bus patch object in the DSP.
		const AliMUONBusPatchHeaderStruct*  fHeader;  ///< Pointer to bus patch in DDL payload.
		const UInt_t* fData;  ///< Pointer to the bus patch data.
		const Bool_t* fParityOk;  ///< Array of flags indicating if the parity of the given data word in fData is good or not.		
	};
	
	/// Return the number of blocks in the DDL payload.
	UInt_t GetBlockCount() const
	{
		return fDecoder.GetHandler().BlockCount();
	}
	
	/// Return the first block header.
	const AliBlockHeader* GetFirstBlockHeader() const
	{
		return fDecoder.GetHandler().BlockHeader(0);
	}
	
	/// Return the i'th block header or NULL if not found.
	const AliBlockHeader* GetBlockHeader(UInt_t i) const
	{
		return fDecoder.GetHandler().BlockHeader(i);
	}
	
	/// Returns the number of DSPs for the given block number.
	UInt_t GetDspCount(UInt_t block) const
	{
		const AliBlockHeader* b = GetBlockHeader(block);
		return b != NULL ? b->GetDspCount() : 0;
	}
	
	/// Returns the i'th DSP header for the given block number or NULL if not found.
	const AliDspHeader* GetDspHeader(UInt_t block, UInt_t i) const
	{
		const AliBlockHeader* b = GetBlockHeader(block);
		return b != NULL ? b->GetDspHeader(i) : NULL;
	}
	
	/// Returns the number of bus patches for the given block and dsp number.
	UInt_t GetBusPatchCount(UInt_t block, UInt_t dsp) const
	{
		const AliDspHeader* d = GetDspHeader(block, dsp);
		return d != NULL ? d->GetBusPatchCount() : 0;
	}
	
	/// Returns the i'th bus patch for the given block and dsp.
	const AliBusPatch* GetBusPatch(UInt_t block, UInt_t dsp, UInt_t i) const
	{
		const AliDspHeader* d = GetDspHeader(block, dsp);
		return d != NULL ? d->GetBusPatch(i) : NULL;
	}

	/// Returns the current bus patch being decoded or NULL if none found.
	const AliBusPatch* CurrentBusPatch() const
	{
		return (fCurrentBusPatch != fDecoder.GetHandler().EndOfBusPatch()) ?
			fCurrentBusPatch : NULL;
	}

	/// Returns the current DSP being decoded or NULL if none found.
	const AliDspHeader* CurrentDspHeader() const
	{
		const AliBusPatch* busPatch = CurrentBusPatch();
		return (busPatch != NULL) ? busPatch->GetDspHeader() : NULL;
	}

	/// Returns the current block header being decoded or NULL if none found.
	const AliBlockHeader* CurrentBlockHeader() const
	{
		const AliDspHeader* dsp = CurrentDspHeader();
		return (dsp != NULL) ? dsp->GetBlockHeader() : NULL;
	}

private:

	// Do not allow copying of this class.
        /// Not implemented
	AliMUONRawStreamTrackerHP(const AliMUONRawStreamTrackerHP& stream);
        /// Not implemented
	AliMUONRawStreamTrackerHP& operator = (const AliMUONRawStreamTrackerHP& stream);
	
	/// This is the custom event handler (callback interface) class which
	/// unpacks raw data words and fills an internal buffer with decoded digits
	/// as they are decoded by the high performance decoder.
	/// Any errors are logged to the parent AliMUONVRawStreamTracker, so one
	/// must set this pointer appropriately before decoding and DDL payload.
	class AliDecoderEventHandler : public AliMUONTrackerDDLDecoderEventHandler
	{
	public:
	
		/// Default constructor.
		AliDecoderEventHandler();
		/// Default destructor.
		virtual ~AliDecoderEventHandler();
		
		/// Sets the internal arrays based on the maximum number of structures allowed.
		void SetMaxStructs(UInt_t maxBlocks, UInt_t maxDsps, UInt_t maxBusPatches);
		
		/// Sets the raw stream object which should be the parent of this class.
		void SetRawStream(AliMUONVRawStreamTracker* rawStream) { fRawStream = rawStream; }
		
		/// Return the number of blocks found in the payload.
		UInt_t BlockCount() const { return fBlockCount; };
		
		/// Return the i'th block structure.
		const AliBlockHeader* BlockHeader(UInt_t i) const
		{
			return i < fBlockCount ? &fBlocks[i] : NULL;
		}

		/// Return the first bus patch decoded.
		const AliBusPatch* FirstBusPatch() const { return fBusPatches; }

		/// Returns the marker to the end of bus patches. i.e. one position past the last bus patch.
		const AliBusPatch* EndOfBusPatch() const { return fEndOfBusPatches; }

		/// Returns the number of parity errors found in the DDL.
		UInt_t ParityErrorCount() const { return fParityErrors; }
		/// Returns the number of glitch errors found in the DDL.
		UInt_t GlitchErrorCount() const { return fGlitchErrors; }
		/// Returns the number of padding errors found in the DDL.
		UInt_t PaddingErrorCount() const { return fPaddingErrors; }

		/// Returns the warnings flag.
		Bool_t Warnings() const { return fWarnings; }
		/// Sets the warnings flag.
		void Warnings(Bool_t value) { fWarnings = value; }
		
		// The following methods are inherited from AliMUONTrackerDDLDecoderEventHandler:
		
		/// New buffer handler.
		void OnNewBuffer(const void* buffer, UInt_t bufferSize);

		/// End of buffer handler marks the end of bus patches.
		void OnEndOfBuffer(const void* /*buffer*/, UInt_t /*bufferSize*/)
		{
			fEndOfBusPatches = fCurrentBusPatch+1;
		}
		
		/// New block handler is called by the decoder whenever a new block
		/// structure is found. We just mark the new block and increment the
		/// internal counter.
		void OnNewBlock(const AliMUONBlockHeaderStruct* header, const void* /*data*/)
		{
			assert( fBlockCount < (UInt_t)fRawStream->GetMaxBlock() and header != NULL );
			if (fBlockCount > 0) fCurrentBlock->SetNext(fCurrentBlock+1);  // Link the block unless it is the first one.
			*(++fCurrentBlock) = AliBlockHeader(fCurrentDSP+1, header);
			fBlockCount++;
		}
		
		/// New DSP handler is called by the decoder whenever a new DSP
		/// structure is found. We just mark the DSP and increment the
		/// appropriate counters.
		void OnNewDSP(const AliMUONDSPHeaderStruct* header, const void* /*data*/)
		{
			assert( fCurrentBlock->GetDspCount() < (UInt_t)fRawStream->GetMaxDsp() and header != NULL );
			if (fCurrentBlock->GetDspCount() > 0) fCurrentDSP->SetNext(fCurrentDSP+1); // Link the DSP unless it is the first one.
			*(++fCurrentDSP) = AliDspHeader(fCurrentBlock, fCurrentBusPatch+1, header);
			fCurrentBlock->IncDspCount();
		}
		
		/// New bus patch handler.
		/// This is called by the high performance decoder when a new bus patch
		/// is found within the DDL payload.
		void OnNewBusPatch(const AliMUONBusPatchHeaderStruct* header, const void* data)
		{
			assert( fCurrentDSP->GetBusPatchCount() < (UInt_t)fRawStream->GetMaxBus() and header != NULL and data != NULL );
			if (fCurrentDSP->GetBusPatchCount() > 0) fCurrentBusPatch->SetNext(fCurrentBusPatch+1); // Link the bus patch unless it is the first one.
			*(++fCurrentBusPatch) = AliBusPatch(fCurrentDSP, header, reinterpret_cast<const UInt_t*>(data), fCurrentParityOkFlag+1);
			fCurrentDSP->IncBusPatchCount();
		}
		
		/// Raw data word handler.
		void OnData(UInt_t /*data*/, bool parityError)
		{
			assert( fCurrentParityOkFlag < fParityOk + fMaxChannels );
			*(++fCurrentParityOkFlag) = Bool_t(not parityError);
		}
		
		/// Error handler.
		void OnError(ErrorCode error, const void* location);
	
	private:
	
		// Do not allow copying of this class.
                /// Not implemented
		AliDecoderEventHandler(const AliDecoderEventHandler& /*obj*/);
                /// Not implemented
		AliDecoderEventHandler& operator = (const AliDecoderEventHandler& /*obj*/);

		AliMUONVRawStreamTracker* fRawStream; //!< Pointer to the parent raw stream object.
		const void* fBufferStart;   //!< Pointer to the start of the current DDL payload buffer.
		UInt_t fBlockCount;  //!< Number of blocks filled in fBlocks.
		AliBlockHeader* fBlocks;  //!< Array of blocks. [0..fMaxBlocks-1]
		AliDspHeader* fDSPs;      //!< Array of DSPs. [0..fMaxDsps*fMaxBlocks-1]
		AliBusPatch* fBusPatches; //!< Array of bus patches. [0..fMaxBusPatches*fMaxDsps*fMaxBlocks-1]
		AliBusPatch* fEndOfBusPatches;  //!< Marks the last bus patch.
		UInt_t fMaxChannels;   //!< Maximum number of elements that can be stored in fParityOk.
		Bool_t* fParityOk;     //!< Array of flags for indicating if the parity is good for a raw data word.
		AliBlockHeader* fCurrentBlock;  //!< Current block in fBlocks.
		AliDspHeader* fCurrentDSP;      //!< Current DSP in fDSPs.
		AliBusPatch* fCurrentBusPatch;  //!< Current bus patch in fBusPatches.
		Bool_t* fCurrentParityOkFlag;  //!< Current parity flag to be set in fParityOk.
		UInt_t fParityErrors;   //!< Number of parity errors found in DDL.
		UInt_t fGlitchErrors;   //!< Number of glitch errors found in DDL.
		UInt_t fPaddingErrors;  //!< Number of padding errors found in DDL.
		Bool_t fWarnings;       //!< Flag indicating if we should generate a warning for errors.
	};
	
	AliMUONTrackerDDLDecoder<AliDecoderEventHandler> fDecoder;  //!< The decoder for the DDL payload.
	Int_t fDDL;         //!< The current DDL number being handled.
	Int_t fBufferSize;  //!< This is the buffer size in bytes of fBuffer.
	UChar_t* fBuffer;   //!< This is the buffer in which we store the DDL payload read from AliRawReader.
	const AliBusPatch* fCurrentBusPatch;  //!< The current data word to return by Next().
	const UInt_t* fCurrentData;  //!< The current data word to return by Next().
	const UInt_t* fEndOfData;  //!< The last data word in the current bus patch.
	Bool_t fHadError;   //!< Flag indicating if there was a decoding error or not.
	Bool_t fDone;       //!< Flag indicating if the iteration is done or not.

	ClassDef(AliMUONRawStreamTrackerHP, 0) // High performance decoder for reading MUON raw digits from tracking chamber DDL data.
};

#endif  // ALIMUONRAWSTREAMTRACKERHP_H


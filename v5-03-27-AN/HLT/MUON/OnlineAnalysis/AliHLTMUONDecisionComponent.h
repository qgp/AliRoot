#ifndef AliHLTMUONDECISIONCOMPONENT_H
#define AliHLTMUONDECISIONCOMPONENT_H
/* This file is property of and copyright by the ALICE HLT Project        *
 * ALICE Experiment at CERN, All rights reserved.                         *
 * See cxx source for full Copyright notice                               */

// $Id: $

///
///  @file   AliHLTMUONDecisionComponent.h
///  @author Artur Szostak <artursz@iafrica.com>
///  @date   30 April 2008
///  @brief  Declares the decision component for dimuon HLT triggering.
///

#include "AliHLTMUONProcessor.h"
#include "AliHLTMUONDataTypes.h"

#if __GNUC__ && __GNUC__ < 3
#define std
#endif

extern "C" struct AliHLTMUONTrackStruct;
extern "C" struct AliHLTMUONMansoTrackStruct;
extern "C" struct AliHLTMUONTrackDecisionStruct;
extern "C" struct AliHLTMUONPairDecisionStruct;
extern "C" struct AliHLTMUONSinglesDecisionBlockStruct;
extern "C" struct AliHLTMUONPairsDecisionBlockStruct;


/**
 * @class AliHLTMUONDecisionComponent
 * @brief Dimuon HLT trigger decision component.
 *
 * This class implements the dimuon HLT trigger decision component.
 * The dimuon trigger decision is generated by first applying two pT cuts to the
 * single tracks: a low cut for the J/psi family and a high cut for the upsilon
 * family. From the tracks that pass the cuts, we count them and build up some
 * statistics like the number of tracks passing the low or high pT cut.
 * The algorithm then looks at pairs of tracks and similarly counts the number
 * like sign or unlike sign pairs where both tracks passed the low or high pT cut
 * or pairs that did not pass any cuts.
 * At this point the invariant mass of the pairs is calculated and two mass cuts
 * are applied. The number of pairs that pass the low or high mass cut are then
 * counted. The results are encoded into two data blocks, one for trigger decisions
 * for single tracks and another for the track pairs.
 *
 * <h2>General properties:</h2>
 *
 * Component ID: \b MUONDecisionComponent <br>
 * Library: \b libAliHLTMUON.so <br>
 * Input Data Types: \li AliHLTMUONConstants::MansoTracksBlockDataType() = "MANTRACK:MUON" <br>
 *                   \li AliHLTMUONConstants::TracksBlockDataType() = "TRACKS  :MUON" <br>
 * Output Data Types: \li AliHLTMUONConstants::SinglesDecisionBlockDataType() = "DECIDSIN:MUON"
 *                    \li AliHLTMUONConstants::PairsDecisionBlockDataType() = "DECIDPAR:MUON" <br>
 *
 * <h2>Mandatory arguments:</h2>
 * None.
 *
 * <h2>Optional arguments:</h2>
 * <!-- NOTE: ignore the \li. <i> and </i>: it's just doxygen formatting -->
 * \li -lowptcut <i>value</i> <br>
 *      Sets the low pT cut value to use when applying the decision. The <i>value</i>
 *      should be a floating point number and has units GeV/c. If this parameter is
 *      specified then it will not be loaded from CDB. <br>
 * \li -highptcut <i>value</i> <br>
 *      Sets the high pT cut value to use when applying the decision. The <i>value</i>
 *      should be a floating point number and has units GeV/c. If this parameter is
 *      specified then it will not be loaded from CDB. <br>
 * \li -lowmasscut <i>value</i> <br>
 *      Sets the low invariant mass cut value to use when applying the decision.
 *      The <i>value</i> should be a floating point number and has units GeV/c^2.
 *      If this parameter is specified then it will not be loaded from CDB. <br>
 * \li -highmasscut <i>value</i> <br>
 *      Sets the high invariant mass cut value to use when applying the decision.
 *      The <i>value</i> should be a floating point number and has units GeV/c^2.
 *      If this parameter is specified then it will not be loaded from CDB. <br>
 * \li -no_singles_detail <br>
 *      If specified the detailed decision information for tracks is not added to
 *      the output. Only the scalar values are then present in the output data block
 *      for decisions on single tracks. <br>
 * \li -no_pairs_detail <br>
 *      If specified the detailed decision information for track pairs is not added
 *      to the output. Only the scalar values are then present in the output data
 *      block for decisions for track pairs. <br>
 * \li -warn_on_unexpected_block <br>
 *      This will cause the component to generate warnings when it receives data block
 *      types it does not know how to handle. Without this option the component only
 *      generates debug messages when they are compiled in. <br>
 * \li -cdbpath <i>path</i> <br>
 *      This allows one to override the path to use for the CDB location.
 *      <i>path</i> must be a valid CDB URI. By default the HLT system framework
 *      sets the CDB path. <br>
 * \li -run <i>number</i> <br>
 *      This allows one to override the run number to use. <i>number</i> must be
 *      a positive integer number. By default the HLT system framework sets the
 *      run number. <br>
 * \li -delaysetup <br>
 *      If indicated then part of the initialisation of the component is forcefully
 *      delayed to the first event received, i.e. the Start-of-Run event. <br>
 * \li -dumponerror <br>
 *      This flag will cause the component to dump the data blocks it received if
 *      an error occurs during the processing of an event. <br>
 * \li -dumppath <i>path</i> <br>
 *      Allows one to specify the path in which to dump the received data blocks
 *      if an error occurs. <br>
 *
 * <h2>Standard configuration:</h2>
 * The configuration is taken from the CDB by default. It can be overridden with
 * the command line arguments.
 *
 * <h2>Default CDB entries:</h2>
 * HLT/ConfigMUON/DecisionComponent - Contains a TMap with the cut parameters.
 *
 * <h2>Performance:</h2>
 * For worst case numbers of tracks the decision component requires less than a
 * millisecond to process an event.
 *
 * <h2>Memory consumption:</h2>
 * This is a linear function of the input data size, but only a fraction. Thus the
 * memory usage is minimal. It should be under 1 MBytes.
 *
 * <h2>Output size:</h2>
 * This will depend linearly on the number of tracks found. But for nominal
 * multiplicities this should be less than 16 kBytes.
 *
 * @ingroup alihlt_dimuon_component
 */
class AliHLTMUONDecisionComponent : public AliHLTMUONProcessor
{
public:
	AliHLTMUONDecisionComponent();
	virtual ~AliHLTMUONDecisionComponent();

	// Public functions to implement the AliHLTProcessor interface.
	// These functions are required for the registration process.
	virtual const char* GetComponentID();
	virtual void GetInputDataTypes(AliHLTComponentDataTypeList& list);
	virtual AliHLTComponentDataType GetOutputDataType();
	virtual int GetOutputDataTypes(AliHLTComponentDataTypeList& list);
	virtual void GetOutputDataSize(unsigned long& constBase, double& inputMultiplier);
	virtual AliHLTComponent* Spawn();

protected:

	// Protected functions to implement the AliHLTProcessor interface.
	// These functions provide initialization as well as the actual processing
	// capabilities of the component.
	virtual int DoInit(int argc, const char** argv);
	virtual int DoDeinit();
	virtual int Reconfigure(const char* cdbEntry, const char* componentId);
	virtual int ReadPreprocessorValues(const char* modules);
	virtual int DoEvent(
			const AliHLTComponentEventData& evtData,
			const AliHLTComponentBlockData* blocks,
			AliHLTComponentTriggerData& trigData,
			AliHLTUInt8_t* outputPtr,
			AliHLTUInt32_t& size,
			AliHLTComponentBlockDataList& outputBlocks
		);
	
	using AliHLTProcessor::DoEvent;

private:

	// Do not allow copying of this class.
	AliHLTMUONDecisionComponent(const AliHLTMUONDecisionComponent& /*obj*/);
	AliHLTMUONDecisionComponent& operator = (const AliHLTMUONDecisionComponent& /*obj*/);
	
	/**
	 * Reads the cut parameters from the CDB.
	 * \param setLowPtCut  Indicates if the low pT cut should be set (default true).
	 * \param setHighPtCut  Indicates if the high pT cut should be set (default true).
	 * \param setLowMassCut  Indicates if the low invariant mass cut should be set (default true).
	 * \param setHighMassCut  Indicates if the high invariant mass cut should be set (default true).
	 * \return 0 is returned on success and a non-zero value to indicate failure.
	 */
	int ReadConfigFromCDB(
			bool setLowPtCut = true, bool setHighPtCut = true,
			bool setLowMassCut = true, bool setHighMassCut = true
		);
	
	/// Internal track information structure for the fTracks buffer.
	struct AliTrackInfo
	{
		AliHLTInt32_t fId;  /// Track ID.
		AliHLTFloat32_t fPx, fPy, fPz; /// Momentum vector.
		AliHLTMUONParticleSign fSign;  /// The particle's charge sign.
	};
	
	/**
	 * Creates a new element in fTracks and returns it.
	 * NULL is returned if no more memory could be allocated.
	 */
	AliTrackInfo* NewTrack();
	
	/// Adds Manso track information to the list of tracks to process.
	int AddTrack(const AliHLTMUONMansoTrackStruct* track);

	/// Adds track information from the full tracker component to the list of tracks to process.
	int AddTrack(const AliHLTMUONTrackStruct* track);
	
	int ApplyTriggerAlgorithm(
			AliHLTMUONSinglesDecisionBlockStruct& singlesHeader,
			AliHLTMUONTrackDecisionStruct* singlesDecision,
			AliHLTMUONPairsDecisionBlockStruct& pairsHeader,
			AliHLTMUONPairDecisionStruct* pairsDecision
		);
	
	AliHLTUInt32_t fMaxTracks; /// The maximum number of elements that can be stored in fTracks.
	AliHLTUInt32_t fTrackCount;  /// The current number of elements stored in fTracks.
	AliTrackInfo* fTracks;  /// Pointers to the Manso track structures in input data blocks.
	AliHLTFloat32_t fLowPtCut;  /// The low pT cut value to apply to tracks. [GeV/c]
	AliHLTFloat32_t fHighPtCut;  /// The high pT cut value to apply to tracks. [GeV/c]
	AliHLTFloat32_t fLowMassCut;  /// The low invariant mass cut value to apply to tracks. [GeV/c^2]
	AliHLTFloat32_t fHighMassCut;  /// The high invariant mass cut value to apply to tracks. [GeV/c^2]
	bool fWarnForUnexpecedBlock;  /// Flag indicating if we should log a warning if we got a block of an unexpected type.
	bool fLowPtCutSet; ///< Indicates if the low pT cut parameter was set on the command line.
	bool fHighPtCutSet; ///< Indicates if the high pT cut parameter was set on the command line.
	bool fLowMassCutSet; ///< Indicates if the low invariant mass cut parameter was set on the command line.
	bool fHighMassCutSet; ///< Indicates if the high invariant mass cut parameter was set on the command line.
	bool fFillSinglesDetail; ///< If true then detailed trigger decision information for single tracks is generated.
	bool fFillPairsDetail; ///< If true then detailed trigger decision information for track pairs is generated.
	
	ClassDef(AliHLTMUONDecisionComponent, 0);  // Trigger decision component for the dimuon HLT.
};

#endif // AliHLTMUONDECISIONCOMPONENT_H

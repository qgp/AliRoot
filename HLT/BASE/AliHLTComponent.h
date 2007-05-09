//-*- Mode: C++ -*-
// @(#) $Id$

#ifndef ALIHLTCOMPONENT_H
#define ALIHLTCOMPONENT_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/** @file   AliHLTComponent.h
    @author Matthias Richter, Timm Steinbeck
    @date   
    @brief  Base class declaration for HLT components. 
    @note   The class is both used in Online (PubSub) and Offline (AliRoot)
            context
                                                                          */
/**
 * @defgroup alihlt_component Component handling of the HLT module
 * This section describes the the component handling for the HLT module.
 */

#include <vector>
#include <string>
#include "AliHLTLogging.h"
#include "AliHLTDataTypes.h"
//#include "AliHLTDefinitions.h"

/* Matthias Dec 2006
 * The names have been changed for Aliroot's coding conventions sake
 * The old names are defined for backward compatibility with the 
 * stand alone SampleLib package
 */
typedef AliHLTComponentLogSeverity AliHLTComponent_LogSeverity;
typedef AliHLTComponentEventData AliHLTComponent_EventData;
typedef AliHLTComponentShmData AliHLTComponent_ShmData;
typedef AliHLTComponentDataType AliHLTComponent_DataType;
typedef AliHLTComponentBlockData AliHLTComponent_BlockData;
typedef AliHLTComponentTriggerData AliHLTComponent_TriggerData;
typedef AliHLTComponentEventDoneData AliHLTComponent_EventDoneData;

class AliHLTComponentHandler;
class TObjArray;
class TStopwatch;

/**
 * @class AliHLTComponent
 * Base class of HLT data processing components.
 * The class provides a common interface for HLT data processing components.
 * The interface can be accessed from the online HLT framework or the AliRoot
 * offline analysis framework.
 * @section alihltcomponent-properties Component identification and properties
 * Each component must provide a unique ID, input and output data type indications,
 * and a spawn function.
 * @subsection alihltcomponent-req-methods Required property methods
 * - @ref GetComponentID
 * - @ref GetInputDataTypes (see @ref alihltcomponent-type for default
 *   implementations.)
 * - @ref GetOutputDataType (see @ref alihltcomponent-type for default
 *   implementations.)
 * - @ref GetOutputDataSize (see @ref alihltcomponent-type for default
 *   implementations.)
 * - @ref Spawn
 *
 * @subsection alihltcomponent-opt-mehods Optional handlers
 * - @ref DoInit
 * - @ref DoDeinit
 *
 * @subsection alihltcomponent-processing-mehods Data processing
 * 
 * 
 * @subsection alihltcomponent-type Component type
 * Components can be of type
 * - @ref kSource     components which only produce data 
 * - @ref kProcessor  components which consume and produce data
 * - @ref kSink       components which only consume data
 *
 * where data production and consumption refer to the analysis data stream. In
 * order to indicate the type, a child component can overload the
 * @ref GetComponentType function.
 * @subsubsection alihltcomponent-type-std Standard implementations
 * Components in general do not need to implement this function, standard
 * implementations of the 3 types are available:
 * - AliHLTDataSource for components of type @ref kSource <br>
 *   All types of data sources can inherit from AliHLTDataSource and must
 *   implement the @ref AliHLTDataSource::GetEvent method. The class
 *   also implements a standard method for @ref GetInputDataTypes.
 *   
 * - AliHLTProcessor for components of type @ref kProcessor <br>
 *   All types of data processors can inherit from AliHLTDataSource and must
 *   implement the @ref AliHLTProcessor::DoEvent method.
 *
 * - AliHLTDataSink for components of type @ref kSink <br>
 *   All types of data processors can inherit from AliHLTDataSource and must
 *   implement the @ref AliHLTDataSink::DumpEvent method. The class
 *   also implements a standard method for @ref GetOutputDataType and @ref
 *   GetOutputDataSize.
 *
 * @subsection alihltcomponent-environment Running environment
 *
 * In order to adapt to different environments (on-line/off-line), the component
 * gets an environment structure with function pointers. The base class provides
 * member functions for those environment dependend functions. The member 
 * functions are used by the component implementation and are re-mapped to the
 * corresponding functions.
 * @section alihltcomponent-interfaces Component interfaces
 * Each of the 3 standard component base classes AliHLTProcessor, AliHLTDataSource
 * and AliHLTDataSink provides it's own processing method (see
 * @ref alihltcomponent-type-std), which splits into a high and a low-level
 * method. For the @ref alihltcomponent-low-level-interface, all parameters are
 * shipped as function arguments, the component is supposed to dump data to the
 * output buffer and handle all block descriptors. 
 * The @ref alihltcomponent-high-level-interface is the standard processing
 * method and will be used whenever the low-level method is not overloaded.
 *
 * @subsection alihltcomponent-high-level-interface High-level interface
 * The high-level component interface provides functionality to exchange ROOT
 * structures between components. In contrast to the 
 * @ref alihltcomponent-low-level-interface, a couple of functions can be used
 * to access data blocks of the input stream
 * and send data blocks or ROOT TObject's to the output stream. The functionality
 * is hidden from the user and is implemented by using ROOT's TMessage class.
 *
 * @subsubsection alihltcomponent-high-level-int-methods Interface methods
 * The interface provides a couple of methods in order to get objects from the
 * input, data blocks (non TObject) from the input, and to push back objects and
 * data blocks to the output. For convenience there are several functions of 
 * identical name (and similar behavior) with different parameters defined.
 * Please refer to the function documentation.
 * - @ref GetNumberOfInputBlocks <br>
 *        return the number of data blocks in the input stream
 * - @ref GetFirstInputObject <br>
 *        get the first object of a specific data type
 * - @ref GetNextInputObject <br>
 *        get the next object of same data type as last GetFirstInputObject/Block call
 * - @ref GetFirstInputBlock <br>
 *        get the first block of a specific data type
 * - @ref GetNextInputBlock <br>
 *        get the next block of same data type as last GetFirstInputBlock/Block call
 * - @ref PushBack <br>
 *        insert an object or data buffer into the output
 * - @ref CreateEventDoneData <br>
 *        add event information to the output
 * 
 * In addition, the processing methods are simplified a bit by cutting out most of
 * the parameters. The component implementation 
 * @see AliHLTProcessor AliHLTDataSource AliHLTDataSink
 *
 * \em IMPORTANT: objects and block descriptors provided by the high-level interface
 *  <b>MUST NOT BE DELETED</b> by the caller.
 *
 * @subsubsection alihltcomponent-high-level-int-guidelines High-level interface guidelines
 * - Structures must inherit from the ROOT object base class TObject in order be 
 * transported by the transportation framework.
 * - all pointer members must be transient (marked <tt>//!</tt> behind the member
 * definition), i.e. will not be stored/transported, or properly marked
 * (<tt>//-></tt>) in order to call the streamer of the object the member is pointing
 * to. The latter is not recomended. Structures to be transported between components
 * should be streamlined.
 * - no use of stl vectors/strings, use appropriate ROOT classes instead 
 * 
 * @subsection alihltcomponent-low-level-interface Low-level interface
 * The low-level component interface consists of the specific data processing
 * methods for @ref AliHLTProcessor, @ref AliHLTDataSource, and @ref AliHLTDataSink.
 * - @ref AliHLTProcessor::DoEvent
 * - @ref AliHLTDataSource::GetEvent
 * - @ref AliHLTDataSink::DumpEvent
 * 
 * 
 * @section alihltcomponent-handling Component handling 
 * The handling of HLT analysis components is carried out by the AliHLTComponentHandler.
 * Component are registered automatically at load-time of the component shared library
 * under the following suppositions:
 * - the component library has to be loaded from the AliHLTComponentHandler using the
 *   @ref AliHLTComponentHandler::LoadLibrary method.
 * - the component implementation defines one global object (which is generated
 *   when the library is loaded)
 *
 * @subsection alihltcomponent-design-rules General design considerations
 * The analysis code should be implemented in one or more destict class(es). A 
 * \em component should be implemented which interface the destict analysis code to the
 * component interface. This component generates the analysis object dynamically. <br>
 *
 * Assume you have an implemetation <tt> AliHLTDetMyAnalysis </tt>, another class <tt>
 * AliHLTDetMyAnalysisComponent </tt> contains:
 * <pre>
 * private:
 *   AliHLTDetMyAnalysis* fMyAnalysis;  //!
 * </pre>
 * The object should then be instantiated in the DoInit handler of 
 * <tt>AliHLTDetMyAnalysisComponent </tt>, and cleaned in the DoDeinit handler.
 *
 * Further rules:
 * - avoid big static arrays in the component, allocate the memory at runtime
 *
 * @ingroup alihlt_component
 * @section alihltcomponent-members Class members
 */
class AliHLTComponent : public AliHLTLogging {
 public:
  /** standard constructor */
  AliHLTComponent();
  /** not a valid copy constructor, defined according to effective C++ style */
  AliHLTComponent(const AliHLTComponent&);
  /** not a valid assignment op, but defined according to effective C++ style */
  AliHLTComponent& operator=(const AliHLTComponent&);
  /** standard destructor */
  virtual ~AliHLTComponent();

  /** component type definitions */
  enum TComponentType { kUnknown=0, kSource=1, kProcessor=2, kSink=3 };

  /**
   * Init function to prepare data processing.
   * Initialization of common data structures for a sequence of events.
   * The call is redirected to the internal method @ref DoInit which can be
   * overridden by the child class.<br>
   * During Init also the environment structure is passed to the component.
   * @param environ        environment pointer with environment dependend function
   *                       calls
   * @param environParam   additionel parameter for function calls, the pointer
   *                       is passed as it is
   * @param argc           size of the argument array
   * @param argv           agument array for component initialization
   */
  virtual int Init( AliHLTComponentEnvironment* environ, void* environParam, int argc, const char** argv );

  /**
   * Clean-up function to terminate data processing.
   * Clean-up of common data structures after data processing.
   * The call is redirected to the internal method @ref DoDeinit which can be
   * overridden by the child class.
   */
  virtual int Deinit();

  /**
   * Processing of one event.
   * The method is the entrance of the event processing. The parameters are
   * cached for uses with the high-level interface and the DoProcessing
   * implementation is called.
   *
   * @param evtData
   * @param blocks
   * @param trigData
   * @param outputPtr
   * @param size
   * @param outputBlockCnt  out: size of the output block array, set by the component
   * @param outputBlocks    out: the output block array is allocated internally
   * @param edd
   * @return neg. error code if failed
   */
  int ProcessEvent( const AliHLTComponentEventData& evtData, const AliHLTComponentBlockData* blocks, 
			    AliHLTComponentTriggerData& trigData, AliHLTUInt8_t* outputPtr, 
			    AliHLTUInt32_t& size, AliHLTUInt32_t& outputBlockCnt, 
			    AliHLTComponentBlockData*& outputBlocks,
			    AliHLTComponentEventDoneData*& edd );

  /**
   * Internal processing of one event.
   * The method is pure virtual and implemented by the child classes 
   * - @ref AliHLTProcessor
   * - @ref AliHLTDataSource
   * - @ref AliHLTDataSink
   *
   * @param evtData
   * @param blocks
   * @param trigData
   * @param outputPtr
   * @param size
   * @param outputBlocks    out: the output block array is allocated internally
   * @param edd
   * @return neg. error code if failed
   */
  virtual int DoProcessing( const AliHLTComponentEventData& evtData, const AliHLTComponentBlockData* blocks, 
			    AliHLTComponentTriggerData& trigData, AliHLTUInt8_t* outputPtr, 
			    AliHLTUInt32_t& size,
			    vector<AliHLTComponentBlockData>& outputBlocks,
			    AliHLTComponentEventDoneData*& edd ) = 0;

  // Information member functions for registration.

  /**
   * Get the type of the component.
   * The function is pure virtual and must be implemented by the child class.
   * @return component type id
   */
  virtual TComponentType GetComponentType() = 0; // Source, sink, or processor

  /**
   * Get the id of the component.
   * Each component is identified by a unique id.
   * The function is pure virtual and must be implemented by the child class.
   * @return component id (string)
   */
  virtual const char* GetComponentID() = 0;

  /**
   * Get the input data types of the component.
   * The function is pure virtual and must be implemented by the child class.
   * @return list of data types in the vector reference
   */
  virtual void GetInputDataTypes( vector<AliHLTComponentDataType>& ) = 0;

  /**
   * Get the output data type of the component.
   * The function is pure virtual and must be implemented by the child class.
   * @return output data type
   */
  virtual AliHLTComponentDataType GetOutputDataType() = 0;

  /**
   * Get a ratio by how much the data volume is shrinked or enhanced.
   * The function is pure virtual and must be implemented by the child class.
   * @param constBase        <i>return</i>: additive part, independent of the
   *                                   input data volume  
   * @param inputMultiplier  <i>return</i>: multiplication ratio
   * @return values in the reference variables
   */
  virtual void GetOutputDataSize( unsigned long& constBase, double& inputMultiplier ) = 0;

  /**
   * Spawn function.
   * Each component must implement a spawn function to create a new instance of 
   * the class. Basically the function must return <i>new <b>my_class_name</b></i>.
   * @return new class instance
   */
  virtual AliHLTComponent* Spawn() = 0;

  /**
   * Find matching data types between this component and a consumer component.
   * Currently, a component can produce only one type of data. This restriction is most
   * likely to be abolished in the future.
   * @param pConsumer a component and consumer of the data produced by this component
   * @param tgtList   reference to a vector list to receive the matching data types.
   * @return >= 0 success, neg. error code if failed
   */ 
  int FindMatchingDataTypes(AliHLTComponent* pConsumer, vector<AliHLTComponentDataType>* tgtList);
 
  /**
   * Set the global component handler.
   * The static method is needed for the automatic registration of components. 
   */
  static int SetGlobalComponentHandler(AliHLTComponentHandler* pCH, int bOverwrite=0);

  /**
   * Clear the global component handler.
   * The static method is needed for the automatic registration of components. 
   */
  static int UnsetGlobalComponentHandler();

  /**
   * Helper function to convert the data type to a string.
   */
  static string DataType2Text( const AliHLTComponentDataType& type );

  /**
   * Helper function to print content of data type.
   */
  void PrintDataTypeContent(AliHLTComponentDataType& dt, const char* format=NULL) const;

  /**
   * helper function to initialize AliHLTComponentEventData structure
   */
  static void FillEventData(AliHLTComponentEventData& evtData);

  /**
   * Print info on an AliHLTComponentDataType structure
   * This is just a helper function to examine an @ref AliHLTComponentDataType
   * structur.
   */
  void PrintComponentDataTypeInfo(const AliHLTComponentDataType& dt);

  /**
   * Stopwatch type for benchmarking.
   */
  enum AliHLTStopwatchType {
    /** total time for event processing */
    kSWBase,
    /** detector algorithm w/o interface callbacks */
    kSWDA,
    /** data sources */
    kSWInput,
    /** data sinks */
    kSWOutput,
    /** number of types */
    kSWTypeCount
  };

  /**
   * Helper class for starting and stopping a stopwatch.
   * The guard can be used by instantiating an object in a function. The
   * specified stopwatch is started and the previous stopwatch put on
   * hold. When the function is terminated, the object is deleted automatically
   * deleted, stopping the stopwatch and starting the one on hold.<br>
   * \em IMPORTANT: never create dynamic objects from this guard as this violates
   * the idea of a guard.
   */
  class AliHLTStopwatchGuard {
  public:
    /** standard constructor (not for use) */
    AliHLTStopwatchGuard();
    /** constructor */
    AliHLTStopwatchGuard(TStopwatch* pStart);
    /** copy constructor (not for use) */
    AliHLTStopwatchGuard(AliHLTStopwatchGuard&);
    /** destructor */
    ~AliHLTStopwatchGuard();

  private:
    /**
     * Hold the previous guard for the existence of this guard.
     * Checks whether this guard controls a new stopwatch. In that case, the
     * previous guard and its stopwatch are put on hold.
     * @param pSucc        instance of the stopwatch of the new guard
     * @return    1 if pSucc is a different stopwatch which should
     *            be started<br>
     *            0 if it controls the same stopwatch
     */
    int Hold(TStopwatch* pSucc);

    /**
     * Resume the previous guard.
     * Checks whether the peceeding guard controls a different stopwatch. In that
     * case, the its stopwatch is resumed.
     * @param pSucc        instance of the stopwatch of the new guard
     * @return    1 if pSucc is a different stopwatch which should
     *            be stopped<br>
     *            0 if it controls the same stopwatch
     */
    int Resume(TStopwatch* pSucc);

    /** the stopwatch controlled by this guard */
    TStopwatch* fpStopwatch;                                                //!transient

    /** previous stopwatch guard, put on hold during existence of the guard */
    AliHLTStopwatchGuard* fpPrec;                                           //!transient

    /** active stopwatch guard */
    static AliHLTStopwatchGuard* fgpCurrent;                                //!transient
  };

  /**
   * Set a stopwatch for a given purpose.
   * @param pSW         stopwatch object
   * @param type        type of the stopwatch
   */
  int SetStopwatch(TObject* pSW, AliHLTStopwatchType type=kSWBase);

  /**
   * Init a set of stopwatches.
   * @param pStopwatches object array of stopwatches
   */
  int SetStopwatches(TObjArray* pStopwatches);

 protected:

  /**
   * Fill AliHLTComponentBlockData structure with default values.
   * @param blockData   reference to data structure
   */
  void FillBlockData( AliHLTComponentBlockData& blockData ) const;

  /**
   * Fill AliHLTComponentShmData structure with default values.
   * @param shmData   reference to data structure
   */
  void FillShmData( AliHLTComponentShmData& shmData ) const;

  /**
   * Fill AliHLTComponentDataType structure with default values.
   * @param dataType   reference to data structure
   */
  void FillDataType( AliHLTComponentDataType& dataType ) const;
  
  /**
   * Copy data type structure
   * Copies the value an AliHLTComponentDataType structure to another one
   * @param[out] tgtdt   target structure
   * @param[in] srcdt   source structure
   */
  void CopyDataType(AliHLTComponentDataType& tgtdt, const AliHLTComponentDataType& srcdt);

  /**
   * Set the ID and Origin of an AliHLTComponentDataType structure.
   * The function sets the fStructureSize member and copies the strings
   * to the ID and Origin. Only characters from the valid part of the string
   * are copied, the rest is fille with 0's.
   * Please note that the fID and fOrigin members are not strings, just arrays of
   * chars of size @ref kAliHLTComponentDataTypefIDsize and
   * @ref kAliHLTComponentDataTypefOriginSize respectively and not necessarily with
   * a terminating zero.
   * @param tgtdt   target data type structure
   * @param id      ID string
   * @param origin  Origin string
   */
  void SetDataType(AliHLTComponentDataType& tgtdt, const char* id, const char* origin);

  /**
   * Default method for the internal initialization.
   * The method is called by @ref Init
   */
  virtual int DoInit( int argc, const char** argv );

  /**
   * Default method for the internal clean-up.
   * The method is called by @ref Deinit
   */
  virtual int DoDeinit();

  /**
   * General memory allocation method.
   * All memory which is going to be used 'outside' of the interface must
   * be provided by the framework (online or offline).
   * The method is redirected to a function provided by the current
   * framework. Function pointers are transferred via the @ref
   * AliHLTComponentEnvironment structure.
   */
  void* AllocMemory( unsigned long size );

  /**
   * Helper function to create a monolithic BlockData description block out
   * of a list BlockData descriptors.
   * For convenience, inside the interface vector lists are used, to make the
   * interface pure C style, monilithic blocks must be exchanged. 
   * The method is redirected to a function provided by the current
   * framework. Function pointers are transferred via the @ref
   * AliHLTComponentEnvironment structure.
   */
  int MakeOutputDataBlockList( const vector<AliHLTComponentBlockData>& blocks, AliHLTUInt32_t* blockCount,
			       AliHLTComponentBlockData** outputBlocks );

  /**
   * Fill the EventDoneData structure.
   * The method is redirected to a function provided by the current
   * framework. Function pointers are transferred via the @ref
   * AliHLTComponentEnvironment structure.
   */
  int GetEventDoneData( unsigned long size, AliHLTComponentEventDoneData** edd );

  /**
   * Helper function to convert the data type to a string.
   */
  void DataType2Text(const AliHLTComponentDataType& type, char output[kAliHLTComponentDataTypefIDsize+kAliHLTComponentDataTypefOriginSize+2]) const;

  /**
   * Get event number.
   * @return value of the internal event counter
   */
  int GetEventCount() const;

  /**
   * Get the number of input blocks.
   * @return number of input blocks
   */
  int GetNumberOfInputBlocks() const;

  /**
   * Get the first object of a specific data type from the input data.
   * The hight-level methods provide functionality to transfer ROOT data
   * structures which inherit from TObject.
   * The method looks for the first ROOT object of type dt in the input stream.
   * If also the class name is provided, the object is checked for the right
   * class type. The input data block needs a certain structure, namely the 
   * buffer size as first word. If the cross check fails, the retrieval is
   * silently abondoned, unless the \em bForce parameter is set.<br>
   * \em Note: THE OBJECT MUST NOT BE DELETED by the caller.
   * @param dt          data type of the object
   * @param classname   class name of the object
   * @param bForce      force the retrieval of an object, error messages
   *                    are suppressed if \em bForce is not set
   * @return pointer to @ref TObject, NULL if no objects of specified type
   *         available
   */
  const TObject* GetFirstInputObject(const AliHLTComponentDataType& dt=kAliHLTAnyDataType,
				     const char* classname=NULL,
				     int bForce=0);

  /**
   * Get the first object of a specific data type from the input data.
   * The hight-level methods provide functionality to transfer ROOT data
   * structures which inherit from TObject.
   * The method looks for the first ROOT object of type specified by the ID and 
   * Origin strings in the input stream.
   * If also the class name is provided, the object is checked for the right
   * class type. The input data block needs a certain structure, namely the 
   * buffer size as first word. If the cross check fails, the retrieval is
   * silently abondoned, unless the \em bForce parameter is set.<br>
   * \em Note: THE OBJECT MUST NOT BE DELETED by the caller.
   * @param dtID        data type ID of the object
   * @param dtOrigin    data type origin of the object
   * @param classname   class name of the object
   * @param bForce      force the retrieval of an object, error messages
   *                    are suppressed if \em bForce is not set
   * @return pointer to @ref TObject, NULL if no objects of specified type
   *         available
   */
  const TObject* GetFirstInputObject(const char* dtID, 
				     const char* dtOrigin,
				     const char* classname=NULL,
				     int bForce=0);

  /**
   * Get the next object of a specific data type from the input data.
   * The hight-level methods provide functionality to transfer ROOT data
   * structures which inherit from TObject.
   * The method looks for the next ROOT object of type and class specified
   * to the previous @ref GetFirstInputObject call.<br>
   * \em Note: THE OBJECT MUST NOT BE DELETED by the caller.
   * @param bForce      force the retrieval of an object, error messages
   *                    are suppressed if \em bForce is not set
   * @return pointer to @ref TObject, NULL if no more objects available
   */
  const TObject* GetNextInputObject(int bForce=0);

  /**
   * Get data type of an input block.
   * Get data type of the object previously fetched via
   * GetFirstInputObject/NextInputObject or the last one if no object
   * specified.
   * @param pObject     pointer to TObject
   * @return data specification, kAliHLTVoidDataSpec if failed
   */
  AliHLTComponentDataType GetDataType(const TObject* pObject=NULL);

  /**
   * Get data specification of an input block.
   * Get data specification of the object previously fetched via
   * GetFirstInputObject/NextInputObject or the last one if no object
   * specified.
   * @param pObject     pointer to TObject
   * @return data specification, kAliHLTVoidDataSpec if failed
   */
  AliHLTUInt32_t GetSpecification(const TObject* pObject=NULL);

  /**
   * Get the first block of a specific data type from the input data.
   * The method looks for the first block of type dt in the input stream. It is intended
   * to be used within the high-level interface.<br>
   * \em Note: THE BLOCK DESCRIPTOR MUST NOT BE DELETED by the caller.
   * @param dt          data type of the block
   * @return pointer to @ref AliHLTComponentBlockData
   */
  const AliHLTComponentBlockData* GetFirstInputBlock(const AliHLTComponentDataType& dt=kAliHLTAnyDataType);

  /**
   * Get the first block of a specific data type from the input data.
   * The method looks for the first block of type specified by the ID and 
   * Origin strings in the input stream.  It is intended
   * to be used within the high-level interface.<br>
   * \em Note: THE BLOCK DESCRIPTOR MUST NOT BE DELETED by the caller.
   * @param dtID        data type ID of the block
   * @param dtOrigin    data type origin of the block
   * @return pointer to @ref AliHLTComponentBlockData
   */
  const AliHLTComponentBlockData* GetFirstInputBlock(const char* dtID, 
						      const char* dtOrigin);

  /**
   * Get input block by index.<br>
   * \em Note: THE BLOCK DESCRIPTOR MUST NOT BE DELETED by the caller.
   * @return pointer to AliHLTComponentBlockData, NULL if index out of range
   */
  const AliHLTComponentBlockData* GetInputBlock(int index);

  /**
   * Get the next block of a specific data type from the input data.
   * The method looks for the next block  of type and class specified
   * to the previous @ref GetFirstInputBlock call.
   * To be used within the high-level interface.<br>
   * \em Note: THE BLOCK DESCRIPTOR MUST NOT BE DELETED by the caller.
   */
  const AliHLTComponentBlockData* GetNextInputBlock();

  /**
   * Get data specification of an input block.
   * Get data specification of the input bblock previously fetched via
   * GetFirstInputObject/NextInputObject or the last one if no block
   * specified.
   * @param pBlock     pointer to input block
   * @return data specification, kAliHLTVoidDataSpec if failed
   */
  AliHLTUInt32_t GetSpecification(const AliHLTComponentBlockData* pBlock=NULL);

  /**
   * Insert an object into the output.
   * @param pObject     pointer to root object
   * @param dt          data type of the object
   * @param spec        data specification
   * @return neg. error code if failed 
   */
  int PushBack(TObject* pObject, const AliHLTComponentDataType& dt, 
	       AliHLTUInt32_t spec=kAliHLTVoidDataSpec);

  /**
   * Insert an object into the output.
   * @param pObject     pointer to root object
   * @param dtID        data type ID of the object
   * @param dtOrigin    data type origin of the object
   * @param spec        data specification
   * @return neg. error code if failed 
   */
  int PushBack(TObject* pObject, const char* dtID, const char* dtOrigin,
	       AliHLTUInt32_t spec=kAliHLTVoidDataSpec);

  /**
   * Insert an object into the output.
   * @param pBuffer     pointer to buffer
   * @param iSize       size of the buffer
   * @param dt          data type of the object
   * @param spec        data specification
   * @return neg. error code if failed 
   */
  int PushBack(void* pBuffer, int iSize, const AliHLTComponentDataType& dt,
	       AliHLTUInt32_t spec=kAliHLTVoidDataSpec);

  /**
   * Insert an object into the output.
   * @param pBuffer     pointer to buffer
   * @param iSize       size of the buffer
   * @param dtID        data type ID of the object
   * @param dtOrigin    data type origin of the object
   * @param spec        data specification
   * @return neg. error code if failed 
   */
  int PushBack(void* pBuffer, int iSize, const char* dtID, const char* dtOrigin,
	       AliHLTUInt32_t spec=kAliHLTVoidDataSpec);

  /**
   * Estimate size of a TObject
   * @param pObject
   * @return buffer size in byte
   */
  int EstimateObjectSize(TObject* pObject) const;

  /**
   * Insert event-done data information into the output.
   * @param edd          event-done data information
   */
  int CreateEventDoneData(AliHLTComponentEventDoneData edd);

 private:
  /**
   * Increment the internal event counter.
   * To be used by the friend classes AliHLTProcessor, AliHLTDataSource
   * and AliHLTDataSink.
   * @return new value of the internal event counter
   * @internal
   */
  int IncrementEventCounter();

  /**
   * Find the first input block of specified data type beginning at index.
   * @param dt          data type
   * @param startIdx    index to start the search
   * @return index of the block, -ENOENT if no block found
   *
   * @internal
   */
  int FindInputBlock(const AliHLTComponentDataType& dt, int startIdx=-1) const;

  /**
   * Get index in the array of input bocks.
   * Calculate index and check integrety of a block data structure pointer.
   * @param pBlock      pointer to block data
   * @return index of the block, -ENOENT if no block found
   *
   * @internal
   */
  int FindInputBlock(const AliHLTComponentBlockData* pBlock) const;

  /**
   * Create an object from a specified input block.
   * @param idx         index of the input block
   * @param bForce      force the retrieval of an object, error messages
   *                    are suppressed if \em bForce is not set
   * @return pointer to TObject, caller must delete the object after use
   *
   * @internal
   */
  TObject* CreateInputObject(int idx, int bForce=0);

  /**
   * Get input object
   * Get object from the input block list. The methods first checks whether the
   * object was already created. If not, it is created by @ref CreateInputObject
   * and inserted into the list of objects.
   * @param idx         index in the input block list
   * @param classname   name of the class, object is checked for correct class
   *                    name if set
   * @param bForce      force the retrieval of an object, error messages
   *                    are suppressed if \em bForce is not set
   * @return pointer to TObject
   *
   * @internal
   */
  TObject* GetInputObject(int idx, const char* classname=NULL, int bForce=0);

  /**
   * Clean the list of input objects.
   * Cleanup is done at the end of each event processing.
   */
  int CleanupInputObjects();

  /**
   * Insert a buffer into the output block stream.
   * This is the only method to insert blocks into the output stream, called
   * from all types of the Pushback method. The actual data might have been
   * written to the output buffer already. In that case NULL can be provided
   * as buffer, only the block descriptor will be build.
   * @param pBuffer     pointer to buffer
   * @param iSize       size of the buffer in byte
   * @param dt          data type
   * @param spec        data specification
   */
  int InsertOutputBlock(void* pBuffer, int iSize,
			const AliHLTComponentDataType& dt,
			AliHLTUInt32_t spec);


  /** The global component handler instance */
  static AliHLTComponentHandler* fgpComponentHandler;              //! transient

  /** The environment where the component is running in */
  AliHLTComponentEnvironment fEnvironment;                         // see above

  /** Set by ProcessEvent before the processing starts */
  AliHLTEventID_t fCurrentEvent;                                   // see above

  /** internal event no */
  int fEventCount;                                                 // see above

  /** the number of failed events */
  int fFailedEvents;                                               // see above

  /** event data struct of the current event under processing */
  AliHLTComponentEventData fCurrentEventData;                      // see above

  /** array of input data blocks of the current event */
  const AliHLTComponentBlockData* fpInputBlocks;                   //! transient

  /** index of the current input block */
  int fCurrentInputBlock;                                          // see above

  /** data type of the last block search */
  AliHLTComponentDataType fSearchDataType;                         // see above

  /** name of the class for the object to search for */
  string fClassName;                                               // see above

  /** array of generated input objects */
  TObjArray* fpInputObjects;                                       //! transient
 
  /** the output buffer */
  AliHLTUInt8_t* fpOutputBuffer;                                   //! transient

  /** size of the output buffer */
  AliHLTUInt32_t fOutputBufferSize;                                // see above

  /** size of data written to output buffer */
  AliHLTUInt32_t fOutputBufferFilled;                              // see above

  /** list of ouput block data descriptors */
  vector<AliHLTComponentBlockData> fOutputBlocks;                  // see above

  /** stopwatch array */
  TObjArray* fpStopwatches;                                        //! transient

  ClassDef(AliHLTComponent, 2)
};
#endif

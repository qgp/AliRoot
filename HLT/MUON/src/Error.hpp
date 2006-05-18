////////////////////////////////////////////////////////////////////////////////
//
// Author: Artur Szostak
// Email:  artur@alice.phy.uct.ac.za | artursz@iafrica.com
//
////////////////////////////////////////////////////////////////////////////////

/* AliHLTMUONCoreError is the base excpetion class used by the dHLT subsystem.
   All child classes used to throw exception should be derived from this
   class to allow easy catching of classes of errors.
   
   AliHLTMUONCoreOutOfMemory is also defined to be used when the system runs
   out of memory. Do not throw this object directly but rather use
   AliHLTMUONCoreThrowOutOfMemory which throws a pree allocated static object.
 */

#ifndef ALIHLTMUONCOREERROR_H
#define ALIHLTMUONCOREERROR_H

#include "BasicTypes.hpp"
#include <exception>
#include <Riostream.h>


class AliHLTMUONCoreError : public std::exception
{
	/* Define the << operator for streams to be able to do something like:

               Error myerror;
               cout << myerror << endl;
	*/
	friend ostream& operator << (ostream& os, const AliHLTMUONCoreError& error)
	{
		os << error.Message();
		return os;
	};
	
public:

	AliHLTMUONCoreError() throw() {};
	virtual ~AliHLTMUONCoreError() throw() {};

	/* Should return a human readable string containing a description of the
	   error.
	 */
	virtual const char* Message() const throw() = 0;
	
	/* Returns an error code describing the error. The error code should be
	   unique to the entire system
	 */
	virtual Int ErrorCode() const throw() = 0;
	
	virtual const char* what() const throw()
	{
		return Message();
	};
};


class AliHLTMUONCoreOutOfMemory : public AliHLTMUONCoreError
{
public:
	virtual const char* Message() const throw();
	virtual Int ErrorCode() const throw();
};


/* When one needs to indicate that no more memory is available one should use the
   AliHLTMUONCoreThrowOutOfMemory method rather than explicitly using the code
       throw OutOfMemory();
   This is because the ThrowOutOfMemory routine throws a preallocated object so
   we are safe from having to allocate more (nonexistant) memory.
 */
void AliHLTMUONCoreThrowOutOfMemory() throw (AliHLTMUONCoreOutOfMemory);


// Error code declarations.
enum
{
	kOutOfMemory = 0x10000001
};


#endif // ALIHLTMUONCOREERROR_H

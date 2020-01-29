/**************************************************************************************************
	Module:       CFatalException.h
	Author:       Laurent Noel
	Date created: 30/07/05

	Definition of the CFatalException class

	Copyright 2007, University of Central Lancashire and Laurent Noel

	Change history:
		V1.0    Created 30/07/05 - LN
**************************************************************************************************/

#ifndef GEN_C_FATAL_EXCEPTION_H_INCLUDED
#define GEN_C_FATAL_EXCEPTION_H_INCLUDED

#include <string>
using namespace std;

#include "GenDefines.h"

namespace gen
{

// Fatal exception caught and handled by guard macros, holds location and description and collates
// call stack string. No exception guards used here - could create infinite loop if triggered
class CFatalException
{
	GEN_CLASS( CFatalException );

public:
	// Constructor takes a description and the filename/line number where the exception occured
	CFatalException
	(
		const char* sDescription,
		const char* sFileName,
		TInt32      iLineNum
	) : msDescription( sDescription ), msFileName( sFileName ),
	    miLineNum( iLineNum ), msCallStack( "" )
	{
	}

	// Unhandled exception constructor - takes filename and function / object where it occurred
	CFatalException
	(
		const char* sFileName,
		const char* sFunction,
		const char* sObject
	) : msDescription( "Unhandled Exception" ), msFileName( sFileName ), 
	    miLineNum( -1 ), msCallStack( "" )
	{
		AppendToCallStack( sFunction, sObject );
	}

private:
	// Disallow use of assignment operator (private and not defined)
	CFatalException& operator=( const CFatalException& );


public:
	// Display the exception details to the user
	void Display() const;


	// Append current class, object and function names to the call stack string as each function in
	// the call stack unwinds. Can specify if this is the final (root) entry to add to the stack
	void AppendToCallStack
	(
		const char* sFunction,
		const char* sObject,
		bool        bRoot = false
	);

private:
	const string msDescription; // Description of the cause of the exception
	const string msFileName;    // File name where the exception occured
	const TInt32 miLineNum;     // Line number within the file at which the exception occured
	string       msCallStack;   // Textual form of function call stack at point of exception
};


} // namespace gen

#endif // GEN_C_FATAL_EXCEPTION_H_INCLUDED

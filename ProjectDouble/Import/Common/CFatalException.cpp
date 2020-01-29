/**************************************************************************************************
	Module:       CFatalException.cpp
	Author:       Laurent Noel
	Date created: 30/07/05

	Implementation of the CFatalException class

	Copyright 2007, University of Central Lancashire and Laurent Noel

	Change history:
		V1.0    Created 30/07/05 - LN
**************************************************************************************************/

#include "CFatalException.h"
#include "Utility.h"

namespace gen
{

// Display the exception details to the user
void CFatalException::Display() const
{
	// Append all information into a single text string
	string sMessage = msDescription + ksNewline + ksNewline;

	// Strip any path from filename
	sMessage += "File: " + LastDelimitedSubstr( msFileName, ksPathSeparator );

	// If line number -ve then unknown line number
	if (miLineNum >= 0)
	{
		sMessage += ",  Line: " + ToString( miLineNum );
	}
	sMessage += ksNewline + ksNewline + "Call stack: " + msCallStack;

	SystemMessageBox( sMessage, "Fatal Exception" );
}


// Append current class, object and function names to the call stack string as each function in
// the call stack unwinds. Can specify if this is the final (root) entry to add to the stack
void CFatalException::AppendToCallStack
(
	const char* sFunction,
	const char* sObject,
	bool        bRoot /*= false*/
)
{
	msCallStack += sFunction;
	if (*sObject != 0)
	{
		msCallStack += "[";
		msCallStack += sObject;
		msCallStack += "]";
	}
	if (!bRoot)
	{
		msCallStack += " <- ";
	}
}


} // namespace gen
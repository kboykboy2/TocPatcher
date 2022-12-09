#pragma once


namespace RFcore
{
	// Error Function
	void CreateError(const char* errorMsg, const char* file, int line);
}

// Throw Error Macro
#define ThrowError(arg) RFcore::CreateError(arg, __FILE__, __LINE__);
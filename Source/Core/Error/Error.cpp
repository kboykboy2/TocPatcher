#include"Error.h"
#include<intrin.h>
#include<iostream>
#include<string>
#include"Source/Core/Logging/Logging.h"

void RFcore::CreateError(const char* errorMsg, const char* file, int line)
{
	std::string ErrorMessage = "An Error Has Occurred In:  " + std::string(file) + "  At Line:  " + std::string(std::to_string(line)) + "\n" + "Error Message: " + std::string(errorMsg) + "\n";
	RFcore::Log(ErrorMessage.c_str(), RFcore::Error);
	__debugbreak();
	exit(-1);
}

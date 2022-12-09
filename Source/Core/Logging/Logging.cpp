#include"Logging.h"
#include<iostream>

#ifdef NDEBUG
bool isDebug = false;
#else
bool isDebug = true;
#endif


// Log To Console Function
void RFcore::Log(const char* string, LogType type)
{
	switch (type)
	{
	case Debug:
		if (isDebug)
		{
			std::cout << "Refactor Log | DEBUG |: " << string << '\n';
		}
		break;
	case Warning:
		std::cout << "Refactor Log | WARNING |: " << string << '\n';
		break;
	case Error:
		std::cout << "Refactor Log | ERROR |: " << string << '\n';
		break;
	case Info:
		std::cout << "Refactor Log | INFO |: " << string << '\n';
		break;
	}
}
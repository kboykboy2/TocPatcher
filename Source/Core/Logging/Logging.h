#pragma once

namespace RFcore
{
	enum LogType
	{
		Debug,
		Warning,
		Error,
		Info

	};

	// Log To Console Function
	void Log(const char* string, LogType type);
}
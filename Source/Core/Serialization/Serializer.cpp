#include"IoStream.h"


//#############################################//
//###### -- Base Serializer Functions -- ######//
//#############################################//

// reverses the order of the bytes given
void RFcore::BaseSerializer::FlipEndian(char* src, uint64 length)
{
	char* bytes = new char[length];

	uint64 bytesIdx = length - 1;
	for (uint64 srcIdx = 0; srcIdx < length; srcIdx++)
	{
		bytes[bytesIdx] = src[srcIdx];
		bytesIdx--;
	}
	memcpy(src, bytes, length);
	delete[] bytes;
}


// reverses the order of the bytes given
char* RFcore::BaseSerializer::FlipEndianAlt(char* src, uint64 length)
{
	char* bytes = new char[length];

	uint64 bytesIdx = length - 1;
	for (uint64 srcIdx = 0; srcIdx < length; srcIdx++)
	{
		bytes[bytesIdx] = src[srcIdx];
		bytesIdx--;
	}

	return bytes;
}
#include"Source/Core/Serialization/Serializer.h"



RFcore::uint64 RFcore::MemStreamReader::Tell()
{
	return currentPos;
}


void RFcore::MemStreamReader::Seek(uint64 offset)
{
	currentPos = offset;
}


void RFcore::MemStreamReader::SerializeBytes(char* data, uint64 len, bool canFlipEndian)
{
	if (currentPos + len > BlockSize) {
		ThrowError("Reading Past End Of Data Stream");
	}
	memcpy(data, &block[currentPos], len);
	if (BigEndian && canFlipEndian) FlipEndian(data, len);
	currentPos += len;
}
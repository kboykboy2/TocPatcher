#include"Source/Core/Serialization/Serializer.h"


RFcore::uint64 RFcore::DiskReader::Tell()
{
	return file.tellg();
}


void RFcore::DiskReader::Seek(uint64 offset)
{
	file.seekg(offset);
}


void RFcore::DiskReader::SerializeBytes(char* data, uint64 len, bool canFlipEndian)
{
	file.read(data, len);
	if (BigEndian && canFlipEndian) FlipEndian(data, len);
}
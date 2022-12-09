#include"Source/Core/Serialization/Serializer.h"

void RFcore::MemReader::ReadBlock(uint64 start)
{
	// Get File Size
	file.seekg(0, std::ios::end);
	uint64 fileSize = file.tellg();
	currentBlockSize = maxBlockSize;
	if (fileSize-blockEnd < maxBlockSize) currentBlockSize = fileSize - blockEnd;
	// Read Data
	if(block)delete[] block;
	block = new char[currentBlockSize];
	file.seekg(start);
	file.read(block, currentBlockSize);
	// Update Info
	blockStart = start;
	blockEnd   = start + currentBlockSize;
	currentBlockPos = 0;
	currentPos = start;
}

void RFcore::MemReader::UpdatePos(uint64 pos)
{
	if (pos < blockStart or pos > blockEnd)
	{
		ReadBlock(pos);
	}
	else {
		currentPos = pos;
		currentBlockPos = pos - blockStart;
	}
}




RFcore::uint64 RFcore::MemReader::Tell()
{
	return currentPos;
}


void RFcore::MemReader::Seek(uint64 offset)
{
	UpdatePos(offset);
}


void RFcore::MemReader::SerializeBytes(char* data, uint64 len, bool canFlipEndian)
{
	if (currentPos + len > blockEnd) {
		ReadBlock(currentPos);
		if (currentPos + len > blockEnd) ThrowError("Reading Past End Of File");
	}
	memcpy(data, &block[currentBlockPos], len);
	if (BigEndian && canFlipEndian) FlipEndian(data, len);
	UpdatePos(currentPos + len);
}
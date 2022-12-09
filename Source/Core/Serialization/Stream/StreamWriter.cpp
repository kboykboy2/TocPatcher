#include"Source/Core/Serialization/Serializer.h"

RFcore::uint64 RFcore::MemStreamWriter::Tell()
{
	return currentPos;
}


void RFcore::MemStreamWriter::Seek(uint64 offset)
{
	currentPos = offset;
}


void RFcore::MemStreamWriter::SerializeBytes(char* data, uint64 len, bool canFlipEndian)
{
	if (BigEndian && canFlipEndian)
	{
		char* newdata = FlipEndianAlt(data, len);
		memcpy(&block[currentPos], newdata, len);
		delete[] newdata;
	}
	else
	{
		memcpy(&block[currentPos], data, len);
	}
	currentPos += len;
}


void RFcore::MemStreamWriter::Finalize()
{
	// Write Pointers
	for (const SerializablePtr& Ptr1 : Pointers)
	{
		for (const SerializablePtr& Ptr2 : SerializedPointers)
		{
			if (Ptr1.Ref == Ptr2.Ref or Ptr1.ptr == Ptr2.ptr)
			{
				Seek(Ptr1.location);
				if (Use64BitPtr)
				{
					uint64 Pointer = Ptr2.ptrValue;
					SerializeBytes((char*)&Pointer, sizeof(Pointer));
				}
				else
				{
					uint32 Pointer = Ptr2.location;
					SerializeBytes((char*)&Pointer, sizeof(Pointer));
				}
				break;
			}
		}
	}
}

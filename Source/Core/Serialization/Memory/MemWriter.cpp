#include"Source/Core/Serialization/Serializer.h"

/*
RFcore::uint64 RFcore::DiskWriter::Tell()
{
	return file.tellg();
}


void RFcore::DiskWriter::Seek(uint64 offset)
{
	file.seekg(offset);
}


void RFcore::DiskWriter::SerializeBytes(char* data, uint64 len, bool canFlipEndian)
{
	if (BigEndian && canFlipEndian)
	{
		char* newdata = FlipEndianAlt(data, len);
		file.write(newdata, len);
		delete[] newdata;
	}
	else
	{
		file.write(data, len);
	}
}


void RFcore::DiskWriter::Finalize()
{
	// Write Pointers
	for (const SerializablePtr& Ptr1 : Pointers)
	{
		for (const SerializablePtr& Ptr2 : SerializedPointers)
		{
			if (Ptr1.Ref == Ptr2.Ref or Ptr1.ptr == Ptr2.ptr)
			{
				file.seekg(Ptr1.location);
				if (Use64BitPtr)
				{
					uint64 Pointer = Ptr2.ptrValue;
					if (BigEndian) {
						file.write(FlipEndianAlt((char*)&Pointer, sizeof(Pointer)), sizeof(Pointer));
					}
					else {
						file.write((char*)&Pointer, sizeof(Pointer));
					}
				}
				else
				{
					uint32 Pointer = Ptr2.location;
					if (BigEndian) {
						file.write(FlipEndianAlt((char*)&Pointer, sizeof(Pointer)), sizeof(Pointer));
					}
					else {
						file.write((char*)&Pointer, sizeof(Pointer));
					}
				}
				break;
			}
		}
	}
	file.close();
}
*/
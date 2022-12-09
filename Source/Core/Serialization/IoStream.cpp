#include"IoStream.h"


//######################################//
//###### -- IoStream Functions -- ######//
//######################################//


//_________________//
// -- Open File -- //
void RFcore::IoStream::OpenFile(std::string filepath, OpenMode open_mode, SerializationMode mode, Endian endian)
{
	Mode = mode;
	FileEndian = endian;
	FilePath = filepath;

	switch (open_mode)
	{
	case Read:
		IsReading = true;
		IsWriting = false;
		switch (mode)
		{
		case Disk:
			Serializer = new DiskReader();
			((DiskReader*)Serializer)->file = std::fstream(filepath, std::ios::in | std::ios::binary);
			break;
		case Memory:
			Serializer = new MemReader();
			((MemReader*)Serializer)->file = std::fstream(filepath, std::ios::in | std::ios::binary);
			break;
		}
		break;
	case Write:
		IsReading = false;
		IsWriting = true;
		switch (mode)
		{
		case Disk:
			Serializer = new DiskWriter();
			((DiskWriter*)Serializer)->file = std::fstream(filepath, std::ios::out | std::ios::binary);
			break;
		}
		break;
	}
	Serializer->BigEndian = (endian == BigEndian);
	IsFileOpen = true;
}

//_________________________//
// -- Open Data Stream -- //
void RFcore::IoStream::OpenDataStream(char* src, uint64 len, OpenMode open_mode, Endian endian)
{
	FileEndian = endian;
	switch (open_mode)
	{
	case Read:
		IsReading = true;
		IsWriting = false;
		Serializer = new MemStreamReader();
		((MemStreamReader*)Serializer)->block = src;
		((MemStreamReader*)Serializer)->BlockSize = len;
		break;
	case Write:
		IsReading = false;
		IsWriting = true;
		Serializer = new MemStreamWriter();
		((MemStreamWriter*)Serializer)->block = src;
		((MemStreamWriter*)Serializer)->BlockSize = len;
		break;
	}

	Serializer->BigEndian = (endian == BigEndian);
	IsFileOpen = true;
}



//____________________________________________________________//
// --------------------- Main Functions --------------------- //

// Update File Endian
void RFcore::IoStream::UpdateEndian(Endian newEndian)
{
	FileEndian = newEndian;
	Serializer->BigEndian = (newEndian == BigEndian);
}

// Go To Location In File
void RFcore::IoStream::seek(uint64 location)
{
	Serializer->Seek(location);
}

// Return Current Location In File
RFcore::uint64 RFcore::IoStream::tell()
{
	return Serializer->Tell();
}







//_____________________________________________________________//
// -- Operator Overload Functions For serializing Variables -- //
// Base Classes
void RFcore::IoStream::operator<<(uint8& var) { Serializer->SerializeBytes((char*)&var, sizeof(var)); }
void RFcore::IoStream::operator<<(int8& var) { Serializer->SerializeBytes((char*)&var, sizeof(var)); }
void RFcore::IoStream::operator<<(uint16& var) { Serializer->SerializeBytes((char*)&var, sizeof(var)); }
void RFcore::IoStream::operator<<(int16& var) { Serializer->SerializeBytes((char*)&var, sizeof(var)); }
void RFcore::IoStream::operator<<(uint32& var) { Serializer->SerializeBytes((char*)&var, sizeof(var)); }
void RFcore::IoStream::operator<<(int32& var) { Serializer->SerializeBytes((char*)&var, sizeof(var)); }
void RFcore::IoStream::operator<<(uint64& var) { Serializer->SerializeBytes((char*)&var, sizeof(var)); }
void RFcore::IoStream::operator<<(int64& var) { Serializer->SerializeBytes((char*)&var, sizeof(var)); }
void RFcore::IoStream::operator<<(float32& var) { Serializer->SerializeBytes((char*)&var, sizeof(var)); }
void RFcore::IoStream::operator<<(float64& var) { Serializer->SerializeBytes((char*)&var, sizeof(var)); }
void RFcore::IoStream::operator<<(SerializableClass& var) { var.Serialize(*this); }

// Array Class
void RFcore::IoStream::operator<<(SerializableArray var)
{
	if (var.type == PtrType::SerializableClass)
	{
		for (uint64 i = 0; i < var.num; i++)
		{
			auto classptr = (SerializableClass*)((int64)(&((SerializableClass*)var.ptr)[0]) + (var.size * i));
			classptr->Serialize(*this);
		}
	}
	else
	{
		for (int i = 0; i < var.num; i++)
		{
			Serializer->SerializeBytes(&((char*)var.ptr)[i * var.size], var.size);
		}
	}
}

// Buffer Class
void RFcore::IoStream::operator<<(SerializableBuffer var)
{
	Serializer->SerializeBytes((char*)var.ptr, (var.size * var.num), false);
}

// Pointer Class
void RFcore::IoStream::operator<<(SerializablePtr var)
{
	var.location = tell();

	uint64 pointer = 0;
	if (Use64BitPtr)
	{
		uint64 pointer64 = 0;
		*this << pointer64;
		pointer = pointer64;
	}
	else
	{
		uint32 pointer32 = 0;
		*this << pointer32;
		pointer = pointer32;
	}
	var.ptrValue = pointer;
	Serializer->Pointers.push_back(var);

}

// Pointer Class
void RFcore::IoStream::operator>>(SerializablePtr var)
{
	uint64 loc = 0;
	char*& t = (char*&)*(char*)var.Ref;

	if (IsReading)
	{
		for (const SerializablePtr& Ptr2 : Serializer->Pointers)
		{
			if (Ptr2.Ref == var.Ref)
			{
				if (Ptr2.ptrValue == 0) {
					t = nullptr;
					var.ptr = t;
					return;
				}
				loc = tell();
				seek(Ptr2.ptrValue);
				break;
			}
		}

		t = new char[var.num * var.size];
		for (int i = 0; i < var.num; i++)
		{
			memcpy((void*)&t[i * var.size], var.ptr, var.size);
		}
		var.ptr = t;
	}
	else
	{
		if (var.wasVoid)
		{
			var.ptr = nullptr;
			return;
		}

		for (SerializablePtr& Ptr2 : Serializer->Pointers)
		{
			if (Ptr2.Ref == var.Ref)
			{
				Ptr2.ptrValue = var.location;
			}
		}
	}
	var.location = tell();
	switch (var.type)
	{
	case PtrType::Char:
		for (uint64 i = 0; i < var.num; i++) { *this << ((int8*)var.ptr)[i]; }
		break;
	case PtrType::Int16:
		for (uint64 i = 0; i < var.num; i++) { *this << ((int16*)var.ptr)[i]; }
		break;
	case PtrType::Int32:
		for (uint64 i = 0; i < var.num; i++) { *this << ((int32*)var.ptr)[i]; }
		break;
	case PtrType::Int64:
		for (uint64 i = 0; i < var.num; i++) { *this << ((int64*)var.ptr)[i]; }
		break;
	case PtrType::Float32:
		for (uint64 i = 0; i < var.num; i++) { *this << ((float32*)var.ptr)[i]; }
		break;
	case PtrType::Float64:
		for (uint64 i = 0; i < var.num; i++) { *this << ((float64*)var.ptr)[i]; }
		break;
	case PtrType::SerializableClass:
		for (uint64 i = 0; i < var.num; i++)
		{
			auto classptr = (SerializableClass*)((int64)(&((SerializableClass*)var.ptr)[0]) + (var.size * i));
			classptr->Serialize(*this);
		}
		break;
	}
	if (IsWriting && var.num > 0)Serializer->SerializedPointers.push_back(var);
	if (IsReading && var.returnToStart) seek(loc);
}











//____________________________________________________________//
// -------------------- Helper Functions -------------------- //


// -- Create Serializable Pointer Class -- //
RFcore::SerializablePtr RFcore::IoStream::ToPtr(PtrType type, void*& classptr, void* new_classptr, uint64 size, uint64 num, bool returnToStart)
{
	bool wasVoid;
	if (!classptr) {
		classptr = new_classptr;
		wasVoid = true;
	}
	else {
		delete[] new_classptr;
		wasVoid = false;
	}
	return { classptr, (uint64)&classptr, size, num, type, wasVoid, returnToStart };

};


// -- Create Serializable Array Class -- //
RFcore::SerializableArray RFcore::IoStream::ToArray(PtrType type, void*& REF, void* classptr, void* new_classptr, uint64 size, uint64 num)
{
	bool wasVoid;
	if (!classptr) {
		REF = new_classptr;
		classptr = new_classptr;
		wasVoid = true;
	}
	else {
		delete[] new_classptr;
		wasVoid = false;
	}
	return { classptr, (uint64)&REF, size, num, type, wasVoid };

};





// -- Get Serializable Pointer Class -- //
RFcore::PtrType RFcore::IoStream::GetType(SerializableClass* var) { return PtrType::SerializableClass; };
RFcore::PtrType RFcore::IoStream::GetType(void* var) { return PtrType::Void; };
RFcore::PtrType RFcore::IoStream::GetType(unsigned long* var) { return PtrType::Int32; };
RFcore::PtrType RFcore::IoStream::GetType(long* var) { return PtrType::Int32; };
RFcore::PtrType RFcore::IoStream::GetType(char* var) { return PtrType::Char; };
RFcore::PtrType RFcore::IoStream::GetType(const char* var) { return PtrType::Char; };
RFcore::PtrType RFcore::IoStream::GetType(unsigned char* var) { return PtrType::Char; };
RFcore::PtrType RFcore::IoStream::GetType(int16* var) { return PtrType::Int16; };
RFcore::PtrType RFcore::IoStream::GetType(uint16* var) { return PtrType::Int16; };
RFcore::PtrType RFcore::IoStream::GetType(int32* var) { return PtrType::Int32; };
RFcore::PtrType RFcore::IoStream::GetType(uint32* var) { return PtrType::Int32; };
RFcore::PtrType RFcore::IoStream::GetType(int64* var) { return PtrType::Int64; };
RFcore::PtrType RFcore::IoStream::GetType(uint64* var) { return PtrType::Int64; };
RFcore::PtrType RFcore::IoStream::GetType(float32* var) { return PtrType::Float32; };
RFcore::PtrType RFcore::IoStream::GetType(float64* var) { return PtrType::Float64; };
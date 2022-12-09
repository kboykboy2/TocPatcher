#pragma once
#include"Serializer.h"

// -- Define Macros -- //

// Ptr(pointer, num = 1, returnToStart = true)
#define Ptr(pointer, ...) RFcore::IoStream::ToPtr(RFcore::IoStream::GetType(pointer), (void*&)pointer, ((void*)new std::remove_pointer<decltype(pointer)>::type()), sizeof(*pointer), __VA_ARGS__)

// Array(pointer, num)
#define Array(pointer, num) RFcore::IoStream::ToArray(RFcore::IoStream::GetType(pointer), (void*&)pointer, (void*)pointer, ((void*)new std::remove_pointer<decltype(pointer)>::type[num]()), sizeof(*pointer), num)

// Buffer(pointer, num)
#define Buffer(pointer, len) RFcore::IoStream::ToArray(RFcore::IoStream::GetType(pointer), (void*&)pointer, (void*)pointer, ((void*)new std::remove_pointer<decltype(pointer)>::type[len]()), sizeof(*pointer), len)



namespace RFcore
{
	//#######################################//
	//###### -- IoStream Definition -- ######//
	//#######################################//

	class IoStream
	{
		// -- Private Vars -- //
		BaseSerializer* Serializer = nullptr;
	public:
		// -- Helper Class -- //
		class SerializableClass
		{
		public:
			virtual void Serialize(IoStream& io) {};
		};

		// -- Public Vars -- //
		Endian            FileEndian;
		SerializationMode Mode;
		bool Use64BitPtr = false;
		bool IsReading = false;
		bool IsWriting = false;
		bool IsFileOpen = false;
		std::string FilePath = "none";



		// -- Open Function Definitions -- //
		void OpenFile(std::string filepath, OpenMode open_mode, SerializationMode serialization_mode, Endian endian = LittleEndian);
		void OpenDataStream(char* src, uint64 len, OpenMode open_mode, Endian endian = LittleEndian);
		// -- Close Function Definitions -- //
		void Close() { Serializer->Finalize(); };
		// -- Constructor Definition -- //
		IoStream(std::string filepath, OpenMode open_mode, SerializationMode serialization_mode, Endian endian = LittleEndian) { OpenFile(filepath, open_mode, serialization_mode, endian); }
		IoStream() {};
		// -- Destructor Definition -- //
		~IoStream() { Close(); };


		// -- Main Function -- //
		void   UpdateEndian(Endian newEndian);
		void   seek(uint64 location);
		uint64 tell();

		// -- Operator Overload Functions For serializing Variables -- //
		void operator<<(uint8& var);
		void operator<<(int8& var);
		void operator<<(uint16& var);
		void operator<<(int16& var);
		void operator<<(uint32& var);
		void operator<<(int32& var);
		void operator<<(uint64& var);
		void operator<<(int64& var);
		void operator<<(float32& var);
		void operator<<(float64& var);
		void operator<<(SerializableClass& var);
		void operator<<(SerializableArray   var);
		void operator<<(SerializableBuffer  var);
		void operator<<(SerializablePtr     var);
		void operator>>(SerializablePtr     var);



		// -- Helper Functions -- //
		static SerializablePtr   ToPtr(PtrType type, void*& classptr, void* new_classptr, uint64 size, uint64 num = 1, bool returnToStart = true);
		static SerializableArray ToArray(PtrType type, void*& REF, void* classptr, void* new_classptr, uint64 size, uint64 num);

		static PtrType GetType(SerializableClass* var);
		static PtrType GetType(void* var);
		static PtrType GetType(unsigned long* var);
		static PtrType GetType(long* var);
		static PtrType GetType(char* var);
		static PtrType GetType(const char* var);
		static PtrType GetType(unsigned char* var);
		static PtrType GetType(int16* var);
		static PtrType GetType(uint16* var);
		static PtrType GetType(int32* var);
		static PtrType GetType(uint32* var);
		static PtrType GetType(int64* var);
		static PtrType GetType(uint64* var);
		static PtrType GetType(float32* var);
		static PtrType GetType(float64* var);

	};
}
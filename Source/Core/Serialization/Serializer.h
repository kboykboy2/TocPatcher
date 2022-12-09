#pragma once
#include<fstream>
#include<iostream>
#include<type_traits>
#include<vector>
#include<string>
#include"Source/Core/Types.h"
#include"Source/Core/Error/Error.h"


namespace RFcore
{
	//####################################//
	//###### -- Enum Definitions -- ######//
	//####################################//

	enum class PtrType
	{
		Char,
		Int16,
		Int32,
		Int64,
		Float32,
		Float64,
		SerializableClass,
		Void
	};

	enum OpenMode
	{
		Read,
		Write
	};

	enum Endian
	{
		LittleEndian,
		BigEndian
	};

	enum SerializationMode
	{
		// Slower
		Disk,
		// Faster
		Memory,
		// Fastest || !! WARNING !! || Read Full Discription Below Before Use
		Memory_Fast

		/*
		Disk:
		this mode will access the disk every time a read/write function is called, this is pretty slow and generally not recommended.

		Memory:
		if reading this mode will read into large portions of the file into memory, and then data will be read by accessing the data in memory.
		if writing then a large amount of memory will be allocated, and when a write function is called it will add that data to memory,
		when the close function is called it will take that memory and write it to the disk.

		Memory_Fast:
		this mode is the fastest but can be unsafe if used wrong, and can only be used for reading, it works by reading the entire file into memory...
		then when a read function is called it will set the pointer of the variable to the location in memory that is being read,
		meaning no data is being copied instead your variable will directly access the memory of the file, if you then set the variable to a new value...
		it will modify the memory itself and if you read that value again you would get the new value, and if you close the file you will loose all data.
		This mode is best used in "Index" archives where archives are split into seperate "Index" and "Data" files

		(if the file is to big to be read into memory then open function will throw an error )

		*/
	};




	//############################################//
	//###### -- Helper Class Definitions -- ######//
	//############################################//

	struct SerializablePtr
	{
		void* ptr = nullptr;
		uint64  Ref = 0;
		uint64  size = 0;
		uint64  num = 0;
		PtrType type = PtrType::Void;
		bool wasVoid = false;
		bool returnToStart = true;

		uint64 location = 0;
		uint64 ptrValue = 0;
	};

	struct SerializableArray
	{
		void* ptr = nullptr;
		uint64  Ref = 0;
		uint64  size = 0;
		uint64  num = 0;
		PtrType type = PtrType::Void;
		bool wasVoid = false;
	};

	struct SerializableBuffer
	{
		void* ptr = nullptr;
		uint64  Ref = 0;
		uint64  size = 0;
		uint64  num = 0;
		PtrType type = PtrType::Void;
		bool wasVoid = false;
	};




	//##########################################//
	//###### -- Serializer Definitions -- ######//
	//##########################################//

	class BaseSerializer
	{
	public:
		std::vector<SerializablePtr> Pointers;
		std::vector<SerializablePtr> SerializedPointers;
		bool BigEndian = false;
		bool Use64BitPtr = false;

		virtual uint64 Tell() { return 0; };
		virtual void   Seek(uint64 offset) {};
		virtual void   SerializeBytes(char* data, uint64 len, bool canFlipEndian = true) {};
		virtual void   Finalize() {};

		void  FlipEndian(char* src, uint64 length);
		char* FlipEndianAlt(char* src, uint64 length);
	};


	// ----- Disk ----- //
	class DiskReader : public BaseSerializer
	{
	public:
		std::fstream file;

		uint64 Tell();
		void   Seek(uint64 offset);
		void   SerializeBytes(char* data, uint64 len, bool canFlipEndian = true);
		void   Finalize() { file.close(); }
	};
	class DiskWriter : public BaseSerializer
	{
	public:
		std::fstream file;

		uint64 Tell();
		void   Seek(uint64 offset);
		void   SerializeBytes(char* data, uint64 len, bool canFlipEndian = true);
		void   Finalize();
	};


	// ----- Memory ----- //
	class MemReader : public BaseSerializer
	{
		// Private Vars
		uint64 maxBlockSize = 512000000;
		uint64 currentBlockSize=0, blockEnd=0, blockStart=0;
		uint64 currentPos = 0;
		uint64 currentBlockPos = 0;
		char* block = nullptr;
		// Private Functions
		void ReadBlock(uint64 start);
		void UpdatePos(uint64 pos);
	public:
		std::fstream file;

		uint64 Tell();
		void   Seek(uint64 offset);
		void   SerializeBytes(char* data, uint64 len, bool canFlipEndian = true);
		void   Finalize() { file.close(); }
	};
	class MemWriter : public BaseSerializer
	{
	public:
		std::fstream file;

		uint64 Tell();
		void   Seek(uint64 offset);
		void   SerializeBytes(char* data, uint64 len, bool canFlipEndian = true);
		void   Finalize();
	};




	// ----- MemoryStream ----- //
	class MemStreamReader : public BaseSerializer
	{
	public:
		uint64 BlockSize;
		uint64 currentPos = 0;
		char* block = nullptr;

		uint64 Tell();
		void   Seek(uint64 offset);
		void   SerializeBytes(char* data, uint64 len, bool canFlipEndian = true);
		void   Finalize() {}
	};
	class MemStreamWriter : public BaseSerializer
	{
	public:
		uint64 BlockSize;
		uint64 currentPos = 0;
		char* block = nullptr;

		uint64 Tell();
		void   Seek(uint64 offset);
		void   SerializeBytes(char* data, uint64 len, bool canFlipEndian = true);
		void   Finalize();
	};
}
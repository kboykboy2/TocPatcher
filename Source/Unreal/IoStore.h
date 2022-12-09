#include"Source/Core/Serialization/IoStream.h"
#include"lib/rijndael/rijndael.h"
using namespace RFcore;
#define MAX_COMPRESSION_METHODS 8
char PakEncryptionKey[32]{0};


//_____________________________________________________________________//
// --------------------- Unreal Types Definition --------------------- //

class FGUID : public IoStream::SerializableClass
{
public:
	uint32 A;
	uint32 B;
	uint32 C;
	uint32 D;

	void Serialize(IoStream& io)
	{
		io << A;
		io << B;
		io << C;
		io << D;
	}
};

class FString : public IoStream::SerializableClass
{
public:
	uint32 Length;
	char*  String;

	void Serialize(IoStream& io)
	{
		if (io.IsWriting) Length = strlen(String)+1;
		io << Length;
		io << Buffer(String, Length);
	}
};


template<class T>
class TArray : public IoStream::SerializableClass
{
	std::vector<T> Vector;
public:
	void pushback(T& Item) { Vector.push_back(Item); }
	void resize(int size)  { Vector.resize(size); }
	int  size()            { return Vector.size(); }
	T& operator[](int idx) { return Vector[idx]; }

	void Serialize(IoStream& io)
	{
		uint32 size = Vector.size();
		io << size;
		if (io.IsReading) resize(size);
		for (T& item : Vector)
		{
			io << item;
		}
	}
	void SerializeData(IoStream& io)
	{
		for (T& item : Vector)
		{
			io << item;
		}
	}
};







// Taken from umodel source
#define AES_KEYBITS		256
void appDecryptAES(char* Data, int Size, const char* Key, int KeyLen)
{
	if (KeyLen <= 0)
	{
		KeyLen = strlen(Key);
	}

	unsigned long rk[RKLENGTH(AES_KEYBITS)];
	int nrounds = rijndaelSetupDecrypt(rk, (const unsigned char*)Key, AES_KEYBITS);

	for (int pos = 0; pos < Size; pos += 16)
	{
		rijndaelDecrypt(rk, nrounds, (const unsigned char*)Data + pos, (unsigned char*)Data + pos);
	}
}

void appEncryptAES(char* Data, int Size, const char* Key, int KeyLen)
{
	if (KeyLen <= 0)
	{
		KeyLen = strlen(Key);
	}

	unsigned long rk[RKLENGTH(AES_KEYBITS)];
	int nrounds = rijndaelSetupEncrypt(rk, (const unsigned char*)Key, AES_KEYBITS);

	for (int pos = 0; pos < Size; pos += 16)
	{
		rijndaelEncrypt(rk, nrounds, (const unsigned char*)Data + pos, (unsigned char*)Data + pos);
	}
}

//______________________________________________________________________//
// --------------------- IoStore Enum Definitions --------------------- //

enum class EIoContainerFlags : uint8
{
	None       = 0,
	Compressed = 1,
	Encrypted  = 2,
	Signed     = 4,
	Indexed    = 8,
};

//________________________________________________________________________//
// --------------------- IoStore ChunkId Definition --------------------- //
class FIoChunkId : public IoStream::SerializableClass
{
public:
	uint64 ID;
	uint16 Index;
	uint8 Padding;
	uint8 Type;
	void Serialize(IoStream& io)
	{
		io << ID;
		io << Index;
		io << Padding;
		io << Type;
	}
};

//__________________________________________________________________________________//
// --------------------- IoStore Offset And Length Definition --------------------- //
class FIoOffsetAndLength : public IoStream::SerializableClass
{
public:
	uint64 Offset; // 5
	uint64 Length; // 5
	void Serialize(IoStream& io)
	{
		char data[10] { 0 };
		memcpy(&data[0], &Offset, 5);
		memcpy(&data[5], &Length, 5);

		io << Buffer(data, 10);

		memcpy(&Offset, &data[0], 5);
		memcpy(&Length, &data[5], 5);
	}
};

//___________________________________________________________________________________________//
// --------------------- IoStore Toc Compressed Block Entry Definition --------------------- //
class FIoStoreTocCompressedBlockEntry : public IoStream::SerializableClass
{
public:
	uint64 Offset;          // 5
	uint32 CompBlockSize;   // 3
	uint32 UnCompBlockSize; // 3
	uint8  CompMethod;
	void Serialize(IoStream& io) 
	{
		char data[11]{ 0 };
		memcpy(&data[0], &Offset, 5);
		memcpy(&data[5], &CompBlockSize, 3);
		memcpy(&data[8], &UnCompBlockSize, 3);

		io << Buffer(data, 11);
		io << CompMethod;
		
		memcpy(&Offset, &data[0], 5);
		memcpy(&CompBlockSize, &data[5], 3);
		memcpy(&UnCompBlockSize, &data[8], 3);
	}
};

//__________________________________________________________________________//
// --------------------- IoStore Signature Definition --------------------- //
class Signature : public IoStream::SerializableClass
{
public:
	uint32 HashSize;
	char* tocSignature;
	char* blockSignature;
	char* CompressedBlockHashes;

	void Serialize(IoStream& io, uint32 CompressedBlockEntryCount)
	{
		io << HashSize;
		io << Buffer(tocSignature, HashSize);
		io << Buffer(blockSignature, HashSize);
		io << Buffer(CompressedBlockHashes, CompressedBlockEntryCount*20); // SHA hash for every compressed block
	}
};




//___________________________________________________________________________//
// --------------------- IoStore Toc Header Definition --------------------- //
#define TOC_MAGIC "-==--==--==--==-"
class FIoStoreTocHeader : public IoStream::SerializableClass
{
public:
	// Header
	char Magic[16];
	uint32 Version;
	uint32 HeaderSize;           // 144
	uint32 EntryCount;
	uint32 CompBlockCount;
	uint32 CompBlockEntrySize;   // 12
	uint32 CompMethodNameCount;
	uint32 CompMethodNameLength; // 32
	uint32 CompBlockSize;        // 0x10000
	uint32 DirectoryIndexSize;
	uint32 PartitionCount;       // 0
	uint64 ContainerId;
	FGUID  EncryptionKey;
	uint8  ContainterFlags;
	uint8  Pad[7];			     // pad to uint64
	uint64 PartitionSize;
	char   reserved[48];

	void Serialize(IoStream& io)
	{
		// Header
		io << Buffer(Magic, 16);
		io << Version;
		io << HeaderSize;
		io << EntryCount;
		io << CompBlockCount;
		io << CompBlockEntrySize;
		io << CompMethodNameCount;
		io << CompMethodNameLength;
		io << CompBlockSize;
		io << DirectoryIndexSize;
		io << PartitionCount;
		io << ContainerId;
		io << EncryptionKey;
		io << ContainterFlags;
		io << Array(Pad, sizeof(Pad));
		io << PartitionSize;
		io << Buffer(reserved, sizeof(reserved));
	}
};



//__________________________________________________________________________//
// --------------------- IoStore Directory Definition --------------------- //
struct FIoDirectoryIndexEntry : public IoStream::SerializableClass
{
	uint32 Name;
	uint32 FirstChildEntry;
	uint32 NextSiblingEntry;
	uint32 FirstFileEntry;

	void Serialize(IoStream& io)
	{
		io << Name;
		io << FirstChildEntry;
		io << NextSiblingEntry;
		io << FirstFileEntry;
	}
};
struct FIoFileIndexEntry : public IoStream::SerializableClass
{
	uint32 Name;
	uint32 NextFileEntry;
	uint32 UserData;		// index in ChunkIds and ChunkOffsetLengths

	void Serialize(IoStream& io)
	{
		io << Name;
		io << NextFileEntry;
		io << UserData;
	}
};
struct FIoDirectoryIndexResource : public IoStream::SerializableClass
{
	FString MountPoint;
	TArray<FIoDirectoryIndexEntry> DirectoryEntries;
	TArray<FIoFileIndexEntry>      FileEntries;
	TArray<FString>                StringTable;

	void Serialize(IoStream& io)
	{
		io << MountPoint;
		io << DirectoryEntries;
		io << FileEntries;
		io << StringTable;
	}
};

//______________________________________________________________________//
// --------------------- IoStore Chunk Definition --------------------- //
struct FIoTocChunkMeta : public IoStream::SerializableClass
{
	char  hash[32];
	uint8 flags;

	void Serialize(IoStream& io)
	{
		io << Buffer(hash, sizeof(hash));
		io << flags;
	}
};

//__________________________________________________________________________//
// --------------------- IoStore Directory Definition --------------------- //

class FIoStoreTocResource : public IoStream::SerializableClass
{
public:
	FIoStoreTocHeader  Header;
	TArray<FIoChunkId> ChunkIds;
	TArray<FIoOffsetAndLength> ChunkOffsetLengths; // index here corresponds to ChunkIds
	TArray<FIoStoreTocCompressedBlockEntry> CompressionBlocks;
	int CompressionMethods[MAX_COMPRESSION_METHODS];
	Signature signature;
	FIoDirectoryIndexResource DirectoryIndex;
	TArray<FIoTocChunkMeta> MetaData;

	void Serialize(IoStream& io)
	{
		Header.CompBlockCount = CompressionBlocks.size();
		Header.EntryCount = ChunkIds.size();

		io << Header;
		if (io.IsReading){
			ChunkIds.resize(Header.EntryCount); ChunkOffsetLengths.resize(Header.EntryCount); CompressionBlocks.resize(Header.CompBlockCount); MetaData.resize(Header.EntryCount);}
		ChunkIds.SerializeData(io);
		ChunkOffsetLengths.SerializeData(io);
		CompressionBlocks.SerializeData(io);
		io << Array(CompressionMethods, MAX_COMPRESSION_METHODS);
		if (Header.ContainterFlags & (int)EIoContainerFlags::Signed) signature.Serialize(io, Header.CompBlockCount);
		
		int dirstart = io.tell();
		if (Header.ContainterFlags & (int)EIoContainerFlags::Encrypted)
		{
			char* cypherData = new char[Header.DirectoryIndexSize];
			IoStream buffer;
			if (io.IsReading)
			{
				io << Buffer(cypherData, Header.DirectoryIndexSize);
				appDecryptAES(cypherData, Header.DirectoryIndexSize, &PakEncryptionKey[0], 32);
				buffer.OpenDataStream(cypherData, Header.DirectoryIndexSize, OpenMode::Read);
				buffer << DirectoryIndex;
			}
			else
			{
				buffer.OpenDataStream(cypherData, Header.DirectoryIndexSize, OpenMode::Write);
				buffer << DirectoryIndex;
				appEncryptAES(cypherData, Header.DirectoryIndexSize, &PakEncryptionKey[0], 32);
				io << Buffer(cypherData, Header.DirectoryIndexSize);
			}
			buffer.Close();
			delete[] cypherData;
		}
		else
		{
			io << DirectoryIndex;
		}
		int dirend = io.tell();
		MetaData.SerializeData(io);

		if (io.IsWriting)
		{
			Header.DirectoryIndexSize = dirend - dirstart;
			io.seek(0);
			io << Header;
		}
	}
	void Open(std::string path)
	{
		IoStream io(path, Read, Memory, LittleEndian);
		io << *this;
		io.Close();
	}
	void Write(std::string path)
	{
		IoStream io(path, OpenMode::Write, Disk, LittleEndian);
		io << *this;
		io.Close();
	}
};
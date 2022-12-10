#include"Source/Unreal/IoStore.h"
#include"Source/Core/System/System.h"

//#########################################//
//########### Aes Key Functions ###########//
//#########################################//
 
// converts hex string to binary
char* ConvertKey(std::string key, int size)
{
	char* bytes = new char[size/2];

	int idx = 0;
	for (unsigned int i = 0; i < size; i += 2) {
		std::string byteString = key.substr(i, 2);
		char byte = (char)strtol(byteString.c_str(), NULL, 16);
		bytes[idx] = byte;
		idx++;
	}
	return bytes;
}

// get key from file
void GetKey(std::string path)
{
	uint64 size=0; std::string data = System::GetFileData(path, &size);
	if (data.find("0x") + 64 <= size) {
		std::string keystr = data.substr(data.find("0x")+2, 64);
		memcpy((void*)PakEncryptionKey, ConvertKey(keystr, 64), 32);
	}
}


//################################################//
//########### Error Checking Functions ###########//
//################################################//

void PressEnterKey()
{
	std::cout << "\nPress Enter to Exit..." << '\n';
	int in = 0; std::cin.get() >> in;
	exit(-1);
}

void CheckMasterExists(std::string path)
{
	if (!System::FileExists(path)){
		std::cout << "Master.utoc does not exist, please take a .utoc from the game and place it into this folder, and rename it to master.utoc" << '\n';
		PressEnterKey();
	}
}

void CheckArgCount(int count, std::string MasterPath)
{
	if (count < 2)
	{
		FIoStoreTocResource toc_master; toc_master.Open(MasterPath);
		std::cout << "Please input the path to the target toc.\nEither by drag and dropping the toc onto the exe, or through cmd" << "\n\n";

		std::cout << "Master Toc Info:" << '\n';
		std::cout << "CompressionType       : " << (char*)&toc_master.CompressionMethods[0] << '\n';
		std::cout << "Compression Block Size (bytes): " << toc_master.Header.CompBlockSize << '\n';
		std::cout << "Compression Block Size (kb)   : " << (float)toc_master.Header.CompBlockSize / 1024.0 << '\n';

		std::cout << "\nMake sure the target toc is using these same settings" << '\n';

		PressEnterKey();
	}
}

void CheckTargetExt(std::string path)
{
	if (path.find(".utoc") == -1)
	{
		std::cout << "Input file has incorrect extension, should be .utoc" << '\n';
		PressEnterKey();
	}
}

void CheckTargetSettings(FIoStoreTocResource &toc_master, FIoStoreTocResource &toc_target)
{
	bool fail = false;

	std::string targetcomp_low = (char*)&toc_target.CompressionMethods[0], targetcomp_cap = targetcomp_low, mastercomp = (char*)&toc_master.CompressionMethods[0];
	targetcomp_cap[0] = toupper(targetcomp_cap[0]);
	targetcomp_low[0] = tolower(targetcomp_low[0]);
	if (!(targetcomp_low == mastercomp or targetcomp_cap == mastercomp))
	{
		std::cout << "Target toc has incorrect compression format, it is: " << (char*)&toc_target.CompressionMethods[0] << '\n';
		std::cout << "It should be the same as the master, which is: " << (char*)&toc_master.CompressionMethods[0] << '\n';
		fail = true;
	}
	if (toc_target.Header.CompBlockSize != toc_master.Header.CompBlockSize)
	{
		std::cout << "Target toc has incorrect compression block size, it is (bytes): " << (float)toc_target.Header.CompBlockSize / 1024.0 << '\n';
		std::cout << "                                                       (kb)   : " << toc_target.Header.CompBlockSize << '\n';
		std::cout << "It should be the same as the master, which is (bytes): " << toc_master.Header.CompBlockSize << '\n';
		std::cout << "                                              (kb)   : " << (float)toc_master.Header.CompBlockSize / 1024.0 << '\n';
		fail = true;
	}

	if(fail)PressEnterKey();
}

void CheckEncrypted(FIoStoreTocResource& toc_master)
{
	if (toc_master.Header.ContainterFlags & (int)EIoContainerFlags::Encrypted) {
		std::cout << "Master.utoc is encrypted. encrypted tocs are not yet supported" << '\n';
		PressEnterKey();
	}
}


int main(int argc, char* argv[])
{
	std::string KeyPath    = System::GetCurrentExeDirectory() + "\\AesKey.txt";
	std::string MasterPath = System::GetCurrentExeDirectory() + "\\Master.utoc";
	std::string TargetPath;
	if (System::FileExists(KeyPath))GetKey(KeyPath);
	FIoStoreTocResource toc_master, toc_target;
	
	// make sure master toc exists before opening, and make sure target argument has been passed
	CheckMasterExists(MasterPath);
	CheckArgCount(argc, MasterPath);
	CheckTargetExt(argv[1]); TargetPath = argv[1];

	// open toc files
	toc_master.Open(MasterPath);
	toc_target.Open(TargetPath);
	
	// check settings
	CheckTargetSettings(toc_master, toc_target);
	CheckEncrypted(toc_master);

	
	
	// update chunk info
	for (int i = 0; i < toc_target.Header.EntryCount; i++)
	{
		toc_master.ChunkIds[i]           = toc_target.ChunkIds[i];
		toc_master.ChunkOffsetLengths[i] = toc_target.ChunkOffsetLengths[i];
		toc_master.MetaData[i]           = toc_target.MetaData[i];
	}
	
	// update compressed block entry info
	for (int i = 0; i < toc_target.Header.CompBlockCount; i++){
		toc_master.CompressionBlocks[i]               = toc_target.CompressionBlocks[i];
	}
	
	
	// update directory entries
	for (int i = 0; i < toc_target.DirectoryIndex.DirectoryEntries.size(); i++){
		toc_master.DirectoryIndex.DirectoryEntries[i] = toc_target.DirectoryIndex.DirectoryEntries[i];
	}
	// update file entries
	for (int i = 0; i < toc_target.DirectoryIndex.FileEntries.size(); i++){
		toc_master.DirectoryIndex.FileEntries[i]      = toc_target.DirectoryIndex.FileEntries[i];
	}
	
	// update string table
	int diff = 0;
	for (int i = 0; i < toc_target.DirectoryIndex.StringTable.size(); i++) {
		diff += toc_master.DirectoryIndex.StringTable[i].Length - toc_target.DirectoryIndex.StringTable[i].Length;
		toc_master.DirectoryIndex.StringTable[i] = toc_target.DirectoryIndex.StringTable[i];
	}
	// update mount point
	diff += toc_master.DirectoryIndex.MountPoint.Length - toc_target.DirectoryIndex.MountPoint.Length;
	toc_master.DirectoryIndex.MountPoint = toc_target.DirectoryIndex.MountPoint;

	if (diff < 0)
	{
		// fix directory length
		for (int i = toc_target.DirectoryIndex.StringTable.size(); i < toc_master.DirectoryIndex.StringTable.size(); i++) {
			std::string str(toc_master.DirectoryIndex.StringTable[i].String);
			int oldsize = toc_master.DirectoryIndex.StringTable[i].Length;
			int removableAmount = oldsize - 2;
			if (removableAmount > (diff * -1))
			{
				str = str.substr((diff * -1));

				char* cstr = new char[str.size() + 1];
				memcpy(cstr, (char*)str.c_str(), str.size() + 1);
				toc_master.DirectoryIndex.StringTable[i].String = cstr;
				toc_master.DirectoryIndex.StringTable[i].Length = str.size() + 1;
				break;
			}
			else
			{
				str = str.substr(removableAmount);
			}
			char* cstr = new char[str.size() + 1];
			memcpy(cstr, (char*)str.c_str(), str.size() + 1);
			toc_master.DirectoryIndex.StringTable[i].String = cstr;
			toc_master.DirectoryIndex.StringTable[i].Length = str.size() + 1;
			diff += removableAmount;
		}
	}
	if (diff > 0)
	{
		// fix directory length
		for (int i = toc_target.DirectoryIndex.StringTable.size(); i < toc_master.DirectoryIndex.StringTable.size(); i++) {
			std::string str(toc_master.DirectoryIndex.StringTable[i].String);
			int oldsize = toc_master.DirectoryIndex.StringTable[i].Length;

			char* cstr = new char[diff + 1]{0};
			for (int idx = 0; idx < diff; idx++) cstr[idx] = 'E';
			toc_master.DirectoryIndex.StringTable[i].String = cstr;
			toc_master.DirectoryIndex.StringTable[i].Length = diff + 1;
			break;
		}
	}

	for (int idx = toc_target.DirectoryIndex.StringTable.size(); idx < toc_master.DirectoryIndex.StringTable.size(); idx++)
	{
		for (int i = 0; i < strlen(toc_master.DirectoryIndex.StringTable[idx].String); i++)
		{
			toc_master.DirectoryIndex.StringTable[idx].String[i] = 'E';
		}
	}
	
	toc_master.Write(TargetPath);


	std::cout << "Target toc was successfully patched" << "\n\n";
	PressEnterKey();
}
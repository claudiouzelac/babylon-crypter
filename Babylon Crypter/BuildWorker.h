
#include "CryptoStuff.h"

#pragma once
class BuildWorker
{
public:
	BuildWorker(PCHAR,PCHAR,PCHAR);
	~BuildWorker();
	void xEncryptFile();
	void ChangeIcon(PCHAR);
	
	

private:

	typedef struct _ICONDIRENTRY 
	{
        BYTE bWidth;
        BYTE bHeight;
        BYTE bColorCount;
        BYTE bReserved;
        WORD wPlanes;
        WORD wBitCount;
        DWORD dwBytesInRes;
        DWORD dwImageOffset;
   } ICONDIRENTRY, * LPICONDIRENTRY;
	typedef struct _ICONDIR 
	{
        WORD idReserved;
        WORD idType;
        WORD idCount;
        ICONDIRENTRY idEntries[1];
    }   ICONDIR, * LPICONDIR;

	#pragma pack(push)
    #pragma pack(2)
    typedef struct _GRPICONDIRENTRY 
	{
        BYTE bWidth;
        BYTE bHeight;
        BYTE bColorCount;
        BYTE bReserved;
        WORD wPlanes;
        WORD wBitCount;
        DWORD dwBytesInRes;
        WORD nID;
    } GRPICONDIRENTRY, * LPGRPICONDIRENTRY;
    #pragma pack(pop)

    #pragma pack(push)
    #pragma pack(2)
    typedef struct _GRPICONDIR
	{
        WORD idReserved;
        WORD idType;
        WORD idCount;
        GRPICONDIRENTRY idEntries[1];
    } GRPICONDIR, * LPGRPICONDIR;
    #pragma pack(pop)




	PCHAR m_SavePath;
	PCHAR m_PePath;
	PCHAR m_StubPath;
	HANDLE m_PEHandle;
	PCHAR m_peBuffer;
	DWORD m_PESize;
	bool CopyToSavePath();
	CryptoStuff * crypto;
	void ParsePE();


};


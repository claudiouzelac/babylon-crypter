#include "StdAfx.h"
#include "CryptoStuff.h"


CryptoStuff::CryptoStuff()
{

}


CryptoStuff::~CryptoStuff()
{

}

PCHAR CryptoStuff::ByteRounder(PCHAR data, int size)
{
	PCHAR n;
	n = (PCHAR)GlobalAlloc(GPTR, size);
	memcpy(n, data, size);
    for (int i = 0; i < size; i++)
	{
        n[i] += 10;
	}
	return n;
}

void CryptoStuff::Encrypt(PTCHAR inp, DWORD inplen)
{
    TCHAR Sbox[257], Sbox2[257];
    unsigned long i, j, t, x;
    static const TCHAR  OurUnSecuredKey[] = "www.undeth.com" ;
    static const int OurKeyLen = _tcslen(OurUnSecuredKey);    
    TCHAR temp , k;
    i = j = k = t =  x = 0;
    temp = 0;
    ZeroMemory(Sbox, sizeof(Sbox));
    ZeroMemory(Sbox2, sizeof(Sbox2));
    for(i = 0; i < 256U; i++)
    {
        Sbox[i] = (TCHAR)i;
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
        j = (j + (unsigned long) Sbox[i] + (unsigned long) Sbox2[i]) % 256U ;
        temp =  Sbox[i];                    
        Sbox[i] = Sbox[j];
        Sbox[j] =  temp;
    }
    i = j = 0;
    for(x = 0; x < inplen; x++)
    {
        i = (i + 1U) % 256U;
        j = (j + (unsigned long) Sbox[i]) % 256U;
        temp = Sbox[i];
        Sbox[i] = Sbox[j] ;
        Sbox[j] = temp;
        t = ((unsigned long) Sbox[i] + (unsigned long) Sbox[j]) %  256U ;
        k = Sbox[t];
        inp[x] = (inp[x] ^ k);
    }    

}
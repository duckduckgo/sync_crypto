#include "resource.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "..\..\native_lib\DDGSyncCrypto.h"

__declspec(dllexport) DDGSyncCryptoResult __cdecl ddgSyncGenerateAccountKeys(
    unsigned char primaryKey[DDGSYNCCRYPTO_PRIMARY_KEY_SIZE],
    unsigned char secretKey[DDGSYNCCRYPTO_SECRET_KEY_SIZE],
    unsigned char protectedSecretKey[DDGSYNCCRYPTO_PROTECTED_SECRET_KEY_SIZE],
    unsigned char passwordHash[DDGSYNCCRYPTO_HASH_SIZE],
    const char* userId,
    const char* password
);

__declspec(dllexport) DDGSyncCryptoResult __cdecl ddgSyncPrepareForLogin(
    unsigned char* passwordHash,
    unsigned char* stretchedPrimaryKey,
    unsigned char primaryKey[DDGSYNCCRYPTO_PRIMARY_KEY_SIZE]);

__declspec(dllexport) DDGSyncCryptoResult __cdecl ddgSyncPrepareForConnect(
    unsigned char primaryKey[DDGSYNCCRYPTO_PUBLIC_KEY_SIZE],
    unsigned char secretKey[DDGSYNCCRYPTO_PRIVATE_KEY_SIZE]);

__declspec(dllexport) DDGSyncCryptoResult __cdecl ddgSyncEncrypt(
    unsigned char* encryptedBytes,
    unsigned char* rawBytes,
    unsigned long long rawBytesLength,
    unsigned char* secretKey);

__declspec(dllexport) DDGSyncCryptoResult __cdecl ddgSyncDecrypt(
    unsigned char* rawBytes,
    unsigned char* encryptedBytes,
    unsigned long long encryptedBytesLength,
    unsigned char* secretKey);

__declspec(dllexport) DDGSyncCryptoResult __cdecl ddgSyncSeal(
    unsigned char* sealed,
    unsigned char primaryKey[DDGSYNCCRYPTO_PUBLIC_KEY_SIZE],
    unsigned char* message,
    unsigned long long messageLength);

__declspec(dllexport) DDGSyncCryptoResult __cdecl ddgSyncSealOpen(
    unsigned char* cyphertext,
    unsigned long long cypherTextLength,
    unsigned char primaryKey[DDGSYNCCRYPTO_PUBLIC_KEY_SIZE],
    unsigned char secretKey[DDGSYNCCRYPTO_PRIVATE_KEY_SIZE],
    unsigned char* rawBytes);

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


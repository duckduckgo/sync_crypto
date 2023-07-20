#include "resource.h"

//#define WIN32_LEAN_AND_MEAN
#include <windows.h>

extern "C" {
#include "../../native_lib/DDGSyncCrypto.h"
#include "DDGSyncCryptoSizes.h"
}

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


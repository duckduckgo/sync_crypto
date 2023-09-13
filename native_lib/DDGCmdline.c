#include <stdio.h>
#include <string.h>
#include <sodium.h>
#include "DDGSyncCrypto.h"

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Need 2 args, got %d\n", argc);
        return 1;
    }

    unsigned char primaryKey[DDGSYNCCRYPTO_PRIMARY_KEY_SIZE];
    unsigned char secretKey[DDGSYNCCRYPTO_SECRET_KEY_SIZE];
    unsigned char protectedSecretKey[DDGSYNCCRYPTO_PROTECTED_SECRET_KEY_SIZE];
    unsigned char passwordHash[DDGSYNCCRYPTO_HASH_SIZE];
    
    DDGSyncCryptoResult res;

    res = ddgSyncGenerateAccountKeys(primaryKey, secretKey, protectedSecretKey, passwordHash, argv[1], argv[2]);
    if (res != 0) {
        printf("errorcode: %d\n", res);
        return 2;
    }

    {
        unsigned char printbuf[sizeof(primaryKey)*2];
        sodium_bin2base64((char*)printbuf, sizeof(printbuf), primaryKey, sizeof(primaryKey), sodium_base64_VARIANT_ORIGINAL);
        printf("%s\n", printbuf);
    }
    {
        unsigned char printbuf[sizeof(secretKey)*2];
        sodium_bin2base64((char*)printbuf, sizeof(printbuf), secretKey, sizeof(secretKey), sodium_base64_VARIANT_ORIGINAL);
        printf("%s\n", printbuf);
    }
    {
        unsigned char printbuf[sizeof(protectedSecretKey)*2];
        sodium_bin2base64((char*)printbuf, sizeof(printbuf), protectedSecretKey, sizeof(protectedSecretKey), sodium_base64_VARIANT_ORIGINAL);
        printf("%s\n", printbuf);
    }
    {
        unsigned char printbuf[sizeof(passwordHash)*2];
        sodium_bin2base64((char*)printbuf, sizeof(printbuf), passwordHash, sizeof(passwordHash), sodium_base64_VARIANT_ORIGINAL);
        printf("%s\n", printbuf);
    }

    return 0;
}

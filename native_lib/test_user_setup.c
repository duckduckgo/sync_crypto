#include <algorithm>
#include <stdio.h>
#include <string.h>
#include <sodium.h>
#include "DDGSyncCrypto.h"

using std::min;

// Contexts must be 8 characters long but are otherwise arbitrary
#define DDGSYNC_STRETCHED_PRIMARY_KEY_CONTEXT "Stretchy"
#define DDGSYNC_PASSWORD_HASH_CONTEXT "Password"

enum DDGSyncCryptoSubkeyIds : int {
    DDGSyncCryptoPasswordHashSubkey = 1,
    DDGSyncCryptoStretchedPrimaryKeySubkey,
};

DDGSyncCryptoResult ddgSyncGenerateAccountKeysWithSecretKey(
    unsigned char primaryKey[DDGSYNCCRYPTO_SECRET_KEY_SIZE],
    unsigned char *secretKey,
    unsigned char protectedSecretKey[DDGSYNCCRYPTO_PROTECTED_SECRET_KEY_SIZE],
    unsigned char passwordHash[DDGSYNCCRYPTO_HASH_SIZE],
    const char *userId,
    const char *password) {

    unsigned char salt[crypto_pwhash_SALTBYTES];
    unsigned char stretchedPrimaryKey[DDGSYNCCRYPTO_STRETCHED_PRIMARY_KEY_SIZE];
    unsigned char nonceBytes[crypto_secretbox_NONCEBYTES];

    if (NULL == userId) {
        return DDGSYNCCRYPTO_INVALID_USERID;
    }

    if (NULL == password) {
        return DDGSYNCCRYPTO_INVALID_PASSWORD;
    }

    memset(salt, 0, crypto_pwhash_SALTBYTES);
    memcpy(salt, userId, min((size_t)crypto_pwhash_SALTBYTES, strlen(userId)));

    if (0 != crypto_pwhash(primaryKey,
                           DDGSYNCCRYPTO_PRIMARY_KEY_SIZE,
                           password,
                           strlen(password),
                           salt,
                           crypto_pwhash_OPSLIMIT_INTERACTIVE,
                           crypto_pwhash_MEMLIMIT_INTERACTIVE,
                           crypto_pwhash_ALG_DEFAULT)) {

        return DDGSYNCCRYPTO_CREATE_PRIMARY_KEY_FAILED;
    }

    if (0 != crypto_kdf_derive_from_key(passwordHash,
                                        DDGSYNCCRYPTO_HASH_SIZE,
                                        DDGSyncCryptoPasswordHashSubkey,
                                        DDGSYNC_PASSWORD_HASH_CONTEXT,
                                        primaryKey)) {

        return DDGSYNCCRYPTO_CREATE_PASSWORD_HASH_FAILED;
    }

    if (0 != crypto_kdf_derive_from_key(stretchedPrimaryKey,
                                        DDGSYNCCRYPTO_STRETCHED_PRIMARY_KEY_SIZE,
                                        DDGSyncCryptoStretchedPrimaryKeySubkey,
                                        DDGSYNC_STRETCHED_PRIMARY_KEY_CONTEXT,
                                        primaryKey)) {

        return DDGSYNCCRYPTO_CREATE_STRETCHED_PRIMARY_KEY_FAILED;
    }

    randombytes_buf(nonceBytes, crypto_secretbox_NONCEBYTES);

    if (0 != crypto_secretbox_easy(protectedSecretKey,
                                   secretKey,
                                   DDGSYNCCRYPTO_SECRET_KEY_SIZE,
                                   nonceBytes,
                                   stretchedPrimaryKey)) {
        return DDGSYNCCRYPTO_CREATE_PROTECTED_SECRET_KEY_FAILED;
    }

    memcpy(&protectedSecretKey[crypto_secretbox_MACBYTES + DDGSYNCCRYPTO_SECRET_KEY_SIZE], nonceBytes, crypto_secretbox_NONCEBYTES);

    return DDGSYNCCRYPTO_OK;
}

int main(int argc, char **argv) {
    if (argc != 4) {
        printf("Usage: %s secretkey username password\n", argv[0]);
        return 1;
    }

    unsigned char primaryKey[DDGSYNCCRYPTO_PRIMARY_KEY_SIZE];
    unsigned char secretKey[DDGSYNCCRYPTO_SECRET_KEY_SIZE];
    unsigned char protectedSecretKey[DDGSYNCCRYPTO_PROTECTED_SECRET_KEY_SIZE];
    unsigned char passwordHash[DDGSYNCCRYPTO_HASH_SIZE];
    
    DDGSyncCryptoResult res;
    {
        int err = sodium_base642bin(secretKey, DDGSYNCCRYPTO_SECRET_KEY_SIZE, argv[1], strlen(argv[1]), NULL, NULL, NULL, sodium_base64_VARIANT_ORIGINAL);
        if (err != 0) {
            printf("err %d b64 decoding secretKey\n", err);
            return 2;
        }
    }

    res = ddgSyncGenerateAccountKeysWithSecretKey(primaryKey, secretKey, protectedSecretKey, passwordHash, argv[2], argv[3]);
    if (res != 0) {
        printf("errorcode: %d\n", res);
        return 3;
    }

    printf("{\n");
    {
        unsigned char printbuf[sizeof(primaryKey)*2];
        sodium_bin2base64((char*)printbuf, sizeof(printbuf), primaryKey, sizeof(primaryKey), sodium_base64_VARIANT_ORIGINAL);
        printf("\t\"primaryKey\": \"%s\",\n", printbuf);
    }
    {
        unsigned char printbuf[sizeof(secretKey)*2];
        sodium_bin2base64((char*)printbuf, sizeof(printbuf), secretKey, sizeof(secretKey), sodium_base64_VARIANT_ORIGINAL);
        printf("\t\"secretKey\": \"%s\",\n", printbuf);
    }
    {
        unsigned char printbuf[sizeof(protectedSecretKey)*2];
        sodium_bin2base64((char*)printbuf, sizeof(printbuf), protectedSecretKey, sizeof(protectedSecretKey), sodium_base64_VARIANT_ORIGINAL);
        printf("\t\"protectedSecretKey\": \"%s\",\n", printbuf);
    }
    {
        unsigned char printbuf[sizeof(passwordHash)*2];
        sodium_bin2base64((char*)printbuf, sizeof(printbuf), passwordHash, sizeof(passwordHash), sodium_base64_VARIANT_ORIGINAL);
        printf("\t\"passwordHash\": \"%s\"\n", printbuf);
    }
    printf("}\n");

    return 0;
}

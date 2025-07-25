
#include <string.h>

#include "DDGSyncCrypto.h"
#include "sodium.h"

#ifndef min
#define min(x, y) (x < y) ? x : y
#endif

// Contexts must be 8 characters long but are otherwise arbitrary
#define DDGSYNC_STRETCHED_PRIMARY_KEY_CONTEXT "Stretchy"
#define DDGSYNC_PASSWORD_HASH_CONTEXT "Password"

typedef enum DDGSyncCryptoSubkeyIds {
    DDGSyncCryptoPasswordHashSubkey = 1,
    DDGSyncCryptoStretchedPrimaryKeySubkey,
} DDGSyncCryptoSubkeyIds;

DDGSyncCryptoResult ddgSyncGenerateAccountKeys(
    unsigned char primaryKey[DDGSYNCCRYPTO_PRIMARY_KEY_SIZE],
    unsigned char secretKey[DDGSYNCCRYPTO_SECRET_KEY_SIZE],
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
    memcpy(salt, userId, min(crypto_pwhash_SALTBYTES, strlen(userId)));

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

    randombytes_buf(secretKey, DDGSYNCCRYPTO_SECRET_KEY_SIZE);
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

DDGSyncCryptoResult ddgSyncPrepareForLogin(
    unsigned char *passwordHash,
    unsigned char *stretchedPrimaryKey,
    unsigned char primaryKey[DDGSYNCCRYPTO_PRIMARY_KEY_SIZE]) {

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

    return DDGSYNCCRYPTO_OK;
}

DDGSyncCryptoResult ddgSyncPrepareForConnect(
    unsigned char primaryKey[DDGSYNCCRYPTO_PUBLIC_KEY_SIZE],
    unsigned char secretKey[DDGSYNCCRYPTO_PRIVATE_KEY_SIZE]) {

    if (0 != crypto_box_keypair(primaryKey, secretKey)) {
        return DDGSYNCCRYPTO_CONNECT_KEY_FAILED;
    }

    return DDGSYNCCRYPTO_OK;
}

DDGSyncCryptoResult ddgSyncEncrypt(
    unsigned char *encryptedBytes,
    unsigned char *rawBytes,
    unsigned long long rawBytesLength,
    unsigned char *secretKey) {

    unsigned char nonceBytes[crypto_secretbox_NONCEBYTES];

    randombytes_buf(nonceBytes, crypto_secretbox_NONCEBYTES);

    if (0 != crypto_secretbox_easy(encryptedBytes, rawBytes, rawBytesLength, nonceBytes, secretKey)) {
        return DDGSYNCCRYPTO_ENCRYPTION_FAILED;
    }

    memcpy(&encryptedBytes[crypto_secretbox_MACBYTES + rawBytesLength], nonceBytes, crypto_secretbox_NONCEBYTES);

    return DDGSYNCCRYPTO_OK;
}

extern DDGSyncCryptoResult ddgSyncDecrypt(
    unsigned char *rawBytes,
    unsigned char *encryptedBytes,
    unsigned long long encryptedBytesLength,
    unsigned char *secretKey) {

    unsigned char nonceBytes[crypto_secretbox_NONCEBYTES];

    memcpy(nonceBytes, &encryptedBytes[encryptedBytesLength - crypto_secretbox_NONCEBYTES], crypto_secretbox_NONCEBYTES);

    if (0 != crypto_secretbox_open_easy(rawBytes, encryptedBytes, encryptedBytesLength - crypto_secretbox_NONCEBYTES, nonceBytes, secretKey)) {
        return DDGSYNCCRYPTO_DECRYPTION_FAILED;
    }

    return DDGSYNCCRYPTO_OK;
}

DDGSyncCryptoResult ddgSyncSeal(
    unsigned char *sealed,
    unsigned char primaryKey[DDGSYNCCRYPTO_PUBLIC_KEY_SIZE],
    unsigned char *message,
    unsigned long long messageLength) {

    unsigned char* output = (unsigned char*)malloc((crypto_box_SEALBYTES + (size_t)messageLength) * sizeof(unsigned char));
    if (!output) {
        return DDGSYNCCRYPTO_SEAL_FAILED;
    }

    DDGSyncCryptoResult result;
    if (crypto_box_seal(output, message, messageLength, primaryKey) != 0) {
        result = DDGSYNCCRYPTO_SEAL_FAILED;
    }
    else {
        memcpy(sealed, output, crypto_box_SEALBYTES + (size_t)messageLength);
        result = DDGSYNCCRYPTO_OK;
    }
    free(output);
    return result;
}

DDGSyncCryptoResult ddgSyncSealOpen(
    unsigned char *cyphertext,
    unsigned long long cypherTextLength,
    unsigned char primaryKey[DDGSYNCCRYPTO_PUBLIC_KEY_SIZE],
    unsigned char secretKey[DDGSYNCCRYPTO_PRIVATE_KEY_SIZE],
    unsigned char *rawBytes) {

    unsigned char* decrypted = (unsigned char*)malloc(((size_t)cypherTextLength - crypto_box_SEALBYTES) * sizeof(unsigned char));
    if (!decrypted) {
        return DDGSYNCCRYPTO_SEAL_FAILED;
    }

    DDGSyncCryptoResult result;
    if (crypto_box_seal_open(decrypted, cyphertext, cypherTextLength, primaryKey, secretKey) != 0) {
        result = DDGSYNCCRYPTO_SEAL_FAILED;
    }
    else {
        memcpy(rawBytes, decrypted, (size_t)cypherTextLength - crypto_box_SEALBYTES);
        result = DDGSYNCCRYPTO_OK;
    }

    free(decrypted);
    return result;
}

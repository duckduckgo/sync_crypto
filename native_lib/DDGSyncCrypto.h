#ifdef _WIN32
#include <sodium.h>
#else
#include "sodium/crypto_pwhash.h"
#include "sodium/crypto_box.h"
#include "sodium/crypto_secretbox.h"
#endif

#ifndef DDGSyncCrypto_h
#define DDGSyncCrypto_h

typedef enum : int {
    DDGSYNCCRYPTO_HASH_SIZE = 32,
    DDGSYNCCRYPTO_PRIMARY_KEY_SIZE = 32,
    DDGSYNCCRYPTO_SECRET_KEY_SIZE = 32,
    DDGSYNCCRYPTO_STRETCHED_PRIMARY_KEY_SIZE = 32,
    DDGSYNCCRYPTO_PROTECTED_SECRET_KEY_SIZE = (crypto_secretbox_MACBYTES + DDGSYNCCRYPTO_STRETCHED_PRIMARY_KEY_SIZE + crypto_secretbox_NONCEBYTES),
    DDGSYNCCRYPTO_ENCRYPTED_EXTRA_BYTES_SIZE = (crypto_secretbox_MACBYTES + crypto_secretbox_NONCEBYTES),
    DDGSYNCCRYPTO_PUBLIC_KEY_SIZE = crypto_box_PUBLICKEYBYTES,
    DDGSYNCCRYPTO_PRIVATE_KEY_SIZE = crypto_box_SECRETKEYBYTES,
    DDGSYNCCRYPTO_SEAL_EXTRA_BYTES_SIZE = crypto_box_SEALBYTES
} DDGSyncCryptoSizes;

typedef enum : int {
    DDGSYNCCRYPTO_OK,
    DDGSYNCCRYPTO_UNKNOWN_ERROR,
    DDGSYNCCRYPTO_INVALID_USERID,
    DDGSYNCCRYPTO_INVALID_PASSWORD,
    DDGSYNCCRYPTO_CREATE_PRIMARY_KEY_FAILED,
    DDGSYNCCRYPTO_CREATE_PASSWORD_HASH_FAILED,
    DDGSYNCCRYPTO_CREATE_STRETCHED_PRIMARY_KEY_FAILED,
    DDGSYNCCRYPTO_CREATE_PROTECTED_SECRET_KEY_FAILED,
    DDGSYNCCRYPTO_ENCRYPTION_FAILED,
    DDGSYNCCRYPTO_DECRYPTION_FAILED,
    DDGSYNCCRYPTO_CONNECT_KEY_FAILED,
    DDGSYNCCRYPTO_SEAL_FAILED,
} DDGSyncCryptoResult;

/**
 * Used to create data needed to create an account.  Once the server returns a JWT, store the primary and secret key.
 *
 * @param primaryKey OUT - store this.  In combination with user id, this is the recovery key.
 * @param secretKey OUT - store this. This is used to encrypt an decrypt e2e data.
 * @param protectedSecretKey OUT - do not store this.  Send to /sign up endpoint.
 * @param passwordHash OUT - do not store this.  Send to /signup endpoint.
 * @param userId IN
 * @param password IN
 */
extern DDGSyncCryptoResult ddgSyncGenerateAccountKeys(
    unsigned char primaryKey[DDGSYNCCRYPTO_PRIMARY_KEY_SIZE],
    unsigned char secretKey[DDGSYNCCRYPTO_SECRET_KEY_SIZE],
    unsigned char protectedSecretKey[DDGSYNCCRYPTO_PROTECTED_SECRET_KEY_SIZE],
    unsigned char passwordHash[DDGSYNCCRYPTO_HASH_SIZE],
    const char *userId,
    const char *password
);

/**
 * Prepare keys for calling /login when using a recovery code.  Once the protected secret key has been retrieved, use `ddgSyncDecrypt` to extract the secret key, using the stretched primary key as the secret key for the decryption.
 *
 * @param passwordHash OUT
 * @param stretchedPrimaryKey OUT
 * @param primaryKey IN
 */
extern DDGSyncCryptoResult ddgSyncPrepareForLogin(
    unsigned char *passwordHash,
    unsigned char *stretchedPrimaryKey,
    unsigned char primaryKey[DDGSYNCCRYPTO_PRIMARY_KEY_SIZE]
);

/**
 * Creates a secret key and a corresponding public key used during connect flow.
 *
 * @param primaryKey OUT
 * @param secretKey OUT
 */
DDGSyncCryptoResult ddgSyncPrepareForConnect(
    unsigned char primaryKey[DDGSYNCCRYPTO_PUBLIC_KEY_SIZE],
    unsigned char secretKey[DDGSYNCCRYPTO_PRIVATE_KEY_SIZE]
);

/**
 * @param encryptedBytes OUT - the size of this should be equal to the size of data to encrypt, plus crypto_secretbox_MACBYTES (16 bytes) plus crypto_secretbox_NONCEBYTES (16).  The output will be the encrypted data, plus MAC, plus nonce.
 * @param rawBytes IN - the data to be encrypted.  Should be of size specified by rawDataLength
 * @param rawBytesLength IN - the length of the data to be encrypted
 * @param secretKey IN - the secret key (assumed to be of length DDGSYNCCRYPTO_SECRET_KEY_SIZE)
 */
extern DDGSyncCryptoResult ddgSyncEncrypt(
    unsigned char *encryptedBytes,
    unsigned char *rawBytes,
    unsigned long long rawBytesLength,
    unsigned char *secretKey
);

/**
 * @param rawBytes OUT - the decrypted data.  Should be of size specified by encryptedBytesLength - crypto_secretbox_MACBYTES (16 bytes) plus crypto_secretbox_NONCEBYTES (16)
 * @param encryptedBytes INT - the bytes to decrypt
 * @param rawBytesLength IN - the length of the data to be decrypted
 * @param secretKey IN - the secret key (assumed to be of length DDGSYNCCRYPTO_SECRET_KEY_SIZE)
 */
extern DDGSyncCryptoResult ddgSyncDecrypt(
    unsigned char *rawBytes,
    unsigned char *encryptedBytes,
    unsigned long long encryptedBytesLength,
    unsigned char *secretKey
);

/**
 * Used to encrypt a message. It's expected to use a public key generated using ddgSyncPrepareForConnect.
 *
 * @param sealed OUT - the size of this should be equal to the size of data to encrypt, plus crypto_box_SEALBYTES.
 * @param primaryKey IN - the key used for encryption (assumed to be of length DDGSYNCCRYPTO_PUBLIC_KEY_SIZE)
 * @param message IN - the data to be encrypted.  Should be of size specified by rawDataLength
 * @param messageLength IN - the length of the data to be encrypted
 */
extern DDGSyncCryptoResult ddgSyncSeal(
    unsigned char *sealed,
    unsigned char primaryKey[DDGSYNCCRYPTO_PUBLIC_KEY_SIZE],
    unsigned char *message,
    unsigned long long messageLength
);

/**
 * Used to decrypt a message from another device. It's expected to use a key pair generated using ddgSyncPrepareForConnect.
 *
 * @param cyphertext OUT - the decrypted data.  Should be of size specified by cypherTextLength - crypto_box_SEALBYTES.
 * @param cypherTextLength IN - the length of the data to be decrypted
 * @param primaryKey IN - the key used for encryption (assumed to be of length DDGSYNCCRYPTO_PUBLIC_KEY_SIZE)
 * @param secretKey INT - the secret key (assumed to be of length DDGSYNCCRYPTO_PRIVATE_KEY_SIZE)
 * @param rawBytes INT - the bytes to decrypt
 */
extern DDGSyncCryptoResult ddgSyncSealOpen(
    unsigned char *cyphertext,
    unsigned long long cypherTextLength,
    unsigned char primaryKey[DDGSYNCCRYPTO_PUBLIC_KEY_SIZE],
    unsigned char secretKey[DDGSYNCCRYPTO_PRIVATE_KEY_SIZE],
    unsigned char *rawBytes
);

#endif /* DDGSyncCrypto_h */

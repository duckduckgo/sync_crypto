#include <algorithm>
#include <stdio.h>
#include <string.h>
#include <sodium.h>
#include "../native_lib/DDGSyncCrypto.h" // only require type defs, not actual implementation

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

size_t from_base64(unsigned char bin[], const char *input)
{
    const size_t input_len = strlen(input);
    const size_t bin_maxlen = input_len * 3 / 4;

    size_t len = 0;
    int err = sodium_base642bin(bin, bin_maxlen, input, input_len, NULL, &len, NULL, sodium_base64_VARIANT_ORIGINAL);
    return (err != 0) ? -1 : len;
}

void to_base64(char b64[], const unsigned char * const bin, const size_t bin_len)
{
    sodium_bin2base64(b64, bin_len * 2, bin, bin_len, sodium_base64_VARIANT_ORIGINAL);
}

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s username password secret_key\n", argv[0]);
        fprintf(stderr, "  secret_key must be encoded in base64, with %d bytes.\n", DDGSYNCCRYPTO_SECRET_KEY_SIZE);
        return 1;
    }
    if (sodium_init() == -1) {
        fprintf(stderr, "Error: failed to run sodium_init()\n");
        return 1;
    }

    // Parse secret_key
    const char *b64_secret_key_input = argv[3];
    unsigned char bin_secret_key[strlen(b64_secret_key_input) * 3 / 4];
    memset(bin_secret_key, 0, sizeof(bin_secret_key));
    
    const size_t bin_secret_key_len = from_base64(bin_secret_key, b64_secret_key_input);
    if (bin_secret_key_len == -1) {
        fprintf(stderr, "Error: failed to decode secret_key! Must be a value base64 string.\n");
        return 2;
    }
    if (bin_secret_key_len != DDGSYNCCRYPTO_SECRET_KEY_SIZE) {
        fprintf(stderr, "Error: secret_key MUST have %d bytes! Input has %ld bytes.\n", DDGSYNCCRYPTO_SECRET_KEY_SIZE, bin_secret_key_len);
        return 2;
    }

    // Generate remaining secrets
    const char *username = argv[1];
    const char *password = argv[2];
    unsigned char primary_key[DDGSYNCCRYPTO_PRIMARY_KEY_SIZE] = {0};
    unsigned char protected_secret_key[DDGSYNCCRYPTO_PROTECTED_SECRET_KEY_SIZE] = {0};
    unsigned char hashed_password[DDGSYNCCRYPTO_HASH_SIZE] = {0};
    DDGSyncCryptoResult res = ddgSyncGenerateAccountKeysWithSecretKey(primary_key, bin_secret_key, protected_secret_key, hashed_password, username, password);
    if (res != 0) {
        fprintf(stderr, "Error generating account keys: %d\n", res);
        return 3;
    }

    char b64_primary_key[sizeof(primary_key) * 2];
    char b64_secret_key[bin_secret_key_len * 2];
    char b64_protected_secret_key[sizeof(protected_secret_key) * 2];
    char b64_hashed_password[sizeof(hashed_password) * 2];
    
    memset(b64_primary_key, 0, sizeof(b64_primary_key));
    memset(b64_secret_key, 0, sizeof(b64_secret_key));
    memset(b64_protected_secret_key, 0, sizeof(b64_protected_secret_key));
    memset(b64_hashed_password, 0, sizeof(b64_hashed_password));

    to_base64(b64_primary_key, primary_key, sizeof(primary_key));
    to_base64(b64_secret_key, bin_secret_key, bin_secret_key_len);
    to_base64(b64_protected_secret_key, protected_secret_key, sizeof(protected_secret_key));
    to_base64(b64_hashed_password, hashed_password, sizeof(hashed_password));
    printf("{\n");
    printf("  \"primary_key\": \"%s\",\n", b64_primary_key);
    printf("  \"secret_key\": \"%s\",\n", b64_secret_key);
    printf("  \"protected_encryption_key\": \"%s\",\n", b64_protected_secret_key);
    printf("  \"hashed_password\": \"%s\"\n", b64_hashed_password);
    printf("}\n");

    return 0;
}

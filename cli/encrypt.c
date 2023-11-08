#include <algorithm>
#include <stdio.h>
#include <string.h>
#include <sodium.h>
#include "../native_lib/DDGSyncCrypto.h"

using std::min;

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
    if (argc != 3) {
        fprintf(stderr, "Usage: %s msg_to_encrypt encryption_key\n", argv[0]);
        fprintf(stderr, "  encryption_key must be encoded in base64.\n");
        return 1;
    }
    if (sodium_init() == -1) {
        fprintf(stderr, "Error: failed to run sodium_init()\n");
        return 1;
    }

    // Parse encryption_key
    const char *b64_key = argv[2];
    unsigned char bin_key[strlen(b64_key) * 3 / 4];
    memset(bin_key, 0, sizeof(bin_key));
    const size_t bin_key_len = from_base64(bin_key, b64_key);
    if (bin_key_len == -1) {
        fprintf(stderr, "Error: failed to decode encrypted_key! Must be a value base64 string.\n");
        return 2;
    }

    char* debug = getenv("DEBUG");
    if (debug) {
        printf("---\n");
        printf("key len = %ld (base64) = %ld (bytes)\n", strlen(b64_key), bin_key_len);
        {
            char printbuf[bin_key_len * 2];
            memset(printbuf, 0, sizeof(printbuf));
            to_base64(printbuf, bin_key, bin_key_len);
            printf("re-encoded key: '%s'\n", printbuf);
        }
        printf("---\n");
    }

    {
        const char *raw_msg = argv[1];
        const size_t raw_msg_len = strlen(raw_msg);
        const size_t encrypted_len = raw_msg_len + crypto_secretbox_MACBYTES + crypto_secretbox_NONCEBYTES;
        unsigned char encrypted[encrypted_len];
        memset(encrypted, 0, sizeof(encrypted));
        const int err = ddgSyncEncrypt(encrypted, (unsigned char*)raw_msg, raw_msg_len, bin_key);
        if (err != 0) {
            fprintf(stderr, "Error: (%d) failed to encrypt message!\n", err);
            return 2;
        }

        char b64_encrypted[encrypted_len * 2];
        memset(b64_encrypted, 0, sizeof(b64_encrypted));
        to_base64(b64_encrypted, encrypted, encrypted_len);
        printf("%s\n", b64_encrypted);
    }

    return 0;
}

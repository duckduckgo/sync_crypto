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
    if (sodium_init() == -1) {
        fprintf(stderr, "Error: failed to run sodium_init()\n");
        return 1;
    }

    unsigned char bin_publickey[crypto_box_PUBLICKEYBYTES];
    unsigned char bin_secretkey[crypto_box_SECRETKEYBYTES];
    if (crypto_box_keypair(bin_publickey, bin_secretkey) == -1) {
        fprintf(stderr, "Error: failed to call crypto_box_keypair.\n");
        return 2;
    }

    {
        char b64[crypto_box_PUBLICKEYBYTES * 2];
        memset(b64, 0, sizeof(b64));
        to_base64(b64, bin_publickey, crypto_box_PUBLICKEYBYTES);
        printf("public (%d bytes): %s\n", crypto_box_PUBLICKEYBYTES, b64);
    }
    {
        char b64[crypto_box_SECRETKEYBYTES * 2];
        memset(b64, 0, sizeof(b64));
        to_base64(b64, bin_secretkey, crypto_box_SECRETKEYBYTES);
        printf("secret (%d bytes): %s\n", crypto_box_SECRETKEYBYTES, b64);
    }

    return 0;
}

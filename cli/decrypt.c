#include <algorithm>
#include <stdio.h>
#include <string.h>
#include <sodium.h> // /usr/include/sodium.h
#include "../native_lib/DDGSyncCrypto.h"

using std::min;

size_t from_base64(unsigned char bin[], const char *input)
{
    const size_t input_len = strlen(input);
    const size_t bin_maxlen = input_len * 3 / 4;

    size_t len = 0;
    int err = sodium_base642bin(
        bin, bin_maxlen ,
        input, input_len,
        NULL, &len, NULL, sodium_base64_VARIANT_ORIGINAL
    );

    return (err != 0) ? -1 : len;
}

void to_base64(char b64[], const unsigned char * const bin, const size_t bin_len)
{
    sodium_bin2base64(b64, bin_len * 2, bin, bin_len, sodium_base64_VARIANT_ORIGINAL);
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s encrypted_data encryption_key\n", argv[0]);
        fprintf(stderr, "  Both inputs must be encoded in base64\n");
        return 1;
    }
    if (sodium_init() == -1) {
        return 1;
    }

    // Parse encrypted_data
    const char *b64_msg = argv[1];
    unsigned char bin_msg[strlen(b64_msg) * 3 / 4];
    const size_t bin_msg_len = from_base64(bin_msg, b64_msg);
    if (bin_msg_len == -1) {
        fprintf(stderr, "Error decoding encrypted_data! Must be a value base64 string.\n");
        return 2;
    }

    // Parse encryption_key
    const char *b64_key = argv[2];
    unsigned char bin_key[strlen(b64_key) * 3 / 4];
    const size_t bin_key_len = from_base64(bin_key, b64_key);
    if (bin_key_len == -1) {
        fprintf(stderr, "Error decoding encryption_key! Must be a value base64 string.\n");
        return 2;
    }

    {
        printf("msg len: %ld => %ld\n", strlen(b64_msg), bin_msg_len);
        char printbuf[bin_msg_len * 2];
        to_base64(printbuf, bin_msg, bin_msg_len);
        printf("re-encoded input: '%s'\n", printbuf);
    }
    {
        printf("key len: %ld => %ld\n", strlen(b64_key),   bin_key_len);
        char printbuf[bin_key_len * 2];
        to_base64(printbuf, bin_key, bin_key_len);
        printf("re-encoded key: '%s'\n", printbuf);
    }

    return 0;
}

#pragma once

#include <sodium.h>

#ifndef DDGChaCha20Poly1305_h
#define DDGChaCha20Poly1305_h

typedef enum DDGChaCha20Poly1305Sizes {
    DDGCHACHA20POLY1305_KEY_SIZE = crypto_aead_chacha20poly1305_ietf_KEYBYTES,
    DDGCHACHA20POLY1305_NONCE_SIZE = crypto_aead_chacha20poly1305_ietf_NPUBBYTES,
    DDGCHACHA20POLY1305_TAG_SIZE = crypto_aead_chacha20poly1305_ietf_ABYTES,
} DDGChaCha20Poly1305Sizes;

typedef enum DDGChaCha20Poly1305Result {
    DDGCHACHA20POLY1305_UNKNOWN_ERROR,
    DDGCHACHA20POLY1305_OK,
    DDGCHACHA20POLY1305_INVALID_KEY,
    DDGCHACHA20POLY1305_INVALID_DATA,
    DDGCHACHA20POLY1305_INVALID_NONCE,
    DDGCHACHA20POLY1305_DECRYPTION_FAILED,
} DDGChaCha20Poly1305Result;

/**
 * Decrypt data using ChaCha20Poly1305.
 * The nonce is provided separately, while the tag is part of encryptedBytes.
 *
 * @param rawBytes OUT - the decrypted data. Should be of size encryptedBytesLength - DDGCHACHA20POLY1305_TAG_SIZE
 * @param encryptedBytes IN - the encrypted data with authentication tag appended
 * @param encryptedBytesLength IN - the length of the encrypted data (including tag)
 * @param nonce IN - the nonce (assumed to be of length DDGCHACHA20POLY1305_NONCE_SIZE)
 * @param key IN - the secret key (assumed to be of length DDGCHACHA20POLY1305_KEY_SIZE)
 */
extern DDGChaCha20Poly1305Result ddgChaCha20Poly1305Decrypt(
    unsigned char *rawBytes,
    unsigned char *encryptedBytes,
    unsigned long long encryptedBytesLength,
    unsigned char nonce[DDGCHACHA20POLY1305_NONCE_SIZE],
    unsigned char key[DDGCHACHA20POLY1305_KEY_SIZE]
);

extern int ddgChaCha20Poly1305Get_DDGCHACHA20POLY1305_KEY_SIZE();

extern int ddgChaCha20Poly1305Get_DDGCHACHA20POLY1305_NONCE_SIZE();

extern int ddgChaCha20Poly1305Get_DDGCHACHA20POLY1305_TAG_SIZE();

#endif /* DDGChaCha20Poly1305_h */



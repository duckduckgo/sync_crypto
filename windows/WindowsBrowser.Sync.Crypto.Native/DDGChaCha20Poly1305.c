#include "DDGChaCha20Poly1305.h"

#include <sodium.h>
#include <string.h>

DDGChaCha20Poly1305Result ddgChaCha20Poly1305Decrypt(
    unsigned char *rawBytes,
    unsigned char *encryptedBytes,
    unsigned long long encryptedBytesLength,
    unsigned char nonce[DDGCHACHA20POLY1305_NONCE_SIZE],
    unsigned char key[DDGCHACHA20POLY1305_KEY_SIZE]) {
    
    unsigned long long decryptedLength;
    
    if (NULL == rawBytes) {
        return DDGCHACHA20POLY1305_INVALID_DATA;
    }
    
    if (NULL == encryptedBytes) {
        return DDGCHACHA20POLY1305_INVALID_DATA;
    }
    
    if (NULL == nonce) {
        return DDGCHACHA20POLY1305_INVALID_NONCE;
    }
    
    if (NULL == key) {
        return DDGCHACHA20POLY1305_INVALID_KEY;
    }
    
    if (encryptedBytesLength < DDGCHACHA20POLY1305_TAG_SIZE) {
        return DDGCHACHA20POLY1305_INVALID_DATA;
    }
    
    if (0 != crypto_aead_chacha20poly1305_ietf_decrypt(
        rawBytes,
        &decryptedLength,
        NULL, // nsec (not used for this algo according to libsodium docs)
        encryptedBytes,
        encryptedBytesLength,
		NULL, // additional data (not used)
		0,    // additional data length (not used)
        nonce,
        key)) {
        return DDGCHACHA20POLY1305_DECRYPTION_FAILED;
    }
    
    return DDGCHACHA20POLY1305_OK;
}

int ddgChaCha20Poly1305Get_DDGCHACHA20POLY1305_KEY_SIZE() {
	return DDGCHACHA20POLY1305_KEY_SIZE;
}

int ddgChaCha20Poly1305Get_DDGCHACHA20POLY1305_NONCE_SIZE() {
	return DDGCHACHA20POLY1305_NONCE_SIZE;
}

int ddgChaCha20Poly1305Get_DDGCHACHA20POLY1305_TAG_SIZE() {
	return DDGCHACHA20POLY1305_TAG_SIZE;
}

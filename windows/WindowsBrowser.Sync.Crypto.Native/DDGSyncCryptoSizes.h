// Since there's no easy way to map DDGSyncCryptoSizes to a C# enum without duplicating code.
// Follow the same approach as on Android, export the enum/consts as methods.

extern int ddgSyncGet_DDGSYNCCRYPTO_HASH_SIZE();

extern int ddgSyncGet_DDGSYNCCRYPTO_PRIMARY_KEY_SIZE();

extern int ddgSyncGet_DDGSYNCCRYPTO_SECRET_KEY_SIZE();

extern int ddgSyncGet_DDGSYNCCRYPTO_STRETCHED_PRIMARY_KEY_SIZE();

extern int ddgSyncGet_DDGSYNCCRYPTO_PROTECTED_SECRET_KEY_SIZE();

extern int ddgSyncGet_DDGSYNCCRYPTO_ENCRYPTED_EXTRA_BYTES_SIZE();

extern int ddgSyncGet_DDGSYNCCRYPTO_PUBLIC_KEY_SIZE();

extern int ddgSyncGet_DDGSYNCCRYPTO_PRIVATE_KEY_SIZE();

extern int ddgSyncGet_DDGSYNCCRYPTO_SEAL_EXTRA_BYTES_SIZE();
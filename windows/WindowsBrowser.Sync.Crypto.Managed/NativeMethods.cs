using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace WindowsBrowser.Sync.Crypto.Managed
{
    public class NativeMethods
    {
        private const string DllPath = "WindowsBrowser.Sync.Crypto.Native.dll";

        [DllImport(DllPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ddgSyncGet_DDGSYNCCRYPTO_HASH_SIZE();

        [DllImport(DllPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ddgSyncGet_DDGSYNCCRYPTO_PRIMARY_KEY_SIZE();

        [DllImport(DllPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ddgSyncGet_DDGSYNCCRYPTO_SECRET_KEY_SIZE();

        [DllImport(DllPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ddgSyncGet_DDGSYNCCRYPTO_STRETCHED_PRIMARY_KEY_SIZE();

        [DllImport(DllPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ddgSyncGet_DDGSYNCCRYPTO_PROTECTED_SECRET_KEY_SIZE();

        [DllImport(DllPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ddgSyncGet_DDGSYNCCRYPTO_ENCRYPTED_EXTRA_BYTES_SIZE();

        [DllImport(DllPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ddgSyncGet_DDGSYNCCRYPTO_PUBLIC_KEY_SIZE();

        [DllImport(DllPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ddgSyncGet_DDGSYNCCRYPTO_PRIVATE_KEY_SIZE();

        [DllImport(DllPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern int ddgSyncGet_DDGSYNCCRYPTO_SEAL_EXTRA_BYTES_SIZE();

        [DllImport(DllPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern DDGSyncCryptoResult ddgSyncGenerateAccountKeys(
            byte[] primaryKey,
            byte[] secretKey,
            byte[] protectedSecretKey,
            byte[] passwordHash,
            string userId,
            string password);

        [DllImport(DllPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern DDGSyncCryptoResult ddgSyncPrepareForLogin(
            byte[] passwordHash,
            byte[] stretchedPrimaryKey,
            byte[] primaryKey
        );

        [DllImport(DllPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern DDGSyncCryptoResult ddgSyncPrepareForConnect(
            byte[] primaryKey,
            byte[] secretKey
        );

        [DllImport(DllPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern DDGSyncCryptoResult ddgSyncEncrypt(
            byte[] encryptedBytes,
            byte[] rawBytes,
            ulong rawBytesLength,
            byte[] secretKey
        );

        [DllImport(DllPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern DDGSyncCryptoResult ddgSyncDecrypt(
            byte[] rawBytes,
            byte[] encryptedBytes,
            ulong encryptedBytesLength,
            byte[] secretKey
        );

        [DllImport(DllPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern DDGSyncCryptoResult ddgSyncSeal(
            byte[] sealedBytes,
            byte[] primaryKey,
            byte[] message,
            ulong messageLength
        );

        [DllImport(DllPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern DDGSyncCryptoResult ddgSyncSealOpen(
            byte[] cyphertext,
            ulong cypherTextLength,
            byte[] primaryKey,
            byte[] secretKey,
            byte[] rawBytes
        );
    }
}

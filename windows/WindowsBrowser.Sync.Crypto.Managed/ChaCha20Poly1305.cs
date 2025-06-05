using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using static WindowsBrowser.Sync.Crypto.Managed.NativeMethods;

namespace WindowsBrowser.Sync.Crypto.Managed
{
    public class ChaCha20Poly1305InvalidDataException : Exception
    {
    }

    public class ChaCha20Poly1305InvalidNonceException : Exception
    {
    }

    public class ChaCha20Poly1305InvalidKeyException : Exception
    {
    }

    public class ChaCha20Poly1305DecryptionFailedException : Exception
    {
    }

    public static class ChaCha20Poly1305
    {
        [DllImport(DllPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern int ddgChaCha20Poly1305Get_DDGCHACHA20POLY1305_KEY_SIZE();

        [DllImport(DllPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern int ddgChaCha20Poly1305Get_DDGCHACHA20POLY1305_NONCE_SIZE();

        [DllImport(DllPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern int ddgChaCha20Poly1305Get_DDGCHACHA20POLY1305_TAG_SIZE();

        [DllImport(DllPath, CallingConvention = CallingConvention.Cdecl)]
        private static extern DDGChaCha20Poly1305Result ddgChaCha20Poly1305Decrypt(
            byte[] rawBytes,
            byte[] encryptedBytes,
            ulong encryptedBytesLength,
            byte[] nonce,
            byte[] key
        );

        public static byte[] Decrypt(byte[] ciphertext, byte[] nonce, byte[] key)
        {
            var tagSize = ddgChaCha20Poly1305Get_DDGCHACHA20POLY1305_TAG_SIZE();

            ThrowForInvalidParameters(ciphertext, nonce, key, tagSize);

            var decrypted = new byte[ciphertext.Length - tagSize];
            var chaChaResult = ddgChaCha20Poly1305Decrypt(
                decrypted,
                ciphertext,
                (ulong)ciphertext.Length,
                nonce,
                key);

            if (chaChaResult == DDGChaCha20Poly1305Result.Ok)
            {
                return decrypted;
            }
            else
            {
                throw new ChaCha20Poly1305DecryptionFailedException();
            }
        }

        private static void ThrowForInvalidParameters(byte[] ciphertext, byte[] nonce, byte[] key, int tagSize)
        {
            if (ciphertext == null || ciphertext.Length <= tagSize)
            {
                throw new ChaCha20Poly1305InvalidDataException();
            }

            if (nonce == null || nonce.Length != ddgChaCha20Poly1305Get_DDGCHACHA20POLY1305_NONCE_SIZE())
            {
                throw new ChaCha20Poly1305InvalidNonceException();
            }

            if (key == null || key.Length != ddgChaCha20Poly1305Get_DDGCHACHA20POLY1305_KEY_SIZE())
            {
                throw new ChaCha20Poly1305InvalidKeyException();
            }
        }
    }
}

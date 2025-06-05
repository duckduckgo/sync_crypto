using System;

namespace WindowsBrowser.Sync.Crypto.Managed.Tests
{
    [TestClass]
    public sealed class ChaCha20Poly1305Tests
    {
        [TestMethod]
        public void CanDecryptionWithChaCha20Poly1305UsingLibSodium()
        {
            // Prepare test data by using built in .NET ChaCha20Poly1305 implementation
            var key = new byte[] { 233, 143, 55, 215, 244, 225, 250, 67, 61, 25, 48, 77, 194, 37, 128, 66, 9, 14, 45, 29, 126, 234, 118, 112, 212, 31, 115, 141, 8, 114, 150, 96 };
            var nonce = new byte[12] { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
            var text = new byte[32]
            {
                1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32
            };
            var chiper = new byte[32];
            var tag = new byte[16];
            var netChaCha = new System.Security.Cryptography.ChaCha20Poly1305(key);
            netChaCha.Encrypt(nonce, text, chiper, tag, null);

            // Libsodium expects the tag to be appended to the end of the ciphertext
            var decrypted = new byte[32];
            var encryptedBytesWithTag = new byte[chiper.Length + tag.Length];
            Buffer.BlockCopy(chiper, 0, encryptedBytesWithTag, 0, chiper.Length);
            Buffer.BlockCopy(tag, 0, encryptedBytesWithTag, chiper.Length, tag.Length);

            var result = ChaCha20Poly1305.Decrypt(encryptedBytesWithTag, nonce, key);
            CollectionAssert.AreEqual(result, text);
        }

        [TestMethod]
        public void WillReportFailureForInvalidEncryptedData()
        {
            // Prepare test data by using built in .NET ChaCha20Poly1305 implementation
            var key = new byte[] { 233, 143, 55, 215, 244, 225, 250, 67, 61, 25, 48, 77, 194, 37, 128, 66, 9, 14, 45, 29, 126, 234, 118, 112, 212, 31, 115, 141, 8, 114, 150, 96 };
            var nonce = new byte[12] { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
            var text = new byte[32]
            {
                1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32
            };
            var chiper = new byte[32];
            var tag = new byte[16];
            var netChaCha = new System.Security.Cryptography.ChaCha20Poly1305(key);
            netChaCha.Encrypt(nonce, text, chiper, tag, null);

            // #1 scenario: only provide the tag
            var decrypted = new byte[32];
            var encryptedBytesWithTag = new byte[tag.Length];
            Buffer.BlockCopy(tag, 0, encryptedBytesWithTag, 0, tag.Length);
            Assert.ThrowsException<ChaCha20Poly1305InvalidDataException>(() => ChaCha20Poly1305.Decrypt(encryptedBytesWithTag, nonce, key));

            //#2 scenario: only few bytes provided
            Buffer.BlockCopy(tag, 0, encryptedBytesWithTag, 0, tag.Length);
            encryptedBytesWithTag = encryptedBytesWithTag[1..4];
            Assert.ThrowsException<ChaCha20Poly1305InvalidDataException>(() => ChaCha20Poly1305.Decrypt(encryptedBytesWithTag, nonce, key));

            //#3 scenario: data null
            Assert.ThrowsException<ChaCha20Poly1305InvalidDataException>(() => ChaCha20Poly1305.Decrypt(encryptedBytesWithTag, nonce, key));

            //#4 scenario: nonce invalid
            var invalidNonce = new byte[11]; // Should be 12 bytes
            chiper = new byte[64];
            Assert.ThrowsException<ChaCha20Poly1305InvalidNonceException>(() => ChaCha20Poly1305.Decrypt(chiper, invalidNonce, key));

            //#5 scenario: key invalid
            var invalidKey = new byte[31]; // Should be 32 bytes
            chiper = new byte[32];
            Assert.ThrowsException<ChaCha20Poly1305InvalidKeyException>(() => ChaCha20Poly1305.Decrypt(chiper, nonce, invalidKey));

            //#6 scenario: key null
            chiper = new byte[32];
            Assert.ThrowsException<ChaCha20Poly1305InvalidKeyException>(() => ChaCha20Poly1305.Decrypt(chiper, nonce, null));
        }
    }
}

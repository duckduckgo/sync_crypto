using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace WindowsBrowser.Sync.Crypto.Managed
{
    public enum DDGChaCha20Poly1305Result
    {
        UnknownError,
        Ok,
        InvalidKey,
        InvalidData,
        InvalidNonce,
        DecryptionFailed
    }
}
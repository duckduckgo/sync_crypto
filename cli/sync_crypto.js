#!/usr/bin/env node

const { parseArgs } = require('node:util');
const sodium = require('libsodium-wrappers');
const shell = require('./shell');

const {
  from_string,
  to_string,
  from_base64,
  to_base64,
  base64_variants,
} = sodium;

const outputVariant = base64_variants[process.env['OUTPUT_VARIANT']] || base64_variants.ORIGINAL;

const all_base64_variants = Object.keys(base64_variants);
const from_base64_any = (value, variants = all_base64_variants) => {
  try {
    return from_base64(value, base64_variants[variants[0]]);
  } catch (e) {
    if (e.message !== 'invalid input' && e.message !== 'incomplete input') { throw e };
    if (variants.length === 1) { throw e };
    return from_base64_any(value, variants.slice(1));
  }
}

const try_decode_base64 = (value, fieldName) => {
  try {
    return from_base64_any(value);
  } catch (e) {
    console.error(`✗ Error: failed to decode ${fieldName}! Must be a valid base64 string.`);
    if (e.message !== 'invalid input' && e.message !== 'incomplete input') { throw e };
    process.exit(shell.sysexits.EX_DATAERR);
  }
};

// Uses crypto_secretbox_easy
const encrypt = (message, encryption_key_b64) => {
  const nonceBytes = sodium.crypto_secretbox_NONCEBYTES || 24;
  const encryption_key = try_decode_base64(encryption_key_b64, 'encryption_key_b64');

  const nonce = sodium.randombytes_buf(nonceBytes);
  const encrypted = Buffer.from([...sodium.crypto_secretbox_easy(from_string(message), nonce, encryption_key), ...nonce]);

  console.log(to_base64(encrypted, outputVariant));
};

// Uses crypto_secretbox_open_easy
const decrypt = (encrypted_msg_b64, encryption_key_b64) => {
  const nonceBytes = sodium.crypto_secretbox_NONCEBYTES || 24;
  const encrypted_msg_and_nonce = try_decode_base64(encrypted_msg_b64, 'encrypted_msg_b64');
  const encryption_key = try_decode_base64(encryption_key_b64, 'encryption_key_b64');

  if (encrypted_msg_and_nonce.length < nonceBytes + sodium.crypto_secretbox_MACBYTES) {
      throw new Error(`Message too short! Expected at least ${nonceBytes + sodium.crypto_secretbox_MACBYTES}. Got ${encrypted_msg_and_nonce.length}.`);
  }

  const splitPoint = encrypted_msg_and_nonce.length - nonceBytes;
  const nonce = encrypted_msg_and_nonce.slice(splitPoint );
  const encrypted_msg = encrypted_msg_and_nonce.slice(0, splitPoint);

  const decrypted = sodium.crypto_secretbox_open_easy(encrypted_msg, nonce, encryption_key);

  console.log(to_string(decrypted));
};

// Uses crypto_sign_keypair to generate a Ed25519 key pair, then converts it to a Curve25519 key pair. Provides both.
const keypair = () => {
  const signingPair = sodium.crypto_sign_keypair();

  // https://libsodium.gitbook.io/doc/advanced/ed25519-curve25519
  const encryptingPublicKey = sodium.crypto_sign_ed25519_pk_to_curve25519(signingPair.publicKey);
  const encryptingPrivateKey = sodium.crypto_sign_ed25519_sk_to_curve25519(signingPair.privateKey);

  console.log(JSON.stringify({
    Ed25519: {
      public: to_base64(signingPair.publicKey, outputVariant),
      private: to_base64(signingPair.privateKey, outputVariant),
    },
    curve25519:{
      public: to_base64(encryptingPublicKey, outputVariant),
      private: to_base64(encryptingPrivateKey, outputVariant),
    }
  }, null, '  '));
};

// Uses crypto_secretbox_seal
const seal = (message, public_key_b64) => {
  const public_key = try_decode_base64(public_key_b64, 'public_key_b64');

  const encrypted = sodium.crypto_box_seal(from_string(message), public_key);
  console.log(to_base64(encrypted, outputVariant));
};

// Uses crypto_secretbox_seal_open
const seal_open = (encrypted_msg_b64, public_key_b64, private_key_b64) => {
  const message = try_decode_base64(encrypted_msg_b64, 'encrypted_msg_b64');
  const public_key = try_decode_base64(public_key_b64, 'public_key_b64');
  const private_key = try_decode_base64(private_key_b64, 'private_key_b64');

  const decrypted = sodium.crypto_box_seal_open(message, public_key, private_key);
  console.log(to_string(decrypted));
};

// Uses crypto_sign_detached
const sign = (message, sign_private_key_b64) => {
  const sign_private_key = try_decode_base64(sign_private_key_b64, 'sign_private_key_b64');

  const signature = sodium.crypto_sign_detached(message, sign_private_key)
  console.log(to_base64(signature, outputVariant));
};

// Uses crypto_sign_verify_detached
const sign_verify = (signature_b64, message, sign_public_key_b64) => {
  const signature = try_decode_base64(signature_b64, 'signature_b64');
  const sign_public_key = try_decode_base64(sign_public_key_b64, 'sign_public_key_b64');

  const isValid = sodium.crypto_sign_verify_detached(signature, from_string(message), sign_public_key);
  if (isValid) {
    console.debug('✔ signature is valid.');
  } else {
    console.error('✗ signature is invalid!');
    process.exit(1);
  }
};

const commands = {
  encrypt,
  decrypt,
  keypair,
  seal,
  seal_open,
  sign,
  sign_verify
};

const run = async () => {
  const args = parseArgs({ allowPositionals: true });

  const commandName = args.positionals.shift();
  if (!commandName) {
    console.error(`Need a command! Available commands:`);
    Object.keys(commands).forEach((name) => {
      const fnArgs = (commands[name].toString().match(/\((.*)\)/)?.[1] || '').split(', ').filter(v => v);
      const argList = fnArgs.map((arg) => `{${arg}}`).join(' ');
      console.error(`  - ${name} ${argList}`);
    });
    console.error(`Environment variable options:`);
    console.error(`  - OUTPUT_VARIANT: ${all_base64_variants.join(', ')}`);
    process.exit(shell.sysexits.EX_USAGE);
  }

  const command = commands[commandName];
  if (!command) {
    console.error(`✗ '${command}' is not a command.`);
    process.exit(shell.sysexits.EX_USAGE);
  }

  await sodium.ready;
  try {
    command(...args.positionals);
  } catch (e) {
    console.error(e);
    process.exit(1);
  }
};

if (require.main === module) {
  run();
}

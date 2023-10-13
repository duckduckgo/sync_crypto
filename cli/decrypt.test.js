#!/usr/bin/env -S node --test

const { describe, it } = require('node:test');
const assert = require('node:assert');

const { tryShell } = require('./shell.js');

describe('decrypt', () => {
  it('prints usage if no params passed', () => {
    const res = tryShell('./bin/decrypt');

    assert.deepEqual(res, {
      status: 1,
      stderr:
        'Usage: ./bin/decrypt encrypted_msg encryption_key\n' +
        '  Both inputs must be encoded in base64.',
    });
  });

  it('prints error if encrypted_msg not valid base64', () => {
    const res = tryShell('./bin/decrypt xx0.0xx Q0kK');

    assert.deepEqual(res, { status: 2, stderr: 'Error: failed to decode encrypted_msg! Must be a value base64 string.' });
  });

  it('prints error if encrypted_msg not valid base64', () => {
    const res = tryShell('./bin/decrypt Q0kK xx0.0xx');

    assert.deepEqual(res, { status: 2, stderr: 'Error: failed to decode encrypted_key! Must be a value base64 string.' });
  });

  it('decrypts valid message', () => {
    const cmd = "./bin/decrypt 'TOVjSkOZcz5ZVfcFWM6gSwxfhEn0oJwYWdWuT2/bR55z/qvC6Tp8BdZ5LPATIiSIeQ==' 'FCn9kPsQ+2KZNIiKwm4vfRlThm4BrqZv96YYpPM98so='";
    const res = tryShell(cmd);

    if (res.status !== 0) {
      return assert.deepEqual(res, { status: 0 });
    }
    assert.deepEqual(res.stdout, 'Bookmarks');
  });
})

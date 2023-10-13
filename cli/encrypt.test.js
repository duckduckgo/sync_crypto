#!/usr/bin/env -S node --test

const { describe, it } = require('node:test');
const assert = require('node:assert');

const { tryShell } = require('./shell.js');

describe('encrypt', () => {
  it('prints usage if no params passed', () => {
    const res = tryShell('./bin/encrypt');

    assert.deepEqual(res, {
      status: 1,
      stderr:
        'Usage: ./bin/encrypt msg_to_encrypt encryption_key\n' +
        '  encryption_key must be encoded in base64.',
    });
  });

  it('prints error if encrypted_key not valid base64', () => {
    const res = tryShell('./bin/encrypt "whatsup doc" xx0.0xx');

    assert.deepEqual(res, { status: 2, stderr: 'Error: failed to decode encrypted_key! Must be a value base64 string.' });
  });

  it('encrypts valid message', () => {
    const cmd = "./bin/encrypt 'whatsup doc' 'FCn9kPsQ+2KZNIiKwm4vfRlThm4BrqZv96YYpPM98so='";
    const res = tryShell(cmd);

    if (res.status !== 0) {
      return assert.deepEqual(res, { status: 0 });
    }

    assert.equal(res.stdout.replace(/[0-9a-zA-Z/+]/g, '#'), '####################################################################');
  });
})

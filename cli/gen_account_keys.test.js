#!/usr/bin/env -S node --test

/**
 * NodeJS docs:
 * - https://nodejs.org/dist/latest-v18.x/docs/api/test.html
 * - https://nodejs.org/docs/latest-v18.x/api/assert.html
 */
const { describe, it } = require('node:test');
const assert = require('node:assert');

const { tryShell } = require('./shell.js');

describe('gen_account_keys', () => {
  it('prints usage if no params passed', () => {
    const res = tryShell('./bin/gen_account_keys');

    assert.deepEqual(res, {
      status: 1,
      stderr: 'Usage: ./bin/gen_account_keys username password secretKey',
    });
  });

  it('prints error if secretKey not valid base64', () => {
    const res = tryShell('./bin/gen_account_keys user pass xx0.0xx');

    assert.deepEqual(res, {
      status: 2,
      stderr: 'Error decoding secretKey. It must contain 32 bytes encoded in base64.',
    });
  });

  it('prints json', () => {
    const res = tryShell('./bin/gen_account_keys user pass aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaAAA=');

    if (res.status !== 0) {
      return assert.deepEqual(res, { status: 0 });
    }

    const json = JSON.parse(res.stdout);
    json.protected_encryption_key = json.protected_encryption_key.replace(/[0-9a-zA-Z/+]/g, '#');

    assert.deepEqual(json, {
      hashed_password: 'BeD1dTKV6Pcatiaf/Ssp2aWdAR8duUT6sA9otTybmcI=',
      primary_key: 'o6LSTn8t6E6Dj5XU1CvTPuGbPjRt6Mqu0sNqjRn/txo=',
      protected_encryption_key: '################################################################################################',
      secret_key: 'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaAAA=',
    });
  });
})

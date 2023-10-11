#!/usr/bin/env -S node --test

/**
 * NodeJS docs:
 * - https://nodejs.org/dist/latest-v18.x/docs/api/test.html
 * - https://nodejs.org/docs/latest-v18.x/api/assert.html
 */
const { describe, it } = require('node:test');
const { execSync } = require('node:child_process');
const assert = require('node:assert');

const shell = (cmd, options) => execSync(cmd, options).toString().trim();

const tryShell = (cmd, options) => {
  try {
    const stdout = shell(cmd, { stdio: 'pipe', ...options });
    return { status: 0, stdout };
  } catch (err) {
    const stdout = err.stdout.toString().trim();
    const stderr = err.stderr.toString().trim();
    return { status: err.status, stdout, stderr };
  }
};

describe('gen_account_keys', () => {
  it('prints usage if no params passed', () => {
    const res = tryShell('./bin/gen_account_keys');

    assert.deepEqual(res, {
      status: 1,
      stderr: 'Usage: ./bin/gen_account_keys username password secretKey',
      stdout: '',
    });
  });

  it('prints error if secretKey not valid base64', () => {
    const res = tryShell('./bin/gen_account_keys user pass xx0.0xx');

    assert.deepEqual(res, {
      status: 2,
      stderr: 'Error decoding secretKey. It must contain 32 bytes encoded in base64.',
      stdout: '',
    });
  });

  it('prints json', () => {
    const res = tryShell('./bin/gen_account_keys user pass aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaAAA=');

    assert.equal(res.status, 0);

    const json = JSON.parse(res.stdout);
    json.protectedSecretKey = json.protectedSecretKey.replace(/[0-9a-zA-Z/+]/g, '#');

    assert.deepEqual(json, {
      passwordHash: 'BeD1dTKV6Pcatiaf/Ssp2aWdAR8duUT6sA9otTybmcI=',
      primaryKey: 'o6LSTn8t6E6Dj5XU1CvTPuGbPjRt6Mqu0sNqjRn/txo=',
      protectedSecretKey: '################################################################################################',
      secretKey: 'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaAAA=',
    });
  });
})

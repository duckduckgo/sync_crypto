#!/usr/bin/env -S node

const core = require('@actions/core');
const github = require('@actions/github');

const { execSync } = require('node:child_process');
const { randomUUID } = require('node:crypto');

const shell = (cmd, options) => execSync(cmd, options).toString().trim();

const base64 = (value) => Buffer.from(value).toString('base64');

const buildCLI = () => shell('make dist-ci');

const generateAccountKeys = async () => {
  const user_id = randomUUID();
  const password = randomUUID();
  const secretKey = 'FCn9kPsQ+2KZNIiKwm4vfRlThm4BrqZv96YYpPM98so='; // this key is bound to the pre-encrypted data

  const cmd = `./bin/gen_account_keys '${user_id}' '${password}' '${secretKey}'`;
  console.log(`Executing \`${cmd}\`…`);

  const output = JSON.parse(shell(cmd));
  console.log('./bin/gen_account_keys output:', output);
  const { hashed_password, primary_key, protected_secret_key, secret_key } = output;

  const device_id = randomUUID();
  const device_name = base64(process.env.GITHUB_WORKFLOW_REF || 'CI');
  const device_type = base64('CI');
  return {
    user_id, hashed_password,
    device_id, device_name, device_type,
    primary_key, protected_secret_key, secret_key
  };
};

const createAccount = () => {
  // token=$(curl -X POST -H "Content-Type: application/json" -d '{
  //   "user_id": "'$user_id'",
  //   "hashed_password": "'$passwordHash'"
  //   "device_id": "'$device_id'",
  //   "device_name": "'$device_name'",
  //   "device_type": "'$device_type'",
  // }' $url/sync/signup | jq -r '.token')
  throw new Error('Not Implemented');
};

const recoveryKey = ({ user_id, primary_key }) => {
  const recoveryKeyObj = { recovery: { user_id, primary_key } };
  return base64(JSON.stringify(recoveryKeyObj));
};

const run = async () => {
  buildCLI();
  const keys = await generateAccountKeys();

  console.log('keys', keys);

  // TODO: create account
  // TODO: store bookmarks and credentials

  core.setOutput('recovery-code', recoveryKey(keys));
};

Promise.resolve()
  .then(run)
  .catch((error) => core.setFailed(error.message));

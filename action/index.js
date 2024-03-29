#!/usr/bin/env -S node

const { execSync } = require('node:child_process');
const { randomUUID } = require('node:crypto');
const path = require('node:path');
const actions = require('./github-actions.js');
const syncApi = require('./sync-api.js');

const sample1Json = require('./data/sample1.json');

const srcdir = path.resolve(__dirname, '..');

const base64 = (value) => Buffer.from(value).toString('base64');

const shell = (cmd, options) => execSync(cmd, options).toString().trim();

const buildCLI = () => shell('make dist', { cwd: srcdir });

const generateAccountKeys = async () => {
  const user_id = randomUUID();
  const password = randomUUID();

  // this secretKey is bound to the pre-encrypted data at ./data/sample1.json
  const secretKey = 'FCn9kPsQ+2KZNIiKwm4vfRlThm4BrqZv96YYpPM98so=';

  const cmd = `./bin/gen_account_keys '${user_id}' '${password}' '${secretKey}'`;
  console.log(`Executing \`${cmd}\`…`);

  const output = JSON.parse(shell(cmd, { cwd: srcdir }));
  console.log('./bin/gen_account_keys output:', output);
  const { hashed_password, primary_key, protected_encryption_key, secret_key } = output;

  const device_id = randomUUID();
  const raw_device_type = 'github:duckduckgo/sync_crypto';
  const raw_device_name = process.env.GITHUB_REF || raw_device_type;
  const device_type = shell(`./bin/encrypt '${raw_device_type}' '${primary_key}'`, { cwd: srcdir });
  const device_name = shell(`./bin/encrypt '${raw_device_name}' '${primary_key}'`, { cwd: srcdir });
  return {
    user_id, hashed_password,
    device_id, device_name, device_type,
    primary_key, protected_encryption_key, secret_key
  };
};

const createAccount = async (accountKeys) => {
  console.log('POST /signup…');
  const response = await syncApi.signup(accountKeys);
  console.log('Response:', response);
  return response.token;
};

const storeData = async (jwt) => {
  console.log('PATCH /data…');
  const response = await syncApi.patchData(jwt, sample1Json);
  console.log('Response:', response);
  return response;
};

const recoveryCodeBase64 = ({ user_id, primary_key }) => {
  const recoveryKeyObj = { recovery: { user_id, primary_key } };
  return base64(JSON.stringify(recoveryKeyObj));
};

const run = async () => {
  const dryRun = actions.getInput('dry-run') === 'true';
  const debug = actions.getInput('debug') === 'true';

  buildCLI();
  const keys = await generateAccountKeys();

  console.log('Account keys:', keys);

  if (dryRun) {
    actions.notice('dry-run enabled! Not calling API.');
  } else {
    const jwt = await createAccount(keys);
    if (debug) { actions.notice(`jwt = '${base64(jwt)}' (in base64)`); }

    actions.setState('jwt', jwt);
    await storeData(jwt);
  }

  const recoveryCode = recoveryCodeBase64(keys);
  actions.notice(`recovery-code = '${recoveryCode}'`);
  actions.setOutput('recovery-code', recoveryCode);
};

return Promise.resolve()
  .then(run)
  .catch((err) => {
    console.error(err);
    actions.setFailed(err);
  });

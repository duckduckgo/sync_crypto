#!/usr/bin/env -S node

const { execSync } = require('node:child_process');
const { randomUUID } = require('node:crypto');
const path = require('node:path');
const actions = require('./github-actions.js');
const syncApi = require('./sync-api.js');

const srcdir = path.resolve(__dirname, '..');

const base64 = (value) => Buffer.from(value).toString('base64');

const shell = (cmd, options) => execSync(cmd, options).toString().trim();

const buildCLI = () => shell('make dist-ci', { cwd: srcdir });

const generateAccountKeys = async () => {
  const user_id = randomUUID();
  const password = randomUUID();
  const secretKey = 'FCn9kPsQ+2KZNIiKwm4vfRlThm4BrqZv96YYpPM98so='; // this key is bound to the pre-encrypted data

  const cmd = `./bin/gen_account_keys '${user_id}' '${password}' '${secretKey}'`;
  console.log(`Executing \`${cmd}\`…`);

  const output = JSON.parse(shell(cmd, { cwd: srcdir }));
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

const createAccount = async (accountKeys) => {
  const response = await syncApi.signup(accountKeys);
  console.log('Response from POST /signup:', response);
  return response.token;
};

const recoveryCodeBase64 = ({ user_id, primary_key }) => {
  const recoveryKeyObj = { recovery: { user_id, primary_key } };
  return base64(JSON.stringify(recoveryKeyObj));
};

const run = async () => {
  const dryRun = actions.getInput('dry-run') !== 'false';
  if (dryRun) { actions.notice('dryRun = ' + dryRun); }

  buildCLI();
  const keys = await generateAccountKeys();

  console.log('Account keys:', keys);
  const jwt = await createAccount(keys);
  actions.notice(`jwt = '${jwt}'`);

  // TODO: store bookmarks and credentials

  const recoveryCode = recoveryCodeBase64(keys);
  actions.notice(`recovery-code = '${recoveryCode}'`);
  actions.setOutput('recovery-code', recoveryCode);
};

return Promise.resolve()
  .then(run)
  .catch((err) => {
    actions.setFailed(err.toString())
  });

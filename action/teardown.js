#!/usr/bin/env -S node

const actions = require('./github-actions.js');
const syncApi = require('./sync-api.js');

const deleteAccount = async (jwt) => {
  console.log('POST /delete-account…');
  const response = await syncApi.deleteAccount(jwt);
  console.log('Response:', response);
  return response;
};

const run = async () => {
  const dryRun = actions.getInput('dry-run') === 'true';
  const noTeardown = actions.getInput('no-teardown') === 'true';

  if (dryRun) { return console.log('dry-run enabled! Nothing to teardown.'); }
  if (noTeardown) { return console.log('no-teardown enabled! Leaving test account open.'); }

  const jwt = actions.getState('jwt');
  if (!jwt) { return actions.setFailed('No JWT found!?'); }

  return deleteAccount(jwt);
};

return Promise.resolve()
  .then(run)
  .catch((err) => {
    console.error(err);
    actions.setFailed(err);
  });

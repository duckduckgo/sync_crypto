#!/usr/bin/env -S node

const core = require('@actions/core');
const github = require('@actions/github');

try {
  const payload = JSON.stringify(github.context.payload, undefined, 2)
  console.log(`The event payload: ${payload}`);

  core.setOutput('recovery-code', 'dummy');
} catch (error) {
  core.setFailed(error.message);
}


const fs = require('node:fs');
const os = require('node:os');

const escapeMessage = (value) => (
  (value || '').replace(/%/g, '%25').replace(/\r/g, '%0D').replace(/\n/g, '%0A')
);

// Equivalent to `echo "{name}={value}" >> "$GITHUB_OUTPUT"`
const setOutput = (name, value) => {
  const filePath = process.env.GITHUB_OUTPUT;
  const githubCmd = `${name}=${value}${os.EOL}`;
  fs.appendFileSync(filePath, githubCmd, { encoding: 'utf8' });
}

const error = (message) => {
  console.error(`::error::${escapeMessage(message)}`);
};
const warning = (message) => {
  console.log(`::warning::${escapeMessage(message)}`);
};
const notice = (message) => {
  console.log(`::notice::${escapeMessage(message)}`);
};

const getInput = (name) => {
  const val = process.env[`INPUT_${name.replace(/ /g, '_').toUpperCase()}`] || '';
  return val.trim();
}

const setFailed = (message) => {
  process.exitCode = 1;
  error(message);
}

module.exports = {
  setOutput,
  error,
  warning,
  notice,
  getInput,
  setFailed,
};

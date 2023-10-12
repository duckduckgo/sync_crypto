
const fs = require('node:fs');
const os = require('node:os');

const escapeMessage = (value) => (
  (value || '').toString().replace(/%/g, '%25').replace(/\r/g, '%0D').replace(/\n/g, '%0A')
);

// Equivalent to `echo "{name}={value}" >> "$GITHUB_OUTPUT"`
const setOutput = (name, value) => {
  const filePath = process.env.GITHUB_OUTPUT;
  const githubCmd = `${name}=${value}${os.EOL}`;
  fs.appendFileSync(filePath, githubCmd, { encoding: 'utf8' });
}

const getProps = (message) => {
  if (!(message instanceof Error)) {
    return '';
  }

  const cwd = process.cwd();
  const stackLine = message.stack.split('\n').filter(line => line.includes(cwd)).pop();
  if (!stackLine) {
    return '';
  }
  const [file, line, col] = stackLine
    .replace(/^.*\(/, '')
    .replace(/\)$/, '')
    .replace(cwd + '/', '')
    .split(':');
  const props = { file, line, col };
  return Object.entries(props).map(([name, value]) => `${name}=${value}`).join(',');
};

const error = (message) => {
  const props = getProps(message);
  console.error(`::error ${props}::${escapeMessage(message)}`);
};
const warning = (message) => {
  const props = getProps(message);
  console.log(`::warning ${props}::${escapeMessage(message)}`);
};
const notice = (message) => {
  const props = getProps(message);
  console.log(`::notice ${props}::${escapeMessage(message)}`);
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

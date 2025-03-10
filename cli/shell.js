#!/usr/bin/env node

const { execSync, exec } = require('node:child_process');
const { promisify } = require('node:util');

const execProm = promisify(exec);

const shellSync = (cmd, options) => (execSync(cmd, options) || '').toString().trim();
const shell = async (cmd, options) => execProm(cmd, options);

const tee = async (cmd, options) => {
  const proc = execProm(cmd, options);
  proc.child.stdout.pipe(process.stdout);
  proc.child.stderr.pipe(process.stderr);
  return proc;
};

const tryShell = (cmd, options) => {
  try {
    const stdout = shellSync(cmd, { stdio: 'pipe', ...options });
    return { status: 0, stdout };
  } catch (err) {
    return Object.fromEntries(
      [
        ['status', err.status],
        ['stdout', err.stdout.toString().trim()],
        ['stderr', err.stderr.toString().trim()],
      ].filter(([k, v]) => v)
    );
  }
};

// https://man.openbsd.org/sysexits or /usr/include/sysexits.h
const sysexits = {
  EX_USAGE: 64,
  EX_DATAERR: 65,
  EX_CANTCREAT: 73,
  EX_NOPERM: 77,
};

module.exports = {
  tryShell,
  shellSync,
  shell,
  tee,
  sysexits
};

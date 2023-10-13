
const { execSync } = require('node:child_process');

const shell = (cmd, options) => execSync(cmd, options).toString().trim();

const tryShell = (cmd, options) => {
  try {
    const stdout = shell(cmd, { stdio: 'pipe', ...options });
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

module.exports = {
  shell,
  tryShell,
};

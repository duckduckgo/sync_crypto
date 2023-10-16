# sync_crypto

# GitHub Action

This action creates an account on the staging evironment of DuckDuckGo's Sync API.

## Outputs

- `recovery-code` • A Sync API's recovery code for the new account.

## Inputs

- `debug` • Optional. Output more debug information, if set to `"true"`.
- `dry-run` • Optional. Does not call the Sync API, if set to `"true"`.
- `no-teardown` • Optional. Does not delete account, if set to `"true"`.

## Example usage

```yaml
jobs:
  example_job:
    runs-on: ubuntu-latest
    name: Example
    steps:
      - name: Create Sync test account for Sync. Return a recovery code.
        uses: duckduckgo/sync_crypto/action@main
        id: sync-test-account
      - name: Show recovery code
        run: echo "::notice title=recovery code::${{ steps.sync-test-account.outputs.recovery-code }}"
```

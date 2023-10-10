curl -O- https://github.com/duckduckgo/sync-config/blob/lkruger/sync1/test/setup.sh
curl -O- https://github.com/duckduckgo/sync-config/blob/lkruger/sync1/test/sync_predefined_autofill.json
curl -O- https://github.com/duckduckgo/sync-config/blob/lkruger/sync1/test/sync_predefined_bookmarks.json
curl -O- https://github.com/duckduckgo/sync-config/blob/lkruger/sync1/test/test_user_setup
curl -O- https://github.com/duckduckgo/sync-config/blob/lkruger/sync1/test/teardown.sh

json="$(./setup.sh)"
echo "$json"

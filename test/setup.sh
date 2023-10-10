#!/bin/bash
primaryKey='L5xaECJvESAuX8QWf2OfQPPcOFxi8zhClV/2Bg+CK84='
secretKey='FCn9kPsQ+2KZNIiKwm4vfRlThm4BrqZv96YYpPM98so='
recoveryKey='eyJyZWNvdmVyeSI6eyJwcmltYXJ5X2tleSI6Ikw1eGFFQ0p2RVNBdVg4UVdmMk9mUVBQY09GeGk4emhDbFYvMkJnK0NLODQ9IiwidXNlcl9pZCI6IjNkZjgwYjcwLWU3YzQtNGEwMC1hOTQyLTdiNjgwMjU1YTE2ZCJ9fQ=='

autofill="$(jq . sync_predefined_autofill.json)"
bookmarks="$(jq . sync_predefined_bookmarks.json)"

host=usw-lkruger1.duckduckgo.com
#host=dev-sync-use.duckduckgo.com
url=https://$host

user_id=$(uuid)
password=password
device_id=testdevice
device_name=test_device
device_type=androidd

hashes=$(./test_user_setup $secretKey $user_id $password)
protectedSecretKey=$(echo $hashes | jq -r '.protectedSecretKey')
passwordHash=$(echo $hashes | jq -r '.passwordHash')

token=$(curl -X POST -H "Content-Type: application/json" -d '{
  "user_id": "'$user_id'",
  "device_id": "'$device_id'",
  "device_name": "'$device_name'",
  "device_type": "'$device_type'",
  "protected_encryption_key": "'$primaryKey'",
  "hashed_password": "'$passwordHash'"
}' $url/sync/signup | jq -r '.token')

curl -X PATCH -H "Content-Type: application/json" \
    -H "Authorization: Bearer $token" -d "$autofill" $url/sync/data
curl -X PATCH -H "Content-Type: application/json" \
    -H "Authorization: Bearer $token" -d "$bookmarks" $url/sync/data

curl -H "Authorization: Bearer $token" $url/sync/data

# Return to the caller
echo '{"user_id":"'"$user_id"'", "protectedSecretKey": "'$protectedSecretKey'", "token": "'"$token"'"}'

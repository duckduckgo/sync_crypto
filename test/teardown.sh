#!/bin/bash
token="$1"

curl -X POST -H "Authorization: Bearer $token" $url/sync/delete-account


.PHONY: all install install-ci clean dist info test version

all: dist

install:
	sudo apt install -qq libsodium-dev

install-ci:
	sudo apt-get update -q
	sudo apt-get install -qy libsodium-dev

dist:    install    bin/
dist-ci: install-ci bin/
bin/: bin/gen_account_keys bin/decrypt bin/encrypt

clean:
	rm -rf ./bin/

bin/gen_account_keys: ./cli/gen_account_keys.c
	mkdir -p ./bin/
	g++ ./cli/gen_account_keys.c -l sodium -o ./bin/gen_account_keys
	ls -lha ./bin/gen_account_keys

bin/encrypt: ./cli/encrypt.c ./native_lib/*
	mkdir -p ./bin/
	g++ ./cli/encrypt.c ./native_lib/DDGSyncCrypto.c -l sodium -o ./bin/encrypt
	ls -lha ./bin/encrypt

bin/decrypt: ./cli/decrypt.c ./native_lib/*
	mkdir -p ./bin/
	g++ ./cli/decrypt.c ./native_lib/DDGSyncCrypto.c -l sodium -o ./bin/decrypt
	ls -lha ./bin/decrypt

test: bin/gen_account_keys
	node --test ./cli/*.test.js



.PHONY: all install install-ci clean dist info test version

all: dist

install:
	sudo apt install -yqq libsodium-dev

install-ci:
	sudo apt update
	sudo apt install -y libsodium-dev

dist: install bin/gen_account_keys
dist-ci: install-ci bin/gen_account_keys

clean:
	rm -rf ./bin/

bin/gen_account_keys: ./cli/gen_account_keys.c
	mkdir -p ./bin/
	g++ ./cli/gen_account_keys.c -l sodium -o ./bin/gen_account_keys
	ls -lha ./bin/gen_account_keys

test: bin/gen_account_keys
	node --test ./cli/*.test.js


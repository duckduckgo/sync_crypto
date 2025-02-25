
.PHONY: all install install-ci clean dist info test version

OS_UNAME := $(shell uname)

ifeq ($(OS_UNAME), Darwin)
	LIB_INCLUDE_PATH = -I./native_lib/third-party/apple/Clibsodium.xcframework/macos-arm64_arm64e_x86_64/Headers/
	HEADER_INCLUDE_PATH = -L./native_lib/third-party/apple/Clibsodium.xcframework/macos-arm64_arm64e_x86_64/
endif

all: dist

install:
ifneq ($(OS_UNAME), Darwin)
	sudo apt-get install -qq libsodium-dev
endif

install-ci:
ifneq ($(OS_UNAME), Darwin)
	sudo apt-get update -q
	sudo apt-get install -qy libsodium-dev
endif

dist:    install    bin/
dist-ci: install-ci bin/
bin/: bin/gen_account_keys bin/decrypt bin/encrypt
	ls -lA ./bin/

clean:
	rm -rf ./bin/

bin/gen_account_keys: ./cli/gen_account_keys.c
	mkdir -p ./bin/
	g++ $(HEADER_INCLUDE_PATH) ./cli/gen_account_keys.c $(LIB_INCLUDE_PATH) -l sodium -o ./bin/gen_account_keys
	ls -lha ./bin/gen_account_keys

bin/encrypt: ./cli/encrypt.c ./native_lib/*
	mkdir -p ./bin/
	g++ $(HEADER_INCLUDE_PATH) ./cli/encrypt.c ./native_lib/DDGSyncCrypto.c $(LIB_INCLUDE_PATH) -l sodium -o ./bin/encrypt

bin/decrypt: ./cli/decrypt.c ./native_lib/*
	mkdir -p ./bin/
	g++ $(HEADER_INCLUDE_PATH) ./cli/decrypt.c ./native_lib/DDGSyncCrypto.c $(LIB_INCLUDE_PATH) -l sodium -o ./bin/decrypt

test: bin/
	node --test ./cli/*.test.js


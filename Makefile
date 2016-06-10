.PHONY: all clean

test: recover-libsecp256k1 recover-openssl
	./test.sh

clean:
	rm -f recover-libsecp256k1 recover-openssl

recover-libsecp256k1: recover-libsecp256k1.c util.h
	cc -Wall -Wno-unused-function -I./libsecp256k1 -o $@ $<

recover-openssl: recover-openssl.c util.h
	cc -Wall -Wno-unused-function -lcrypto -o $@ $<

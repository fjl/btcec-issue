.PHONY: all clean

BINARIES = recover-libsecp256k1 recover-openssl scalarmult-openssl scalarmult-libsecp256k1
CFLAGS =
#CFLAGS = -Wall -Wno-unused-function

test: $(BINARIES)
	./test.sh

clean:
	rm -f $(BINARIES)

recover-libsecp256k1: recover-libsecp256k1.c util.h
	cc $(CFLAGS)  -I./libsecp256k1 -o $@ $<

recover-openssl: recover-openssl.c util.h
	cc $(CFLAGS) -lcrypto -o $@ $<

scalarmult-libsecp256k1: scalarmult-libsecp256k1.c util.h
	cc $(CFLAGS) -I./libsecp256k1 -o $@ $<

scalarmult-openssl: scalarmult-openssl.c util.h
	cc $(CFLAGS) -lcrypto -o $@ $<

#define USE_NUM_NONE
#define USE_FIELD_10X26
#define USE_FIELD_INV_BUILTIN
#define USE_SCALAR_8X32
#define USE_SCALAR_INV_BUILTIN
#define NDEBUG
#include "include/secp256k1.h"
#include "src/secp256k1.c"
#include "src/modules/recovery/main_impl.h"

#include "./util.h"

int
main(int argc, char* argv[])
{
	if (argc < 3) {
		fatal("need two arguments, message hash and signature");
	}
	const unsigned char* msg = parsehex(argv[1], 32);
	const unsigned char* sig = parsehex(argv[2], 65);
	if (!msg) {
		fatal("invalid message, need 64 hex characters");
	}
	if (!sig) {
		fatal("invalid signature, need 130 hex characters");
	}

	secp256k1_context* ctx;
	ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);

	secp256k1_pubkey* pubkey = alloca(sizeof(secp256k1_pubkey));
	secp256k1_ecdsa_recoverable_signature* recsig = alloca(sizeof(secp256k1_ecdsa_recoverable_signature));
	if (!secp256k1_ecdsa_recoverable_signature_parse_compact(ctx, recsig, &sig[1], sig[0])) {
		fatal("invalid signature");
	}
	if (!secp256k1_ecdsa_recover(ctx, pubkey, recsig, msg)) {
		fatal("recover failed");
	}

	size_t pubkeylen = 65;
	unsigned char* spubkey = alloca(pubkeylen);
	secp256k1_ec_pubkey_serialize(ctx, spubkey, &pubkeylen, pubkey, SECP256K1_EC_UNCOMPRESSED);
	printf("pubkey: %s\n", encodehex(spubkey, pubkeylen));
	return 0;
}

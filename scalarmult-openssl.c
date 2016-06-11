#include <openssl/bn.h>
#include <openssl/ecdsa.h>
#include <openssl/obj_mac.h>

#include "util.h"

int
main(int argc, char* argv[])
{
	if (argc < 3) {
		fatal("need two arguments, ec point and scalar");
	}

	BN_CTX* ctx = BN_CTX_new();
	if (!ctx) {
		fatal("can't allocate bignum context");
	}
	EC_GROUP* group = EC_GROUP_new_by_curve_name(NID_secp256k1);
	if (!group) {
		fatal("can't get secp256k1 group");
	}
	EC_POINT* point = EC_POINT_hex2point(group, argv[1], NULL, ctx);
	if (!point) {
		fatal("can't parse first argument as hex ec point");
	}
	BIGNUM* scalar = NULL;
	if (!BN_hex2bn(&scalar, argv[1])) {
		fatal("can't parse second argument as hex number");
	}

	EC_POINT* result = EC_POINT_new(group);
	if (!result) {
		fatal("can't allocate bignum");
	}
	if (!EC_POINT_mul(group, result, NULL, point, scalar, ctx)) {
		fatal("point multiplication failed");
	}
	printf("result: %s\n", EC_POINT_point2hex(group, result, POINT_CONVERSION_UNCOMPRESSED, ctx));
}

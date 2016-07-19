 #define USE_NUM_NONE
#define USE_FIELD_10X26
#define USE_FIELD_INV_BUILTIN
#define USE_SCALAR_8X32
#define USE_SCALAR_INV_BUILTIN
#define NDEBUG
#include "include/secp256k1.h"
#include "src/secp256k1.c"

#include "./util.h"

/** Multiply point by scalar in constant time.
 *  Returns: 1: multiplication was successful
 *           0: scalar was invalid (zero or overflow)
 *  Args:    ctx:      pointer to a context object (cannot be NULL)
 *  Out:     point:    the multiplied point (usually secret)
 *  In:      point:    pointer to a 64-byte bytepublic point,
 *                     encoded as two 256bit big-endian numbers.
 *           scalar:   a 32-byte scalar with which to multiply the point
 */
int secp256k1_pubkey_scalar_mul(const secp256k1_context* ctx, unsigned char *point, const unsigned char *scalar) {
	int ret = 0;
	int overflow = 0;
	secp256k1_fe feX, feY;
	secp256k1_gej res;
	secp256k1_ge ge;
	secp256k1_scalar s;
	ARG_CHECK(point != NULL);
	ARG_CHECK(scalar != NULL);
	(void)ctx;

	secp256k1_fe_set_b32(&feX, point);
	secp256k1_fe_set_b32(&feY, point+32);
	secp256k1_ge_set_xy(&ge, &feX, &feY);
	secp256k1_scalar_set_b32(&s, scalar, &overflow);
	if (overflow || secp256k1_scalar_is_zero(&s)) {
		ret = 0;
	} else {
		secp256k1_ecmult_const(&res, &ge, &s);
		secp256k1_ge_set_gej(&ge, &res);
		/* Note: can't use secp256k1_pubkey_save here because it is not constant time. */
		secp256k1_fe_normalize(&ge.x);
		secp256k1_fe_normalize(&ge.y);
		secp256k1_fe_get_b32(point, &ge.x);
		secp256k1_fe_get_b32(point+32, &ge.y);
		ret = 1;
	}
	secp256k1_scalar_clear(&s);
	return ret;
}

int
main(int argc, char* argv[])
{
	if (argc < 3) {
		fatal("need two arguments, ec point and scalar");
	}
	unsigned char* point = parsehex(argv[1], 65);
	const unsigned char* scalar = parsehex(argv[2], 32);
	if (!point) {
		fatal("invalid point, need at most 130 hex characters");
	}
	if (!scalar) {
		fatal("invalid point, need at most 64 hex characters");
	}
	if (point[0] != 4) {
		fatal("invalid point, first byte must be 4");
	}
	point++; // strip uncompressed point flag

	secp256k1_context* ctx;
	ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);
	if (!secp256k1_pubkey_scalar_mul(ctx, point, scalar)) {
		fatal("scalar mult failed");
	}

	printf("pubkey: 04%s\n", encodehex(point, 64));
}

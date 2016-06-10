#include <openssl/bn.h>
#include <openssl/ecdsa.h>
#include <openssl/rand.h>
#include <openssl/obj_mac.h>

#include "util.h"

// ECDSA_SIG_recover_key_GFp as used in bitcoind before they switched to
// libsecp256k1.

// Perform ECDSA key recovery (see SEC1 4.1.6) for curves over (mod p)-fields
// recid selects which key is recovered
// if check is non-zero, additional checks are performed
int
ECDSA_SIG_recover_key_GFp(EC_KEY *eckey, ECDSA_SIG *ecsig, const unsigned char *msg, int msglen, int recid, int check)
{
	if (!eckey) return 0;

	int ret = 0;
	BN_CTX *ctx = NULL;

	BIGNUM *x = NULL;
	BIGNUM *e = NULL;
	BIGNUM *order = NULL;
	BIGNUM *sor = NULL;
	BIGNUM *eor = NULL;
	BIGNUM *field = NULL;
	EC_POINT *R = NULL;
	EC_POINT *O = NULL;
	EC_POINT *Q = NULL;
	BIGNUM *rr = NULL;
	BIGNUM *zero = NULL;
	int n = 0;
	int i = recid / 2;

	const EC_GROUP *group = EC_KEY_get0_group(eckey);
	if ((ctx = BN_CTX_new()) == NULL) { ret = -1; goto err; }
	BN_CTX_start(ctx);
	order = BN_CTX_get(ctx);
	if (!EC_GROUP_get_order(group, order, ctx)) { ret = -2; goto err; }
	x = BN_CTX_get(ctx);
	if (!BN_copy(x, order)) { ret=-1; goto err; }
	if (!BN_mul_word(x, i)) { ret=-1; goto err; }
	if (!BN_add(x, x, ecsig->r)) { ret=-1; goto err; }
	field = BN_CTX_get(ctx);
	if (!EC_GROUP_get_curve_GFp(group, field, NULL, NULL, ctx)) { ret=-2; goto err; }
	if (BN_cmp(x, field) >= 0) { ret=0; goto err; }
	if ((R = EC_POINT_new(group)) == NULL) { ret = -2; goto err; }
	if (!EC_POINT_set_compressed_coordinates_GFp(group, R, x, recid % 2, ctx)) { ret=0; goto err; }
	if (check)
	{
		if ((O = EC_POINT_new(group)) == NULL) { ret = -2; goto err; }
		if (!EC_POINT_mul(group, O, NULL, R, order, ctx)) { ret=-2; goto err; }
		if (!EC_POINT_is_at_infinity(group, O)) { ret = 0; goto err; }
	}
	if ((Q = EC_POINT_new(group)) == NULL) { ret = -2; goto err; }
	n = EC_GROUP_get_degree(group);
	e = BN_CTX_get(ctx);
	if (!BN_bin2bn(msg, msglen, e)) { ret=-1; goto err; }
	if (8*msglen > n) BN_rshift(e, e, 8-(n & 7));
	zero = BN_CTX_get(ctx);
	if (!BN_zero(zero)) { ret=-1; goto err; }
	if (!BN_mod_sub(e, zero, e, order, ctx)) { ret=-1; goto err; }
	rr = BN_CTX_get(ctx);
	if (!BN_mod_inverse(rr, ecsig->r, order, ctx)) { ret=-1; goto err; }
	sor = BN_CTX_get(ctx);
	if (!BN_mod_mul(sor, ecsig->s, rr, order, ctx)) { ret=-1; goto err; }
	eor = BN_CTX_get(ctx);
	if (!BN_mod_mul(eor, e, rr, order, ctx)) { ret=-1; goto err; }
	if (!EC_POINT_mul(group, Q, eor, R, sor, ctx)) { ret=-2; goto err; }
	if (!EC_KEY_set_public_key(eckey, Q)) { ret=-2; goto err; }

	ret = 1;

err:
	if (ctx) {
		BN_CTX_end(ctx);
		BN_CTX_free(ctx);
	}
	if (R != NULL) EC_POINT_free(R);
	if (O != NULL) EC_POINT_free(O);
	if (Q != NULL) EC_POINT_free(Q);
	return ret;
}

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

	ECDSA_SIG* osig = ECDSA_SIG_new();
	EC_KEY* opubkey = EC_KEY_new_by_curve_name(NID_secp256k1);
	if (!osig || !opubkey) {
		fatal("can't allocate");
	}
	BN_bin2bn(&sig[1],  32, osig->r);
	BN_bin2bn(&sig[33], 32, osig->s);

	if (ECDSA_SIG_recover_key_GFp(opubkey, osig, msg, 32, sig[0], 0) != 1) {
		fatal("recover failed");
	}
	char* pubkey = EC_POINT_point2hex(
		EC_KEY_get0_group(opubkey),
		EC_KEY_get0_public_key(opubkey),
		POINT_CONVERSION_UNCOMPRESSED,
		NULL);
	printf("pubkey: %s\n", pubkey);
}

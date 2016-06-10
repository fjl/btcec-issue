#include <stdio.h>
#include <string.h>

static void
fatal(char* message)
{
	fprintf(stderr, "%s\n", message);
	exit(1);
}

static const unsigned char*
parsehex(char* input, size_t len)
{
	unsigned char* ret;
	unsigned int b;
	if (strlen(input) != len*2) {
		return NULL;
	}
	if ((ret = calloc(len, 1)) == NULL) {
		fatal("can't allocate");
	}
	for (int i = 0; i < len; i++) {
		if (!sscanf(&input[2*i], "%02x", &b)) {
			free(ret);
			return NULL;
		}
		ret[i] = (unsigned char)b;
	}
	return ret;
}

static char*
encodehex(const unsigned char* data, size_t len)
{
	char* hex = calloc(2*len+1, 1);
	if (!hex) {
		fatal("can't allocate");
	}
	for (size_t i = 0; i < len; i++) {
		sprintf(&hex[i*2], "%02X", data[i]);
	}
	return hex;
}

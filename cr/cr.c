#include <err.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "md5.h"

//static const unsigned char hash[] = { 0xc1, 0x0b, 0xce, 0xcb, 0x7b, 0xf3, 0x7b, 0x1c, 0xe7, 0xc1, 0xaf, 0xbe, 0x0e, 0xc6, 0x6a, 0x0b };
static const unsigned char hash[] = { 0x6c, 0x8d, 0x87, 0xe6, 0xc4, 0xbc, 0x74, 0x7c, 0x67, 0xba, 0x4b, 0x99, 0x3d, 0xbc, 0x64, 0x4f };

#define MAX_LEN 40

#ifdef VARIANTS
static const char subs['z'][3] = {
	[ 'a' ] = "@4",
	[ 'b' ] = "8",
	[ 'e' ] = "3",
	[ 'i' ] = "!",
	[ 'l' ] = "1",
	[ 'o' ] = "0",
	[ 's' ] = "$5",
	[ 't' ] = "7",
};
#endif

static const char *compute(const char *arr, unsigned len, unsigned count)
{
	MD5_CTX md5_ctx;
	unsigned char md5[16];
	unsigned i, len0 = len + 1;

	for (i = 0; i < count; i++) {
		const char *try = &arr[i * len0];

		MD5_Init(&md5_ctx);
		MD5_Update(&md5_ctx, try, len);
		MD5_Final(md5, &md5_ctx);

		if (!memcmp(md5, hash, 16))
			return try;
	}

	return NULL;
}

static void getvariants(const char lower[MAX_LEN], char *all, unsigned len)
{
#ifdef VARIANTS
	const char *sub;
	unsigned len0 = len + 1;
	unsigned i, words = 1;

	for (i = 0; i < len; i++) {
		unsigned j, orig_words = words;

		for (sub = subs[(unsigned)lower[i]]; *sub; sub++) {
			for (j = 0; j < orig_words; j++) {
				const char *word = &all[j * len0];
				char *word_variant = &all[words++ * len0];

				strcpy(word_variant, word);

				word_variant[i] = *sub;
			}
		}
	}
#else
	(void)lower;
	(void)all;
	(void)len;
#endif
}

int main()
{
	char *all = NULL;
	char buf[MAX_LEN];
	char lower[MAX_LEN];
	char consuming[MAX_LEN];
	unsigned iter = 0;
	unsigned long counter = 0;
	unsigned last_size = 0;

	while (fgets(buf, sizeof(buf), stdin) != NULL) {
		unsigned int count, size, len = strlen(buf);
		unsigned int i = i;
		const char *try;

		if (buf[len - 1] == '\n') {
			buf[len - 1] = 0;
			len--;
		}

		count = 1;
#ifdef VARIANTS
		for (i = 0; i < len; i++) {
			const char *sub;

			lower[i] = tolower(buf[i]);

			sub = subs[(unsigned)lower[i]];
			count *= strlen(sub) + 1;
		}
#endif

		size = count * (len + 1);
		if (size > last_size) {
			all = realloc(all, size);
			if (!all)
				err(1, "malloc(%u)", count * (len + 1));
			last_size = size;
			strcpy(consuming, buf);
		}

		strcpy(all, buf);

		getvariants(lower, all, len);

		if (!(iter % 500000))
			printf("trying %u. word from dict, tried %lu combinations\n", iter, counter);

		counter += count;
		try = compute(all, len, count);
		if (try) {
			printf("got it: '%s', it was %lu-th attempt, most consuming: %s\n", try, counter, consuming);
			break;
		}
		iter++;
	}

	free(all);
	return 0;
}

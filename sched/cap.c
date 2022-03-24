#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/capability.h>

int main(int argc, char **argv)
{
	char *txt_caps;
	cap_t cap = cap_from_text(argv[1]);
	if (!cap)
		err(1, "cap_from_text");

	txt_caps = cap_to_text(cap, NULL);
	if (!txt_caps)
		err(1, "cap_to_text");

	printf("caps: %s\n", txt_caps);

	cap_free(txt_caps);
	cap_free(cap);
	return 0;
}


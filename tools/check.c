// check for extra whitespace

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char buf[1000];
char *check(void);

int
main(int argc, char *argv[])
{
	int line = 0;
	char *s;
	FILE *f;

	if (argc < 2)
		exit(1);

	f = fopen(argv[1], "r");

	if (f == NULL) {
		printf("cannot open %s\n", argv[1]);
		exit(1);
	}

	while (fgets(buf, sizeof buf, f)) {

		line++;

		s = check();

		if (s)
			printf("%s, line %d\n", s, line);
	}

	fclose(f);
}

char *
check(void)
{
	int i, n;

	n = strlen(buf);

	if (n < 1 || buf[n - 1] != '\n')
		return "missing newline";

	if (n == 1)
		return NULL; // ok

	// check for weird ascii chars

	for (i = 0; i < n; i++) {
		if (buf[i] >= ' ' && buf[i] < 0x7f)
			continue;
		if (buf[i] == '\t' || buf[i] == '\n')
			continue;
		return "ascii error";
	}

	// check trailing space

	if (strstr(buf, " \n") || strstr(buf, "\t\n"))
		return "trailing space";

	if (strncmp(buf, "//", 2) == 0 || strncmp(buf, "\t//", 3) == 0)
		return NULL;

	if (strstr(buf, "  ") || strstr(buf, " \t") || strstr(buf, "\t "))
		return "consecutive space";

	return NULL;
}

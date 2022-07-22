// check for extra whitespace

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

char buf[1000];

int check(void);

int
main(int argc, char *argv[])
{
	int line = 0;
	FILE *f;

	if (argc < 2)
		exit(1);

	f = fopen(argv[1], "r");

	if (f == NULL) {
		fprintf(stderr, "cannot open %s\n", argv[1]);
		exit(1);
	}

	while (fgets(buf, sizeof buf, f)) {

		line++;

		if (check())
			fprintf(stderr, "file %s, line %d\n", argv[1], line);
	}

	fclose(f);

	return 0;
}

int
check(void)
{
	int i, n;

	n = strlen(buf);

	if (n < 1 || buf[n - 1] != '\n') {
		fprintf(stderr, "missing newline\n");
		return -1;
	}

	if (n == 1)
		return 0; // ok

	// check for weird ascii chars

	for (i = 0; i < n; i++) {
		if (buf[i] >= ' ' && buf[i] < 0x7f)
			continue;
		if (buf[i] == '\t' || buf[i] == '\n')
			continue;
		fprintf(stderr, "ascii error\n");
		return -1;
	}

	// check trailing space

	if (strstr(buf, " \n") || strstr(buf, "\t\n")) {
		fprintf(stderr, "trailing space\n");
		return -1;
	}

	if (strncmp(buf, "//", 2) == 0 || strncmp(buf, "\t//", 3) == 0)
		return 0;

	if (strstr(buf, "  ") || strstr(buf, " /t") || strstr(buf, "\t ")) {
		fprintf(stderr, "extra spaces\n");
		return -1;
	}

	return 0; // ok
}

// make eigenmath.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

int filter(const struct dirent *p);
void emit_file(char *);
int check_contents(char *);

#define PATH "../src/"

int
main()
{
	int i, n;
	struct dirent **p;
	static char s[100];

	system("cat preamble");

	emit_file(PATH "defs1.h");
	emit_file(PATH "prototypes.h");
	emit_file(PATH "globals.c");

	n = scandir(PATH, &p, filter, alphasort);

	for (i = 0; i < n; i++) {
		if (strcmp(p[i]->d_name, "globals.c") == 0)
			continue;
		strcpy(s, PATH);
		strcat(s, p[i]->d_name);
		emit_file(s);
	}

	return 0;
}

int
filter(const struct dirent *p)
{
	int len = strlen(p->d_name);

	if (len > 2 && strcmp(p->d_name + len - 2, ".c") == 0)
		return 1;
	else
		return 0;
}

void
emit_file(char *filename)
{
	int line = 0;
	FILE *f;
	static char buf[1000];

	f = fopen(filename, "r");

	if (f == NULL) {
		fprintf(stderr, "cannot open %s\n", filename);
		exit(1);
	}

	while (fgets(buf, sizeof buf, f)) {

		line++;

		if (check_contents(buf)) {
			fprintf(stderr, "file %s, line %d\n", filename, line);
			exit(1);
		}

		if (strcmp(buf, "#include \"defs.h\"\n") == 0)
			continue;

		printf("%s", buf);
	}

	fclose(f);
}

int
check_contents(char *buf)
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

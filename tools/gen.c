// make eigenmath.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

int filter(const struct dirent *p);
void emit_file(char *);
void emit_line(char *);
int check_line(char *);

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
	int line = 0, state = 0;
	FILE *f;
	static char s[1000];

	emit_line("\n");

	f = fopen(filename, "r");

	if (f == NULL) {
		fprintf(stderr, "cannot open %s\n", filename);
		exit(1);
	}

	while (fgets(s, sizeof s, f)) {

		line++;

		if (check_line(s)) {
			fprintf(stderr, "file %s, line %d\n", filename, line);
			exit(1);
		}

		if (strcmp(s, "#include \"defs.h\"\n") == 0)
			continue;

		if (state && *s == '\n')
			continue; // skip blank lines in functions

		emit_line(s);

		if (*s == '{')
			state = 1;

		if (*s == '}')
			state = 0;
	}

	fclose(f);
}

int
check_line(char *line)
{
	int i, n;

	n = strlen(line);

	if (n < 1 || line[n - 1] != '\n') {
		fprintf(stderr, "missing newline\n");
		return -1;
	}

	if (n == 1)
		return 0; // ok

	// check for weird ascii chars

	for (i = 0; i < n; i++) {
		if (line[i] >= ' ' && line[i] < 0x7f)
			continue;
		if (line[i] == '\t' || line[i] == '\n')
			continue;
		fprintf(stderr, "ascii error\n");
		return -1;
	}

	// check trailing space

	if (strstr(line, " \n") || strstr(line, "\t\n")) {
		fprintf(stderr, "trailing space\n");
		return -1;
	}

	if (strncmp(line, "//", 2) == 0 || strncmp(line, "\t//", 3) == 0)
		return 0;

	if (strstr(line, "  ") || strstr(line, " /t") || strstr(line, "\t ")) {
		fprintf(stderr, "extra spaces\n");
		return -1;
	}

	return 0; // ok
}

void
emit_line(char *line)
{
	static int c = '\n';

	if (c == '\n' && *line == '\n')
		return; // don't print more than one blank line in a row

	printf("%s", line);

	c = *line;
}

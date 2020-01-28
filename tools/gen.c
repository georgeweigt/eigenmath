#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

int filter(const struct dirent *p);
void emit_file(char *);
void emit_line(char *);
void check_line(char *, int, char *);

int prev = '\n';
char filename[100];

#define PATH "../src/"

int
main()
{
	int i, n;
	struct dirent **p;
	system("cat preamble.h");
	emit_file(PATH "defs1.h");
	emit_file(PATH "prototypes.h");
	emit_file(PATH "globals.c");
	n = scandir(PATH, &p, filter, alphasort);
	for (i = 0; i < n; i++) {
		if (strcmp(p[i]->d_name, "globals.c") == 0)
			continue;
		strcpy(filename, PATH);
		strcat(filename, p[i]->d_name);
		emit_file(filename);
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
	int line = 1, state = 0;
	FILE *f;
	static char str[1000];

	emit_line("\n");

	f = fopen(filename, "r");

	if (f == NULL) {
		fprintf(stderr, "cannot open %s\n", filename);
		exit(1);
	}

	while (fgets(str, sizeof str, f)) {

		check_line(filename, line++, str);

		if (strcmp(str, "#include \"defs.h\"\n") == 0)
			continue;

		if (state && *str == '\n')
			continue; // skip blank lines in functions

		emit_line(str);

		if (*str == '{')
			state = 1;

		if (*str == '}')
			state = 0;
	}

	fclose(f);
}

void
check_line(char *filename, int line_number, char *line)
{
	int i, n;
	n = strlen(line);
	if (n < 1 || line[n - 1] != '\n') {
		fprintf(stderr, "file %s line %d: line error\n", filename, line_number);
		exit(1);
	}
	if (n < 2)
		return;
	// check for weird ascii
	for (i = 0; i < n; i++) {
		if (line[i] >= ' ' && line[i] < 0x7f)
			continue;
		if (line[i] == '\t' || line[i] == '\n')
			continue;
		fprintf(stderr, "%s line %d: ascii error\n", filename, line_number);
		exit(1);
	}
	// check leading space
	for (i = 0; i < n; i++) {
		if (line[i] > ' ')
			break;
		if (line[i] == '\t')
			continue;
		fprintf(stderr, "%s line %d: leading space\n", filename, line_number);
		// exit(1);
	}
	// check trailing space
	if (line[n - 2] <= ' ') {
		fprintf(stderr, "%s line %d: trailing space\n", filename, line_number);
		exit(1);
	}
}

void
emit_line(char *line)
{
	if (prev == '\n' && *line == '\n')
		return; // don't print more than one blank line in a row
	printf("%s", line);
	prev = *line;
}

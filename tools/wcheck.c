// check for extra whitespace

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void check_file(char *);
char *check_line(char *);

char buf[1000];

int
main(int argc, char *argv[])
{
	int i;
	for (i = 1; i < argc; i++)
		check_file(argv[i]);
}

void
check_file(char *filename)
{
	int line = 0;
	char *s;
	FILE *f;

	f = fopen(filename, "r");

	if (f == NULL) {
		printf("cannot open %s\n", filename);
		return;
	}

	while (fgets(buf, sizeof buf, f)) {

		line++;

		s = check_line(buf);

		if (s) {
			printf("%s line %d: %s\n", filename, line, s);
			puts(buf);
		}
	}

	if (line && buf[0] == '\n') {
		printf("%s line %d: trailing newline\n", filename, line);
		puts(buf);
	}

	fclose(f);
}

char *
check_line(char *buf)
{
	int i, len;

	len = strlen(buf);

	if (len < 1 || buf[len - 1] != '\n')
		return "missing newline";

	for (i = 0; i < len; i++) {
		if (buf[i] >= ' ' && buf[i] < 0x7f)
			continue;
		if (buf[i] == '\t' || buf[i] == '\n')
			continue;
		return "ascii error";
	}

	if (strstr(buf, " \n") || strstr(buf, "\t\n"))
		return "trailing space";

	if (strncmp(buf, "//", 2) == 0 || strncmp(buf, "\t//", 3) == 0)
		return NULL;

	if (strstr(buf, "  ") || strstr(buf, " \t") || strstr(buf, "\t "))
		return "consecutive space";

	return NULL;
}

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

char filename[100];

int filter(const struct dirent *p);
void scan(char *s);

int
main(int argc, char *argv[])
{
	int i, n;
	struct dirent **p;
	n = scandir(argv[1], &p, filter, alphasort);
	for (i = 0; i < n; i++) {
		strcpy(filename, argv[1]);
		strcat(filename, p[i]->d_name);
		scan(filename);
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

#define BUFLEN 1000

char buf1[BUFLEN];
char buf2[BUFLEN];
char buf3[BUFLEN];

void
scan(char *s)
{
	char *a, *b, *c, *t;
	FILE *f;
	f = fopen(s, "r");
	if (f == NULL) {
		printf("cannot open %s\n", s);
		exit(1);
	}
	a = fgets(buf1, BUFLEN, f);
	b = fgets(buf2, BUFLEN, f);
	c = fgets(buf3, BUFLEN, f);
	while (c) {
		if (*c == '{' && strncmp(a, "static", 5) != 0) {
			a[strlen(a) - 1] = 0; // erase newline
			b[strlen(b) - 1] = 0;
			printf("%s %s;\n", a, b);
		}
		t = a;
		a = b;
		b = c;
		c = fgets(t, BUFLEN, f);
	}
	fclose(f);
}

// To build a new prototypes.h:
//
//	gcc make-prototypes.c
//	./a.out >prototypes.h

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

void g(char *);

int
filter(const struct dirent *p)
{
	int len = strlen(p->d_name);

	if (strcmp(p->d_name, "make-prototypes.c") == 0)
		return 0;

	if (len > 2 && strcmp(p->d_name + len - 2, ".c") == 0)
		return 1;
	else
		return 0;
}

int
main()
{
	int i, n, x;
	struct dirent **p;
	n = scandir(".", &p, filter, alphasort);
	for (i = 0; i < n; i++)
		g(p[i]->d_name);
	return 0;
}

char str1[10000], str2[10000], str3[10000];

void
g(char *s)
{
	char *a, *b, *c, *t;
	FILE *f;
	f = fopen(s, "r");
	if (f == NULL) {
		printf("cannot open %s\n", s);
		exit(1);
	}
	a = fgets(str1, sizeof str1, f);
	b = fgets(str2, sizeof str2, f);
	c = fgets(str3, sizeof str3, f);
	while (1) {
		if (c == NULL)
			break;
		if (*c == '{' && strncmp(a, "static", 5) != 0) {
			a[strlen(a) - 1] = 0; // erase newline
			b[strlen(b) - 1] = 0;
			printf("%s %s;\n", a, b);
		}
		t = a;
		a = b;
		b = c;
		c = fgets(t, 1000, f);
	}
	fclose(f);
}

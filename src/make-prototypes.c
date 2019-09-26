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
		//printf("<a href=\"%s\">%s</a><br>\n", p[i]->d_name, p[i]->d_name);
		g(p[i]->d_name);
	return 0;
}

char str1[1000], str2[1000], str3[1000];

void
g(char *s)
{
	char *a, *b, *c, *t;
	FILE *f;
	if (strcmp(s, "MainOSX.c") == 0 || strcmp(s, "MainXP.c") == 0
	|| strcmp(s, "prototype-tool.c") == 0
	|| strcmp(s, "html-tool.c") == 0)
		return;
//	printf("\n// %s\n", s);
	f = fopen(s, "r");
	if (f == NULL) {
		printf("cannot open %s\n", s);
		exit(1);
	}
	a = fgets(str1, 1000, f);
	b = fgets(str2, 1000, f);
	c = fgets(str3, 1000, f);
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

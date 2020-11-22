#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char buf[1000];
char str[100];

char *roman[] = {
"Alpha",
"Beta",
"Gamma",
"Delta",
"Epsilon",
"Zeta",
"Eta",
"Theta",
"Iota",
"Kappa",
"Lambda",
"Mu",
"Nu",
"Xi",
"Omicron",
"Pi",
"Rho",
"Sigma",
"Tau",
"Upsilon",
"Phi",
"Chi",
"Psi",
"Omega",
"hbar",
"less",
"equal",
"greater",
"parenleft",
"parenright",
"plus",
"minus",
"product",
"greaterequal",
"lessequal",
};

char *italic[] = {
"alpha",
"beta",
"gamma",
"delta",
"epsilon",
"zeta",
"eta",
"theta",
"iota",
"kappa",
"lambda",
"mu",
"nu",
"xi",
"omicron",
"pi",
"rho",
"sigma",
"tau",
"upsilon",
"phi",
"chi",
"psi",
"omega",
};

int
main()
{
	int i, w;
	char *s, *t;
	FILE *f;

	f = fopen("Times_New_Roman.afm", "r");

	for (i = 0; i < 17; i++)
		fgets(buf, sizeof buf, f);

	printf("const roman_width_tab = [\n");

	for (i = 0; i < 128; i++) {
		fgets(buf, sizeof buf, f);
		s = strchr(buf, 'X') + 1;
		w = atoi(s);
		printf("%d,", w);
		if (i % 8 == 7)
			printf("\n");
	}

	printf("];\n");

	fclose(f);

	f = fopen("Times_New_Roman.afm", "r");

	for (i = 0; i < 17; i++)
		fgets(buf, sizeof buf, f);

	for (;;) {
		fgets(buf, sizeof buf, f);
		if (buf[0] != 'C')
			break;
		s = strchr(buf, 'X') + 1;
		w = atoi(s);
		s = strchr(buf, 'N') + 2;
		sscanf(s, "%s", str);
		for (i = 0; i < sizeof roman / sizeof (char *); i++) {
			t = roman[i];
			if (strcmp(str, t) == 0) {
				printf("\"&%s;\":%d,\n", t, w);
				break;
			}
		}
	}

	fclose(f);

	f = fopen("Times_New_Roman_Italic.afm", "r");

	for (i = 0; i < 17; i++)
		fgets(buf, sizeof buf, f);

	printf("const italic_width_tab = [\n");

	for (i = 0; i < 128; i++) {
		fgets(buf, sizeof buf, f);
		s = strchr(buf, 'X') + 1;
		w = atoi(s);
		printf("%d,", w);
		if (i % 8 == 7)
			printf("\n");
	}

	printf("];\n");

	fclose(f);

	f = fopen("Times_New_Roman_Italic.afm", "r");

	for (i = 0; i < 17; i++)
		fgets(buf, sizeof buf, f);

	for (;;) {
		fgets(buf, sizeof buf, f);
		if (buf[0] != 'C')
			break;
		s = strchr(buf, 'X') + 1;
		w = atoi(s);
		s = strchr(buf, 'N') + 2;
		sscanf(s, "%s", str);
		for (i = 0; i < sizeof italic / sizeof (char *); i++) {
			t = italic[i];
			if (strcmp(str, t) == 0) {
				printf("\"&%s;\":%d,\n", t, w);
				break;
			}
		}
	}

	fclose(f);
}

#include "app.h"

#undef N
#define N 49

struct gtab {
	char *s;
	int italic;
} gtab[N] = {

	{"Alpha",	0},
	{"Beta",	0},
	{"Gamma",	0},
	{"Delta",	0},
	{"Epsilon",	0},
	{"Zeta",	0},
	{"Eta",		0},
	{"Theta",	0},
	{"Iota",	0},
	{"Kappa",	0},
	{"Lambda",	0},
	{"Mu",		0},
	{"Nu",		0},
	{"Xi",		0},
	{"Omicron",	0},
	{"Pi",		0},
	{"Rho",		0},
	{"Sigma",	0},
	{"Tau",		0},
	{"Upsilon",	0},
	{"Phi",		0},
	{"Chi",		0},
	{"Psi",		0},
	{"Omega",	0},

	{"alpha",	1},
	{"beta",	1},
	{"gamma",	1},
	{"delta",	1},
	{"epsilon",	1},
	{"zeta",	1},
	{"eta",		1},
	{"theta",	1},
	{"iota",	1},
	{"kappa",	1},
	{"lambda",	1},
	{"mu",		1},
	{"nu",		1},
	{"xi",		1},
	{"omicron",	1},
	{"pi",		1},
	{"rho",		1},
	{"sigma",	1},
	{"tau",		1},
	{"upsilon",	1},
	{"phi",		1},
	{"chi",		1},
	{"psi",		1},
	{"omega",	1},

	{"hbar",	0},
};

int
emit_symbol_fragment(char *symbol, int k)
{
	int c, i, n;
	char *s;

	for (i = 0; i < N; i++) {
		s = gtab[i].s;
		n = (int) strlen(s);
		if (strncmp(symbol + k, s, n) == 0)
			break;
	}

	if (i == N) {
		emit_italic_char(s[k]);
		return k + 1;
	}

	c = i + 128;

	if (gtab[i].italic)
		emit_italic_char(c);
	else
		emit_roman_char(c);

	return k + n;
}

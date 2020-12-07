#include "app.h"

#define N 49

char *symbol_name_tab[N] = {

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

	"hbar",
};

int symbol_italic_tab[N] = {
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,
	1,
};

int
emit_symbol_fragment(char *s, int k)
{
	int char_num, i, n;
	char *t;

	for (i = 0; i < N; i++) {
		t = symbol_name_tab[i];
		n = (int) strlen(t);
		if (strncmp(s + k, t, n) == 0)
			break;
	}

	if (i == N) {
		if (isdigit(s[k]))
			emit_roman_char(s[k]);
		else
			emit_italic_char(s[k]);
		return k + 1;
	}

	char_num = i + 128;

	if (symbol_italic_tab[i])
		emit_italic_char(char_num);
	else
		emit_roman_char(char_num);

	return k + n;
}

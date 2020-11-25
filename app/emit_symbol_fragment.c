#include "app.h"

#define N 49

struct {
	char *s;
	int italic;
} symbol_name_tab[N] = {

	{"Alpha",	0}, // 128
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

	{"alpha",	1}, // 152
	{"beta",	1}, // 153
	{"gamma",	1}, // 154
	{"delta",	1}, // 155
	{"epsilon",	1}, // 156
	{"zeta",	1}, // 157
	{"eta",		1}, // 158
	{"theta",	1}, // 159
	{"iota",	1}, // 160
	{"kappa",	1}, // 161
	{"lambda",	1}, // 162
	{"mu",		1}, // 163
	{"nu",		1}, // 164
	{"xi",		1}, // 165
	{"omicron",	1}, // 166
	{"pi",		1}, // 167
	{"rho",		1}, // 168
	{"sigma",	1}, // 169
	{"tau",		1}, // 170
	{"upsilon",	1}, // 171
	{"phi",		1}, // 172
	{"chi",		1}, // 173
	{"psi",		1}, // 174
	{"omega",	1}, // 175

	{"hbar",	1},
};

int
emit_symbol_fragment(char *name, int k)
{
	int char_num, i, n;
	char *s;

	for (i = 0; i < N; i++) {
		s = symbol_name_tab[i].s;
		n = (int) strlen(s);
		if (strncmp(name + k, s, n) == 0)
			break;
	}

	if (i == N) {
		if (isdigit(name[k]))
			emit_roman_char(name[k]);
		else
			emit_italic_char(name[k]);
		return k + 1;
	}

	char_num = i + 128;

	if (symbol_name_tab[i].italic)
		emit_italic_char(char_num);
	else
		emit_roman_char(char_num);

	return k + n;
}

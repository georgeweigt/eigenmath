#include "defs.h"

extern void get_height_width(int *, int *, int, char *s);

static void cm_emit_expr(struct atom *);
static void cm_emit_term(struct atom *);
static void cm_emit_multiply(struct atom *, int);
static void cm_emit_factor(struct atom *);
static void cm_emit_power(struct atom *);
static void cm_emit_denominator(struct atom *, int);
static void cm_emit_subexpr(struct atom *);
static void cm_fixup_power(int, int);
static void cm_move(int, int, int, int);
static void cm_get_size(int, int, int *, int *, int *);
static void cm_emit_function(struct atom *);
static void cm_emit_symbol(struct atom *);
static void cm_emit_string(struct atom *);
static void cm_fixup_fraction(int, int, int);
static void cm_cm_emit_number(struct atom *, int);
static void cm_emit_str(int, char *);
static void cm_emit_char(int, int);
static int cm_count_denominators(struct atom *);
static void cm_emit_fraction(struct atom *, int);
static void cm_emit_numerical_fraction(struct atom *);
static void cm_emit_tensor(struct atom *);
static int cm_isdenominator(struct atom *);
static void cm_emit_flat_tensor(struct atom *);
static void cm_emit_tensor_inner(struct atom *, int, int *);
static void cm_emit_index_function(struct atom *);
static void cm_emit_factorial_function(struct atom *);
static void normy(void);
static int xheight(void);
static void cm_emit_thick_space(void);
static void cm_emit_thin_space(void);
static void cm_emit_very_thin_space(void);
static int mheight(void);
static int shim(void);
static int hrule_thickness(void);
static void cm_emit_hrule(int);
static int isgreek(char *);
static void do_groups(void);
static void do_group(int, int);
static void cm_emit_numerators(struct atom *);
static void cm_emit_denominators(struct atom *);
static int subscript_dy(void);
static void cm_emit_minus_sign_space(void);
static void check_chartab(void);

#define SMALL_FONT 1
#define DEFAULT_FONT 2
#define TIMES_FONT 3
#define ITALIC_TIMES_FONT 4
#define SYMBOL_FONT 5
#define ITALIC_SYMBOL_FONT 6
#define SMALL_TIMES_FONT 7
#define SMALL_ITALIC_TIMES_FONT 8
#define SMALL_SYMBOL_FONT 9
#define SMALL_ITALIC_SYMBOL_FONT 10

#define DRAW_HRULE 20
#define DRAW_LEFT_DELIMETER 21
#define DRAW_RIGHT_DELIMETER 22
#define DRAW_SPACE 32

extern struct text_metric text_metric[11];

#define NGREEK 34

static struct {
	char *s;
	int font, n;
} greek[NGREEK] = {
	{"Gamma",	SYMBOL_FONT,		128},
	{"Delta",	SYMBOL_FONT,		129},
	{"Theta",	SYMBOL_FONT,		130},
	{"Lambda",	SYMBOL_FONT,		131},
	{"Xi",		SYMBOL_FONT,		132},
	{"Pi",		SYMBOL_FONT,		133},
	{"Sigma",	SYMBOL_FONT,		134},
	{"Upsilon",	SYMBOL_FONT,		135},
	{"Phi",		SYMBOL_FONT,		136},
	{"Psi",		SYMBOL_FONT,		137},
	{"Omega",	SYMBOL_FONT,		138},
	{"alpha",	ITALIC_SYMBOL_FONT,	139},
	{"beta",	ITALIC_SYMBOL_FONT,	140},
	{"gamma",	ITALIC_SYMBOL_FONT,	141},
	{"delta",	ITALIC_SYMBOL_FONT,	142},
	{"epsilon",	ITALIC_SYMBOL_FONT,	143},
	{"zeta",	ITALIC_SYMBOL_FONT,	144},
	{"eta",		ITALIC_SYMBOL_FONT,	145},
	{"theta",	ITALIC_SYMBOL_FONT,	146},
	{"iota",	ITALIC_SYMBOL_FONT,	147},
	{"kappa",	ITALIC_SYMBOL_FONT,	148},
	{"lambda",	ITALIC_SYMBOL_FONT,	149},
	{"mu",		ITALIC_SYMBOL_FONT,	150},
	{"nu",		ITALIC_SYMBOL_FONT,	151},
	{"xi",		ITALIC_SYMBOL_FONT,	152},
	{"pi",		ITALIC_SYMBOL_FONT,	153},
	{"rho",		ITALIC_SYMBOL_FONT,	154},
	{"sigma",	ITALIC_SYMBOL_FONT,	155},
	{"tau",		ITALIC_SYMBOL_FONT,	156},
	{"upsilon",	ITALIC_SYMBOL_FONT,	157},
	{"phi",		ITALIC_SYMBOL_FONT,	158},
	{"chi",		ITALIC_SYMBOL_FONT,	159},
	{"psi",		ITALIC_SYMBOL_FONT,	160},
	{"omega",	ITALIC_SYMBOL_FONT,	161},
};

// symbol codes starting from 162 after greek letters

#define MULTIPLICATION 162
#define PARTIAL 163
#define MINUS 164

#define MINUS_STR "\244" // 164 in octal

extern void shipout(struct display *);

static struct charspec {
	int cmd, h, w, x, y;
	char *s;
} *chartab;

static int indx, cmax, level, xpos;

void
cmdisplay(void)
{
	int h, i, k, len, n, w, y;
	struct display *d;
	uint8_t *buf;

	save();

	p1 = pop();

	indx = 0;
	level = 0;
	xpos = 0;

	if (car(p1) == symbol(SETQ)) {
		cm_emit_expr(cadr(p1));
		cm_emit_thick_space();
		cm_emit_char(SYMBOL_FONT, '=');
		cm_emit_thick_space();
		cm_emit_expr(caddr(p1));
	} else
		cm_emit_expr(p1);

	do_groups();

	normy();

	cm_get_size(0, indx, &h, &w, &y);

	// figure out how much space is needed

	n = 1; // one byte for end of buffer

	for (i = 0; i < indx; i++) {
		if (chartab[i].cmd == DRAW_SPACE)
			continue; // we're not going to output spaces
		else if (chartab[i].s)
			n += 13 + (int) strlen(chartab[i].s);
		else
			n += 17;
	}

	d = (struct display *) malloc(sizeof (struct display) + n);

	if (d == NULL)
		malloc_kaput();

	d->len = n;
	d->type = 1;
	d->attr = 0;
	d->w = w;
	d->h = h;

	buf = d->buf;

	k = 0;

	for (i = 0; i < indx; i++) {

		if (chartab[i].cmd == DRAW_SPACE)
			continue;

		buf[k++] = (uint8_t) chartab[i].cmd;

		buf[k++] = (uint8_t) (chartab[i].x >> 24);
		buf[k++] = (uint8_t) (chartab[i].x >> 16);
		buf[k++] = (uint8_t) (chartab[i].x >> 8);
		buf[k++] = (uint8_t) chartab[i].x;

		buf[k++] = (uint8_t) (chartab[i].y >> 24);
		buf[k++] = (uint8_t) (chartab[i].y >> 16);
		buf[k++] = (uint8_t) (chartab[i].y >> 8);
		buf[k++] = (uint8_t) chartab[i].y;

		if (chartab[i].s) {
			len = (int) strlen(chartab[i].s);
			buf[k++] = (uint8_t) (len >> 24);
			buf[k++] = (uint8_t) (len >> 16);
			buf[k++] = (uint8_t) (len >> 8);
			buf[k++] = (uint8_t) len;
			strcpy((char *) buf + k, chartab[i].s);
			free(chartab[i].s);
			k += len;
		} else {
			buf[k++] = (uint8_t) (chartab[i].w >> 24);
			buf[k++] = (uint8_t) (chartab[i].w >> 16);
			buf[k++] = (uint8_t) (chartab[i].w >> 8);
			buf[k++] = (uint8_t) chartab[i].w;
			buf[k++] = (uint8_t) (chartab[i].h >> 24);
			buf[k++] = (uint8_t) (chartab[i].h >> 16);
			buf[k++] = (uint8_t) (chartab[i].h >> 8);
			buf[k++] = (uint8_t) chartab[i].h;
		}
	}

	buf[k++] = 0; // end of buffer

	shipout(d);

	restore();
}

static void
cm_emit_expr(struct atom *p)
{
	if (car(p) == symbol(ADD)) {
		save();
		p1 = cdr(p);
		if (isnegativeterm(car(p1))) {
			cm_emit_char(SYMBOL_FONT, MINUS);
		}
		cm_emit_term(car(p1));
		p1 = cdr(p1);
		while (iscons(p1)) {
			if (isnegativeterm(car(p1))) {
				cm_emit_thick_space();
				cm_emit_char(SYMBOL_FONT, MINUS);
				cm_emit_thick_space();
			} else {
				cm_emit_thick_space();
				cm_emit_char(SYMBOL_FONT, '+');
				cm_emit_thick_space();
			}
			cm_emit_term(car(p1));
			p1 = cdr(p1);
		}
		restore();
	} else {
		if (isnegativeterm(p)) {
			cm_emit_char(SYMBOL_FONT, MINUS);
		}
		cm_emit_term(p);
	}
}

static void
cm_emit_term(struct atom *p)
{
	int n;
	if (car(p) == symbol(MULTIPLY)) {
		n = cm_count_denominators(p);
		if (n && level == 0)
			cm_emit_fraction(p, n);
		else
			cm_emit_multiply(p, n);
	} else
		cm_emit_factor(p);
}

static int
cm_isdenominator(struct atom *p)
{
	if (car(p) == symbol(POWER) && cadr(p) != symbol(EXP1) && isnegativeterm(caddr(p)))
		return 1;
	else
		return 0;
}

static int
cm_count_denominators(struct atom *p)
{
	int count = 0;
	struct atom *q;
	p = cdr(p);
	while (iscons(p)) {
		q = car(p);
		if (cm_isdenominator(q))
			count++;
		p = cdr(p);
	}
	return count;
}

// an integer factor is 2, 2^3, etc.

static int
count_integer_factors(struct atom *p)
{
	int n = 0;
	while (iscons(p)) {
		if (isintegerfactor(car(p)))
			n++;
		p = cdr(p);
	}
	return n;
}

// n is the number of denominators, not counting a fraction like 1/2

static void
cm_emit_multiply(struct atom *p, int n)
{
	int k;
	if (n == 0) {
		p = cdr(p);
		if (isplusone(car(p)) || isminusone(car(p)))
			p = cdr(p);
		k = count_integer_factors(p);
		cm_emit_factor(car(p));
		p = cdr(p);
		while (iscons(p)) {
			if (k > 1) {
				cm_emit_thin_space();
				cm_emit_char(SYMBOL_FONT, MULTIPLICATION); // multiplication symbol
				cm_emit_thin_space();
			} else {
				cm_emit_thin_space();
			}
			cm_emit_factor(car(p));
			p = cdr(p);
		}
	} else {
		cm_emit_numerators(p);
		cm_emit_char(TIMES_FONT, '/');
		// need grouping if more than one denominator
		if (n > 1 || isfraction(cadr(p))) {
			cm_emit_char(TIMES_FONT, '(');
			cm_emit_denominators(p);
			cm_emit_char(TIMES_FONT, ')');
		} else
			cm_emit_denominators(p);
	}
}

#define A p3
#define B p4

// sign of term has already been cm_emitted

static void
cm_emit_fraction(struct atom *p, int d)
{
	int count, k1, k2, n, x;

	cm_emit_minus_sign_space();

	save();

	A = one;
	B = one;

	// handle numerical coefficient

	if (isrational(cadr(p))) {
		push(cadr(p));
		numerator();
		absval();
		A = pop();
		push(cadr(p));
		denominator();
		B = pop();
	}

	if (isdouble(cadr(p))) {
		push(cadr(p));
		absval();
		A = pop();
	}

	// count numerators

	if (isplusone(A))
		n = 0;
	else
		n = 1;
	p1 = cdr(p);
	if (isnum(car(p1)))
		p1 = cdr(p1);
	while (iscons(p1)) {
		p2 = car(p1);
		if (cm_isdenominator(p2))
			;
		else
			n++;
		p1 = cdr(p1);
	}

	// cm_emit numerators

	x = xpos;

	k1 = indx;

	count = 0;

	// cm_emit numerical coefficient

	if (!isplusone(A)) {
		cm_cm_emit_number(A, 0);
		count++;
	}

	// skip over "multiply"

	p1 = cdr(p);

	// skip over numerical coefficient, already handled

	if (isnum(car(p1)))
		p1 = cdr(p1);

	while (iscons(p1)) {
		p2 = car(p1);
		if (cm_isdenominator(p2))
			;
		else {
			if (count > 0)
				cm_emit_thin_space();
			if (n == 1)
				cm_emit_expr(p2);
			else
				cm_emit_factor(p2);
			count++;
		}
		p1 = cdr(p1);
	}

	if (count == 0)
		cm_emit_char(TIMES_FONT, '1');

	// cm_emit denominators

	k2 = indx;

	count = 0;

	if (!isplusone(B)) {
		cm_cm_emit_number(B, 0);
		count++;
		d++;
	}

	p1 = cdr(p);

	if (isrational(car(p1)))
		p1 = cdr(p1);

	while (iscons(p1)) {
		p2 = car(p1);
		if (cm_isdenominator(p2)) {
			if (count > 0)
				cm_emit_thin_space();
			cm_emit_denominator(p2, d);
			count++;
		}
		p1 = cdr(p1);
	}

	cm_fixup_fraction(x, k1, k2);

	restore();
}

// p points to a multiply

static void
cm_emit_numerators(struct atom *p)
{
	int n;

	save();

	p1 = one;

	p = cdr(p);

	if (isrational(car(p))) {
		push(car(p));
		numerator();
		absval();
		p1 = pop();
		p = cdr(p);
	} else if (isdouble(car(p))) {
		push(car(p));
		absval();
		p1 = pop();
		p = cdr(p);
	}

	n = 0;

	if (!isplusone(p1)) {
		cm_cm_emit_number(p1, 0);
		n++;
	}

	while (iscons(p)) {
		if (cm_isdenominator(car(p)))
			;
		else {
			if (n > 0)
				cm_emit_thin_space();
			cm_emit_factor(car(p));
			n++;
		}
		p = cdr(p);
	}

	if (n == 0)
		cm_emit_char(TIMES_FONT, '1');

	restore();
}

// p points to a multiply

static void
cm_emit_denominators(struct atom *p)
{
	int n;

	save();

	n = 0;

	p = cdr(p);

	if (isfraction(car(p))) {
		push(car(p));
		denominator();
		p1 = pop();
		cm_cm_emit_number(p1, 0);
		n++;
		p = cdr(p);
	}

	while (iscons(p)) {
		if (cm_isdenominator(car(p))) {
			if (n > 0)
				cm_emit_thin_space();
			cm_emit_denominator(car(p), 0);
			n++;
		}
		p = cdr(p);
	}

	restore();
}

static void
cm_emit_factor(struct atom *p)
{
	if (istensor(p)) {
		if (level == 0)
			cm_emit_tensor(p);
		else
			cm_emit_flat_tensor(p);
		return;
	}

	if (isdouble(p)) {
		cm_cm_emit_number(p, 0);
		return;
	}

	if (car(p) == symbol(ADD) || car(p) == symbol(MULTIPLY)) {
		cm_emit_subexpr(p);
		return;
	}

	if (car(p) == symbol(POWER)) {
		cm_emit_power(p);
		return;
	}

	if (iscons(p)) {
		cm_emit_function(p);
		return;
	}

	if (isnum(p)) {
		if (level == 0)
			cm_emit_numerical_fraction(p);
		else
			cm_cm_emit_number(p, 0);
		return;
	}

	if (isstr(p)) {
		cm_emit_string(p);
		return;
	}

	cm_emit_symbol(p);
}

static void
cm_emit_numerical_fraction(struct atom *p)
{
	int k1, k2, x;

	save();

	push(p);
	numerator();
	absval();
	A = pop();

	push(p);
	denominator();
	B = pop();

	if (isplusone(B)) {
		cm_cm_emit_number(A, 0);
		restore();
		return;
	}

	level++;
	cm_emit_minus_sign_space();
	x = xpos;
	k1 = indx;
	cm_cm_emit_number(A, 0);
	k2 = indx;
	cm_cm_emit_number(B, 0);
	level--;

	cm_fixup_fraction(x, k1, k2);

	restore();
}

// if it's a factor then it doesn't need parens around it, i.e. 1/sin(theta)^2

static int
cm_isfactor(struct atom *p)
{
	if (p->k == DOUBLE)
		return 0; // double is like multiply, for example 1.2 * 10^6
	if (iscons(p) && car(p) != symbol(ADD) && car(p) != symbol(MULTIPLY) && car(p) != symbol(POWER))
		return 1;
	if (issymbol(p))
		return 1;
	if (isfraction(p))
		return 0;
	if (isnegativenumber(p))
		return 0;
	if (isnum(p))
		return 1;
	if (istensor(p))
		return 1;
	return 0;
}

static void
cm_emit_power(struct atom *p)
{
	int k1, k2, x;

	// imaginary unit

	if (isimaginaryunit(p)) {
		if (isimaginaryunit(binding[SYMBOL_J])) {
			cm_emit_char(ITALIC_TIMES_FONT, 'j');
			return;
		}
		if (isimaginaryunit(binding[SYMBOL_I])) {
			cm_emit_char(ITALIC_TIMES_FONT, 'i');
			return;
		}
	}

	if (cadr(p) == symbol(EXP1)) {
		cm_emit_str(TIMES_FONT, "exp");
		cm_emit_char(TIMES_FONT, '(');
		cm_emit_expr(caddr(p));
		cm_emit_char(TIMES_FONT, ')');
		return;
	}

	// special case: level > 0 which means we are already in a superscript
	//
	//                                                      1
	// so, display something like x^(-1) as 1/x instead of ---
	//                                                      x

	if (level > 0 && isminusone(caddr(p))) {
		cm_emit_char(TIMES_FONT, '1');
		cm_emit_char(TIMES_FONT, '/');
		if (cm_isfactor(cadr(p)))
			cm_emit_factor(cadr(p));
		else
			cm_emit_subexpr(cadr(p));
		return;
	}

	// special case: something like x^(-1)
	//
	//             1              -1
	// display as --- instead of x
	//             x

	if (isnegativeterm(caddr(p))) {
		cm_emit_minus_sign_space();
		x = xpos;
		k1 = indx;
		cm_emit_char(TIMES_FONT, '1');
		k2 = indx;
		cm_emit_denominator(p, 1);
		cm_fixup_fraction(x, k1, k2);
		return;
	}

	k1 = indx;
	if (cm_isfactor(cadr(p)))
		cm_emit_factor(cadr(p));
	else
		cm_emit_subexpr(cadr(p));
	k2 = indx;
	level++;
	cm_emit_very_thin_space(); // FIXME
	cm_emit_expr(caddr(p));
	level--;
	cm_fixup_power(k1, k2);
}

// if n == 1 then cm_emit as expr (no parens)

// p is a power

static void
cm_emit_denominator(struct atom *p, int n)
{
	int k1, k2;

	// special case: 1 over something

	if (isminusone(caddr(p))) {
		if (n == 1)
			cm_emit_expr(cadr(p));
		else
			cm_emit_factor(cadr(p));
		return;
	}

	k1 = indx;

	// cm_emit base

	if (cm_isfactor(cadr(p)))
		cm_emit_factor(cadr(p));
	else
		cm_emit_subexpr(cadr(p));

	k2 = indx;

	// cm_emit exponent, don't cm_emit minus sign

	// to get here, caddr(p) must be a term, not an expr

	// see isdenominator()

	level++;
	cm_emit_very_thin_space(); // FIXME
	cm_emit_term(caddr(p));
	level--;

	cm_fixup_power(k1, k2);
}

static void
cm_emit_function(struct atom *p)
{
	if (car(p) == symbol(INDEX) && issymbol(cadr(p))) {
		cm_emit_index_function(p);
		return;
	}

	if (car(p) == symbol(FACTORIAL)) {
		cm_emit_factorial_function(p);
		return;
	}

	cm_emit_symbol(car(p));
	cm_emit_char(TIMES_FONT, '(');
	p = cdr(p);
	if (iscons(p)) {
		cm_emit_expr(car(p));
		p = cdr(p);
		while (iscons(p)) {
			cm_emit_char(TIMES_FONT, ',');
			cm_emit_expr(car(p));
			p = cdr(p);
		}
	}
	cm_emit_char(TIMES_FONT, ')');
}

static void
cm_emit_index_function(struct atom *p)
{
	p = cdr(p);
	if (caar(p) == symbol(ADD) || caar(p) == symbol(MULTIPLY) || caar(p) == symbol(POWER) || caar(p) == symbol(FACTORIAL))
		cm_emit_subexpr(car(p));
	else
		cm_emit_expr(car(p));
	cm_emit_char(TIMES_FONT, '[');
	p = cdr(p);
	if (iscons(p)) {
		cm_emit_expr(car(p));
		p = cdr(p);
		while(iscons(p)) {
			cm_emit_char(TIMES_FONT, ',');
			cm_emit_expr(car(p));
			p = cdr(p);
		}
	}
	cm_emit_char(TIMES_FONT, ']');
}

static void
cm_emit_factorial_function(struct atom *p)
{
	p = cadr(p);
	if (car(p) == symbol(ADD) || car(p) == symbol(MULTIPLY) || car(p) == symbol(POWER) || car(p) == symbol(FACTORIAL))
		cm_emit_subexpr(p);
	else
		cm_emit_expr(p);
	cm_emit_char(TIMES_FONT, '!');
}

static void
cm_emit_subexpr(struct atom *p)
{
	cm_emit_char(TIMES_FONT, '(');
	cm_emit_expr(p);
	cm_emit_char(TIMES_FONT, ')');
}

// these are printed in italic, no subscripting

static char *specname[] = {
	"autoexpand",
	"bake",
	"cross",
	"curl",
	"div",
	"last",
	"nil",
	"reverse",
	"trace",
	"tty",
};

static void
cm_emit_symbol(struct atom *p)
{
	int i, k, n, w;
	char *s;

	if (!issymbol(p)) {	// should not get here, but just in case...
		cm_emit_char(TIMES_FONT, '(');
		cm_emit_expr(p);
		cm_emit_char(TIMES_FONT, ')');
		return;
	}

	if (p == symbol(EXP1)) {
		cm_emit_str(TIMES_FONT, "exp");
		cm_emit_char(TIMES_FONT, '(');
		cm_emit_char(TIMES_FONT, '1');
		cm_emit_char(TIMES_FONT, ')');
		return;
	}

	if (p == symbol(DERIVATIVE)) {
//		cm_emit_char(SYMBOL_FONT, PARTIAL);
		cm_emit_char(TIMES_FONT, 'd');
		return;
	}

	s = get_printname(p);

	if (iskeyword(p)) {
		cm_emit_str(TIMES_FONT, s);
		return;
	}

	if (s[0] == '$') {
		cm_emit_str(TIMES_FONT, s + 1);
		return;
	}

	// check for special names like "last"

	for (i = 0; i < sizeof specname / sizeof (char *); i++) {
		if (strcmp(s, specname[i]) == 0) {
			cm_emit_str(ITALIC_TIMES_FONT, s);
			return;
		}
	}

	// parse greek letters

	k = indx;

	n = isgreek(s);

	if (n == -1)
		if (isalpha(*s))
			cm_emit_char(ITALIC_TIMES_FONT, *s++);
		else
			cm_emit_char(TIMES_FONT, *s++);
	else {
		cm_emit_char(greek[n].font, greek[n].n);
		s += strlen(greek[n].s);
	}

	// feynman slash?

	if (strncmp(s, "slash", 5) == 0) {
		cm_emit_char(SYMBOL_FONT, '/');
		w = chartab[k].w - chartab[indx - 1].w;
		chartab[indx - 1].x = chartab[k].x + w / 2;
		xpos = chartab[k].x + chartab[k].w;
		s += 5;
	}

	k = indx;

	level++;

	while (*s) {
		n = isgreek(s);
		if (n == -1)
			if (isalpha(*s))
				cm_emit_char(ITALIC_TIMES_FONT, *s++);
			else
				cm_emit_char(TIMES_FONT, *s++);
		else {
			cm_emit_char(greek[n].font, greek[n].n);
			s += strlen(greek[n].s);
		}
	}

	level--;

	// subscript

	for (i = k; i < indx; i++)
		chartab[i].y += subscript_dy();
}

static void
cm_emit_string(struct atom *p)
{
	//cm_emit_str(TIMES_FONT, "\"");
	cm_emit_str(TIMES_FONT, p->u.str);
	//cm_emit_str(TIMES_FONT, "\"");
}

static void
cm_fixup_fraction(int x, int k1, int k2)
{
	int dx, dy, w;
	int h1, w1, y1;
	int h2, w2, y2;

	cm_get_size(k1, k2, &h1, &w1, &y1);
	cm_get_size(k2, indx, &h2, &w2, &y2);

	// We want to shift the numerator horizontally so it's centered.

	// Rounding up looks better.

	if (w2 > w1)
		dx = (w2 - w1 + 1) / 2;	// shift numerator right
	else
		dx = 0;

	// We want to shift the numerator vertically so it's above the rule.
	//
	//	h1 + shim + mheight = 1 - y
	//
	//	h1 + shim + mheight = 1 - y1 - dy
	//
	//	dy = 1 - y1 - h1 - shim - mheight

	dy = 1 - y1 - h1 - shim() - mheight();

	cm_move(k1, k2, dx, dy);

	if (w2 > w1)
		dx = -w1;
	else
		dx = -w1 + (w1 - w2) / 2;

	//	y2 + dy = -mheight + hrule_thickness + shim
	//
	//	dy = -mheight + hrule_thickness + shim - y2

	dy = -mheight() + shim() - y2;

	cm_move(k2, indx, dx, dy);

	if (w2 > w1)
		w = w2;
	else
		w = w1;

	xpos = x;

	cm_emit_hrule(w);
}

//	Indices k1 through k2-1 are the glyph on the left.
//
//	Indices k2 through index-1 are the glyph to be superscripted.

static void
cm_fixup_power(int k1, int k2)
{
	int dx, dy;
	int h1, w1, y1;
	int h2, w2, y2;

	cm_get_size(k1, k2, &h1, &w1, &y1);
	cm_get_size(k2, indx, &h2, &w2, &y2);

	// move bottom of superscript to x height
	//
	// Example:
	//
	//  y = -9	****	]
	//	-8	*	]
	//	-7	***	] h2 = 5
	//	-6	*	]
	//	-5	****	]
	//	-4		]
	//	-3		]
	//	-2		] xheight = 5
	//	-1		]
	//	 0		]
	//	 1
	//
	// want y such that
	//
	//	h2 + xheight = 1 - y
	//
	// already have y2 which we need to adjust by dy so that y = y2 + dy
	//
	//	h2 + xheight = 1 - y2 - dy
	//
	//	dy = 1 - y2 - h2 - xheight

	dy = 1 - y2 - h2 - xheight();

	// if y1 < y2 + dy then boost up some more

	if (y1 < y2 + dy)
		dy = y1 - y2;

	dx = 0;

	cm_move(k2, indx, dx, dy);
}

static void
cm_move(int j, int k, int dx, int dy)
{
	int i;
	for (i = j; i < k; i++) {
		chartab[i].x += dx;
		chartab[i].y += dy;
	}
}

// finds the bounding rectangle and vertical position

static void
cm_get_size(int j, int k, int *h, int *w, int *y)
{
	int i;
	int min_x, max_x, min_y, max_y;
	min_x = chartab[j].x;
	max_x = chartab[j].x + chartab[j].w - 1;
	min_y = chartab[j].y;
	max_y = chartab[j].y + chartab[j].h - 1;
	for (i = j + 1; i < k; i++) {
		if (chartab[i].x < min_x)
			min_x = chartab[i].x;
		if (chartab[i].x + chartab[i].w - 1 > max_x)
			max_x = chartab[i].x + chartab[i].w - 1;
		if (chartab[i].y < min_y)
			min_y = chartab[i].y;
		if (chartab[i].y + chartab[i].h - 1 > max_y)
			max_y = chartab[i].y + chartab[i].h - 1;
	}
	*h = max_y - min_y + 1;
	*w = max_x - min_x + 1;
	*y = min_y;
}

static void
cm_emit_char(int font, int c)
{
	char s[2];
	s[0] = c;
	s[1] = 0;
	cm_emit_str(font, s);
}

static void
cm_emit_str(int font, char *s)
{
	int h, w;
	check_chartab();
	if (level)
		font += 4;
	get_height_width(&h, &w, font, s);
	chartab[indx].cmd = font;
	chartab[indx].s = strdup(s);
	chartab[indx].x = xpos;
	chartab[indx].y = -text_metric[font].ascent;
	chartab[indx].h = h;
	chartab[indx].w = w;
	xpos += w;
	indx++;
}

static void
cm_cm_emit_number(struct atom *p, int cm_emit_sign)
{
	int i, k1, k2, len;
	char *s;
	static char buf[100];
	switch (p->k) {
	case RATIONAL:
		s = mstr(p->u.q.a);
		if (*s == '-' && cm_emit_sign == 0)
			s++;
		cm_emit_str(TIMES_FONT, s);
		s = mstr(p->u.q.b);
		if (strcmp(s, "1") == 0)
			break;
		cm_emit_char(TIMES_FONT, '/');
		cm_emit_str(TIMES_FONT, s);
		break;
	case DOUBLE:
		len = sprintf(buf, "%g", p->u.d);
		for (i = 0; i < len; i++)
			if (buf[i] == 'E' || buf[i] == 'e') {
				buf[i] = 0;
				break;
			}
		s = buf;
		if (*s == '-' && cm_emit_sign == 0)
			s++;
		cm_emit_str(TIMES_FONT, s);
		if (strchr(s, '.') == NULL)
			cm_emit_str(TIMES_FONT, ".0");
		if (i < len) {
			s = buf + i + 1;
			cm_emit_char(SYMBOL_FONT, MULTIPLICATION);
			k1 = indx;
			cm_emit_str(TIMES_FONT, "10");
			k2 = indx;
			level++;
			cm_emit_very_thin_space(); // FIXME
			if (*s == '+')
				s++;
			else if (*s == '-') {
				cm_emit_char(SYMBOL_FONT, MINUS);
				s++;
			}
			// don't emit leading zeroes
			while (*s && *s == '0')
				s++;
			if (*s)
				cm_emit_str(TIMES_FONT, s);
			level--;
			cm_fixup_power(k1, k2);
		}
		break;
	default:
		break;
	}
}

#define SPACE_BETWEEN_COLUMNS 20
#define SPACE_BETWEEN_ROWS 20

static void
cm_emit_tensor(struct atom *p)
{
	int h, i, j, k, n, w, x, y;
	int nrow, ncol;
	int dw, dx, dy;
	int col_width[100], ymin[100], ymax[100];

	static struct {
		int y, h, w, index1, index2;
	} elem[10000];

	if (p->u.tensor->ndim > 2)
		stop("Tensor rank > 2 in display().\nTry using print() or set tty=1.");

	nrow = p->u.tensor->dim[0];

	if (p->u.tensor->ndim == 2)
		ncol = p->u.tensor->dim[1];
	else
		ncol = 1;

	n = nrow * ncol;

	if (nrow > 100 || ncol > 100)
		stop("too many tensor components to display, try using print() or set tty=1");

	cm_emit_char(TIMES_FONT, '(');

	// horizontal coordinate of the matrix

	x = xpos;

	// cm_emit each component

	for (i = 0; i < n; i++) {
		elem[i].index1 = indx;
		cm_emit_expr(p->u.tensor->elem[i]);
		elem[i].index2 = indx;
		cm_get_size(elem[i].index1, indx, &elem[i].h, &elem[i].w, &elem[i].y);
		xpos = x; // put all components at the same x
	}

	// calculate the height of each row

	for (i = 0; i < nrow; i++) {
		ymin[i] = elem[i * ncol].y;
		ymax[i] = elem[i * ncol].y + elem[i * ncol].h;
		for (j = 1; j < ncol; j++) {
			k = i * ncol + j;
			if (elem[k].y < ymin[i])
				ymin[i] = elem[k].y;
			y = elem[k].y + elem[k].h;
			if (y > ymax[i])
				ymax[i] = y;
		}
	}

	// calculate the width of each column

	for (i = 0; i < ncol; i++) {
		col_width[i] = elem[i].w;
		for (j = 1; j < nrow; j++) {
			k = j * ncol + i;
			if (elem[k].w > col_width[i])
				col_width[i] = elem[k].w;
		}
	}

	// this is the overall height of the matrix

	h = (nrow - 1) * SPACE_BETWEEN_ROWS;
	for (i = 0; i < nrow; i++)
		h += ymax[i] - ymin[i];

	// this is the overall width of the matrix

	w = (ncol - 1) * SPACE_BETWEEN_COLUMNS;
	for (i = 0; i < ncol; i++)
		w += col_width[i];

	// this is the y coordinate of the entire tensor

	y = -(h / 2) - mheight();

	// move rows up and down

	for (i = 0; i < nrow; i++) {
		dy = y - ymin[i];
		for (j = 0; j < ncol; j++) {
			k = i * ncol + j;
			cm_move(elem[k].index1, elem[k].index2, 0, dy);
		}
		y += ymax[i] - ymin[i] + SPACE_BETWEEN_ROWS;
	}

	// move columns to the right

	dw = 0;
	for (i = 0; i < nrow; i++) {
		dw = 0;
		for (j = 0; j < ncol; j++) {
			k = i * ncol + j;
			dx = dw + (col_width[j] - elem[k].w) / 2;
			cm_move(elem[k].index1, elem[k].index2, dx, 0);
			dw += col_width[j] + SPACE_BETWEEN_COLUMNS;
		}
	}

	xpos = x + w;

	cm_emit_char(TIMES_FONT, ')');
}

static void
cm_emit_flat_tensor(struct atom *p)
{
	int k = 0;
	cm_emit_tensor_inner(p, 0, &k);
}

static void
cm_emit_tensor_inner(struct atom *p, int j, int *k)
{
	int i;
	cm_emit_char(TIMES_FONT, '(');
	for (i = 0; i < p->u.tensor->dim[j]; i++) {
		if (j + 1 == p->u.tensor->ndim) {
			cm_emit_expr(p->u.tensor->elem[*k]);
			*k = *k + 1;
		} else
			cm_emit_tensor_inner(p, j + 1, k);
		if (i + 1 < p->u.tensor->dim[j])
			cm_emit_char(TIMES_FONT, ',');
	}
	cm_emit_char(TIMES_FONT, ')');
}

static void
cm_emit_hrule(int w)
{
	check_chartab();
	chartab[indx].cmd = DRAW_HRULE;
	chartab[indx].s = NULL;
	chartab[indx].x = xpos;
	chartab[indx].y = -mheight();
	chartab[indx].w = w;
	chartab[indx].h = hrule_thickness();
	xpos += w;
	indx++;
}

static void
cm_emit_space(int w)
{
	check_chartab();
	chartab[indx].cmd = DRAW_SPACE;
	chartab[indx].s = NULL;
	chartab[indx].x = xpos;
	chartab[indx].y = 0;
	chartab[indx].h = 0;
	chartab[indx].w = w;
	xpos += w;
	indx++;
}

static void
cm_emit_thick_space(void)
{
	int w;
	if (level == 0)
		w = text_metric[TIMES_FONT].width;
	else
		w = text_metric[SMALL_TIMES_FONT].width;
	cm_emit_space(w);
}

static void
cm_emit_thin_space(void)
{
	int w;
	if (level == 0)
		w = text_metric[TIMES_FONT].width / 2;
	else
		w = text_metric[SMALL_TIMES_FONT].width / 2;
	cm_emit_space(w);
}

static void
cm_emit_very_thin_space(void)
{
	int w;
	if (level == 0)
		w = text_metric[TIMES_FONT].width / 6;
	else
		w = text_metric[SMALL_TIMES_FONT].width / 6;
	cm_emit_space(w);
}

// if minus sign previously then cm_emit thin space

static void
cm_emit_minus_sign_space(void)
{
	if (indx && chartab[indx - 1].s && strcmp(chartab[indx - 1].s, MINUS_STR) == 0)
		cm_emit_thin_space();
}

static int
xheight(void)
{
	if (level == 0)
		return text_metric[TIMES_FONT].xheight;
	else
		return text_metric[SMALL_TIMES_FONT].xheight;

}

static int
subscript_dy(void)
{
	if (level == 0)
		return text_metric[TIMES_FONT].ascent / 3 + 1;
	else
		return text_metric[SMALL_TIMES_FONT].ascent / 3 + 1;
}

// math height

static int
mheight(void)
{
	if (level == 0)
		return 6; // tuned for 18 pt Times New Roman
	else
		return 5;
}

static int
shim(void)
{
	return 2;
}

static int
hrule_thickness(void)
{
	return 1;
}

static int
isgreek(char *s)
{
	int i;
	for (i = 0; i < NGREEK; i++)
		if (strncmp(greek[i].s, s, strlen(greek[i].s)) == 0)
			return i;
	return -1;
}

// for each pair of delimiters adjust the size if necessary

static void
do_groups(void)
{
	int i, j, n;
	for (i = 0; i < indx; i++) {
		if (chartab[i].s == NULL)
			continue;
		if (strcmp(chartab[i].s, "(") != 0)
			continue;
		n = 0;
		for (j = i + 1; j < indx; j++) {
			if (chartab[j].s == NULL)
				continue;
			if (strcmp(chartab[j].s, "(") == 0) {
				n++;
				continue;
			}
			if (strcmp(chartab[j].s, ")") != 0)
				continue;
			if (n) {
				n--;
				continue;
			}
			do_group(i, j);
			break;
		}
	}
}

// grow delimiters, maybe

static void
do_group(int i, int j)
{
	int h, w, y, ymin, ymax;

	cm_get_size(i, j, &h, &w, &y);

	ymin = chartab[i].y - chartab[i].h / 2;
	ymax = chartab[i].y + chartab[i].h + chartab[i].h / 3;

	if (y < ymin || y + h > ymax) {
		free(chartab[i].s); // free left paren
		free(chartab[j].s); // free right paren
		chartab[i].s = NULL;
		chartab[j].s = NULL;
		chartab[i].cmd = DRAW_LEFT_DELIMETER;
		chartab[j].cmd = DRAW_RIGHT_DELIMETER;
		chartab[i].y = y;
		chartab[j].y = y;
		chartab[i].h = h;
		chartab[j].h = h;
	}
}

// scan chartab and normalize y so the minimum y is 0

static void
normy(void)
{
	int i, miny;

	if (indx == 0)
		return;

	miny = chartab[0].y;

	for (i = 1; i < indx; i++)
		if (chartab[i].y < miny)
			miny = chartab[i].y;

	for (i = 0; i < indx; i++)
		chartab[i].y -= miny;
}

static void
check_chartab(void)
{
	int n;
	struct charspec *t;
	if (indx == cmax) {
		n = cmax + 1000;
		t = realloc(chartab, n * sizeof (struct charspec));
		if (t == NULL)
			stop("Out of memory");
		chartab = t;
		cmax = n;
	}
}

#include "defs.h"

#define CLIP 120 // max width of display

#define TABLE_HSPACE 3
#define TABLE_VSPACE 1

#define EMIT_SPACE 1
#define EMIT_CHAR 2
#define EMIT_LIST 3
#define EMIT_SUPERSCRIPT 4
#define EMIT_SUBSCRIPT 5
#define EMIT_SUBEXPR 6
#define EMIT_FRACTION 7
#define EMIT_TABLE 8

#define OPCODE(p) ((int) car(p)->u.d)
#define HEIGHT(p) ((int) cadr(p)->u.d)
#define DEPTH(p) ((int) caddr(p)->u.d)
#define WIDTH(p) ((int) cadddr(p)->u.d)

#define VAL1(p) ((int) car(p)->u.d)
#define VAL2(p) ((int) cadr(p)->u.d)

#define PLUS_SIGN '+'
#define MINUS_SIGN 0xe28892
#define MULTIPLY_SIGN 0xc397
#define GREATEREQUAL 0xe289a5
#define LESSEQUAL 0xe289a4

#define BDLL 0xe295b4 // BOX DRAW LIGHT LEFT
#define BDLR 0xe295b6 // BOX DRAW LIGHT RIGHT

#define BDLH 0xe29480 // BOX DRAW LIGHT HORIZONTAL
#define BDLV 0xe29482 // BOX DRAW LIGHT VERTICAL

#define BDLDAR 0xe2948c // BOX DRAW LIGHT DOWN AND RIGHT
#define BDLDAL 0xe29490 // BOX DRAW LIGHT DOWN AND LEFT
#define BDLUAR 0xe29494 // BOX DRAW LIGHT UP AND RIGHT
#define BDLUAL 0xe29498 // BOX DRAW LIGHT UP AND LEFT

#define imax(a, b) (a > b ? a : b)
#define imin(a, b) (a < b ? a : b)

int emit_level;
int display_nrow;
int display_ncol;
int *display_buf;

void
display(void)
{
	int c, d, h, i, j, n, w;

	save();

	emit_level = 0;

	p1 = pop();

	emit_list(p1);

	p1 = pop();

	h = HEIGHT(p1);
	d = DEPTH(p1);
	w = WIDTH(p1);

	display_nrow = h + d;
	display_ncol = imin(CLIP, w);

	n = display_nrow * display_ncol * sizeof (int); // number of bytes

	display_buf = malloc(n);

	if (display_buf == NULL)
		malloc_kaput();

	memset(display_buf, 0, n);

	emit_draw(0, h - 1, p1);

	for (i = 0; i < display_nrow; i++) {
		for (j = 0; j < display_ncol; j++) {
			c = display_buf[i * display_ncol + j];
			if (c == 0)
				putchar(' ');
			else if (c < 256)
				putchar(c);
			else if (c < 65536) {
				putchar(c >> 8);
				putchar(c & 0xff);
			} else {
				putchar(c >> 16);
				putchar(c >> 8 & 0xff);
				putchar(c & 0xff);
			}
		}
		putchar('\n');
	}

	free(display_buf);

	restore();
}

void
emit_args(struct atom *p)
{
	int t;

	p = cdr(p);

	if (!iscons(p)) {
		emit_roman_char('(');
		emit_roman_char(')');
		return;
	}

	t = tos;

	emit_expr(car(p));

	p = cdr(p);

	while (iscons(p)) {
		emit_roman_char(',');
		emit_expr(car(p));
		p = cdr(p);
	}

	emit_update_list(t);

	emit_update_subexpr();
}

void
emit_base(struct atom *p)
{
	if (isnegativenumber(p) || isfraction(p) || isdouble(p) || car(p) == symbol(ADD) || car(p) == symbol(MULTIPLY) || car(p) == symbol(POWER))
		emit_subexpr(p);
	else
		emit_expr(p);
}

void
emit_char(int x, int y, int c)
{
	if (x >= 0 && x < display_ncol && y >= 0 && y < display_nrow)
		display_buf[y * display_ncol + x] = c;
}

void
emit_delims(int x, int y, int h, int d, int w)
{
	if (h > 1 || d > 0) {
		emit_left_delim(x, y, h, d, w);
		emit_right_delim(x + w - 1, y, h, d, w);
	} else {
		emit_char(x, y, '(');
		emit_char(x + w - 1, y, ')');
	}
}

void
emit_denominators(struct atom *p)
{
	int n, t;
	char *s;
	struct atom *q;

	t = tos;

	n = count_denominators(p);

	p = cdr(p);
	q = car(p);

	if (isrational(q)) {
		if (!MEQUAL(q->u.q.b, 1)) {
			s = mstr(q->u.q.b);
			emit_roman_string(s);
			n++;
		}
		p = cdr(p);
	}

	while (iscons(p)) {

		q = car(p);
		p = cdr(p);

		if (!isdenominator(q))
			continue;

		if (tos - t)
			emit_space();

		if (isminusone(caddr(q))) {
			q = cadr(q);
			if (car(q) == symbol(ADD) && n == 1)
				emit_expr(q); // parens not needed
			else
				emit_factor(q);
		} else {
			emit_base(cadr(q));
			emit_numeric_exponent(caddr(q)); // sign is not emitted
		}
	}

	emit_update_list(t);
}

void
emit_double(struct atom *p)
{
	int i, j, k, t;

	if (p->u.d == 0.0) {
		emit_roman_char('0');
		return;
	}

	sprintf(tbuf, "%g", fabs(p->u.d));

	k = 0;

	while (isdigit(tbuf[k]) || tbuf[k] == '.')
		k++;

	// handle trailing zeroes

	j = k;

	if (strchr(tbuf, '.'))
		while (tbuf[j - 1] == '0' && tbuf[j - 2] != '.')
			j--;

	for (i = 0; i < j; i++)
		emit_roman_char(tbuf[i]);

	if (tbuf[k] != 'E' && tbuf[k] != 'e')
		return;

	k++;

	emit_roman_char(MULTIPLY_SIGN);

	emit_roman_string("10");

	// superscripted exponent

	emit_level++;

	t = tos;

	// sign of exponent

	if (tbuf[k] == '+')
		k++;
	else if (tbuf[k] == '-') {
		emit_roman_char(MINUS_SIGN);
		k++;
	}

	// skip leading zeroes in exponent

	while (tbuf[k] == '0')
		k++;

	emit_roman_string(tbuf + k);

	emit_update_list(t);

	emit_level--;

	emit_update_superscript();
}

void
emit_draw(int x, int y, struct atom *p)
{
	int d, dx, dy, h, i, k, w;

	k = OPCODE(p);
	h = HEIGHT(p);
	d = DEPTH(p);
	w = WIDTH(p);

	p = cddddr(p);

	switch (k) {

	case EMIT_SPACE:
		break;

	case EMIT_CHAR:
		emit_char(x, y, VAL1(p));
		break;

	case EMIT_LIST:
		p = car(p);
		while (iscons(p)) {
			emit_draw(x, y, car(p));
			x += WIDTH(car(p));
			p = cdr(p);
		}
		break;

	case EMIT_SUPERSCRIPT:
	case EMIT_SUBSCRIPT:
		x += VAL1(p);
		y += VAL2(p);
		p = caddr(p);
		emit_draw(x, y, p);
		break;

	case EMIT_SUBEXPR:
		emit_delims(x, y, h, d, w);
		x += 1;
		emit_draw(x, y, car(p));
		break;

	case EMIT_FRACTION:

		// horizontal line

		emit_char(x, y, BDLR);

		for (i = 1; i < w - 1; i++)
			emit_char(x + i, y, BDLH);

		emit_char(x + w - 1, y, BDLL);

		// numerator

		dx = (w - WIDTH(car(p))) / 2;
		dy = -h + HEIGHT(car(p));
		emit_draw(x + dx, y + dy, car(p));

		// denominator

		p = cdr(p);
		dx = (w - WIDTH(car(p))) / 2;
		dy = d - DEPTH(car(p));
		emit_draw(x + dx, y + dy, car(p));

		break;

	case EMIT_TABLE:
		emit_delims(x, y, h, d, w);
		emit_table(x + 2, y - h + 1, p);
		break;
	}
}

void
emit_exponent(struct atom *p)
{
	if (isnum(p) && !isnegativenumber(p)) {
		emit_numeric_exponent(p); // sign is not emitted
		return;
	}

	emit_level++;
	emit_list(p);
	emit_level--;

	emit_update_superscript();
}

void
emit_expr(struct atom *p)
{
	if (isnegativeterm(p) || (car(p) == symbol(ADD) && isnegativeterm(cadr(p))))
		emit_roman_char(MINUS_SIGN);

	if (car(p) == symbol(ADD))
		emit_expr_nib(p);
	else
		emit_term(p);
}

void
emit_expr_nib(struct atom *p)
{
	p = cdr(p);
	emit_term(car(p));
	p = cdr(p);
	while (iscons(p)) {
		if (isnegativeterm(car(p)))
			emit_infix_operator(MINUS_SIGN);
		else
			emit_infix_operator(PLUS_SIGN);
		emit_term(car(p));
		p = cdr(p);
	}
}

void
emit_factor(struct atom *p)
{
	if (isrational(p)) {
		emit_rational(p);
		return;
	}

	if (isdouble(p)) {
		emit_double(p);
		return;
	}

	if (issymbol(p)) {
		emit_symbol(p);
		return;
	}

	if (isstr(p)) {
		emit_string(p);
		return;
	}

	if (istensor(p)) {
		emit_tensor(p);
		return;
	}

	if (iscons(p)) {
		if (car(p) == symbol(POWER))
			emit_power(p);
		else if (car(p) == symbol(ADD) || car(p) == symbol(MULTIPLY))
			emit_subexpr(p);
		else
			emit_function(p);
		return;
	}
}

void
emit_frac(struct atom *p)
{
	emit_numerators(p);
	emit_denominators(p);
	emit_update_fraction();
}

void
emit_function(struct atom *p)
{
	// d(f(x),x)

	if (car(p) == symbol(DERIVATIVE)) {
		emit_roman_char('d');
		emit_args(p);
		return;
	}

	// n!

	if (car(p) == symbol(FACTORIAL)) {
		p = cadr(p);
		if (isposint(p) || issymbol(p))
			emit_expr(p);
		else
			emit_subexpr(p);
		emit_roman_char('!');
		return;
	}

	// A[1,2]

	if (car(p) == symbol(INDEX)) {
		p = cdr(p);
		if (issymbol(car(p)))
			emit_symbol(car(p));
		else
			emit_subexpr(car(p));
		emit_indices(p);
		return;
	}

	if (car(p) == symbol(SETQ) || car(p) == symbol(TESTEQ)) {
		emit_expr(cadr(p));
		emit_infix_operator('=');
		emit_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTGE)) {
		emit_expr(cadr(p));
		emit_infix_operator(GREATEREQUAL);
		emit_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTGT)) {
		emit_expr(cadr(p));
		emit_infix_operator('>');
		emit_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTLE)) {
		emit_expr(cadr(p));
		emit_infix_operator(LESSEQUAL);
		emit_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTLT)) {
		emit_expr(cadr(p));
		emit_infix_operator('<');
		emit_expr(caddr(p));
		return;
	}

	// default

	if (issymbol(car(p)))
		emit_symbol(car(p));
	else
		emit_subexpr(car(p));

	emit_args(p);
}

void
emit_indices(struct atom *p)
{
	emit_roman_char('[');

	p = cdr(p);

	if (iscons(p)) {
		emit_expr(car(p));
		p = cdr(p);
		while (iscons(p)) {
			emit_roman_char(',');
			emit_expr(car(p));
			p = cdr(p);
		}
	}

	emit_roman_char(']');
}

void
emit_infix_operator(int c)
{
	emit_space();
	emit_roman_char(c);
	emit_space();
}

void
emit_left_delim(int x, int y, int h, int d, int w)
{
	int i;

	emit_char(x, y - h + 1, BDLDAR);

	for (i = 1; i < h + d - 1; i++)
		emit_char(x, y - h + 1 + i, BDLV);

	emit_char(x, y + d, BDLUAR);
}

void
emit_list(struct atom *p)
{
	int t = tos;
	emit_expr(p);
	emit_update_list(t);
}

void
emit_matrix(struct atom *p, int d, int k)
{
	int i, j, m, n, span;

	if (d == p->u.tensor->ndim) {
		emit_list(p->u.tensor->elem[k]);
		return;
	}

	// compute element span

	span = 1;

	for (i = d + 2; i < p->u.tensor->ndim; i++)
		span *= p->u.tensor->dim[i];

	n = p->u.tensor->dim[d];	// number of rows
	m = p->u.tensor->dim[d + 1];	// number of columns

	for (i = 0; i < n; i++)
		for (j = 0; j < m; j++)
			emit_matrix(p, d + 2, k + (i * m + j) * span);

	emit_update_table(n, m);
}

void
emit_numerators(struct atom *p)
{
	int t;
	char *s;
	struct atom *q;

	t = tos;

	p = cdr(p);
	q = car(p);

	if (isrational(q)) {
		if (!MEQUAL(q->u.q.a, 1)) {
			s = mstr(q->u.q.a);
			emit_roman_string(s);
		}
		p = cdr(p);
	}

	while (iscons(p)) {

		q = car(p);
		p = cdr(p);

		if (isdenominator(q))
			continue;

		if (tos - t)
			emit_space();

		emit_factor(q);
	}

	if (t == tos)
		emit_roman_char('1'); // no numerators

	emit_update_list(t);
}

// p is rational or double, sign is not emitted

void
emit_numeric_exponent(struct atom *p)
{
	int t;
	char *s;

	emit_level++;

	t = tos;

	if (isrational(p)) {
		s = mstr(p->u.q.a);
		emit_roman_string(s);
		if (!MEQUAL(p->u.q.b, 1)) {
			emit_roman_char('/');
			s = mstr(p->u.q.b);
			emit_roman_string(s);
		}
	} else
		emit_double(p);

	emit_update_list(t);

	emit_level--;

	emit_update_superscript();
}

void
emit_power(struct atom *p)
{
	if (cadr(p) == symbol(EXP1)) {
		emit_roman_string("exp");
		emit_subexpr(caddr(p));
		return;
	}

	if (isimaginaryunit(p)) {
		if (isimaginaryunit(get_binding(symbol(SYMBOL_J)))) {
			emit_roman_char('j');
			return;
		}
		if (isimaginaryunit(get_binding(symbol(SYMBOL_I)))) {
			emit_roman_char('i');
			return;
		}
	}

	if (isnegativenumber(caddr(p))) {
		emit_reciprocal(p);
		return;
	}

	emit_base(cadr(p));
	emit_exponent(caddr(p));
}

void
emit_rational(struct atom *p)
{
	int t;
	char *s;

	if (MEQUAL(p->u.q.b, 1)) {
		s = mstr(p->u.q.a);
		emit_roman_string(s);
		return;
	}

	emit_level++;

	t = tos;
	s = mstr(p->u.q.a);
	emit_roman_string(s);
	emit_update_list(t);

	t = tos;
	s = mstr(p->u.q.b);
	emit_roman_string(s);
	emit_update_list(t);

	emit_level--;

	emit_update_fraction();
}

// p = y^x where x is a negative number

void
emit_reciprocal(struct atom *p)
{
	int t;

	emit_roman_char('1'); // numerator

	t = tos;

	if (isminusone(caddr(p)))
		emit_expr(cadr(p));
	else {
		emit_base(cadr(p));
		emit_numeric_exponent(caddr(p)); // sign is not emitted
	}

	emit_update_list(t);

	emit_update_fraction();
}

void
emit_right_delim(int x, int y, int h, int d, int w)
{
	int i;

	emit_char(x, y - h + 1, BDLDAL);

	for (i = 1; i < h + d - 1; i++)
		emit_char(x, y - h + 1 + i, BDLV);

	emit_char(x, y + d, BDLUAL);
}

void
emit_roman_char(int c)
{
	int d, h, w;

	h = 1;
	d = 0;
	w = 1;

	push_double(EMIT_CHAR);
	push_double(h);
	push_double(d);
	push_double(w);
	push_double(c);

	list(5);
}

void
emit_roman_string(char *s)
{
	while (*s)
		emit_roman_char(*s++);
}

void
emit_space(void)
{
	push_double(EMIT_SPACE);
	push_double(0);
	push_double(0);
	push_double(1);

	list(4);
}

void
emit_string(struct atom *p)
{
	emit_roman_string(p->u.str);
}

void
emit_subexpr(struct atom *p)
{
	emit_list(p);
	emit_update_subexpr();
}

void
emit_symbol(struct atom *p)
{
	int k, t;
	char *s;

	if (p == symbol(EXP1)) {
		emit_roman_string("exp(1)");
		return;
	}

	s = printname(p);

	if (iskeyword(p) || p == symbol(LAST) || p == symbol(TRACE)) {
		emit_roman_string(s);
		return;
	}

	k = emit_symbol_fragment(s, 0);

	if (s[k] == '\0')
		return;

	// emit subscript

	emit_level++;

	t = tos;

	while (s[k] != '\0')
		k = emit_symbol_fragment(s, k);

	emit_update_list(t);

	emit_level--;

	emit_update_subscript();
}

#undef N
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

int symbol_unicode_tab[N] = {

	0xce91, // Alpha
	0xce92, // Beta
	0xce93, // Gamma
	0xce94, // Delta
	0xce95, // Epsilon
	0xce96, // Zeta
	0xce97, // Eta
	0xce98, // Theta
	0xce99, // Iota
	0xce9a, // Kappa
	0xce9b, // Lambda
	0xce9c, // Mu
	0xce9d, // Nu
	0xce9e, // Xi
	0xce9f, // Omicron
	0xcea0, // Pi
	0xcea1, // Rho
	0xcea3, // Sigma
	0xcea4, // Tau
	0xcea5, // Upsilon
	0xcea6, // Phi
	0xcea7, // Chi
	0xcea8, // Psi
	0xcea9, // Omega

	0xceb1, // alpha
	0xceb2, // beta
	0xceb3, // gamma
	0xceb4, // delta
	0xceb5, // epsilon
	0xceb6, // zeta
	0xceb7, // eta
	0xceb8, // theta
	0xceb9, // iota
	0xceba, // kappa
	0xcebb, // lambda
	0xcebc, // mu
	0xcebd, // nu
	0xcebe, // xi
	0xcebf, // omicron
	0xcf80, // pi
	0xcf81, // rho
	0xcf83, // sigma
	0xcf84, // tau
	0xcf85, // upsilon
	0xcf86, // phi
	0xcf87, // chi
	0xcf88, // psi
	0xcf89, // omega

	0xc4a7, // hbar
};

int
emit_symbol_fragment(char *s, int k)
{
	int c, i, n;
	char *t;

	for (i = 0; i < N; i++) {
		t = symbol_name_tab[i];
		n = (int) strlen(t);
		if (strncmp(s + k, t, n) == 0)
			break;
	}

	if (i == N) {
		emit_roman_char(s[k]);
		return k + 1;
	}

	c = symbol_unicode_tab[i];

	emit_roman_char(c);

	return k + n;
}

void
emit_table(int x, int y, struct atom *p)
{
	int cx, dx, i, j, m, n;
	int column_width, elem_width, row_depth, row_height;
	struct atom *d, *h, *w, *table;

	n = VAL1(p);
	m = VAL2(p);

	p = cddr(p);

	table = car(p);
	h = cadr(p);
	d = caddr(p);

	for (i = 0; i < n; i++) { // for each row

		row_height = VAL1(h);
		row_depth = VAL1(d);

		y += row_height;

		dx = 0;

		w = cadddr(p);

		for (j = 0; j < m; j++) { // for each column
			column_width = VAL1(w);
			elem_width = WIDTH(car(table));
			cx = x + dx + (column_width - elem_width) / 2; // center horizontal
			emit_draw(cx, y, car(table));
			dx += column_width + TABLE_HSPACE;
			table = cdr(table);
			w = cdr(w);
		}

		y += row_depth + TABLE_VSPACE;

		h = cdr(h);
		d = cdr(d);
	}
}

void
emit_tensor(struct atom *p)
{
	if (p->u.tensor->ndim % 2 == 1)
		emit_vector(p); // odd rank
	else
		emit_matrix(p, 0, 0); // even rank
}

void
emit_term(struct atom *p)
{
	if (car(p) == symbol(MULTIPLY))
		emit_term_nib(p);
	else
		emit_factor(p);
}

void
emit_term_nib(struct atom *p)
{
	if (count_denominators(p) > 0) {
		emit_frac(p);
		return;
	}

	// no denominators

	p = cdr(p);

	if (isminusone(car(p)) && !isdouble(car(p)))
		p = cdr(p); // sign already emitted

	emit_factor(car(p));

	p = cdr(p);

	while (iscons(p)) {
		emit_space();
		emit_factor(car(p));
		p = cdr(p);
	}
}

void
emit_update_fraction(void)
{
	int d, h, w;

	save();

	p2 = pop(); // denominator
	p1 = pop(); // numerator

	h = HEIGHT(p1) + DEPTH(p1);
	d = HEIGHT(p2) + DEPTH(p2);
	w = imax(WIDTH(p1), WIDTH(p2));

	h += 1;
	w += 2;

	push_double(EMIT_FRACTION);
	push_double(h);
	push_double(d);
	push_double(w);
	push(p1);
	push(p2);

	list(6);

	restore();
}

void
emit_update_list(int t)
{
	int d, h, i, w;

	if (tos - t == 1)
		return;

	save();

	h = 0;
	d = 0;
	w = 0;

	for (i = t; i < tos; i++) {
		p1 = stack[i];
		h = imax(h, HEIGHT(p1));
		d = imax(d, DEPTH(p1));
		w += WIDTH(p1);
	}

	list(tos - t);
	p1 = pop();

	push_double(EMIT_LIST);
	push_double(h);
	push_double(d);
	push_double(w);
	push(p1);

	list(5);

	restore();
}

void
emit_update_subexpr(void)
{
	int d, h, w;

	save();

	p1 = pop();

	h = HEIGHT(p1);
	d = DEPTH(p1);
	w = WIDTH(p1);

	if (h > 1 || d > 0) {
		h += 1;
		d += 1;
	}

	w += 2;

	push_double(EMIT_SUBEXPR);
	push_double(h);
	push_double(d);
	push_double(w);
	push(p1);

	list(5);

	restore();
}

void
emit_update_subscript(void)
{
	int d, dx, dy, h, w;

	save();

	p1 = pop();

	h = HEIGHT(p1);
	d = DEPTH(p1);
	w = WIDTH(p1);

	dx = 0;
	dy = 1;

	push_double(EMIT_SUBSCRIPT);
	push_double(h);
	push_double(d + dy);
	push_double(w);
	push_double(dx);
	push_double(dy);
	push(p1);

	list(7);

	restore();
}

void
emit_update_superscript(void)
{
	int d, dx, dy, h, w, y;

	save();

	p2 = pop(); // exponent
	p1 = pop(); // base

	h = HEIGHT(p2);
	d = DEPTH(p2);
	w = WIDTH(p2);

	// y is distance from baseline to bottom of superscript

	y = HEIGHT(p1) - d - 1;

	y = imax(y, 1);

	dx = 0;
	dy = -(y + d);

	h = y + h + d;
	d = 0;

	if (OPCODE(p1) == EMIT_SUBSCRIPT) {
		dx = -WIDTH(p1);
		w = imax(0, w - WIDTH(p1));
	}

	push(p1); // base

	push_double(EMIT_SUPERSCRIPT);
	push_double(h);
	push_double(d);
	push_double(w);
	push_double(dx);
	push_double(dy);
	push(p2);

	list(7);

	restore();
}

void
emit_update_table(int n, int m)
{
	int i, j, t;
	int d, h, w;
	int total_height, total_width;

	save();

	total_height = 0;
	total_width = 0;

	t = tos - n * m;

	// height of each row

	for (i = 0; i < n; i++) { // for each row
		h = 0;
		for (j = 0; j < m; j++) { // for each column
			p1 = stack[t + i * m + j];
			h = imax(h, HEIGHT(p1));
		}
		push_double(h);
		total_height += h;
	}

	list(n);
	p2 = pop();

	// depth of each row

	for (i = 0; i < n; i++) { // for each row
		d = 0;
		for (j = 0; j < m; j++) { // for each column
			p1 = stack[t + i * m + j];
			d = imax(d, DEPTH(p1));
		}
		push_double(d);
		total_height += d;
	}

	list(n);
	p3 = pop();

	// width of each column

	for (j = 0; j < m; j++) { // for each column
		w = 0;
		for (i = 0; i < n; i++) { // for each row
			p1 = stack[t + i * m + j];
			w = imax(w, WIDTH(p1));
		}
		push_double(w);
		total_width += w;
	}

	list(m);
	p4 = pop();

	// h, d, w for entire table centered vertical

	total_height += (n - 1) * TABLE_VSPACE + 2; // +2 for delimiters
	total_width += (m - 1) * TABLE_HSPACE + 4; // +4 for delimiters

	h = total_height / 2 + 1;
	d = total_height - h;
	w = total_width;

	list(n * m);
	p1 = pop();

	push_double(EMIT_TABLE);
	push_double(h);
	push_double(d);
	push_double(w);
	push_double(n);
	push_double(m);
	push(p1);
	push(p2);
	push(p3);
	push(p4);

	list(10);

	restore();
}

void
emit_vector(struct atom *p)
{
	int i, n, span, t;

	// compute element span

	span = 1;

	for (i = 1; i < p->u.tensor->ndim; i++)
		span *= p->u.tensor->dim[i];

	t = tos;

	n = p->u.tensor->dim[0]; // number of rows

	for (i = 0; i < n; i++)
		emit_matrix(p, 1, i * span);

	emit_update_table(n, 1);
}

int
count_denominators(struct atom *p)
{
	int n = 0;
	p = cdr(p);
	while (iscons(p)) {
		if (isdenominator(car(p)))
			n++;
		p = cdr(p);
	}
	return n;
}

int
isdenominator(struct atom *p)
{
	return car(p) == symbol(POWER) && isnegativenumber(caddr(p));
}

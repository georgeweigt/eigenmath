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

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

int fmt_level;
int fmt_nrow;
int fmt_ncol;
int *fmt_buf;

void
display(void)
{
	int c, d, h, i, j, n, w;

	save();

	fmt_level = 0;

	p1 = pop();

	fmt_list(p1);

	p1 = pop();

	h = HEIGHT(p1);
	d = DEPTH(p1);
	w = WIDTH(p1);

	fmt_nrow = h + d;
	fmt_ncol = MIN(CLIP, w);

	n = fmt_nrow * fmt_ncol * sizeof (int); // number of bytes

	fmt_buf = malloc(n);

	if (fmt_buf == NULL)
		malloc_kaput();

	memset(fmt_buf, 0, n);

	fmt_draw(0, h - 1, p1);

	for (i = 0; i < fmt_nrow; i++) {
		for (j = 0; j < fmt_ncol; j++) {
			c = fmt_buf[i * fmt_ncol + j];
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

	free(fmt_buf);

	restore();
}

void
fmt_args(struct atom *p)
{
	int t;

	p = cdr(p);

	if (!iscons(p)) {
		fmt_roman_char('(');
		fmt_roman_char(')');
		return;
	}

	t = tos;

	fmt_expr(car(p));

	p = cdr(p);

	while (iscons(p)) {
		fmt_roman_char(',');
		fmt_expr(car(p));
		p = cdr(p);
	}

	fmt_update_list(t);

	fmt_update_subexpr();
}

void
fmt_base(struct atom *p)
{
	if (isnegativenumber(p) || isfraction(p) || isdouble(p) || car(p) == symbol(ADD) || car(p) == symbol(MULTIPLY) || car(p) == symbol(POWER))
		fmt_subexpr(p);
	else
		fmt_expr(p);
}

void
fmt_denominators(struct atom *p)
{
	int n, t;
	char *s;
	struct atom *q;

	t = tos;

	n = count_denominators(p);

	p = cdr(p);
	q = car(p);

	while (iscons(p)) {

		q = car(p);
		p = cdr(p);

		if (!isdenominator(q))
			continue;

		if (tos > t)
			fmt_space();

		if (isrational(q)) {
			s = mstr(q->u.q.b);
			fmt_roman_string(s);
			continue;
		}

		if (isminusone(caddr(q))) {
			q = cadr(q);
			if (car(q) == symbol(ADD) && n == 1)
				fmt_expr(q); // parens not needed
			else
				fmt_factor(q);
		} else {
			fmt_base(cadr(q));
			fmt_numeric_exponent(caddr(q)); // sign is not emitted
		}
	}

	fmt_update_list(t);
}

void
fmt_double(struct atom *p)
{
	int i, j, k, t;

	if (p->u.d == 0.0) {
		fmt_roman_char('0');
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
		fmt_roman_char(tbuf[i]);

	if (tbuf[k] != 'E' && tbuf[k] != 'e')
		return;

	k++;

	fmt_roman_char(MULTIPLY_SIGN);

	fmt_roman_string("10");

	// superscripted exponent

	fmt_level++;

	t = tos;

	// sign of exponent

	if (tbuf[k] == '+')
		k++;
	else if (tbuf[k] == '-') {
		fmt_roman_char(MINUS_SIGN);
		k++;
	}

	// skip leading zeroes in exponent

	while (tbuf[k] == '0')
		k++;

	fmt_roman_string(tbuf + k);

	fmt_update_list(t);

	fmt_level--;

	fmt_update_superscript();
}

void
fmt_exponent(struct atom *p)
{
	if (isnum(p) && !isnegativenumber(p)) {
		fmt_numeric_exponent(p); // sign is not emitted
		return;
	}

	fmt_level++;
	fmt_list(p);
	fmt_level--;

	fmt_update_superscript();
}

void
fmt_expr(struct atom *p)
{
	if (isnegativeterm(p) || (car(p) == symbol(ADD) && isnegativeterm(cadr(p))))
		fmt_roman_char(MINUS_SIGN);

	if (car(p) == symbol(ADD))
		fmt_expr_nib(p);
	else
		fmt_term(p);
}

void
fmt_expr_nib(struct atom *p)
{
	p = cdr(p);
	fmt_term(car(p));
	p = cdr(p);
	while (iscons(p)) {
		if (isnegativeterm(car(p)))
			fmt_infix_operator(MINUS_SIGN);
		else
			fmt_infix_operator(PLUS_SIGN);
		fmt_term(car(p));
		p = cdr(p);
	}
}

void
fmt_factor(struct atom *p)
{
	if (isrational(p)) {
		fmt_rational(p);
		return;
	}

	if (isdouble(p)) {
		fmt_double(p);
		return;
	}

	if (issymbol(p)) {
		fmt_symbol(p);
		return;
	}

	if (isstr(p)) {
		fmt_string(p);
		return;
	}

	if (istensor(p)) {
		fmt_tensor(p);
		return;
	}

	if (iscons(p)) {
		if (car(p) == symbol(POWER))
			fmt_power(p);
		else if (car(p) == symbol(ADD) || car(p) == symbol(MULTIPLY))
			fmt_subexpr(p);
		else
			fmt_function(p);
		return;
	}
}

void
fmt_frac(struct atom *p)
{
	fmt_numerators(p);
	fmt_denominators(p);
	fmt_update_fraction();
}

void
fmt_function(struct atom *p)
{
	// d(f(x),x)

	if (car(p) == symbol(DERIVATIVE)) {
		fmt_roman_char('d');
		fmt_args(p);
		return;
	}

	// n!

	if (car(p) == symbol(FACTORIAL)) {
		p = cadr(p);
		if (isposint(p) || issymbol(p))
			fmt_expr(p);
		else
			fmt_subexpr(p);
		fmt_roman_char('!');
		return;
	}

	// A[1,2]

	if (car(p) == symbol(INDEX)) {
		p = cdr(p);
		if (issymbol(car(p)))
			fmt_symbol(car(p));
		else
			fmt_subexpr(car(p));
		fmt_indices(p);
		return;
	}

	if (car(p) == symbol(SETQ) || car(p) == symbol(TESTEQ)) {
		fmt_expr(cadr(p));
		fmt_infix_operator('=');
		fmt_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTGE)) {
		fmt_expr(cadr(p));
		fmt_infix_operator(GREATEREQUAL);
		fmt_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTGT)) {
		fmt_expr(cadr(p));
		fmt_infix_operator('>');
		fmt_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTLE)) {
		fmt_expr(cadr(p));
		fmt_infix_operator(LESSEQUAL);
		fmt_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTLT)) {
		fmt_expr(cadr(p));
		fmt_infix_operator('<');
		fmt_expr(caddr(p));
		return;
	}

	// default

	if (issymbol(car(p)))
		fmt_symbol(car(p));
	else
		fmt_subexpr(car(p));

	fmt_args(p);
}

void
fmt_indices(struct atom *p)
{
	fmt_roman_char('[');

	p = cdr(p);

	if (iscons(p)) {
		fmt_expr(car(p));
		p = cdr(p);
		while (iscons(p)) {
			fmt_roman_char(',');
			fmt_expr(car(p));
			p = cdr(p);
		}
	}

	fmt_roman_char(']');
}

void
fmt_infix_operator(int c)
{
	fmt_space();
	fmt_roman_char(c);
	fmt_space();
}

void
fmt_list(struct atom *p)
{
	int t = tos;
	fmt_expr(p);
	fmt_update_list(t);
}

void
fmt_matrix(struct atom *p, int d, int k)
{
	int i, j, m, n, span;

	if (d == p->u.tensor->ndim) {
		fmt_list(p->u.tensor->elem[k]);
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
			fmt_matrix(p, d + 2, k + (i * m + j) * span);

	fmt_update_table(n, m);
}

void
fmt_numerators(struct atom *p)
{
	int n, t;
	char *s;
	struct atom *q;

	t = tos;

	n = count_numerators(p);

	p = cdr(p);
	q = car(p);

	while (iscons(p)) {

		q = car(p);
		p = cdr(p);

		if (!isnumerator(q))
			continue;

		if (tos > t)
			fmt_space();

		if (isrational(q)) {
			s = mstr(q->u.q.a);
			fmt_roman_string(s);
			continue;
		}

		if (car(q) == symbol(ADD) && n == 1)
			fmt_expr(q); // parens not needed
		else
			fmt_factor(q);
	}

	if (t == tos)
		fmt_roman_char('1'); // no numerators

	fmt_update_list(t);
}

// p is rational or double, sign is not emitted

void
fmt_numeric_exponent(struct atom *p)
{
	int t;
	char *s;

	fmt_level++;

	t = tos;

	if (isrational(p)) {
		s = mstr(p->u.q.a);
		fmt_roman_string(s);
		if (!MEQUAL(p->u.q.b, 1)) {
			fmt_roman_char('/');
			s = mstr(p->u.q.b);
			fmt_roman_string(s);
		}
	} else
		fmt_double(p);

	fmt_update_list(t);

	fmt_level--;

	fmt_update_superscript();
}

void
fmt_power(struct atom *p)
{
	if (cadr(p) == symbol(EXP1)) {
		fmt_roman_string("exp");
		fmt_args(cdr(p));
		return;
	}

	if (isimaginaryunit(p)) {
		if (isimaginaryunit(get_binding(symbol(SYMBOL_J)))) {
			fmt_roman_char('j');
			return;
		}
		if (isimaginaryunit(get_binding(symbol(SYMBOL_I)))) {
			fmt_roman_char('i');
			return;
		}
	}

	if (isnegativenumber(caddr(p))) {
		fmt_reciprocal(p);
		return;
	}

	fmt_base(cadr(p));
	fmt_exponent(caddr(p));
}

void
fmt_rational(struct atom *p)
{
	int t;
	char *s;

	if (MEQUAL(p->u.q.b, 1)) {
		s = mstr(p->u.q.a);
		fmt_roman_string(s);
		return;
	}

	fmt_level++;

	t = tos;
	s = mstr(p->u.q.a);
	fmt_roman_string(s);
	fmt_update_list(t);

	t = tos;
	s = mstr(p->u.q.b);
	fmt_roman_string(s);
	fmt_update_list(t);

	fmt_level--;

	fmt_update_fraction();
}

// p = y^x where x is a negative number

void
fmt_reciprocal(struct atom *p)
{
	int t;

	fmt_roman_char('1'); // numerator

	t = tos;

	if (isminusone(caddr(p)))
		fmt_expr(cadr(p));
	else {
		fmt_base(cadr(p));
		fmt_numeric_exponent(caddr(p)); // sign is not emitted
	}

	fmt_update_list(t);

	fmt_update_fraction();
}

void
fmt_roman_char(int c)
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
fmt_roman_string(char *s)
{
	while (*s)
		fmt_roman_char(*s++);
}

void
fmt_space(void)
{
	push_double(EMIT_SPACE);
	push_double(0);
	push_double(0);
	push_double(1);

	list(4);
}

void
fmt_string(struct atom *p)
{
	fmt_roman_string(p->u.str);
}

void
fmt_subexpr(struct atom *p)
{
	fmt_list(p);
	fmt_update_subexpr();
}

void
fmt_symbol(struct atom *p)
{
	int k, t;
	char *s;

	if (p == symbol(EXP1)) {
		fmt_roman_string("exp(1)");
		return;
	}

	s = printname(p);

	if (iskeyword(p) || p == symbol(LAST) || p == symbol(TRACE)) {
		fmt_roman_string(s);
		return;
	}

	k = fmt_symbol_fragment(s, 0);

	if (s[k] == '\0')
		return;

	// emit subscript

	fmt_level++;

	t = tos;

	while (s[k] != '\0')
		k = fmt_symbol_fragment(s, k);

	fmt_update_list(t);

	fmt_level--;

	fmt_update_subscript();
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
fmt_symbol_fragment(char *s, int k)
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
		fmt_roman_char(s[k]);
		return k + 1;
	}

	c = symbol_unicode_tab[i];

	fmt_roman_char(c);

	return k + n;
}

void
fmt_table(int x, int y, struct atom *p)
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
			fmt_draw(cx, y, car(table));
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
fmt_tensor(struct atom *p)
{
	if (p->u.tensor->ndim % 2 == 1)
		fmt_vector(p); // odd rank
	else
		fmt_matrix(p, 0, 0); // even rank
}

void
fmt_term(struct atom *p)
{
	if (car(p) == symbol(MULTIPLY))
		fmt_term_nib(p);
	else
		fmt_factor(p);
}

void
fmt_term_nib(struct atom *p)
{
	if (find_denominator(p)) {
		fmt_frac(p);
		return;
	}

	// no denominators

	p = cdr(p);

	if (isminusone(car(p)) && !isdouble(car(p)))
		p = cdr(p); // sign already emitted

	fmt_factor(car(p));

	p = cdr(p);

	while (iscons(p)) {
		fmt_space();
		fmt_factor(car(p));
		p = cdr(p);
	}
}

void
fmt_update_fraction(void)
{
	int d, h, w;

	save();

	p2 = pop(); // denominator
	p1 = pop(); // numerator

	h = HEIGHT(p1) + DEPTH(p1);
	d = HEIGHT(p2) + DEPTH(p2);
	w = MAX(WIDTH(p1), WIDTH(p2));

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
fmt_update_list(int t)
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
		h = MAX(h, HEIGHT(p1));
		d = MAX(d, DEPTH(p1));
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
fmt_update_subexpr(void)
{
	int d, h, w;

	save();

	p1 = pop();

	h = HEIGHT(p1);
	d = DEPTH(p1);
	w = WIDTH(p1);

	// delimiters have vertical symmetry (h - m == d + m, m = 1/2)

	if (h > 1 || d > 0) {
		h = MAX(h, d + 1) + 1; // plus extra
		d = h - 1; // by symmetry
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
fmt_update_subscript(void)
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
fmt_update_superscript(void)
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

	y = MAX(y, 1);

	dx = 0;
	dy = -(y + d);

	h = y + h + d;
	d = 0;

	if (OPCODE(p1) == EMIT_SUBSCRIPT) {
		dx = -WIDTH(p1);
		w = MAX(0, w - WIDTH(p1));
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
fmt_update_table(int n, int m)
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
			h = MAX(h, HEIGHT(p1));
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
			d = MAX(d, DEPTH(p1));
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
			w = MAX(w, WIDTH(p1));
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
fmt_vector(struct atom *p)
{
	int i, n, span;

	// compute element span

	span = 1;

	n = p->u.tensor->ndim;

	for (i = 1; i < n; i++)
		span *= p->u.tensor->dim[i];

	n = p->u.tensor->dim[0]; // number of rows

	for (i = 0; i < n; i++)
		fmt_matrix(p, 1, i * span);

	fmt_update_table(n, 1); // n rows, 1 column
}

int
find_denominator(struct atom *p)
{
	struct atom *q;
	p = cdr(p);
	while (iscons(p)) {
		q = car(p);
		if (car(q) == symbol(POWER) && isnegativenumber(caddr(q)))
			return 1;
		p = cdr(p);
	}
	return 0;
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
	if (car(p) == symbol(POWER) && isnegativenumber(caddr(p)))
		return 1;
	else if (isrational(p) && !MEQUAL(p->u.q.b, 1))
		return 1;
	else
		return 0;
}

int
count_numerators(struct atom *p)
{
	int n = 0;
	p = cdr(p);
	while (iscons(p)) {
		if (isnumerator(car(p)))
			n++;
		p = cdr(p);
	}
	return n;
}

int
isnumerator(struct atom *p)
{
	if (car(p) == symbol(POWER) && isnegativenumber(caddr(p)))
		return 0;
	else if (isrational(p) && MEQUAL(p->u.q.a, 1))
		return 0;
	else
		return 1;
}

void
fmt_draw(int x, int y, struct atom *p)
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
		fmt_draw_char(x, y, VAL1(p));
		break;

	case EMIT_LIST:
		p = car(p);
		while (iscons(p)) {
			fmt_draw(x, y, car(p));
			x += WIDTH(car(p));
			p = cdr(p);
		}
		break;

	case EMIT_SUPERSCRIPT:
	case EMIT_SUBSCRIPT:
		dx = VAL1(p);
		dy = VAL2(p);
		p = caddr(p);
		fmt_draw(x + dx, y + dy, p);
		break;

	case EMIT_SUBEXPR:
		fmt_draw_delims(x, y, h, d, w);
		fmt_draw(x + 1, y, car(p));
		break;

	case EMIT_FRACTION:

		// horizontal line

		fmt_draw_char(x, y, BDLR);

		for (i = 1; i < w - 1; i++)
			fmt_draw_char(x + i, y, BDLH);

		fmt_draw_char(x + w - 1, y, BDLL);

		// numerator

		dx = (w - WIDTH(car(p))) / 2;
		dy = -h + HEIGHT(car(p));
		fmt_draw(x + dx, y + dy, car(p));

		// denominator

		p = cdr(p);
		dx = (w - WIDTH(car(p))) / 2;
		dy = d - DEPTH(car(p));
		fmt_draw(x + dx, y + dy, car(p));

		break;

	case EMIT_TABLE:
		fmt_draw_delims(x, y, h, d, w);
		fmt_draw_table(x + 2, y - h + 1, p);
		break;
	}
}

void
fmt_draw_char(int x, int y, int c)
{
	if (x >= 0 && x < fmt_ncol && y >= 0 && y < fmt_nrow)
		fmt_buf[y * fmt_ncol + x] = c;
}

void
fmt_draw_delims(int x, int y, int h, int d, int w)
{
	if (h > 1 || d > 0) {
		fmt_draw_ldelim(x, y, h, d, w);
		fmt_draw_rdelim(x + w - 1, y, h, d, w);
	} else {
		fmt_draw_char(x, y, '(');
		fmt_draw_char(x + w - 1, y, ')');
	}
}

void
fmt_draw_ldelim(int x, int y, int h, int d, int w)
{
	int i;

	fmt_draw_char(x, y - h + 1, BDLDAR);

	for (i = 1; i < h + d - 1; i++)
		fmt_draw_char(x, y - h + 1 + i, BDLV);

	fmt_draw_char(x, y + d, BDLUAR);
}

void
fmt_draw_rdelim(int x, int y, int h, int d, int w)
{
	int i;

	fmt_draw_char(x, y - h + 1, BDLDAL);

	for (i = 1; i < h + d - 1; i++)
		fmt_draw_char(x, y - h + 1 + i, BDLV);

	fmt_draw_char(x, y + d, BDLUAL);
}

void
fmt_draw_table(int x, int y, struct atom *p)
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
			fmt_draw(cx, y, car(table));
			dx += column_width + TABLE_HSPACE;
			table = cdr(table);
			w = cdr(w);
		}

		y += row_depth + TABLE_VSPACE;

		h = cdr(h);
		d = cdr(d);
	}
}

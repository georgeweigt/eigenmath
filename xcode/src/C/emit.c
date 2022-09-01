#include "app.h"

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
		emit_thin_space();
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
emit_box(void)
{
	double x1, x2, y1, y2;

	x1 = DRAW_LEFT_MARGIN;
	x2 = DRAW_LEFT_MARGIN + DRAW_WIDTH;

	y1 = 0.0;
	y2 = DRAW_HEIGHT;

	// left

	emit_push(DRAW_STROKE);
	emit_push(x1);
	emit_push(y1);
	emit_push(x1);
	emit_push(y2);
	emit_push(DRAW_AXIS_STROKE);

	// right

	emit_push(DRAW_STROKE);
	emit_push(x2);
	emit_push(y1);
	emit_push(x2);
	emit_push(y2);
	emit_push(DRAW_AXIS_STROKE);

	// top

	emit_push(DRAW_STROKE);
	emit_push(x1);
	emit_push(y1);
	emit_push(x2);
	emit_push(y1);
	emit_push(DRAW_AXIS_STROKE);

	// bottom

	emit_push(DRAW_STROKE);
	emit_push(x1);
	emit_push(y2);
	emit_push(x2);
	emit_push(y2);
	emit_push(DRAW_AXIS_STROKE);
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

	while (iscons(p)) {

		q = car(p);
		p = cdr(p);

		if (!isdenominator(q))
			continue;

		if (tos > t)
			emit_medium_space();

		if (isrational(q)) {
			s = mstr(q->u.q.b);
			emit_roman_string(s);
			continue;
		}

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
	int t;
	char buf[24], *s;

	sprintf(buf, "%g", fabs(p->u.d));

	s = buf;

	while (*s && *s != 'E' && *s != 'e')
		emit_roman_char(*s++);

	if (!*s)
		return;

	s++;

	emit_roman_char(MULTIPLY_SIGN);

	emit_roman_string("10");

	// superscripted exponent

	emit_level++;

	t = tos;

	// sign of exponent

	if (*s == '+')
		s++;
	else if (*s == '-') {
		emit_roman_char(MINUS_SIGN);
		emit_thin_space();
		s++;
	}

	// skip leading zeroes in exponent

	while (*s == '0')
		s++;

	emit_roman_string(s);

	emit_update_list(t);

	emit_level--;

	emit_update_superscript();
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
	if (isnegativeterm(p) || (car(p) == symbol(ADD) && isnegativeterm(cadr(p)))) {
		emit_roman_char(MINUS_SIGN);
		emit_thin_space();
	}

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
emit_formula(double x, double y, struct atom *p)
{
	int char_num, font_num, k;
	double d, dx, dy, h, w;

	k = OPCODE(p);
	h = HEIGHT(p);
	d = DEPTH(p);
	w = WIDTH(p);

	p = cddddr(p);

	switch (k) {

	case EMIT_SPACE:
		break;

	case EMIT_CHAR:
		font_num = VAL1(p);
		char_num = VAL2(p);
		emit_formula_char(x, y, font_num, char_num);
		break;

	case EMIT_LIST:
		p = car(p);
		while (iscons(p)) {
			emit_formula(x, y, car(p));
			x += WIDTH(car(p));
			p = cdr(p);
		}
		break;

	case EMIT_SUPERSCRIPT:
	case EMIT_SUBSCRIPT:
		dx = VAL1(p);
		dy = VAL2(p);
		p = caddr(p);
		emit_formula(x + dx, y + dy, p);
		break;

	case EMIT_SUBEXPR:
		emit_formula_delims(x, y, h, d, w, FONT_SIZE * DELIM_STROKE, ROMAN_FONT);
		dx = get_char_width(ROMAN_FONT, '(');
		emit_formula(x + dx, y, car(p));
		break;

	case EMIT_SMALL_SUBEXPR:
		emit_formula_delims(x, y, h, d, w, SMALL_FONT_SIZE * DELIM_STROKE, SMALL_ROMAN_FONT);
		dx = get_char_width(SMALL_ROMAN_FONT, '(');
		emit_formula(x + dx, y, car(p));
		break;

	case EMIT_FRACTION:
		emit_formula_fraction(x, y, h, d, w, FONT_SIZE * FRAC_STROKE, ROMAN_FONT, p);
		break;

	case EMIT_SMALL_FRACTION:
		emit_formula_fraction(x, y, h, d, w, SMALL_FONT_SIZE * FRAC_STROKE, SMALL_ROMAN_FONT, p);
		break;

	case EMIT_TABLE:
		emit_formula_delims(x, y, h, d, w, 1.2 * FONT_SIZE * DELIM_STROKE, ROMAN_FONT);
		dx = get_char_width(ROMAN_FONT, '(');
		emit_formula_table(x + dx, y - h, p);
		break;
	}
}

void
emit_formula_delims(double x, double y, double h, double d, double w, double stroke_width, int font_num)
{
	double cd, ch, cw;

	ch = get_cap_height(font_num);
	cd = get_char_depth(font_num, '(');
	cw = get_char_width(font_num, '(');

	if (h > ch || d > cd) {
		emit_formula_ldelim(x, y, h, d, cw, stroke_width);
		emit_formula_rdelim(x + w - cw, y, h, d, cw, stroke_width);
	} else {
		emit_formula_char(x, y, font_num, '(');
		emit_formula_char(x + w - cw, y, font_num, ')');
	}
}

void
emit_formula_ldelim(double x, double y, double h, double d, double w, double stroke_width)
{
	double x1, x2, y1, y2;

	x1 = round(x + 0.5 * w);
	x2 = x1 + round(0.5 * w);

	y1 = round(y - h);
	y2 = round(y + d);

	emit_formula_stroke(x1, y1, x1, y2, stroke_width); // stem stroke
	emit_formula_stroke(x1, y1, x2, y1, stroke_width); // top stroke
	emit_formula_stroke(x1, y2, x2, y2, stroke_width); // bottom stroke
}

void
emit_formula_rdelim(double x, double y, double h, double d, double w, double stroke_width)
{
	double x1, x2, y1, y2;

	x1 = round(x + 0.5 * w);
	x2 = x1 - round(0.5 * w);

	y1 = round(y - h);
	y2 = round(y + d);

	emit_formula_stroke(x1, y1, x1, y2, stroke_width); // stem stroke
	emit_formula_stroke(x1, y1, x2, y1, stroke_width); // top stroke
	emit_formula_stroke(x1, y2, x2, y2, stroke_width); // bottom stroke
}

void
emit_formula_fraction(double x, double y, double h, double d, double w, double stroke_width, int font_num, struct atom *p)
{
	double dx, dy;

	// horizontal line

	dy = get_operator_height(font_num);
	emit_formula_stroke(x, y - dy, x + w, y - dy, stroke_width);

	// numerator

	dx = (w - WIDTH(car(p))) / 2.0;
	dy = h - HEIGHT(car(p));
	emit_formula(x + dx, y - dy, car(p));

	// denominator

	p = cdr(p);
	dx = (w - WIDTH(car(p))) / 2.0;
	dy = d - DEPTH(car(p));
	emit_formula(x + dx, y + dy, car(p));
}

void
emit_formula_table(double x, double y, struct atom *p)
{
	int i, j, m, n;
	double column_width, dx, elem_width, row_depth, row_height;
	struct atom *d, *h, *w, *table;

	n = (int) VAL1(p);
	m = (int) VAL2(p);

	p = cddr(p);

	table = car(p);
	h = cadr(p);
	d = caddr(p);

	for (i = 0; i < n; i++) { // for each row

		row_height = VAL1(h);
		row_depth = VAL1(d);

		y += row_height;

		dx = 0.0;

		w = cadddr(p);

		for (j = 0; j < m; j++) { // for each column

			column_width = VAL1(w);
			elem_width = WIDTH(car(table));
			emit_formula(x + dx + (column_width - elem_width) / 2.0, y, car(table));
			dx += column_width;
			table = cdr(table);
			w = cdr(w);
		}

		y += row_depth;

		h = cdr(h);
		d = cdr(d);
	}
}

void
emit_formula_char(double x, double y, int font_num, int char_num)
{
	emit_push(DRAW_CHAR);
	emit_push(x);
	emit_push(y);
	emit_push(font_num);
	emit_push(char_num);
}

void
emit_formula_stroke(double x1, double y1, double x2, double y2, double stroke_width)
{
	emit_push(DRAW_STROKE);
	emit_push(x1);
	emit_push(y1);
	emit_push(x2);
	emit_push(y2);
	emit_push(stroke_width);
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
emit_graph(void)
{
	int i;
	double x, y;
	struct atom *p1;

	emit_level = 1; // small font
	emit_index = 0;
	emit_count = 3 * draw_count + 37; // 37 = 1 for DRAW_END and 6 for each stroke (6 strokes)

	push_double(xmin);
	p1 = pop();
	emit_list(p1); // advance emit_count, leave result on stack

	push_double(xmax);
	p1 = pop();
	emit_list(p1); // advance emit_count, leave result on stack

	push_double(ymin);
	p1 = pop();
	emit_list(p1); // advance emit_count, leave result on stack

	push_double(ymax);
	p1 = pop();
	emit_list(p1); // advance emit_count, leave result on stack

	emit_display = malloc(sizeof (struct display) + emit_count * sizeof (double));

	if (emit_display == NULL)
		exit(1);

	emit_labels(); // uses the above results on stack

	emit_box();

	emit_xaxis();
	emit_yaxis();

	for (i = 0; i < draw_count; i++) {

		x = draw_buf[i].x;
		y = draw_buf[i].y;

		if (!inrange(x, y))
			continue;

		x += DRAW_LEFT_MARGIN;
		y = DRAW_HEIGHT - y;

		emit_push(DRAW_POINT);
		emit_push(x);
		emit_push(y);
	}

	emit_push(DRAW_END);

	emit_display->type = 1;
	emit_display->color = BLACK;

	emit_display->height = VPAD + DRAW_HEIGHT + DRAW_XLABEL_MARGIN + VPAD;
	emit_display->width = DRAW_LEFT_MARGIN + DRAW_WIDTH + DRAW_RIGHT_MARGIN;

	emit_display->dx = 0.0;
	emit_display->dy = VPAD;

	shipout(emit_display);
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
			emit_thin_space();
			emit_expr(car(p));
			p = cdr(p);
		}
	}

	emit_roman_char(']');
}

void
emit_infix_operator(int c)
{
	emit_thick_space();
	emit_roman_char(c);
	emit_thick_space();
}

void
emit_italic_char(int char_num)
{
	int font_num;
	double d, h, w;

	if (emit_level == 0)
		font_num = ITALIC_FONT;
	else
		font_num = SMALL_ITALIC_FONT;

	h = get_cap_height(font_num);
	d = get_char_depth(font_num, char_num);
	w = get_char_width(font_num, char_num);

	push_double(EMIT_CHAR);
	push_double(h);
	push_double(d);
	push_double(w);
	push_double(font_num);
	push_double(char_num);

	list(6);

	emit_count += 5; // alloc 5 for drawing italic char

	if (char_num == 'f')
		emit_thin_space();
}

void
emit_italic_string(char *s)
{
	while (*s)
		emit_italic_char(*s++);
}

void
emit_labels(void)
{
	double h, w, x, y;
	struct atom *p1;

	// ymax

	p1 = pop();

	h = HEIGHT(p1);
	w = WIDTH(p1);

	x = DRAW_LEFT_MARGIN - w - DRAW_YLABEL_MARGIN;
	y = h;

	emit_formula(x, y, p1);

	// ymin

	p1 = pop();

	w = WIDTH(p1);

	x = DRAW_LEFT_MARGIN - w - DRAW_YLABEL_MARGIN;
	y = DRAW_HEIGHT;

	emit_formula(x, y, p1);

	// xmax

	p1 = pop();

	w = WIDTH(p1);

	x = DRAW_LEFT_MARGIN + DRAW_WIDTH - w / 2.0; // center horizontally
	y = DRAW_HEIGHT + DRAW_XLABEL_MARGIN;

	emit_formula(x, y, p1);

	// xmin

	p1 = pop();

	w = WIDTH(p1);

	x = DRAW_LEFT_MARGIN - w / 2.0; // center horizontally
	y = DRAW_HEIGHT + DRAW_XLABEL_MARGIN;

	emit_formula(x, y, p1);
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
emit_medium_space(void)
{
	double w;

	if (emit_level == 0)
		w = 0.5 * get_char_width(ROMAN_FONT, 'n');
	else
		w = 0.5 * get_char_width(SMALL_ROMAN_FONT, 'n');

	push_double(EMIT_SPACE);
	push_double(0.0);
	push_double(0.0);
	push_double(w);

	list(4);
}

void
emit_numerators(struct atom *p)
{
	int n, t;
	char *s;
	struct atom *q;

	t = tos;
	n = count_numerators(p);
	p = cdr(p);

	while (iscons(p)) {

		q = car(p);
		p = cdr(p);

		if (!isnumerator(q))
			continue;

		if (tos > t)
			emit_medium_space();

		if (isrational(q)) {
			s = mstr(q->u.q.a);
			emit_roman_string(s);
			continue;
		}

		if (car(q) == symbol(ADD) && n == 1)
			emit_expr(q); // parens not needed
		else
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
		emit_args(cdr(p));
		return;
	}

	if (isimaginaryunit(p)) {
		if (isimaginaryunit(get_binding(symbol(J_LOWER)))) {
			emit_italic_char('j');
			return;
		}
		if (isimaginaryunit(get_binding(symbol(I_LOWER)))) {
			emit_italic_char('i');
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
emit_push(double d)
{
	if (emit_index == emit_count)
		stop("internal error: emit_push");
	emit_display->mem[emit_index++] = d;
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
emit_roman_char(int char_num)
{
	int font_num;
	double d, h, w;

	if (emit_level == 0)
		font_num = ROMAN_FONT;
	else
		font_num = SMALL_ROMAN_FONT;

	h = get_cap_height(font_num);
	d = get_char_depth(font_num, char_num);
	w = get_char_width(font_num, char_num);

	push_double(EMIT_CHAR);
	push_double(h);
	push_double(d);
	push_double(w);
	push_double(font_num);
	push_double(char_num);

	list(6);

	emit_count += 5; // alloc 5 for drawing roman char
}

void
emit_roman_string(char *s)
{
	while (*s)
		emit_roman_char(*s++);
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

	if (iskeyword(p) || p == symbol(LAST) || p == symbol(TRACE) || p == symbol(TTY)) {
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
	int char_num, i, n = 0;
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
	if (find_denominator(p)) {
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
		emit_medium_space();
		emit_factor(car(p));
		p = cdr(p);
	}
}

// 12.0625 + 3.9375 = 16.0 for TEXT_FONT

void
emit_text(char *buf, int len, int color)
{
	int i;
	double d, h, w;
	struct display *p;

	p = malloc(sizeof (struct display) + len);

	if (p == NULL)
		exit(1);

	p->len = len;

	memcpy(p->buf, buf, len);

	h = get_ascent(TEXT_FONT) + 1.0;
	d = get_descent(TEXT_FONT) + 3.0;

	w = 0.0;

	for (i = 0; i < len; i++)
		w += get_char_width(TEXT_FONT, buf[i]);

	p->type = 0;
	p->color = color;

	p->height = h + d;
	p->width = HPAD + w + HPAD;

	p->dx = HPAD;
	p->dy = h;

	shipout(p);
}

void
emit_thick_space(void)
{
	double w;

	if (emit_level == 0)
		w = get_char_width(ROMAN_FONT, 'n');
	else
		w = get_char_width(SMALL_ROMAN_FONT, 'n');

	push_double(EMIT_SPACE);
	push_double(0.0);
	push_double(0.0);
	push_double(w);

	list(4);
}

void
emit_thin_space(void)
{
	double w;

	if (emit_level == 0)
		w = 0.25 * get_char_width(ROMAN_FONT, 'n');
	else
		w = 0.25 * get_char_width(SMALL_ROMAN_FONT, 'n');

	push_double(EMIT_SPACE);
	push_double(0.0);
	push_double(0.0);
	push_double(w);

	list(4);
}

void
emit_update_fraction(void)
{
	int font_num, opcode;
	double d, h, m, v, w;
	struct atom *p1, *p2;

	p2 = pop(); // denominator
	p1 = pop(); // numerator

	h = HEIGHT(p1) + DEPTH(p1);
	d = HEIGHT(p2) + DEPTH(p2);
	w = fmax(WIDTH(p1), WIDTH(p2));

	if (emit_level == 0) {
		opcode = EMIT_FRACTION;
		font_num = ROMAN_FONT;
	} else {
		opcode = EMIT_SMALL_FRACTION;
		font_num = SMALL_ROMAN_FONT;
	}

	m = get_operator_height(font_num);

	v = 0.75 * m; // extra vertical space

	h += v + m;
	d += v - m;

	w += get_char_width(font_num, 'n') / 2.0; // make horizontal line a bit wider

	push_double(opcode);
	push_double(h);
	push_double(d);
	push_double(w);
	push(p1);
	push(p2);

	list(6);

	emit_count += 6; // alloc 6 for drawing horizontal line
}

void
emit_update_list(int t)
{
	int i, n;
	double d, h, w;
	struct atom *p1;

	n = tos - t;

	if (n == 1)
		return;

	h = 0.0;
	d = 0.0;
	w = 0.0;

	for (i = t; i < tos; i++) {
		p1 = stack[i];
		h = fmax(h, HEIGHT(p1));
		d = fmax(d, DEPTH(p1));
		w += WIDTH(p1);
	}

	list(n);
	p1 = pop();

	push_double(EMIT_LIST);
	push_double(h);
	push_double(d);
	push_double(w);
	push(p1);

	list(5);
}

void
emit_update_subexpr(void)
{
	int font_num, opcode;
	double d, h, m, w;
	struct atom *p1;

	p1 = pop();

	h = HEIGHT(p1);
	d = DEPTH(p1);
	w = WIDTH(p1);

	if (emit_level == 0) {
		opcode = EMIT_SUBEXPR;
		font_num = ROMAN_FONT;
	} else {
		opcode = EMIT_SMALL_SUBEXPR;
		font_num = SMALL_ROMAN_FONT;
	}

	h = fmax(h, get_cap_height(font_num));
	d = fmax(d, get_descent(font_num));

	// delimiters have vertical symmetry (h - m == d + m)

	if (h > get_cap_height(font_num) || d > get_descent(font_num)) {
		m = get_operator_height(font_num);
		h = fmax(h, d + 2.0 * m) + 0.5 * m; // plus a little extra
		d = h - 2.0 * m; // by symmetry
	}

	w += 2.0 * get_char_width(font_num, '(');

	push_double(opcode);
	push_double(h);
	push_double(d);
	push_double(w);
	push(p1);

	list(5);

	emit_count += 36; // alloc 36 for drawing delimiters
}

void
emit_update_subscript(void)
{
	int font_num;
	double d, dx, dy, h, t, w;
	struct atom *p1;

	p1 = pop();

	if (emit_level == 0)
		font_num = ROMAN_FONT;
	else
		font_num = SMALL_ROMAN_FONT;

	t = get_char_width(font_num, 'n') / 6.0;

	h = get_cap_height(font_num);
	d = DEPTH(p1);
	w = t + WIDTH(p1);

	dx = t;
	dy = h / 2.0;

	d += dy;

	push_double(EMIT_SUBSCRIPT);
	push_double(h);
	push_double(d);
	push_double(w);
	push_double(dx);
	push_double(dy);
	push(p1);

	list(7);
}

void
emit_update_superscript(void)
{
	int font_num;
	double d, dx, dy, h, t, w, y;
	struct atom *p1, *p2;

	p2 = pop(); // exponent
	p1 = pop(); // base

	if (emit_level == 0)
		font_num = ROMAN_FONT;
	else
		font_num = SMALL_ROMAN_FONT;

	t = get_char_width(font_num, 'n') / 6.0;

	h = HEIGHT(p2);
	d = DEPTH(p2);
	w = t + WIDTH(p2);

	// y is height of base

	y = HEIGHT(p1);

	// adjust

	y -= (h + d) / 2.0;

	y = fmax(y, get_xheight(font_num));

	dx = t;
	dy = -(y + d);

	h = y + h + d;
	d = 0.0;

	if (OPCODE(p1) == EMIT_SUBSCRIPT) {
		dx = -WIDTH(p1) + t;
		w = fmax(0.0, w - WIDTH(p1));
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
}

void
emit_update_table(int n, int m)
{
	int i, j, t;
	double d, h, w;
	double total_height, total_width;
	struct atom *p1, *p2, *p3, *p4;

	total_height = 0.0;
	total_width = 0.0;

	t = tos - n * m;

	// height of each row

	for (i = 0; i < n; i++) { // for each row
		h = 0.0;
		for (j = 0; j < m; j++) { // for each column
			p1 = stack[t + i * m + j];
			h = fmax(h, HEIGHT(p1));
		}
		h += TABLE_VSPACE;
		push_double(h);
		total_height += h;
	}

	list(n);
	p2 = pop();

	// depth of each row

	for (i = 0; i < n; i++) { // for each row
		d = 0.0;
		for (j = 0; j < m; j++) { // for each column
			p1 = stack[t + i * m + j];
			d = fmax(d, DEPTH(p1));
		}
		d += TABLE_VSPACE;
		push_double(d);
		total_height += d;
	}

	list(n);
	p3 = pop();

	// width of each column

	for (j = 0; j < m; j++) { // for each column
		w = 0.0;
		for (i = 0; i < n; i++) { // for each row
			p1 = stack[t + i * m + j];
			w = fmax(w, WIDTH(p1));
		}
		w += 2.0 * TABLE_HSPACE;
		push_double(w);
		total_width += w;
	}

	list(m);
	p4 = pop();

	// h, d, w for entire table

	h = total_height / 2.0 + get_operator_height(ROMAN_FONT);
	d = total_height - h;
	w = total_width + 2.0 * get_char_width(ROMAN_FONT, '(');

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

	emit_count += 36; // alloc 36 for drawing delimiters
}

void
emit_vector(struct atom *p)
{
	int i, n, span;

	// compute element span

	span = 1;

	n = p->u.tensor->ndim;

	for (i = 1; i < n; i++)
		span *= p->u.tensor->dim[i];

	n = p->u.tensor->dim[0]; // number of rows

	for (i = 0; i < n; i++)
		emit_matrix(p, 1, i * span);

	emit_update_table(n, 1); // n rows, 1 column
}

void
emit_xaxis(void)
{
	double x1, x2, y;

	y = DRAW_HEIGHT * (0.0 - ymin) / (ymax - ymin);
	y = DRAW_HEIGHT - y; // flip the y coordinate

	if (y <= 0 || y >= DRAW_HEIGHT)
		return;

	x1 = DRAW_LEFT_MARGIN;
	x2 = DRAW_LEFT_MARGIN + DRAW_WIDTH;

	emit_push(DRAW_STROKE);
	emit_push(x1);
	emit_push(y);
	emit_push(x2);
	emit_push(y);
	emit_push(DRAW_AXIS_STROKE);
}

void
emit_yaxis(void)
{
	double x, y1, y2;

	x = DRAW_WIDTH * (0.0 - xmin) / (xmax - xmin);

	if (x <= 0 || x >= DRAW_WIDTH)
		return;

	x += DRAW_LEFT_MARGIN;

	y1 = 0.0;
	y2 = DRAW_HEIGHT;

	emit_push(DRAW_STROKE);
	emit_push(x);
	emit_push(y1);
	emit_push(x);
	emit_push(y2);
	emit_push(DRAW_AXIS_STROKE);
}

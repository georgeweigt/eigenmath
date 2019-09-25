//	Examples:
//
//	   012345678
//	-2 .........
//	-1 .........
//	 0 ..hello..	x=2, y=0, h=1, w=5
//	 1 .........
//	 2 .........
//
//	   012345678
//	-2 .........
//	-1 ..355....
//	 0 ..---....	x=2, y=-1, h=3, w=3
//	 1 ..113....
//	 2 .........

#include "defs.h"

#undef YMAX
#define YMAX 10000

struct glyph {
	int c, x, y;
} chartab[YMAX];

int yindex, level, emit_x;
int expr_level;
int display_flag;

void
display(void)
{
	save();

	p1 = pop();

	yindex = 0;
	level = 0;
	emit_x = 0;

	emit_top_expr(p1);

	print_it();

	restore();
}

void
emit_top_expr(struct atom *p)
{
	if (car(p) == symbol(SETQ)) {
		emit_expr(cadr(p));
		emit_str(" = ");
		p = caddr(p);
	}

	if (istensor(p))
		emit_tensor(p);
	else
		emit_expr(p);
}

int
will_be_displayed_as_fraction(struct atom *p)
{
	if (level > 0)
		return 0;
	if (isfraction(p))
		return 1;
	if (car(p) != symbol(MULTIPLY))
		return 0;
	if (isfraction(cadr(p)))
		return 1;
	while (iscons(p)) {
		if (isdenominator(car(p)))
			return 1;
		p = cdr(p);
	}
	return 0;
}

void
emit_expr(struct atom *p)
{
	expr_level++;
	if (car(p) == symbol(ADD)) {
		p = cdr(p);
		if (is_negative(car(p))) {
			emit_char('-');
			if (will_be_displayed_as_fraction(car(p)))
				emit_char(' ');
		}
		emit_term(car(p));
		p = cdr(p);
		while (iscons(p)) {
			if (is_negative(car(p))) {
				emit_char(' ');
				emit_char('-');
				emit_char(' ');
			} else {
				emit_char(' ');
				emit_char('+');
				emit_char(' ');
			}
			emit_term(car(p));
			p = cdr(p);
		}
	} else {
		if (is_negative(p)) {
			emit_char('-');
			if (will_be_displayed_as_fraction(p))
				emit_char(' ');
		}
		emit_term(p);
	}
	expr_level--;
}

void
emit_unsigned_expr(struct atom *p)
{
	if (car(p) == symbol(ADD)) {
		p = cdr(p);
		emit_term(car(p));
		p = cdr(p);
		while (iscons(p)) {
			if (is_negative(car(p))) {
				emit_char(' ');
				emit_char('-');
				emit_char(' ');
			} else {
				emit_char(' ');
				emit_char('+');
				emit_char(' ');
			}
			emit_term(car(p));
			p = cdr(p);
		}
	} else
		emit_term(p);
}

int
is_negative(struct atom *p)
{
	if (isnegativenumber(p))
		return 1;
	if (car(p) == symbol(MULTIPLY) && isnegativenumber(cadr(p)))
		return 1;
	return 0;
}

void
emit_term(struct atom *p)
{
	int n;
	if (car(p) == symbol(MULTIPLY)) {
		n = count_denominators(p);
		if (n && level == 0)
			emit_fraction(p, n);
		else
			emit_multiply(p, n);
	} else
		emit_factor(p);
}

int
isdenominator(struct atom *p)
{
	if (car(p) == symbol(POWER) && cadr(p) != symbol(EXP1) && is_negative(caddr(p)))
		return 1;
	else
		return 0;
}

int
count_denominators(struct atom *p)
{
	int count = 0;
	struct atom *q;
	p = cdr(p);
	while (iscons(p)) {
		q = car(p);
		if (isdenominator(q))
			count++;
		p = cdr(p);
	}
	return count;
}

// n is the number of denominators, not counting a fraction like 1/2

void
emit_multiply(struct atom *p, int n)
{
	if (n == 0) {
		p = cdr(p);
		if (isplusone(car(p)) || isminusone(car(p)))
			p = cdr(p);
		emit_factor(car(p));
		p = cdr(p);
		while (iscons(p)) {
			emit_char(' ');
			emit_factor(car(p));
			p = cdr(p);
		}
	} else {
		emit_numerators(p);
		emit_char('/');
		// need grouping if more than one denominator
		if (n > 1 || isfraction(cadr(p))) {
			emit_char('(');
			emit_denominators(p);
			emit_char(')');
		} else
			emit_denominators(p);
	}
}

#undef A
#undef B

#define A p3
#define B p4

// sign of term has already been emitted

void
emit_fraction(struct atom *p, int d)
{
	int count, k1, k2, n, x;

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
		if (isdenominator(p2))
			;
		else
			n++;
		p1 = cdr(p1);
	}

	// emit numerators

	x = emit_x;

	k1 = yindex;

	count = 0;

	// emit numerical coefficient

	if (!isplusone(A)) {
		emit_number(A, 0);
		count++;
	}

	// skip over "multiply"

	p1 = cdr(p);

	// skip over numerical coefficient, already handled

	if (isnum(car(p1)))
		p1 = cdr(p1);

	while (iscons(p1)) {
		p2 = car(p1);
		if (isdenominator(p2))
			;
		else {
			if (count > 0)
				emit_char(' ');
			if (n == 1)
				emit_expr(p2);
			else
				emit_factor(p2);
			count++;
		}
		p1 = cdr(p1);
	}

	if (count == 0)
		emit_char('1');

	// emit denominators

	k2 = yindex;

	count = 0;

	if (!isplusone(B)) {
		emit_number(B, 0);
		count++;
		d++;
	}

	p1 = cdr(p);

	if (isrational(car(p1)))
		p1 = cdr(p1);

	while (iscons(p1)) {
		p2 = car(p1);
		if (isdenominator(p2)) {
			if (count > 0)
				emit_char(' ');
			emit_denominator(p2, d);
			count++;
		}
		p1 = cdr(p1);
	}

	fixup_fraction(x, k1, k2);

	restore();
}

// p points to a multiply

void
emit_numerators(struct atom *p)
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
		emit_number(p1, 0);
		n++;
	}

	while (iscons(p)) {
		if (isdenominator(car(p)))
			;
		else {
			if (n > 0)
				emit_char(' ');
			emit_factor(car(p));
			n++;
		}
		p = cdr(p);
	}

	if (n == 0)
		emit_char('1');

	restore();
}

// p points to a multiply

void
emit_denominators(struct atom *p)
{
	int n;

	save();

	n = 0;

	p = cdr(p);

	if (isfraction(car(p))) {
		push(car(p));
		denominator();
		p1 = pop();
		emit_number(p1, 0);
		n++;
		p = cdr(p);
	}

	while (iscons(p)) {
		if (isdenominator(car(p))) {
			if (n > 0)
				emit_char(' ');
			emit_denominator(car(p), 0);
			n++;
		}
		p = cdr(p);
	}

	restore();
}

void
emit_factor(struct atom *p)
{
	if (istensor(p)) {
		emit_flat_tensor(p);
		return;
	}

	if (isdouble(p)) {
		emit_number(p, 0);
		return;
	}

	if (car(p) == symbol(ADD) || car(p) == symbol(MULTIPLY)) {
		emit_subexpr(p);
		return;
	}

	if (car(p) == symbol(POWER)) {
		emit_power(p);
		return;
	}

	if (iscons(p)) {
		emit_function(p);
		return;
	}

	if (isnum(p)) {
		if (level == 0)
			emit_numerical_fraction(p);
		else
			emit_number(p, 0);
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
}

void
emit_numerical_fraction(struct atom *p)
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
		emit_number(A, 0);
		restore();
		return;
	}

	x = emit_x;

	k1 = yindex;

	emit_number(A, 0);

	k2 = yindex;

	emit_number(B, 0);

	fixup_fraction(x, k1, k2);

	restore();
}

// if it's a factor then it doesn't need parens around it, i.e. 1/sin(theta)^2

int
isfactor(struct atom *p)
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
	return 0;
}

void
emit_power(struct atom *p)
{
	int k1, k2, x;

	// imaginary unit

	if (isimaginaryunit(p)) {
		if (isimaginaryunit(binding[SYMBOL_J])) {
			emit_char('j');
			return;
		}
		if (isimaginaryunit(binding[SYMBOL_I])) {
			emit_char('i');
			return;
		}
	}

	if (cadr(p) == symbol(EXP1)) {
		emit_str("exp(");
		emit_expr(caddr(p));
		emit_char(')');
		return;
	}

	if (level > 0) {
		if (isminusone(caddr(p))) {
			emit_char('1');
			emit_char('/');
			if (isfactor(cadr(p)))
				emit_factor(cadr(p));
			else
				emit_subexpr(cadr(p));
		} else {
			if (isfactor(cadr(p)))
				emit_factor(cadr(p));
			else
				emit_subexpr(cadr(p));
			emit_char('^');
			if (isfactor(caddr(p)))
				emit_factor(caddr(p));
			else
				emit_subexpr(caddr(p));
		}
		return;
	}

	// special case: 1 over something

	if (is_negative(caddr(p))) {
		x = emit_x;
		k1 = yindex;
		emit_char('1');
		k2 = yindex;
		emit_denominator(p, 1);
		fixup_fraction(x, k1, k2);
		return;
	}

	k1 = yindex;
	if (isfactor(cadr(p)))
		emit_factor(cadr(p));
	else
		emit_subexpr(cadr(p));
	k2 = yindex;
	level++;
	emit_expr(caddr(p));
	level--;
	fixup_power(k1, k2);
}

// if n == 1 then emit as expr (no parens)

// p is a power

void
emit_denominator(struct atom *p, int n)
{
	int k1, k2;

	// special case: 1 over something

	if (isminusone(caddr(p))) {
		if (n == 1)
			emit_expr(cadr(p));
		else
			emit_factor(cadr(p));
		return;
	}

	k1 = yindex;

	// emit base

	if (isfactor(cadr(p)))
		emit_factor(cadr(p));
	else
		emit_subexpr(cadr(p));

	k2 = yindex;

	// emit exponent, don't emit minus sign

	level++;

	emit_unsigned_expr(caddr(p));

	level--;

	fixup_power(k1, k2);
}

void
emit_function(struct atom *p)
{
	if (car(p) == symbol(INDEX) && issymbol(cadr(p))) {
		emit_index_function(p);
		return;
	}

	if (car(p) == symbol(FACTORIAL)) {
		emit_factorial_function(p);
		return;
	}

	if (car(p) == symbol(DERIVATIVE))
		emit_char('d');
	else
		emit_symbol(car(p));
	emit_char('(');
	p = cdr(p);
	if (iscons(p)) {
		emit_expr(car(p));
		p = cdr(p);
		while (iscons(p)) {
			emit_char(',');
			emit_expr(car(p));
			p = cdr(p);
		}
	}
	emit_char(')');
}

void
emit_index_function(struct atom *p)
{
	p = cdr(p);
	if (caar(p) == symbol(ADD) || caar(p) == symbol(MULTIPLY) || caar(p) == symbol(POWER) || caar(p) == symbol(FACTORIAL))
		emit_subexpr(car(p));
	else
		emit_expr(car(p));
	emit_char('[');
	p = cdr(p);
	if (iscons(p)) {
		emit_expr(car(p));
		p = cdr(p);
		while(iscons(p)) {
			emit_char(',');
			emit_expr(car(p));
			p = cdr(p);
		}
	}
	emit_char(']');
}

void
emit_factorial_function(struct atom *p)
{
	p = cadr(p);
	if (car(p) == symbol(ADD) || car(p) == symbol(MULTIPLY) || car(p) == symbol(POWER) || car(p) == symbol(FACTORIAL))
		emit_subexpr(p);
	else
		emit_expr(p);
	emit_char('!');
}

void
emit_subexpr(struct atom *p)
{
	emit_char('(');
	emit_expr(p);
	emit_char(')');
}

void
emit_symbol(struct atom *p)
{
	char *s;

	if (p == symbol(EXP1)) {
		emit_str("exp(1)");
		return;
	}

	s = get_printname(p);
	while (*s)
		emit_char(*s++);
}

void
emit_string(struct atom *p)
{
	char *s;
	s = p->u.str;
	while (*s)
		emit_char(*s++);
}

void
fixup_fraction(int x, int k1, int k2)
{
	int dx, dy, i, w, y;
	int h1, w1, y1;
	int h2, w2, y2;

	get_size(k1, k2, &h1, &w1, &y1);
	get_size(k2, yindex, &h2, &w2, &y2);

	if (w2 > w1)
		dx = (w2 - w1) / 2;	// shift numerator right
	else
		dx = 0;

	dx++; // add 1 to center with vinculum

	// this is how much is below the baseline

	y = y1 + h1 - 1;

	dy = -y - 1;

	move(k1, k2, dx, dy);

	if (w2 > w1)
		dx = -w1;
	else
		dx = -w1 + (w1 - w2) / 2;

	dx++; // add 1 to center with vinculum

	dy = -y2 + 1;

	move(k2, yindex, dx, dy);

	if (w2 > w1)
		w = w2;
	else
		w = w1;

	w += 2; // make vinculum 2 chars wider

	emit_x = x;

	for (i = 0; i < w; i++)
		emit_char('-');
}

void
fixup_power(int k1, int k2)
{
	int dy;
	int h1, w1, y1;
	int h2, w2, y2;

	get_size(k1, k2, &h1, &w1, &y1);
	get_size(k2, yindex, &h2, &w2, &y2);

	// move superscript to baseline

	dy = -y2 - h2 + 1;

	// now move above base

	dy += y1 - 1;

	move(k2, yindex, 0, dy);
}

void
move(int j, int k, int dx, int dy)
{
	int i;
	for (i = j; i < k; i++) {
		chartab[i].x += dx;
		chartab[i].y += dy;
	}
}

// finds the bounding rectangle and vertical position

void
get_size(int j, int k, int *h, int *w, int *y)
{
	int i;
	int min_x, max_x, min_y, max_y;
	min_x = chartab[j].x;
	max_x = chartab[j].x;
	min_y = chartab[j].y;
	max_y = chartab[j].y;
	for (i = j + 1; i < k; i++) {
		if (chartab[i].x < min_x)
			min_x = chartab[i].x;
		if (chartab[i].x > max_x)
			max_x = chartab[i].x;
		if (chartab[i].y < min_y)
			min_y = chartab[i].y;
		if (chartab[i].y > max_y)
			max_y = chartab[i].y;
	}
	*h = max_y - min_y + 1;
	*w = max_x - min_x + 1;
	*y = min_y;
}

void
displaychar(int c)
{
	emit_char(c);
}

void
emit_char(int c)
{
	if (yindex == YMAX)
		return;
	chartab[yindex].c = c;
	chartab[yindex].x = emit_x;
	chartab[yindex].y = 0;
	yindex++;
	emit_x++;
}

void
emit_str(char *s)
{
	while (*s)
		emit_char(*s++);
}

void
emit_number(struct atom *p, int emit_sign)
{
	int k1, k2;
	char *s;
	switch (p->k) {
	case RATIONAL:
		s = mstr(p->u.q.a);
		if (*s == '-' && emit_sign == 0)
			s++;
		while (*s)
			emit_char(*s++);
		s = mstr(p->u.q.b);
		if (strcmp(s, "1") == 0)
			break;
		emit_char('/');
		while (*s)
			emit_char(*s++);
		break;
	case DOUBLE:
		sprintf(tbuf, "%g", p->u.d);
		s = tbuf;
		if (*s == '-') {
			if (emit_sign)
				print_char('-');
			s++;
		}
		while (isdigit(*s))
			emit_char(*s++);
		if (*s == '.') {
			emit_char(*s++);
			while (isdigit(*s))
				emit_char(*s++);
		} else
			emit_str(".0");
		if (*s == 'E' || *s == 'e') {
			s++;
			emit_char(' ');
			k1 = yindex;
			emit_str("10");
			k2 = yindex;
			level++;
			if (*s == '+')
				s++;
			else if (*s == '-')
				emit_char(*s++);
			while (*s == '0')
				s++; // skip leading zeroes
			emit_str(s);
			level--;
			fixup_power(k1, k2);
		}
		break;
	default:
		break;
	}
}

int
display_cmp(const void *aa, const void *bb)
{
	struct glyph *a, *b;

	a = (struct glyph *) aa;
	b = (struct glyph *) bb;

	if (a->y < b->y)
		return -1;

	if (a->y > b->y)
		return 1;

	if (a->x < b->x)
		return -1;

	if (a->x > b->x)
		return 1;

	return 0;
}

void
print_it(void)
{
	int i, x, y;

	qsort(chartab, yindex, sizeof (struct glyph), display_cmp);

	x = 0;

	y = chartab[0].y;

	for (i = 0; i < yindex; i++) {

		while (chartab[i].y > y) {
			printchar('\n');
			x = 0;
			y++;
		}

		while (chartab[i].x > x) {
			printchar(' ');
			x++;
		}

		printchar(chartab[i].c);

		x++;
	}

	printchar('\n');
}

char print_buffer[10000];

char *
getdisplaystr(void)
{
	yindex = 0;
	level = 0;
	emit_x = 0;
	emit_expr(pop());
	fill_buf();
	return print_buffer;
}

void
fill_buf(void)
{
	int i, k, x, y;

	qsort(chartab, yindex, sizeof (struct glyph), display_cmp);

	k = 0;
	x = 0;
	y = chartab[0].y;

	for (i = 0; i < yindex; i++) {

		while (chartab[i].y > y) {
			if (k < sizeof print_buffer - 2)
				print_buffer[k++] = '\n';
			x = 0;
			y++;
		}

		while (chartab[i].x > x) {
			if (k < sizeof print_buffer - 2)
				print_buffer[k++] = ' ';
			x++;
		}

		if (k < sizeof print_buffer - 2)
			print_buffer[k++] = chartab[i].c;

		x++;
	}

	if (k == sizeof print_buffer - 2)
		printf("warning: print buffer full\n");

	print_buffer[k++] = '\n';
	print_buffer[k++] = '\0';
}

#undef N

#define N 100

struct elem {
	int x, y, h, w, index, count;
} elem[N];

#define SPACE_BETWEEN_COLUMNS 3
#define SPACE_BETWEEN_ROWS 1

void
emit_tensor(struct atom *p)
{
	int i, n, nrow, ncol;
	int x, y;
	int h, w;
	int dx, dy;
	int eh, ew;
	int row, col;

	if (p->u.tensor->ndim > 2) {
		emit_flat_tensor(p);
		return;
	}

	nrow = p->u.tensor->dim[0];

	if (p->u.tensor->ndim == 2)
		ncol = p->u.tensor->dim[1];
	else
		ncol = 1;

	n = nrow * ncol;

	if (n > N) {
		emit_flat_tensor(p);
		return;
	}

	// horizontal coordinate of the matrix

	x = emit_x;

	// emit each element

	for (i = 0; i < n; i++) {
		elem[i].index = yindex;
		elem[i].x = emit_x;
		emit_expr(p->u.tensor->elem[i]);
		elem[i].count = yindex - elem[i].index;
		get_size(elem[i].index, yindex, &elem[i].h, &elem[i].w, &elem[i].y);
	}

	// find element height and width

	eh = 0;
	ew = 0;

	for (i = 0; i < n; i++) {
		if (elem[i].h > eh)
			eh = elem[i].h;
		if (elem[i].w > ew)
			ew = elem[i].w;
	}

	// this is the overall height of the matrix

	h = nrow * eh + (nrow - 1) * SPACE_BETWEEN_ROWS;

	// this is the overall width of the matrix

	w = ncol * ew + (ncol - 1) * SPACE_BETWEEN_COLUMNS;

	// this is the vertical coordinate of the matrix

	y = -(h / 2);

	// move elements around

	for (row = 0; row < nrow; row++) {
		for (col = 0; col < ncol; col++) {

			i = row * ncol + col;

			// first move to upper left corner of matrix

			dx = x - elem[i].x;
			dy = y - elem[i].y;

			move(elem[i].index, elem[i].index + elem[i].count, dx, dy);

			// now move to official position

			dx = 0;

			if (col > 0)
				dx = col * (ew + SPACE_BETWEEN_COLUMNS);

			dy = 0;

			if (row > 0)
				dy = row * (eh + SPACE_BETWEEN_ROWS);

			// small correction for horizontal centering

			dx += (ew - elem[i].w) / 2;

			// small correction for vertical centering

			dy += (eh - elem[i].h) / 2;

			move(elem[i].index, elem[i].index + elem[i].count, dx, dy);
		}
	}

	emit_x = x + w;
}

void
emit_flat_tensor(struct atom *p)
{
	int k = 0;
	emit_tensor_inner(p, 0, &k);
}

void
emit_tensor_inner(struct atom *p, int j, int *k)
{
	int i;
	emit_char('(');
	for (i = 0; i < p->u.tensor->dim[j]; i++) {
		if (j + 1 == p->u.tensor->ndim) {
			emit_expr(p->u.tensor->elem[*k]);
			*k = *k + 1;
		} else
			emit_tensor_inner(p, j + 1, k);
		if (i + 1 < p->u.tensor->dim[j])
			emit_char(',');
	}
	emit_char(')');
}

const HPAD = 10;
const VPAD = 10;

const FONT_SIZE = 24;
const SMALL_FONT_SIZE = 18;

const ROMAN_FONT = 1;
const ITALIC_FONT = 2;
const SMALL_ROMAN_FONT = 3;
const SMALL_ITALIC_FONT = 4;

const LEFT_PAREN = 40;
const RIGHT_PAREN = 41;
const LESS_SIGN = 60;
const EQUALS_SIGN = 61;
const GREATER_SIGN = 62;
const LOWER_N = 110;

const PLUS_SIGN = 177;
const MINUS_SIGN = 178;
const MULTIPLY_SIGN = 179;
const GREATEREQUAL = 180;
const LESSEQUAL = 181;

const EMIT_SPACE = 1;
const EMIT_CHAR = 2;
const EMIT_LIST = 3;
const EMIT_SUPERSCRIPT = 4;
const EMIT_SUBSCRIPT = 5;
const EMIT_SUBEXPR = 6;
const EMIT_SMALL_SUBEXPR = 7;
const EMIT_FRACTION = 8;
const EMIT_SMALL_FRACTION = 9;
const EMIT_TABLE = 10;

const THIN_STROKE = 1;
const MEDIUM_STROKE = 2;
const THICK_STROKE = 2.5;

const TABLE_HSPACE = 10;
const TABLE_VSPACE = 10;

var emit_level;

function
display()
{
	var d, h, p1, w, x, y;

	emit_level = 0;

	p1 = pop();

	emit_list(p1);

	p1 = pop();

	h = height(p1);
	d = depth(p1);
	w = width(p1);

	x = HPAD;
	y = h + VPAD;

	h += d + 2 * VPAD;
	w += 2 * HPAD;

	h = "height='" + h + "'";
	w = "width='" + w + "'";

	outbuf = "<svg " + h + w + ">";

	draw_formula(x, y, p1);

	outbuf += "</svg><br>";

	stdout.innerHTML += outbuf;
}

function
emit_args(p)
{
	var t;

	p = cdr(p);

	if (!iscons(p)) {
		emit_roman_string("(");
		emit_roman_string(")");
		return;
	}

	t = stack.length;

	emit_expr(car(p));

	p = cdr(p);

	while (iscons(p)) {
		emit_roman_string(",");
		emit_expr(car(p));
		p = cdr(p);
	}

	emit_update_list(t);

	emit_update_subexpr();
}

function
emit_base(p)
{
	if (isnegativenumber(p) || isfraction(p) || isdouble(p) || car(p) == symbol(ADD) || car(p) == symbol(MULTIPLY) || car(p) == symbol(POWER))
		emit_subexpr(p);
	else
		emit_expr(p);
}

function
emit_denominators(p)
{
	var n, q, t;

	t = stack.length;

	n = count_denominators(p);

	p = cdr(p);
	q = car(p);

	if (isrational(q)) {
		if (q.b != 1) {
			emit_roman_string(q.b.toFixed(0));
			n++;
		}
		p = cdr(p);
	}

	while (iscons(p)) {

		q = car(p);
		p = cdr(p);

		if (!isdenominator(q))
			continue;

		if (stack.length > t)
			emit_medium_space();

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

function
emit_double(p)
{
	var j, k, s, t;

	s = Math.abs(p.d).toPrecision(6);

	k = 0;

	while (isdigit(s.charAt(k)) || s.charAt(k) == ".")
		k++;

	// handle trailing zeroes

	j = k;

	if (s.indexOf(".") > 0) {
		while (s.charAt(j - 1) == "0")
			j--;
		if (s.charAt(j - 1) == ".")
			j--;
	}

	emit_roman_string(s.substring(0, j));

	if (s.charAt(k) != "E" && s.charAt(k) != "e")
		return;

	k++;

	emit_roman_char(MULTIPLY_SIGN);

	emit_roman_string("10");

	// superscripted exponent

	emit_level++;

	t = stack.length;

	// sign of exponent

	if (s.charAt(k) == "+")
		k++;
	else if (s.charAt(k) == "-") {
		emit_roman_char(MINUS_SIGN);
		emit_thin_space();
		k++;
	}

	// skip leading zeroes in exponent

	while (s.charAt(k) == "0")
		k++;

	emit_roman_string(s.substring(k));

	emit_update_list(t);

	emit_level--;

	emit_update_superscript();
}

function
emit_exponent(p)
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

function
emit_expr(p)
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

function
emit_expr_nib(p)
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

function
emit_factor(p)
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

	if (isstring(p)) {
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

function
emit_fraction(p)
{
	emit_numerators(p);
	emit_denominators(p);
	emit_update_fraction();
}

function
emit_function(p)
{
	// d(f(x),x)

	if (car(p) == symbol(DERIVATIVE)) {
		emit_roman_string("d");
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
		emit_roman_string("!");
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
		emit_infix_operator(EQUALS_SIGN);
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
		emit_infix_operator(GREATER_SIGN);
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
		emit_infix_operator(LESS_SIGN);
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

function
emit_indices(p)
{
	emit_roman_string("[");

	p = cdr(p);

	if (iscons(p)) {
		emit_expr(car(p));
		p = cdr(p);
		while (iscons(p)) {
			emit_roman_string(",");
			emit_expr(car(p));
			p = cdr(p);
		}
	}

	emit_roman_string("]");
}

function
emit_infix_operator(char_num)
{
	emit_thick_space();
	emit_roman_char(char_num);
	emit_thick_space();
}

function
emit_italic_char(char_num)
{
	var d, font_num, h, w;

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
}

function
emit_italic_string(s)
{
	var i;
	for (i = 0; i < s.length; i++)
		emit_italic_char(s.charCodeAt(i));
}

function
emit_list(p)
{
	var t = stack.length;
	emit_expr(p);
	emit_update_list(t);
}

function
emit_matrix(p, d, k)
{
	var i, j, m, n, span;

	if (d == p.dim.length) {
		emit_list(p.elem[k]);
		return;
	}

	// compute element span

	span = 1;

	n = p.dim.length;

	for (i = d + 2; i < n; i++)
		span *= p.dim[i];

	n = p.dim[d];		// number of rows
	m = p.dim[d + 1];	// number of columns

	for (i = 0; i < n; i++)
		for (j = 0; j < m; j++)
			emit_matrix(p, d + 2, k + (i * m + j) * span);

	emit_update_table(n, m);
}

function
emit_medium_space()
{
	var w;

	if (emit_level == 0)
		w = get_char_width(ROMAN_FONT, LOWER_N);
	else
		w = get_char_width(SMALL_ROMAN_FONT, LOWER_N);

	w *= 0.5;

	push_double(EMIT_SPACE);
	push_double(0.0);
	push_double(0.0);
	push_double(w);

	list(4);
}

function
emit_numerators(p)
{
	var q, t;

	t = stack.length;

	p = cdr(p);
	q = car(p);

	if (isrational(q)) {
		if (Math.abs(q.a) != 1)
			emit_roman_string(Math.abs(q.a).toFixed(0));
		p = cdr(p);
	}

	while (iscons(p)) {

		q = car(p);
		p = cdr(p);

		if (isdenominator(q))
			continue;

		if (stack.length > t)
			emit_medium_space();

		emit_factor(q);
	}

	if (stack.length == t)
		emit_roman_string("1"); // no numerators

	emit_update_list(t);
}

// p is rational or double, sign is not emitted

function
emit_numeric_exponent(p)
{
	var t;

	emit_level++;

	t = stack.length;

	if (isrational(p)) {
		emit_roman_string(Math.abs(p.a).toFixed(0));
		if (p.b != 1) {
			emit_roman_string("/");
			emit_roman_string(p.b.toFixed(0));
		}
	} else
		emit_double(p);

	emit_update_list(t);

	emit_level--;

	emit_update_superscript();
}

function
emit_power(p)
{
	if (cadr(p) == symbol(EXP1)) {
		emit_roman_string("exp");
		emit_args(cdr(p));
		return;
	}

	if (isimaginaryunit(p)) {
		if (isimaginaryunit(get_binding(symbol(SYMBOL_J)))) {
			emit_italic_string("j");
			return;
		}
		if (isimaginaryunit(get_binding(symbol(SYMBOL_I)))) {
			emit_italic_string("i");
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

function
emit_rational(p)
{
	var t;

	if (p.b == 1) {
		emit_roman_string(Math.abs(p.a).toFixed(0));
		return;
	}

	emit_level++;

	t = stack.length;
	emit_roman_string(Math.abs(p.a).toFixed(0));
	emit_update_list(t);

	t = stack.length;
	emit_roman_string(p.b.toFixed(0));
	emit_update_list(t);

	emit_level--;

	emit_update_fraction();
}

// p = y^x where x is a negative number

function
emit_reciprocal(p)
{
	var t;

	emit_roman_string("1"); // numerator

	t = stack.length;

	if (isminusone(caddr(p)))
		emit_expr(cadr(p));
	else {
		emit_base(cadr(p));
		emit_numeric_exponent(caddr(p)); // sign is not emitted
	}

	emit_update_list(t);

	emit_update_fraction();
}

function
emit_roman_char(char_num)
{
	var d, font_num, h, w;

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
}

function
emit_roman_string(s)
{
	var i;
	for (i = 0; i < s.length; i++)
		emit_roman_char(s.charCodeAt(i));
}

function
emit_string(p)
{
	emit_roman_string(p.string);
}

function
emit_subexpr(p)
{
	emit_list(p);
	emit_update_subexpr();
}

function
emit_symbol(p)
{
	var k, s, t;

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

	if (k == s.length)
		return;

	// emit subscript

	emit_level++;

	t = stack.length;

	while (k < s.length)
		k = emit_symbol_fragment(s, k);

	emit_update_list(t);

	emit_level--;

	emit_update_subscript();
}

const symbol_name_tab = [

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
];

const italic_tab = [
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,
	0,
];

function
emit_symbol_fragment(s, k)
{
	var char_num, i, n, t;

	n = symbol_name_tab.length;

	for (i = 0; i < n; i++) {
		t = symbol_name_tab[i];
		if (s.startsWith(t, k))
			break;
	}

	if (i == n) {
		if (isdigit(s.charAt(k)))
			emit_roman_char(s.charCodeAt(k));
		else
			emit_italic_char(s.charCodeAt(k));
		return k + 1;
	}

	char_num = i + 128;

	if (italic_tab[i])
		emit_italic_char(char_num);
	else
		emit_roman_char(char_num);

	return k + t.length;
}

function
emit_tensor(p)
{
	if (p.dim.length % 2 == 1)
		emit_vector(p); // odd rank
	else
		emit_matrix(p, 0, 0); // even rank
}

function
emit_term(p)
{
	if (car(p) == symbol(MULTIPLY))
		emit_term_nib(p);
	else
		emit_factor(p);
}

function
emit_term_nib(p)
{
	if (count_denominators(p) > 0) {
		emit_fraction(p);
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

function
emit_thick_space()
{
	var w;

	if (emit_level == 0)
		w = get_char_width(ROMAN_FONT, LOWER_N);
	else
		w = get_char_width(SMALL_ROMAN_FONT, LOWER_N);

	push_double(EMIT_SPACE);
	push_double(0.0);
	push_double(0.0);
	push_double(w);

	list(4);
}

function
emit_thin_space()
{
	var w;

	if (emit_level == 0)
		w = get_char_width(ROMAN_FONT, LOWER_N);
	else
		w = get_char_width(SMALL_ROMAN_FONT, LOWER_N);

	w *= 0.25;

	push_double(EMIT_SPACE);
	push_double(0.0);
	push_double(0.0);
	push_double(w);

	list(4);
}

function
emit_update_fraction()
{
	var d, font_num, h, m, opcode, p1, p2, v, w;

	p2 = pop(); // denominator
	p1 = pop(); // numerator

	h = height(p1) + depth(p1);
	d = height(p2) + depth(p2);
	w = Math.max(width(p1), width(p2));

	if (emit_level == 0) {
		opcode = EMIT_FRACTION;
		font_num = ROMAN_FONT;
	} else {
		opcode = EMIT_SMALL_FRACTION;
		font_num = SMALL_ROMAN_FONT;
	}

	m = get_operator_height(font_num); // midpoint

	v = 0.75 * m; // extra vertical space

	h += v + m;
	d += v - m;

	w += get_char_width(font_num, LOWER_N) / 2; // make horizontal line a bit wider

	push_double(opcode);
	push_double(h);
	push_double(d);
	push_double(w);
	push(p1);
	push(p2);

	list(6);
}

function
emit_update_list(t)
{
	var d, h, i, p1, w;

	if (stack.length - t == 1)
		return;

	h = 0;
	d = 0;
	w = 0;

	for (i = t; i < stack.length; i++) {
		p1 = stack[i];
		h = Math.max(h, height(p1));
		d = Math.max(d, depth(p1));
		w += width(p1);
	}

	list(stack.length - t);
	p1 = pop();

	push_double(EMIT_LIST);
	push_double(h);
	push_double(d);
	push_double(w);
	push(p1);

	list(5);
}

function
emit_update_subexpr()
{
	var d, font_num, h, opcode, p1, w;

	p1 = pop();

	h = height(p1);
	d = depth(p1);
	w = width(p1);

	if (emit_level == 0) {
		opcode = EMIT_SUBEXPR;
		font_num = ROMAN_FONT;
	} else {
		opcode = EMIT_SMALL_SUBEXPR;
		font_num = SMALL_ROMAN_FONT;
	}

	h = Math.max(h, get_cap_height(font_num));
	d = Math.max(d, get_char_depth(font_num, LEFT_PAREN));

	w += 2 * get_char_width(font_num, LEFT_PAREN);

	push_double(opcode);
	push_double(h);
	push_double(d);
	push_double(w);
	push(p1);

	list(5);
}

function
emit_update_subscript()
{
	var d, dx, dy, font_num, h, p1, w;

	p1 = pop();

	d = depth(p1);
	w = width(p1);

	if (emit_level == 0)
		font_num = ROMAN_FONT;
	else
		font_num = SMALL_ROMAN_FONT;

	h = get_cap_height(font_num);

	dx = 0;
	dy = h / 2;

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

function
emit_update_superscript()
{
	var d, dx, dy, h, p1, p2, w, y;

	p2 = pop(); // exponent
	p1 = pop(); // base

	h = height(p2);
	d = depth(p2);
	w = width(p2);

	// y is height of base

	y = height(p1);

	// adjust

	y -= (h + d) / 2;

	if (emit_level == 0)
		y = Math.max(y, get_xheight(ROMAN_FONT));
	else
		y = Math.max(y, get_xheight(SMALL_ROMAN_FONT));

	dx = 0;
	dy = -(y + d);

	h = y + h + d;
	d = 0;

	if (opcode(p1) == EMIT_SUBSCRIPT) {
		dx = -width(p1);
		w = Math.max(0, w - width(p1));
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

function
emit_update_table(n, m)
{
	var d, h, i, j, p1, p2, p3, p4, t, total_height, total_width, w;

	total_height = 0;
	total_width = 0;

	t = stack.length - n * m;

	// max height for each row

	for (i = 0; i < n; i++) { // for each row
		h = 0;
		for (j = 0; j < m; j++) { // for each column
			p1 = stack[t + i * m + j];
			h = Math.max(h, height(p1));
		}
		push_double(h);
		total_height += h;
	}

	list(n);
	p2 = pop();

	// max depth for each row

	for (i = 0; i < n; i++) { // for each row
		d = 0;
		for (j = 0; j < m; j++) { // for each column
			p1 = stack[t + i * m + j];
			d = Math.max(d, depth(p1));
		}
		push_double(d);
		total_height += d;
	}

	list(n);
	p3 = pop();

	// max width for each column

	for (j = 0; j < m; j++) { // for each column
		w = 0;
		for (i = 0; i < n; i++) { // for each row
			p1 = stack[t + i * m + j];
			w = Math.max(w, width(p1));
		}
		push_double(w);
		total_width += w;
	}

	list(m);
	p4 = pop();

	// padding

	total_height += n * 2 * TABLE_VSPACE;
	total_width += m * 2 * TABLE_HSPACE;

	// h, d, w for entire table

	h = total_height / 2 + get_operator_height(ROMAN_FONT);
	d = total_height - h;
	w = total_width + 2 * get_char_width(ROMAN_FONT, LEFT_PAREN);

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
}

function
emit_vector(p)
{
	var i, n, span;

	// compute element span

	span = 1;

	n = p.dim.length;

	for (i = 1; i < n; i++)
		span *= p.dim[i];

	n = p.dim[0]; // number of rows

	for (i = 0; i < n; i++)
		emit_matrix(p, 1, i * span);

	emit_update_table(n, 1); // n rows, 1 column
}

function
draw_formula(x, y, p)
{
	var char_num, d, dx, dy, font_num, h, k, stroke_width, w;

	k = opcode(p);
	h = height(p);
	d = depth(p);
	w = width(p);

	p = cddddr(p);

	switch (k) {

	case EMIT_SPACE:
		break;

	case EMIT_CHAR:
		font_num = val1(p);
		char_num = val2(p);
		draw_char(x, y, font_num, char_num);
		break;

	case EMIT_LIST:
		p = car(p);
		while (iscons(p)) {
			draw_formula(x, y, car(p));
			x += width(car(p));
			p = cdr(p);
		}
		break;

	case EMIT_SUPERSCRIPT:
	case EMIT_SUBSCRIPT:
		dx = val1(p);
		dy = val2(p);
		p = caddr(p);
		draw_formula(x + dx, y + dy, p);
		break;

	case EMIT_SUBEXPR:
		draw_delims(x, y, h, d, w, MEDIUM_STROKE, ROMAN_FONT);
		dx = get_char_width(ROMAN_FONT, LEFT_PAREN);
		draw_formula(x + dx, y, car(p));
		break;

	case EMIT_SMALL_SUBEXPR:
		draw_delims(x, y, h, d, w, THIN_STROKE, SMALL_ROMAN_FONT);
		dx = get_char_width(SMALL_ROMAN_FONT, LEFT_PAREN);
		draw_formula(x + dx, y, car(p));
		break;

	case EMIT_FRACTION:
	case EMIT_SMALL_FRACTION:

		// horizontal line

		if (k == EMIT_FRACTION) {
			dy = get_operator_height(ROMAN_FONT);
			stroke_width = MEDIUM_STROKE;
		} else {
			dy = get_operator_height(SMALL_ROMAN_FONT);
			stroke_width = THIN_STROKE;
		}

		draw_stroke(x, y - dy, x + w, y - dy, stroke_width);

		// numerator

		dx = (w - width(car(p))) / 2;
		dy = -h + height(car(p));
		draw_formula(x + dx, y + dy, car(p));

		// denominator

		p = cdr(p);
		dx = (w - width(car(p))) / 2;
		dy = d - depth(car(p));
		draw_formula(x + dx, y + dy, car(p));

		break;

	case EMIT_TABLE:
		draw_delims(x, y, h, d, w, THICK_STROKE, ROMAN_FONT);
		dx = get_char_width(ROMAN_FONT, LEFT_PAREN);
		draw_table(x + dx, y - h, p);
		break;
	}
}

const char_str_tab = [

	"&Alpha;",
	"&Beta;",
	"&Gamma;",
	"&Delta;",
	"&Epsilon;",
	"&Zeta;",
	"&Eta;",
	"&Theta;",
	"&Iota;",
	"&Kappa;",
	"&Lambda;",
	"&Mu;",
	"&Nu;",
	"&Xi;",
	"&Omicron;",
	"&Pi;",
	"&Rho;",
	"&Sigma;",
	"&Tau;",
	"&Upsilon;",
	"&Phi;",
	"&Chi;",
	"&Psi;",
	"&Omega;",

	"&alpha;",
	"&beta;",
	"&gamma;",
	"&delta;",
	"&epsilon;",
	"&zeta;",
	"&eta;",
	"&theta;",
	"&iota;",
	"&kappa;",
	"&lambda;",
	"&mu;",
	"&nu;",
	"&xi;",
	"&omicron;",
	"&pi;",
	"&rho;",
	"&sigma;",
	"&tau;",
	"&upsilon;",
	"&phi;",
	"&chi;",
	"&psi;",
	"&omega;",

	"&hbar;",	// 176

	"&plus;",	// 177
	"&minus;",	// 178
	"&times;",	// 179
	"&ge;",		// 180
	"&le;",		// 181
];

function
draw_char(x, y, font_num, char_num)
{
	var s, t;

	if (char_num < 32 || char_num > 181)
		s = "?";
	else if (char_num == 34)
		s = "&quot;";
	else if (char_num == 38)
		s = "&amp;"
	else if (char_num == 60)
		s = "&lt;";
	else if (char_num == 62)
		s = "&gt;";
	else if (char_num < 128)
		s = String.fromCharCode(char_num);
	else
		s = char_str_tab[char_num - 128];

	t = "<text style='font-family:\"Times New Roman\";";

	switch (font_num) {
	case ROMAN_FONT:
		t += "font-size:" + FONT_SIZE + "px;";
		break;
	case ITALIC_FONT:
		t += "font-size:" + FONT_SIZE + "px;font-style:italic;";
		break;
	case SMALL_ROMAN_FONT:
		t += "font-size:" + SMALL_FONT_SIZE + "px;";
		break;
	case SMALL_ITALIC_FONT:
		t += "font-size:" + SMALL_FONT_SIZE + "px;font-style:italic;";
		break;
	}

	x = "x='" + x + "'";
	y = "y='" + y + "'";

	t += "'" + x + y + ">" + s + "</text>\n";

	outbuf += t;
}

function
draw_delims(x, y, h, d, w, stroke_width, font_num)
{
	var cd, ch, cw;

	ch = get_cap_height(font_num);
	cd = get_char_depth(font_num, LEFT_PAREN);
	cw = get_char_width(font_num, LEFT_PAREN);

	if (h > ch || d > cd) {
		draw_left_delim(x, y, h, d, cw, stroke_width);
		draw_right_delim(x + w - cw, y, h, d, cw, stroke_width);
	} else {
		draw_char(x, y, font_num, LEFT_PAREN);
		draw_char(x + w - cw, y, font_num, RIGHT_PAREN);
	}
}

function
draw_left_delim(x, y, h, d, w, stroke_width)
{
	var x1, x2, y1, y2;

	x1 = x + 0.25 * w;
	x2 = x + 0.75 * w;

	y1 = y - h + stroke_width / 2.0;
	y2 = y + d - stroke_width / 2.0;

	draw_stroke(x1, y1, x1, y2, stroke_width); // stem stroke
	draw_stroke(x1, y1, x2, y1, stroke_width); // top stroke
	draw_stroke(x1, y2, x2, y2, stroke_width); // bottom stroke
}

function
draw_right_delim(x, y, h, d, w, stroke_width)
{
	var x1, x2, y1, y2;

	x1 = x + 0.75 * w;
	x2 = x + 0.25 * w;

	y1 = y - h + stroke_width / 2.0;
	y2 = y + d - stroke_width / 2.0;

	draw_stroke(x1, y1, x1, y2, stroke_width); // stem stroke
	draw_stroke(x1, y1, x2, y1, stroke_width); // top stroke
	draw_stroke(x1, y2, x2, y2, stroke_width); // bottom stroke
}

function
draw_stroke(x1, y1, x2, y2, stroke_width)
{
	var s;

	x1 = "x1='" + x1 + "'";
	x2 = "x2='" + x2 + "'";

	y1 = "y1='" + y1 + "'";
	y2 = "y2='" + y2 + "'";

	s = "<line " + x1 + y1 + x2 + y2 + "style='stroke:black;stroke-width:" + stroke_width + "'/>\n"

	outbuf += s;
}

function
draw_table(x, y, p)
{
	var cx, d, dx, h, i, j, m, n, w;
	var column_width, elem_width, row_depth, row_height, table;

	n = val1(p);
	m = val2(p);

	p = cddr(p);

	table = car(p);
	h = cadr(p);
	d = caddr(p);

	for (i = 0; i < n; i++) { // for each row

		row_height = val1(h);
		row_depth = val1(d);

		y += TABLE_VSPACE + row_height;

		dx = 0;

		w = cadddr(p);

		for (j = 0; j < m; j++) { // for each column
			column_width = val1(w);
			elem_width = width(car(table));
			cx = x + dx + TABLE_HSPACE + (column_width - elem_width) / 2; // center horizontal
			draw_formula(cx, y, car(table));
			dx += column_width + 2 * TABLE_HSPACE;
			table = cdr(table);
			w = cdr(w);
		}

		y += row_depth + TABLE_VSPACE;

		h = cdr(h);
		d = cdr(d);
	}
}

function
opcode(p)
{
	return car(p).d;
}

function
height(p)
{
	return cadr(p).d;
}

function
depth(p)
{
	return caddr(p).d;
}

function
width(p)
{
	return cadddr(p).d;
}

function
val1(p)
{
	return car(p).d;
}

function
val2(p)
{
	return cadr(p).d;
}

// printf("%g %g %g\n",
// 2048 * get_cap_height(ROMAN_FONT) / FONT_SIZE,
// 2048 * get_descent(ROMAN_FONT) / FONT_SIZE,
// 2048 * get_xheight(ROMAN_FONT) / FONT_SIZE);
//
// 1356 443 916

const FONT_MAG = 1 / 2048;
const FONT_CAP_HEIGHT = 1356;
const FONT_DESCENT = 443;
const FONT_XHEIGHT = 916;

function
get_cap_height(font_num)
{
	switch (font_num) {
	case ROMAN_FONT:
	case ITALIC_FONT:
		return FONT_MAG * FONT_CAP_HEIGHT * FONT_SIZE;
	case SMALL_ROMAN_FONT:
	case SMALL_ITALIC_FONT:
		return FONT_MAG * FONT_CAP_HEIGHT * SMALL_FONT_SIZE;
	}
}

const roman_descent_tab = [

	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

//	  ! " # $ % & ' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ?
	0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

//	@ A B C D E F G H I J K L M N O P Q R S T U V W X Y Z [   ] ^ _
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,

//	` a b c d e f g h i j k l m n o p q r s t u v w x y z { | } ~
	0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,0,1,1,1,0,0,

	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // upper case greek
	0,1,1,0,0,1,1,0,0,0,0,1,0,1,0,0,1,0,0,0,1,1,1,0, // lower case greek

	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
];

const italic_descent_tab = [

	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

//	  ! " # $ % & ' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ?
	0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

//	@ A B C D E F G H I J K L M N O P Q R S T U V W X Y Z [   ] ^ _
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,

//	` a b c d e f g h i j k l m n o p q r s t u v w x y z { | } ~
	0,0,0,0,0,0,1,1,0,0,1,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,0,1,1,1,0,0,

	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // upper case greek
	0,1,1,0,0,1,1,0,0,0,0,1,0,1,0,0,1,0,0,0,1,1,1,0, // lower case greek

	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
];

function
get_char_depth(font_num, char_num)
{
	switch (font_num) {
	case ROMAN_FONT:
		return FONT_MAG * FONT_DESCENT * FONT_SIZE * roman_descent_tab[char_num];
	case ITALIC_FONT:
		return FONT_MAG * FONT_DESCENT * FONT_SIZE * italic_descent_tab[char_num];
	case SMALL_ROMAN_FONT:
		return FONT_MAG * FONT_DESCENT * SMALL_FONT_SIZE * roman_descent_tab[char_num];
	case SMALL_ITALIC_FONT:
		return FONT_MAG * FONT_DESCENT * SMALL_FONT_SIZE * italic_descent_tab[char_num];
	}
}

const roman_width_tab = [

909,909,909,909,909,909,909,909,
909,909,909,909,909,909,909,909,
909,909,909,909,909,909,909,909,
909,909,909,909,909,909,909,909,

512,682,836,1024,1024,1706,1593,369,		// printable ascii
682,682,1024,1155,512,682,512,569,
1024,1024,1024,1024,1024,1024,1024,1024,
1024,1024,569,569,1155,1155,1155,909,
1886,1479,1366,1366,1479,1251,1139,1479,
1479,682,797,1479,1251,1821,1479,1479,
1139,1479,1366,1139,1251,1479,1479,1933,
1479,1479,1251,682,569,682,961,1024,
682,909,1024,909,1024,909,682,1024,
1024,569,569,1024,569,1593,1024,1024,
1024,1024,682,797,569,1024,1024,1479,
1024,1024,909,983,410,983,1108,1593,

1479,1366,1184,1253,1251,1251,1479,1479,	// upper case greek
682,1479,1485,1821,1479,1317,1479,1479,
1139,1192,1251,1479,1497,1479,1511,1522,

1073,1042,905,965,860,848,1071,981,		// lower case greek
551,1032,993,1098,926,913,1024,1034,
1022,1104,823,1014,1182,909,1282,1348,

1024,1155,1155,1155,1124,1124,1012,909,		// other symbols

909,909,909,909,909,909,909,909,
909,909,909,909,909,909,909,909,
909,909,909,909,909,909,909,909,
909,909,909,909,909,909,909,909,
909,909,909,909,909,909,909,909,
909,909,909,909,909,909,909,909,
909,909,909,909,909,909,909,909,
909,909,909,909,909,909,909,909,
909,909,909,909,909,909,909,909,
];

const italic_width_tab = [

1024,1024,1024,1024,1024,1024,1024,1024,
1024,1024,1024,1024,1024,1024,1024,1024,
1024,1024,1024,1024,1024,1024,1024,1024,
1024,1024,1024,1024,1024,1024,1024,1024,

512,682,860,1024,1024,1706,1593,438,		// printable ascii
682,682,1024,1382,512,682,512,569,
1024,1024,1024,1024,1024,1024,1024,1024,
1024,1024,682,682,1382,1382,1382,1024,
1884,1251,1251,1366,1479,1251,1251,1479,
1479,682,909,1366,1139,1706,1366,1479,
1251,1479,1251,1024,1139,1479,1251,1706,
1251,1139,1139,797,569,797,864,1024,
682,1024,1024,909,1024,909,569,1024,
1024,569,569,909,569,1479,1024,1024,
1024,1024,797,797,569,1024,909,1366,
909,909,797,819,563,819,1108,1593,

1251,1251,1165,1253,1251,1139,1479,1479,	// upper case greek
682,1366,1237,1706,1366,1309,1479,1479,
1251,1217,1139,1139,1559,1251,1440,1481,

1075,1020,807,952,807,829,1016,1006,		// lower case greek
569,983,887,1028,909,877,1024,1026,
983,1010,733,940,1133,901,1272,1446,

1024,1382,1382,1382,1124,1124,1012,1024,	// other symbols

1024,1024,1024,1024,1024,1024,1024,1024,
1024,1024,1024,1024,1024,1024,1024,1024,
1024,1024,1024,1024,1024,1024,1024,1024,
1024,1024,1024,1024,1024,1024,1024,1024,
1024,1024,1024,1024,1024,1024,1024,1024,
1024,1024,1024,1024,1024,1024,1024,1024,
1024,1024,1024,1024,1024,1024,1024,1024,
1024,1024,1024,1024,1024,1024,1024,1024,
1024,1024,1024,1024,1024,1024,1024,1024,
];

// too tight, multiply by 1.1 to loosen up

function
get_char_width(font_num, char_num)
{
	switch (font_num) {
	case ROMAN_FONT:
		return 1.1 * FONT_MAG * FONT_SIZE * roman_width_tab[char_num];
	case ITALIC_FONT:
		return 1.1 * FONT_MAG * FONT_SIZE * italic_width_tab[char_num];
	case SMALL_ROMAN_FONT:
		return 1.1 * FONT_MAG * SMALL_FONT_SIZE * roman_width_tab[char_num];
	case SMALL_ITALIC_FONT:
		return 1.1 * FONT_MAG * SMALL_FONT_SIZE * italic_width_tab[char_num];
	}
}

function
get_xheight(font_num)
{
	switch (font_num) {
	case ROMAN_FONT:
	case ITALIC_FONT:
		return FONT_MAG * FONT_XHEIGHT * FONT_SIZE;
	case SMALL_ROMAN_FONT:
	case SMALL_ITALIC_FONT:
		return FONT_MAG * FONT_XHEIGHT * SMALL_FONT_SIZE;
	}
}

function
get_operator_height(font_num)
{
	return get_cap_height(font_num) / 2;
}

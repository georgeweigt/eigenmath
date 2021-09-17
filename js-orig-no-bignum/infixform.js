function
infixform_subexpr(p)
{
	infixform_write("(");
	infixform_expr(p);
	infixform_write(")");
}

function
infixform_expr(p)
{
	if (isnegativeterm(p) || (car(p) == symbol(ADD) && isnegativeterm(cadr(p))))
		infixform_write("-");
	if (car(p) == symbol(ADD))
		infixform_expr_nib(p);
	else
		infixform_term(p);
}

function
infixform_expr_nib(p)
{
	infixform_term(cadr(p));
	p = cddr(p);
	while (iscons(p)) {
		if (isnegativeterm(car(p)))
			infixform_write(" - ");
		else
			infixform_write(" + ");
		infixform_term(car(p));
		p = cdr(p);
	}
}

function
infixform_term(p)
{
	if (car(p) == symbol(MULTIPLY))
		infixform_term_nib(p);
	else
		infixform_factor(p);
}

function
infixform_term_nib(p)
{
	if (find_denominator(p)) {
		infixform_numerators(p);
		infixform_write(" / ");
		infixform_denominators(p);
		return;
	}

	// no denominators

	p = cdr(p);

	if (isminusone(car(p)))
		p = cdr(p); // sign already emitted

	infixform_factor(car(p));

	p = cdr(p);

	while (iscons(p)) {
		infixform_write(" "); // space in between factors
		infixform_factor(car(p));
		p = cdr(p);
	}
}

function
infixform_numerators(p)
{
	var k, q, s;

	k = 0;

	p = cdr(p);

	while (iscons(p)) {

		q = car(p);
		p = cdr(p);

		if (!isnumerator(q))
			continue;

		if (++k > 1)
			infixform_write(" "); // space in between factors

		if (isrational(q)) {
			s = Math.abs(q.a).toFixed(0);
			infixform_write(s);
			continue;
		}

		infixform_factor(q);
	}

	if (k == 0)
		infixform_write("1");
}

function
infixform_denominators(p)
{
	var k, n, q, s;

	n = count_denominators(p);

	if (n > 1)
		infixform_write("(");

	k = 0;

	p = cdr(p);

	while (iscons(p)) {

		q = car(p);
		p = cdr(p);

		if (!isdenominator(q))
			continue;

		if (++k > 1)
			infixform_write(" "); // space in between factors

		if (isrational(q)) {
			s = q.b.toFixed(0);
			infixform_write(s);
			continue;
		}

		if (isminusone(caddr(q))) {
			q = cadr(q);
			infixform_factor(q);
		} else {
			infixform_base(cadr(q));
			infixform_write("^");
			infixform_numeric_exponent(caddr(q)); // sign is not emitted
		}
	}

	if (n > 1)
		infixform_write(")");
}

function
infixform_factor(p)
{
	if (isrational(p)) {
		infixform_rational(p);
		return;
	}

	if (isdouble(p)) {
		infixform_double(p);
		return;
	}

	if (issymbol(p)) {
		if (p == symbol(EXP1))
			infixform_write("exp(1)");
		else
			infixform_write(printname(p));
		return;
	}

	if (isstring(p)) {
		infixform_write(p.string);
		return;
	}

	if (istensor(p)) {
		infixform_tensor(p);
		return;
	}

	if (car(p) == symbol(ADD) || car(p) == symbol(MULTIPLY)) {
		infixform_subexpr(p);
		return;
	}

	if (car(p) == symbol(POWER)) {
		infixform_power(p);
		return;
	}

	if (car(p) == symbol(FACTORIAL)) {
		infixform_factorial(p);
		return;
	}

	if (car(p) == symbol(INDEX)) {
		infixform_index(p);
		return;
	}

	// use d if for derivative if d not defined

	if (car(p) == symbol(DERIVATIVE) && get_usrfunc(symbol(SYMBOL_D)) == symbol(NIL)) {
		infixform_write("d");
		infixform_arglist(p);
		return;
	}

	if (car(p) == symbol(SETQ)) {
		infixform_expr(cadr(p));
		infixform_write(" = ");
		infixform_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTEQ)) {
		infixform_expr(cadr(p));
		infixform_write(" == ");
		infixform_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTGE)) {
		infixform_expr(cadr(p));
		infixform_write(" >= ");
		infixform_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTGT)) {
		infixform_expr(cadr(p));
		infixform_write(" > ");
		infixform_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTLE)) {
		infixform_expr(cadr(p));
		infixform_write(" <= ");
		infixform_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTLT)) {
		infixform_expr(cadr(p));
		infixform_write(" < ");
		infixform_expr(caddr(p));
		return;
	}

	// other function

	if (iscons(p)) {
		infixform_base(car(p));
		infixform_arglist(p);
		return;
	}

	infixform_write(" ? ");
}

function
infixform_power(p)
{
	if (cadr(p) == symbol(EXP1)) {
		infixform_write("exp(");
		infixform_expr(caddr(p));
		infixform_write(")");
		return;
	}

	if (isimaginaryunit(p)) {
		if (isimaginaryunit(get_binding(symbol(SYMBOL_J)))) {
			infixform_write("j");
			return;
		}
		if (isimaginaryunit(get_binding(symbol(SYMBOL_I)))) {
			infixform_write("i");
			return;
		}
	}

	if (isnegativenumber(caddr(p))) {
		infixform_reciprocal(p);
		return;
	}

	infixform_base(cadr(p));

	infixform_write("^");

	p = caddr(p); // p now points to exponent

	if (isnum(p))
		infixform_numeric_exponent(p);
	else if (car(p) == symbol(ADD) || car(p) == symbol(MULTIPLY) || car(p) == symbol(POWER) || car(p) == symbol(FACTORIAL))
		infixform_subexpr(p);
	else
		infixform_expr(p);
}

// p = y^x where x is a negative number

function
infixform_reciprocal(p)
{
	infixform_write("1 / "); // numerator
	if (isminusone(caddr(p))) {
		p = cadr(p);
		infixform_factor(p);
	} else {
		infixform_base(cadr(p));
		infixform_write("^");
		infixform_numeric_exponent(caddr(p)); // sign is not emitted
	}
}

function
infixform_factorial(p)
{
	infixform_base(cadr(p));
	infixform_write("!");
}

function
infixform_index(p)
{
	infixform_base(cadr(p));
	infixform_write("[");
	p = cddr(p);
	if (iscons(p)) {
		infixform_expr(car(p));
		p = cdr(p);
		while (iscons(p)) {
			infixform_write(",");
			infixform_expr(car(p));
			p = cdr(p);
		}
	}
	infixform_write("]");
}

function
infixform_arglist(p)
{
	infixform_write("(");
	p = cdr(p);
	if (iscons(p)) {
		infixform_expr(car(p));
		p = cdr(p);
		while (iscons(p)) {
			infixform_write(",");
			infixform_expr(car(p));
			p = cdr(p);
		}
	}
	infixform_write(")");
}

// sign is not emitted

function
infixform_rational(p)
{
	var s;

	s = Math.abs(p.a).toFixed(0);
	infixform_write(s);

	if (p.b == 1)
		return;

	infixform_write("/");

	s = p.b.toFixed(0);
	infixform_write(s);
}

// sign is not emitted

function
infixform_double(p)
{
	var i, j, k, s;

	s = Math.abs(p.d).toPrecision(6);

	k = 0;

	while (k < s.length && s.charAt(k) != "." && s.charAt(k) != "E" && s.charAt(k) != "e")
		k++;

	infixform_write(s.substring(0, k));

	// handle trailing zeroes

	if (s.charAt(k) == ".") {

		i = k++;

		while (k < s.length && s.charAt(k) != "E" && s.charAt(k) != "e")
			k++;

		j = k;

		while (s.charAt(j - 1) == "0")
			j--;

		if (j - i > 1)
			infixform_write(s.substring(i, j));
	}

	if (s.charAt(k) != "E" && s.charAt(k) != "e")
		return;

	k++;

	infixform_write(" 10^");

	if (s.charAt(k) == "-") {
		infixform_write("(-");
		k++;
		while (s.charAt(k) == "0") // skip leading zeroes
			k++;
		infixform_write(s.substring(k));
		infixform_write(")");
	} else {
		if (s.charAt(k) == "+")
			k++;
		while (s.charAt(k) == "0") // skip leading zeroes
			k++;
		infixform_write(s.substring(k));
	}
}

function
infixform_base(p)
{
	if (isnum(p))
		infixform_numeric_token(p);
	else if (car(p) == symbol(ADD) || car(p) == symbol(MULTIPLY) || car(p) == symbol(POWER) || car(p) == symbol(FACTORIAL))
		infixform_subexpr(p);
	else
		infixform_expr(p);
}

// p is rational or double

function
infixform_numeric_token(p)
{
	var s;

	if (isdouble(p)) {
		s = p.d.toPrecision(6);
		if (p.d >= 0 && s.indexOf("E") == -1 && s.indexOf("e") == -1)
			infixform_double(p);
		else
			infixform_subexpr(p);
		return;
	}

	if (p.a >= 0 && isinteger(p))
		infixform_rational(p);
	else
		infixform_subexpr(p);
}

// p is rational or double, sign is not emitted

function
infixform_numeric_exponent(p)
{
	var s;

	if (isdouble(p)) {
		s = Math.abs(p.d).toPrecision(6);
		if (s.indexOf("E") == -1 && s.indexOf("e") == -1)
			infixform_double(p);
		else {
			infixform_write("(");
			infixform_double(p);
			infixform_write(")");
		}
		return;
	}

	if (isinteger(p))
		infixform_rational(p);
	else {
		infixform_write("(");
		infixform_rational(p);
		infixform_write(")");
	}
}

function
infixform_tensor(p)
{
	infixform_tensor_nib(p, 0, 0);
}

function
infixform_tensor_nib(p, d, k)
{
	var i, n, span;

	if (d == p.dim.length) {
		infixform_expr(p.elem[k]);
		return;
	}

	span = 1;

	n = p.dim.length;

	for (i = d + 1; i < n; i++)
		span *= p.dim[i];

	infixform_write("(");

	n = p.dim[d];

	for (i = 0; i < n; i++) {

		infixform_tensor_nib(p, d + 1, k);

		if (i < n - 1)
			infixform_write(",");

		k += span;
	}

	infixform_write(")");
}

function
infixform_write(s)
{
	outbuf += s;
}

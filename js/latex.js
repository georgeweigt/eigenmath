var latex_buf;

function
latex()
{
	var p1 = pop();
	latex_buf = "";
	latex_puts("<p>$$");
	latex_expr(p1);
	latex_puts("$$</p>");
	fputs(latex_buf);
}

function
latex_expr(p)
{
	var q;

	if (car(p) == symbol(ADD)) {
		p = cdr(p);
		q = car(p);
		if (isnegativenumber(q) || (car(q) == symbol(MULTIPLY) && isnegativenumber(cadr(q))))
			latex_puts("-");
		latex_term(q);
		p = cdr(p);
		while (iscons(p)) {
			q = car(p);
			if (isnegativenumber(q) || (car(q) == symbol(MULTIPLY) && isnegativenumber(cadr(q))))
				latex_puts("-");
			else
				latex_puts("+");
			latex_term(q);
			p = cdr(p);
		}
	} else {
		if (isnegativenumber(p) || (car(p) == symbol(MULTIPLY) && isnegativenumber(cadr(p))))
			latex_puts("-");
		latex_term(p);
	}
}

function
latex_term(p)
{
	var n = 0, q, t;

	if (car(p) == symbol(MULTIPLY)) {

		// any denominators?

		t = cdr(p);

		while (iscons(t)) {
			q = car(t);
			if (car(q) == symbol(POWER) && isnegativenumber(caddr(q)))
				break;
			t = cdr(t);
		}

		if (iscons(t)) {
			latex_puts("\\frac{");
			latex_numerators(p);
			latex_puts("}{");
			latex_denominators(p);
			latex_puts("}");
			return;
		}

		// no denominators

		p = cdr(p);
		q = car(p);

		if (isrational(q) && isminusone(q))
			p = cdr(p); // skip -1

		while (iscons(p)) {
			if (++n > 1)
				latex_puts("\\,"); // thin space between factors
			latex_factor(car(p));
			p = cdr(p);
		}
	} else
		latex_factor(p);
}

function
latex_numerators(p)
{
	var n = 0, q;

	p = cdr(p);
	q = car(p);

	if (isrational(q)) {
		if (Math.abs(q.a) != 1) {
			latex_puts(Math.abs(q.a).toFixed(0));
			n++;
		}
		p = cdr(p);
	}

	while (iscons(p)) {

		q = car(p);

		if (car(q) == symbol(POWER) && isnegativenumber(caddr(q))) {
			p = cdr(p);
			continue; // printed in denominator
		}

		if (n)
			latex_puts("\\,"); // thin space between factors

		latex_factor(q);
		n++;
		p = cdr(p);
	}

	if (n == 0)
		latex_puts("1"); // there were no numerators
}

function
latex_denominators(p)
{
	var n = 0, q;

	p = cdr(p);
	q = car(p);

	if (isrational(q)) {
		if (q.b != 1) {
			latex_puts(q.b.toFixed(0));
			n++;
		}
		p = cdr(p);
	}

	while (iscons(p)) {

		q = car(p);

		if (car(q) != symbol(POWER) || !isnegativenumber(caddr(q))) {
			p = cdr(p);
			continue; // not a denominator
		}

		if (n)
			latex_puts("\\,"); // thin space between factors

		// example (-1)^(-1/4)

		if (isminusone(cadr(q))) {
			latex_puts("(-1)^{");
			latex_number(caddr(q)); // -1/4 (sign not printed)
			latex_puts("}");
			n++;
			p = cdr(p);
			continue;
		}

		// example 1/y

		if (isminusone(caddr(q))) {
			latex_factor(cadr(q)); // y
			n++;
			p = cdr(p);
			continue;
		}

		// example 1/y^2

		latex_base(cadr(q)); // y
		latex_puts("^{");
		latex_number(caddr(q)); // -2 (sign not printed)
		latex_puts("}");

		n++;
		p = cdr(p);
	}

	if (n == 0)
		latex_puts("1"); // there were no denominators
}

function
latex_factor(p)
{
	if (isrational(p)) {
		latex_rational(p);
		return;
	}

	if (isdouble(p)) {
		latex_double(p);
		return;
	}

	if (issymbol(p)) {
		latex_symbol(p);
		return;
	}

	if (isstring(p)) {
		latex_string(p);
		return;
	}

	if (istensor(p)) {
		latex_tensor(p);
		return;
	}

	if (iscons(p)) {
		if (car(p) == symbol(POWER))
			latex_power(p);
		else if (car(p) == symbol(ADD) || car(p) == symbol(MULTIPLY))
			latex_subexpr(p);
		else
			latex_function(p);
		return;
	}
}

function
latex_number(p)
{
	if (isrational(p))
		latex_rational(p);
	else
		latex_double(p);
}

function
latex_rational(p)
{
	if (p.b == 1) {
		latex_puts(Math.abs(p.a).toFixed(0));
		return;
	}

	latex_puts("\\frac{");

	latex_puts(Math.abs(p.a).toFixed(0));

	latex_puts("}{");

	latex_puts(p.b.toFixed(0));

	latex_puts("}");
}

function
latex_double(p)
{
	var i, j, k, s;

	if (p.d == 0) {
		latex_puts("0");
		return;
	}

	s = Math.abs(p.d).toPrecision(6) + 'T'; // terminator

	k = 0;

	while (isdigit(s[k]) || s[k] == '.')
		k++;

	// handle trailing zeroes

	i = s.indexOf(".");

	if (i == -1)
		latex_puts(s.substring(0, k));
	else {
		for (j = k - 1; j > i + 1; j--) {
			if (s[j] != '0')
				break;
		}
		latex_puts(s.substring(0, j + 1));
	}

	if (s[k] != 'E' && s[k] != 'e')
		return;

	k++;

	latex_puts("\\times10^{");

	if (s[k] == '+')
		k++;
	else if (s[k] == '-') {
		k++;
		latex_puts("-");
	}

	while (s[k] == '0')
		k++; // skip leading zeroes

	latex_puts(s.substring(k, s.length - 1));

	latex_puts("}");
}

function
latex_power(p)
{
	// case (-1)^x

	if (isminusone(cadr(p))) {
		latex_imaginary(p);
		return;
	}

	// case e^x

	if (cadr(p) == symbol(EXP1)) {
		latex_puts("\\operatorname{exp}\\left(");
		latex_expr(caddr(p)); // x
		latex_puts("\\right)");
		return;
	}

	// example 1/y

	if (isminusone(caddr(p))) {
		latex_puts("\\frac{1}{");
		latex_expr(cadr(p)); // y
		latex_puts("}");
		return;
	}

	// example 1/y^2

	if (isnegativenumber(caddr(p))) {
		latex_puts("\\frac{1}{");
		latex_base(cadr(p)); // y
		latex_puts("^{");
		latex_number(caddr(p)); // -2 (sign not printed)
		latex_puts("}}");
		return;
	}

	// example y^x

	latex_base(cadr(p)); // y
	latex_puts("^{");
	latex_exponent(caddr(p)); // x
	latex_puts("}");
}

function
latex_base(p)
{
	if (isfraction(p) || isdouble(p) || car(p) == symbol(POWER))
		latex_subexpr(p);
	else
		latex_factor(p);
}

function
latex_exponent(p)
{
	if (car(p) == symbol(POWER))
		latex_subexpr(p);
	else
		latex_factor(p);
}

// case (-1)^x

function
latex_imaginary(p)
{
	if (isimaginaryunit(p)) {
		if (isimaginaryunit(binding[SYMBOL_J])) {
			latex_puts("j");
			return;
		}
		if (isimaginaryunit(binding[SYMBOL_I])) {
			latex_puts("i");
			return;
		}
	}

	// example (-1)^(-1/4)

	if (isnegativenumber(caddr(p))) {
		latex_puts("\\frac{1}{(-1)^{");
		latex_number(caddr(p)); // -1/4 (sign not printed)
		latex_puts("}}");
		return;
	}

	// example (-1)^x

	latex_puts("(-1)^{");
	latex_expr(caddr(p)); // x
	latex_puts("}");
}

function
latex_function(p)
{
	// d(f(x),x)

	if (car(p) == symbol(DERIVATIVE)) {
		latex_puts("\\operatorname{d}\\left(");
		latex_arglist(p);
		latex_puts("\\right)");
		return;
	}

	// n!

	if (car(p) == symbol(FACTORIAL)) {
		p = cadr(p);
		if (isposint(p) || issymbol(p))
			latex_expr(p);
		else
			latex_subexpr(p);
		latex_puts("!");
		return;
	}

	// A[1,2]

	if (car(p) == symbol(INDEX)) {
		p = cdr(p);
		if (issymbol(car(p)))
			latex_symbol(car(p));
		else
			latex_subexpr(car(p));
		latex_puts("\\left[");
		latex_arglist(p);
		latex_puts("\\right]");
		return;
	}

	if (car(p) == symbol(SETQ)) {
		latex_expr(cadr(p));
		latex_puts("=");
		latex_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTEQ)) {
		latex_expr(cadr(p));
		latex_puts("=");
		latex_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTGE)) {
		latex_expr(cadr(p));
		latex_puts("\\geq ");
		latex_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTGT)) {
		latex_expr(cadr(p));
		latex_puts(" > ");
		latex_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTLE)) {
		latex_expr(cadr(p));
		latex_puts("\\leq ");
		latex_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTLT)) {
		latex_expr(cadr(p));
		latex_puts(" < ");
		latex_expr(caddr(p));
		return;
	}

	// default

	if (issymbol(car(p)))
		latex_symbol(car(p));
	else
		latex_subexpr(car(p));
	latex_puts("\\left(");
	latex_arglist(p);
	latex_puts("\\right)");
}

function
latex_arglist(p)
{
	p = cdr(p);
	if (iscons(p)) {
		latex_expr(car(p));
		p = cdr(p);
		while (iscons(p)) {
			latex_puts(",");
			latex_expr(car(p));
			p = cdr(p);
		}
	}
}

function
latex_subexpr(p)
{
	latex_puts("\\left(");
	latex_expr(p);
	latex_puts("\\right)");
}

function
latex_symbol(p)
{
	var k, n, s;

	if (iskeyword(p) || p == symbol(LAST) || p == symbol(TRACE)) {
		latex_puts("\\operatorname{");
		latex_puts(printname(p));
		latex_puts("}");
		return;
	}

	if (p == symbol(EXP1)) {
		latex_puts("\\operatorname{exp}(1)");
		return;
	}

	s = printname(p);
	n = latex_symbol_scan(s, 0);

	if (n == s.length) {
		latex_symbol_shipout(s);
		return;
	}

	// symbol has subscript

	latex_symbol_shipout(s.substring(0, n));

	k = n;

	latex_puts("_{");

	while (k < s.length) {
		n = latex_symbol_scan(s, k);
		latex_symbol_shipout(s.substring(k, k + n));
		k += n;
	}

	latex_puts("}");
}

var latex_greek_tab = [
	"Alpha","Beta","Gamma","Delta","Epsilon","Zeta","Eta","Theta","Iota",
	"Kappa","Lambda","Mu","Nu","Xi","Pi","Rho","Sigma","Tau","Upsilon",
	"Phi","Chi","Psi","Omega",
	"alpha","beta","gamma","delta","epsilon","zeta","eta","theta","iota",
	"kappa","lambda","mu","nu","xi","pi","rho","sigma","tau","upsilon",
	"phi","chi","psi","omega",
];

function
latex_symbol_scan(s, k)
{
	var i, m, n, t;

	n = latex_greek_tab.length;

	for (i = 0; i < n; i++) {

		t = latex_greek_tab[i];

		m = t.length;

		if (s.substring(k, k + m) == t)
			return m;
	}

	return 1;
}

function
latex_symbol_shipout(s)
{
	if (s.length == 1)
		latex_puts(s);
	else
		latex_puts("\\" + s + " "); // greek letter
}

function
latex_tensor(p)
{
	var i, k, n;

	// if odd rank then vector

	if (p.dim.length % 2 == 1) {
		latex_puts("\\begin{pmatrix}");
		k = 0;
		n = p.dim[0];
		for (i = 0; i < n; i++) {
			k = latex_matrix(p, 1, k);
			if (i < n - 1)
				latex_puts("\\cr "); // row separator
		}
		latex_puts("\\end{pmatrix}");
	} else
		latex_matrix(p, 0, 0);
}

function
latex_matrix(p, d, k)
{
	var i, j, m, n;

	if (d == p.dim.length) {
		latex_expr(p.elem[k]);
		return k + 1;
	}

	n = p.dim[d];
	m = p.dim[d + 1];

	latex_puts("\\begin{pmatrix}");

	for (i = 0; i < n; i++) {
		for (j = 0; j < m; j++) {
			k = latex_matrix(p, d + 2, k);
			if (j < m - 1)
				latex_puts(" & "); // column separator
		}
		if (i < n - 1)
			latex_puts("\\cr "); // row separator
	}

	latex_puts("\\end{pmatrix}");

	return k;
}

function
latex_string(p)
{
	latex_puts("\\text{" + p.string + "}");
}

function
latex_puts(s)
{
	latex_buf += s;
}

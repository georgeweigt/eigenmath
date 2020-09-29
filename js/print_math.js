const MML_MINUS = "<mo>-</mo>"
const MML_MINUS_1 = "<mo>(</mo><mo>-</mo><mn>1</mn><mo>)</mo>"
const MML_LP = "<mrow><mo>(</mo>"
const MML_RP = "<mo>)</mo></mrow>"
const MML_LB = "<mrow><mo>[</mo>"
const MML_RB = "<mo>]</mo></mrow>"

var mml_buf;

function
print_math()
{
	var p1 = pop();
	mml_buf = "<p><math>";
	mml_expr(p1);
	mml_puts("</math></p>");
	fputs(mml_buf);
}

function
mml_expr(p)
{
	var q;

	if (car(p) == symbol(ADD)) {
		p = cdr(p);
		q = car(p);
		if (isnegativenumber(q) || (car(q) == symbol(MULTIPLY) && isnegativenumber(cadr(q))))
			mml_puts(MML_MINUS);
		mml_term(q);
		p = cdr(p);
		while (iscons(p)) {
			q = car(p);
			if (isnegativenumber(q) || (car(q) == symbol(MULTIPLY) && isnegativenumber(cadr(q))))
				mml_mo("-");
			else
				mml_mo("+");
			mml_term(q);
			p = cdr(p);
		}
	} else {
		if (isnegativenumber(p) || (car(p) == symbol(MULTIPLY) && isnegativenumber(cadr(p))))
			mml_puts(MML_MINUS);
		mml_term(p);
	}
}

function
mml_term(p)
{
	var q, t;

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
			mml_puts("<mfrac>");
			mml_puts("<mrow>");
			mml_numerators(p);
			mml_puts("</mrow>");
			mml_puts("<mrow>");
			mml_denominators(p);
			mml_puts("</mrow>");
			mml_puts("</mfrac>");
			return;
		}

		// no denominators

		p = cdr(p);
		q = car(p);

		if (isrational(q) && isminusone(q))
			p = cdr(p); // skip -1

		mml_factor(car(p));
		p = cdr(p);

		while (iscons(p)) {
			mml_puts("<mtext>&nbsp;</mtext>");
			mml_factor(car(p));
			p = cdr(p);
		}
	} else
		mml_factor(p);
}

function
mml_numerators(p)
{
	var n, q;

	n = 0;

	p = cdr(p);
	q = car(p);

	if (isrational(q)) {
		if (Math.abs(q.a) != 1) {
			mml_mn(Math.abs(q.a).toFixed(0));
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

		if (n > 0)
			mml_puts("<mtext>&nbsp;</mtext>");

		mml_factor(q);
		n++;
		p = cdr(p);
	}

	if (n == 0)
		mml_mn("1"); // there were no numerators
}

function
mml_denominators(p)
{
	var n, q;

	n = 0;

	p = cdr(p);
	q = car(p);

	if (isrational(q)) {
		if (q.b != 1) {
			mml_mn(q.b.toFixed(0));
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

		if (n > 0)
			mml_puts("<mtext>&nbsp;</mtext>");

		// example (-1)^(-1/4)

		if (isminusone(cadr(q))) {
			mml_puts("<msup>");
			mml_puts("<mrow>");
			mml_puts(MML_MINUS_1); // (-1)
			mml_puts("</mrow>");
			mml_puts("<mrow>");
			mml_number(caddr(q)); // -1/4 (sign not printed)
			mml_puts("</mrow>");
			mml_puts("</msup>");
			n++;
			p = cdr(p);
			continue;
		}

		// example 1/y

		if (isminusone(caddr(q))) {
			mml_factor(cadr(q)); // y
			n++;
			p = cdr(p);
			continue;
		}

		// example 1/y^2

		mml_puts("<msup>");
		mml_puts("<mrow>");
		mml_base(cadr(q)); // y
		mml_puts("</mrow>");
		mml_puts("<mrow>");
		mml_number(caddr(q)); // -2 (sign not printed)
		mml_puts("</mrow>");
		mml_puts("</msup>");

		n++;
		p = cdr(p);
	}

	if (n == 0)
		mml_mn("1"); // there were no denominators
}

function
mml_factor(p)
{
	if (isrational(p)) {
		mml_rational(p);
		return;
	}

	if (isdouble(p)) {
		mml_double(p);
		return;
	}

	if (issymbol(p)) {
		mml_symbol(p);
		return;
	}

	if (isstring(p)) {
		mml_string(p);
		return;
	}

	if (istensor(p)) {
		mml_tensor(p);
		return;
	}

	if (iscons(p)) {
		if (car(p) == symbol(POWER))
			mml_power(p);
		else if (car(p) == symbol(ADD) || car(p) == symbol(MULTIPLY))
			mml_subexpr(p);
		else
			mml_function(p);
		return;
	}
}

function
mml_number(p)
{
	if (isrational(p))
		mml_rational(p);
	else
		mml_double(p);
}

function
mml_rational(p)
{
	if (p.b == 1) {
		mml_mn(Math.abs(p.a).toFixed(0));
		return;
	}

	mml_puts("<mfrac>");
	mml_mn(Math.abs(p.a).toFixed(0));
	mml_mn(p.b.toFixed(0));
	mml_puts("</mfrac>");
}

function
mml_double(p)
{
	var i, j, k, s;

	if (p.d == 0) {
		mml_mn("0");
		return;
	}

	s = Math.abs(p.d).toPrecision(6) + 'T'; // terminator

	k = 0;

	while (isdigit(s[k]) || s[k] == '.')
		k++;

	// handle trailing zeroes

	i = s.indexOf(".");

	if (i == -1)
		mml_mn(s.substring(0, k));
	else {
		for (j = k - 1; j > i + 1; j--) {
			if (s[j] != '0')
				break;
		}
		mml_mn(s.substring(0, j + 1));
	}

	if (s[k] != 'E' && s[k] != 'e')
		return;

	k++;

	mml_mo("&times;");

	mml_puts("<msup>");
	mml_mn("10");
	mml_puts("<mrow>");

	if (s[k] == '+')
		k++;
	else if (s[k] == '-') {
		k++;
		mml_puts(MML_MINUS);
	}

	while (s[k] == '0')
		k++; // skip leading zeroes

	mml_mn(s.substring(k, s.length - 1));

	mml_puts("</mrow>");
	mml_puts("</msup>");
}

function
mml_power(p)
{
	// (-1)^x

	if (isminusone(cadr(p))) {
		mml_imaginary(p);
		return;
	}

	// e^x

	if (cadr(p) == symbol(EXP1)) {
		mml_mi("exp");
		mml_puts(MML_LP);
		mml_expr(caddr(p)); // x
		mml_puts(MML_RP);
		return;
	}

	// example 1/y

	if (isminusone(caddr(p))) {
		mml_puts("<mfrac>");
		mml_mn("1"); // 1
		mml_puts("<mrow>");
		mml_expr(cadr(p)); // y
		mml_puts("</mrow>");
		mml_puts("</mfrac>");
		return;
	}

	// example 1/y^2

	if (isnegativenumber(caddr(p))) {
		mml_puts("<mfrac>");
		mml_mn("1"); // 1
		mml_puts("<msup>");
		mml_puts("<mrow>");
		mml_base(cadr(p)); // y
		mml_puts("</mrow>");
		mml_puts("<mrow>");
		mml_number(caddr(p)); // -2 (sign not printed)
		mml_puts("</mrow>");
		mml_puts("</msup>");
		mml_puts("</mfrac>");
		return;
	}

	// example y^x

	mml_puts("<msup>");
	mml_puts("<mrow>");
	mml_base(cadr(p)); // y
	mml_puts("</mrow>");
	mml_puts("<mrow>");
	mml_exponent(caddr(p)); // x
	mml_puts("</mrow>");
	mml_puts("</msup>");
}

function
mml_base(p)
{
	if (isfraction(p) || isdouble(p) || car(p) == symbol(POWER))
		mml_subexpr(p);
	else
		mml_factor(p);
}

function
mml_exponent(p)
{
	if (car(p) == symbol(POWER))
		mml_subexpr(p);
	else
		mml_factor(p);
}

// case (-1)^x

function
mml_imaginary(p)
{
	if (isimaginaryunit(p)) {
		if (isimaginaryunit(get_binding(symbol(SYMBOL_J)))) {
			mml_mi("j");
			return;
		}
		if (isimaginaryunit(get_binding(symbol(SYMBOL_I)))) {
			mml_mi("i");
			return;
		}
	}

	// example (-1)^(-1/4)

	if (isnegativenumber(caddr(p))) {
		mml_puts("<mfrac>");
		mml_mn("1");
		mml_puts("<msup>");
		mml_puts("<mrow>");
		mml_puts(MML_MINUS_1); // (-1)
		mml_puts("</mrow>");
		mml_puts("<mrow>");
		mml_number(caddr(p)); // -1/4 (sign not printed)
		mml_puts("</mrow>");
		mml_puts("</msup>");
		mml_puts("</mfrac>");
		return;
	}

	// example (-1)^x

	mml_puts("<msup>");
	mml_puts("<mrow>");
	mml_puts(MML_MINUS_1); // (-1)
	mml_puts("</mrow>");
	mml_puts("<mrow>");
	mml_expr(caddr(p)); // x
	mml_puts("</mrow>");
	mml_puts("</msup>");
}

function
mml_function(p)
{
	// d(f(x),x)

	if (car(p) == symbol(DERIVATIVE)) {
		mml_puts("<mi mathvariant='normal'>d</mi>");
		mml_puts(MML_LP);
		mml_arglist(p);
		mml_puts(MML_RP);
		return;
	}

	// n!

	if (car(p) == symbol(FACTORIAL)) {
		p = cadr(p);
		if (isposint(p) || issymbol(p))
			mml_expr(p);
		else
			mml_subexpr(p);
		mml_mo("!");
		return;
	}

	// A[1,2]

	if (car(p) == symbol(INDEX)) {
		p = cdr(p);
		if (issymbol(car(p)))
			mml_symbol(car(p));
		else
			mml_subexpr(car(p));
		mml_puts(MML_LB);
		mml_arglist(p);
		mml_puts(MML_RB);
		return;
	}

	if (car(p) == symbol(SETQ)) {
		mml_expr(cadr(p));
		mml_mo("=");
		mml_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTEQ)) {
		mml_expr(cadr(p));
		mml_mo("=");
		mml_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTGE)) {
		mml_expr(cadr(p));
		mml_mo("&ge;");
		mml_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTGT)) {
		mml_expr(cadr(p));
		mml_mo("&gt;");
		mml_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTLE)) {
		mml_expr(cadr(p));
		mml_mo("&le;");
		mml_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTLT)) {
		mml_expr(cadr(p));
		mml_mo("&lt");
		mml_expr(caddr(p));
		return;
	}

	// default

	if (issymbol(car(p)))
		mml_symbol(car(p));
	else
		mml_subexpr(car(p));

	mml_puts(MML_LP);
	mml_arglist(p);
	mml_puts(MML_RP);
}

function
mml_arglist(p)
{
	p = cdr(p);
	if (iscons(p)) {
		mml_expr(car(p));
		p = cdr(p);
		while(iscons(p)) {
			mml_mo(",");
			mml_expr(car(p));
			p = cdr(p);
		}
	}
}

function
mml_subexpr(p)
{
	mml_puts(MML_LP);
	mml_expr(p);
	mml_puts(MML_RP);
}

function
mml_symbol(p)
{
	var k, n, s;

	if (iskeyword(p) || p == symbol(LAST) || p == symbol(TRACE)) {
		mml_mi(printname(p));
		return;
	}

	if (p == symbol(EXP1)) {
		mml_mi("exp");
		mml_puts(MML_LP);
		mml_mn("1");
		mml_puts(MML_RP);
		return;
	}

	s = printname(p);

	n = mml_symbol_scan(s, 0);

	if (n == s.length) {
		mml_symbol_shipout(s);
		return;
	}

	// print symbol with subscript

	mml_puts("<msub>");

	mml_symbol_shipout(s.substring(0, n));

	mml_puts("<mrow>");

	k = n;

	while (k < s.length) {
		n = mml_symbol_scan(s, k);
		mml_symbol_shipout(s.substring(k, k + n));
		k += n;
	}

	mml_puts("</mrow>");
	mml_puts("</msub>");
}

var mml_greek_tab = [
	"Alpha","Beta","Gamma","Delta","Epsilon","Zeta","Eta","Theta","Iota",
	"Kappa","Lambda","Mu","Nu","Xi","Pi","Rho","Sigma","Tau","Upsilon",
	"Phi","Chi","Psi","Omega",
	"alpha","beta","gamma","delta","epsilon","zeta","eta","theta","iota",
	"kappa","lambda","mu","nu","xi","pi","rho","sigma","tau","upsilon",
	"phi","chi","psi","omega",
];

function
mml_symbol_scan(s, k)
{
	var i, m, n, t;

	n = mml_greek_tab.length;

	for (i = 0; i < n; i++) {

		t = mml_greek_tab[i];

		m = t.length;

		if (s.substring(k, k + m) == t)
			return m;
	}

	return 1;
}

function
mml_symbol_shipout(s)
{
	if (s.length == 1) {
		mml_puts("<mi>");
		mml_puts(s);
		mml_puts("</mi>");
		return;
	}

	// greek

	if (s[0] >= 'A' && s[0] <= 'Z') {
		mml_puts("<mi mathvariant='normal'>&"); // upper case
		mml_puts(s);
		mml_puts(";</mi>");
	} else {
		mml_puts("<mi>&");
		mml_puts(s);
		mml_puts(";</mi>");
	}
}

function
mml_tensor(p)
{
	var i, k, n;

	k = 0;

	// if odd rank then vector

	if (p.dim.length % 2 == 1) {
		mml_puts(MML_LP);
		mml_puts("<mtable>");
		n = p.dim[0];
		for (i = 0; i < n; i++) {
			mml_puts("<mtr><mtd>");
			k = mml_matrix(p, 1, k);
			mml_puts("</mtd></mtr>");
		}
		mml_puts("</mtable>");
		mml_puts(MML_RP);
	} else
		mml_matrix(p, 0, k);
}

function
mml_matrix(p, d, k)
{
	var i, j, m, n;

	if (d == p.dim.length) {
		mml_expr(p.elem[k]);
		return k + 1;
	}

	n = p.dim[d];
	m = p.dim[d + 1];

	mml_puts(MML_LP);
	mml_puts("<mtable>");

	for (i = 0; i < n; i++) {
		mml_puts("<mtr>");
		for (j = 0; j < m; j++) {
			mml_puts("<mtd>");
			k = mml_matrix(p, d + 2, k);
			mml_puts("</mtd>");
		}
		mml_puts("</mtr>");
	}

	mml_puts("</mtable>");
	mml_puts(MML_RP);

	return k;
}

function
mml_string(p)
{
	var s = p.string;

	mml_puts("<mtext>");

	s = s.replace(/&/g, "&amp;");
	s = s.replace(/</g, "&lt;");
	s = s.replace(/>/g, "&gt;");
	s = s.replace(/\n/g, "<br>");

	mml_puts(s);

	mml_puts("</mtext>");
}

function
mml_mi(s)
{
	mml_puts("<mi>");
	mml_puts(s);
	mml_puts("</mi>");
}

function
mml_mn(s)
{
	mml_puts("<mn>");
	mml_puts(s);
	mml_puts("</mn>");
}

function
mml_mo(s)
{
	mml_puts("<mo>");
	mml_puts(s);
	mml_puts("</mo>");
}

function
mml_puts(s)
{
	mml_buf += s;
}

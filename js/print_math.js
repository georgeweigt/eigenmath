const MML_MINUS = "<mo>-</mo>"
const MML_MINUS_1 = "<mo>(</mo><mo>-</mo><mn>1</mn><mo>)</mo>"
const MML_LP = "<mrow><mo>(</mo>"
const MML_RP = "<mo>)</mo></mrow>"
const MML_LB = "<mrow><mo>[</mo>"
const MML_RB = "<mo>]</mo></mrow>"

function
print_math()
{
	var p1 = pop();
	outbuf = "";
	append("<p><math>");
	print_expr(p1);
	append("</math></p>");
	fputs(outbuf);
}

function
print_expr(p)
{
	var q;

	if (car(p) == symbol(ADD)) {
		p = cdr(p);
		q = car(p);
		if (isnegativenumber(q) || (car(q) == symbol(MULTIPLY) && isnegativenumber(cadr(q))))
			append(MML_MINUS);
		print_term(q);
		p = cdr(p);
		while (iscons(p)) {
			q = car(p);
			if (isnegativenumber(q) || (car(q) == symbol(MULTIPLY) && isnegativenumber(cadr(q))))
				print_mo("-");
			else
				print_mo("+");
			print_term(q);
			p = cdr(p);
		}
	} else {
		if (isnegativenumber(p) || (car(p) == symbol(MULTIPLY) && isnegativenumber(cadr(p))))
			append(MML_MINUS);
		print_term(p);
	}
}

function
print_term(p)
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
			append("<mfrac>");
			append("<mrow>");
			print_numerators(p);
			append("</mrow>");
			append("<mrow>");
			print_denominators(p);
			append("</mrow>");
			append("</mfrac>");
			return;
		}

		// no denominators

		p = cdr(p);
		q = car(p);

		if (isrational(q) && isminusone(q))
			p = cdr(p); // skip -1

		print_factor(car(p));
		p = cdr(p);

		while (iscons(p)) {
			append("<mtext>&nbsp;</mtext>");
			print_factor(car(p));
			p = cdr(p);
		}
	} else
		print_factor(p);
}

function
print_numerators(p)
{
	var n, q;

	n = 0;

	p = cdr(p);
	q = car(p);

	if (isrational(q)) {
		if (Math.abs(q.a) != 1) {
			print_mn(Math.abs(q.a).toFixed(0));
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
			append("<mtext>&nbsp;</mtext>");

		print_factor(q);
		n++;
		p = cdr(p);
	}

	if (n == 0)
		print_mn("1"); // there were no numerators
}

function
print_denominators(p)
{
	var n, q;

	n = 0;

	p = cdr(p);
	q = car(p);

	if (isrational(q)) {
		if (q.b != 1) {
			print_mn(q.b.toFixed(0));
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
			append("<mtext>&nbsp;</mtext>");

		// example (-1)^(-1/4)

		if (isminusone(cadr(q))) {
			append("<msup>");
			append("<mrow>");
			append(MML_MINUS_1); // (-1)
			append("</mrow>");
			append("<mrow>");
			print_number(caddr(q)); // -1/4 (sign not printed)
			append("</mrow>");
			append("</msup>");
			n++;
			p = cdr(p);
			continue;
		}

		// example 1/y

		if (isminusone(caddr(q))) {
			print_factor(cadr(q)); // y
			n++;
			p = cdr(p);
			continue;
		}

		// example 1/y^2

		append("<msup>");
		append("<mrow>");
		print_base(cadr(q)); // y
		append("</mrow>");
		append("<mrow>");
		print_number(caddr(q)); // -2 (sign not printed)
		append("</mrow>");
		append("</msup>");

		n++;
		p = cdr(p);
	}

	if (n == 0)
		print_mn("1"); // there were no denominators
}

function
print_factor(p)
{
	if (isrational(p)) {
		print_rational(p);
		return;
	}

	if (isdouble(p)) {
		print_double(p);
		return;
	}

	if (issymbol(p)) {
		print_symbol(p);
		return;
	}

	if (isstring(p)) {
		print_string(p);
		return;
	}

	if (istensor(p)) {
		print_tensor(p);
		return;
	}

	if (iscons(p)) {
		if (car(p) == symbol(POWER))
			print_power(p);
		else if (car(p) == symbol(ADD) || car(p) == symbol(MULTIPLY))
			print_subexpr(p);
		else
			print_function(p);
		return;
	}
}

function
print_number(p)
{
	if (isrational(p))
		print_rational(p);
	else
		print_double(p);
}

function
print_rational(p)
{
	if (p.b == 1) {
		print_mn(Math.abs(p.a).toFixed(0));
		return;
	}

	append("<mfrac>");
	print_mn(Math.abs(p.a).toFixed(0));
	print_mn(p.b.toFixed(0));
	append("</mfrac>");
}

function
print_double(p)
{
	var i, j, k, s;

	if (p.d == 0) {
		print_mn("0");
		return;
	}

	s = Math.abs(p.d).toPrecision(6) + 'T'; // terminator

	k = 0;

	while (isdigit(s[k]) || s[k] == '.')
		k++;

	// handle trailing zeroes

	i = s.indexOf(".");

	if (i == -1)
		print_mn(s.substring(0, k));
	else {
		for (j = k - 1; j > i + 1; j--) {
			if (s[j] != '0')
				break;
		}
		print_mn(s.substring(0, j + 1));
	}

	if (s[k] != 'E' && s[k] != 'e')
		return;

	k++;

	print_mo("&times;");

	append("<msup>");
	print_mn("10");
	append("<mrow>");

	if (s[k] == '+')
		k++;
	else if (s[k] == '-') {
		k++;
		append(MML_MINUS);
	}

	while (s[k] == '0')
		k++; // skip leading zeroes

	print_mn(s.substring(k, s.length - 1));

	append("</mrow>");
	append("</msup>");
}

function
print_power(p)
{
	// (-1)^x

	if (isminusone(cadr(p))) {
		print_imaginary(p);
		return;
	}

	// e^x

	if (cadr(p) == symbol(EXP1)) {
		print_mi("exp");
		append(MML_LP);
		print_expr(caddr(p)); // x
		append(MML_RP);
		return;
	}

	// example 1/y

	if (isminusone(caddr(p))) {
		append("<mfrac>");
		print_mn("1"); // 1
		append("<mrow>");
		print_expr(cadr(p)); // y
		append("</mrow>");
		append("</mfrac>");
		return;
	}

	// example 1/y^2

	if (isnegativenumber(caddr(p))) {
		append("<mfrac>");
		print_mn("1"); // 1
		append("<msup>");
		append("<mrow>");
		print_base(cadr(p)); // y
		append("</mrow>");
		append("<mrow>");
		print_number(caddr(p)); // -2 (sign not printed)
		append("</mrow>");
		append("</msup>");
		append("</mfrac>");
		return;
	}

	// example y^x

	append("<msup>");
	append("<mrow>");
	print_base(cadr(p)); // y
	append("</mrow>");
	append("<mrow>");
	print_exponent(caddr(p)); // x
	append("</mrow>");
	append("</msup>");
}

function
print_base(p)
{
	if (isfraction(p) || isdouble(p) || car(p) == symbol(POWER))
		print_subexpr(p);
	else
		print_factor(p);
}

function
print_exponent(p)
{
	if (car(p) == symbol(POWER))
		print_subexpr(p);
	else
		print_factor(p);
}

// case (-1)^x

function
print_imaginary(p)
{
	if (isimaginaryunit(p)) {
		if (isimaginaryunit(get_binding(symbol(SYMBOL_J)))) {
			print_mi("j");
			return;
		}
		if (isimaginaryunit(get_binding(symbol(SYMBOL_I)))) {
			print_mi("i");
			return;
		}
	}

	// example (-1)^(-1/4)

	if (isnegativenumber(caddr(p))) {
		append("<mfrac>");
		print_mn("1");
		append("<msup>");
		append("<mrow>");
		append(MML_MINUS_1); // (-1)
		append("</mrow>");
		append("<mrow>");
		print_number(caddr(p)); // -1/4 (sign not printed)
		append("</mrow>");
		append("</msup>");
		append("</mfrac>");
		return;
	}

	// example (-1)^x

	append("<msup>");
	append("<mrow>");
	append(MML_MINUS_1); // (-1)
	append("</mrow>");
	append("<mrow>");
	print_expr(caddr(p)); // x
	append("</mrow>");
	append("</msup>");
}

function
print_function(p)
{
	// d(f(x),x)

	if (car(p) == symbol(DERIVATIVE)) {
		append("<mi mathvariant='normal'>d</mi>");
		append(MML_LP);
		print_arglist(p);
		append(MML_RP);
		return;
	}

	// n!

	if (car(p) == symbol(FACTORIAL)) {
		p = cadr(p);
		if (isposint(p) || issymbol(p))
			print_expr(p);
		else
			print_subexpr(p);
		print_mo("!");
		return;
	}

	// A[1,2]

	if (car(p) == symbol(INDEX)) {
		p = cdr(p);
		if (issymbol(car(p)))
			print_symbol(car(p));
		else
			print_subexpr(car(p));
		append(MML_LB);
		print_arglist(p);
		append(MML_RB);
		return;
	}

	if (car(p) == symbol(SETQ)) {
		print_expr(cadr(p));
		print_mo("=");
		print_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTEQ)) {
		print_expr(cadr(p));
		print_mo("=");
		print_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTGE)) {
		print_expr(cadr(p));
		print_mo("&ge;");
		print_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTGT)) {
		print_expr(cadr(p));
		print_mo("&gt;");
		print_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTLE)) {
		print_expr(cadr(p));
		print_mo("&le;");
		print_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTLT)) {
		print_expr(cadr(p));
		print_mo("&lt");
		print_expr(caddr(p));
		return;
	}

	// default

	if (issymbol(car(p)))
		print_symbol(car(p));
	else
		print_subexpr(car(p));

	append(MML_LP);
	print_arglist(p);
	append(MML_RP);
}

function
print_arglist(p)
{
	p = cdr(p);
	if (iscons(p)) {
		print_expr(car(p));
		p = cdr(p);
		while(iscons(p)) {
			print_mo(",");
			print_expr(car(p));
			p = cdr(p);
		}
	}
}

function
print_subexpr(p)
{
	append(MML_LP);
	print_expr(p);
	append(MML_RP);
}

function
print_symbol(p)
{
	var k, n, s;

	if (iskeyword(p) || p == symbol(LAST) || p == symbol(TRACE)) {
		print_mi(printname(p));
		return;
	}

	if (p == symbol(EXP1)) {
		print_mi("exp");
		append(MML_LP);
		print_mn("1");
		append(MML_RP);
		return;
	}

	s = printname(p);

	n = print_symbol_scan(s, 0);

	if (n == s.length) {
		print_symbol_shipout(s);
		return;
	}

	// print symbol with subscript

	append("<msub>");

	print_symbol_shipout(s.substring(0, n));

	append("<mrow>");

	k = n;

	while (k < s.length) {
		n = print_symbol_scan(s, k);
		print_symbol_shipout(s.substring(k, k + n));
		k += n;
	}

	append("</mrow>");
	append("</msub>");
}

var print_greek_tab = [
	"Alpha","Beta","Gamma","Delta","Epsilon","Zeta","Eta","Theta","Iota",
	"Kappa","Lambda","Mu","Nu","Xi","Pi","Rho","Sigma","Tau","Upsilon",
	"Phi","Chi","Psi","Omega",
	"alpha","beta","gamma","delta","epsilon","zeta","eta","theta","iota",
	"kappa","lambda","mu","nu","xi","pi","rho","sigma","tau","upsilon",
	"phi","chi","psi","omega",
];

function
print_symbol_scan(s, k)
{
	var i, m, n, t;

	n = print_greek_tab.length;

	for (i = 0; i < n; i++) {

		t = print_greek_tab[i];

		m = t.length;

		if (s.substring(k, k + m) == t)
			return m;
	}

	return 1;
}

function
print_symbol_shipout(s)
{
	if (s.length == 1) {
		append("<mi>");
		append(s);
		append("</mi>");
		return;
	}

	// greek

	if (s[0] >= 'A' && s[0] <= 'Z') {
		append("<mi mathvariant='normal'>&"); // upper case
		append(s);
		append(";</mi>");
	} else {
		append("<mi>&");
		append(s);
		append(";</mi>");
	}
}

function
print_tensor(p)
{
	var i, k, n;

	k = 0;

	// if odd rank then vector

	if (p.dim.length % 2 == 1) {
		append(MML_LP);
		append("<mtable>");
		n = p.dim[0];
		for (i = 0; i < n; i++) {
			append("<mtr><mtd>");
			k = print_matrix(p, 1, k);
			append("</mtd></mtr>");
		}
		append("</mtable>");
		append(MML_RP);
	} else
		print_matrix(p, 0, k);
}

function
print_matrix(p, d, k)
{
	var i, j, m, n;

	if (d == p.dim.length) {
		print_expr(p.elem[k]);
		return k + 1;
	}

	n = p.dim[d];
	m = p.dim[d + 1];

	append(MML_LP);
	append("<mtable>");

	for (i = 0; i < n; i++) {
		append("<mtr>");
		for (j = 0; j < m; j++) {
			append("<mtd>");
			k = print_matrix(p, d + 2, k);
			append("</mtd>");
		}
		append("</mtr>");
	}

	append("</mtable>");
	append(MML_RP);

	return k;
}

function
print_string(p)
{
	var s = p.string;

	append("<mtext>");

	s = s.replace(/&/g, "&amp;");
	s = s.replace(/</g, "&lt;");
	s = s.replace(/>/g, "&gt;");
	s = s.replace(/\n/g, "<br>");

	append(s);

	append("</mtext>");
}

function
print_mi(s)
{
	append("<mi>");
	append(s);
	append("</mi>");
}

function
print_mn(s)
{
	append("<mn>");
	append(s);
	append("</mn>");
}

function
print_mo(s)
{
	append("<mo>");
	append(s);
	append("</mo>");
}

function
append(s)
{
	outbuf += s;
}

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
	print_expr(p1);
	stdout.innerHTML += "<p><math>" + outbuf + "</math></p>";
}

function
print_expr(p)
{
	var q;

	if (car(p) == symbol(ADD)) {
		p = cdr(p);
		q = car(p);
		if (isnegativenumber(q) || (car(q) == symbol(MULTIPLY) && isnegativenumber(cadr(q))))
			printf(MML_MINUS);
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
			printf(MML_MINUS);
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
			printf("<mfrac>");
			printf("<mrow>");
			print_numerators(p);
			printf("</mrow>");
			printf("<mrow>");
			print_denominators(p);
			printf("</mrow>");
			printf("</mfrac>");
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
			printf("<mtext>&nbsp;</mtext>");
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
			printf("<mtext>&nbsp;</mtext>");

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
			printf("<mtext>&nbsp;</mtext>");

		// example (-1)^(-1/4)

		if (isminusone(cadr(q))) {
			printf("<msup>");
			printf("<mrow>");
			printf(MML_MINUS_1); // (-1)
			printf("</mrow>");
			printf("<mrow>");
			print_number(caddr(q)); // -1/4 (sign not printed)
			printf("</mrow>");
			printf("</msup>");
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

		printf("<msup>");
		printf("<mrow>");
		print_base(cadr(q)); // y
		printf("</mrow>");
		printf("<mrow>");
		print_number(caddr(q)); // -2 (sign not printed)
		printf("</mrow>");
		printf("</msup>");

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

	printf("<mfrac>");
	print_mn(Math.abs(p.a).toFixed(0));
	print_mn(p.b.toFixed(0));
	printf("</mfrac>");
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

	printf("<msup>");
	print_mn("10");
	printf("<mrow>");

	if (s[k] == '+')
		k++;
	else if (s[k] == '-') {
		k++;
		printf(MML_MINUS);
	}

	while (s[k] == '0')
		k++; // skip leading zeroes

	print_mn(s.substring(k, s.length - 1));

	printf("</mrow>");
	printf("</msup>");
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
		printf(MML_LP);
		print_expr(caddr(p)); // x
		printf(MML_RP);
		return;
	}

	// example 1/y

	if (isminusone(caddr(p))) {
		printf("<mfrac>");
		print_mn("1"); // 1
		printf("<mrow>");
		print_expr(cadr(p)); // y
		printf("</mrow>");
		printf("</mfrac>");
		return;
	}

	// example 1/y^2

	if (isnegativenumber(caddr(p))) {
		printf("<mfrac>");
		print_mn("1"); // 1
		printf("<msup>");
		printf("<mrow>");
		print_base(cadr(p)); // y
		printf("</mrow>");
		printf("<mrow>");
		print_number(caddr(p)); // -2 (sign not printed)
		printf("</mrow>");
		printf("</msup>");
		printf("</mfrac>");
		return;
	}

	// example y^x

	printf("<msup>");
	printf("<mrow>");
	print_base(cadr(p)); // y
	printf("</mrow>");
	printf("<mrow>");
	print_exponent(caddr(p)); // x
	printf("</mrow>");
	printf("</msup>");
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
		printf("<mfrac>");
		print_mn("1");
		printf("<msup>");
		printf("<mrow>");
		printf(MML_MINUS_1); // (-1)
		printf("</mrow>");
		printf("<mrow>");
		print_number(caddr(p)); // -1/4 (sign not printed)
		printf("</mrow>");
		printf("</msup>");
		printf("</mfrac>");
		return;
	}

	// example (-1)^x

	printf("<msup>");
	printf("<mrow>");
	printf(MML_MINUS_1); // (-1)
	printf("</mrow>");
	printf("<mrow>");
	print_expr(caddr(p)); // x
	printf("</mrow>");
	printf("</msup>");
}

function
print_function(p)
{
	// d(f(x),x)

	if (car(p) == symbol(DERIVATIVE)) {
		printf("<mi mathvariant='normal'>d</mi>");
		printf(MML_LP);
		print_arglist(p);
		printf(MML_RP);
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
		printf(MML_LB);
		print_arglist(p);
		printf(MML_RB);
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

	printf(MML_LP);
	print_arglist(p);
	printf(MML_RP);
}

function
print_arglist(p)
{
	p = cdr(p);
	if (iscons(p)) {
		print_expr(car(p));
		p = cdr(p);
		while (iscons(p)) {
			print_mo(",");
			print_expr(car(p));
			p = cdr(p);
		}
	}
}

function
print_subexpr(p)
{
	printf(MML_LP);
	print_expr(p);
	printf(MML_RP);
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
		printf(MML_LP);
		print_mn("1");
		printf(MML_RP);
		return;
	}

	s = printname(p);

	n = print_symbol_scan(s, 0);

	if (n == s.length) {
		print_symbol_shipout(s);
		return;
	}

	// print symbol with subscript

	printf("<msub>");

	print_symbol_shipout(s.substring(0, n));

	printf("<mrow>");

	k = n;

	while (k < s.length) {
		n = print_symbol_scan(s, k);
		print_symbol_shipout(s.substring(k, k + n));
		k += n;
	}

	printf("</mrow>");
	printf("</msub>");
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
	var i, n, t;
	n = print_greek_tab.length;
	for (i = 0; i < n; i++) {
		t = print_greek_tab[i];
		if (s.startsWith(t, k))
			return t.length;
	}
	return 1;
}

function
print_symbol_shipout(s)
{
	if (s.length == 1) {
		printf("<mi>");
		printf(s);
		printf("</mi>");
		return;
	}

	// greek

	if (s[0] >= 'A' && s[0] <= 'Z') {
		printf("<mi mathvariant='normal'>&"); // upper case
		printf(s);
		printf(";</mi>");
	} else {
		printf("<mi>&");
		printf(s);
		printf(";</mi>");
	}
}

function
print_tensor(p)
{
	var i, k, n;

	k = 0;

	// if odd rank then vector

	if (p.dim.length % 2 == 1) {
		printf(MML_LP);
		printf("<mtable>");
		n = p.dim[0];
		for (i = 0; i < n; i++) {
			printf("<mtr><mtd>");
			k = print_matrix(p, 1, k);
			printf("</mtd></mtr>");
		}
		printf("</mtable>");
		printf(MML_RP);
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

	printf(MML_LP);
	printf("<mtable>");

	for (i = 0; i < n; i++) {
		printf("<mtr>");
		for (j = 0; j < m; j++) {
			printf("<mtd>");
			k = print_matrix(p, d + 2, k);
			printf("</mtd>");
		}
		printf("</mtr>");
	}

	printf("</mtable>");
	printf(MML_RP);

	return k;
}

function
print_string(p)
{
	var s = p.string;

	printf("<mtext>");

	s = s.replace(/&/g, "&amp;");
	s = s.replace(/</g, "&lt;");
	s = s.replace(/>/g, "&gt;");
	s = s.replace(/\n/g, "<br>");

	printf(s);

	printf("</mtext>");
}

function
print_mi(s)
{
	printf("<mi>");
	printf(s);
	printf("</mi>");
}

function
print_mn(s)
{
	printf("<mn>");
	printf(s);
	printf("</mn>");
}

function
print_mo(s)
{
	printf("<mo>");
	printf(s);
	printf("</mo>");
}

function
printf(s)
{
	outbuf += s;
}

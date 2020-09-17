/*
BSD 2-Clause License

Copyright (c) 2020, George Weigt
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
function
abs()
{
	var h, p1;

	p1 = pop();

	if (istensor(p1)) {
		if (p1.dim.length != 1)
			stopf("abs: tensor rank > 1");
		push(p1);
		push(p1);
		conj();
		inner();
		push_rational(1, 2);
		power();
		return;
	}

	if (isnum(p1)) {
		push(p1);
		if (isnegativenumber(p1))
			negate();
		return;
	}

	if (iscomplexnumber(p1)) {
		push(p1);
		push(p1);
		conj();
		multiply();
		push_rational(1, 2);
		power();
		return;
	}

	// abs(1/a) evaluates to 1/abs(a)

	if (car(p1) == symbol(POWER) && isnegativeterm(caddr(p1))) {
		push(p1);
		reciprocate();
		abs();
		reciprocate();
		return;
	}

	// abs(a*b) evaluates to abs(a)*abs(b)

	if (car(p1) == symbol(MULTIPLY)) {
		h = stack.length;
		p1 = cdr(p1);
		while (iscons(p1)) {
			push(car(p1));
			abs();
			p1 = cdr(p1);
		}
		multiply_factors(stack.length - h);
		return;
	}

	if (isnegativeterm(p1) || (car(p1) == symbol(ADD) && isnegativeterm(cadr(p1)))) {
		push(p1);
		negate();
		p1 = pop();
	}

	push_symbol(ABS);
	push(p1);
	list(2);
}
function
add()
{
	add_terms(2);
}
function
add_numbers(p1, p2)
{
	var a, b;

	if (isrational(p1) && isrational(p2)) {
		add_rationals(p1, p2);
		return;
	}

	push(p1);
	a = pop_double();

	push(p2);
	b = pop_double();

	push_double(a + b);
}
function
add_rationals(p1, p2)
{
	var a, b, c;

	a = p1.a * p2.b + p1.b * p2.a;
	b = p1.b * p2.b;

	if (a == 0) {
		push_integer(0);
		return;
	}

	if (Math.abs(a) > 1e10 || b > 1e10) {
		push_double(a / b);
		return;
	}

	if (b > 1) {
		c = gcd_integers(a, b);
		a /= c;
		b /= c;
	}

	push_rational(a, b);
}
function
add_tensors(p1, p2)
{
	var i, n;

	if (!compatible_dimensions(p1, p2))
		stopf("incompatible tensor arithmetic");

	p1 = copy_tensor(p1);

	n = p1.elem.length;

	for (i = 0; i < n; i++) {
		push(p1.elem[i]);
		push(p2.elem[i]);
		add();
		p1.elem[i] = pop();
	}

	push(p1);
}
// n is number of terms on stack

function
add_terms(n)
{
	var h;

	if (n < 2)
		return;

	if (n == 2 && isnum(stack[stack.length - 2]) && isnum(stack[stack.length - 1])) {
		var p2 = pop();
		var p1 = pop();
		add_numbers(p1, p2);
		return;
	}

	h = stack.length - n;

	flatten_terms(h);

	combine_terms(h);

	n = stack.length - h;

	switch (n) {
	case 0:
		push_integer(0); // all terms canceled
		break;
	case 1:
		break;
	default:
		list(n);
		push_symbol(ADD);
		swap();
		cons();
		break;
	}
}
function
adj()
{
	var col, i, j, k, n, row, p1, p2, p3;

	p1 = pop();

	if (!istensor(p1) || p1.dim.length != 2 || p1.dim[0] != p1.dim[1])
		stopf("adj: square matrix expected");

	n = p1.dim[0];

	// p2 is the adjunct matrix

	p2 = alloc_tensor();

	p2.dim[0] = n;
	p2.dim[1] = n;

	if (n == 2) {
		p2.elem[0] = p1.elem[3];
		push(p1.elem[1]);
		negate();
		p2.elem[1] = pop();
		push(p1.elem[2]);
		negate();
		p2.elem[2] = pop();
		p2.elem[3] = p1.elem[0];
		push(p2);
		return;
	}

	// p3 is for computing cofactors

	p3 = alloc_tensor();

	p3.dim[0] = n - 1;
	p3.dim[1] = n - 1;

	for (row = 0; row < n; row++) {
		for (col = 0; col < n; col++) {
			k = 0;
			for (i = 0; i < n; i++)
				for (j = 0; j < n; j++)
					if (i != row && j != col)
						p3.elem[k++] = p1.elem[n * i + j];
			push(p3);
			det();
			if ((row + col) % 2)
				negate();
			p2.elem[n * col + row] = pop(); // transpose
		}
	}

	push(p2);
}
function
alloc_tensor()
{
	return {dim:[], elem:[]};
}
function
any_radical_factors(h)
{
	var i, n;
	n = stack.length;
	for (i = h; i < n; i++)
		if (isradical(stack[i]))
			return 1;
	return 0;
}
function
arccos()
{
	var p1;

	p1 = pop();

	if (isdouble(p1)) {
		push_double(Math.acos(p1.d));
		return;
	}

	// arccos(z) = -i log(z + sqrt(z^2 - 1))

	if (isdoublez(p1)) {
		push(imaginaryunit);
		negate();
		push(p1);
		push(p1);
		push(p1);
		multiply();
		push_double(-1.0);
		add();
		push_rational(1, 2);
		power();
		add();
		log();
		multiply();
		return;
	}

	// if p1 == 1/sqrt(2) then return 1/4*pi (45 degrees)

	if (isoneoversqrttwo(p1)) {
		push_rational(1, 4);
		push_symbol(PI);
		multiply();
		return;
	}

	// if p1 == -1/sqrt(2) then return 3/4*pi (135 degrees)

	if (isminusoneoversqrttwo(p1)) {
		push_rational(3, 4);
		push_symbol(PI);
		multiply();
		return;
	}

	// arccos(0) = 1/2 pi

	if (iszero(p1)) {
		push_rational(1, 2);
		push_symbol(PI);
		multiply();
		return;
	}

	// arccos(1/2) = 1/3 pi

	if (equalq(p1, 1 ,2)) {
		push_rational(1, 3);
		push_symbol(PI);
		multiply();
		return;
	}

	// arccos(1) = 0

	if (equaln(p1, 1)) {
		push_integer(0);
		return;
	}

	// arccos(-1/2) = 2/3 pi

	if (equalq(p1, -1, 2)) {
		push_rational(2, 3);
		push_symbol(PI);
		multiply();
		return;
	}

	// arccos(-1) = pi

	if (equaln(p1, -1)) {
		push_symbol(PI);
		return;
	}

	push_symbol(ARCCOS);
	push(p1);
	list(2);
}
function
arccosh()
{
	var p1 = pop();

	if (isdouble(p1) && p1.d >= 1.0) {
		push_double(Math.acosh(p1.d));
		return;
	}

	// arccosh(z) = log(z + (z^2 - 1)^(1/2))

	if (isdouble(p1) || isdoublez(p1)) {
		push(p1);
		push(p1);
		multiply();
		push_double(-1.0);
		add();
		push_rational(1, 2);
		power();
		push(p1);
		add();
		log();
		return;
	}

	if (isplusone(p1)) {
		push_integer(0);
		return;
	}

	if (car(p1) == symbol(COSH)) {
		push(cadr(p1));
		return;
	}

	push_symbol(ARCCOSH);
	push(p1);
	list(2);
}
function
arcsin()
{
	var n, p1, p2;

	p1 = pop();

	if (isdouble(p1)) {
		push_double(Math.asin(p1.d));
		return;
	}

	// arcsin(z) = -i log(i z + (1 - z^2)^(1/2))

	if (isdoublez(p1)) {
		push(imaginaryunit);
		negate();
		push(imaginaryunit);
		push(p1);
		multiply();
		push_double(1.0);
		push(p1);
		push(p1);
		multiply();
		subtract();
		push_rational(1, 2);
		power();
		add();
		log();
		multiply();
		return;
	}

	// if p1 == 1/sqrt(2) then return 1/4*pi (45 degrees)

	if (isoneoversqrttwo(p1)) {
		push_rational(1, 4);
		push_symbol(PI);
		multiply();
		return;
	}

	// if p1 == -1/sqrt(2) then return -1/4*pi (-45 degrees)

	if (isminusoneoversqrttwo(p1)) {
		push_rational(-1, 4);
		push_symbol(PI);
		multiply();
		return;
	}

	if (!isrational(p1)) {
		push_symbol(ARCSIN);
		push(p1);
		list(2);
		return;
	}

	push(p1);
	push_integer(2);
	multiply();
	p2 = pop();

	if (isinteger(p2))
		n = p2.a;
	else
		n = 99;

	switch (n) {

	case -2:
		push_rational(-1, 2);
		push_symbol(PI);
		multiply();
		break;

	case -1:
		push_rational(-1, 6);
		push_symbol(PI);
		multiply();
		break;

	case 0:
		push_integer(0);
		break;

	case 1:
		push_rational(1, 6);
		push_symbol(PI);
		multiply();
		break;

	case 2:
		push_rational(1, 2);
		push_symbol(PI);
		multiply();
		break;

	default:
		push_symbol(ARCSIN);
		push(p1);
		list(2);
		break;
	}
}
function
arcsinh()
{
	var p1 = pop();

	if (isdouble(p1)) {
		push_double(Math.asinh(p1.d));
		return;
	}

	// arcsinh(z) = log(z + (z^2 + 1)^(1/2))

	if (isdoublez(p1)) {
		push(p1);
		push(p1);
		multiply();
		push_double(1.0);
		add();
		push_rational(1, 2);
		power();
		push(p1);
		add();
		log();
		return;
	}

	if (iszero(p1)) {
		push(p1);
		return;
	}

	// arcsinh(-x) = -arcsinh(x)

	if (isnegative(p1)) {
		push(p1);
		negate();
		arcsinh();
		negate();
		return;
	}

	if (car(p1) == symbol(SINH)) {
		push(cadr(p1));
		return;
	}

	push_symbol(ARCSINH);
	push(p1);
	list(2);
}
function
arctan()
{
	var X, Y, Z;

	X = pop();
	Y = pop();

	if (isnum(X) && isnum(Y)) {
		arctan_numbers(X, Y);
		return;
	}

	// arctan(z) = 1/2 i log((i + z) / (i - z))

	if (!iszero(X) && (isdoublez(X) || isdoublez(Y))) {
		push(Y);
		push(X);
		divide();
		Z = pop();
		push_double(0.5);
		push(imaginaryunit);
		multiply();
		push(imaginaryunit);
		push(Z);
		add();
		push(imaginaryunit);
		push(Z);
		subtract();
		divide();
		log();
		multiply();
		return;
	}

	if (car(Y) == symbol(TAN) && isplusone(X)) {
		push(cadr(Y)); // x of tan(x)
		return;
	}

	push_symbol(ARCTAN);
	push(Y);
	push(X);
	list(3);
}
function
arctan_numbers(X, Y)
{
	var x, y, T;

	if (iszero(X) && iszero(Y)) {
		push_symbol(ARCTAN);
		push_integer(0);
		push_integer(0);
		list(3);
		return;
	}

	if (isnum(X) && isnum(Y) && (isdouble(X) || isdouble(Y))) {
		push(X);
		x = pop_double();
		push(Y);
		y = pop_double();
		push_double(Math.atan2(y, x));
		return;
	}

	// X and Y are rational numbers

	if (iszero(Y)) {
		if (X.a >= 0)
			push_integer(0);
		else
			push_integer(-1);
		push_symbol(PI);
		multiply();
		return;
	}

	if (iszero(X)) {
		if (Y.a >= 0)
			push_rational(1, 2);
		else
			push_rational(-1, 2);
		push_symbol(PI);
		multiply();
		return;
	}

	// convert fractions to integers

	push(Y);
	push(X);
	divide();
	abs();
	T = pop();

	push(T);
	numerator();
	if (Y.a < 0)
		negate();
	Y = pop();

	push(T);
	denominator();
	if (X.a < 0)
		negate();
	X = pop();

	// compare numerators and denominators, ignore signs

	if (Math.abs(X.a) != Math.abs(Y.a) || X.b != Y.b) {
		// not equal
		push_symbol(ARCTAN);
		push(Y);
		push(X);
		list(3);
		return;
	}

	// X == Y (modulo sign)

	if (X.a >= 0)
		if (Y.a >= 0)
			push_rational(1, 4);
		else
			push_rational(-1, 4);
	else
		if (Y.a >= 0)
			push_rational(3, 4);
		else
			push_rational(-3, 4);

	push_symbol(PI);
	multiply();
}
function
arctanh()
{
	var p1 = pop();

	if (isplusone(p1) || isminusone(p1)) {
		push_symbol(ARCTANH);
		push(p1);
		list(2);
		return;
	}

	if (isdouble(p1)) {
		push_double(Math.atanh(p1.d));
		return;
	}

	// arctanh(z) = 1/2 (log(1 + z) - log(1 - z))

	if (isdoublez(p1)) {
		push_double(1.0);
		push(p1);
		add();
		log();
		push_double(1.0);
		push(p1);
		subtract();
		log();
		subtract();
		push_double(0.5);
		multiply();
		return;
	}

	if (iszero(p1)) {
		push_integer(0);
		return;
	}

	// arctanh(-x) = -arctanh(x)

	if (isnegative(p1)) {
		push(p1);
		negate();
		arctanh();
		negate();
		return;
	}

	if (car(p1) == symbol(TANH)) {
		push(cadr(p1));
		return;
	}

	push_symbol(ARCTANH);
	push(p1);
	list(2);
}
function
arg()
{
	var p1 = pop();

	push(p1);
	numerator();
	arg_nib();

	push(p1);
	denominator();
	arg_nib();

	subtract();
}
function
arg_nib()
{
	var h, p1, RE, IM;

	p1 = pop();

	if (isrational(p1)) {
		if (p1.a >= 0)
			push_integer(0);
		else {
			push_symbol(PI);
			negate();
		}
		return;
	}

	if (isdouble(p1)) {
		if (p1.d >= 0.0)
			push_double(0.0);
		else
			push_double(-Math.PI);
		return;
	}

	// (-1) ^ expr

	if (car(p1) == symbol(POWER) && isminusone(cadr(p1))) {
		push_symbol(PI);
		push(caddr(p1));
		multiply();
		return;
	}

	// e ^ expr

	if (car(p1) == symbol(POWER) && cadr(p1) == symbol(EXP1)) {
		push(caddr(p1));
		imag();
		return;
	}

	if (car(p1) == symbol(MULTIPLY)) {
		h = stack.length;
		p1 = cdr(p1);
		while (iscons(p1)) {
			push(car(p1));
			arg();
			p1 = cdr(p1);
		}
		add_terms(stack.length - h);
		return;
	}

	if (car(p1) == symbol(ADD)) {
		push(p1);
		rect(); // convert polar and clock forms
		p1 = pop();
		push(p1);
		real();
		RE = pop();
		push(p1);
		imag();
		IM = pop();
		push(IM);
		push(RE);
		arctan();
		return;
	}

	push_integer(0); // p1 is real
}
function
caaddr(p)
{
	return car(car(cdr(cdr(p))));
}
function
caadr(p)
{
	return car(car(cdr(p)));
}
function
cadaddr(p)
{
	return car(cdr(car(cdr(cdr(p)))));
}
function
cadadr(p)
{
	return car(cdr(car(cdr(p))));
}
function
cadddr(p)
{
	return car(cdr(cdr(cdr(p))));
}
function
caddr(p)
{
	return car(cdr(cdr(p)));
}
function
cadr(p)
{
	return car(cdr(p));
}
function
cancel_factor()
{
	var h, p1, p2;

	p2 = pop();
	p1 = pop();

	if (car(p2) == symbol(ADD)) {
		h = stack.length;
		p2 = cdr(p2);
		while (iscons(p2)) {
			push(p1);
			push(car(p2));
			multiply();
			p2 = cdr(p2);
		}
		add_terms(stack.length - h);
		return;
	}

	push(p1);
	push(p2);
	multiply();
}
function
car(p)
{
	if ("car" in p)
		return p.car;
	else
		return symbol(NIL);
}
function
cdadr(p)
{
	return cdr(car(cdr(p)));
}
function
cddadr(p)
{
	return cdr(cdr(car(cdr(p))));
}
function
cddr(p)
{
	return cdr(cdr(p));
}
function
cdr(p)
{
	if ("cdr" in p)
		return p.cdr;
	else
		return symbol(NIL);
}
function
circexp()
{
	circexp_subst();
	evalf();
}
function
circexp_subst()
{
	var i, h, n, p1;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1.elem.length;
		for (i = 0; i < n; i++) {
			push(p1.elem[i]);
			circexp_subst();
			p1.elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (car(p1) == symbol(COS)) {
		push_symbol(EXPCOS);
		push(cadr(p1));
		circexp_subst();
		list(2);
		return;
	}

	if (car(p1) == symbol(SIN)) {
		push_symbol(EXPSIN);
		push(cadr(p1));
		circexp_subst();
		list(2);
		return;
	}

	if (car(p1) == symbol(TAN)) {
		push_symbol(EXPTAN);
		push(cadr(p1));
		circexp_subst();
		list(2);
		return;
	}

	if (car(p1) == symbol(COSH)) {
		push_symbol(EXPCOSH);
		push(cadr(p1));
		circexp_subst();
		list(2);
		return;
	}

	if (car(p1) == symbol(SINH)) {
		push_symbol(EXPSINH);
		push(cadr(p1));
		circexp_subst();
		list(2);
		return;
	}

	if (car(p1) == symbol(TANH)) {
		push_symbol(EXPTANH);
		push(cadr(p1));
		circexp_subst();
		list(2);
		return;
	}

	// none of the above

	if (iscons(p1)) {
		h = stack.length;
		push(car(p1));
		p1 = cdr(p1);
		while (iscons(p1)) {
			push(car(p1));
			circexp_subst();
			p1 = cdr(p1);
		}
		list(stack.length - h);
		return;
	}

	push(p1);
}
function
clock()
{
	var p1 = pop();

	push(p1);
	mag();

	push_integer(-1); // base

	push(p1);
	arg();
	push_symbol(PI);
	divide();

	power();

	multiply();
}
function
cmp_args(p1)
{
	var p2;

	p1 = cdr(p1);
	push(car(p1));
	evalf();

	p1 = cdr(p1);
	push(car(p1));
	evalf();

	p2 = pop();
	p1 = pop();

	if (istensor(p1) || istensor(p2))
		stopf("tensor comparison");

	push(p1);
	push(p2);
	subtract();
	p1 = pop();

	if (!isnum(p1)) {
		push(p1);
		floatf(); // try converting pi and e
		p1 = pop();
		if (!isnum(p1))
			stopf("non-numerical comparison");
	}

	if (iszero(p1))
		return 0;

	if (isrational(p1)) {
		if (p1.a > 0)
			return 1;
		else
			return -1;
	}

	if (p1.d > 0)
		return 1;
	else
		return -1;
}
function
cmp_expr(p1, p2)
{
	var n;

	if (p1 == p2)
		return 0;

	if (p1 == symbol(NIL))
		return -1;

	if (p2 == symbol(NIL))
		return 1;

	if (isnum(p1) && isnum(p2))
		return compare_numbers(p1, p2);

	if (isnum(p1))
		return -1;

	if (isnum(p2))
		return 1;

	if (isstring(p1) && isstring(p2))
		return compare_strings(p1.string, p2.string);

	if (isstring(p1))
		return -1;

	if (isstring(p2))
		return 1;

	if (issymbol(p1) && issymbol(p2))
		return compare_strings(printname(p1), printname(p2));

	if (issymbol(p1))
		return -1;

	if (issymbol(p2))
		return 1;

	if (istensor(p1) && istensor(p2))
		return 0;

	if (istensor(p1))
		return -1;

	if (istensor(p2))
		return 1;

	while (iscons(p1) && iscons(p2)) {
		n = cmp_expr(car(p1), car(p2));
		if (n != 0)
			return n;
		p1 = cdr(p1);
		p2 = cdr(p2);
	}

	if (iscons(p2))
		return -1;

	if (iscons(p1))
		return 1;

	return 0;
}
function
cmp_factors(p1, p2)
{
	var a, b, c;
	var base1, base2, expo1, expo2;

	a = order_factor(p1);
	b = order_factor(p2);

	if (a < b)
		return -1;

	if (a > b)
		return 1;

	if (car(p1) == symbol(POWER)) {
		base1 = cadr(p1);
		expo1 = caddr(p1);
	} else {
		base1 = p1;
		expo1 = one;
	}

	if (car(p2) == symbol(POWER)) {
		base2 = cadr(p2);
		expo2 = caddr(p2);
	} else {
		base2 = p2;
		expo2 = one;
	}

	c = cmp_expr(base1, base2);

	if (c == 0)
		c = cmp_expr(expo2, expo1); // swapped to reverse sort order

	return c;
}
function
cmp_factors_provisional(p1, p2)
{
	if (car(p1) == symbol(POWER))
		p1 = cadr(p1); // p1 = base

	if (car(p2) == symbol(POWER))
		p2 = cadr(p2); // p2 = base

	return cmp_expr(p1, p2);
}
function
cmp_terms(p1, p2)
{
	var a, b, c;

	// 1st level: imaginary terms on the right

	a = isimaginaryterm(p1);
	b = isimaginaryterm(p2);

	if (a == 0 && b == 1)
		return -1; // ok

	if (a == 1 && b == 0)
		return 1; // out of order

	// 2nd level: numericals on the right

	if (isnum(p1) && isnum(p2))
		return 0; // don't care about order, save time, don't compare

	if (isnum(p1))
		return 1; // out of order

	if (isnum(p2))
		return -1; // ok

	// 3rd level: sort by factors

	a = 0;
	b = 0;

	if (car(p1) == symbol(MULTIPLY)) {
		p1 = cdr(p1);
		a = 1; // p1 is a list of factors
		if (isnum(car(p1))) {
			// skip over coeff
			p1 = cdr(p1);
			if (cdr(p1) == symbol(NIL)) {
				p1 = car(p1);
				a = 0;
			}
		}
	}

	if (car(p2) == symbol(MULTIPLY)) {
		p2 = cdr(p2);
		b = 1; // p2 is a list of factors
		if (isnum(car(p2))) {
			// skip over coeff
			p2 = cdr(p2);
			if (cdr(p2) == symbol(NIL)) {
				p2 = car(p2);
				b = 0;
			}
		}
	}

	if (a == 0 && b == 0)
		return cmp_factors(p1, p2);

	if (a == 0 && b == 1) {
		c = cmp_factors(p1, car(p2));
		if (c == 0)
			c = -1; // length(p1) < length(p2)
		return c;
	}

	if (a == 1 && b == 0) {
		c = cmp_factors(car(p1), p2);
		if (c == 0)
			c = 1; // length(p1) > length(p2)
		return c;
	}

	while (iscons(p1) && iscons(p2)) {
		c = cmp_factors(car(p1), car(p2));
		if (c)
			return c;
		p1 = cdr(p1);
		p2 = cdr(p2);
	}

	if (iscons(p1))
		return 1; // length(p1) > length(p2)

	if (iscons(p2))
		return -1; // length(p1) < length(p2)

	return 0;
}
function
collect_numerical_factors(h, COEFF)
{
	var i, n, p1;

	n = stack.length;

	for (i = h; i < n; i++) {

		p1 = stack[i];

		if (isnum(p1)) {
			multiply_numbers(COEFF, p1);
			COEFF = pop();
			stack.splice(i, 1); // remove factor
			i--;
			n--;
		}
	}

	return COEFF;
}
function
combine_factors(h)
{
	var i, n;
	sort_factors_provisional(h);
	n = stack.length;
	for (i = h; i < n - 1; i++) {
		if (combine_factors_nib(i, i + 1)) {
			stack.splice(i + 1, 1); // remove factor
			i--; // use same index again
			n--;
		}
	}
}
function
combine_factors_nib(i, j)
{
	var p1, p2, BASE1, EXPO1, BASE2, EXPO2;

	p1 = stack[i];
	p2 = stack[j];

	if (car(p1) == symbol(POWER)) {
		BASE1 = cadr(p1);
		EXPO1 = caddr(p1);
	} else {
		BASE1 = p1;
		EXPO1 = one;
	}

	if (car(p2) == symbol(POWER)) {
		BASE2 = cadr(p2);
		EXPO2 = caddr(p2);
	} else {
		BASE2 = p2;
		EXPO2 = one;
	}

	if (!equal(BASE1, BASE2))
		return 0;

	if (isdouble(BASE2))
		BASE1 = BASE2; // if mixed rational and double, use double

	push_symbol(POWER);
	push(BASE1);
	push(EXPO1);
	push(EXPO2);
	add();
	list(3);

	stack[i] = pop();

	return 1;
}
// congruent terms are combined by adding numerical coefficients

function
combine_terms(h)
{
	var i, n;
	sort_terms(h);
	n = stack.length;
	for (i = h; i < n - 1; i++) {
		if (combine_terms_nib(i, i + 1)) {
			if (!istensor(stack[i]) && iszero(stack[i]))
				stack.splice(i, 2); // remove 2
			else
				stack.splice(i + 1, 1); // remove 1
			i--; // use same index again
			n = stack.length;
		}
	}
}
function
combine_terms_nib(i, j)
{
	var coeff1, coeff2, denorm, p1, p2;

	p1 = stack[i];
	p2 = stack[j];

	if (istensor(p1) && istensor(p2)) {
		add_tensors(p1, p2);
		stack[i] = pop();
		return 1;
	}

	if (istensor(p1) || istensor(p2))
		stopf("incompatible tensor arithmetic");

	if (iszero(p2))
		return 1;

	if (iszero(p1)) {
		stack[i] = p2;
		return 1;
	}

	if (isnum(p1) && isnum(p2)) {
		add_numbers(p1, p2);
		stack[i] = pop();
		return 1;
	}

	if (isnum(p1) || isnum(p2))
		return 0; // cannot add number and something else

	coeff1 = one;
	coeff2 = one;

	denorm = 0;

	if (car(p1) == symbol(MULTIPLY)) {
		p1 = cdr(p1);
		denorm = 1;
		if (isnum(car(p1))) {
			coeff1 = car(p1);
			p1 = cdr(p1);
			if (cdr(p1) == symbol(NIL)) {
				p1 = car(p1);
				denorm = 0;
			}
		}
	}

	if (car(p2) == symbol(MULTIPLY)) {
		p2 = cdr(p2);
		if (isnum(car(p2))) {
			coeff2 = car(p2);
			p2 = cdr(p2);
			if (cdr(p2) == symbol(NIL))
				p2 = car(p2);
		}
	}

	if (!equal(p1, p2))
		return 0;

	add_numbers(coeff1, coeff2);

	coeff1 = pop();

	if (iszero(coeff1)) {
		stack[i] = coeff1;
		return 1;
	}

	if (isplusone(coeff1) && !isdouble(coeff1)) {
		if (denorm) {
			push_symbol(MULTIPLY);
			push(p1);
			cons();
		} else
			push(p1);
	} else {
		if (denorm) {
			push_symbol(MULTIPLY);
			push(coeff1);
			push(p1);
			cons();
			cons();
		} else {
			push_symbol(MULTIPLY);
			push(coeff1);
			push(p1);
			list(3);
		}
	}

	stack[i] = pop();

	return 1;
}
function
compare_numbers(p1, p2)
{
	var d1, d2;

	if (isrational(p1) && isrational(p2))
		return compare_rationals(p1, p2);

	push(p1);
	d1 = pop_double();

	push(p2);
	d2 = pop_double();

	return d1 - d2;
}
function
compare_rationals(p1, p2)
{
	var p3;

	subtract_rationals(p1, p2);

	p3 = pop();

	if (isrational(p3))
		return p3.a;
	else
		return p3.d;
}
function
compare_strings(s1, s2)
{
	if (s1 === s2)
		return 0;

	if (s1 < s2)
		return -1;
	else
		return 1;
}
function
compatible_dimensions(p1, p2)
{
	var i, n;

	if (!istensor(p1) && !istensor(p2))
		return 1; // both are scalars

	if (!istensor(p1) || !istensor(p2))
		return 0; // scalar and tensor

	n = p1.dim.length;

	if (n != p2.dim.length)
		return 0;

	for (i = 0; i < n; i++)
		if (p1.dim[i] != p2.dim[i])
			return 0;

	return 1;
}
function
conj()
{
	conj_subst();
	evalf();
}
function
conj_subst()
{
	var h, i, n, p1;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1.elem.length;
		for (i = 0; i < n; i++) {
			push(p1.elem[i]);
			conj_subst();
			p1.elem[i] = pop();
		}
		push(p1);
		return;
	}

	// (-1) ^ expr

	if (car(p1) == symbol(POWER) && isminusone(cadr(p1))) {
		push_symbol(POWER);
		push_integer(-1);
		push(caddr(p1));
		negate();
		list(3);
		return;
	}

	if (iscons(p1)) {
		h = stack.length;
		push(car(p1));
		p1 = cdr(p1);
		while (iscons(p1)) {
			push(car(p1));
			conj_subst();
			p1 = cdr(p1);
		}
		list(stack.length - h);
		return;
	}

	push(p1);
}
function
cons()
{
	var p1, p2;
	p2 = pop();
	p1 = pop();
	push({car:p1, cdr:p2});
}
const ABS = "abs";
const ADJ = "adj";
const AND = "and";
const ARCCOS = "arccos";
const ARCCOSH = "arccosh";
const ARCSIN = "arcsin";
const ARCSINH = "arcsinh";
const ARCTAN = "arctan";
const ARCTANH = "arctanh";
const ARG = "arg";
const BINDING = "binding";
const CHECK = "check";
const CIRCEXP = "circexp";
const CLEAR = "clear";
const CLOCK = "clock";
const CONJ = "conj";
const CONTRACT = "contract";
const COS = "cos";
const COSH = "cosh";
const DEFINT = "defint";
const DENOMINATOR = "denominator";
const DERIVATIVE = "derivative";
const DET = "det";
const DO = "do";
const DOT = "dot";
const EVAL = "eval";
const EXP = "exp";
const EXPCOS = "expcos";
const EXPCOSH = "expcosh";
const EXPSIN = "expsin";
const EXPSINH = "expsinh";
const EXPTAN = "exptan";
const EXPTANH = "exptanh";
const FACTORIAL = "factorial";
const FLOAT = "float";
const FLOOR = "floor";
const FOR = "for";
const IMAG = "imag";
const INNER = "inner";
const INTEGRAL = "integral";
const INV = "inv";
const LOG = "log";
const MAG = "mag";
const NIL = "nil";
const NOT = "not";
const NUMBER = "number";
const NUMERATOR = "numerator";
const OR = "or";
const OUTER = "outer";
const POLAR = "polar";
const PREFIXFORM = "prefixform";
const PRINT = "print";
const PRODUCT = "product";
const QUOTE = "quote";
const RANK = "rank";
const RATIONALIZE = "rationalize";
const REAL = "real";
const RECT = "rect";
const SGN = "sgn";
const SIN = "sin";
const SINH = "sinh";
const SQRT = "sqrt";
const STOP = "stop";
const SUM = "sum";
const TAN = "tan";
const TANH = "tanh";
const TEST = "test";
const TESTEQ = "testeq";
const TESTGE = "testge";
const TESTGT = "testgt";
const TESTLE = "testle";
const TESTLT = "testlt";
const TRANSPOSE = "transpose";
const UNIT = "unit";
const ZERO = "zero";

const ADD = "+";
const MULTIPLY = "*";
const POWER = "^";
const INDEX = "[";
const SETQ = "=";

const EXP1 = "(e)";
const METAA = "(a)";
const METAB = "(b)";
const METAX = "(x)";
const SPECX = "(X)";

const LAST = "last";
const PI = "pi";
const TRACE = "trace";

const SYMBOL_D = "d";
const SYMBOL_I = "i";
const SYMBOL_J = "j";
const SYMBOL_S = "s";
const SYMBOL_T = "t";
const SYMBOL_X = "x";
const SYMBOL_Y = "y";
const SYMBOL_Z = "z";
function
contract()
{
	var h, i, j, k, m, n, ncol, ndim, nelem, nrow, p1, p2, p3;
	var index = [];

	p3 = pop();
	p2 = pop();
	p1 = pop();

	if (!istensor(p1))
		stopf("contract: tensor expected");

	ndim = p1.dim.length;

	push(p2);
	n = pop_integer();

	push(p3);
	m = pop_integer();

	if (n < 1 || n > ndim || m < 1 || m > ndim || n == m)
		stopf("contract: index err");

	n--; // make zero based
	m--;

	ncol = p1.dim[n];
	nrow = p1.dim[m];

	if (ncol != nrow)
		stopf("contract: index err");

	// nelem is the number of elements in result

	nelem = p1.elem.length / ncol / nrow;

	p2 = alloc_tensor();

	for (i = 0; i < ndim; i++)
		index[i] = 0;

	for (i = 0; i < nelem; i++) {

		for (j = 0; j < ncol; j++) {
			index[n] = j;
			index[m] = j;
			k = index[0];
			for (h = 1; h < ndim; h++)
				k = k * p1.dim[h] + index[h];
			push(p1.elem[k]);
		}

		add_terms(ncol);

		p2.elem[i] = pop();

		// increment index

		for (j = ndim - 1; j >= 0; j--) {
			if (j == n || j == m)
				continue;
			if (++index[j] < p1.dim[j])
				break;
			index[j] = 0;
		}
	}

	if (nelem == 1) {
		push(p2.elem[0]);
		return;
	}

	// add dim info

	k = 0;

	for (i = 0; i < ndim; i++)
		if (i != n && i != m)
			p2.dim[k++] = p1.dim[i];

	push(p2);
}
function
copy_tensor(p1)
{
	var i, n, p2;

	p2 = alloc_tensor();

	n = p1.dim.length;

	for (i = 0; i < n; i++)
		p2.dim[i] = p1.dim[i];

	n = p1.elem.length;

	for (i = 0; i < n; i++)
		p2.elem[i] = p1.elem[i];

	return p2;
}
function
cos()
{
	var n, p1, p2, X, Y;

	p1 = pop();

	if (isdouble(p1)) {
		push_double(Math.cos(p1.d));
		return;
	}

	// cos(z) = 1/2 (exp(i z) + exp(-i z))

	if (isdoublez(p1)) {
		push_double(0.5);
		push(imaginaryunit);
		push(p1);
		multiply();
		exp();
		push(imaginaryunit);
		negate();
		push(p1);
		multiply();
		exp();
		add();
		multiply();
		return;
	}

	// cos(-x) = cos(x)

	if (isnegative(p1)) {
		push(p1);
		negate();
		cos();
		return;
	}

	if (car(p1) == symbol(ADD)) {
		cos_sum(p1);
		return;
	}

	// cos(arctan(y,x)) = x (x^2 + y^2)^(-1/2)

	if (car(p1) == symbol(ARCTAN)) {
		X = caddr(p1);
		Y = cadr(p1);
		push(X);
		push(X);
		push(X);
		multiply();
		push(Y);
		push(Y);
		multiply();
		add();
		push_rational(-1, 2);
		power();
		multiply();
		return;
	}

	// multiply by 180/pi

	push(p1); // nonnegative by code above
	push_integer(180);
	multiply();
	push_symbol(PI);
	divide();
	p2 = pop()

	if (isrational(p2))
		n = p2.a / p2.b;
	else if (isdouble(p2))
		n = p2.d;
	else
		n = 1; // force default case

	switch (n % 360) {
	case 90:
	case 270:
		push_integer(0);
		break;
	case 60:
	case 300:
		push_rational(1, 2);
		break;
	case 120:
	case 240:
		push_rational(-1, 2);
		break;
	case 45:
	case 315:
		push_rational(1, 2);
		push_integer(2);
		push_rational(1, 2);
		power();
		multiply();
		break;
	case 135:
	case 225:
		push_rational(-1, 2);
		push_integer(2);
		push_rational(1, 2);
		power();
		multiply();
		break;
	case 30:
	case 330:
		push_rational(1, 2);
		push_integer(3);
		push_rational(1, 2);
		power();
		multiply();
		break;
	case 150:
	case 210:
		push_rational(-1, 2);
		push_integer(3);
		push_rational(1, 2);
		power();
		multiply();
		break;
	case 0:
		push_integer(1);
		break;
	case 180:
		push_integer(-1);
		break;
	default:
		push_symbol(COS);
		push(p1);
		list(2);
		break;
	}
}
function
cos_sum(p1) // cos(x + n/2 pi) = cos(x) cos(n/2 pi) - sin(x) sin(n/2 pi)
{
	var p2, p3;
	p2 = cdr(p1);
	while (iscons(p2)) {
		push_integer(2);
		push(car(p2));
		multiply();
		push_symbol(PI);
		divide();
		p3 = pop();
		if (isinteger(p3)) {
			push(p1);
			push(car(p2));
			subtract();
			p3 = pop();
			push(p3);
			cos();
			push(car(p2));
			cos();
			multiply();
			push(p3);
			sin();
			push(car(p2));
			sin();
			multiply();
			subtract();
			return;
		}
		p2 = cdr(p2);
	}
	push_symbol(COS);
	push(p1);
	list(2);
}
function
cosh()
{
	var p1 = pop();

	// cosh(z) = 1/2 (exp(z) + exp(-z))

	if (isdouble(p1) || isdoublez(p1)) {
		push_rational(1, 2);
		push(p1);
		exp();
		push(p1);
		negate();
		exp();
		add();
		multiply();
		return;
	}

	if (iszero(p1)) {
		push_integer(1);
		return;
	}

	// cosh(-x) = cosh(x)

	if (isnegative(p1)) {
		push(p1);
		negate();
		cosh();
		return;
	}

	if (car(p1) == symbol(ARCCOSH)) {
		push(cadr(p1));
		return;
	}

	push_symbol(COSH);
	push(p1);
	list(2);
}
function
dabs(p1, p2)
{
	push(cadr(p1));
	push(p2);
	derivative();
	push(cadr(p1));
	sgn();
	multiply();
}
function
darccos(p1, p2)
{
	push(cadr(p1));
	push(p2);
	derivative();
	push_integer(1);
	push(cadr(p1));
	push_integer(2);
	power();
	subtract();
	push_rational(-1, 2);
	power();
	multiply();
	negate();
}
function
darccosh(p1, p2)
{
	push(cadr(p1));
	push(p2);
	derivative();
	push(cadr(p1));
	push_integer(2);
	power();
	push_integer(-1);
	add();
	push_rational(-1, 2);
	power();
	multiply();
}
function
darcsin(p1, p2)
{
	push(cadr(p1));
	push(p2);
	derivative();
	push_integer(1);
	push(cadr(p1));
	push_integer(2);
	power();
	subtract();
	push_rational(-1, 2);
	power();
	multiply();
}
function
darcsinh(p1, p2)
{
	push(cadr(p1));
	push(p2);
	derivative();
	push(cadr(p1));
	push_integer(2);
	power();
	push_integer(1);
	add();
	push_rational(-1, 2);
	power();
	multiply();
}
function
darctan(p1, p2)
{
	push(cadr(p1));
	push(p2);
	derivative();
	push_integer(1);
	push(cadr(p1));
	push_integer(2);
	power();
	add();
	reciprocate();
	multiply();
}
function
darctanh(p1, p2)
{
	push(cadr(p1));
	push(p2);
	derivative();
	push_integer(1);
	push(cadr(p1));
	push_integer(2);
	power();
	subtract();
	reciprocate();
	multiply();
}
function
dcos(F, X)
{
	push(cadr(F));
	push(X);
	derivative();
	push(cadr(F));
	sin();
	multiply();
	negate();
}
function
dcosh(p1, p2)
{
	push(cadr(p1));
	push(p2);
	derivative();
	push(cadr(p1));
	sinh();
	multiply();
}
function
dd(p1, p2)
{
	var p3;

	// d(f(x,y),x)

	push(cadr(p1));
	push(p2);
	derivative();

	p3 = pop();

	if (car(p3) == symbol(DERIVATIVE)) {

		// sort dx terms

		push_symbol(DERIVATIVE);
		push_symbol(DERIVATIVE);
		push(cadr(p3));

		if (lessp(caddr(p3), caddr(p1))) {
			push(caddr(p3));
			list(3);
			push(caddr(p1));
		} else {
			push(caddr(p1));
			list(3);
			push(caddr(p3));
		}

		list(3);

	} else {
		push(p3);
		push(caddr(p1));
		derivative();
	}
}
function
decomp()
{
	var p1, F, X;

	X = pop();
	F = pop();

	// is the entire expression constant?

	if (!findf(F, X)) {
		push(F);
		return;
	}

	// sum?

	if (car(F) == symbol(ADD)) {
		decomp_sum(F, X);
		return;
	}

	// product?

	if (car(F) == symbol(MULTIPLY)) {
		decomp_product(F, X);
		return;
	}

	// naive decomp if not sum or product

	p1 = cdr(F);
	while (iscons(p1)) {
		push(car(p1));
		push(X);
		decomp();
		p1 = cdr(p1);
	}
}
function
decomp_product(F, X)
{
	var h, n, p1;

	// decomp factors involving x

	p1 = cdr(F);
	while (iscons(p1)) {
		if (findf(car(p1), X)) {
			push(car(p1));
			push(X);
			decomp();
		}
		p1 = cdr(p1);
	}

	// multiply together all constant factors

	h = stack.length;
	p1 = cdr(F);
	while (iscons(p1)) {
		if (!findf(car(p1), X))
			push(car(p1));
		p1 = cdr(p1);
	}

	n = stack.length - h;

	if (n > 1)
		multiply_factors(n);
}
function
decomp_sum(F, X)
{
	var h, n, p1;

	// decomp terms involving x

	p1 = cdr(F);
	while (iscons(p1)) {
		if (findf(car(p1), X)) {
			push(car(p1));
			push(X);
			decomp();
		}
		p1 = cdr(p1);
	}

	// add together all constant terms

	h = stack.length;
	p1 = cdr(F);
	while (iscons(p1)) {
		if (!findf(car(p1), X))
			push(car(p1));
		p1 = cdr(p1);
	}

	n = stack.length - h;

	if (n) {
		add_terms(n);
		p1 = pop();
		push(p1);
		push(p1);
		negate(); // need both +a, -a for some integrals
	}
}
function
denominator()
{
	var p0, p1, p2;

	p1 = pop();

	if (isrational(p1)) {
		push_integer(p1.b);
		return;
	}

	p2 = one; // p2 is denominator

	while (divisor(p1)) {

		p0 = pop(); // p0 is a denominator

		push(p0); // cancel in orig expr
		push(p1);
		cancel_factor();
		p1 = pop();

		push(p0); // update denominator
		push(p2);
		cancel_factor();
		p2 = pop();
	}

	push(p2);
}
function
derivative()
{
	var F, X;

	X = pop();
	F = pop();

	if (istensor(F)) {
		if (istensor(X))
			dtt(F, X);
		else
			dts(F, X);
	} else {
		if (istensor(X))
			dst(F, X);
		else
			dss(F, X);
	}
}
function
det()
{
	var h, i, j, k, m, n, p1, p2;

	p1 = pop();

	if (!istensor(p1) || p1.dim.length != 2 || p1.dim[0] != p1.dim[1])
		stopf("det: square matrix expected");

	n = p1.dim[0];

	switch (n) {
	case 1:
		push(p1.elem[0]);
		return;
	case 2:
		push(p1.elem[0]);
		push(p1.elem[3]);
		multiply();
		push(p1.elem[1]);
		push(p1.elem[2]);
		multiply();
		subtract();
		return;
	}

	p2 = alloc_tensor();

	p2.dim[0] = n - 1;
	p2.dim[1] = n - 1;

	h = stack.length;

	for (m = 0; m < n; m++) {
		if (iszero(p1.elem[m]))
			continue;
		k = 0;
		for (i = 1; i < n; i++)
			for (j = 0; j < n; j++)
				if (j != m)
					p2.elem[k++] = p1.elem[n * i + j];
		push(p2);
		det();
		push(p1.elem[m]);
		multiply();
		if (m % 2)
			negate();
	}

	n = stack.length - h;

	if (n == 0)
		push_integer(0);
	else
		add_terms(n);
}
function
dfunction(p1, p2)
{
	var p3 = cdr(p1); // argument list

	if (p3 == symbol(NIL) || findf(p3, p2)) {
		push_symbol(DERIVATIVE);
		push(p1);
		push(p2);
		list(3);
		return;
	}

	push_integer(0);
}
function
divide()
{
	if (isnum(stack[stack.length - 2]) && isnum(stack[stack.length - 1])) {
		var p2 = pop();
		var p1 = pop();
		divide_numbers(p1, p2);
		return;
	}

	reciprocate();
	multiply();
}
function
divide_numbers(p1, p2)
{
	var a, b;

	if (isrational(p1) && isrational(p2)) {
		divide_rationals(p1, p2);
		return;
	}

	push(p1);
	a = pop_double();

	push(p2);
	b = pop_double();

	push_double(a / b);
}
function
divide_rationals(p1, p2)
{
	var a, b, c;

	a = p1.a * p2.b;
	b = p1.b * p2.a;

	if (b < 0) {
		a = -a;
		b = -b;
	}

	if (a == 0) {
		push_integer(0);
		return;
	}

	if (Math.abs(a) > 1e10 || b > 1e10) {
		push_double(a / b);
		return;
	}

	if (b > 1) {
		c = gcd_integers(a, b)
		a /= c;
		b /= c;
	}

	push_rational(a, b);
}
function
divisor(p)
{
	if (car(p) == symbol(ADD)) {
		p = cdr(p);
		while (iscons(p)) {
			if (divisor_term(car(p)))
				return 1;
			p = cdr(p);
		}
		return 0;
	}

	return divisor_term(p);
}
function
divisor_factor(p)
{
	if (isrational(p)) {
		if (isinteger(p))
			return 0;
		push_integer(p.b);
		return 1;
	}

	if (car(p) == symbol(POWER) && !isminusone(cadr(p)) && isnegativeterm(caddr(p))) {
		if (isminusone(caddr(p)))
			push(cadr(p));
		else {
			push_symbol(POWER);
			push(cadr(p));
			push(caddr(p));
			negate();
			list(3);
		}
		return 1;
	}

	return 0;
}
function
divisor_term(p)
{
	if (car(p) == symbol(MULTIPLY)) {
		p = cdr(p);
		while (iscons(p)) {
			if (divisor_factor(car(p)))
				return 1;
			p = cdr(p);
		}
		return 0;
	}

	return divisor_factor(p);
}
function
dlog(p1, p2)
{
	push(cadr(p1));
	push(p2);
	derivative();
	push(cadr(p1));
	divide();
}
//	     v
//	y = u
//
//	log y = v log u
//
//	1 dy   v du           dv
//	- -- = - -- + (log u) --
//	y dx   u dx           dx
//
//	dy    v  v du           dv
//	-- = u  (- -- + (log u) --)
//	dx       u dx           dx

function
dpower(F, X)
{
	if (isnum(cadr(F)) && isnum(caddr(F))) {
		push_integer(0); // irr or imag
		return;
	}

	push(caddr(F));		// v/u
	push(cadr(F));
	divide();

	push(cadr(F));		// du/dx
	push(X);
	derivative();

	multiply();

	push(cadr(F));		// log u
	log();

	push(caddr(F));		// dv/dx
	push(X);
	derivative();

	multiply();

	add();
	push(F);		// u^v
	multiply();
}
function
dproduct(p1, p2)
{
	var i, j, n, p3;

	n = lengthf(p1) - 1;

	for (i = 0; i < n; i++) {

		p3 = cdr(p1);

		for (j = 0; j < n; j++) {
			push(car(p3));
			if (i == j) {
				push(p2);
				derivative();
			}
			p3 = cdr(p3);
		}

		multiply_factors(n);
	}

	add_terms(n);
}
function
dsin(F, X)
{
	push(cadr(F));
	push(X);
	derivative();
	push(cadr(F));
	cos();
	multiply();
}
function
dsinh(p1, p2)
{
	push(cadr(p1));
	push(p2);
	derivative();
	push(cadr(p1));
	cosh();
	multiply();
}
function
dss(F, X)
{
	if (equal(F, X)) {
		push_integer(1); // d(x,x)
		return;
	}

	if (!iscons(F)) {
		push_integer(0); // d(const,x)
		return;
	}

	if (car(F) == symbol(ADD)) {
		dsum(F, X);
		return;
	}

	if (car(F) == symbol(MULTIPLY)) {
		dproduct(F, X);
		return;
	}

	if (car(F) == symbol(POWER)) {
		dpower(F, X);
		return;
	}

	if (car(F) == symbol(DERIVATIVE)) {
		dd(F, X);
		return;
	}

	if (car(F) == symbol(LOG)) {
		dlog(F, X);
		return;
	}

	if (car(F) == symbol(SIN)) {
		dsin(F, X);
		return;
	}

	if (car(F) == symbol(COS)) {
		dcos(F, X);
		return;
	}

	if (car(F) == symbol(TAN)) {
		dtan(F, X);
		return;
	}

	if (car(F) == symbol(ARCSIN)) {
		darcsin(F, X);
		return;
	}

	if (car(F) == symbol(ARCCOS)) {
		darccos(F, X);
		return;
	}

	if (car(F) == symbol(ARCTAN)) {
		darctan(F, X);
		return;
	}

	if (car(F) == symbol(SINH)) {
		dsinh(F, X);
		return;
	}

	if (car(F) == symbol(COSH)) {
		dcosh(F, X);
		return;
	}

	if (car(F) == symbol(TANH)) {
		dtanh(F, X);
		return;
	}

	if (car(F) == symbol(ARCSINH)) {
		darcsinh(F, X);
		return;
	}

	if (car(F) == symbol(ARCCOSH)) {
		darccosh(F, X);
		return;
	}

	if (car(F) == symbol(ARCTANH)) {
		darctanh(F, X);
		return;
	}

	if (car(F) == symbol(ABS)) {
		dabs(F, X);
		return;
	}

	if (car(F) == symbol(INTEGRAL) && caddr(F) == X) {
		push(cadr(F));
		return;
	}

	dfunction(F, X);
}
function
dst(p1, p2)
{
	var i, n, p3;

	p3 = copy_tensor(p2);

	n = p2.elem.length;

	for (i = 0; i < n; i++) {
		push(p1);
		push(p2.elem[i]);
		derivative();
		p3.elem[i] = pop();
	}

	push(p3);
}
function
dsum(p1, p2)
{
	var h = stack.length;

	p1 = cdr(p1);

	while (iscons(p1)) {
		push(car(p1));
		push(p2);
		derivative();
		p1 = cdr(p1);
	}

	add_terms(stack.length - h);
}
function
dtan(p1, p2)
{
	push(cadr(p1));
	push(p2);
	derivative();
	push(cadr(p1));
	cos();
	push_integer(-2);
	power();
	multiply();
}
function
dtanh(p1, p2)
{
	push(cadr(p1));
	push(p2);
	derivative();
	push(cadr(p1));
	cosh();
	push_integer(-2);
	power();
	multiply();
}
function
dts(p1, p2)
{
	var i, n, p3;

	p3 = copy_tensor(p1);

	n = p1.elem.length;

	for (i = 0; i < n; i++) {
		push(p1.elem[i]);
		push(p2);
		derivative();
		p3.elem[i] = pop();
	}

	push(p3);
}
function
dtt(p1, p2)
{
	var i, j, m, n, p3;

	p3 = alloc_tensor();

	n = p1.elem.length;
	m = p2.elem.length;

	for (i = 0; i < n; i++) {
		for (j = 0; j < m; j++) {
			push(p1.elem[i]);
			push(p2.elem[j]);
			derivative();
			p3.elem[m * i + j] = pop();
		}
	}

	p3.dim = p1.dim.concat(p2.dim);

	push(p3);
}
function
equal(p1, p2)
{
	if (p1 == p2)
		return 1;

	if (iscons(p1) && iscons(p2)) {
		while (iscons(p1) && iscons(p2)) {
			if (!equal(car(p1), car(p2)))
				return 0;
			p1 = cdr(p1);
			p2 = cdr(p2);
		}
		return p1 == symbol(NIL) && p2 == symbol(NIL);
	}

	if (isrational(p1) && isrational(p2))
		return p1.a == p2.a && p1.b == p2.b;

	if (isrational(p1) && isdouble(p2))
		return p1.a / p1.b == p2.d;

	if (isdouble(p1) && isrational(p2))
		return p1.d == p2.a / p2.b;

	if (isdouble(p1) && isdouble(p1))
		return p1.d == p2.d;

	if (issymbol(p1) && issymbol(p2))
		return p1.printname == p2.printname;

	if (isstring(p1) && isstring(p2))
		return p1.string == p2.string;

	return 0; // no need to compare tensors
}
function
equaln(p, n)
{
	if (isrational(p))
		return p.a == n && p.b == 1;
	else if (isdouble(p))
		return p.d == n;
	else
		return 0;
}
function
equalq(p, a, b)
{
	if (isrational(p))
		return p.a == a && p.b == b;
	else if (isdouble(p))
		return p.d == a / b;
	else
		return 0;
}
function
eval_abs(p1)
{
	push(cadr(p1));
	evalf();
	abs();
}
function
eval_add(p1)
{
	var h = stack.length;
	p1 = cdr(p1);
	while (iscons(p1)) {
		push(car(p1));
		evalf();
		p1 = cdr(p1);
	}
	add_terms(stack.length - h);
}
function
eval_adj(p1)
{
	push(cadr(p1));
	evalf();
	adj();
}
function
eval_and(p1)
{
	var p2;
	p1 = cdr(p1);
	while (iscons(p1)) {
		push(car(p1));
		evalp();
		p2 = pop();
		if (iszero(p2)) {
			push_integer(0);
			return;
		}
		p1 = cdr(p1);
	}
	push_integer(1);
}
function
eval_and_print_result()
{
	var p1, p2;

	p1 = pop();
	push(p1);
	evalf();
	p2 = pop();

	push(p1);
	push(p2);
	print_result();

	if (p2 != symbol(NIL))
		set_binding(symbol(LAST), p2);
}
function
eval_arccos(p1)
{
	push(cadr(p1));
	evalf();
	arccos();
}
function
eval_arccosh(p1)
{
	push(cadr(p1));
	evalf();
	arccosh();
}
function
eval_arcsin(p1)
{
	push(cadr(p1));
	evalf();
	arcsin();
}
function
eval_arcsinh(p1)
{
	push(cadr(p1));
	evalf();
	arcsinh();
}
function
eval_arctan(p1)
{
	push(cadr(p1));
	evalf();
	if (caddr(p1) == symbol(NIL))
		push_integer(1);
	else {
		push(caddr(p1));
		evalf();
	}
	arctan();
}
function
eval_arctanh(p1)
{
	push(cadr(p1));
	evalf();
	arctanh();
}
function
eval_arg(p1)
{
	push(cadr(p1));
	evalf();
	arg();
}
function
eval_binding(p1)
{
	push(get_binding(cadr(p1)));
}
function
eval_check(p1)
{
	push(cadr(p1));
	evalp();
	p1 = pop();
	if (iszero(p1))
		stopf("check");
	push_symbol(NIL); // no result is printed
}
function
eval_circexp(p1)
{
	push(cadr(p1));
	evalf();
	circexp();
}
function
eval_clear(p1)
{
	p1; // suppress eslint warning

	save_binding(symbol(TRACE));

	binding = {};
	arglist = {};

	initscript();

	restore_binding(symbol(TRACE));

	push_symbol(NIL);
}
function
eval_clock(p1)
{
	push(cadr(p1));
	evalf();
	clock();
}
function
eval_conj(p1)
{
	push(cadr(p1));
	evalf();
	conj();
}
function
eval_contract(p1)
{
	push(cadr(p1));
	evalf();
	p1 = cddr(p1);
	if (iscons(p1)) {
		push(car(p1));
		evalf();
		push(cadr(p1));
		evalf();
	} else {
		push_integer(1);
		push_integer(2);
	}
	contract();
}
function
eval_cos(p1)
{
	push(cadr(p1));
	evalf();
	cos();
}
function
eval_cosh(p1)
{
	push(cadr(p1));
	evalf();
	cosh();
}
function
eval_defint(p1)
{
	var A, B, F, X;

	expanding++; // in case integral is in denominator

	push(cadr(p1));
	evalf();
	F = pop();

	p1 = cddr(p1);

	do {
		if (lengthf(p1) < 3)
			stopf("defint: missing argument");

		push(car(p1));
		p1 = cdr(p1);
		evalf();
		X = pop();

		push(car(p1));
		p1 = cdr(p1);
		evalf();
		A = pop();

		push(car(p1));
		p1 = cdr(p1);
		evalf();
		B = pop();

		push(F);
		push(X);
		integral();
		F = pop();

		push(F);
		push(X);
		push(B);
		subst();
		evalf();

		push(F);
		push(X);
		push(A);
		subst();
		evalf();

		subtract();
		F = pop();

	} while (iscons(p1));

	push(F);

	expanding--;
}
function
eval_denominator(p1)
{
	push(cadr(p1));
	evalf();
	denominator();
}
function
eval_derivative(p1)
{
	var i, n, flag, X, Y;

	push(cadr(p1));
	evalf();
	p1 = cddr(p1);

	if (!iscons(p1)) {
		guess();
		derivative();
		return;
	}

	flag = 0;

	while (iscons(p1) || flag) {

		if (flag) {
			X = Y;
			flag = 0;
		} else {
			push(car(p1));
			evalf();
			X = pop();
			p1 = cdr(p1);
		}

		if (isnum(X)) {
			push(X);
			n = pop_integer();
			guess();
			X = pop();
			for (i = 0; i < n; i++) {
				push(X);
				derivative();
			}
			continue;
		}

		if (iscons(p1)) {

			push(car(p1));
			evalf();
			Y = pop();
			p1 = cdr(p1);

			if (isnum(Y)) {
				push(Y);
				n = pop_integer();
				for (i = 0; i < n; i++) {
					push(X);
					derivative();
				}
				continue;
			}

			flag = 1;
		}

		push(X);
		derivative();
	}
}
function
eval_det(p1)
{
	push(cadr(p1));
	evalf();
	det();
}
function
eval_do(p1)
{
	push_integer(0);
	p1 = cdr(p1);
	while (iscons(p1)) {
		pop();
		push(car(p1));
		evalf();
		p1 = cdr(p1);
	}
}
function
eval_dot(p1)
{
	eval_inner(p1);
}
function
eval_eval(p1)
{
	push(cadr(p1));
	evalf();
	p1 = cddr(p1);
	while (iscons(p1)) {
		push(car(p1));
		evalf();
		push(cadr(p1));
		evalf();
		subst();
		p1 = cddr(p1);
	}
	evalf();
}
function
eval_exp(p1)
{
	push(cadr(p1));
	evalf();
	exp();
}
function
eval_expcos(p1)
{
	push(cadr(p1));
	evalf();
	expcos();
}
function
eval_expcosh(p1)
{
	push(cadr(p1));
	evalf();
	expcosh();
}
function
eval_expsin(p1)
{
	push(cadr(p1));
	evalf();
	expsin();
}
function
eval_expsinh(p1)
{
	push(cadr(p1));
	evalf();
	expsinh();
}
function
eval_exptan(p1)
{
	push(cadr(p1));
	evalf();
	exptan();
}
function
eval_exptanh(p1)
{
	push(cadr(p1));
	evalf();
	exptanh();
}
function
eval_factorial(p1)
{
	push(cadr(p1));
	evalf();
	factorial();
}
function
eval_float(p1)
{
	push(cadr(p1));
	evalf();
	floatf();
}
function
eval_floor(p1)
{
	push(cadr(p1));
	evalf();
	p1 = pop();

	if (isrational(p1)) {
		push_integer(Math.floor(p1.a / p1.b));
		return;
	}

	if (isdouble(p1)) {
		push_double(Math.floor(p1.d));
		return;
	}

	push_symbol(FLOOR);
	push(p1);
	list(2);
}
function
eval_for(p1)
{
	var j, k, p2, p3;

	p2 = cadr(p1);

	if (!isusersymbol(p2))
		stopf("symbol expected");

	p1 = cddr(p1);

	push(car(p1));
	evalf();
	j = pop_integer();

	push(cadr(p1));
	evalf();
	k = pop_integer();

	p1 = cddr(p1);

	save_binding(p2);

	for (;;) {
		push_integer(j);
		p3 = pop();
		set_binding(p2, p3);
		p3 = p1;
		while (iscons(p3)) {
			push(car(p3));
			evalf();
			pop();
			p3 = cdr(p3);
		}
		if (j < k)
			j++;
		else if (j > k)
			j--;
		else
			break;
	}

	restore_binding(p2);

	push_symbol(NIL); // return value
}
function
eval_imag(p1)
{
	push(cadr(p1));
	evalf();
	imag();
}
function
eval_index(p1)
{
	var k;

	p1 = cdr(p1);

	push(car(p1));
	p1 = cdr(p1);
	evalf();

	while (iscons(p1)) {

		push(car(p1));
		p1 = cdr(p1);
		evalf();
		k = pop_integer();

		index(k);
	}
}
function
eval_inner(p1)
{
	push(cadr(p1));
	evalf();
	p1 = cddr(p1);
	while (iscons(p1)) {
		push(car(p1));
		evalf();
		inner();
		p1 = cdr(p1);
	}
}
function
eval_integral(p1)
{
	var flag, i, n, X, Y;

	expanding++;

	push(cadr(p1));
	evalf();
	p1 = cddr(p1);

	if (!iscons(p1)) {
		guess();
		integral();
		expanding--;
		return;
	}

	flag = 0;

	while (iscons(p1) || flag) {

		if (flag) {
			X = Y;
			flag = 0;
		} else {
			push(car(p1));
			evalf();
			X = pop();
			p1 = cdr(p1);
		}

		if (isnum(X)) {
			push(X);
			n = pop_integer();
			guess();
			X = pop();
			for (i = 0; i < n; i++) {
				push(X);
				integral();
			}
			continue;
		}

		if (iscons(p1)) {

			push(car(p1));
			evalf();
			Y = pop();
			p1 = cdr(p1);

			if (isnum(Y)) {
				push(Y);
				n = pop_integer();
				for (i = 0; i < n; i++) {
					push(X);
					integral();
				}
				continue;
			}

			flag = 1;
		}

		push(X);
		integral();
	}

	expanding--;
}
function
eval_inv(p1)
{
	push(cadr(p1));
	evalf();
	inv();
}
function
eval_log(p1)
{
	push(cadr(p1));
	evalf();
	log();
}
function
eval_mag(p1)
{
	push(cadr(p1));
	evalf();
	mag();
}
function
eval_multiply(p1)
{
	var h = stack.length;

	p1 = cdr(p1);

	while (iscons(p1)) {
		push(car(p1));
		evalf();
		p1 = cdr(p1);
	}

	multiply_factors(stack.length - h);
}
function
eval_nil(p1)
{
	p1; // suppress eslint warning
	push_symbol(NIL);
}
function
eval_not(p1)
{
	push(cadr(p1));
	evalp();
	p1 = pop();
	if (iszero(p1))
		push_integer(1);
	else
		push_integer(0);
}
function
eval_number(p1)
{
	push(cadr(p1));
	evalf();
	p1 = pop();

	if (isnum(p1))
		push_integer(1);
	else
		push_integer(0);
}
function
eval_numerator(p1)
{
	push(cadr(p1));
	evalf();
	numerator();
}
function
eval_or(p1)
{
	var p2;
	p1 = cdr(p1);
	while (iscons(p1)) {
		push(car(p1));
		evalp();
		p2 = pop();
		if (!iszero(p2)) {
			push_integer(1);
			return;
		}
		p1 = cdr(p1);
	}
	push_integer(0);
}
function
eval_outer(p1)
{
	push(cadr(p1));
	evalf();
	p1 = cddr(p1);
	while (iscons(p1)) {
		push(car(p1));
		evalf();
		outer();
		p1 = cdr(p1);
	}
}
function
eval_polar(p1)
{
	push(cadr(p1));
	evalf();
	polar();
}
function
eval_power(p1)
{
	var t, p2;

	push(caddr(p1)); // exponent
	evalf();
	p2 = pop();

	if (isnegativenumber(p2)) {
		// don't expand in denominators
		t = expanding;
		expanding = 0;
		push(cadr(p1)); // base
		evalf();
		push(p2);
		power();
		expanding = t;
	} else {
		push(cadr(p1)); // base
		evalf();
		push(p2);
		power();
	}
}
function
eval_prefixform(p1)
{
	var s;
	push(cadr(p1));
	evalf();
	p1 = pop();
	s = prefixform(p1);
	push_string(s);
}
function
eval_print(p1)
{
	p1 = cdr(p1);
	while (iscons(p1)) {
		push(car(p1));
		push(car(p1));
		evalf();
		print_result();
		p1 = cdr(p1);
	}
	push_symbol(NIL);
}
function
eval_product(p1)
{
	var h, j, k, p2, p3;

	p2 = cadr(p1);

	if (!isusersymbol(p2))
		stopf("symbol expected");

	p1 = cddr(p1);

	push(car(p1));
	evalf();
	j = pop_integer();

	push(cadr(p1));
	evalf();
	k = pop_integer();

	p1 = caddr(p1);

	save_binding(p2);

	h = stack.length;

	for (;;) {
		push_integer(j);
		p3 = pop();
		set_binding(p2, p3);
		push(p1);
		evalf();
		if (j < k)
			j++;
		else if (j > k)
			j--;
		else
			break;
	}

	multiply_factors(stack.length - h);

	restore_binding(p2);
}
function
eval_quote(p1)
{
	push(cadr(p1)); // not evaluated
}
function
eval_rank(p1)
{
	push(cadr(p1));
	evalf();
	p1 = pop();

	if (istensor(p1))
		push_integer(p1.dim.length);
	else
		push_integer(0);
}
function
eval_rationalize(p1)
{
	push(cadr(p1));
	evalf();
	rationalize();
}
function
eval_real(p1)
{
	push(cadr(p1));
	evalf();
	real();
}
function
eval_rect(p1)
{
	push(cadr(p1));
	evalf();
	rect();
}
function
eval_setq(p1)
{
	var p2;

	push_symbol(NIL); // result

	if (caadr(p1) == symbol(INDEX)) {
		setq_indexed(p1);
		return;
	}

	if (iscons(cadr(p1))) {
		setq_userfunc(p1);
		return;
	}

	if (!isusersymbol(cadr(p1)))
		stopf("symbol expected");

	push(caddr(p1));
	evalf();
	p2 = pop();
	set_binding(cadr(p1), p2);
}
function
eval_sgn(p1)
{
	push(cadr(p1));
	evalf();
	sgn();
}
function
eval_sin(p1)
{
	push(cadr(p1));
	evalf();
	sin();
}
function
eval_sinh(p1)
{
	push(cadr(p1));
	evalf();
	sinh();
}
function
eval_sqrt(p1)
{
	push(cadr(p1));
	evalf();
	push_rational(1, 2);
	power();
}
function
eval_stop(p1)
{
	p1; // suppress eslint warning
	stopf("stop function");
}
function
eval_sum(p1)
{
	var h, j, k, p2, p3;

	p2 = cadr(p1);

	if (!isusersymbol(p2))
		stopf("symbol expected");

	p1 = cddr(p1);

	push(car(p1));
	evalf();
	j = pop_integer();

	push(cadr(p1));
	evalf();
	k = pop_integer();

	p1 = caddr(p1);

	save_binding(p2);

	h = stack.length;

	for (;;) {
		push_integer(j);
		p3 = pop();
		set_binding(p2, p3);
		push(p1);
		evalf();
		if (j < k)
			j++;
		else if (j > k)
			j--;
		else
			break;
	}

	add_terms(stack.length - h);

	restore_binding(p2);
}
function
eval_symbol(p1)
{
	var p2;

	if (iscons(p1)) {
		eval_userfunc(p1);
		return;
	}

	p2 = get_binding(p1);

	push(p2);

	if (p1 != p2)
		evalf();
}
function
eval_tan(p1)
{
	push(cadr(p1));
	evalf();
	tan();
}
function
eval_tanh(p1)
{
	push(cadr(p1));
	evalf();
	tanh();
}
function
eval_tensor(p1)
{
	var i, n;

	p1 = copy_tensor(p1);

	n = p1.elem.length;

	for (i = 0; i < n; i++) {
		push(p1.elem[i]);
		evalf();
		p1.elem[i] = pop();
	}

	push(p1);

	promote_tensor();
}
function
eval_test(p1)
{
	var p2;
	p1 = cdr(p1);
	while (iscons(p1)) {
		if (cdr(p1) == symbol(NIL)) {
			push(car(p1)); // default case
			evalf();
			return;
		}
		push(car(p1));
		evalp();
		p2 = pop();
		if (!iszero(p2)) {
			push(cadr(p1));
			evalf();
			return;
		}
		p1 = cddr(p1);
	}
	push_integer(0);
}
function
eval_testeq(p1)
{
	var i, n, p2, p3;

	push(cadr(p1));
	evalf();

	push(caddr(p1));
	evalf();

	p2 = pop();
	p1 = pop();

	// null tensors are equal no matter the dimensions

	if (iszero(p1) && iszero(p2)) {
		push_integer(1);
		return;
	}

	if (!istensor(p1) && !istensor(p2)) {
		if (testeq(p1, p2))
			push_integer(1);
		else
			push_integer(0);
		return;
	}

	if (!istensor(p1) && istensor(p2)) {
		p3 = p1;
		p1 = p2;
		p2 = p3;
	}

	if (istensor(p1) && !istensor(p2)) {
		if (!iszero(p2)) {
			push_integer(0); // tensor not equal scalar
			return;
		}
		n = p1.elem.length;
		for (i = 0; i < n; i++) {
			if (!testeq(p1.elem[i], zero)) {
				push_integer(0);
				return;
			}
		}
		push_integer(1);
		return;
	}

	// both p1 and p2 are tensors

	if (!compatible_dimensions(p1, p2)) {
		push_integer(0);
		return;
	}

	n = p1.elem.length;

	for (i = 0; i < n; i++) {
		if (!testeq(p1.elem[i], p2.elem[i])) {
			push_integer(0);
			return;
		}
	}

	push_integer(1);
}
function
eval_testge(p1)
{
	if (cmp_args(p1) >= 0)
		push_integer(1);
	else
		push_integer(0);
}
function
eval_testgt(p1)
{
	if (cmp_args(p1) > 0)
		push_integer(1);
	else
		push_integer(0);
}
function
eval_testle(p1)
{
	if (cmp_args(p1) <= 0)
		push_integer(1);
	else
		push_integer(0);
}
function
eval_testlt(p1)
{
	if (cmp_args(p1) < 0)
		push_integer(1);
	else
		push_integer(0);
}
function
eval_transpose(p1)
{
	var n, m, p2;

	push(cadr(p1));
	evalf();
	p2 = pop();
	if (!istensor(p2))
		stopf("transpose: tensor expected");
	push(p2);

	p1 = cddr(p1);

	if (!iscons(p1)) {
		transpose(1, 2);
		return;
	}

	push(car(p1));
	evalf();
	n = pop_integer();

	push(cadr(p1));
	evalf();
	m = pop_integer();

	transpose(n, m);
}
function
eval_unit(p1)
{
	var i, j, n;

	push(cadr(p1));
	evalf();
	n = pop_integer();

	if (n < 2)
		stopf("unit: index err");

	p1 = alloc_tensor();

	p1.dim[0] = n;
	p1.dim[1] = n;

	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			if (i == j)
				p1.elem[n * i + j] = one;
			else
				p1.elem[n * i + j] = zero;

	push(p1);
}
function
eval_userfunc(p1)
{
	var h, p2, F, A, B, S;

	// use "derivative" instead of "d" if there is no user function "d"

	if (car(p1) == symbol(SYMBOL_D) && get_arglist(symbol(SYMBOL_D)) == symbol(NIL)) {
		eval_derivative(p1);
		return;
	}

	F = get_binding(car(p1));
	A = get_arglist(car(p1));
	B = cdr(p1);

	// undefined function?

	if (F == car(p1)) {
		h = stack.length;
		push(F);
		p1 = B;
		while (iscons(p1)) {
			push(car(p1));
			evalf();
			p1 = cdr(p1);
		}
		list(stack.length - h);
		return;
	}

	// create the argument substitution list S

	p1 = A;
	p2 = B;
	h = stack.length;
	while (iscons(p1) && iscons(p2)) {
		push(car(p1));
		push(car(p2));
		evalf();
		p1 = cdr(p1);
		p2 = cdr(p2);
	}
	list(stack.length - h);
	S = pop();

	// evaluate the function body

	push(F);

	if (iscons(S))
		rewrite(S);

	evalf();
}
function
eval_zero(p1)
{
	var i, m, n, p2;

	p1 = cdr(p1);
	p2 = alloc_tensor();

	m = 1;

	while (iscons(p1)) {
		push(car(p1));
		evalf();
		n = pop_integer();
		if (n < 2)
			stopf("zero: index err");
		p2.dim.push(n);
		m *= n;
		p1 = cdr(p1);
	}

	for (i = 0; i < m; i++)
		p2.elem[i] = zero;

	push(p2);
}
function
evalf()
{
	var p1;

	if (++evaldepth == 1000)
		stopf("circular eval");

	p1 = pop();

	if (iscons(p1) && issymbol(car(p1)))
		car(p1).func(p1);
	else if (iskeyword(p1)) {
		// bare keyword
		push(p1);
		push_symbol(LAST);
		list(2);
		p1 = pop();
		car(p1).func(p1);
	} else if (issymbol(p1))
		eval_symbol(p1); // keyword already handled
	else if (istensor(p1))
		eval_tensor(p1);
	else
		push(p1);

	evaldepth--;
}
function
evalp()
{
	var p1 = pop();
	if (car(p1) == symbol(SETQ))
		eval_testeq(p1);
	else {
		push(p1);
		evalf();
	}
}
function
exp()
{
	push_symbol(EXP1);
	swap();
	power();
}
function
expand_sum_factors(h)
{
	var i, n, p1, p2;

	n = stack.length;

	if (n - h < 2)
		return;

	// search for a sum factor

	for (i = h; i < n; i++) {
		p2 = stack[i];
		if (car(p2) == symbol(ADD))
			break;
	}

	if (i == n)
		return; // no sum factors

	// remove the sum factor

	stack.splice(i, 1);

	n = stack.length - h;

	if (n > 1) {
		sort_factors(h);
		list(n);
		push_symbol(MULTIPLY);
		swap();
		cons();
	}

	p1 = pop(); // p1 is the multiplier

	p2 = cdr(p2); // p2 is the sum

	while (iscons(p2)) {
		push(p1);
		push(car(p2));
		multiply();
		p2 = cdr(p2);
	}

	add_terms(stack.length - h);
}
function
expcos()
{
	var p1 = pop();

	push(imaginaryunit);
	push(p1);
	multiply();
	exp();
	push_rational(1, 2);
	multiply();

	push(imaginaryunit);
	negate();
	push(p1);
	multiply();
	exp();
	push_rational(1, 2);
	multiply();

	add();
}
function
expcosh()
{
	var p1 = pop();
	push(p1);
	exp();
	push(p1);
	negate();
	exp();
	add();
	push_rational(1, 2);
	multiply();
}
function
expsin()
{
	var p1 = pop();

	push(imaginaryunit);
	push(p1);
	multiply();
	exp();
	push(imaginaryunit);
	divide();
	push_rational(1, 2);
	multiply();

	push(imaginaryunit);
	negate();
	push(p1);
	multiply();
	exp();
	push(imaginaryunit);
	divide();
	push_rational(1, 2);
	multiply();

	subtract();
}
function
expsinh()
{
	var p1 = pop();
	push(p1);
	exp();
	push(p1);
	negate();
	exp();
	subtract();
	push_rational(1, 2);
	multiply();
}
function
exptan()
{
	var p1;

	push_integer(2);
	push(imaginaryunit);
	multiply_factors(3);
	exp();

	p1 = pop();

	push(imaginaryunit);
	push(imaginaryunit);
	push(p1);
	multiply();
	subtract();

	push(p1);
	push_integer(1);
	add();

	divide();
}
function
exptanh()
{
	var p1;
	push_integer(2);
	multiply();
	exp();
	p1 = pop();
	push(p1);
	push_integer(1);
	subtract();
	push(p1);
	push_integer(1);
	add();
	divide();
}
function
factor()
{
	var numer, denom, INPUT, BASE, EXPO;

	INPUT = pop();

	if (car(INPUT) == symbol(POWER)) {

		BASE = cadr(INPUT);
		EXPO = caddr(INPUT);

		if (!isrational(BASE) || !isrational(EXPO)) {
			push(INPUT); // cannot factor
			return;
		}

		if (equaln(BASE, -1)) {
			push(INPUT); // -1 to the M
			return;
		}

		numer = BASE.a;
		denom = BASE.b;

		if (numer < 0) {
			push_symbol(POWER);
			push_integer(-1);
			push(EXPO);
			list(3); // leave on stack
			numer = -numer;
		}

		if (numer != 1)
			factor_number(numer, EXPO);

		if (denom != 1) {
			// flip sign of exponent
			push(EXPO);
			negate();
			EXPO = pop();
			factor_number(denom, EXPO);
		}

		return;
	}

	if (!isrational(INPUT) || iszero(INPUT) || isplusone(INPUT) || isminusone(INPUT)) {
		push(INPUT);
		return;
	}

	numer = INPUT.a;
	denom = INPUT.b;

	if (numer < 0) {
		push_integer(-1);
		numer = -numer;
	}

	if (numer != 1)
		factor_number(numer, one);

	if (denom != 1)
		factor_number(denom, minusone);
}
// n is a number, exponent is a rational

function
factor_number(n, exponent)
{
	var d, k, m, t;

	for (k = 0; k < 10000; k++) {

		d = primetab[k];

		if (n / d < d)
			break; // n is 1 or prime

		m = 0;

		while (n % d == 0) {
			n /= d;
			m++;
		}

		if (m == 0)
			continue;

		push_integer(d);

		push_integer(m);
		push(exponent);
		multiply();
		t = pop();

		if (!isplusone(t)) {
			push_symbol(POWER);
			swap();
			push(t);
			list(3);
		}
	}

	if (n > 1) {
		push_integer(n);
		if (!isplusone(exponent)) {
			push_symbol(POWER);
			swap();
			push(exponent);
			list(3);
		}
	}
}
function
factorial()
{
	var i, m, n, p;

	p = pop();

	if (isrational(p) && p.a >= 0 && p.b == 1)
		n = p.a;
	else if (isdouble(p) && p.d >= 0 && Math.floor(p.d) == p.d)
		n = p.d;
	else {
		push_symbol(FACTORIAL);
		push(p);
		list(2);
		return;
	}

	m = 1;

	for (i = 2; i <= n; i++)
		m *= i;

	if (isrational(p))
		push_integer(m);
	else
		push_double(m);
}
function
findf(p, q) // is q in p?
{
	var i, n;

	if (equal(p, q))
		return 1;

	if (istensor(p)) {
		n = p.elem.length;
		for (i = 0; i < n; i++) {
			if (findf(p.elem[i], q))
				return 1;
		}
		return 0;
	}

	while (iscons(p)) {
		if (findf(car(p), q))
			return 1;
		p = cdr(p);
	}

	return 0;
}
function
flatten_factors(h)
{
	var i, n, p1;
	n = stack.length;
	for (i = h; i < n; i++) {
		p1 = stack[i];
		if (car(p1) == symbol(MULTIPLY)) {
			p1 = cdr(p1);
			stack[i] = car(p1);
			p1 = cdr(p1);
			while (iscons(p1)) {
				push(car(p1));
				p1 = cdr(p1);
			}
		}
	}
}
function
flatten_terms(h)
{
	var i, n, p1;

	n = stack.length;

	for (i = h; i < n; i++) {

		p1 = stack[i];

		if (car(p1) == symbol(ADD)) {
			p1 = cdr(p1);
			stack[i] = car(p1);
			p1 = cdr(p1);
			while (iscons(p1)) {
				push(car(p1));
				p1 = cdr(p1);
			}
		}
	}
}
function
float_subst()
{
	var h, i, n, p1;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1.elem.length
		for (i = 0; i < n; i++) {
			push(p1.elem[i]);
			float_subst();
			p1.elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (p1 == symbol(PI)) {
		push_double(Math.PI);
		return;
	}

	if (p1 == symbol(EXP1)) {
		push_double(Math.E);
		return;
	}

	if (isrational(p1)) {
		push_double(p1.a / p1.b);
		return;
	}

	// don't float exponential

	if (car(p1) == symbol(POWER) && cadr(p1) == symbol(EXP1)) {
		push_symbol(POWER);
		push_symbol(EXP1);
		push(caddr(p1));
		float_subst();
		list(3);
		return;
	}

	// don't float imaginary unit, but multiply it by 1.0

	if (car(p1) == symbol(POWER) && isminusone(cadr(p1))) {
		push_symbol(MULTIPLY);
		push_double(1);
		push_symbol(POWER);
		push(cadr(p1));
		push(caddr(p1));
		float_subst();
		list(3);
		list(3);
		return;
	}

	if (iscons(p1)) {
		h = stack.length;
		push(car(p1));
		p1 = cdr(p1);
		while (iscons(p1)) {
			push(car(p1));
			float_subst();
			p1 = cdr(p1);
		}
		list(stack.length - h);
		return;
	}

	push(p1);
}
function
floatf()
{
	float_subst();
	evalf();
}
function
gcd_integers(a, b)
{
	var t;
	a = Math.abs(a);
	b = Math.abs(b);
	while (b) {
		t = b;
		b = a % b;
		a = t;
	}
	return a;
}
function
get_arglist(p)
{
	if (p.printname in arglist)
		return arglist[p.printname];
	else
		return symbol(NIL);
}
function
get_binding(p)
{
	if (p.printname in binding)
		return binding[p.printname];
	else
		return p;
}
function
guess()
{
	var p = pop();
	push(p);
	if (findf(p, symbol(SYMBOL_X)))
		push_symbol(SYMBOL_X);
	else if (findf(p, symbol(SYMBOL_Y)))
		push_symbol(SYMBOL_Y);
	else if (findf(p, symbol(SYMBOL_Z)))
		push_symbol(SYMBOL_Z);
	else if (findf(p, symbol(SYMBOL_T)))
		push_symbol(SYMBOL_T);
	else if (findf(p, symbol(SYMBOL_S)))
		push_symbol(SYMBOL_S);
	else
		push_symbol(SYMBOL_X);
}
function
imag()
{
	var p1;
	rect();
	p1 = pop();
	push_rational(-1, 2);
	push(imaginaryunit);
	push(p1);
	push(p1);
	conj();
	subtract();
	multiply_factors(3);
}
function
index(k)
{
	var i, n, p1, p2;

	p1 = pop();

	if (!istensor(p1) || k < 1 || k > p1.dim[0])
		stopf("index error");

	k--; // make zero based

	n = p1.elem.length / p1.dim[0];

	if (n == 1) {
		push(p1.elem[k]);
		return;
	}

	p2 = alloc_tensor();

	for (i = 0; i < n; i++)
		p2.elem[i] = p1.elem[k * n + i];

	n = p1.dim.length;

	for (i = 1; i < n; i++)
		p2.dim[i - 1] = p1.dim[i];

	push(p2);
}
function
init()
{
	expanding = 1;
	evaldepth = 0;

	stack = [];
	frame = [];

	arglist = {};
	binding = {};

	push_integer(0);
	zero = pop();

	push_integer(1);
	one = pop();

	push_integer(-1);
	minusone = pop();

	push_symbol(POWER);
	push_integer(-1);
	push_rational(1, 2);
	list(3);
	imaginaryunit = pop();
}
var init_script = [
"i = sqrt(-1)",
"trace = 0",
];

function
initscript()
{
	var i, n

	n = init_script.length;

	for (i = 0; i < n; i++) {
		scan(init_script[i], 0);
		evalf();
		pop();
	}
}
function
inner()
{
	var i, j, k, n, mcol, mrow, ncol, nrow, p1, p2, p3;

	p2 = pop();
	p1 = pop();

	if (!istensor(p1) && !istensor(p2)) {
		push(p1);
		push(p2);
		multiply();
		return;
	}

	if (istensor(p1) && !istensor(p2)) {
		p3 = p1;
		p1 = p2;
		p2 = p3;
	}

	if (!istensor(p1) && istensor(p2)) {
		p2 = copy_tensor(p2);
		n = p2.elem.length;
		for (i = 0; i < n; i++) {
			push(p1);
			push(p2.elem[i]);
			multiply();
			p2.elem[i] = pop();
		}
		push(p2);
		return;
	}

	ncol = p1.dim[p1.dim.length - 1];
	mrow = p2.dim[0];

	if (ncol != mrow)
		stopf("inner: dimension err");

	//	nrow is the number of rows in p1
	//
	//	mcol is the number of columns p2
	//
	//	Example:
	//
	//	A[3][3][4] B[4][4][3]
	//
	//	  3  3				nrow = 3 * 3 = 9
	//
	//	                4  3		mcol = 4 * 3 = 12

	nrow = p1.elem.length / ncol;
	mcol = p2.elem.length / mrow;

	p3 = alloc_tensor();

	for (i = 0; i < nrow; i++) {
		for (j = 0; j < mcol; j++) {
			for (k = 0; k < ncol; k++) {
				push(p1.elem[i * ncol + k]);
				push(p2.elem[k * mcol + j]);
				multiply();
			}
			add_terms(ncol);
			p3.elem[i * mcol + j] = pop();
		}
	}

	n = p1.dim.length + p2.dim.length - 2;

	if (n == 0) {
		push(p3.elem[0]); // scalar result
		return;
	}

	k = 0;

	n = p1.dim.length - 1;

	for (i = 0; i < n; i++)
		p3.dim[k++] = p1.dim[i];

	n = p2.dim.length;

	for (i = 1; i < n; i++)
		p3.dim[k++] = p2.dim[i];

	push(p3);
}
function
integral()
{
	var h, p1, F, X;

	X = pop();
	F = pop();

	if (car(F) == symbol(ADD)) {
		h = stack.length;
		p1 = cdr(F);
		while (iscons(p1)) {
			push(car(p1));
			push(X);
			integral();
			p1 = cdr(p1);
		}
		add_terms(stack.length - h);
		return;
	}

	if (car(F) == symbol(MULTIPLY)) {
		partition_integrand(F, X);
		F = pop();		// pop var part
		integral_nib(F, X);
		multiply();		// multiply by const part
		return;
	}

	integral_nib(F, X);
}
function
integral_classify(p)
{
	var t = 0;

	if (iscons(p)) {
		while (iscons(p)) {
			t |= integral_classify(car(p));
			p = cdr(p);
		}
		return t;
	}

	if (p == symbol(EXP1))
		return 1;

	if (p == symbol(LOG))
		return 2;

	if (p == symbol(SIN) || p == symbol(COS) || p == symbol(TAN))
		return 4;

	return 0;
}
function
integral_lookup(F, h)
{
	var t, table;

	t = integral_classify(F);

	if ((t & 1) && integral_search(F, h, integral_tab_exp))
		return;

	if ((t & 2) && integral_search(F, h, integral_tab_log))
		return;

	if ((t & 4) && integral_search(F, h, integral_tab_trig))
		return;

	if (car(F) == symbol(POWER))
		table = integral_tab_power;
	else
		table = integral_tab;

	if (integral_search(F, h, table))
		return;

	stopf("integral: no solution found");
}
function
integral_nib(F, X)
{
	var h;

	save_binding(symbol(METAA));
	save_binding(symbol(METAB));
	save_binding(symbol(METAX));

	set_binding(symbol(METAX), X);

	// put constants in F(X) on the stack

	h = stack.length;

	push_integer(1); // 1 is a candidate for a or b

	push(F);
	push(X);
	decomp();

	integral_lookup(F, h);

	restore_binding(symbol(METAX));
	restore_binding(symbol(METAB));
	restore_binding(symbol(METAA));
}
function
integral_search(F, h, table)
{
	var i, n, I, C;

	n = table.length;

	for (i = 0; i < n; i += 3) {

		scan1(table[i + 0]); // integrand
		I = pop();

		scan1(table[i + 2]); // condition
		C = pop();

		if (integral_search_nib(F, I, C, h))
			break;
	}

	stack.splice(h); // pop all

	if (i == n)
		return 0;

	scan1(table[i + 1]); // answer
	evalf();

	return 1;
}
function
integral_search_nib(F, I, C, h)
{
	var i, j, n, p1;

	n = stack.length;

	for (i = h; i < n; i++) {

		set_binding(symbol(METAA), stack[i]);

		for (j = h; j < n; j++) {

			set_binding(symbol(METAB), stack[j]);

			push(C);			// condition ok?
			evalf();
			p1 = pop();
			if (iszero(p1))
				continue;		// no, go to next j

			push(F);			// F = I?
			push(I);
			evalf();
			subtract();
			p1 = pop();
			if (iszero(p1))
				return 1;		// yes
		}
	}

	return 0;					// no
}
var integral_tab_exp = [

// x^n exp(a x + b)

	"exp(a x)",
	"exp(a x) / a",
	"1",

	"exp(a x + b)",
	"exp(a x + b) / a",
	"1",

	"x exp(a x)",
	"exp(a x) (a x - 1) / (a^2)",
	"1",

	"x exp(a x + b)",
	"exp(a x + b) (a x - 1) / (a^2)",
	"1",

	"x^2 exp(a x)",
	"exp(a x) (a^2 x^2 - 2 a x + 2) / (a^3)",
	"1",

	"x^2 exp(a x + b)",
	"exp(a x + b) (a^2 x^2 - 2 a x + 2) / (a^3)",
	"1",

	"x^3 exp(a x)",
	"(a^3 x^3 - 3 a^2 x^2 + 6 a x - 6) exp(a x) / a^4",
	"1",

	"x^3 exp(a x + b)",
	"(a^3 x^3 - 3 a^2 x^2 + 6 a x - 6) exp(a x + b) / a^4",
	"1",

// x^n exp(a x^2 + b)

// erfi(x) = -i erf(i x)

	"exp(a x^2)",
	"-1/2 i sqrt(pi) erf(i sqrt(a) x) / sqrt(a)",
	"1",

	"exp(a x^2 + b)",
	"-1/2 i sqrt(pi) exp(b) erf(i sqrt(a) x) / sqrt(a)",
	"1",

	"x exp(a x^2)",
	"1/2 exp(a x^2) / a",
	"1",

	"x exp(a x^2 + b)",
	"1/2 exp(a x^2 + b) / a",
	"1",

	"x^2 exp(a x^2)",
	"1/2 x exp(a x^2) / a + 1/4 i sqrt(pi) erf(i sqrt(a) x) / a^(3/2)",
	"1",

	"x^2 exp(a x^2 + b)",
	"1/2 x exp(a x^2 + b) / a + 1/4 i sqrt(pi) exp(b) erf(i sqrt(a) x) / a^(3/2)",
	"1",

	"x^3 exp(a x^2)",
	"1/2 exp(a x^2) (x^2 / a - 1 / a^2)",
	"1",

	"x^3 exp(a x^2 + b)",
	"1/2 exp(a x^2) exp(b) (x^2 / a - 1 / a^2)",
	"1",

	"x exp(a x + b x)",
	"exp(a x + b x) (a x + b x + 1) / (a + b)^2",
	"1",
];

// log(a x) is transformed to log(a) + log(x)

var integral_tab_log = [

	"log(x)",
	"x log(x) - x",
	"1",

	"log(a x + b)",
	"x log(a x + b) + b log(a x + b) / a - x",
	"1",

	"x log(x)",
	"x^2 log(x) 1/2 - x^2 1/4",
	"1",

	"x log(a x + b)",
	"1/2 (a x - b) (a x + b) log(a x + b) / a^2 - 1/4 x (a x - 2 b) / a",
	"1",

	"x^2 log(x)",
	"x^3 log(x) 1/3 - 1/9 x^3",
	"1",

	"x^2 log(a x + b)",
	"1/3 (a x + b) (a^2 x^2 - a b x + b^2) log(a x + b) / a^3 - 1/18 x (2 a^2 x^2 - 3 a b x + 6 b^2) / a^2",
	"1",

	"log(x)^2",
	"x log(x)^2 - 2 x log(x) + 2 x",
	"1",

	"log(a x + b)^2",
	"(a x + b) (log(a x + b)^2 - 2 log(a x + b) + 2) / a",
	"1",

	"log(x) / x^2",
	"-(log(x) + 1) / x",
	"1",

	"log(a x + b) / x^2",
	"a log(x) / b - (a x + b) log(a x + b) / (b x)",
	"1",

	"1 / (x (a + log(x)))",
	"log(a + log(x))",
	"1",
];

var integral_tab_trig = [

	"sin(a x)",
	"-cos(a x) / a",
	"1",

	"cos(a x)",
	"sin(a x) / a",
	"1",

	"tan(a x)",
	"-log(cos(a x)) / a",
	"1",

	"sin(a x)^2",
	"x 1/2 - sin(2 a x) / (4 a)",
	"1",

	"sin(a x)^3",
	"-cos(a x) (sin(a x)^2 + 2) / (3 a)",
	"1",

	"sin(a x)^4",
	"3/8 x - sin(2 a x) / (4 a) + sin(4 a x) / (32 a)",
	"1",

	"cos(a x)^2",
	"x 1/2 + sin(2 a x) / (4 a)",
	"1",

	"cos(a x)^3",
	"sin(a x) (cos(a x)^2 + 2) / (3 a)",
	"1",

	"cos(a x)^4",
	"3/8 x + sin(2 a x) / (4 a) + sin(4 a x) / (32 a)",
	"1",

	"sin(a x) cos(a x)",
	"1/2 sin(a x)^2 / a",
	"1",

	"sin(a x) cos(a x)^2",
	"-1/3 cos(a x)^3 / a",
	"1",

	"sin(a x)^2 cos(a x)",
	"1/3 sin(a x)^3 / a",
	"1",

	"sin(a x)^2 cos(a x)^2",
	"1/8 x - 1/32 sin(4 a x) / a",
	"1",
// 329
	"1 / sin(a x) / cos(a x)",
	"log(tan(a x)) / a",
	"1",
// 330
	"1 / sin(a x) / cos(a x)^2",
	"(1 / cos(a x) + log(tan(a x 1/2))) / a",
	"1",
// 331
	"1 / sin(a x)^2 / cos(a x)",
	"(log(tan(pi 1/4 + a x 1/2)) - 1 / sin(a x)) / a",
	"1",
// 333
	"1 / sin(a x)^2 / cos(a x)^2",
	"-2 / (a tan(2 a x))",
	"1",
//
	"sin(a x) / cos(a x)",
	"-log(cos(a x)) / a",
	"1",

	"sin(a x) / cos(a x)^2",
	"1 / a / cos(a x)",
	"1",

	"sin(a x)^2 / cos(a x)",
	"-(sin(a x) + log(cos(a x / 2) - sin(a x / 2)) - log(sin(a x / 2) + cos(a x / 2))) / a",
	"1",

	"sin(a x)^2 / cos(a x)^2",
	"tan(a x) / a - x",
	"1",
//
	"cos(a x) / sin(a x)",
	"log(sin(a x)) / a",
	"1",

	"cos(a x) / sin(a x)^2",
	"-1 / (a sin(a x))",
	"1",

	"cos(a x)^2 / sin(a x)",
	"cos(a x) / a + log(sin(a x / 2)) / a - log(cos(a x / 2)) / a",
	"1",

	"cos(a x)^2 / sin(a x)^2",
	"-x - cos(a x) / sin(a x) / a",
	"1",
//
	"sin(a + b x)",
	"-cos(a + b x) / b",
	"1",

	"cos(a + b x)",
	"sin(a + b x) / b",
	"1",

	"x sin(a x)",
	"sin(a x) / (a^2) - x cos(a x) / a",
	"1",

	"x^2 sin(a x)",
	"2 x sin(a x) / (a^2) - (a^2 x^2 - 2) cos(a x) / (a^3)",
	"1",

	"x cos(a x)",
	"cos(a x) / (a^2) + x sin(a x) / a",
	"1",

	"x^2 cos(a x)",
	"2 x cos(a x) / (a^2) + (a^2 x^2 - 2) sin(a x) / (a^3)",
	"1",

	"1 / tan(a x)",
	"log(sin(a x)) / a",
	"1",

	"1 / cos(a x)",
	"log(tan(pi 1/4 + a x 1/2)) / a",
	"1",

	"1 / sin(a x)",
	"log(tan(a x 1/2)) / a",
	"1",

	"1 / sin(a x)^2",
	"-1 / (a tan(a x))",
	"1",

	"1 / cos(a x)^2",
	"tan(a x) / a",
	"1",

	"1 / (b + b sin(a x))",
	"-tan(pi 1/4 - a x 1/2) / (a b)",
	"1",

	"1 / (b - b sin(a x))",
	"tan(pi 1/4 + a x 1/2) / (a b)",
	"1",

	"1 / (b + b cos(a x))",
	"tan(a x 1/2) / (a b)",
	"1",

	"1 / (b - b cos(a x))",
	"-1 / (tan(a x 1/2) a b)",
	"1",

	"1 / (a + b sin(x))",
	"log((a tan(x 1/2) + b - sqrt(b^2 - a^2)) / (a tan(x 1/2) + b + sqrt(b^2 - a^2))) / sqrt(b^2 - a^2)",
	"b^2 - a^2",

	"1 / (a + b cos(x))",
	"log((sqrt(b^2 - a^2) tan(x 1/2) + a + b) / (sqrt(b^2 - a^2) tan(x 1/2) - a - b)) / sqrt(b^2 - a^2)",
	"b^2 - a^2",

	"x sin(a x) sin(b x)",
	"1/2 ((x sin(x (a - b)))/(a - b) - (x sin(x (a + b)))/(a + b) + cos(x (a - b))/(a - b)^2 - cos(x (a + b))/(a + b)^2)",
	"and(not(a + b == 0),not(a - b == 0))",
];

var integral_tab_power = [

	"a", // for forms c^d where both c and d are constant expressions
	"a x",
	"1",

	"1 / x",
	"log(x)",
	"1",

	"x^a",			// integrand
	"x^(a + 1) / (a + 1)",	// answer
	"not(a = -1)",		// condition

	"a^x",
	"a^x / log(a)",
	"or(not(number(a)),a>0)",

	"1 / (a + b x)",
	"log(a + b x) / b",
	"1",
// 124
	"sqrt(a x + b)",
	"2/3 (a x + b)^(3/2) / a",
	"1",
// 138
	"sqrt(a x^2 + b)",
	"1/2 x sqrt(a x^2 + b) + 1/2 b log(sqrt(a) sqrt(a x^2 + b) + a x) / sqrt(a)",
	"1",
// 131
	"1 / sqrt(a x + b)",
	"2 sqrt(a x + b) / a",
	"1",

	"1 / ((a + b x)^2)",
	"-1 / (b (a + b x))",
	"1",

	"1 / ((a + b x)^3)",
	"-1 / ((2 b) ((a + b x)^2))",
	"1",
// 16
	"1 / (a x^2 + b)",
	"arctan(sqrt(a) x / sqrt(b)) / sqrt(a) / sqrt(b)",
	"1",
// 17
	"1 / sqrt(a x^2 + b)",
	"log(sqrt(a) sqrt(a x^2 + b) + a x) / sqrt(a)",
	"1",
// 65
	"1 / (a x^2 + b)^2",
	"1/2 ((arctan((sqrt(a) x) / sqrt(b))) / (sqrt(a) b^(3/2)) + x / (a b x^2 + b^2))",
	"1",
// 165
	"(a x^2 + b)^(-3/2)",
	"x / b / sqrt(a x^2 + b)",
	"1",
// 74
	"1 / (a x^3 + b)",
	"-log(a^(2/3) x^2 - a^(1/3) b^(1/3) x + b^(2/3))/(6 a^(1/3) b^(2/3))" +
	" + log(a^(1/3) x + b^(1/3))/(3 a^(1/3) b^(2/3))" +
	" - (i log(1 - (i (1 - (2 a^(1/3) x)/b^(1/3)))/sqrt(3)))/(2 sqrt(3) a^(1/3) b^(2/3))" +
	" + (i log(1 + (i (1 - (2 a^(1/3) x)/b^(1/3)))/sqrt(3)))/(2 sqrt(3) a^(1/3) b^(2/3))", // from Wolfram Alpha
	"1",
// 77
	"1 / (a x^4 + b)",
	"-log(-sqrt(2) a^(1/4) b^(1/4) x + sqrt(a) x^2 + sqrt(b))/(4 sqrt(2) a^(1/4) b^(3/4))" +
	" + log(sqrt(2) a^(1/4) b^(1/4) x + sqrt(a) x^2 + sqrt(b))/(4 sqrt(2) a^(1/4) b^(3/4))" +
	" - (i log(1 - i (1 - (sqrt(2) a^(1/4) x)/b^(1/4))))/(4 sqrt(2) a^(1/4) b^(3/4))" +
	" + (i log(1 + i (1 - (sqrt(2) a^(1/4) x)/b^(1/4))))/(4 sqrt(2) a^(1/4) b^(3/4))" +
	" + (i log(1 - i ((sqrt(2) a^(1/4) x)/b^(1/4) + 1)))/(4 sqrt(2) a^(1/4) b^(3/4))" +
	" - (i log(1 + i ((sqrt(2) a^(1/4) x)/b^(1/4) + 1)))/(4 sqrt(2) a^(1/4) b^(3/4))", // from Wolfram Alpha
	"1",
// 164
	"sqrt(a + x^6 + 3 a^(1/3) x^4 + 3 a^(2/3) x^2)",
	"1/4 (x sqrt((x^2 + a^(1/3))^3) + 3/2 a^(1/3) x sqrt(x^2 + a^(1/3)) + 3/2 a^(2/3) log(x + sqrt(x^2 + a^(1/3))))",
	"1",
// 165
	"sqrt(-a + x^6 - 3 a^(1/3) x^4 + 3 a^(2/3) x^2)",
	"1/4 (x sqrt((x^2 - a^(1/3))^3) - 3/2 a^(1/3) x sqrt(x^2 - a^(1/3)) + 3/2 a^(2/3) log(x + sqrt(x^2 - a^(1/3))))",
	"1",

	"sinh(x)^2",
	"sinh(2 x) 1/4 - x 1/2",
	"1",

	"tanh(x)^2",
	"x - tanh(x)",
	"1",

	"cosh(x)^2",
	"sinh(2 x) 1/4 + x 1/2",
	"1",
];

var integral_tab = [

	"a",
	"a x",
	"1",

	"x",
	"1/2 x^2",
	"1",
// 18
	"x / sqrt(a x^2 + b)",
	"sqrt(a x^2 + b) / a",
	"1",

	"x / (a + b x)",
	"x / b - a log(a + b x) / (b b)",
	"1",

	"x / ((a + b x)^2)",
	"(log(a + b x) + a / (a + b x)) / (b^2)",
	"1",
// 33
	"x^2 / (a + b x)",
	"a^2 log(a + b x) / b^3 + x (b x - 2 a) / (2 b^2)",
	"1",
// 34
	"x^2 / (a + b x)^2",
	"(-a^2 / (a + b x) - 2 a log(a + b x) + b x) / b^3",
	"1",

	"x^2 / (a + b x)^3",
	"(log(a + b x) + 2 a / (a + b x) - a^2 / (2 ((a + b x)^2))) / (b^3)",
	"1",

	"1 / x / (a + b x)",
	"-log((a + b x) / x) / a",
	"1",

	"1 / x / (a + b x)^2",
	"1 / (a (a + b x)) - log((a + b x) / x) / (a^2)",
	"1",

	"1 / x / (a + b x)^3",
	"(1/2 ((2 a + b x) / (a + b x))^2 + log(x / (a + b x))) / (a^3)",
	"1",

	"1 / x^2 / (a + b x)",
	"-1 / (a x) + b log((a + b x) / x) / (a^2)",
	"1",

	"1 / x^3 / (a + b x)",
	"(2 b x - a) / (2 a^2 x^2) + b^2 log(x / (a + b x)) / (a^3)",
	"1",

	"1 / x^2 / (a + b x)^2",
	"-(a + 2 b x) / (a^2 x (a + b x)) + 2 b log((a + b x) / x) / (a^3)",
	"1",

	"x / (a + b x^2)",
	"log(a + b x^2) / (2 b)",
	"1",
// 64
	"x^2 / (a x^2 + b)",
	"1/2 i a^(-3/2) sqrt(b) (log(1 + i sqrt(a) x / sqrt(b)) - log(1 - i sqrt(a) x / sqrt(b))) + x / a",
	"1",

	"1 / x * 1 / (a + b x^2)",
	"1 log(x^2 / (a + b x^2)) / (2 a)",
	"1",
// 71
	"1 / x^2 * 1 / (a x^2 + b)",
	"1/2 i sqrt(a) b^(-3/2) (log(1 + i sqrt(a) x / sqrt(b)) - log(1 - i sqrt(a) x / sqrt(b))) - 1 / (b x)",
	"1",
// 76
	"x^2 / (a + b x^3)",
	"1 log(a + b x^3) / (3 b)",
	"1",

	"x / (a + b x^4)",
	"sqrt(b / a) arctan(x^2 sqrt(b / a)) / (2 b)",
	"or(not(number(a b)),testgt(a b,0))",

	"x / (a + b x^4)",
	"sqrt(-b / a) log((x^2 - sqrt(-a / b)) / (x^2 + sqrt(-a / b))) / (4 b)",
	"or(not(number(a b)),testlt(a b,0))",

	"x^2 / (a + b x^4)",
	"1 (1/2 log((x^2 - 2 (a 1/4 / b)^(1/4) x + 2 sqrt(a 1/4 / b)) / (x^2 + 2 (a 1/4 / b)^(1/4) x + 2 sqrt(a 1/4 / b))) + arctan(2 (a 1/4 / b)^(1/4) x / (2 sqrt(a 1/4 / b) - x^2))) / (4 b (a 1/4 / b)^(1/4))",
	"or(not(number(a b)),testgt(a b,0))",

	"x^2 / (a + b x^4)",
	"1 (log((x - (-a / b)^(1/4)) / (x + (-a / b)^(1/4))) + 2 arctan(x / ((-a / b)^(1/4)))) / (4 b (-a / b)^(1/4))",
	"or(not(number(a b)),testlt(a b,0))",

	"x^3 / (a + b x^4)",
	"1 log(a + b x^4) / (4 b)",
	"1",

	"x sqrt(a + b x)",
	"-2 (2 a - 3 b x) sqrt((a + b x)^3) 1/15 / (b^2)",
	"1",

	"x^2 sqrt(a + b x)",
	"2 (8 a^2 - 12 a b x + 15 b^2 x^2) sqrt((a + b x)^3) 1/105 / (b^3)",
	"1",

	"x^2 sqrt(a + b x^2)",
	"(sqrt(b) x sqrt(a + b x^2) (a + 2 b x^2) - a^2 log(sqrt(b) sqrt(a + b x^2) + b x)) / (8 b^(3/2))",
	"1",
// 128
	"sqrt(a x + b) / x",
	"2 sqrt(a x + b) - 2 sqrt(b) arctanh(sqrt(a x + b) / sqrt(b))",
	"1",
// 129
	"sqrt(a x + b) / x^2",
	"-sqrt(a x + b) / x - a arctanh(sqrt(a x + b) / sqrt(b)) / sqrt(b)",
	"1",

	"x / sqrt(a + b x)",
	"-2 (2 a - b x) sqrt(a + b x) / (3 (b^2))",
	"1",

	"x^2 / sqrt(a + b x)",
	"2 (8 a^2 - 4 a b x + 3 b^2 x^2) sqrt(a + b x) / (15 (b^3))",
	"1",
// 134
	"1 / x / sqrt(a x + b)",
	"-2 arctanh(sqrt(a x + b) / sqrt(b)) / sqrt(b)",
	"1",
// 137
	"1 / x^2 / sqrt(a x + b)",
	"a arctanh(sqrt(a x + b) / sqrt(b)) / b^(3/2) - sqrt(a x + b) / (b x)",
	"1",
// 158
	"1 / x / sqrt(a x^2 + b)",
	"(log(x) - log(sqrt(b) sqrt(a x^2 + b) + b)) / sqrt(b)",
	"1",
// 160
	"sqrt(a x^2 + b) / x",
	"sqrt(a x^2 + b) - sqrt(b) log(sqrt(b) sqrt(a x^2 + b) + b) + sqrt(b) log(x)",
	"1",
// 163
	"x sqrt(a x^2 + b)",
	"1/3 (a x^2 + b)^(3/2) / a",
	"1",
// 166
	"x (a x^2 + b)^(-3/2)",
	"-1 / a / sqrt(a x^2 + b)",
	"1",

	"x sqrt(a + x^6 + 3 a^(1/3) x^4 + 3 a^(2/3) x^2)",
	"1/5 sqrt((x^2 + a^(1/3))^5)",
	"1",
// 168
	"x^2 sqrt(a x^2 + b)",
	"1/8 a^(-3/2) (sqrt(a) x sqrt(a x^2 + b) (2 a x^2 + b) - b^2 log(sqrt(a) sqrt(a x^2 + b) + a x))",
	"and(number(a),a>0)",
// 169
	"x^3 sqrt(a x^2 + b)",
	"1/15 sqrt(a x^2 + b) (3 a^2 x^4 + a b x^2 - 2 b^2) / a^2",
	"1",
// 171
	"x^2 / sqrt(a x^2 + b)",
	"1/2 a^(-3/2) (sqrt(a) x sqrt(a x^2 + b) - b log(sqrt(a) sqrt(a x^2 + b) + a x))",
	"1",
// 172
	"x^3 / sqrt(a x^2 + b)",
	"1/3 (a x^2 - 2 b) sqrt(a x^2 + b) / a^2",
	"1",
// 173
	"1 / x^2 / sqrt(a x^2 + b)",
	"-sqrt(a x^2 + b) / (b x)",
	"1",
// 174
	"1 / x^3 / sqrt(a x^2 + b)",
	"-sqrt(a x^2 + b) / (2 b x^2) + a (log(sqrt(b) sqrt(a x^2 + b) + b) - log(x)) / (2 b^(3/2))",
	"1",
// 216
	"sqrt(a x^2 + b) / x^2",
	"sqrt(a) log(sqrt(a) sqrt(a x^2 + b) + a x) - sqrt(a x^2 + b) / x",
	"and(number(a),a>0)",
// 217
	"sqrt(a x^2 + b) / x^3",
	"1/2 (-sqrt(a x^2 + b) / x^2 - (a log(sqrt(b) sqrt(a x^2 + b) + b)) / sqrt(b) + (a log(x)) / sqrt(b))",
	"and(number(b),b>0)",

	"arcsin(a x)",
	"x arcsin(a x) + sqrt(1 - a^2 x^2) / a",
	"1",

	"arccos(a x)",
	"x arccos(a x) + sqrt(1 - a^2 x^2) / a",
	"1",

	"arctan(a x)",
	"x arctan(a x) - log(1 + a^2 x^2) / (2 a)",
	"1",

	"sinh(x)",
	"cosh(x)",
	"1",

	"cosh(x)",
	"sinh(x)",
	"1",

	"tanh(x)",
	"log(cosh(x))",
	"1",

	"x sinh(x)",
	"x cosh(x) - sinh(x)",
	"1",

	"x cosh(x)",
	"x sinh(x) - cosh(x)",
	"1",

	"erf(a x)",
	"x erf(a x) + exp(-a^2 x^2) / (a sqrt(pi))",
	"1",

	"x^2 (1 - x^2)^(3/2)",
	"(x sqrt(1 - x^2) (-8 x^4 + 14 x^2 - 3) + 3 arcsin(x)) 1/48",
	"1",

	"x^2 (1 - x^2)^(5/2)",
	"(x sqrt(1 - x^2) (48 x^6 - 136 x^4 + 118 x^2 - 15) + 15 arcsin(x)) 1/384",
	"1",

	"x^4 (1 - x^2)^(3/2)",
	"(-x sqrt(1 - x^2) (16 x^6 - 24 x^4 + 2 x^2 + 3) + 3 arcsin(x)) 1/128",
	"1",
];
function
inv()
{
	var p1, p2;

	p1 = pop();

	if (!istensor(p1) || p1.dim.length != 2 || p1.dim[0] != p1.dim[1])
		stopf("inv: square matrix expected");

	push(p1);
	adj();
	push(p1);
	det();
	p2 = pop();
	if (iszero(p2))
		stopf("inv: singular matrix");
	push(p2);
	divide();
}
function
isalnum(c)
{
	return isalpha(c) || isdigit(c);
}
function
isalpha(c)
{
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}
function
iscomplexnumber(p)
{
	return isimaginarynumber(p) || (lengthf(p) == 3 && car(p) == symbol(ADD) && isnum(cadr(p)) && isimaginarynumber(caddr(p)));
}
function
iscons(p)
{
	if ("car" in p)
		return 1;
	else
		return 0;
}
function
isdigit(c)
{
	return c >= '0' && c <= '9';
}
function
isdouble(p)
{
	return "d" in p;
}
function
isdoublez(p)
{
	if (car(p) == symbol(ADD)) {

		if (lengthf(p) != 3)
			return 0;

		if (!isdouble(cadr(p))) // x
			return 0;

		p = caddr(p);
	}

	if (car(p) != symbol(MULTIPLY))
		return 0;

	if (lengthf(p) != 3)
		return 0;

	if (!isdouble(cadr(p))) // y
		return 0;

	p = caddr(p);

	if (car(p) != symbol(POWER))
		return 0;

	if (!equaln(cadr(p), -1))
		return 0;

	if (!equalq(caddr(p), 1, 2))
		return 0;

	return 1;
}
function
isfraction(p)
{
	return isrational(p) && !isinteger(p);
}
function
isimaginaryfactor(p)
{
	return car(p) == symbol(POWER) && isminusone(cadr(p));
}
function
isimaginarynumber(p)
{
	return isimaginaryunit(p) || (lengthf(p) == 3 && car(p) == symbol(MULTIPLY) && isnum(cadr(p)) && isimaginaryunit(caddr(p)));
}
function
isimaginaryterm(p)
{
	if (isimaginaryfactor(p))
		return 1;

	if (car(p) == symbol(MULTIPLY)) {
		p = cdr(p);
		while (iscons(p)) {
			if (isimaginaryfactor(car(p)))
				return 1;
			p = cdr(p);
		}
	}

	return 0;
}
function
isimaginaryunit(p)
{
	return car(p) == symbol(POWER) && isminusone(cadr(p)) && equalq(caddr(p), 1, 2);
}
function
isinteger(p)
{
	return ("b" in p) && p.b == 1;
}
function
iskeyword(p)
{
	return issymbol(p) && p.func != eval_symbol;
}
function
isminusone(p)
{
	return equaln(p, -1);
}
function
isminusoneoversqrttwo(p)
{
	return lengthf(p) == 3 && car(p) == symbol(MULTIPLY) && isminusone(cadr(p)) && isoneoversqrttwo(caddr(p));
}
function
isnegative(p)
{
	return isnegativeterm(p) || (car(p) == symbol(ADD) && isnegativeterm(cadr(p)));
}
function
isnegativenumber(p)
{
	return (isrational(p) && p.a < 0) || (isdouble(p) && p.d < 0);
}
function
isnegativeterm(p)
{
	return isnegativenumber(p) || (car(p) == symbol(MULTIPLY) && isnegativenumber(cadr(p)));
}
function
isnum(p)
{
	if ("d" in p || "a" in p)
		return 1;
	else
		return 0;
}
function
isoneoversqrttwo(p)
{
	return car(p) == symbol(POWER) && equaln(cadr(p), 2) && equalq(caddr(p), -1, 2);
}
function
isplusone(p)
{
	return equaln(p, 1);
}
function
isposint(p)
{
	return isinteger(p) && p.a > 0;
}
function
isradical(p)
{
	return car(p) == symbol(POWER) && isposint(cadr(p)) && isfraction(caddr(p));
}
function
isrational(p)
{
	return "a" in p;
}
function
isstring(p)
{
	return "string" in p;
}
function
issymbol(p)
{
	return "func" in p;
}
function
istensor(p)
{
	return "elem" in p;
}
function
isusersymbol(p)
{
	return p.func == eval_symbol;
}
function
iszero(p)
{
	var i, n;

	if (isrational(p))
		return p.a == 0;

	if (isdouble(p))
		return p.d == 0;

	if (istensor(p)) {
		n = p.elem.length;
		for (i = 0; i < n; i++) {
			if (!iszero(p.elem[i]))
				return 0;
		}
		return 1;
	}

	return 0;
}
function
lengthf(p)
{
	var n = 0;
	while (iscons(p)) {
		n++;
		p = cdr(p);
	}
	return n;
}
function
lessp(p1, p2)
{
	return cmp_expr(p1, p2) < 0;
}
function
list(n)
{
	var i;
	push_symbol(NIL);
	for (i = 0; i < n; i++)
		cons();
}
function
log()
{
	var h, i, n, p1, p2;

	p1 = pop();

	// log(0), log(0.0) unchanged

	if (iszero(p1)) {
		push_symbol(LOG);
		push(p1);
		list(2);
		return;
	}

	if (isdouble(p1)) {
		if (p1.d > 0.0)
			push_double(Math.log(p1.d));
		else {
			push_double(Math.log(-p1.d));
			push(imaginaryunit);
			push_double(Math.PI);
			multiply();
			add();
		}
		return;
	}

	// log(z) -> log(mag(z)) + i arg(z)

	if (isdoublez(p1)) {
		push(p1);
		mag();
		log();
		push(imaginaryunit);
		push(p1);
		arg();
		multiply();
		add();
		return;
	}

	// log(1) -> 0

	if (isplusone(p1)) {
		push_integer(0);
		return;
	}

	// log(e) -> 1

	if (p1 == symbol(EXP1)) {
		push_integer(1);
		return;
	}

	if (isnegativenumber(p1)) {
		push(p1);
		negate();
		log();
		push(imaginaryunit);
		push_symbol(PI);
		multiply();
		add();
		return;
	}

	// log(10) -> log(2) + log(5)

	if (isrational(p1)) {
		h = stack.length;
		push(p1);
		factor();
		n = stack.length;
		for (i = h; i < n; i++) {
			p2 = stack[i];
			if (car(p2) == symbol(POWER)) {
				push(caddr(p2)); // exponent
				push_symbol(LOG);
				push(cadr(p2)); // base
				list(2);
				multiply();
			} else {
				push_symbol(LOG);
				push(p2);
				list(2);
			}
			stack[i] = pop();
		}
		add_terms(stack.length - h);
		return;
	}

	// log(a ^ b) -> b log(a)

	if (car(p1) == symbol(POWER)) {
		push(caddr(p1));
		push(cadr(p1));
		log();
		multiply();
		return;
	}

	// log(a * b) -> log(a) + log(b)

	if (car(p1) == symbol(MULTIPLY)) {
		h = stack.length;
		p1 = cdr(p1);
		while (iscons(p1)) {
			push(car(p1));
			log();
			p1 = cdr(p1);
		}
		add_terms(stack.length - h);
		return;
	}

	push_symbol(LOG);
	push(p1);
	list(2);
}
function
lookup(s)
{
	if (!(s in symtab))
		symtab[s] = {printname:s, func:eval_symbol};
	return symtab[s];
}
function
mag()
{
	var h, p1, RE, IM;

	p1 = pop();

	if (isnum(p1)) {
		push(p1);
		abs();
		return;
	}

	// -1 to a power?

	if (car(p1) == symbol(POWER) && isminusone(cadr(p1))) {
		push_integer(1);
		return;
	}

	// exponential?

	if (car(p1) == symbol(POWER) && cadr(p1) == symbol(EXP1)) {
		push(caddr(p1));
		real();
		exp();
		return;
	}

	// product?

	if (car(p1) == symbol(MULTIPLY)) {
		p1 = cdr(p1);
		h = stack.length;
		while (iscons(p1)) {
			push(car(p1));
			mag();
			p1 = cdr(p1);
		}
		multiply_factors(stack.length - h);
		return;
	}

	// sum?

	if (car(p1) == symbol(ADD)) {
		push(p1);
		rect(); // convert polar terms, if any
		p1 = pop();
		push(p1);
		real();
		RE = pop();
		push(p1);
		imag();
		IM = pop();
		push(RE);
		push(RE);
		multiply();
		push(IM);
		push(IM);
		multiply();
		add();
		push_rational(1, 2);
		power();
		return;
	}

	// real

	push(p1);
}
function
mod_integers(p1, p2)
{
	push_integer(p1.a % p2.a);
}
function
multiply()
{
	multiply_factors(2);
}
// n is number of factors on stack

function
multiply_factors(n)
{
	var h, COEFF, TFACT;

	if (n < 2)
		return;

	if (n == 2 && isnum(stack[stack.length - 2]) && isnum(stack[stack.length - 1])) {
		var p2 = pop();
		var p1 = pop();
		multiply_numbers(p1, p2);
		return;
	}

	h = stack.length - n;

	flatten_factors(h);

	TFACT = tensor_factor(h);

	COEFF = collect_numerical_factors(h, one);

	if (iszero(COEFF)) {
		stack.splice(h); // pop all
		push(COEFF);
		return;
	}

	combine_factors(h);

	normalize_power_factors(h);

	COEFF = collect_numerical_factors(h, COEFF);

	COEFF = reduce_radical_factors(h, COEFF);

	if (!isplusone(COEFF) || isdouble(COEFF))
		push(COEFF);

	if (expanding)
		expand_sum_factors(h); // success leaves one expr on stack

	n = stack.length - h;

	switch (n) {
	case 0:
		push_integer(1);
		break;
	case 1:
		break;
	default:
		sort_factors(h); // previously sorted provisionally
		list(n);
		push_symbol(MULTIPLY);
		swap();
		cons();
		break;
	}

	if (istensor(TFACT)) {
		push(TFACT);
		inner();
	}
}
function
multiply_noexpand()
{
	var t = expanding;
	expanding = 0;
	multiply();
	expanding = t;
}
function
multiply_numbers(p1, p2)
{
	var a, b;

	if (isrational(p1) && isrational(p2)) {
		multiply_rationals(p1, p2);
		return;
	}

	push(p1);
	a = pop_double();

	push(p2);
	b = pop_double();

	push_double(a * b);
}
function
multiply_rationals(p1, p2)
{
	var a, b, c;

	a = p1.a * p2.a;
	b = p1.b * p2.b;

	if (a == 0) {
		push_integer(0);
		return;
	}

	if (Math.abs(a) > 1e10 || b > 1e10) {
		push_double(a / b);
		return;
	}

	if (b > 1) {
		c = gcd_integers(a, b)
		a /= c;
		b /= c;
	}

	push_rational(a, b);
}
function
negate()
{
	var p1 = pop();

	if (isrational(p1)) {
		push_rational(-p1.a, p1.b);
		return;
	}

	if (isdouble(p1)) {
		push_double(-p1.d);
		return;
	}

	push_integer(-1);
	push(p1);
	multiply();
}
function
normalize_power_factors(h)
{
	var i, n, p1;
	n = stack.length;
	for (i = h; i < n; i++) {
		p1 = stack[i];
		if (car(p1) == symbol(POWER)) {
			push(cadr(p1));
			push(caddr(p1));
			power();
			p1 = pop();
			if (car(p1) == symbol(MULTIPLY)) {
				p1 = cdr(p1);
				stack[i] = car(p1);
				p1 = cdr(p1);
				while (iscons(p1)) {
					push(car(p1));
					p1 = cdr(p1);
				}
			} else
				stack[i] = p1;
		}
	}
}
function
numerator()
{
	var p1 = pop();

	if (isrational(p1)) {
		push_integer(p1.a);
		return;
	}

	while (divisor(p1)) {
		push(p1);
		cancel_factor();
		p1 = pop();
	}

	push(p1);
}
//  1   number
//  2   number to power (root)
//  3   -1 to power (imaginary)
//  4   other factor (symbol, power, func, etc)
//  5   exponential
//  6   derivative

function
order_factor(p)
{
	if (isnum(p))
		return 1;

	if (p == symbol(EXP1))
		return 5;

	if (car(p) == symbol(DERIVATIVE) || car(p) == symbol(SYMBOL_D))
		return 6;

	if (car(p) == symbol(POWER)) {

		p = cadr(p); // p = base

		if (isminusone(p))
			return 3; // base = -1

		if (isnum(p))
			return 2;

		if (p == symbol(EXP1))
			return 5;

		if (car(p) == symbol(DERIVATIVE) || car(p) == symbol(SYMBOL_D))
			return 6;
	}

	return 4;
}
function
outer()
{
	var i, j, k, n, ncol, nrow, p1, p2, p3;

	p2 = pop();
	p1 = pop();

	if (!istensor(p1) && !istensor(p2)) {
		push(p1);
		push(p2);
		multiply();
		return;
	}

	if (istensor(p1) && !istensor(p2)) {
		p3 = p1;
		p1 = p2;
		p2 = p3;
	}

	if (!istensor(p1) && istensor(p2)) {
		p2 = copy_tensor(p2);
		n = p2.elem.length;
		for (i = 0; i < n; i++) {
			push(p1);
			push(p2.elem[i]);
			multiply();
			p2.elem[i] = pop();
		}
		push(p2);
		return;
	}

	p3 = alloc_tensor();

	nrow = p1.elem.length;
	ncol = p2.elem.length;

	for (i = 0; i < nrow; i++)
		for (j = 0; j < ncol; j++) {
			push(p1.elem[i]);
			push(p2.elem[j]);
			multiply();
			p3.elem[i * ncol + j] = pop();
		}

	k = 0;

	n = p1.dim.length;

	for (i = 0; i < n; i++)
		p3.dim[k++] = p1.dim[i];

	n = p2.dim.length

	for (i = 0; i < n; i++)
		p3.dim[k++] = p2.dim[i];

	push(p3);
}
function
partition_integrand(F, X)
{
	var h, p1;

	// push const part

	h = stack.length;
	p1 = cdr(F);
	while (iscons(p1)) {
		if (!findf(car(p1), X))
			push(car(p1));
		p1 = cdr(p1);
	}

	if (h == stack.length)
		push_integer(1);
	else
		multiply_factors(stack.length - h);

	// push var part

	h = stack.length;
	p1 = cdr(F);
	while (iscons(p1)) {
		if (findf(car(p1), X))
			push(car(p1));
		p1 = cdr(p1);
	}

	if (h == stack.length)
		push_integer(1);
	else
		multiply_factors(stack.length - h);
}
function
polar()
{
	var p1 = pop();
	push(p1);
	mag();
	push(imaginaryunit);
	push(p1);
	arg();
	multiply();
	exp();
	multiply();
}
function
pop()
{
	if (stack.length == 0)
		stopf("stack error");
	return stack.pop();
}
function
pop_double()
{
	var p = pop();

	if (isrational(p))
		return p.a / p.b;

	if (isdouble(p))
		return p.d;

	return 0;
}
function
pop_integer()
{
	var p1 = pop();

	if (isrational(p1) && p1.b == 1)
		return p1.a;

	if (isdouble(p1) && Math.floor(p1.d) == p1.d)
		return p1.d;

	stopf("integer expected");
}
function
power()
{
	var h, p3, BASE, EXPO;

	EXPO = pop();
	BASE = pop();

	if (istensor(BASE)) {
		power_tensor();
		return;
	}

	if (BASE == symbol(EXP1) && isdouble(EXPO)) {
		push_double(Math.E);
		BASE = pop();
	}

	if (BASE == symbol(PI) && isdouble(EXPO)) {
		push_double(Math.PI);
		BASE = pop();
	}

	if (power_precheck(BASE, EXPO))
		return;

	// BASE and EXPO numerical?

	if (isnum(BASE) && isnum(EXPO)) {
		power_numbers(BASE, EXPO);
		return;
	}

	// BASE = e ?

	if (BASE == symbol(EXP1)) {
		power_natural_number(BASE, EXPO);
		return;
	}

	// do this before checking for (a + b)^n

	if (iscomplexnumber(BASE)) {
		power_complex_number(BASE, EXPO);
		return;
	}

	// (a + b)^n -> (a + b) * (a + b) ...

	if (car(BASE) == symbol(ADD)) {
		power_sum(BASE, EXPO);
		return;
	}

	// (a * b) ^ c -> (a ^ c) * (b ^ c)

	if (car(BASE) == symbol(MULTIPLY)) {
		h = stack.length;
		p3 = cdr(BASE);
		while (iscons(p3)) {
			push(car(p3));
			push(EXPO);
			power();
			p3 = cdr(p3);
		}
		multiply_factors(stack.length - h);
		return;
	}

	// (a^b)^c -> a^(b * c)

	if (car(BASE) == symbol(POWER)) {
		push(cadr(BASE));
		push(caddr(BASE));
		push(EXPO);
		expanding++; // expand products of exponents
		multiply();
		expanding--;
		power();
		return;
	}

	// none of the above

	push_symbol(POWER);
	push(BASE);
	push(EXPO);
	list(3);
}
function
power_complex_double(BASE, EXPO, X, Y)
{
	var expo, r, theta, x, y;

	push(X);
	x = pop_double();

	push(Y);
	y = pop_double();

	push(EXPO);
	expo = pop_double();

	r = Math.sqrt(x * x + y * y);
	theta = Math.atan2(y, x);

	r = Math.pow(r, expo);
	theta = expo * theta;

	x = r * Math.cos(theta);
	y = r * Math.sin(theta);

	push_double(x);
	push_double(y);
	push(imaginaryunit);
	multiply();
	add();
}
function
power_complex_minus(X, Y, n)
{
	var i, R, PX, PY;

	// R = X^2 + Y^2

	push(X);
	push(X);
	multiply();
	push(Y);
	push(Y);
	multiply();
	add();
	R = pop();

	// X = X / R

	push(X);
	push(R);
	divide();
	X = pop();

	// Y = -Y / R

	push(Y);
	negate();
	push(R);
	divide();
	Y = pop();

	PX = X;
	PY = Y;

	for (i = 1; i < n; i++) {

		push(PX);
		push(X);
		multiply();
		push(PY);
		push(Y);
		multiply();
		subtract();

		push(PX);
		push(Y);
		multiply();
		push(PY);
		push(X);
		multiply();
		add();

		PY = pop();
		PX = pop();
	}

	// X + i*Y

	push(PX);
	push(imaginaryunit);
	push(PY);
	multiply();
	add();
}
function
power_complex_number(BASE, EXPO)
{
	var n, X, Y;

	if (!isnum(EXPO)) {
		push_symbol(POWER);
		push(BASE);
		push(EXPO);
		list(3);
		return;
	}

	// lisp(2+3*i) = (add 2 (multiply 3 (power -1 1/2)))

	// lisp(1+i) = (add 1 (power -1 1/2))

	// lisp(3*i) = (multiply 3 (power -1 1/2))

	// lisp(i) = (power -1 1/2)

	if (car(BASE) == symbol(ADD)) {
		X = cadr(BASE);
		if (caaddr(BASE) == symbol(MULTIPLY))
			Y = cadaddr(BASE);
		else
			Y = one;
	} else if (car(BASE) == symbol(MULTIPLY)) {
		X = zero;
		Y = cadr(BASE);
	} else {
		X = zero;
		Y = one;
	}

	if (isdouble(X) || isdouble(Y) || isdouble(EXPO)) {
		power_complex_double(BASE, EXPO, X, Y);
		return;
	}

	if (!isinteger(EXPO)) {
		power_complex_rational(BASE, EXPO, X, Y);
		return;
	}

	if (!isinteger(EXPO)) {
		push_symbol(POWER);
		push(BASE);
		push(EXPO);
		list(3);
		return;
	}

	n = EXPO.a;

	if (n > 0)
		power_complex_plus(X, Y, n);
	else if (n < 0)
		power_complex_minus(X, Y, -n);
	else
		push_integer(1);
}
function
power_complex_plus(X, Y, n)
{
	var i, PX, PY;

	PX = X;
	PY = Y;

	for (i = 1; i < n; i++) {

		push(PX);
		push(X);
		multiply();
		push(PY);
		push(Y);
		multiply();
		subtract();

		push(PX);
		push(Y);
		multiply();
		push(PY);
		push(X);
		multiply();
		add();

		PY = pop();
		PX = pop();
	}

	// X + i Y

	push(PX);
	push(imaginaryunit);
	push(PY);
	multiply();
	add();
}
function
power_complex_rational(BASE, EXPO, X, Y)
{
	// calculate sqrt(X^2 + Y^2) ^ (1/2 * EXPO)

	push(X);
	push(X);
	multiply();
	push(Y);
	push(Y);
	multiply();
	add();
	push_rational(1, 2);
	push(EXPO);
	multiply();
	power();

	// calculate (-1) ^ (EXPO * arctan(Y, X) / pi)

	push(Y);
	push(X);
	arctan();
	push_symbol(PI);
	divide();
	push(EXPO);
	multiply();

	EXPO = pop();
	power_minusone(BASE, EXPO);

	// result = sqrt(X^2 + Y^2) ^ (1/2 * EXPO) * (-1) ^ (EXPO * arctan(Y, X) / pi)

	multiply();
}
// convert (-1)^(a/b) to c (-1)^x where c=1 or c=-1 and -1/2 < x <= 1/2
//
// r = a mod b (remainder of a/b)
//
// q = (a/b - r) mod 2
//
// s = sign of a/b
//
// case			c	x
//
// s=1 q=0 r/b <= 1/2	1	r/b
// s=1 q=0 r/b > 1/2	-1	-(b - r)/b
//
// s=1 q=1 r/b <= 1/2	-1	r/b
// s=1 q=1 r/b > 1/2	1	-(b - r)/b
//
// s=-1 q=0 r/b < 1/2	1	-r/b
// s=-1 q=0 r/b >= 1/2	-1	(b - r)/b
//
// s=-1 q=1 r/b < 1/2	-1	-r/b
// s=-1 q=1 r/b >= 1/2	1	(b - r)/b
//
// q=1 flips sign of c
//
// s=-1 flips sign of x

function
power_minusone(BASE, EXPO)
{
	var a, b, c, q, r, s, t, theta, x, y;

	if (!isnum(EXPO)) {
		push_symbol(POWER);
		push_integer(-1);
		push(EXPO);
		list(3);
		if (isdouble(BASE)) {
			// BASE = -1.0, keep double
			push_symbol(MULTIPLY);
			swap();
			push_double(1.0);
			swap();
			list(3);
		}
		return;
	}

	if (equalq(EXPO, 1, 2)) {
		push(imaginaryunit);
		if (isdouble(BASE)) {
			// BASE = -1.0, keep double
			push_symbol(MULTIPLY);
			swap();
			push_double(1.0);
			swap();
			list(3);
		}
		return;
	}

	if (isdouble(EXPO)) {
		theta = EXPO.d * Math.PI; // pi is the polar angle for -1
		x = Math.cos(theta);
		y = Math.sin(theta);
		if (Math.abs(x) < 1e-12)
			x = 0.0;
		if (Math.abs(y) < 1e-12)
			y = 0.0;
		push_double(x);
		push_double(y);
		push(imaginaryunit);
		multiply();
		add();
		return;
	}

	// integer exponent?

	if (isinteger(EXPO)) {
		if (EXPO.a % 2 == 1) {
			// odd exponent
			if (isdouble(BASE))
				push_double(-1.0); // BASE = -1.0, keep double
			else
				push_integer(-1);
		} else {
			// even exponent
			if (isdouble(BASE))
				push_double(1.0); // BASE = -1.0, keep double
			else
				push_integer(1);
		}
		return;
	}

	a = Math.abs(EXPO.a);
	b = Math.abs(EXPO.b);

	q = Math.floor(a / b);
	r = a % b;
	t = r + r;

	if (t < b) {
		a = r;
		c = 1;
		s = 1;
	}

	if (t == b) {
		if (EXPO.a >= 0) {
			a = r;
			c = 1;
			s = 1;
		} else {
			a = b - r;
			c = -1;
			s = -1;
		}
	}

	if (t > b) {
		a = b - r;
		c = -1;
		s = -1;
	}

	if (q % 2 == 1)
		c *= -1;

	if (EXPO.a < 0)
		s *= -1;

	push_symbol(POWER);
	push_integer(-1);

	if (s == 1)
		push_rational(a, b);
	else
		push_rational(-a, b);

	list(3);

	if (c == -1) {
		push_symbol(MULTIPLY);
		swap();
		if (isdouble(BASE))
			push_double(-1.0); // BASE = -1.0, keep double
		else
			push_integer(-1);
		swap();
		list(3);
	} else if (isdouble(BASE)) {
		// BASE = -1.0, keep double
		push_symbol(MULTIPLY);
		swap();
		push_double(1.0);
		swap();
		list(3);
	}
}
function
power_natural_number(BASE, EXPO)
{
	var x, y;

	// exp(x + i y) = exp(x) (cos(y) + i sin(y))

	if (isdoublez(EXPO)) {
		if (car(EXPO) == symbol(ADD)) {
			x = cadr(EXPO).d;
			y = cadaddr(EXPO).d;
		} else {
			x = 0.0;
			y = cadr(EXPO).d;
		}
		push_double(Math.exp(x));
		push_double(y);
		cos();
		push(imaginaryunit);
		push_double(y);
		sin();
		multiply();
		add();
		multiply();
		return;
	}

	// e^log(expr) -> expr

	if (car(EXPO) == symbol(LOG)) {
		push(cadr(EXPO));
		return;
	}

	if (simplify_polar_expr(EXPO))
		return;

	// none of the above

	push_symbol(POWER);
	push(BASE);
	push(EXPO);
	list(3);
}
// BASE and EXPO are numbers

function
power_numbers(BASE, EXPO)
{
	var base, expo;

	if (isinteger(EXPO)) {
		power_simple(BASE, EXPO);
		return;
	}

	if (isminusone(BASE)) {
		power_minusone(BASE, EXPO);
		return;
	}

	if (isnegativenumber(BASE)) {
		power_minusone(BASE, EXPO);
		push(BASE);
		negate();
		BASE = pop();
		power_numbers(BASE, EXPO);
		multiply();
		return;
	}

	if (isrational(BASE) && isrational(EXPO)) {
		power_rationals(BASE, EXPO);
		return;
	}

	push(BASE);
	base = pop_double();

	push(EXPO);
	expo = pop_double();

	push_double(Math.pow(base, expo));
}
function
power_precheck(BASE, EXPO)
{
	// 1^expr = expr^0 = 1

	if (isplusone(BASE) || iszero(EXPO)) {
		if (isdouble(BASE) || isdouble(EXPO))
			push_double(1.0);
		else
			push_integer(1);
		return 1;
	}

	// expr^1 = expr

	if (isplusone(EXPO)) {
		push(BASE);
		return 1;
	}

	// 0^expr

	if (iszero(BASE)) {
		if (isnum(EXPO)) {
			if (isdouble(BASE) || isdouble(EXPO))
				push_double(0.0);
			else
				push_integer(0);
		} else {
			push_symbol(POWER);
			push(BASE);
			push(EXPO);
			list(3);
		}
		return 1;
	}

	return 0;
}
function
power_rationals(BASE, EXPO)
{
	var d, h, i, n, base, expo, p1, COEFF;

	h = stack.length;

	// put factors on stack

	push_symbol(POWER);
	push(BASE);
	push(EXPO);
	list(3);

	factor();

	// normalize factors

	n = stack.length;

	for (i = h; i < n; i++) {
		p1 = stack[i];
		if (car(p1) == symbol(POWER)) {
			BASE = cadr(p1);
			EXPO = caddr(p1);
			power_rationals_nib(BASE, EXPO);
			stack[i] = pop(); // 1 or 2 factors on stack, fill hole with tos
		}
	}

	// combine numbers (leaves radicals on stack)

	COEFF = one;

	n = stack.length;

	for (i = h; i < n; i++) {
		p1 = stack[i];
		if (isnum(p1)) {
			push(COEFF);
			push(p1);
			multiply();
			COEFF = pop();
			stack.splice(i, 1);
			i--;
			n--;
		}
	}

	// float radicals if COEFF is double (can happen due to auto conversion of rational to double)

	n = stack.length;

	if (isdouble(COEFF) && n - h > 0) {
		d = COEFF.d;
		n = stack.length;
		for (i = h; i < n; i++) {
			p1 = stack[i];
			BASE = cadr(p1);
			EXPO = caddr(p1);
			base = BASE.a / BASE.b;
			expo = EXPO.a / EXPO.b;
			d *= Math.pow(base, expo);
		}
		stack.splice(h); // pop all
		push_double(d);
		COEFF = pop();
	}

	// finalize

	n = stack.length - h;

	if (n == 0 || !isplusone(COEFF) || isdouble(COEFF)) {
		push(COEFF);
		n++;
	}

	if (n == 1)
		return;

	sort_factors(h);
	list(n);
	push_symbol(MULTIPLY);
	swap();
	cons();
}
function
power_rationals_nib(BASE, EXPO) // BASE is prime number, EXPO is rational
{
	var n, q, r;

	// integer power?

	if (EXPO.b == 1) {

		n = Math.pow(BASE.a, Math.abs(EXPO.a));

		if (EXPO.a < 0)
			push_rational(1, n); // reciprocate
		else
			push_rational(n, 1);

		return;
	}

	q = Math.floor(Math.abs(EXPO.a) / EXPO.b);
	r = Math.abs(EXPO.a) % EXPO.b;

	// whole part

	if (q > 0) {

		n = Math.pow(BASE.a, q);

		if (EXPO.a < 0)
			push_rational(1, n); // reciprocate
		else
			push_rational(n, 1);
	}

	// remainder (BASE is prime hence no roots)

	if (EXPO.a < 0)
		r = -r;

	push_symbol(POWER);
	push(BASE);
	push_rational(r, EXPO.b);
	list(3);
}
function
power_simple(BASE, EXPO) // EXPO is an integer
{
	var a, b, base, expo;

	if (isrational(BASE) && isrational(EXPO)) {

		a = Math.pow(BASE.a, Math.abs(EXPO.a));
		b = Math.pow(BASE.b, Math.abs(EXPO.a));

		if (isnegativenumber(EXPO))
			push_rational(b, a); // reciprocate
		else
			push_rational(a, b);

		return;

	}

	push(BASE);
	base = pop_double();

	push(EXPO);
	expo = pop_double();

	push_double(Math.pow(base, expo));
}
function
power_sum(BASE, EXPO)
{
	var h, i, n, p3, p4;

	if (expanding == 0 || !isnum(EXPO)) {
		push_symbol(POWER);
		push(BASE);
		push(EXPO);
		list(3);
		return;
	}

	if (isrational(EXPO))
		n = EXPO.a / EXPO.b;
	else
		n = EXPO.d;

	if (n < 0 || n != Math.floor(n)) {
		push_symbol(POWER);
		push(BASE);
		push(EXPO);
		list(3);
		return;
	}

	// square the sum first (prevents infinite loop through multiply)

	h = stack.length;

	p3 = cdr(BASE);

	while (iscons(p3)) {
		p4 = cdr(BASE);
		while (iscons(p4)) {
			push(car(p3));
			push(car(p4));
			multiply();
			p4 = cdr(p4);
		}
		p3 = cdr(p3);
	}

	add_terms(stack.length - h);

	// continue up to power n

	for (i = 2; i < n; i++) {
		push(BASE);
		multiply();
	}
}
function
power_tensor(BASE, EXPO)
{
	var i, j, k, n;

	// first and last dims must be equal

	n = BASE.dim.length;

	if (BASE.dim[0] != BASE.dim[n - 1]) {
		push_symbol(POWER);
		push(BASE);
		push(EXPO);
		list(3);
		return;
	}

	if (!isnum(EXPO)) {
		push_symbol(POWER);
		push(BASE);
		push(EXPO);
		list(3);
		return;
	}

	if (isrational(EXPO))
		n = EXPO.a / EXPO.b;
	else
		n = EXPO.d;

	if (n < 0 || n != Math.floor(n)) {
		push_symbol(POWER);
		push(BASE);
		push(EXPO);
		list(3);
		return;
	}

	if (n == 0) {
		n = BASE.dim[0];
		BASE = alloc_tensor();
		BASE.dim[0] = n;
		BASE.dim[1] = n;
		k = 0;
		for (i = 0; i < n; i++)
			for (j = 0; j < n; j++)
				if (i == j)
					BASE.elem[k++] = one;
				else
					BASE.elem[k++] = zero;
		return;
	}

	push(BASE);

	for (i = 1; i < n; i++) {
		push(BASE);
		inner();
	}
}
function
prefixform(p)
{
	if (iscons(p)) {
		var s = "(";
		s += prefixform(car(p));
		p = cdr(p);
		while (iscons(p)) {
			s += " ";
			s += prefixform(car(p));
			p = cdr(p);
		}
		s += ")";
		return s;
	}

	if (isinteger(p))
		return p.a.toFixed(0);

	if (isrational(p))
		return p.a.toFixed(0) + "/" + p.b.toFixed(0);

	if (isdouble(p))
		return p.d.toPrecision(6);

	if (issymbol(p))
		return "" + p.printname;

	if (isstring(p))
		return "'" + p.string + "'";

	if (istensor(p))
		return "(tensor)";

	return "?";
}
function
prep_symbol_equals(p1, p2)
{
	push(p2);

	if (!issymbol(p1))
		return;

	if (p1 == p2)
		return; // A = A

	if (iskeyword(p1))
		return; // keyword like "float"

	if (p1 == symbol(SYMBOL_I) && isimaginaryunit(p2))
		return;

	if (p1 == symbol(SYMBOL_J) && isimaginaryunit(p2))
		return;

	p2 = pop();

	push_symbol(SETQ);
	push(p1);
	push(p2);
	list(3);
}
var primetab = [
2,3,5,7,11,13,17,19,
23,29,31,37,41,43,47,53,
59,61,67,71,73,79,83,89,
97,101,103,107,109,113,127,131,
137,139,149,151,157,163,167,173,
179,181,191,193,197,199,211,223,
227,229,233,239,241,251,257,263,
269,271,277,281,283,293,307,311,
313,317,331,337,347,349,353,359,
367,373,379,383,389,397,401,409,
419,421,431,433,439,443,449,457,
461,463,467,479,487,491,499,503,
509,521,523,541,547,557,563,569,
571,577,587,593,599,601,607,613,
617,619,631,641,643,647,653,659,
661,673,677,683,691,701,709,719,
727,733,739,743,751,757,761,769,
773,787,797,809,811,821,823,827,
829,839,853,857,859,863,877,881,
883,887,907,911,919,929,937,941,
947,953,967,971,977,983,991,997,
1009,1013,1019,1021,1031,1033,1039,1049,
1051,1061,1063,1069,1087,1091,1093,1097,
1103,1109,1117,1123,1129,1151,1153,1163,
1171,1181,1187,1193,1201,1213,1217,1223,
1229,1231,1237,1249,1259,1277,1279,1283,
1289,1291,1297,1301,1303,1307,1319,1321,
1327,1361,1367,1373,1381,1399,1409,1423,
1427,1429,1433,1439,1447,1451,1453,1459,
1471,1481,1483,1487,1489,1493,1499,1511,
1523,1531,1543,1549,1553,1559,1567,1571,
1579,1583,1597,1601,1607,1609,1613,1619,
1621,1627,1637,1657,1663,1667,1669,1693,
1697,1699,1709,1721,1723,1733,1741,1747,
1753,1759,1777,1783,1787,1789,1801,1811,
1823,1831,1847,1861,1867,1871,1873,1877,
1879,1889,1901,1907,1913,1931,1933,1949,
1951,1973,1979,1987,1993,1997,1999,2003,
2011,2017,2027,2029,2039,2053,2063,2069,
2081,2083,2087,2089,2099,2111,2113,2129,
2131,2137,2141,2143,2153,2161,2179,2203,
2207,2213,2221,2237,2239,2243,2251,2267,
2269,2273,2281,2287,2293,2297,2309,2311,
2333,2339,2341,2347,2351,2357,2371,2377,
2381,2383,2389,2393,2399,2411,2417,2423,
2437,2441,2447,2459,2467,2473,2477,2503,
2521,2531,2539,2543,2549,2551,2557,2579,
2591,2593,2609,2617,2621,2633,2647,2657,
2659,2663,2671,2677,2683,2687,2689,2693,
2699,2707,2711,2713,2719,2729,2731,2741,
2749,2753,2767,2777,2789,2791,2797,2801,
2803,2819,2833,2837,2843,2851,2857,2861,
2879,2887,2897,2903,2909,2917,2927,2939,
2953,2957,2963,2969,2971,2999,3001,3011,
3019,3023,3037,3041,3049,3061,3067,3079,
3083,3089,3109,3119,3121,3137,3163,3167,
3169,3181,3187,3191,3203,3209,3217,3221,
3229,3251,3253,3257,3259,3271,3299,3301,
3307,3313,3319,3323,3329,3331,3343,3347,
3359,3361,3371,3373,3389,3391,3407,3413,
3433,3449,3457,3461,3463,3467,3469,3491,
3499,3511,3517,3527,3529,3533,3539,3541,
3547,3557,3559,3571,3581,3583,3593,3607,
3613,3617,3623,3631,3637,3643,3659,3671,
3673,3677,3691,3697,3701,3709,3719,3727,
3733,3739,3761,3767,3769,3779,3793,3797,
3803,3821,3823,3833,3847,3851,3853,3863,
3877,3881,3889,3907,3911,3917,3919,3923,
3929,3931,3943,3947,3967,3989,4001,4003,
4007,4013,4019,4021,4027,4049,4051,4057,
4073,4079,4091,4093,4099,4111,4127,4129,
4133,4139,4153,4157,4159,4177,4201,4211,
4217,4219,4229,4231,4241,4243,4253,4259,
4261,4271,4273,4283,4289,4297,4327,4337,
4339,4349,4357,4363,4373,4391,4397,4409,
4421,4423,4441,4447,4451,4457,4463,4481,
4483,4493,4507,4513,4517,4519,4523,4547,
4549,4561,4567,4583,4591,4597,4603,4621,
4637,4639,4643,4649,4651,4657,4663,4673,
4679,4691,4703,4721,4723,4729,4733,4751,
4759,4783,4787,4789,4793,4799,4801,4813,
4817,4831,4861,4871,4877,4889,4903,4909,
4919,4931,4933,4937,4943,4951,4957,4967,
4969,4973,4987,4993,4999,5003,5009,5011,
5021,5023,5039,5051,5059,5077,5081,5087,
5099,5101,5107,5113,5119,5147,5153,5167,
5171,5179,5189,5197,5209,5227,5231,5233,
5237,5261,5273,5279,5281,5297,5303,5309,
5323,5333,5347,5351,5381,5387,5393,5399,
5407,5413,5417,5419,5431,5437,5441,5443,
5449,5471,5477,5479,5483,5501,5503,5507,
5519,5521,5527,5531,5557,5563,5569,5573,
5581,5591,5623,5639,5641,5647,5651,5653,
5657,5659,5669,5683,5689,5693,5701,5711,
5717,5737,5741,5743,5749,5779,5783,5791,
5801,5807,5813,5821,5827,5839,5843,5849,
5851,5857,5861,5867,5869,5879,5881,5897,
5903,5923,5927,5939,5953,5981,5987,6007,
6011,6029,6037,6043,6047,6053,6067,6073,
6079,6089,6091,6101,6113,6121,6131,6133,
6143,6151,6163,6173,6197,6199,6203,6211,
6217,6221,6229,6247,6257,6263,6269,6271,
6277,6287,6299,6301,6311,6317,6323,6329,
6337,6343,6353,6359,6361,6367,6373,6379,
6389,6397,6421,6427,6449,6451,6469,6473,
6481,6491,6521,6529,6547,6551,6553,6563,
6569,6571,6577,6581,6599,6607,6619,6637,
6653,6659,6661,6673,6679,6689,6691,6701,
6703,6709,6719,6733,6737,6761,6763,6779,
6781,6791,6793,6803,6823,6827,6829,6833,
6841,6857,6863,6869,6871,6883,6899,6907,
6911,6917,6947,6949,6959,6961,6967,6971,
6977,6983,6991,6997,7001,7013,7019,7027,
7039,7043,7057,7069,7079,7103,7109,7121,
7127,7129,7151,7159,7177,7187,7193,7207,
7211,7213,7219,7229,7237,7243,7247,7253,
7283,7297,7307,7309,7321,7331,7333,7349,
7351,7369,7393,7411,7417,7433,7451,7457,
7459,7477,7481,7487,7489,7499,7507,7517,
7523,7529,7537,7541,7547,7549,7559,7561,
7573,7577,7583,7589,7591,7603,7607,7621,
7639,7643,7649,7669,7673,7681,7687,7691,
7699,7703,7717,7723,7727,7741,7753,7757,
7759,7789,7793,7817,7823,7829,7841,7853,
7867,7873,7877,7879,7883,7901,7907,7919,
7927,7933,7937,7949,7951,7963,7993,8009,
8011,8017,8039,8053,8059,8069,8081,8087,
8089,8093,8101,8111,8117,8123,8147,8161,
8167,8171,8179,8191,8209,8219,8221,8231,
8233,8237,8243,8263,8269,8273,8287,8291,
8293,8297,8311,8317,8329,8353,8363,8369,
8377,8387,8389,8419,8423,8429,8431,8443,
8447,8461,8467,8501,8513,8521,8527,8537,
8539,8543,8563,8573,8581,8597,8599,8609,
8623,8627,8629,8641,8647,8663,8669,8677,
8681,8689,8693,8699,8707,8713,8719,8731,
8737,8741,8747,8753,8761,8779,8783,8803,
8807,8819,8821,8831,8837,8839,8849,8861,
8863,8867,8887,8893,8923,8929,8933,8941,
8951,8963,8969,8971,8999,9001,9007,9011,
9013,9029,9041,9043,9049,9059,9067,9091,
9103,9109,9127,9133,9137,9151,9157,9161,
9173,9181,9187,9199,9203,9209,9221,9227,
9239,9241,9257,9277,9281,9283,9293,9311,
9319,9323,9337,9341,9343,9349,9371,9377,
9391,9397,9403,9413,9419,9421,9431,9433,
9437,9439,9461,9463,9467,9473,9479,9491,
9497,9511,9521,9533,9539,9547,9551,9587,
9601,9613,9619,9623,9629,9631,9643,9649,
9661,9677,9679,9689,9697,9719,9721,9733,
9739,9743,9749,9767,9769,9781,9787,9791,
9803,9811,9817,9829,9833,9839,9851,9857,
9859,9871,9883,9887,9901,9907,9923,9929,
9931,9941,9949,9967,9973,10007,10009,10037,
10039,10061,10067,10069,10079,10091,10093,10099,
10103,10111,10133,10139,10141,10151,10159,10163,
10169,10177,10181,10193,10211,10223,10243,10247,
10253,10259,10267,10271,10273,10289,10301,10303,
10313,10321,10331,10333,10337,10343,10357,10369,
10391,10399,10427,10429,10433,10453,10457,10459,
10463,10477,10487,10499,10501,10513,10529,10531,
10559,10567,10589,10597,10601,10607,10613,10627,
10631,10639,10651,10657,10663,10667,10687,10691,
10709,10711,10723,10729,10733,10739,10753,10771,
10781,10789,10799,10831,10837,10847,10853,10859,
10861,10867,10883,10889,10891,10903,10909,10937,
10939,10949,10957,10973,10979,10987,10993,11003,
11027,11047,11057,11059,11069,11071,11083,11087,
11093,11113,11117,11119,11131,11149,11159,11161,
11171,11173,11177,11197,11213,11239,11243,11251,
11257,11261,11273,11279,11287,11299,11311,11317,
11321,11329,11351,11353,11369,11383,11393,11399,
11411,11423,11437,11443,11447,11467,11471,11483,
11489,11491,11497,11503,11519,11527,11549,11551,
11579,11587,11593,11597,11617,11621,11633,11657,
11677,11681,11689,11699,11701,11717,11719,11731,
11743,11777,11779,11783,11789,11801,11807,11813,
11821,11827,11831,11833,11839,11863,11867,11887,
11897,11903,11909,11923,11927,11933,11939,11941,
11953,11959,11969,11971,11981,11987,12007,12011,
12037,12041,12043,12049,12071,12073,12097,12101,
12107,12109,12113,12119,12143,12149,12157,12161,
12163,12197,12203,12211,12227,12239,12241,12251,
12253,12263,12269,12277,12281,12289,12301,12323,
12329,12343,12347,12373,12377,12379,12391,12401,
12409,12413,12421,12433,12437,12451,12457,12473,
12479,12487,12491,12497,12503,12511,12517,12527,
12539,12541,12547,12553,12569,12577,12583,12589,
12601,12611,12613,12619,12637,12641,12647,12653,
12659,12671,12689,12697,12703,12713,12721,12739,
12743,12757,12763,12781,12791,12799,12809,12821,
12823,12829,12841,12853,12889,12893,12899,12907,
12911,12917,12919,12923,12941,12953,12959,12967,
12973,12979,12983,13001,13003,13007,13009,13033,
13037,13043,13049,13063,13093,13099,13103,13109,
13121,13127,13147,13151,13159,13163,13171,13177,
13183,13187,13217,13219,13229,13241,13249,13259,
13267,13291,13297,13309,13313,13327,13331,13337,
13339,13367,13381,13397,13399,13411,13417,13421,
13441,13451,13457,13463,13469,13477,13487,13499,
13513,13523,13537,13553,13567,13577,13591,13597,
13613,13619,13627,13633,13649,13669,13679,13681,
13687,13691,13693,13697,13709,13711,13721,13723,
13729,13751,13757,13759,13763,13781,13789,13799,
13807,13829,13831,13841,13859,13873,13877,13879,
13883,13901,13903,13907,13913,13921,13931,13933,
13963,13967,13997,13999,14009,14011,14029,14033,
14051,14057,14071,14081,14083,14087,14107,14143,
14149,14153,14159,14173,14177,14197,14207,14221,
14243,14249,14251,14281,14293,14303,14321,14323,
14327,14341,14347,14369,14387,14389,14401,14407,
14411,14419,14423,14431,14437,14447,14449,14461,
14479,14489,14503,14519,14533,14537,14543,14549,
14551,14557,14561,14563,14591,14593,14621,14627,
14629,14633,14639,14653,14657,14669,14683,14699,
14713,14717,14723,14731,14737,14741,14747,14753,
14759,14767,14771,14779,14783,14797,14813,14821,
14827,14831,14843,14851,14867,14869,14879,14887,
14891,14897,14923,14929,14939,14947,14951,14957,
14969,14983,15013,15017,15031,15053,15061,15073,
15077,15083,15091,15101,15107,15121,15131,15137,
15139,15149,15161,15173,15187,15193,15199,15217,
15227,15233,15241,15259,15263,15269,15271,15277,
15287,15289,15299,15307,15313,15319,15329,15331,
15349,15359,15361,15373,15377,15383,15391,15401,
15413,15427,15439,15443,15451,15461,15467,15473,
15493,15497,15511,15527,15541,15551,15559,15569,
15581,15583,15601,15607,15619,15629,15641,15643,
15647,15649,15661,15667,15671,15679,15683,15727,
15731,15733,15737,15739,15749,15761,15767,15773,
15787,15791,15797,15803,15809,15817,15823,15859,
15877,15881,15887,15889,15901,15907,15913,15919,
15923,15937,15959,15971,15973,15991,16001,16007,
16033,16057,16061,16063,16067,16069,16073,16087,
16091,16097,16103,16111,16127,16139,16141,16183,
16187,16189,16193,16217,16223,16229,16231,16249,
16253,16267,16273,16301,16319,16333,16339,16349,
16361,16363,16369,16381,16411,16417,16421,16427,
16433,16447,16451,16453,16477,16481,16487,16493,
16519,16529,16547,16553,16561,16567,16573,16603,
16607,16619,16631,16633,16649,16651,16657,16661,
16673,16691,16693,16699,16703,16729,16741,16747,
16759,16763,16787,16811,16823,16829,16831,16843,
16871,16879,16883,16889,16901,16903,16921,16927,
16931,16937,16943,16963,16979,16981,16987,16993,
17011,17021,17027,17029,17033,17041,17047,17053,
17077,17093,17099,17107,17117,17123,17137,17159,
17167,17183,17189,17191,17203,17207,17209,17231,
17239,17257,17291,17293,17299,17317,17321,17327,
17333,17341,17351,17359,17377,17383,17387,17389,
17393,17401,17417,17419,17431,17443,17449,17467,
17471,17477,17483,17489,17491,17497,17509,17519,
17539,17551,17569,17573,17579,17581,17597,17599,
17609,17623,17627,17657,17659,17669,17681,17683,
17707,17713,17729,17737,17747,17749,17761,17783,
17789,17791,17807,17827,17837,17839,17851,17863,
17881,17891,17903,17909,17911,17921,17923,17929,
17939,17957,17959,17971,17977,17981,17987,17989,
18013,18041,18043,18047,18049,18059,18061,18077,
18089,18097,18119,18121,18127,18131,18133,18143,
18149,18169,18181,18191,18199,18211,18217,18223,
18229,18233,18251,18253,18257,18269,18287,18289,
18301,18307,18311,18313,18329,18341,18353,18367,
18371,18379,18397,18401,18413,18427,18433,18439,
18443,18451,18457,18461,18481,18493,18503,18517,
18521,18523,18539,18541,18553,18583,18587,18593,
18617,18637,18661,18671,18679,18691,18701,18713,
18719,18731,18743,18749,18757,18773,18787,18793,
18797,18803,18839,18859,18869,18899,18911,18913,
18917,18919,18947,18959,18973,18979,19001,19009,
19013,19031,19037,19051,19069,19073,19079,19081,
19087,19121,19139,19141,19157,19163,19181,19183,
19207,19211,19213,19219,19231,19237,19249,19259,
19267,19273,19289,19301,19309,19319,19333,19373,
19379,19381,19387,19391,19403,19417,19421,19423,
19427,19429,19433,19441,19447,19457,19463,19469,
19471,19477,19483,19489,19501,19507,19531,19541,
19543,19553,19559,19571,19577,19583,19597,19603,
19609,19661,19681,19687,19697,19699,19709,19717,
19727,19739,19751,19753,19759,19763,19777,19793,
19801,19813,19819,19841,19843,19853,19861,19867,
19889,19891,19913,19919,19927,19937,19949,19961,
19963,19973,19979,19991,19993,19997,20011,20021,
20023,20029,20047,20051,20063,20071,20089,20101,
20107,20113,20117,20123,20129,20143,20147,20149,
20161,20173,20177,20183,20201,20219,20231,20233,
20249,20261,20269,20287,20297,20323,20327,20333,
20341,20347,20353,20357,20359,20369,20389,20393,
20399,20407,20411,20431,20441,20443,20477,20479,
20483,20507,20509,20521,20533,20543,20549,20551,
20563,20593,20599,20611,20627,20639,20641,20663,
20681,20693,20707,20717,20719,20731,20743,20747,
20749,20753,20759,20771,20773,20789,20807,20809,
20849,20857,20873,20879,20887,20897,20899,20903,
20921,20929,20939,20947,20959,20963,20981,20983,
21001,21011,21013,21017,21019,21023,21031,21059,
21061,21067,21089,21101,21107,21121,21139,21143,
21149,21157,21163,21169,21179,21187,21191,21193,
21211,21221,21227,21247,21269,21277,21283,21313,
21317,21319,21323,21341,21347,21377,21379,21383,
21391,21397,21401,21407,21419,21433,21467,21481,
21487,21491,21493,21499,21503,21517,21521,21523,
21529,21557,21559,21563,21569,21577,21587,21589,
21599,21601,21611,21613,21617,21647,21649,21661,
21673,21683,21701,21713,21727,21737,21739,21751,
21757,21767,21773,21787,21799,21803,21817,21821,
21839,21841,21851,21859,21863,21871,21881,21893,
21911,21929,21937,21943,21961,21977,21991,21997,
22003,22013,22027,22031,22037,22039,22051,22063,
22067,22073,22079,22091,22093,22109,22111,22123,
22129,22133,22147,22153,22157,22159,22171,22189,
22193,22229,22247,22259,22271,22273,22277,22279,
22283,22291,22303,22307,22343,22349,22367,22369,
22381,22391,22397,22409,22433,22441,22447,22453,
22469,22481,22483,22501,22511,22531,22541,22543,
22549,22567,22571,22573,22613,22619,22621,22637,
22639,22643,22651,22669,22679,22691,22697,22699,
22709,22717,22721,22727,22739,22741,22751,22769,
22777,22783,22787,22807,22811,22817,22853,22859,
22861,22871,22877,22901,22907,22921,22937,22943,
22961,22963,22973,22993,23003,23011,23017,23021,
23027,23029,23039,23041,23053,23057,23059,23063,
23071,23081,23087,23099,23117,23131,23143,23159,
23167,23173,23189,23197,23201,23203,23209,23227,
23251,23269,23279,23291,23293,23297,23311,23321,
23327,23333,23339,23357,23369,23371,23399,23417,
23431,23447,23459,23473,23497,23509,23531,23537,
23539,23549,23557,23561,23563,23567,23581,23593,
23599,23603,23609,23623,23627,23629,23633,23663,
23669,23671,23677,23687,23689,23719,23741,23743,
23747,23753,23761,23767,23773,23789,23801,23813,
23819,23827,23831,23833,23857,23869,23873,23879,
23887,23893,23899,23909,23911,23917,23929,23957,
23971,23977,23981,23993,24001,24007,24019,24023,
24029,24043,24049,24061,24071,24077,24083,24091,
24097,24103,24107,24109,24113,24121,24133,24137,
24151,24169,24179,24181,24197,24203,24223,24229,
24239,24247,24251,24281,24317,24329,24337,24359,
24371,24373,24379,24391,24407,24413,24419,24421,
24439,24443,24469,24473,24481,24499,24509,24517,
24527,24533,24547,24551,24571,24593,24611,24623,
24631,24659,24671,24677,24683,24691,24697,24709,
24733,24749,24763,24767,24781,24793,24799,24809,
24821,24841,24847,24851,24859,24877,24889,24907,
24917,24919,24923,24943,24953,24967,24971,24977,
24979,24989,25013,25031,25033,25037,25057,25073,
25087,25097,25111,25117,25121,25127,25147,25153,
25163,25169,25171,25183,25189,25219,25229,25237,
25243,25247,25253,25261,25301,25303,25307,25309,
25321,25339,25343,25349,25357,25367,25373,25391,
25409,25411,25423,25439,25447,25453,25457,25463,
25469,25471,25523,25537,25541,25561,25577,25579,
25583,25589,25601,25603,25609,25621,25633,25639,
25643,25657,25667,25673,25679,25693,25703,25717,
25733,25741,25747,25759,25763,25771,25793,25799,
25801,25819,25841,25847,25849,25867,25873,25889,
25903,25913,25919,25931,25933,25939,25943,25951,
25969,25981,25997,25999,26003,26017,26021,26029,
26041,26053,26083,26099,26107,26111,26113,26119,
26141,26153,26161,26171,26177,26183,26189,26203,
26209,26227,26237,26249,26251,26261,26263,26267,
26293,26297,26309,26317,26321,26339,26347,26357,
26371,26387,26393,26399,26407,26417,26423,26431,
26437,26449,26459,26479,26489,26497,26501,26513,
26539,26557,26561,26573,26591,26597,26627,26633,
26641,26647,26669,26681,26683,26687,26693,26699,
26701,26711,26713,26717,26723,26729,26731,26737,
26759,26777,26783,26801,26813,26821,26833,26839,
26849,26861,26863,26879,26881,26891,26893,26903,
26921,26927,26947,26951,26953,26959,26981,26987,
26993,27011,27017,27031,27043,27059,27061,27067,
27073,27077,27091,27103,27107,27109,27127,27143,
27179,27191,27197,27211,27239,27241,27253,27259,
27271,27277,27281,27283,27299,27329,27337,27361,
27367,27397,27407,27409,27427,27431,27437,27449,
27457,27479,27481,27487,27509,27527,27529,27539,
27541,27551,27581,27583,27611,27617,27631,27647,
27653,27673,27689,27691,27697,27701,27733,27737,
27739,27743,27749,27751,27763,27767,27773,27779,
27791,27793,27799,27803,27809,27817,27823,27827,
27847,27851,27883,27893,27901,27917,27919,27941,
27943,27947,27953,27961,27967,27983,27997,28001,
28019,28027,28031,28051,28057,28069,28081,28087,
28097,28099,28109,28111,28123,28151,28163,28181,
28183,28201,28211,28219,28229,28277,28279,28283,
28289,28297,28307,28309,28319,28349,28351,28387,
28393,28403,28409,28411,28429,28433,28439,28447,
28463,28477,28493,28499,28513,28517,28537,28541,
28547,28549,28559,28571,28573,28579,28591,28597,
28603,28607,28619,28621,28627,28631,28643,28649,
28657,28661,28663,28669,28687,28697,28703,28711,
28723,28729,28751,28753,28759,28771,28789,28793,
28807,28813,28817,28837,28843,28859,28867,28871,
28879,28901,28909,28921,28927,28933,28949,28961,
28979,29009,29017,29021,29023,29027,29033,29059,
29063,29077,29101,29123,29129,29131,29137,29147,
29153,29167,29173,29179,29191,29201,29207,29209,
29221,29231,29243,29251,29269,29287,29297,29303,
29311,29327,29333,29339,29347,29363,29383,29387,
29389,29399,29401,29411,29423,29429,29437,29443,
29453,29473,29483,29501,29527,29531,29537,29567,
29569,29573,29581,29587,29599,29611,29629,29633,
29641,29663,29669,29671,29683,29717,29723,29741,
29753,29759,29761,29789,29803,29819,29833,29837,
29851,29863,29867,29873,29879,29881,29917,29921,
29927,29947,29959,29983,29989,30011,30013,30029,
30047,30059,30071,30089,30091,30097,30103,30109,
30113,30119,30133,30137,30139,30161,30169,30181,
30187,30197,30203,30211,30223,30241,30253,30259,
30269,30271,30293,30307,30313,30319,30323,30341,
30347,30367,30389,30391,30403,30427,30431,30449,
30467,30469,30491,30493,30497,30509,30517,30529,
30539,30553,30557,30559,30577,30593,30631,30637,
30643,30649,30661,30671,30677,30689,30697,30703,
30707,30713,30727,30757,30763,30773,30781,30803,
30809,30817,30829,30839,30841,30851,30853,30859,
30869,30871,30881,30893,30911,30931,30937,30941,
30949,30971,30977,30983,31013,31019,31033,31039,
31051,31063,31069,31079,31081,31091,31121,31123,
31139,31147,31151,31153,31159,31177,31181,31183,
31189,31193,31219,31223,31231,31237,31247,31249,
31253,31259,31267,31271,31277,31307,31319,31321,
31327,31333,31337,31357,31379,31387,31391,31393,
31397,31469,31477,31481,31489,31511,31513,31517,
31531,31541,31543,31547,31567,31573,31583,31601,
31607,31627,31643,31649,31657,31663,31667,31687,
31699,31721,31723,31727,31729,31741,31751,31769,
31771,31793,31799,31817,31847,31849,31859,31873,
31883,31891,31907,31957,31963,31973,31981,31991,
32003,32009,32027,32029,32051,32057,32059,32063,
32069,32077,32083,32089,32099,32117,32119,32141,
32143,32159,32173,32183,32189,32191,32203,32213,
32233,32237,32251,32257,32261,32297,32299,32303,
32309,32321,32323,32327,32341,32353,32359,32363,
32369,32371,32377,32381,32401,32411,32413,32423,
32429,32441,32443,32467,32479,32491,32497,32503,
32507,32531,32533,32537,32561,32563,32569,32573,
32579,32587,32603,32609,32611,32621,32633,32647,
32653,32687,32693,32707,32713,32717,32719,32749,
32771,32779,32783,32789,32797,32801,32803,32831,
32833,32839,32843,32869,32887,32909,32911,32917,
32933,32939,32941,32957,32969,32971,32983,32987,
32993,32999,33013,33023,33029,33037,33049,33053,
33071,33073,33083,33091,33107,33113,33119,33149,
33151,33161,33179,33181,33191,33199,33203,33211,
33223,33247,33287,33289,33301,33311,33317,33329,
33331,33343,33347,33349,33353,33359,33377,33391,
33403,33409,33413,33427,33457,33461,33469,33479,
33487,33493,33503,33521,33529,33533,33547,33563,
33569,33577,33581,33587,33589,33599,33601,33613,
33617,33619,33623,33629,33637,33641,33647,33679,
33703,33713,33721,33739,33749,33751,33757,33767,
33769,33773,33791,33797,33809,33811,33827,33829,
33851,33857,33863,33871,33889,33893,33911,33923,
33931,33937,33941,33961,33967,33997,34019,34031,
34033,34039,34057,34061,34123,34127,34129,34141,
34147,34157,34159,34171,34183,34211,34213,34217,
34231,34253,34259,34261,34267,34273,34283,34297,
34301,34303,34313,34319,34327,34337,34351,34361,
34367,34369,34381,34403,34421,34429,34439,34457,
34469,34471,34483,34487,34499,34501,34511,34513,
34519,34537,34543,34549,34583,34589,34591,34603,
34607,34613,34631,34649,34651,34667,34673,34679,
34687,34693,34703,34721,34729,34739,34747,34757,
34759,34763,34781,34807,34819,34841,34843,34847,
34849,34871,34877,34883,34897,34913,34919,34939,
34949,34961,34963,34981,35023,35027,35051,35053,
35059,35069,35081,35083,35089,35099,35107,35111,
35117,35129,35141,35149,35153,35159,35171,35201,
35221,35227,35251,35257,35267,35279,35281,35291,
35311,35317,35323,35327,35339,35353,35363,35381,
35393,35401,35407,35419,35423,35437,35447,35449,
35461,35491,35507,35509,35521,35527,35531,35533,
35537,35543,35569,35573,35591,35593,35597,35603,
35617,35671,35677,35729,35731,35747,35753,35759,
35771,35797,35801,35803,35809,35831,35837,35839,
35851,35863,35869,35879,35897,35899,35911,35923,
35933,35951,35963,35969,35977,35983,35993,35999,
36007,36011,36013,36017,36037,36061,36067,36073,
36083,36097,36107,36109,36131,36137,36151,36161,
36187,36191,36209,36217,36229,36241,36251,36263,
36269,36277,36293,36299,36307,36313,36319,36341,
36343,36353,36373,36383,36389,36433,36451,36457,
36467,36469,36473,36479,36493,36497,36523,36527,
36529,36541,36551,36559,36563,36571,36583,36587,
36599,36607,36629,36637,36643,36653,36671,36677,
36683,36691,36697,36709,36713,36721,36739,36749,
36761,36767,36779,36781,36787,36791,36793,36809,
36821,36833,36847,36857,36871,36877,36887,36899,
36901,36913,36919,36923,36929,36931,36943,36947,
36973,36979,36997,37003,37013,37019,37021,37039,
37049,37057,37061,37087,37097,37117,37123,37139,
37159,37171,37181,37189,37199,37201,37217,37223,
37243,37253,37273,37277,37307,37309,37313,37321,
37337,37339,37357,37361,37363,37369,37379,37397,
37409,37423,37441,37447,37463,37483,37489,37493,
37501,37507,37511,37517,37529,37537,37547,37549,
37561,37567,37571,37573,37579,37589,37591,37607,
37619,37633,37643,37649,37657,37663,37691,37693,
37699,37717,37747,37781,37783,37799,37811,37813,
37831,37847,37853,37861,37871,37879,37889,37897,
37907,37951,37957,37963,37967,37987,37991,37993,
37997,38011,38039,38047,38053,38069,38083,38113,
38119,38149,38153,38167,38177,38183,38189,38197,
38201,38219,38231,38237,38239,38261,38273,38281,
38287,38299,38303,38317,38321,38327,38329,38333,
38351,38371,38377,38393,38431,38447,38449,38453,
38459,38461,38501,38543,38557,38561,38567,38569,
38593,38603,38609,38611,38629,38639,38651,38653,
38669,38671,38677,38693,38699,38707,38711,38713,
38723,38729,38737,38747,38749,38767,38783,38791,
38803,38821,38833,38839,38851,38861,38867,38873,
38891,38903,38917,38921,38923,38933,38953,38959,
38971,38977,38993,39019,39023,39041,39043,39047,
39079,39089,39097,39103,39107,39113,39119,39133,
39139,39157,39161,39163,39181,39191,39199,39209,
39217,39227,39229,39233,39239,39241,39251,39293,
39301,39313,39317,39323,39341,39343,39359,39367,
39371,39373,39383,39397,39409,39419,39439,39443,
39451,39461,39499,39503,39509,39511,39521,39541,
39551,39563,39569,39581,39607,39619,39623,39631,
39659,39667,39671,39679,39703,39709,39719,39727,
39733,39749,39761,39769,39779,39791,39799,39821,
39827,39829,39839,39841,39847,39857,39863,39869,
39877,39883,39887,39901,39929,39937,39953,39971,
39979,39983,39989,40009,40013,40031,40037,40039,
40063,40087,40093,40099,40111,40123,40127,40129,
40151,40153,40163,40169,40177,40189,40193,40213,
40231,40237,40241,40253,40277,40283,40289,40343,
40351,40357,40361,40387,40423,40427,40429,40433,
40459,40471,40483,40487,40493,40499,40507,40519,
40529,40531,40543,40559,40577,40583,40591,40597,
40609,40627,40637,40639,40693,40697,40699,40709,
40739,40751,40759,40763,40771,40787,40801,40813,
40819,40823,40829,40841,40847,40849,40853,40867,
40879,40883,40897,40903,40927,40933,40939,40949,
40961,40973,40993,41011,41017,41023,41039,41047,
41051,41057,41077,41081,41113,41117,41131,41141,
41143,41149,41161,41177,41179,41183,41189,41201,
41203,41213,41221,41227,41231,41233,41243,41257,
41263,41269,41281,41299,41333,41341,41351,41357,
41381,41387,41389,41399,41411,41413,41443,41453,
41467,41479,41491,41507,41513,41519,41521,41539,
41543,41549,41579,41593,41597,41603,41609,41611,
41617,41621,41627,41641,41647,41651,41659,41669,
41681,41687,41719,41729,41737,41759,41761,41771,
41777,41801,41809,41813,41843,41849,41851,41863,
41879,41887,41893,41897,41903,41911,41927,41941,
41947,41953,41957,41959,41969,41981,41983,41999,
42013,42017,42019,42023,42043,42061,42071,42073,
42083,42089,42101,42131,42139,42157,42169,42179,
42181,42187,42193,42197,42209,42221,42223,42227,
42239,42257,42281,42283,42293,42299,42307,42323,
42331,42337,42349,42359,42373,42379,42391,42397,
42403,42407,42409,42433,42437,42443,42451,42457,
42461,42463,42467,42473,42487,42491,42499,42509,
42533,42557,42569,42571,42577,42589,42611,42641,
42643,42649,42667,42677,42683,42689,42697,42701,
42703,42709,42719,42727,42737,42743,42751,42767,
42773,42787,42793,42797,42821,42829,42839,42841,
42853,42859,42863,42899,42901,42923,42929,42937,
42943,42953,42961,42967,42979,42989,43003,43013,
43019,43037,43049,43051,43063,43067,43093,43103,
43117,43133,43151,43159,43177,43189,43201,43207,
43223,43237,43261,43271,43283,43291,43313,43319,
43321,43331,43391,43397,43399,43403,43411,43427,
43441,43451,43457,43481,43487,43499,43517,43541,
43543,43573,43577,43579,43591,43597,43607,43609,
43613,43627,43633,43649,43651,43661,43669,43691,
43711,43717,43721,43753,43759,43777,43781,43783,
43787,43789,43793,43801,43853,43867,43889,43891,
43913,43933,43943,43951,43961,43963,43969,43973,
43987,43991,43997,44017,44021,44027,44029,44041,
44053,44059,44071,44087,44089,44101,44111,44119,
44123,44129,44131,44159,44171,44179,44189,44201,
44203,44207,44221,44249,44257,44263,44267,44269,
44273,44279,44281,44293,44351,44357,44371,44381,
44383,44389,44417,44449,44453,44483,44491,44497,
44501,44507,44519,44531,44533,44537,44543,44549,
44563,44579,44587,44617,44621,44623,44633,44641,
44647,44651,44657,44683,44687,44699,44701,44711,
44729,44741,44753,44771,44773,44777,44789,44797,
44809,44819,44839,44843,44851,44867,44879,44887,
44893,44909,44917,44927,44939,44953,44959,44963,
44971,44983,44987,45007,45013,45053,45061,45077,
45083,45119,45121,45127,45131,45137,45139,45161,
45179,45181,45191,45197,45233,45247,45259,45263,
45281,45289,45293,45307,45317,45319,45329,45337,
45341,45343,45361,45377,45389,45403,45413,45427,
45433,45439,45481,45491,45497,45503,45523,45533,
45541,45553,45557,45569,45587,45589,45599,45613,
45631,45641,45659,45667,45673,45677,45691,45697,
45707,45737,45751,45757,45763,45767,45779,45817,
45821,45823,45827,45833,45841,45853,45863,45869,
45887,45893,45943,45949,45953,45959,45971,45979,
45989,46021,46027,46049,46051,46061,46073,46091,
46093,46099,46103,46133,46141,46147,46153,46171,
46181,46183,46187,46199,46219,46229,46237,46261,
46271,46273,46279,46301,46307,46309,46327,46337,
46349,46351,46381,46399,46411,46439,46441,46447,
46451,46457,46471,46477,46489,46499,46507,46511,
46523,46549,46559,46567,46573,46589,46591,46601,
46619,46633,46639,46643,46649,46663,46679,46681,
46687,46691,46703,46723,46727,46747,46751,46757,
46769,46771,46807,46811,46817,46819,46829,46831,
46853,46861,46867,46877,46889,46901,46919,46933,
46957,46993,46997,47017,47041,47051,47057,47059,
47087,47093,47111,47119,47123,47129,47137,47143,
47147,47149,47161,47189,47207,47221,47237,47251,
47269,47279,47287,47293,47297,47303,47309,47317,
47339,47351,47353,47363,47381,47387,47389,47407,
47417,47419,47431,47441,47459,47491,47497,47501,
47507,47513,47521,47527,47533,47543,47563,47569,
47581,47591,47599,47609,47623,47629,47639,47653,
47657,47659,47681,47699,47701,47711,47713,47717,
47737,47741,47743,47777,47779,47791,47797,47807,
47809,47819,47837,47843,47857,47869,47881,47903,
47911,47917,47933,47939,47947,47951,47963,47969,
47977,47981,48017,48023,48029,48049,48073,48079,
48091,48109,48119,48121,48131,48157,48163,48179,
48187,48193,48197,48221,48239,48247,48259,48271,
48281,48299,48311,48313,48337,48341,48353,48371,
48383,48397,48407,48409,48413,48437,48449,48463,
48473,48479,48481,48487,48491,48497,48523,48527,
48533,48539,48541,48563,48571,48589,48593,48611,
48619,48623,48647,48649,48661,48673,48677,48679,
48731,48733,48751,48757,48761,48767,48779,48781,
48787,48799,48809,48817,48821,48823,48847,48857,
48859,48869,48871,48883,48889,48907,48947,48953,
48973,48989,48991,49003,49009,49019,49031,49033,
49037,49043,49057,49069,49081,49103,49109,49117,
49121,49123,49139,49157,49169,49171,49177,49193,
49199,49201,49207,49211,49223,49253,49261,49277,
49279,49297,49307,49331,49333,49339,49363,49367,
49369,49391,49393,49409,49411,49417,49429,49433,
49451,49459,49463,49477,49481,49499,49523,49529,
49531,49537,49547,49549,49559,49597,49603,49613,
49627,49633,49639,49663,49667,49669,49681,49697,
49711,49727,49739,49741,49747,49757,49783,49787,
49789,49801,49807,49811,49823,49831,49843,49853,
49871,49877,49891,49919,49921,49927,49937,49939,
49943,49957,49991,49993,49999,50021,50023,50033,
50047,50051,50053,50069,50077,50087,50093,50101,
50111,50119,50123,50129,50131,50147,50153,50159,
50177,50207,50221,50227,50231,50261,50263,50273,
50287,50291,50311,50321,50329,50333,50341,50359,
50363,50377,50383,50387,50411,50417,50423,50441,
50459,50461,50497,50503,50513,50527,50539,50543,
50549,50551,50581,50587,50591,50593,50599,50627,
50647,50651,50671,50683,50707,50723,50741,50753,
50767,50773,50777,50789,50821,50833,50839,50849,
50857,50867,50873,50891,50893,50909,50923,50929,
50951,50957,50969,50971,50989,50993,51001,51031,
51043,51047,51059,51061,51071,51109,51131,51133,
51137,51151,51157,51169,51193,51197,51199,51203,
51217,51229,51239,51241,51257,51263,51283,51287,
51307,51329,51341,51343,51347,51349,51361,51383,
51407,51413,51419,51421,51427,51431,51437,51439,
51449,51461,51473,51479,51481,51487,51503,51511,
51517,51521,51539,51551,51563,51577,51581,51593,
51599,51607,51613,51631,51637,51647,51659,51673,
51679,51683,51691,51713,51719,51721,51749,51767,
51769,51787,51797,51803,51817,51827,51829,51839,
51853,51859,51869,51871,51893,51899,51907,51913,
51929,51941,51949,51971,51973,51977,51991,52009,
52021,52027,52051,52057,52067,52069,52081,52103,
52121,52127,52147,52153,52163,52177,52181,52183,
52189,52201,52223,52237,52249,52253,52259,52267,
52289,52291,52301,52313,52321,52361,52363,52369,
52379,52387,52391,52433,52453,52457,52489,52501,
52511,52517,52529,52541,52543,52553,52561,52567,
52571,52579,52583,52609,52627,52631,52639,52667,
52673,52691,52697,52709,52711,52721,52727,52733,
52747,52757,52769,52783,52807,52813,52817,52837,
52859,52861,52879,52883,52889,52901,52903,52919,
52937,52951,52957,52963,52967,52973,52981,52999,
53003,53017,53047,53051,53069,53077,53087,53089,
53093,53101,53113,53117,53129,53147,53149,53161,
53171,53173,53189,53197,53201,53231,53233,53239,
53267,53269,53279,53281,53299,53309,53323,53327,
53353,53359,53377,53381,53401,53407,53411,53419,
53437,53441,53453,53479,53503,53507,53527,53549,
53551,53569,53591,53593,53597,53609,53611,53617,
53623,53629,53633,53639,53653,53657,53681,53693,
53699,53717,53719,53731,53759,53773,53777,53783,
53791,53813,53819,53831,53849,53857,53861,53881,
53887,53891,53897,53899,53917,53923,53927,53939,
53951,53959,53987,53993,54001,54011,54013,54037,
54049,54059,54083,54091,54101,54121,54133,54139,
54151,54163,54167,54181,54193,54217,54251,54269,
54277,54287,54293,54311,54319,54323,54331,54347,
54361,54367,54371,54377,54401,54403,54409,54413,
54419,54421,54437,54443,54449,54469,54493,54497,
54499,54503,54517,54521,54539,54541,54547,54559,
54563,54577,54581,54583,54601,54617,54623,54629,
54631,54647,54667,54673,54679,54709,54713,54721,
54727,54751,54767,54773,54779,54787,54799,54829,
54833,54851,54869,54877,54881,54907,54917,54919,
54941,54949,54959,54973,54979,54983,55001,55009,
55021,55049,55051,55057,55061,55073,55079,55103,
55109,55117,55127,55147,55163,55171,55201,55207,
55213,55217,55219,55229,55243,55249,55259,55291,
55313,55331,55333,55337,55339,55343,55351,55373,
55381,55399,55411,55439,55441,55457,55469,55487,
55501,55511,55529,55541,55547,55579,55589,55603,
55609,55619,55621,55631,55633,55639,55661,55663,
55667,55673,55681,55691,55697,55711,55717,55721,
55733,55763,55787,55793,55799,55807,55813,55817,
55819,55823,55829,55837,55843,55849,55871,55889,
55897,55901,55903,55921,55927,55931,55933,55949,
55967,55987,55997,56003,56009,56039,56041,56053,
56081,56087,56093,56099,56101,56113,56123,56131,
56149,56167,56171,56179,56197,56207,56209,56237,
56239,56249,56263,56267,56269,56299,56311,56333,
56359,56369,56377,56383,56393,56401,56417,56431,
56437,56443,56453,56467,56473,56477,56479,56489,
56501,56503,56509,56519,56527,56531,56533,56543,
56569,56591,56597,56599,56611,56629,56633,56659,
56663,56671,56681,56687,56701,56711,56713,56731,
56737,56747,56767,56773,56779,56783,56807,56809,
56813,56821,56827,56843,56857,56873,56891,56893,
56897,56909,56911,56921,56923,56929,56941,56951,
56957,56963,56983,56989,56993,56999,57037,57041,
57047,57059,57073,57077,57089,57097,57107,57119,
57131,57139,57143,57149,57163,57173,57179,57191,
57193,57203,57221,57223,57241,57251,57259,57269,
57271,57283,57287,57301,57329,57331,57347,57349,
57367,57373,57383,57389,57397,57413,57427,57457,
57467,57487,57493,57503,57527,57529,57557,57559,
57571,57587,57593,57601,57637,57641,57649,57653,
57667,57679,57689,57697,57709,57713,57719,57727,
57731,57737,57751,57773,57781,57787,57791,57793,
57803,57809,57829,57839,57847,57853,57859,57881,
57899,57901,57917,57923,57943,57947,57973,57977,
57991,58013,58027,58031,58043,58049,58057,58061,
58067,58073,58099,58109,58111,58129,58147,58151,
58153,58169,58171,58189,58193,58199,58207,58211,
58217,58229,58231,58237,58243,58271,58309,58313,
58321,58337,58363,58367,58369,58379,58391,58393,
58403,58411,58417,58427,58439,58441,58451,58453,
58477,58481,58511,58537,58543,58549,58567,58573,
58579,58601,58603,58613,58631,58657,58661,58679,
58687,58693,58699,58711,58727,58733,58741,58757,
58763,58771,58787,58789,58831,58889,58897,58901,
58907,58909,58913,58921,58937,58943,58963,58967,
58979,58991,58997,59009,59011,59021,59023,59029,
59051,59053,59063,59069,59077,59083,59093,59107,
59113,59119,59123,59141,59149,59159,59167,59183,
59197,59207,59209,59219,59221,59233,59239,59243,
59263,59273,59281,59333,59341,59351,59357,59359,
59369,59377,59387,59393,59399,59407,59417,59419,
59441,59443,59447,59453,59467,59471,59473,59497,
59509,59513,59539,59557,59561,59567,59581,59611,
59617,59621,59627,59629,59651,59659,59663,59669,
59671,59693,59699,59707,59723,59729,59743,59747,
59753,59771,59779,59791,59797,59809,59833,59863,
59879,59887,59921,59929,59951,59957,59971,59981,
59999,60013,60017,60029,60037,60041,60077,60083,
60089,60091,60101,60103,60107,60127,60133,60139,
60149,60161,60167,60169,60209,60217,60223,60251,
60257,60259,60271,60289,60293,60317,60331,60337,
60343,60353,60373,60383,60397,60413,60427,60443,
60449,60457,60493,60497,60509,60521,60527,60539,
60589,60601,60607,60611,60617,60623,60631,60637,
60647,60649,60659,60661,60679,60689,60703,60719,
60727,60733,60737,60757,60761,60763,60773,60779,
60793,60811,60821,60859,60869,60887,60889,60899,
60901,60913,60917,60919,60923,60937,60943,60953,
60961,61001,61007,61027,61031,61043,61051,61057,
61091,61099,61121,61129,61141,61151,61153,61169,
61211,61223,61231,61253,61261,61283,61291,61297,
61331,61333,61339,61343,61357,61363,61379,61381,
61403,61409,61417,61441,61463,61469,61471,61483,
61487,61493,61507,61511,61519,61543,61547,61553,
61559,61561,61583,61603,61609,61613,61627,61631,
61637,61643,61651,61657,61667,61673,61681,61687,
61703,61717,61723,61729,61751,61757,61781,61813,
61819,61837,61843,61861,61871,61879,61909,61927,
61933,61949,61961,61967,61979,61981,61987,61991,
62003,62011,62017,62039,62047,62053,62057,62071,
62081,62099,62119,62129,62131,62137,62141,62143,
62171,62189,62191,62201,62207,62213,62219,62233,
62273,62297,62299,62303,62311,62323,62327,62347,
62351,62383,62401,62417,62423,62459,62467,62473,
62477,62483,62497,62501,62507,62533,62539,62549,
62563,62581,62591,62597,62603,62617,62627,62633,
62639,62653,62659,62683,62687,62701,62723,62731,
62743,62753,62761,62773,62791,62801,62819,62827,
62851,62861,62869,62873,62897,62903,62921,62927,
62929,62939,62969,62971,62981,62983,62987,62989,
63029,63031,63059,63067,63073,63079,63097,63103,
63113,63127,63131,63149,63179,63197,63199,63211,
63241,63247,63277,63281,63299,63311,63313,63317,
63331,63337,63347,63353,63361,63367,63377,63389,
63391,63397,63409,63419,63421,63439,63443,63463,
63467,63473,63487,63493,63499,63521,63527,63533,
63541,63559,63577,63587,63589,63599,63601,63607,
63611,63617,63629,63647,63649,63659,63667,63671,
63689,63691,63697,63703,63709,63719,63727,63737,
63743,63761,63773,63781,63793,63799,63803,63809,
63823,63839,63841,63853,63857,63863,63901,63907,
63913,63929,63949,63977,63997,64007,64013,64019,
64033,64037,64063,64067,64081,64091,64109,64123,
64151,64153,64157,64171,64187,64189,64217,64223,
64231,64237,64271,64279,64283,64301,64303,64319,
64327,64333,64373,64381,64399,64403,64433,64439,
64451,64453,64483,64489,64499,64513,64553,64567,
64577,64579,64591,64601,64609,64613,64621,64627,
64633,64661,64663,64667,64679,64693,64709,64717,
64747,64763,64781,64783,64793,64811,64817,64849,
64853,64871,64877,64879,64891,64901,64919,64921,
64927,64937,64951,64969,64997,65003,65011,65027,
65029,65033,65053,65063,65071,65089,65099,65101,
65111,65119,65123,65129,65141,65147,65167,65171,
65173,65179,65183,65203,65213,65239,65257,65267,
65269,65287,65293,65309,65323,65327,65353,65357,
65371,65381,65393,65407,65413,65419,65423,65437,
65447,65449,65479,65497,65519,65521,65537,65539,
65543,65551,65557,65563,65579,65581,65587,65599,
65609,65617,65629,65633,65647,65651,65657,65677,
65687,65699,65701,65707,65713,65717,65719,65729,
65731,65761,65777,65789,65809,65827,65831,65837,
65839,65843,65851,65867,65881,65899,65921,65927,
65929,65951,65957,65963,65981,65983,65993,66029,
66037,66041,66047,66067,66071,66083,66089,66103,
66107,66109,66137,66161,66169,66173,66179,66191,
66221,66239,66271,66293,66301,66337,66343,66347,
66359,66361,66373,66377,66383,66403,66413,66431,
66449,66457,66463,66467,66491,66499,66509,66523,
66529,66533,66541,66553,66569,66571,66587,66593,
66601,66617,66629,66643,66653,66683,66697,66701,
66713,66721,66733,66739,66749,66751,66763,66791,
66797,66809,66821,66841,66851,66853,66863,66877,
66883,66889,66919,66923,66931,66943,66947,66949,
66959,66973,66977,67003,67021,67033,67043,67049,
67057,67061,67073,67079,67103,67121,67129,67139,
67141,67153,67157,67169,67181,67187,67189,67211,
67213,67217,67219,67231,67247,67261,67271,67273,
67289,67307,67339,67343,67349,67369,67391,67399,
67409,67411,67421,67427,67429,67433,67447,67453,
67477,67481,67489,67493,67499,67511,67523,67531,
67537,67547,67559,67567,67577,67579,67589,67601,
67607,67619,67631,67651,67679,67699,67709,67723,
67733,67741,67751,67757,67759,67763,67777,67783,
67789,67801,67807,67819,67829,67843,67853,67867,
67883,67891,67901,67927,67931,67933,67939,67943,
67957,67961,67967,67979,67987,67993,68023,68041,
68053,68059,68071,68087,68099,68111,68113,68141,
68147,68161,68171,68207,68209,68213,68219,68227,
68239,68261,68279,68281,68311,68329,68351,68371,
68389,68399,68437,68443,68447,68449,68473,68477,
68483,68489,68491,68501,68507,68521,68531,68539,
68543,68567,68581,68597,68611,68633,68639,68659,
68669,68683,68687,68699,68711,68713,68729,68737,
68743,68749,68767,68771,68777,68791,68813,68819,
68821,68863,68879,68881,68891,68897,68899,68903,
68909,68917,68927,68947,68963,68993,69001,69011,
69019,69029,69031,69061,69067,69073,69109,69119,
69127,69143,69149,69151,69163,69191,69193,69197,
69203,69221,69233,69239,69247,69257,69259,69263,
69313,69317,69337,69341,69371,69379,69383,69389,
69401,69403,69427,69431,69439,69457,69463,69467,
69473,69481,69491,69493,69497,69499,69539,69557,
69593,69623,69653,69661,69677,69691,69697,69709,
69737,69739,69761,69763,69767,69779,69809,69821,
69827,69829,69833,69847,69857,69859,69877,69899,
69911,69929,69931,69941,69959,69991,69997,70001,
70003,70009,70019,70039,70051,70061,70067,70079,
70099,70111,70117,70121,70123,70139,70141,70157,
70163,70177,70181,70183,70199,70201,70207,70223,
70229,70237,70241,70249,70271,70289,70297,70309,
70313,70321,70327,70351,70373,70379,70381,70393,
70423,70429,70439,70451,70457,70459,70481,70487,
70489,70501,70507,70529,70537,70549,70571,70573,
70583,70589,70607,70619,70621,70627,70639,70657,
70663,70667,70687,70709,70717,70729,70753,70769,
70783,70793,70823,70841,70843,70849,70853,70867,
70877,70879,70891,70901,70913,70919,70921,70937,
70949,70951,70957,70969,70979,70981,70991,70997,
70999,71011,71023,71039,71059,71069,71081,71089,
71119,71129,71143,71147,71153,71161,71167,71171,
71191,71209,71233,71237,71249,71257,71261,71263,
71287,71293,71317,71327,71329,71333,71339,71341,
71347,71353,71359,71363,71387,71389,71399,71411,
71413,71419,71429,71437,71443,71453,71471,71473,
71479,71483,71503,71527,71537,71549,71551,71563,
71569,71593,71597,71633,71647,71663,71671,71693,
71699,71707,71711,71713,71719,71741,71761,71777,
71789,71807,71809,71821,71837,71843,71849,71861,
71867,71879,71881,71887,71899,71909,71917,71933,
71941,71947,71963,71971,71983,71987,71993,71999,
72019,72031,72043,72047,72053,72073,72077,72089,
72091,72101,72103,72109,72139,72161,72167,72169,
72173,72211,72221,72223,72227,72229,72251,72253,
72269,72271,72277,72287,72307,72313,72337,72341,
72353,72367,72379,72383,72421,72431,72461,72467,
72469,72481,72493,72497,72503,72533,72547,72551,
72559,72577,72613,72617,72623,72643,72647,72649,
72661,72671,72673,72679,72689,72701,72707,72719,
72727,72733,72739,72763,72767,72797,72817,72823,
72859,72869,72871,72883,72889,72893,72901,72907,
72911,72923,72931,72937,72949,72953,72959,72973,
72977,72997,73009,73013,73019,73037,73039,73043,
73061,73063,73079,73091,73121,73127,73133,73141,
73181,73189,73237,73243,73259,73277,73291,73303,
73309,73327,73331,73351,73361,73363,73369,73379,
73387,73417,73421,73433,73453,73459,73471,73477,
73483,73517,73523,73529,73547,73553,73561,73571,
73583,73589,73597,73607,73609,73613,73637,73643,
73651,73673,73679,73681,73693,73699,73709,73721,
73727,73751,73757,73771,73783,73819,73823,73847,
73849,73859,73867,73877,73883,73897,73907,73939,
73943,73951,73961,73973,73999,74017,74021,74027,
74047,74051,74071,74077,74093,74099,74101,74131,
74143,74149,74159,74161,74167,74177,74189,74197,
74201,74203,74209,74219,74231,74257,74279,74287,
74293,74297,74311,74317,74323,74353,74357,74363,
74377,74381,74383,74411,74413,74419,74441,74449,
74453,74471,74489,74507,74509,74521,74527,74531,
74551,74561,74567,74573,74587,74597,74609,74611,
74623,74653,74687,74699,74707,74713,74717,74719,
74729,74731,74747,74759,74761,74771,74779,74797,
74821,74827,74831,74843,74857,74861,74869,74873,
74887,74891,74897,74903,74923,74929,74933,74941,
74959,75011,75013,75017,75029,75037,75041,75079,
75083,75109,75133,75149,75161,75167,75169,75181,
75193,75209,75211,75217,75223,75227,75239,75253,
75269,75277,75289,75307,75323,75329,75337,75347,
75353,75367,75377,75389,75391,75401,75403,75407,
75431,75437,75479,75503,75511,75521,75527,75533,
75539,75541,75553,75557,75571,75577,75583,75611,
75617,75619,75629,75641,75653,75659,75679,75683,
75689,75703,75707,75709,75721,75731,75743,75767,
75773,75781,75787,75793,75797,75821,75833,75853,
75869,75883,75913,75931,75937,75941,75967,75979,
75983,75989,75991,75997,76001,76003,76031,76039,
76079,76081,76091,76099,76103,76123,76129,76147,
76157,76159,76163,76207,76213,76231,76243,76249,
76253,76259,76261,76283,76289,76303,76333,76343,
76367,76369,76379,76387,76403,76421,76423,76441,
76463,76471,76481,76487,76493,76507,76511,76519,
76537,76541,76543,76561,76579,76597,76603,76607,
76631,76649,76651,76667,76673,76679,76697,76717,
76733,76753,76757,76771,76777,76781,76801,76819,
76829,76831,76837,76847,76871,76873,76883,76907,
76913,76919,76943,76949,76961,76963,76991,77003,
77017,77023,77029,77041,77047,77069,77081,77093,
77101,77137,77141,77153,77167,77171,77191,77201,
77213,77237,77239,77243,77249,77261,77263,77267,
77269,77279,77291,77317,77323,77339,77347,77351,
77359,77369,77377,77383,77417,77419,77431,77447,
77471,77477,77479,77489,77491,77509,77513,77521,
77527,77543,77549,77551,77557,77563,77569,77573,
77587,77591,77611,77617,77621,77641,77647,77659,
77681,77687,77689,77699,77711,77713,77719,77723,
77731,77743,77747,77761,77773,77783,77797,77801,
77813,77839,77849,77863,77867,77893,77899,77929,
77933,77951,77969,77977,77983,77999,78007,78017,
78031,78041,78049,78059,78079,78101,78121,78137,
78139,78157,78163,78167,78173,78179,78191,78193,
78203,78229,78233,78241,78259,78277,78283,78301,
78307,78311,78317,78341,78347,78367,78401,78427,
78437,78439,78467,78479,78487,78497,78509,78511,
78517,78539,78541,78553,78569,78571,78577,78583,
78593,78607,78623,78643,78649,78653,78691,78697,
78707,78713,78721,78737,78779,78781,78787,78791,
78797,78803,78809,78823,78839,78853,78857,78877,
78887,78889,78893,78901,78919,78929,78941,78977,
78979,78989,79031,79039,79043,79063,79087,79103,
79111,79133,79139,79147,79151,79153,79159,79181,
79187,79193,79201,79229,79231,79241,79259,79273,
79279,79283,79301,79309,79319,79333,79337,79349,
79357,79367,79379,79393,79397,79399,79411,79423,
79427,79433,79451,79481,79493,79531,79537,79549,
79559,79561,79579,79589,79601,79609,79613,79621,
79627,79631,79633,79657,79669,79687,79691,79693,
79697,79699,79757,79769,79777,79801,79811,79813,
79817,79823,79829,79841,79843,79847,79861,79867,
79873,79889,79901,79903,79907,79939,79943,79967,
79973,79979,79987,79997,79999,80021,80039,80051,
80071,80077,80107,80111,80141,80147,80149,80153,
80167,80173,80177,80191,80207,80209,80221,80231,
80233,80239,80251,80263,80273,80279,80287,80309,
80317,80329,80341,80347,80363,80369,80387,80407,
80429,80447,80449,80471,80473,80489,80491,80513,
80527,80537,80557,80567,80599,80603,80611,80621,
80627,80629,80651,80657,80669,80671,80677,80681,
80683,80687,80701,80713,80737,80747,80749,80761,
80777,80779,80783,80789,80803,80809,80819,80831,
80833,80849,80863,80897,80909,80911,80917,80923,
80929,80933,80953,80963,80989,81001,81013,81017,
81019,81023,81031,81041,81043,81047,81049,81071,
81077,81083,81097,81101,81119,81131,81157,81163,
81173,81181,81197,81199,81203,81223,81233,81239,
81281,81283,81293,81299,81307,81331,81343,81349,
81353,81359,81371,81373,81401,81409,81421,81439,
81457,81463,81509,81517,81527,81533,81547,81551,
81553,81559,81563,81569,81611,81619,81629,81637,
81647,81649,81667,81671,81677,81689,81701,81703,
81707,81727,81737,81749,81761,81769,81773,81799,
81817,81839,81847,81853,81869,81883,81899,81901,
81919,81929,81931,81937,81943,81953,81967,81971,
81973,82003,82007,82009,82013,82021,82031,82037,
82039,82051,82067,82073,82129,82139,82141,82153,
82163,82171,82183,82189,82193,82207,82217,82219,
82223,82231,82237,82241,82261,82267,82279,82301,
82307,82339,82349,82351,82361,82373,82387,82393,
82421,82457,82463,82469,82471,82483,82487,82493,
82499,82507,82529,82531,82549,82559,82561,82567,
82571,82591,82601,82609,82613,82619,82633,82651,
82657,82699,82721,82723,82727,82729,82757,82759,
82763,82781,82787,82793,82799,82811,82813,82837,
82847,82883,82889,82891,82903,82913,82939,82963,
82981,82997,83003,83009,83023,83047,83059,83063,
83071,83077,83089,83093,83101,83117,83137,83177,
83203,83207,83219,83221,83227,83231,83233,83243,
83257,83267,83269,83273,83299,83311,83339,83341,
83357,83383,83389,83399,83401,83407,83417,83423,
83431,83437,83443,83449,83459,83471,83477,83497,
83537,83557,83561,83563,83579,83591,83597,83609,
83617,83621,83639,83641,83653,83663,83689,83701,
83717,83719,83737,83761,83773,83777,83791,83813,
83833,83843,83857,83869,83873,83891,83903,83911,
83921,83933,83939,83969,83983,83987,84011,84017,
84047,84053,84059,84061,84067,84089,84121,84127,
84131,84137,84143,84163,84179,84181,84191,84199,
84211,84221,84223,84229,84239,84247,84263,84299,
84307,84313,84317,84319,84347,84349,84377,84389,
84391,84401,84407,84421,84431,84437,84443,84449,
84457,84463,84467,84481,84499,84503,84509,84521,
84523,84533,84551,84559,84589,84629,84631,84649,
84653,84659,84673,84691,84697,84701,84713,84719,
84731,84737,84751,84761,84787,84793,84809,84811,
84827,84857,84859,84869,84871,84913,84919,84947,
84961,84967,84977,84979,84991,85009,85021,85027,
85037,85049,85061,85081,85087,85091,85093,85103,
85109,85121,85133,85147,85159,85193,85199,85201,
85213,85223,85229,85237,85243,85247,85259,85297,
85303,85313,85331,85333,85361,85363,85369,85381,
85411,85427,85429,85439,85447,85451,85453,85469,
85487,85513,85517,85523,85531,85549,85571,85577,
85597,85601,85607,85619,85621,85627,85639,85643,
85661,85667,85669,85691,85703,85711,85717,85733,
85751,85781,85793,85817,85819,85829,85831,85837,
85843,85847,85853,85889,85903,85909,85931,85933,
85991,85999,86011,86017,86027,86029,86069,86077,
86083,86111,86113,86117,86131,86137,86143,86161,
86171,86179,86183,86197,86201,86209,86239,86243,
86249,86257,86263,86269,86287,86291,86293,86297,
86311,86323,86341,86351,86353,86357,86369,86371,
86381,86389,86399,86413,86423,86441,86453,86461,
86467,86477,86491,86501,86509,86531,86533,86539,
86561,86573,86579,86587,86599,86627,86629,86677,
86689,86693,86711,86719,86729,86743,86753,86767,
86771,86783,86813,86837,86843,86851,86857,86861,
86869,86923,86927,86929,86939,86951,86959,86969,
86981,86993,87011,87013,87037,87041,87049,87071,
87083,87103,87107,87119,87121,87133,87149,87151,
87179,87181,87187,87211,87221,87223,87251,87253,
87257,87277,87281,87293,87299,87313,87317,87323,
87337,87359,87383,87403,87407,87421,87427,87433,
87443,87473,87481,87491,87509,87511,87517,87523,
87539,87541,87547,87553,87557,87559,87583,87587,
87589,87613,87623,87629,87631,87641,87643,87649,
87671,87679,87683,87691,87697,87701,87719,87721,
87739,87743,87751,87767,87793,87797,87803,87811,
87833,87853,87869,87877,87881,87887,87911,87917,
87931,87943,87959,87961,87973,87977,87991,88001,
88003,88007,88019,88037,88069,88079,88093,88117,
88129,88169,88177,88211,88223,88237,88241,88259,
88261,88289,88301,88321,88327,88337,88339,88379,
88397,88411,88423,88427,88463,88469,88471,88493,
88499,88513,88523,88547,88589,88591,88607,88609,
88643,88651,88657,88661,88663,88667,88681,88721,
88729,88741,88747,88771,88789,88793,88799,88801,
88807,88811,88813,88817,88819,88843,88853,88861,
88867,88873,88883,88897,88903,88919,88937,88951,
88969,88993,88997,89003,89009,89017,89021,89041,
89051,89057,89069,89071,89083,89087,89101,89107,
89113,89119,89123,89137,89153,89189,89203,89209,
89213,89227,89231,89237,89261,89269,89273,89293,
89303,89317,89329,89363,89371,89381,89387,89393,
89399,89413,89417,89431,89443,89449,89459,89477,
89491,89501,89513,89519,89521,89527,89533,89561,
89563,89567,89591,89597,89599,89603,89611,89627,
89633,89653,89657,89659,89669,89671,89681,89689,
89753,89759,89767,89779,89783,89797,89809,89819,
89821,89833,89839,89849,89867,89891,89897,89899,
89909,89917,89923,89939,89959,89963,89977,89983,
89989,90001,90007,90011,90017,90019,90023,90031,
90053,90059,90067,90071,90073,90089,90107,90121,
90127,90149,90163,90173,90187,90191,90197,90199,
90203,90217,90227,90239,90247,90263,90271,90281,
90289,90313,90353,90359,90371,90373,90379,90397,
90401,90403,90407,90437,90439,90469,90473,90481,
90499,90511,90523,90527,90529,90533,90547,90583,
90599,90617,90619,90631,90641,90647,90659,90677,
90679,90697,90703,90709,90731,90749,90787,90793,
90803,90821,90823,90833,90841,90847,90863,90887,
90901,90907,90911,90917,90931,90947,90971,90977,
90989,90997,91009,91019,91033,91079,91081,91097,
91099,91121,91127,91129,91139,91141,91151,91153,
91159,91163,91183,91193,91199,91229,91237,91243,
91249,91253,91283,91291,91297,91303,91309,91331,
91367,91369,91373,91381,91387,91393,91397,91411,
91423,91433,91453,91457,91459,91463,91493,91499,
91513,91529,91541,91571,91573,91577,91583,91591,
91621,91631,91639,91673,91691,91703,91711,91733,
91753,91757,91771,91781,91801,91807,91811,91813,
91823,91837,91841,91867,91873,91909,91921,91939,
91943,91951,91957,91961,91967,91969,91997,92003,
92009,92033,92041,92051,92077,92083,92107,92111,
92119,92143,92153,92173,92177,92179,92189,92203,
92219,92221,92227,92233,92237,92243,92251,92269,
92297,92311,92317,92333,92347,92353,92357,92363,
92369,92377,92381,92383,92387,92399,92401,92413,
92419,92431,92459,92461,92467,92479,92489,92503,
92507,92551,92557,92567,92569,92581,92593,92623,
92627,92639,92641,92647,92657,92669,92671,92681,
92683,92693,92699,92707,92717,92723,92737,92753,
92761,92767,92779,92789,92791,92801,92809,92821,
92831,92849,92857,92861,92863,92867,92893,92899,
92921,92927,92941,92951,92957,92959,92987,92993,
93001,93047,93053,93059,93077,93083,93089,93097,
93103,93113,93131,93133,93139,93151,93169,93179,
93187,93199,93229,93239,93241,93251,93253,93257,
93263,93281,93283,93287,93307,93319,93323,93329,
93337,93371,93377,93383,93407,93419,93427,93463,
93479,93481,93487,93491,93493,93497,93503,93523,
93529,93553,93557,93559,93563,93581,93601,93607,
93629,93637,93683,93701,93703,93719,93739,93761,
93763,93787,93809,93811,93827,93851,93871,93887,
93889,93893,93901,93911,93913,93923,93937,93941,
93949,93967,93971,93979,93983,93997,94007,94009,
94033,94049,94057,94063,94079,94099,94109,94111,
94117,94121,94151,94153,94169,94201,94207,94219,
94229,94253,94261,94273,94291,94307,94309,94321,
94327,94331,94343,94349,94351,94379,94397,94399,
94421,94427,94433,94439,94441,94447,94463,94477,
94483,94513,94529,94531,94541,94543,94547,94559,
94561,94573,94583,94597,94603,94613,94621,94649,
94651,94687,94693,94709,94723,94727,94747,94771,
94777,94781,94789,94793,94811,94819,94823,94837,
94841,94847,94849,94873,94889,94903,94907,94933,
94949,94951,94961,94993,94999,95003,95009,95021,
95027,95063,95071,95083,95087,95089,95093,95101,
95107,95111,95131,95143,95153,95177,95189,95191,
95203,95213,95219,95231,95233,95239,95257,95261,
95267,95273,95279,95287,95311,95317,95327,95339,
95369,95383,95393,95401,95413,95419,95429,95441,
95443,95461,95467,95471,95479,95483,95507,95527,
95531,95539,95549,95561,95569,95581,95597,95603,
95617,95621,95629,95633,95651,95701,95707,95713,
95717,95723,95731,95737,95747,95773,95783,95789,
95791,95801,95803,95813,95819,95857,95869,95873,
95881,95891,95911,95917,95923,95929,95947,95957,
95959,95971,95987,95989,96001,96013,96017,96043,
96053,96059,96079,96097,96137,96149,96157,96167,
96179,96181,96199,96211,96221,96223,96233,96259,
96263,96269,96281,96289,96293,96323,96329,96331,
96337,96353,96377,96401,96419,96431,96443,96451,
96457,96461,96469,96479,96487,96493,96497,96517,
96527,96553,96557,96581,96587,96589,96601,96643,
96661,96667,96671,96697,96703,96731,96737,96739,
96749,96757,96763,96769,96779,96787,96797,96799,
96821,96823,96827,96847,96851,96857,96893,96907,
96911,96931,96953,96959,96973,96979,96989,96997,
97001,97003,97007,97021,97039,97073,97081,97103,
97117,97127,97151,97157,97159,97169,97171,97177,
97187,97213,97231,97241,97259,97283,97301,97303,
97327,97367,97369,97373,97379,97381,97387,97397,
97423,97429,97441,97453,97459,97463,97499,97501,
97511,97523,97547,97549,97553,97561,97571,97577,
97579,97583,97607,97609,97613,97649,97651,97673,
97687,97711,97729,97771,97777,97787,97789,97813,
97829,97841,97843,97847,97849,97859,97861,97871,
97879,97883,97919,97927,97931,97943,97961,97967,
97973,97987,98009,98011,98017,98041,98047,98057,
98081,98101,98123,98129,98143,98179,98207,98213,
98221,98227,98251,98257,98269,98297,98299,98317,
98321,98323,98327,98347,98369,98377,98387,98389,
98407,98411,98419,98429,98443,98453,98459,98467,
98473,98479,98491,98507,98519,98533,98543,98561,
98563,98573,98597,98621,98627,98639,98641,98663,
98669,98689,98711,98713,98717,98729,98731,98737,
98773,98779,98801,98807,98809,98837,98849,98867,
98869,98873,98887,98893,98897,98899,98909,98911,
98927,98929,98939,98947,98953,98963,98981,98993,
98999,99013,99017,99023,99041,99053,99079,99083,
99089,99103,99109,99119,99131,99133,99137,99139,
99149,99173,99181,99191,99223,99233,99241,99251,
99257,99259,99277,99289,99317,99347,99349,99367,
99371,99377,99391,99397,99401,99409,99431,99439,
99469,99487,99497,99523,99527,99529,99551,99559,
99563,99571,99577,99581,99607,99611,99623,99643,
99661,99667,99679,99689,99707,99709,99713,99719,
99721,99733,99761,99767,99787,99793,99809,99817,
99823,99829,99833,99839,99859,99871,99877,99881,
99901,99907,99923,99929,99961,99971,99989,99991,
100003,100019,100043,100049,100057,100069,100103,100109,
100129,100151,100153,100169,100183,100189,100193,100207,
100213,100237,100267,100271,100279,100291,100297,100313,
100333,100343,100357,100361,100363,100379,100391,100393,
100403,100411,100417,100447,100459,100469,100483,100493,
100501,100511,100517,100519,100523,100537,100547,100549,
100559,100591,100609,100613,100621,100649,100669,100673,
100693,100699,100703,100733,100741,100747,100769,100787,
100799,100801,100811,100823,100829,100847,100853,100907,
100913,100927,100931,100937,100943,100957,100981,100987,
100999,101009,101021,101027,101051,101063,101081,101089,
101107,101111,101113,101117,101119,101141,101149,101159,
101161,101173,101183,101197,101203,101207,101209,101221,
101267,101273,101279,101281,101287,101293,101323,101333,
101341,101347,101359,101363,101377,101383,101399,101411,
101419,101429,101449,101467,101477,101483,101489,101501,
101503,101513,101527,101531,101533,101537,101561,101573,
101581,101599,101603,101611,101627,101641,101653,101663,
101681,101693,101701,101719,101723,101737,101741,101747,
101749,101771,101789,101797,101807,101833,101837,101839,
101863,101869,101873,101879,101891,101917,101921,101929,
101939,101957,101963,101977,101987,101999,102001,102013,
102019,102023,102031,102043,102059,102061,102071,102077,
102079,102101,102103,102107,102121,102139,102149,102161,
102181,102191,102197,102199,102203,102217,102229,102233,
102241,102251,102253,102259,102293,102299,102301,102317,
102329,102337,102359,102367,102397,102407,102409,102433,
102437,102451,102461,102481,102497,102499,102503,102523,
102533,102539,102547,102551,102559,102563,102587,102593,
102607,102611,102643,102647,102653,102667,102673,102677,
102679,102701,102761,102763,102769,102793,102797,102811,
102829,102841,102859,102871,102877,102881,102911,102913,
102929,102931,102953,102967,102983,103001,103007,103043,
103049,103067,103069,103079,103087,103091,103093,103099,
103123,103141,103171,103177,103183,103217,103231,103237,
103289,103291,103307,103319,103333,103349,103357,103387,
103391,103393,103399,103409,103421,103423,103451,103457,
103471,103483,103511,103529,103549,103553,103561,103567,
103573,103577,103583,103591,103613,103619,103643,103651,
103657,103669,103681,103687,103699,103703,103723,103769,
103787,103801,103811,103813,103837,103841,103843,103867,
103889,103903,103913,103919,103951,103963,103967,103969,
103979,103981,103991,103993,103997,104003,104009,104021,
104033,104047,104053,104059,104087,104089,104107,104113,
104119,104123,104147,104149,104161,104173,104179,104183,
104207,104231,104233,104239,104243,104281,104287,104297,
104309,104311,104323,104327,104347,104369,104381,104383,
104393,104399,104417,104459,104471,104473,104479,104491,
104513,104527,104537,104543,104549,104551,104561,104579,
104593,104597,104623,104639,104651,104659,104677,104681,
104683,104693,104701,104707,104711,104717,104723,104729,
];
const BLACK = 1;
const BLUE = 2;
const RED = 3;

function
print_buf(s, color)
{
	var t;

	s = s.replace(/&/g, "&amp;");
	s = s.replace(/</g, "&lt;");
	s = s.replace(/>/g, "&gt;");
	s = s.replace(/\n/g, "<br>");

	switch (color) {

	case BLACK:
		t = "<p><span style='color:black'>";
		break;

	case BLUE:
		t = "<p><span style='color:blue;font-family:courier'>";
		break;

	case RED:
		t = "<p><span style='color:red;font-family:courier'>";
		break;
	}

	fputs(t + s + "</span>");
}
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
	mml_puts("</math>");
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
	var i, j, k, s, t;

	s = Math.abs(p.d).toPrecision(6) + 'T'; // terminator

	k = 0;

	while (isdigit(s[k]) || s[k] == '.')
		k++;

	// handle trailing zeroes

	i = s.indexOf(".");

	if (i == -1)
		t = s.substring(0, k) + ".0";
	else {
		for (j = k - 1; j > i + 1; j--) {
			if (s[j] != '0')
				break;
		}
		t = s.substring(0, j + 1);
	}

	mml_mn(t);

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
function
print_result()
{
	var p1, p2;

	p2 = pop(); // result
	p1 = pop(); // input

	if (p2 == symbol(NIL))
		return;

	prep_symbol_equals(p1, p2);

	print_math();
}
function
printname(p)
{
	if ("printname" in p)
		return p.printname;
	else
		return "?";
}
function
promote_tensor()
{
	var i, j, k, ndim1, ndim2, nelem1, nelem2, p1, p2, p3;

	p1 = pop();

	if (!istensor(p1)) {
		push(p1);
		return;
	}

	ndim1 = p1.dim.length;
	nelem1 = p1.elem.length;

	// check

	p2 = p1.elem[0];

	for (i = 1; i < nelem1; i++) {
		p3 = p1.elem[i];
		if (!compatible_dimensions(p2, p3))
			stopf("tensor dimensions");
	}

	if (!istensor(p2)) {
		push(p1);
		return; // all elements are scalars
	}

	ndim2 = p2.dim.length;
	nelem2 = p2.elem.length;

	// alloc

	p3 = alloc_tensor();

	// merge dimensions

	k = 0;

	for (i = 0; i < ndim1; i++)
		p3.dim[k++] = p1.dim[i];

	for (i = 0; i < ndim2; i++)
		p3.dim[k++] = p2.dim[i];

	// merge elements

	k = 0;

	for (i = 0; i < nelem1; i++) {
		p2 = p1.elem[i];
		for (j = 0; j < nelem2; j++)
			p3.elem[k++] = p2.elem[j];
	}

	push(p3);
}
function
push(a)
{
	stack.push(a);
}
function
push_double(d)
{
	push({d:d});
}
function
push_integer(n)
{
	push({a:n, b:1});
}
function
push_rational(a, b)
{
	if (b == 0)
		stopf("divide by zero");

	if (a == 0)
		b = 1;

	if (b < 0) {
		a = -a;
		b = -b;
	}

	if (Math.abs(a) > 1e10 || b > 1e10)
		push_double(a / b);
	else
		push({a:a, b:b});
}
function
push_string(s)
{
	push({string:s});
}
function
push_symbol(p)
{
	push(symbol(p));
}
function
rationalize()
{
	var i, n, p0, p1, p2;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1.elem.length;
		for (i = 0; i < n; i++) {
			push(p1.elem[i]);
			rationalize();
			p1.elem[i] = pop();
		}
		push(p1);
		return;
	}

	p2 = one;

	while (divisor(p1)) {
		p0 = pop();
		push(p0);
		push(p1);
		cancel_factor();
		p1 = pop();
		push(p0);
		push(p2);
		multiply_noexpand();
		p2 = pop();
	}

	push(p1);
	push(p2);
	reciprocate();
	multiply_noexpand();
}
function
real()
{
	var p1;
	rect();
	p1 = pop();
	push(p1);
	push(p1);
	conj();
	add();
	push_rational(1, 2);
	multiply();
}
function
reciprocate()
{
	push_integer(-1);
	power();
}
function
rect()
{
	var h, p1, p2, BASE, EXPO;

	p1 = pop();

	if (car(p1) == symbol(ADD)) {
		p1 = cdr(p1);
		h = stack.length;
		while (iscons(p1)) {
			push(car(p1));
			rect();
			p1 = cdr(p1);
		}
		add_terms(stack.length - h);
		return;
	}

	if (car(p1) == symbol(MULTIPLY)) {
		p1 = cdr(p1);
		h = stack.length;
		while (iscons(p1)) {
			push(car(p1));
			rect();
			p1 = cdr(p1);
		}
		multiply_factors(stack.length - h);
		return;
	}

	if (car(p1) != symbol(POWER)) {
		push(p1);
		return;
	}

	BASE = cadr(p1);
	EXPO = caddr(p1);

	// handle sum in exponent

	if (car(EXPO) == symbol(ADD)) {
		p1 = cdr(EXPO);
		h = stack.length;
		while (iscons(p1)) {
			push_symbol(POWER);
			push(BASE);
			push(car(p1));
			list(3);
			rect();
			p1 = cdr(p1);
		}
		multiply_factors(stack.length - h);
		return;
	}

	// return mag(p1) * cos(arg(p1)) + i sin(arg(p1)))

	push(p1);
	mag();

	push(p1);
	arg();
	p2 = pop();

	push(p2);
	cos();

	push(imaginaryunit);
	push(p2);
	sin();
	multiply();

	add();

	multiply();
}
function
reduce_radical_double(h, COEFF)
{
	var a, b, c, i, n, p1;

	c = COEFF.d;

	n = stack.length;

	for (i = h; i < n; i++) {

		p1 = stack[i];

		if (isradical(p1)) {

			push(cadr(p1)); // base
			a = pop_double();

			push(caddr(p1)); // exponent
			b = pop_double();

			c = c * Math.pow(a, b); // a > 0 by isradical above

			stack.splice(i, 1); // remove factor

			i--; // use same index again
			n--;
		}
	}

	push_double(c);
	COEFF = pop();

	return COEFF;
}
function
reduce_radical_factors(h, COEFF)
{
	if (!any_radical_factors(h))
		return COEFF;

	if (isrational(COEFF))
		return reduce_radical_rational(h, COEFF);
	else
		return reduce_radical_double(h, COEFF);
}
function
reduce_radical_rational(h, COEFF)
{
	var i, k, n, p1, p2, NUMER, DENOM, BASE, EXPO;

	if (isplusone(COEFF) || isminusone(COEFF))
		return COEFF; // COEFF has no factors, no cancellation is possible

	push(COEFF);
	abs();
	p1 = pop();

	push(p1);
	numerator();
	NUMER = pop();

	push(p1);
	denominator();
	DENOM = pop();

	k = 0;

	n = stack.length;

	for (i = h; i < n; i++) {
		p1 = stack[i];
		if (!isradical(p1))
			continue;
		BASE = cadr(p1);
		EXPO = caddr(p1);
		if (isnegativenumber(EXPO)) {
			mod_integers(NUMER, BASE);
			p2 = pop();
			if (iszero(p2)) {
				push(NUMER);
				push(BASE);
				divide();
				NUMER = pop();
				push_symbol(POWER);
				push(BASE);
				push_integer(1);
				push(EXPO);
				add();
				list(3);
				stack[i] = pop();
				k++;
			}
		} else {
			mod_integers(DENOM, BASE);
			p2 = pop();
			if (iszero(p2)) {
				push(DENOM);
				push(BASE);
				divide();
				DENOM = pop();
				push_symbol(POWER);
				push(BASE);
				push_integer(-1);
				push(EXPO);
				add();
				list(3);
				stack[i] = pop();
				k++;
			}
		}
	}

	if (k) {
		push(NUMER);
		push(DENOM);
		divide();
		if (isnegativenumber(COEFF))
			negate();
		COEFF = pop();
	}

	return COEFF;
}
function
restore_binding(p)
{
	var p1, p2;

	p2 = frame.pop();
	p1 = frame.pop();

	set_binding_and_arglist(p, p1, p2);
}
function
rewrite(p0) // p0 is arg subst list
{
	var count, h, i, n, p1, p2;

	p1 = pop();

	count = 0;

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1.elem.length;
		for (i = 0; i < n; i++) {
			push(p1.elem[i]);
			count += rewrite(p0);
			p1.elem[i] = pop();
		}
		push(p1);
		return count;
	}

	if (iscons(p1)) {

		h = stack.length;

		push(car(p1)); // don't rewrite function name
		p1 = cdr(p1);

		while (iscons(p1)) {
			push(car(p1));
			count += rewrite(p0);
			p1 = cdr(p1);
		}

		list(stack.length - h);

		return count;
	}

	// if not a symbol then done

	if (!issymbol(p1)) {
		push(p1);
		return 0; // no substitution
	}

	// check argument substitution list

	p2 = p0;
	while (iscons(p2)) {
		if (p1 == car(p2)) {
			push(cadr(p2));
			return 1; // substitution occurred
		}
		p2 = cddr(p2);
	}

	// get the symbol's binding, try again

	p2 = get_binding(p1);

	if (p1 == p2) {
		push(p1);
		return 0; // no substitution
	}

	push(p2);

	count = rewrite(p0);

	if (count == 0) {
		pop(); // undo
		push(p1);
	}

	return count;
}
function
run(s)
{
	try {
		run_nib(s);
	}

	catch(err) {
		//
	}

	finally {
		//
	}
}
function
run_nib(s)
{
	var k = 0;

	inbuf = s;

	init();
	initscript();

	for (;;) {

		k = scan_input(k);

		if (k == 0)
			break; // end of input

		eval_and_print_result();
	}
}
function
save_binding(p)
{
	frame.push(get_binding(p));
	frame.push(get_arglist(p));
}
const T_INTEGER = 1001;
const T_DOUBLE = 1002;
const T_SYMBOL = 1003;
const T_FUNCTION = 1004;
const T_NEWLINE = 1005;
const T_STRING = 1006;
const T_GTEQ = 1007;
const T_LTEQ = 1008;
const T_EQ = 1009;
const T_END = 1010;

var scan_mode;
var instring;
var scan_length;
var scan_index;
var scan_level;
var token;
var token_index;
var token_buf;

function
scan(s, k)
{
	scan_mode = 0;
	return scan_nib(s, k);
}

function
scan1(s)
{
	scan_mode = 1; // mode for table of integrals
	return scan_nib(s, 0);
}

function
scan_nib(s, k)
{
	instring = s;

	scan_length = s.length;
	scan_index = k;
	scan_level = 0;

	get_token_skip_newlines();

	if (token == T_END)
		return 0;

	scan_stmt();

	if (token != T_NEWLINE && token != T_END)
		scan_error("expected newline");

	return scan_index;
}

function
scan_stmt()
{
	scan_comparison();
	if (token == '=') {
		get_token_skip_newlines(); // get token after '='
		push_symbol(SETQ);
		swap();
		scan_comparison();
		list(3);
	}
}

function
scan_comparison()
{
	scan_expression();
	switch (token) {
	case T_EQ:
		push_symbol(TESTEQ); // ==
		break;
	case T_LTEQ:
		push_symbol(TESTLE);
		break;
	case T_GTEQ:
		push_symbol(TESTGE);
		break;
	case '<':
		push_symbol(TESTLT);
		break;
	case '>':
		push_symbol(TESTGT);
		break;
	default:
		return;
	}
	swap();
	get_token_skip_newlines(); // get token after rel op
	scan_expression();
	list(3);
}

function
scan_expression()
{
	var h = stack.length, t = token;
	if (token == '+' || token == '-')
		get_token_skip_newlines();
	scan_term();
	if (t == '-')
		static_negate();
	while (token == '+' || token == '-') {
		t = token;
		get_token_skip_newlines(); // get token after '+' or '-'
		scan_term();
		if (t == '-')
			static_negate();
	}
	if (stack.length - h > 1) {
		list(stack.length - h);
		push_symbol(ADD);
		swap();
		cons();
	}
}

function
scan_term()
{
	var h = stack.length, t;

	scan_power();

	while (scan_factor_pending()) {

		t = token;

		if (token == '*' || token == '/')
			get_token_skip_newlines();

		scan_power();

		if (t == '/')
			static_reciprocate();
	}

	if (stack.length - h > 1) {
		list(stack.length - h);
		push_symbol(MULTIPLY);
		swap();
		cons();
	}
}

function
scan_factor_pending()
{
	switch (token) {
	case '*':
	case '/':
	case '(':
	case T_SYMBOL:
	case T_FUNCTION:
	case T_INTEGER:
	case T_DOUBLE:
	case T_STRING:
		return 1;
	default:
		break;
	}
	return 0;
}

function
scan_power()
{
	scan_factor();

	if (token == '^') {

		get_token_skip_newlines();

		push_symbol(POWER);
		swap();
		scan_power();
		list(3);
	}
}

function
scan_factor()
{
	var h, n;

	h = stack.length;

	switch (token) {

	case '(':
		scan_subexpr();
		break;

	case T_SYMBOL:
		scan_symbol();
		break;

	case T_FUNCTION:
		scan_function_call();
		break;

	case T_INTEGER:
		n = parseFloat(token_buf);
		if (n > 1e10)
			push_double(n);
		else
			push_integer(n);
		get_token();
		break;

	case T_DOUBLE:
		n = parseFloat(token_buf);
		push_double(n);
		get_token();
		break;

	case T_STRING:
		scan_string();
		break;

	default:
		scan_error("expected operand");
		break;
	}

	// index

	if (token == '[') {

		scan_level++;

		get_token(); // get token after '['
		push_symbol(INDEX);
		swap();

		scan_expression();

		while (token == ',') {
			get_token(); // get token after ','
			scan_expression();
		}

		if (token != ']')
			scan_error("expected ']'");

		scan_level--;

		get_token(); // get token after ']'

		list(stack.length - h);
	}

	while (token == '!') {
		get_token(); // get token after '!'
		push_symbol(FACTORIAL);
		swap();
		list(2);
	}
}

function
scan_symbol()
{
	if (scan_mode == 1 && token_buf.length == 1) {
		switch (token_buf[0]) {
		case 'a':
			push_symbol(METAA);
			break;
		case 'b':
			push_symbol(METAB);
			break;
		case 'x':
			push_symbol(METAX);
			break;
		default:
			push(lookup(token_buf));
			break;
		}
	} else
		push(lookup(token_buf));
	get_token();
}

function
scan_string()
{
	push_string(token_buf);
	get_token();
}

function
scan_function_call()
{
	var h = stack.length;
	scan_level++;
	push(lookup(token_buf)); // push function name
	get_token(); // get token after function name
	get_token(); // get token after '('
	if (token == ')') {
		scan_level--;
		get_token(); // get token after ')'
		list(1); // function call with no args
		return;
	}
	scan_stmt();
	while (token == ',') {
		get_token(); // get token after ','
		scan_stmt();
	}
	if (token != ')')
		scan_error("expected ')'");
	scan_level--;
	get_token(); // get token after ')'
	list(stack.length - h);
}

function
scan_subexpr()
{
	var h = stack.length;

	scan_level++;

	get_token(); // get token after '('

	scan_stmt();

	while (token == ',') {
		get_token(); // get token after ','
		scan_stmt();
	}

	if (token != ')')
		scan_error("expected ')'");

	scan_level--;

	get_token(); // get token after ')'

	if (stack.length - h > 1)
		vector(h);
}

function
get_token_skip_newlines()
{
	scan_level++;
	get_token();
	scan_level--;
}

function
get_token()
{
	get_token_nib();

	if (scan_level)
		while (token == T_NEWLINE)
			get_token_nib(); // skip over newlines
}

function
get_token_nib()
{
	var c;

	// skip spaces

	while (scan_index < scan_length && (instring[scan_index] == '\t' || instring[scan_index] == ' '))
		scan_index++;

	token_index = scan_index;

	// end of string?

	if (scan_index == scan_length) {
		token = T_END;
		return;
	}

	c = instring[scan_index];

	// newline?

	if (c == '\n') {
		scan_index++;
		token = T_NEWLINE;
		return;
	}

	// comment?

	if (c == '#' || (c == '-' && scan_index < scan_length - 1 && instring[scan_index + 1] == '-')) {

		while (scan_index < scan_length && instring[scan_index] != '\n')
			scan_index++;

		if (scan_index < scan_length)
			scan_index++;

		token = T_NEWLINE;

		return;
	}

	// number?

	if (isdigit(c) || c == '.') {

		while (scan_index < scan_length && isdigit(instring[scan_index]))
			scan_index++;

		if (scan_index < scan_length && instring[scan_index] == '.') {

			scan_index++;

			while (scan_index < scan_length && isdigit(instring[scan_index]))
				scan_index++;

			if (scan_index - token_index == 1)
				scan_error("expected decimal digit"); // only a decimal point

			token = T_DOUBLE;
		} else
			token = T_INTEGER;

		update_token_buf(token_index, scan_index);

		return;
	}

	// symbol?

	if (isalpha(c)) {

		while (scan_index < scan_length && isalnum(instring[scan_index]))
			scan_index++;

		if (scan_index < scan_length && instring[scan_index] == '(')
			token = T_FUNCTION;
		else
			token = T_SYMBOL;

		update_token_buf(token_index, scan_index);

		return;
	}

	// string ?

	if (c == '"') {
		scan_index++;
		while (scan_index < scan_length && instring[scan_index] != '"') {
			if (scan_index == scan_length || instring[scan_index] == '\n') {
				token_index = scan_index;
				scan_error("runaway string");
			}
			scan_index++;
		}
		scan_index++;
		token = T_STRING;
		update_token_buf(token_index + 1, scan_index - 1); // don't include quote chars
		return;
	}

	// relational operator?

	if (scan_index < scan_length) {

		if (c == '=' && instring[scan_index + 1] == '=') {
			scan_index += 2;
			token = T_EQ;
			return;
		}

		if (c == '<' && instring[scan_index + 1] == '=') {
			scan_index += 2;
			token = T_LTEQ;
			return;
		}

		if (c == '>' && instring[scan_index + 1] == '=') {
			scan_index += 2;
			token = T_GTEQ;
			return;
		}
	}

	// single char token

	token = instring[scan_index++];
}

function
update_token_buf(j, k)
{
	token_buf = instring.substring(j, k);
}

function
scan_error(s)
{
	var t = inbuf.substring(trace1, scan_index);

	t += "\nStop: Syntax error, " + s;

	if (token_index < scan_index) {
		t += " instead of ";
		t += instring.substring(token_index, scan_index);
	}

	print_buf(t, RED);

	stopf("");
}
function
scan_input(k)
{
	trace1 = k;
	k = scan(inbuf, k);
	if (k) {
		trace2 = k;
		trace_input();
	}
	return k;
}
function
set_binding(p, b)
{
	if ("printname" in p) {
		binding[p.printname] = b;
		arglist[p.printname] = symbol(NIL);
	}
}
function
set_binding_and_arglist(p, b, a)
{
	if ("printname" in p) {
		binding[p.printname] = b;
		arglist[p.printname] = a;
	}
}
function
set_component(LVAL, RVAL, h)
{
	var i, k, n, t, p1;

	if (!istensor(LVAL) || istensor(RVAL))
		stopf("index err");

	n = stack.length - h;

	if (n != LVAL.dim.length)
		stopf("index err");

	k = 0;

	for (i = 0; i < n; i++) {
		push(stack[h + i]);
		t = pop_integer();
		if (t < 1 || t > LVAL.dim[i])
			stopf("index err");
		k = k * LVAL.dim[i] + t - 1;
	}

	stack.splice(h); // pop all

	p1 = copy_tensor(LVAL);
	p1.elem[k] = RVAL;
	push(p1);
}
function
setq_indexed(p1)
{
	var h, p2, S, LVAL, RVAL;

	S = cadadr(p1);

	if (!isusersymbol(S))
		stopf("symbol expected");

	push(S);
	evalf();
	LVAL = pop();

	push(caddr(p1));
	evalf();
	RVAL = pop();

	h = stack.length;

	p2 = cddadr(p1);

	while (iscons(p2)) {
		push(car(p2));
		evalf();
		p2 = cdr(p2);
	}

	set_component(LVAL, RVAL, h);

	p2 = pop();
	set_binding(S, p2);
}

// Example: a[1] = b
//
// p1----->cons--->cons------------------->cons
//         |       |                       |
//         setq    cons--->cons--->cons    b
//                 |       |       |
//                 index   a       1
//
// caadr(p1) = index
// cadadr(p1) = a
// caddr(p1) = b
function
setq_userfunc(p1)
{
	var A, B, F;

	F = caadr(p1);
	A = cdadr(p1);
	B = caddr(p1);

	if (!issymbol(F))
		stopf("function name?");

	set_binding_and_arglist(F, B, A);
}
function
sgn()
{
	var p1 = pop();

	if (!isnum(p1)) {
		push_symbol(SGN);
		push(p1);
		list(2);
		return;
	}

	if (iszero(p1)) {
		push_integer(0);
		return;
	}

	if (isnegativenumber(p1))
		push_integer(-1);
	else
		push_integer(1);
}
function
simplify_polar_expr(EXPO)
{
	var p1;

	if (car(EXPO) == symbol(ADD)) {
		p1 = cdr(EXPO);
		while (iscons(p1)) {
			if (simplify_polar_term(car(p1))) {
				push(EXPO);
				push(car(p1));
				subtract();
				exp();
				multiply();
				return 1;
			}
			p1 = cdr(p1);
		}
		return 0;
	}

	return simplify_polar_term(EXPO);
}
function
simplify_polar_term(p)
{
	var d, n;

	if (car(p) != symbol(MULTIPLY))
		return 0;

	// exp(i pi) -> -1

	if (lengthf(p) == 3 && isimaginaryunit(cadr(p)) && caddr(p) == symbol(PI)) {
		push_integer(-1);
		return 1;
	}

	if (lengthf(p) != 4 || !isnum(cadr(p)) || !isimaginaryunit(caddr(p)) || cadddr(p) != symbol(PI))
		return 0;

	p = cadr(p); // coeff

	if (isdouble(p)) {
		d = p.d;
		if (Math.floor(d) == d) {
			if (d % 2 == 0)
				push_double(1.0);
			else
				push_double(-1.0);
			return 1;
		}
		if (Math.floor(d) + 0.5 == d) {
			n = (d / 0.5) % 4;
			if (n == 1 || n == -3) {
				push_symbol(MULTIPLY);
				push_double(1.0);
				push(imaginaryunit);
				list(3);
			} else {
				push_symbol(MULTIPLY);
				push_double(-1.0);
				push(imaginaryunit);
				list(3);
			}
			return 1;
		}
		return 0;
	}

	if (p.b == 1) {
		if (Math.abs(p.a) % 2 == 0)
			push_integer(1);
		else
			push_integer(-1);
		return 1;
	}

	if (p.b == 2) {
		n = Math.abs(p.a) % 4;
		if ((n == 1 && p.a > 0) || (n == 3 && p.a < 0))
			push(imaginaryunit);
		else {
			push_symbol(MULTIPLY);
			push_integer(-1);
			push(imaginaryunit);
			list(3);
		}
		return 1;
	}

	return 0;
}
function
sin()
{
	var n, p1, p2, X, Y;

	p1 = pop();

	if (isdouble(p1)) {
		push_double(Math.sin(p1.d));
		return;
	}

	// sin(z) = -i/2 (exp(i z) - exp(-i z))

	if (isdoublez(p1)) {
		push_double(-0.5);
		push(imaginaryunit);
		multiply();
		push(imaginaryunit);
		push(p1);
		multiply();
		exp();
		push(imaginaryunit);
		negate();
		push(p1);
		multiply();
		exp();
		subtract();
		multiply();
		return;
	}

	// sin(-x) = -sin(x)

	if (isnegative(p1)) {
		push(p1);
		negate();
		sin();
		negate();
		return;
	}

	if (car(p1) == symbol(ADD)) {
		sin_sum(p1);
		return;
	}

	// sin(arctan(y,x)) = y (x^2 + y^2)^(-1/2)

	if (car(p1) == symbol(ARCTAN)) {
		X = caddr(p1);
		Y = cadr(p1);
		push(Y);
		push(X);
		push(X);
		multiply();
		push(Y);
		push(Y);
		multiply();
		add();
		push_rational(-1, 2);
		power();
		multiply();
		return;
	}

	// multiply by 180/pi

	push(p1); // nonnegative by code above
	push_integer(180);
	multiply();
	push_symbol(PI);
	divide();
	p2 = pop();

	if (isrational(p2))
		n = p2.a / p2.b;
	else if (isdouble(p2))
		n = p2.d;
	else
		n = 1; // force default case

	switch (n % 360) {
	case 0:
	case 180:
		push_integer(0);
		break;
	case 30:
	case 150:
		push_rational(1, 2);
		break;
	case 210:
	case 330:
		push_rational(-1, 2);
		break;
	case 45:
	case 135:
		push_rational(1, 2);
		push_integer(2);
		push_rational(1, 2);
		power();
		multiply();
		break;
	case 225:
	case 315:
		push_rational(-1, 2);
		push_integer(2);
		push_rational(1, 2);
		power();
		multiply();
		break;
	case 60:
	case 120:
		push_rational(1, 2);
		push_integer(3);
		push_rational(1, 2);
		power();
		multiply();
		break;
	case 240:
	case 300:
		push_rational(-1, 2);
		push_integer(3);
		push_rational(1, 2);
		power();
		multiply();
		break;
	case 90:
		push_integer(1);
		break;
	case 270:
		push_integer(-1);
		break;
	default:
		push_symbol(SIN);
		push(p1);
		list(2);
		break;
	}
}
function
sin_sum(p1) // sin(x + n/2 pi) = sin(x) cos(n/2 pi) + cos(x) sin(n/2 pi)
{
	var p2, p3;
	p2 = cdr(p1);
	while (iscons(p2)) {
		push_integer(2);
		push(car(p2));
		multiply();
		push_symbol(PI);
		divide();
		p3 = pop();
		if (isinteger(p3)) {
			push(p1);
			push(car(p2));
			subtract();
			p3 = pop();
			push(p3);
			sin();
			push(car(p2));
			cos();
			multiply();
			push(p3);
			cos();
			push(car(p2));
			sin();
			multiply();
			add();
			return;
		}
		p2 = cdr(p2);
	}
	push_symbol(SIN);
	push(p1);
	list(2);
}
function
sinh()
{
	var p1 = pop();

	// sinh(z) = 1/2 (exp(z) - exp(-z))

	if (isdouble(p1) || isdoublez(p1)) {
		push_rational(1, 2);
		push(p1);
		exp();
		push(p1);
		negate();
		exp();
		subtract();
		multiply();
		return;
	}

	if (iszero(p1)) {
		push_integer(0);
		return;
	}

	// sinh(-x) -> -sinh(x)

	if (isnegative(p1)) {
		push(p1);
		negate();
		sinh();
		negate();
		return;
	}

	if (car(p1) == symbol(ARCSINH)) {
		push(cadr(p1));
		return;
	}

	push_symbol(SINH);
	push(p1);
	list(2);
}
function
sort_factors(h)
{
	var t = stack.splice(h).sort(cmp_factors);
	stack = stack.concat(t);
}
function
sort_factors_provisional(h)
{
	var t = stack.splice(h).sort(cmp_factors_provisional);
	stack = stack.concat(t);
}
function
sort_terms(h)
{
	var t = stack.splice(h).sort(cmp_terms);
	stack = stack.concat(t);
}
function
static_negate()
{
	var p1 = pop();

	if (isnum(p1)) {
		push(p1);
		negate();
		return;
	}

	if (car(p1) == symbol(MULTIPLY)) {
		push_symbol(MULTIPLY);
		if (isnum(cadr(p1))) {
			push(cadr(p1));
			negate();
			push(cddr(p1));
		} else {
			push_integer(-1);
			push(cdr(p1));
		}
		cons();
		cons();
		return;
	}

	push_symbol(MULTIPLY);
	push_integer(-1);
	push(p1);
	list(3);
}
function
static_reciprocate()
{
	var p1, p2;

	p2 = pop();
	p1 = pop();

	// save divide by zero error for runtime

	if (iszero(p2)) {
		push(p1);
		push_symbol(POWER);
		push(p2);
		push_integer(-1);
		list(3);
		return;
	}

	if (isnum(p1) && isnum(p2)) {
		divide_numbers(p1, p2);
		return;
	}

	if (isnum(p2)) {
		push(p1);
		push(p2);
		reciprocate();
		return;
	}

	push(p1);
	push_symbol(POWER);
	push(p2);
	push_integer(-1);
	list(3);
}
function
stopf(s)
{
	if (s.length > 0) {
		if (trace1 < trace2 && inbuf[trace2 - 1] == '\n')
			trace2--;
		print_buf(inbuf.substring(trace1, trace2) + "\nStop: " + s, RED);
	}

	throw s;
}
function
subst()
{
	var h, i, n, p1, p2, p3;

	p3 = pop(); // new expr
	p2 = pop(); // old expr

	if (p2 == symbol(NIL) || p3 == symbol(NIL))
		return;

	p1 = pop(); // expr

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1.elem.length;
		for (i = 0; i < n; i++) {
			push(p1.elem[i]);
			push(p2);
			push(p3);
			subst();
			p1.elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (iscons(p1)) {
		h = stack.length;
		while (iscons(p1)) {
			push(car(p1));
			push(p2);
			push(p3);
			subst();
			p1 = cdr(p1);
		}
		list(stack.length - h);
		return;
	}

	if (equal(p1, p2))
		push(p3);
	else
		push(p1);
}
function
subtract()
{
	if (isnum(stack[stack.length - 2]) && isnum(stack[stack.length - 1])) {
		var p2 = pop();
		var p1 = pop();
		subtract_numbers(p1, p2);
		return;
	}

	negate();
	add();
}
function
subtract_numbers(p1, p2)
{
	var d1, d2;

	if (isrational(p1) && isrational(p2)) {
		subtract_rationals(p1, p2);
		return;
	}

	push(p1);
	d1 = pop_double();

	push(p2);
	d2 = pop_double();

	push_double(d1 - d2);
}
function
subtract_rationals(p1, p2)
{
	var a, b, c;

	a = p1.a * p2.b - p1.b * p2.a;
	b = p1.b * p2.b;

	if (a == 0) {
		push_integer(0);
		return;
	}

	if (Math.abs(a) > 1e10 || b > 1e10) {
		push_double(a / b);
		return;
	}

	if (b > 1) {
		c = gcd_integers(a, b);
		a /= c;
		b /= c;
	}

	push_rational(a, b);
}
function
swap()
{
	var p1, p2;
	p2 = pop();
	p1 = pop();
	push(p2);
	push(p1);
}
function
symbol(s)
{
	return symtab[s];
}
function
tan()
{
	var n, p1, p2;

	p1 = pop();

	if (isdouble(p1)) {
		push_double(Math.tan(p1.d));
		return;
	}

	if (isdoublez(p1)) {
		push(p1);
		sin();
		push(p1);
		cos();
		divide();
		return;
	}

	// tan(-x) = -tan(x)

	if (isnegative(p1)) {
		push(p1);
		negate();
		tan();
		negate();
		return;
	}

	if (car(p1) == symbol(ADD)) {
		tan_sum(p1);
		return;
	}

	if (car(p1) == symbol(ARCTAN)) {
		push(cadr(p1));
		push(caddr(p1));
		divide();
		return;
	}

	// multiply by 180/pi

	push(p1); // nonnegative by code above
	push_integer(180);
	multiply();
	push_symbol(PI);
	divide();
	p2 = pop();

	if (isrational(p2))
		n = p2.a / p2.b;
	else if (isdouble(p2))
		n = p2.d;
	else
		n = 1; // force default case

	switch (n % 360) {
	case 0:
	case 180:
		push_integer(0);
		break;
	case 30:
	case 210:
		push_rational(1, 3);
		push_integer(3);
		push_rational(1, 2);
		power();
		multiply();
		break;
	case 150:
	case 330:
		push_rational(-1, 3);
		push_integer(3);
		push_rational(1, 2);
		power();
		multiply();
		break;
	case 45:
	case 225:
		push_integer(1);
		break;
	case 135:
	case 315:
		push_integer(-1);
		break;
	case 60:
	case 240:
		push_integer(3);
		push_rational(1, 2);
		power();
		break;
	case 120:
	case 300:
		push_integer(3);
		push_rational(1, 2);
		power();
		negate();
		break;
	default:
		push_symbol(TAN);
		push(p1);
		list(2);
		break;
	}
}
function
tan_sum(p1) // tan(x + n pi) = tan(x)
{
	var p2, p3;
	p2 = cdr(p1);
	while (iscons(p2)) {
		push(car(p2));
		push_symbol(PI);
		divide();
		p3 = pop();
		if (isinteger(p3)) {
			push(p1);
			push(car(p2));
			subtract();
			tan();
			return;
		}
		p2 = cdr(p2);
	}
	push_symbol(TAN);
	push(p1);
	list(2);
}
function
tanh()
{
	var p1 = pop();

	if (isdouble(p1) || isdoublez(p1)) {
		push(p1);
		sinh();
		push(p1);
		cosh();
		divide();
		return;
	}

	if (iszero(p1)) {
		push_integer(0);
		return;
	}

	// tanh(-x) = -tanh(x)

	if (isnegative(p1)) {
		push(p1);
		negate();
		tanh();
		negate();
		return;
	}

	if (car(p1) == symbol(ARCTANH)) {
		push(cadr(p1));
		return;
	}

	push_symbol(TANH);
	push(p1);
	list(2);
}
function
tensor_factor(h)
{
	var i, n, p1, p2;
	p2 = symbol(NIL);
	n = stack.length;
	for (i = h; i < n; i++) {
		p1 = stack[i];
		if (istensor(p1)) {
			if (istensor(p2))
				stopf("dot is used for matrix products");
			p2 = p1;
			stack.splice(i, 1); // remove factor
			i--; // use same index again
			n--;
		}
	}
	return p2;
}
function
testeq(p1, p2)
{
	var p3;
	push(p1);
	push(p2);
	subtract();
	p3 = pop();
	while (divisor(p3)) {
		push(p3);
		cancel_factor();
		p3 = pop();
	}
	return iszero(p3);
}
function
trace_input()
{
	if (!iszero(get_binding(symbol(TRACE))))
		print_buf(instring.substring(trace1, trace2), BLUE);
}
function
transpose(n, m)
{
	var i, j, k, ndim, p1, p2;
	var index = [];

	p1 = pop();

	ndim = p1.dim.length;

	if (n < 1 || n > ndim || m < 1 || m > ndim)
		stopf("index err");

	n--; // make zero based
	m--;

	p2 = alloc_tensor();

	for (i = 0; i < ndim; i++)
		p2.dim[i] = p1.dim[i];

	// interchange indices n and m

	p2.dim[n] = p1.dim[m];
	p2.dim[m] = p1.dim[n];

	for (i = 0; i < ndim; i++)
		index[i] = 0;

	for (i = 0; i < p1.elem.length; i++) {

		k = 0;

		for (j = 0; j < ndim; j++) {
			if (j == n)
				k = k * p1.dim[m] + index[m];
			else if (j == m)
				k = k * p1.dim[n] + index[n];
			else
				k = k * p1.dim[j] + index[j];
		}

		p2.elem[k] = p1.elem[i];

		// increment index

		for (j = ndim - 1; j >= 0; j--) {
			if (++index[j] < p1.dim[j])
				break;
			index[j] = 0;
		}
	}

	push(p2);
}
var stack;
var frame;
var binding;
var arglist;
var zero;
var one;
var minusone;
var imaginaryunit;
var expanding;
var evaldepth;
var inbuf;
var trace1;
var trace2;

var symtab = {
abs:		{printname:ABS,		func:eval_abs},
adj:		{printname:ADJ,		func:eval_adj},
and:		{printname:AND,		func:eval_and},
arccos:		{printname:ARCCOS,	func:eval_arccos},
arccosh:	{printname:ARCCOSH,	func:eval_arccosh},
arcsin:		{printname:ARCSIN,	func:eval_arcsin},
arcsinh:	{printname:ARCSINH,	func:eval_arcsinh},
arctan:		{printname:ARCTAN,	func:eval_arctan},
arctanh:	{printname:ARCTANH,	func:eval_arctanh},
arg:		{printname:ARG,		func:eval_arg},
binding:	{printname:BINDING,	func:eval_binding},
check:		{printname:CHECK,	func:eval_check},
circexp:	{printname:CIRCEXP,	func:eval_circexp},
clear:		{printname:CLEAR,	func:eval_clear},
clock:		{printname:CLOCK,	func:eval_clock},
conj:		{printname:CONJ,	func:eval_conj},
contract:	{printname:CONTRACT,	func:eval_contract},
cos:		{printname:COS,		func:eval_cos},
cosh:		{printname:COSH,	func:eval_cosh},
defint:		{printname:DEFINT,	func:eval_defint},
denominator:	{printname:DENOMINATOR,	func:eval_denominator},
derivative:	{printname:DERIVATIVE,	func:eval_derivative},
det:		{printname:DET,		func:eval_det},
"do":		{printname:DO,		func:eval_do},
dot:		{printname:DOT,		func:eval_dot},
"eval":		{printname:EVAL,	func:eval_eval},
exp:		{printname:EXP,		func:eval_exp},
expcos:		{printname:EXPCOS,	func:eval_expcos},
expcosh:	{printname:EXPCOSH,	func:eval_expcosh},
expsin:		{printname:EXPSIN,	func:eval_expsin},
expsinh:	{printname:EXPSINH,	func:eval_expsinh},
exptan:		{printname:EXPTAN,	func:eval_exptan},
exptanh:	{printname:EXPTANH,	func:eval_exptanh},
factorial:	{printname:FACTORIAL,	func:eval_factorial},
"float":	{printname:FLOAT,	func:eval_float},
floor:		{printname:FLOOR,	func:eval_floor},
"for":		{printname:FOR,		func:eval_for},
imag:		{printname:IMAG,	func:eval_imag},
inner:		{printname:INNER,	func:eval_inner},
integral:	{printname:INTEGRAL,	func:eval_integral},
inv:		{printname:INV,		func:eval_inv},
log:		{printname:LOG,		func:eval_log},
mag:		{printname:MAG,		func:eval_mag},
nil:		{printname:NIL,		func:eval_nil},
not:		{printname:NOT,		func:eval_not},
number:		{printname:NUMBER,	func:eval_number},
numerator:	{printname:NUMERATOR,	func:eval_numerator},
or:		{printname:OR,		func:eval_or},
outer:		{printname:OUTER,	func:eval_outer},
polar:		{printname:POLAR,	func:eval_polar},
prefixform:	{printname:PREFIXFORM,	func:eval_prefixform},
print:		{printname:PRINT,	func:eval_print},
product:	{printname:PRODUCT,	func:eval_product},
quote:		{printname:QUOTE,	func:eval_quote},
rank:		{printname:RANK,	func:eval_rank},
rationalize:	{printname:RATIONALIZE,	func:eval_rationalize},
real:		{printname:REAL,	func:eval_real},
rect:		{printname:RECT,	func:eval_rect},
sgn:		{printname:SGN,		func:eval_sgn},
sin:		{printname:SIN,		func:eval_sin},
sinh:		{printname:SINH,	func:eval_sinh},
sqrt:		{printname:SQRT,	func:eval_sqrt},
"stop":		{printname:STOP,	func:eval_stop},
sum:		{printname:SUM,		func:eval_sum},
tan:		{printname:TAN,		func:eval_tan},
tanh:		{printname:TANH,	func:eval_tanh},
test:		{printname:TEST,	func:eval_test},
testeq:		{printname:TESTEQ,	func:eval_testeq},
testge:		{printname:TESTGE,	func:eval_testge},
testgt:		{printname:TESTGT,	func:eval_testgt},
testle:		{printname:TESTLE,	func:eval_testle},
testlt:		{printname:TESTLT,	func:eval_testlt},
transpose:	{printname:TRANSPOSE,	func:eval_transpose},
unit:		{printname:UNIT,	func:eval_unit},
zero:		{printname:ZERO,	func:eval_zero},

"+":		{printname:ADD,		func:eval_add},
"*":		{printname:MULTIPLY,	func:eval_multiply},
"^":		{printname:POWER,	func:eval_power},
"[":		{printname:INDEX,	func:eval_index},
"=":		{printname:SETQ,	func:eval_setq},

"(e)":		{printname:EXP1,	func:eval_symbol},
"(a)":		{printname:METAA,	func:eval_symbol},
"(b)":		{printname:METAB,	func:eval_symbol},
"(x)":		{printname:METAX,	func:eval_symbol},
"(X)":		{printname:SPECX,	func:eval_symbol},

last:		{printname:LAST,	func:eval_symbol},
pi:		{printname:PI,		func:eval_symbol},
trace:		{printname:TRACE,	func:eval_symbol},

d:		{printname:SYMBOL_D,	func:eval_symbol},
i:		{printname:SYMBOL_I,	func:eval_symbol},
j:		{printname:SYMBOL_J,	func:eval_symbol},
s:		{printname:SYMBOL_S,	func:eval_symbol},
t:		{printname:SYMBOL_T,	func:eval_symbol},
x:		{printname:SYMBOL_X,	func:eval_symbol},
y:		{printname:SYMBOL_Y,	func:eval_symbol},
z:		{printname:SYMBOL_Z,	func:eval_symbol},

};
function
vector(h)
{
	var n, p;

	n = stack.length - h;

	p = alloc_tensor();

	p.dim[0] = n;
	p.elem = stack.splice(h, n);

	push(p);
}

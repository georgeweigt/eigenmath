function
simplify_polar_term(p)
{
	var p0;

	if (car(p) != symbol(MULTIPLY))
		return 0;

	// exp(i pi) -> -1

	if (lengthf(p) == 3 && isimaginaryunit(cadr(p)) && caddr(p) == symbol(PI)) {
		push_integer(-1);
		return 1;
	}

	if (lengthf(p) != 4 || !isnum(cadr(p)) || !isimaginaryunit(caddr(p)) || cadddr(p) != symbol(PI))
		return 0;

	p = cadr(p); // p = coeff

	if (isdouble(p)) {
		if (0 < p.d && p.d < 0.5)
			return 0; // nothing to do
		normalize_polar_double_coeff(p.d);
		return 1;
	}

	// coeff is a rational number

	if (p.a > 0) {
		push(p);
		push_rational(1, 2);
		subtract();
		p0 = pop();
		if (p0.a < 0)
			return 0; // 0 < coeff < 1/2
	}

	normalize_polar_rational_coeff(p);

	return 1;
}

function
normalize_polar_rational_coeff(p)
{
	var n, r;

	// p = p mod 2

	push(p);
	push_integer(2);
	mod();
	p = pop();

	// convert negative rotation to positive

	if (p.a < 0) {
		push_integer(2);
		push(p);
		add();
		p = pop();
	}

	push(p);
	push_rational(1, 2);
	mod();
	r = pop(); // remainder

	push(p);
	push(r);
	subtract();
	push_integer(2);
	multiply();
	n = pop_integer(); // number of 1/4 turns

	switch (n) {

	case 0:
		if (iszero(r))
			push_integer(1);
		else {
			push_symbol(POWER);
			push_symbol(EXP1);
			push_symbol(MULTIPLY);
			push(r);
			push(imaginaryunit);
			push_symbol(PI);
			list(4);
			list(3);
		}
		break;

	case 2:
		if (iszero(r))
			push_integer(-1);
		else {
			push_symbol(MULTIPLY);
			push_integer(-1);
			push_symbol(POWER);
			push_symbol(EXP1);
			push_symbol(MULTIPLY);
			push(r);
			push(imaginaryunit);
			push_symbol(PI);
			list(4);
			list(3);
			list(3);
		}
		break;

	case 1:
		if (iszero(r))
			push(imaginaryunit);
		else {
			push_symbol(MULTIPLY);
			push(imaginaryunit);
			push_symbol(POWER);
			push_symbol(EXP1);
			push_symbol(MULTIPLY);
			push(r);
			push(imaginaryunit);
			push_symbol(PI);
			list(4);
			list(3);
			list(3);
		}
		break;

	case 3:
		if (iszero(r)) {
			push_symbol(MULTIPLY);
			push_integer(-1);
			push(imaginaryunit);
			list(3);
		} else {
			push_symbol(MULTIPLY);
			push_integer(-1);
			push(imaginaryunit);
			push_symbol(POWER);
			push_symbol(EXP1);
			push_symbol(MULTIPLY);
			push(r);
			push(imaginaryunit);
			push_symbol(PI);
			list(4);
			list(3);
			list(4);
		}
		break;
	}
}

function
normalize_polar_double_coeff(coeff)
{
	var n, r;

	// coeff = coeff mod 2

	coeff = coeff % 2;

	// convert negative rotation to positive

	if (coeff < 0)
		coeff += 2;

	n = Math.floor(2 * coeff); // number of 1/4 turns

	r = coeff - n / 2; // remainder

	switch (n) {

	case 0:
		if (r == 0)
			push_integer(1);
		else {
			push_symbol(POWER);
			push_symbol(EXP1);
			push_symbol(MULTIPLY);
			push_double(r);
			push(imaginaryunit);
			push_symbol(PI);
			list(4);
			list(3);
		}
		break;

	case 2:
		if (r == 0)
			push_integer(-1);
		else {
			push_symbol(MULTIPLY);
			push_integer(-1);
			push_symbol(POWER);
			push_symbol(EXP1);
			push_symbol(MULTIPLY);
			push_double(r);
			push(imaginaryunit);
			push_symbol(PI);
			list(4);
			list(3);
			list(3);
		}
		break;

	case 1:
		if (r == 0)
			push(imaginaryunit);
		else {
			push_symbol(MULTIPLY);
			push(imaginaryunit);
			push_symbol(POWER);
			push_symbol(EXP1);
			push_symbol(MULTIPLY);
			push_double(r);
			push(imaginaryunit);
			push_symbol(PI);
			list(4);
			list(3);
			list(3);
		}
		break;

	case 3:
		if (r == 0) {
			push_symbol(MULTIPLY);
			push_integer(-1);
			push(imaginaryunit);
			list(3);
		} else {
			push_symbol(MULTIPLY);
			push_integer(-1);
			push(imaginaryunit);
			push_symbol(POWER);
			push_symbol(EXP1);
			push_symbol(MULTIPLY);
			push_double(r);
			push(imaginaryunit);
			push_symbol(PI);
			list(4);
			list(3);
			list(4);
		}
		break;
	}
}

function
normalize_polar(EXPO)
{
	var h, p1;
	if (car(EXPO) == symbol(ADD)) {
		h = stack.length;
		p1 = cdr(EXPO);
		while (iscons(p1)) {
			EXPO = car(p1);
			if (isdenormalpolar(EXPO))
				normalize_polar_term(EXPO);
			else {
				push_symbol(POWER);
				push_symbol(EXP1);
				push(EXPO);
				list(3);
			}
			p1 = cdr(p1);
		}
		multiply_factors(stack.length - h);
	} else
		normalize_polar_term(EXPO);
}

function
normalize_polar_term(EXPO)
{
	var R;

	// exp(i pi) = -1

	if (lengthf(EXPO) == 3) {
		push_integer(-1);
		return;
	}

	R = cadr(EXPO); // R = coeff of term

	if (isrational(R))
		normalize_polar_term_rational(R);
	else
		normalize_polar_term_double(R);
}

function
normalize_polar_term_rational(R)
{
	var n;

	// R = R mod 2

	push(R);
	push_integer(2);
	mod();
	R = pop();

	// convert negative rotation to positive

	if (R.a < 0) {
		push(R);
		push_integer(2);
		add();
		R = pop();
	}

	push(R);
	push_integer(2);
	multiply();
	floor();
	n = pop_integer(); // number of 90 degree turns

	push(R);
	push_integer(n);
	push_rational(-1, 2);
	multiply();
	add();
	R = pop(); // remainder

	switch (n) {

	case 0:
		if (iszero(R))
			push_integer(1);
		else {
			push_symbol(POWER);
			push_symbol(EXP1);
			push_symbol(MULTIPLY);
			push(R);
			push(imaginaryunit);
			push_symbol(PI);
			list(4);
			list(3);
		}
		break;

	case 1:
		if (iszero(R))
			push(imaginaryunit);
		else {
			push_symbol(MULTIPLY);
			push(imaginaryunit);
			push_symbol(POWER);
			push_symbol(EXP1);
			push_symbol(MULTIPLY);
			push(R);
			push(imaginaryunit);
			push_symbol(PI);
			list(4);
			list(3);
			list(3);
		}
		break;

	case 2:
		if (iszero(R))
			push_integer(-1);
		else {
			push_symbol(MULTIPLY);
			push_integer(-1);
			push_symbol(POWER);
			push_symbol(EXP1);
			push_symbol(MULTIPLY);
			push(R);
			push(imaginaryunit);
			push_symbol(PI);
			list(4);
			list(3);
			list(3);
		}
		break;

	case 3:
		if (iszero(R)) {
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
			push(R);
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
normalize_polar_term_double(R)
{
	var coeff, n, r;

	coeff = R.d;

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

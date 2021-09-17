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

	if (!isinteger(p2)) {
		push_symbol(ARCSIN);
		push(p1);
		list(2);
		return;
	}

	push(p2);
	n = pop_integer();

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

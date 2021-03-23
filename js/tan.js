function
tan()
{
	var n, p1, p2;

	p1 = pop();

	if (isdouble(p1)) {
		push_double(Math.tan(p1.d));
		return;
	}

	// 0?

	if (iszero(p1)) {
		push_integer(0);
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

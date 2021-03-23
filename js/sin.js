function
sin()
{
	var n, p1, p2, X, Y;

	p1 = pop();

	if (isdouble(p1)) {
		push_double(Math.sin(p1.d));
		return;
	}

	// 0?

	if (iszero(p1)) {
		push_integer(0);
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

	// n pi ?

	push(p1);
	push_symbol(PI);
	divide();
	p2 = pop();

	if (!isnum(p2)) {
		push_symbol(SIN);
		push(p1);
		list(2);
		return;
	}

	if (isdouble(p2)) {
		push_double(Math.sin(p2.d * Math.PI));
		return;
	}

	push(p2); // nonnegative by sin(-x) = -sin(x) above
	push_integer(180);
	multiply();
	p2 = pop();

	if (!isinteger(p2)) {
		push_symbol(SIN);
		push(p1);
		list(2);
		return;
	}

	n = p2.a % 360;

	switch (n) {
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

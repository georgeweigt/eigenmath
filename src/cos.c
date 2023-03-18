void
eval_cos(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	cosfunc();
}

void
cosfunc(void)
{
	int i, n;
	double d;
	struct atom *p1, *p2, *X, *Y;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			cosfunc();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (isdouble(p1)) {
		push(p1);
		d = pop_double();
		d = cos(d);
		push_double(d);
		return;
	}

	// cos(z) = 1/2 exp(i z) + 1/2 exp(-i z)

	if (isdoublez(p1)) {
		push_double(0.5);
		push(imaginaryunit);
		push(p1);
		multiply();
		expfunc();
		push(imaginaryunit);
		negate();
		push(p1);
		multiply();
		expfunc();
		add();
		multiply();
		return;
	}

	// cos(-x) = cos(x)

	if (isnegativeterm(p1)) {
		push(p1);
		negate();
		cosfunc();
		return;
	}

	if (car(p1) == symbol(ADD)) {
		cosfunc_sum(p1);
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

	// cos(arcsin(x)) = sqrt(1 - x^2)

	if (car(p1) == symbol(ARCSIN)) {
		push_integer(1);
		push(cadr(p1));
		push_integer(2);
		power();
		subtract();
		push_rational(1, 2);
		power();
		return;
	}

	// n pi ?

	push(p1);
	push_symbol(PI);
	divide();
	p2 = pop();

	if (!isnum(p2)) {
		push_symbol(COS);
		push(p1);
		list(2);
		return;
	}

	if (isdouble(p2)) {
		push(p2);
		d = pop_double();
		d = cos(d * M_PI);
		push_double(d);
		return;
	}

	push(p2); // nonnegative by cos(-x) = cos(x) above
	push_integer(180);
	multiply();
	p2 = pop();

	if (!isinteger(p2)) {
		push_symbol(COS);
		push(p1);
		list(2);
		return;
	}

	push(p2);
	push_integer(360);
	modfunc();
	n = pop_integer();

	switch (n) {
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

// cos(x + n/2 pi) = cos(x) cos(n/2 pi) - sin(x) sin(n/2 pi)

void
cosfunc_sum(struct atom *p1)
{
	struct atom *p2, *p3;
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
			cosfunc();
			push(car(p2));
			cosfunc();
			multiply();
			push(p3);
			sinfunc();
			push(car(p2));
			sinfunc();
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

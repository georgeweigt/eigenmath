function
d_scalar_scalar(p1, p2)
{
	// d(x,x)?

	if (equal(p1, p2)) {
		push_integer(1);
		return;
	}

	// d(a,x)?

	if (!iscons(p1)) {
		push_integer(0);
		return;
	}

	if (car(p1) == symbol(ADD)) {
		dsum();
		return;
	}

	if (car(p1) == symbol(MULTIPLY)) {
		dproduct();
		return;
	}

	if (car(p1) == symbol(POWER)) {
		dpower();
		return;
	}

	if (car(p1) == symbol(DERIVATIVE)) {
		dd();
		return;
	}

	if (car(p1) == symbol(LOG)) {
		dlog();
		return;
	}

	if (car(p1) == symbol(SIN)) {
		dsin();
		return;
	}

	if (car(p1) == symbol(COS)) {
		dcos();
		return;
	}

	if (car(p1) == symbol(TAN)) {
		dtan();
		return;
	}

	if (car(p1) == symbol(ARCSIN)) {
		darcsin();
		return;
	}

	if (car(p1) == symbol(ARCCOS)) {
		darccos();
		return;
	}

	if (car(p1) == symbol(ARCTAN)) {
		darctan();
		return;
	}

	if (car(p1) == symbol(SINH)) {
		dsinh();
		return;
	}

	if (car(p1) == symbol(COSH)) {
		dcosh();
		return;
	}

	if (car(p1) == symbol(TANH)) {
		dtanh();
		return;
	}

	if (car(p1) == symbol(ARCSINH)) {
		darcsinh();
		return;
	}

	if (car(p1) == symbol(ARCCOSH)) {
		darccosh();
		return;
	}

	if (car(p1) == symbol(ARCTANH)) {
		darctanh();
		return;
	}

	if (car(p1) == symbol(ABS)) {
		dabs();
		return;
	}

	if (car(p1) == symbol(INTEGRAL) && caddr(p1) == p2) {
		derivative_of_integral();
		return;
	}

	dfunction();
}

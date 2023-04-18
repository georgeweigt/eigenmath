void
eval_derivative(struct atom *p1)
{
	int flag, i, n;
	struct atom *X, *Y = NULL; // silence compiler

	push(cadr(p1));
	evalf();
	p1 = cddr(p1);

	if (!iscons(p1)) {
		push_symbol(X_LOWER);
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
			push_symbol(X_LOWER);
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

void
derivative(void)
{
	struct atom *F, *X;

	X = pop();
	F = pop();

	if (istensor(F)) {
		if (istensor(X))
			d_tensor_tensor(F, X);
		else
			d_tensor_scalar(F, X);
	} else {
		if (istensor(X))
			d_scalar_tensor(F, X);
		else
			d_scalar_scalar(F, X);
	}
}

void
d_scalar_scalar(struct atom *F, struct atom *X)
{
	if (!isusersymbol(X))
		stopf("derivative: symbol expected");

	// d(x,x)?

	if (equal(F, X)) {
		push_integer(1);
		return;
	}

	// d(a,x)?

	if (!iscons(F)) {
		push_integer(0);
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

	if (car(F) == symbol(ERF)) {
		derf(F, X);
		return;
	}

	if (car(F) == symbol(ERFC)) {
		derfc(F, X);
		return;
	}

	if (car(F) == symbol(INTEGRAL) && caddr(F) == X) {
		push(cadr(F));
		return;
	}

	dfunction(F, X);
}

void
dsum(struct atom *p1, struct atom *p2)
{
	int h = tos;
	p1 = cdr(p1);
	while (iscons(p1)) {
		push(car(p1));
		push(p2);
		derivative();
		p1 = cdr(p1);
	}
	add_terms(tos - h);
}

void
dproduct(struct atom *p1, struct atom *p2)
{
	int i, j, n;
	struct atom *p3;
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

void
dpower(struct atom *F, struct atom *X)
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
	logfunc();

	push(caddr(F));		// dv/dx
	push(X);
	derivative();

	multiply();

	add();

	push(F);		// u^v

	multiply();
}

void
dlog(struct atom *p1, struct atom *p2)
{
	push(cadr(p1));
	push(p2);
	derivative();
	push(cadr(p1));
	divide();
}

//	derivative of derivative
//
//	example: d(d(f(x,y),y),x)
//
//	p1 = d(f(x,y),y)
//
//	p2 = x
//
//	cadr(p1) = f(x,y)
//
//	caddr(p1) = y

void
dd(struct atom *p1, struct atom *p2)
{
	struct atom *p3;

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

// derivative of a generic function

void
dfunction(struct atom *p1, struct atom *p2)
{
	struct atom *p3;

	p3 = cdr(p1); // p3 is the argument list for the function

	if (p3 == symbol(NIL) || findf(p3, p2)) {
		push_symbol(DERIVATIVE);
		push(p1);
		push(p2);
		list(3);
	} else
		push_integer(0);
}

void
dsin(struct atom *p1, struct atom *p2)
{
	push(cadr(p1));
	push(p2);
	derivative();
	push(cadr(p1));
	cosfunc();
	multiply();
}

void
dcos(struct atom *p1, struct atom *p2)
{
	push(cadr(p1));
	push(p2);
	derivative();
	push(cadr(p1));
	sinfunc();
	multiply();
	negate();
}

void
dtan(struct atom *p1, struct atom *p2)
{
	push(cadr(p1));
	push(p2);
	derivative();
	push(cadr(p1));
	cosfunc();
	push_integer(-2);
	power();
	multiply();
}

void
darcsin(struct atom *p1, struct atom *p2)
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

void
darccos(struct atom *p1, struct atom *p2)
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

void
darctan(struct atom *p1, struct atom *p2)
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

void
dsinh(struct atom *p1, struct atom *p2)
{
	push(cadr(p1));
	push(p2);
	derivative();
	push(cadr(p1));
	coshfunc();
	multiply();
}

void
dcosh(struct atom *p1, struct atom *p2)
{
	push(cadr(p1));
	push(p2);
	derivative();
	push(cadr(p1));
	sinhfunc();
	multiply();
}

void
dtanh(struct atom *p1, struct atom *p2)
{
	push(cadr(p1));
	push(p2);
	derivative();
	push(cadr(p1));
	coshfunc();
	push_integer(-2);
	power();
	multiply();
}

void
darcsinh(struct atom *p1, struct atom *p2)
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

void
darccosh(struct atom *p1, struct atom *p2)
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

void
darctanh(struct atom *p1, struct atom *p2)
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

void
derf(struct atom *p1, struct atom *p2)
{
	push(cadr(p1));
	push_integer(2);
	power();
	push_integer(-1);
	multiply();
	expfunc();
	push_symbol(PI);
	push_rational(-1, 2);
	power();
	multiply();
	push_integer(2);
	multiply();
	push(cadr(p1));
	push(p2);
	derivative();
	multiply();
}

void
derfc(struct atom *p1, struct atom *p2)
{
	push(cadr(p1));
	push_integer(2);
	power();
	push_integer(-1);
	multiply();
	expfunc();
	push_symbol(PI);
	push_rational(-1,2);
	power();
	multiply();
	push_integer(-2);
	multiply();
	push(cadr(p1));
	push(p2);
	derivative();
	multiply();
}

// gradient of tensor p1 wrt tensor p2

void
d_tensor_tensor(struct atom *p1, struct atom *p2)
{
	int i, j, k, m, n;
	struct atom *p3;

	if (p1->u.tensor->ndim + p2->u.tensor->ndim > MAXDIM)
		stopf("rank exceeds max");

	n = p1->u.tensor->nelem;
	m = p2->u.tensor->nelem;

	p3 = alloc_tensor(n * m);

	for (i = 0; i < n; i++) {
		for (j = 0; j < m; j++) {
			push(p1->u.tensor->elem[i]);
			push(p2->u.tensor->elem[j]);
			derivative();
			p3->u.tensor->elem[m * i + j] = pop();
		}
	}

	// dim info

	p3->u.tensor->ndim = p1->u.tensor->ndim + p2->u.tensor->ndim;

	k = 0;

	n = p1->u.tensor->ndim;

	for (i = 0; i < n; i++)
		p3->u.tensor->dim[k++] = p1->u.tensor->dim[i];

	n = p2->u.tensor->ndim;

	for (i = 0; i < n; i++)
		p3->u.tensor->dim[k++] = p2->u.tensor->dim[i];

	push(p3);
}

// gradient of scalar p1 wrt tensor p2

void
d_scalar_tensor(struct atom *p1, struct atom *p2)
{
	int i, n;
	struct atom *p3;

	p3 = copy_tensor(p2);

	n = p2->u.tensor->nelem;

	for (i = 0; i < n; i++) {
		push(p1);
		push(p2->u.tensor->elem[i]);
		derivative();
		p3->u.tensor->elem[i] = pop();
	}

	push(p3);
}

// derivative of tensor p1 wrt scalar p2

void
d_tensor_scalar(struct atom *p1, struct atom *p2)
{
	int i, n;
	struct atom *p3;

	p3 = copy_tensor(p1);

	n = p1->u.tensor->nelem;

	for (i = 0; i < n; i++) {
		push(p1->u.tensor->elem[i]);
		push(p2);
		derivative();
		p3->u.tensor->elem[i] = pop();
	}

	push(p3);
}

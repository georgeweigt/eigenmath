#include "defs.h"

#undef X
#undef Y

#define X p4
#define Y p5

void
eval_derivative(void)
{
	int flag, i, n;

	push(cadr(p1));
	eval();
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
			eval();
			X = pop();
			p1 = cdr(p1);
		}

		if (isnum(X)) {
			push(X);
			n = pop_integer();
			if (n == ERR)
				stop("derivative");
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
			eval();
			Y = pop();
			p1 = cdr(p1);

			if (isnum(Y)) {
				push(Y);
				n = pop_integer();
				if (n == ERR)
					stop("derivative");
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
	save();
	p2 = pop();
	p1 = pop();
	if (istensor(p1))
		if (istensor(p2))
			d_tensor_tensor();
		else
			d_tensor_scalar();
	else
		if (istensor(p2))
			d_scalar_tensor();
		else
			d_scalar_scalar();
	restore();
}

void
d_scalar_scalar(void)
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

	if (car(p1) == symbol(ERF)) {
		derf();
		return;
	}

	if (car(p1) == symbol(ERFC)) {
		derfc();
		return;
	}

	if (car(p1) == symbol(BESSELJ)) {
		if (iszero(caddr(p1)))
			dbesselj0();
		else
			dbesseljn();
		return;
	}

	if (car(p1) == symbol(BESSELY)) {
		if (iszero(caddr(p1)))
			dbessely0();
		else
			dbesselyn();
		return;
	}

	if (car(p1) == symbol(INTEGRAL) && caddr(p1) == p2) {
		derivative_of_integral();
		return;
	}

	dfunction();
}

void
dsum(void)
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
dproduct(void)
{
	int i, j, n;
	n = length(p1) - 1;
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
dpower(void)
{
	if (isnum(cadr(p1)) && isnum(caddr(p1))) {
		push_integer(0); // irr or imag
		return;
	}

	push(caddr(p1));	// v/u
	push(cadr(p1));
	divide();

	push(cadr(p1));		// du/dx
	push(p2);
	derivative();

	multiply();

	push(cadr(p1));		// log u
	logarithm();

	push(caddr(p1));	// dv/dx
	push(p2);
	derivative();

	multiply();

	add();

	push(p1);		// u^v

	multiply();
}

void
dlog(void)
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
dd(void)
{
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
dfunction(void)
{
	p3 = cdr(p1);	// p3 is the argument list for the function

	if (p3 == symbol(NIL) || find(p3, p2)) {
		push_symbol(DERIVATIVE);
		push(p1);
		push(p2);
		list(3);
	} else
		push_integer(0);
}

void
dsin(void)
{
	push(cadr(p1));
	push(p2);
	derivative();
	push(cadr(p1));
	scos();
	multiply();
}

void
dcos(void)
{
	push(cadr(p1));
	push(p2);
	derivative();
	push(cadr(p1));
	ssin();
	multiply();
	negate();
}

void
dtan(void)
{
	push(cadr(p1));
	push(p2);
	derivative();
	push(cadr(p1));
	scos();
	push_integer(-2);
	power();
	multiply();
}

void
darcsin(void)
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
darccos(void)
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
darctan(void)
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
dsinh(void)
{
	push(cadr(p1));
	push(p2);
	derivative();
	push(cadr(p1));
	scosh();
	multiply();
}

void
dcosh(void)
{
	push(cadr(p1));
	push(p2);
	derivative();
	push(cadr(p1));
	ssinh();
	multiply();
}

void
dtanh(void)
{
	push(cadr(p1));
	push(p2);
	derivative();
	push(cadr(p1));
	scosh();
	push_integer(-2);
	power();
	multiply();
}

void
darcsinh(void)
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
darccosh(void)
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
darctanh(void)
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
dabs(void)
{
	push(cadr(p1));
	push(p2);
	derivative();
	push(cadr(p1));
	sgn();
	multiply();
}

void
derf(void)
{
	push(cadr(p1));
	push_integer(2);
	power();
	push_integer(-1);
	multiply();
	exponential();
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
derfc(void)
{
	push(cadr(p1));
	push_integer(2);
	power();
	push_integer(-1);
	multiply();
	exponential();
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

void
dbesselj0(void)
{
	push(cadr(p1));
	push(p2);
	derivative();
	push(cadr(p1));
	push_integer(1);
	besselj();
	multiply();
	push_integer(-1);
	multiply();
}

void
dbesseljn(void)
{
	push(cadr(p1));
	push(p2);
	derivative();
	push(cadr(p1));
	push(caddr(p1));
	push_integer(-1);
	add();
	besselj();
	push(caddr(p1));
	push_integer(-1);
	multiply();
	push(cadr(p1));
	divide();
	push(cadr(p1));
	push(caddr(p1));
	besselj();
	multiply();
	add();
	multiply();
}


void
dbessely0(void)
{
	push(cadr(p1));
	push(p2);
	derivative();
	push(cadr(p1));
	push_integer(1);
	besselj();
	multiply();
	push_integer(-1);
	multiply();
}


void
dbesselyn(void)
{
	push(cadr(p1));
	push(p2);
	derivative();
	push(cadr(p1));
	push(caddr(p1));
	push_integer(-1);
	add();
	bessely();
	push(caddr(p1));
	push_integer(-1);
	multiply();
	push(cadr(p1));
	divide();
	push(cadr(p1));
	push(caddr(p1));
	bessely();
	multiply();
	add();
	multiply();
}

void
derivative_of_integral(void)
{
	push(cadr(p1));
}

int
iszero(struct atom *p)
{
	int i;
	if (isrational(p))
		return MZERO(p->u.q.a);
	if (isdouble(p))
		return p->u.d == 0.0;
	if (istensor(p)) {
		for (i = 0; i < p->u.tensor->nelem; i++)
			if (!iszero(p->u.tensor->elem[i]))
				return 0;
		return 1;
	}
	return 0;
}

int
isequaln(struct atom *p, int n)
{
	return isequalq(p, n, 1);
}

int
isequalq(struct atom *p, int a, int b)
{
	int sign;
	if (isrational(p)) {
		if (a < 0) {
			sign = MMINUS;
			a = -a;
		} else
			sign = MPLUS;
		return p->sign == sign && MEQUAL(p->u.q.a, a) && MEQUAL(p->u.q.b, b);
	}
	if (isdouble(p))
		return p->u.d == (double) a / b;
	return 0;
}

int
isplusone(struct atom *p)
{
	return isequaln(p, 1);
}

int
isminusone(struct atom *p)
{
	return isequaln(p, -1);
}

int
isinteger(struct atom *p)
{
	return isrational(p) && MEQUAL(p->u.q.b, 1);
}

int
isfraction(struct atom *p)
{
	return isrational(p) && !MEQUAL(p->u.q.b, 1);
}

int
isposint(struct atom *p)
{
	return isinteger(p) && !isnegativenumber(p);
}

int
isexponential(struct atom *p)
{
	return car(p) == symbol(POWER) && cadr(p) == symbol(EXP1);
}

int
isradicalterm(struct atom *p)
{
	return car(p) == symbol(MULTIPLY) && isnum(cadr(p)) && isradical(caddr(p));
}

int
isradical(struct atom *p)
{
	return car(p) == symbol(POWER) && isposint(cadr(p)) && isfraction(caddr(p));
}

int
isnegativeterm(struct atom *p)
{
	return isnegativenumber(p) || (car(p) == symbol(MULTIPLY) && isnegativenumber(cadr(p)));
}

int
isnegativenumber(struct atom *p)
{
	if (isrational(p))
		return p->sign == MMINUS;
	else if (isdouble(p))
		return p->u.d < 0.0;
	else
		return 0;
}

int
isimaginaryterm(struct atom *p)
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

int
isimaginaryfactor(struct atom *p)
{
	return car(p) == symbol(POWER) && isminusone(cadr(p));
}

int
iscomplexnumber(struct atom *p)
{
	return isimaginarynumber(p) || (lengthf(p) == 3 && car(p) == symbol(ADD) && isnum(cadr(p)) && isimaginarynumber(caddr(p)));
}

int
isimaginarynumber(struct atom *p)
{
	return isimaginaryunit(p) || (lengthf(p) == 3 && car(p) == symbol(MULTIPLY) && isnum(cadr(p)) && isimaginaryunit(caddr(p)));
}

int
isimaginaryunit(struct atom *p)
{
	return car(p) == symbol(POWER) && isminusone(cadr(p)) && isequalq(caddr(p), 1, 2);
}

int
isoneoversqrttwo(struct atom *p)
{
	return car(p) == symbol(POWER) && isequaln(cadr(p), 2) && isequalq(caddr(p), -1, 2);
}

int
isminusoneoversqrttwo(struct atom *p)
{
	return lengthf(p) == 3 && car(p) == symbol(MULTIPLY) && isminusone(cadr(p)) && isoneoversqrttwo(caddr(p));
}

// x + y * (-1)^(1/2) where x and y are double?

int
isdoublez(struct atom *p)
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

	if (!isminusone(cadr(p)))
		return 0;

	if (!isequalq(caddr(p), 1, 2))
		return 0;

	return 1;
}

int
isdenominator(struct atom *p)
{
	if (car(p) == symbol(POWER) && isnegativenumber(caddr(p)))
		return 1;
	else if (isrational(p) && !MEQUAL(p->u.q.b, 1))
		return 1;
	else
		return 0;
}

int
isnumerator(struct atom *p)
{
	if (car(p) == symbol(POWER) && isnegativenumber(caddr(p)))
		return 0;
	else if (isrational(p) && MEQUAL(p->u.q.a, 1))
		return 0;
	else
		return 1;
}

int
allnum(struct atom *p)
{
	if (iscons(p)) {
		p = cdr(p);
		while (iscons(p)) {
			if (!allnum(car(p)))
				return 0;
			p = cdr(p);
		}
		return 1;
	}
	return isnum(p) || p == symbol(PI) || p == symbol(EXP1);
}

int
hasdouble(struct atom *p)
{
	if (iscons(p)) {
		p = cdr(p);
		while (iscons(p)) {
			if (hasdouble(car(p)))
				return 1;
			p = cdr(p);
		}
		return 0;
	}
	return isdouble(p);
}

int
isdenormalpolar(struct atom *p)
{
	if (car(p) == symbol(ADD)) {
		p = cdr(p);
		while (iscons(p)) {
			if (isdenormalpolarterm(car(p)))
				return 1;
			p = cdr(p);
		}
		return 0;
	}

	return isdenormalpolarterm(p);
}

// returns 1 if term is (coeff * i * pi) and coeff < 0 or coeff >= 1/2

int
isdenormalpolarterm(struct atom *p)
{
	if (car(p) != symbol(MULTIPLY))
		return 0;

	if (lengthf(p) == 3 && isimaginaryunit(cadr(p)) && caddr(p) == symbol(PI))
		return 1; // exp(i pi)

	if (lengthf(p) != 4 || !isnum(cadr(p)) || !isimaginaryunit(caddr(p)) || cadddr(p) != symbol(PI))
		return 0;

	p = cadr(p); // p = coeff of term

	if (isnegativenumber(p))
		return 1; // p < 0

	push(p);
	push_rational(-1, 2);
	add();
	p = pop();

	if (!isnegativenumber(p))
		return 1; // p >= 1/2

	return 0;
}

int
issquarematrix(struct atom *p)
{
	return istensor(p) && p->u.tensor->ndim == 2 && p->u.tensor->dim[0] == p->u.tensor->dim[1];
}

int
issmallinteger(struct atom *p)
{
	if (isinteger(p))
		return MLENGTH(p->u.q.a) == 1 && p->u.q.a[0] <= 0x7fffffff;

	if (isdouble(p))
		return p->u.d == floor(p->u.d) && fabs(p->u.d) <= 0x7fffffff;

	return 0;
}

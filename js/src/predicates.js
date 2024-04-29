function
iszero(p)
{
	var i;
	if (isrational(p))
		return bignum_iszero(p.a);
	if (isdouble(p))
		return p.d == 0;
	if (istensor(p)) {
		for (i = 0; i < p.elem.length; i++)
			if (!iszero(p.elem[i]))
				return 0;
		return 1;
	}
	return 0;
}

function
isequaln(p, n)
{
	return isequalq(p, n, 1);
}

function
isequalq(p, a, b)
{
	var sign;
	if (isrational(p)) {
		if (a < 0) {
			sign = -1;
			a = -a;
		} else
			sign = 1;
		return p.sign == sign && bignum_equal(p.a, a) && bignum_equal(p.b, b);
	}
	if (isdouble(p))
		return p.d == a / b;
	return 0;
}

function
isplusone(p)
{
	return isequaln(p, 1);
}

function
isminusone(p)
{
	return isequaln(p, -1);
}

function
isinteger(p)
{
	return isrational(p) && bignum_equal(p.b, 1);
}

function
isfraction(p)
{
	return isrational(p) && !bignum_equal(p.b, 1);
}

function
isposint(p)
{
	return isinteger(p) && !isnegativenumber(p);
}

function
isradicalterm(p)
{
	return car(p) == symbol(MULTIPLY) && isnum(cadr(p)) && isradical(caddr(p));
}

function
isradical(p)
{
	return car(p) == symbol(POWER) && isposint(cadr(p)) && isfraction(caddr(p));
}

function
isnegativeterm(p)
{
	return isnegativenumber(p) || (car(p) == symbol(MULTIPLY) && isnegativenumber(cadr(p)));
}

function
isnegativenumber(p)
{
	if (isrational(p))
		return p.sign == -1;
	else if (isdouble(p))
		return p.d < 0.0;
	else
		return 0;
}

function
isimaginaryterm(p)
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

function
isimaginaryfactor(p)
{
	return car(p) == symbol(POWER) && isminusone(cadr(p));
}

function
iscomplexnumber(p)
{
	return isimaginarynumber(p) || (lengthf(p) == 3 && car(p) == symbol(ADD) && isnum(cadr(p)) && isimaginarynumber(caddr(p)));
}

function
isimaginarynumber(p)
{
	return isimaginaryunit(p) || (lengthf(p) == 3 && car(p) == symbol(MULTIPLY) && isnum(cadr(p)) && isimaginaryunit(caddr(p)));
}

function
isimaginaryunit(p)
{
	return car(p) == symbol(POWER) && isminusone(cadr(p)) && isequalq(caddr(p), 1, 2);
}

function
isoneoversqrttwo(p)
{
	return car(p) == symbol(POWER) && isequaln(cadr(p), 2) && isequalq(caddr(p), -1, 2);
}

function
isminusoneoversqrttwo(p)
{
	return lengthf(p) == 3 && car(p) == symbol(MULTIPLY) && isminusone(cadr(p)) && isoneoversqrttwo(caddr(p));
}

// x + y * (-1)^(1/2) where x and y are double?

function
isdoublez(p)
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

function
isdenominator(p)
{
	if (car(p) == symbol(POWER) && isnegativenumber(caddr(p)))
		return 1;
	else if (isrational(p) && !bignum_equal(p.b, 1))
		return 1;
	else
		return 0;
}

function
isnumerator(p)
{
	if (car(p) == symbol(POWER) && isnegativenumber(caddr(p)))
		return 0;
	else if (isrational(p) && bignum_equal(p.a, 1))
		return 0;
	else
		return 1;
}

function
isdoublesomewhere(p)
{
	if (isdouble(p))
		return 1;

	if (iscons(p)) {
		p = cdr(p);
		while (iscons(p)) {
			if (isdoublesomewhere(car(p)))
				return 1;
			p = cdr(p);
		}
	}

	return 0;
}

function
isdenormalpolar(p)
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

function
isdenormalpolarterm(p)
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

function
issquarematrix(p)
{
	return istensor(p) && p.dim.length == 2 && p.dim[0] == p.dim[1];
}

function
issmallinteger(p)
{
	if (isinteger(p))
		return bignum_issmallnum(p.a);

	if (isdouble(p))
		return p.d == Math.floor(p.d) && Math.abs(p.d) <= 0x7fffffff;

	return 0;
}

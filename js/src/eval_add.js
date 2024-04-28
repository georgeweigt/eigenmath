function
eval_add(p1)
{
	var h = stack.length;
	expanding--; // undo expanding++ in evalf
	p1 = cdr(p1);
	while (iscons(p1)) {
		push(car(p1));
		evalf();
		p1 = cdr(p1);
	}
	add_terms(stack.length - h);
	expanding++;
}

function
add()
{
	add_terms(2);
}

function
add_terms(n)
{
	var h, i, p1, T;

	if (n < 2)
		return;

	h = stack.length - n;

	flatten_terms(h);

	T = combine_tensors(h);

	combine_terms(h);

	if (simplify_terms(h))
		combine_terms(h);

	n = stack.length - h;

	if (n == 0) {
		if (istensor(T))
			push(T);
		else
			push_integer(0);
		return;
	}

	if (n > 1) {
		list(n);
		push_symbol(ADD);
		swap();
		cons(); // prepend ADD to list
	}

	if (!istensor(T))
		return;

	p1 = pop();

	T = copy_tensor(T);

	n = T.elem.length;

	for (i = 0; i < n; i++) {
		push(T.elem[i]);
		push(p1);
		add();
		T.elem[i] = pop();
	}

	push(T);
}

function
flatten_terms(h)
{
	var i, n, p1;
	n = stack.length;
	for (i = h; i < n; i++) {
		p1 = stack[i];
		if (car(p1) == symbol(ADD)) {
			stack[i] = cadr(p1);
			p1 = cddr(p1);
			while (iscons(p1)) {
				push(car(p1));
				p1 = cdr(p1);
			}
		}
	}
}

function
combine_tensors(h)
{
	var i, p1, T;
	T = symbol(NIL);
	for (i = h; i < stack.length; i++) {
		p1 = stack[i];
		if (istensor(p1)) {
			if (istensor(T)) {
				push(T);
				push(p1);
				add_tensors();
				T = pop();
			} else
				T = p1;
			stack.splice(i, 1);
			i--; // use same index again
		}
	}
	return T;
}

function
add_tensors()
{
	var i, n, p1, p2;

	p2 = pop();
	p1 = pop();

	if (!compatible_dimensions(p1, p2))
		stopf("incompatible tensor arithmetic");

	p1 = copy_tensor(p1);

	n = p1.elem.length;

	for (i = 0; i < n; i++) {
		push(p1.elem[i]);
		push(p2.elem[i]);
		add();
		p1.elem[i] = pop();
	}

	push(p1);
}

function
combine_terms(h)
{
	var i;
	sort_terms(h);
	for (i = h; i < stack.length; i++) {
		if (iszero(stack[i])) {
			stack.splice(i, 1); // remove
			i--; // use same index again
			continue;
		}
		if (i + 1 < stack.length && combine_terms_nib(i)) {
			if (iszero(stack[i]))
				stack.splice(i, 2); // remove 2 terms
			else
				stack.splice(i + 1, 1); // remove 1 term
			i--; // use same index again
		}
	}
}

function
combine_terms_nib(i)
{
	var coeff1, coeff2, denorm, p1, p2;

	p1 = stack[i];
	p2 = stack[i + 1];

	if (iszero(p2))
		return 1;

	if (isnum(p1) && isnum(p2)) {
		add_numbers(p1, p2);
		stack[i] = pop();
		return 1;
	}

	if (isnum(p1) || isnum(p2))
		return 0; // cannot add number and something else

	coeff1 = one;
	coeff2 = one;

	denorm = 0;

	if (car(p1) == symbol(MULTIPLY)) {
		p1 = cdr(p1);
		denorm = 1;
		if (isnum(car(p1))) {
			coeff1 = car(p1);
			p1 = cdr(p1);
			if (cdr(p1) == symbol(NIL)) {
				p1 = car(p1);
				denorm = 0;
			}
		}
	}

	if (car(p2) == symbol(MULTIPLY)) {
		p2 = cdr(p2);
		if (isnum(car(p2))) {
			coeff2 = car(p2);
			p2 = cdr(p2);
			if (cdr(p2) == symbol(NIL))
				p2 = car(p2);
		}
	}

	if (!equal(p1, p2))
		return 0;

	add_numbers(coeff1, coeff2);

	coeff1 = pop();

	if (iszero(coeff1)) {
		stack[i] = coeff1;
		return 1;
	}

	if (isplusone(coeff1) && !isdouble(coeff1)) {
		if (denorm) {
			push_symbol(MULTIPLY);
			push(p1); // p1 is a list, not an atom
			cons(); // prepend MULTIPLY
		} else
			push(p1);
	} else {
		if (denorm) {
			push_symbol(MULTIPLY);
			push(coeff1);
			push(p1); // p1 is a list, not an atom
			cons(); // prepend coeff1
			cons(); // prepend MULTIPLY
		} else {
			push_symbol(MULTIPLY);
			push(coeff1);
			push(p1);
			list(3);
		}
	}

	stack[i] = pop();

	return 1;
}

function
sort_terms(h)
{
	var t = stack.splice(h).sort(cmp_terms);
	stack = stack.concat(t);
}

function
cmp_terms(p1, p2)
{
	var a, b, c;

	// 1st level: imaginary terms on the right

	a = isimaginaryterm(p1);
	b = isimaginaryterm(p2);

	if (a == 0 && b == 1)
		return -1; // ok

	if (a == 1 && b == 0)
		return 1; // out of order

	// 2nd level: numericals on the right

	if (isnum(p1) && isnum(p2))
		return 0; // don't care about order, save time, don't compare

	if (isnum(p1))
		return 1; // out of order

	if (isnum(p2))
		return -1; // ok

	// 3rd level: sort by factors

	a = 0;
	b = 0;

	if (car(p1) == symbol(MULTIPLY)) {
		p1 = cdr(p1);
		a = 1; // p1 is a list of factors
		if (isnum(car(p1))) {
			// skip over coeff
			p1 = cdr(p1);
			if (cdr(p1) == symbol(NIL)) {
				p1 = car(p1);
				a = 0;
			}
		}
	}

	if (car(p2) == symbol(MULTIPLY)) {
		p2 = cdr(p2);
		b = 1; // p2 is a list of factors
		if (isnum(car(p2))) {
			// skip over coeff
			p2 = cdr(p2);
			if (cdr(p2) == symbol(NIL)) {
				p2 = car(p2);
				b = 0;
			}
		}
	}

	if (a == 0 && b == 0)
		return cmp_factors(p1, p2);

	if (a == 0 && b == 1) {
		c = cmp_factors(p1, car(p2));
		if (c == 0)
			c = -1; // lengthf(p1) < lengthf(p2)
		return c;
	}

	if (a == 1 && b == 0) {
		c = cmp_factors(car(p1), p2);
		if (c == 0)
			c = 1; // lengthf(p1) > lengthf(p2)
		return c;
	}

	while (iscons(p1) && iscons(p2)) {
		c = cmp_factors(car(p1), car(p2));
		if (c)
			return c;
		p1 = cdr(p1);
		p2 = cdr(p2);
	}

	if (iscons(p1))
		return 1; // lengthf(p1) > lengthf(p2)

	if (iscons(p2))
		return -1; // lengthf(p1) < lengthf(p2)

	return 0;
}

function
simplify_terms(h)
{
	var i, n = 0, p1, p2;
	for (i = h; i < stack.length; i++) {
		p1 = stack[i];
		if (isradicalterm(p1)) {
			push(p1);
			evalf();
			p2 = pop();
			if (!equal(p1, p2)) {
				stack[i] = p2;
				n++;
			}
		}
	}
	return n;
}

function
isradicalterm(p)
{
	return car(p) == symbol(MULTIPLY) && isnum(cadr(p)) && isradical(caddr(p));
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
add_numbers(p1, p2)
{
	var a, b;

	if (isrational(p1) && isrational(p2)) {
		add_rationals(p1, p2);
		return;
	}

	push(p1);
	a = pop_double();

	push(p2);
	b = pop_double();

	push_double(a + b);
}

function
add_rationals(p1, p2)
{
	var a, ab, b, ba, d, sign;

	if (iszero(p1)) {
		push(p2);
		return;
	}

	if (iszero(p2)) {
		push(p1);
		return;
	}

	if (isinteger(p1) && isinteger(p2)) {
		add_integers(p1, p2);
		return;
	}

	ab = bignum_mul(p1.a, p2.b);
	ba = bignum_mul(p1.b, p2.a);

	if (p1.sign == p2.sign) {
		a = bignum_add(ab, ba);
		sign = p1.sign;
	} else {
		switch (bignum_cmp(ab, ba)) {
		case 1:
			a = bignum_sub(ab, ba);
			sign = p1.sign;
			break;
		case 0:
			push_integer(0);
			return;
		case -1:
			a = bignum_sub(ba, ab);
			sign = p2.sign;
			break;
		}
	}

	b = bignum_mul(p1.b, p2.b);

	d = bignum_gcd(a, b);

	a = bignum_div(a, d);
	b = bignum_div(b, d);

	push_bignum(sign, a, b);
}

function
add_integers(p1, p2)
{
	var c, sign;
	if (p1.sign == p2.sign) {
		c = bignum_add(p1.a, p2.a);
		sign = p1.sign;
	} else {
		switch (bignum_cmp(p1.a, p2.a)) {
		case 1:
			c = bignum_sub(p1.a, p2.a);
			sign = p1.sign;
			break;
		case 0:
			push_integer(0);
			return;
		case -1:
			c = bignum_sub(p2.a, p1.a);
			sign = p2.sign;
			break;
		default:
			stopf("error");
		}
	}
	push_bignum(sign, c, bignum_int(1));
}

function
subtract()
{
	negate();
	add();
}

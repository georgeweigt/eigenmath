function
eval_simplify(p1)
{
	push(cadr(p1));
	evalf();
	simplify();
}

function
simplify()
{
	var i, n;
	var p1, p2;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1.elem.length;
		for (i = 0; i < n; i++) {
			push(p1.elem[i]);
			simplify();
			p1.elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (!iscons(p1)) {
		push(p1);
		return;
	}

	push(p1);
	numerator();
	p2 = pop();
	if (iszero(p2)) {
		push_integer(0);
		return;
	}

	push(p1);
	simplify_trig(); // do this first otherwise compton-demo runs out of memory
	p1 = pop();

	if (!iscons(p1)) {
		push(p1);
		return;
	}

	push(p1);
	numerator();
	p2 = pop();
	if (iszero(p2)) {
		push_integer(0);
		return;
	}

	push(p1);
	simplify_nib();
	p1 = pop();

	if (!iscons(p1)) {
		push(p1);
		return;
	}

	push(p1);
	numerator();
	p2 = pop();
	if (iszero(p2)) {
		push_integer(0);
		return;
	}

	if (car(p2) == symbol(ADD) && lengthf(p2) < 10) {
		push(p2);
		push(p2);
		multiply();
		p2 = pop();
		if (iszero(p2)) {
			push_integer(0);
			return;
		}
	}

	push(p1);
}

function
simplify_nib()
{
	var h;
	var p1, p2, p3, NUM, DEN, R;

	p1 = pop();

	if (!iscons(p1)) {
		push(p1);
		return;
	}

	push(p1);
	rect();
	p2 = pop();
	push(p1);
	polar();
	p3 = pop();
	if (simpler(p3, p2))
		p2 = p3;
	if (simpler(p2, p1))
		p1 = p2;

	if (!iscons(p1)) {
		push(p1);
		return;
	}

	push(p1);
	expform();
	rect();
	p2 = pop();
	if (simpler(p2, p1))
		p1 = p2;

	if (!iscons(p1)) {
		push(p1);
		return;
	}

	// depth first

	h = stack.length;
	push(car(p1)); // function name
	p1 = cdr(p1);
	while (iscons(p1)) {
		push(car(p1));
		simplify_nib();
		p1 = cdr(p1);
	}
	list(stack.length - h);
	evalf(); // normalize
	p1 = pop();

	if (!iscons(p1)) {
		push(p1);
		return;
	}

	push(p1);
	numden();
	NUM = pop();
	DEN = pop();

	// NUM / DEN = A / (B / C) = A C / B

	// for example, 1 / (x + y^2 / x) -> x / (x^2 + y^2)

	push(DEN);
	numden();
	DEN = pop();
	push(NUM);
	multiply();
	NUM = pop();

	// search for R such that NUM = R DEN

	if (car(NUM) == symbol(ADD) && car(DEN) == symbol(ADD)) {

		p2 = cdr(DEN);

		while (iscons(p2)) {

			// provisional ratio

			push(cadr(NUM)); // 1st term of numerator
			push(car(p2));
			divide();
			R = pop();

			// check

			push(NUM);
			push(R);
			push(DEN);
			multiply();
			subtract();
			p3 = pop();

			if (iszero(p3)) {
				push(R);
				return;
			}

			p2 = cdr(p2);
		}
	}

	push(NUM);
	push(DEN);
	divide();
	p2 = pop();
	if (simpler(p2, p1)) {
		push(p2);
		return;
	}

	push(DEN);
	push(NUM);
	divide();
	rationalize();
	reciprocate();
	p2 = pop();
	if (simpler(p2, p1)) {
		push(p2);
		return;
	}

	push(p1);
}

// try exponential form

function
simplify_trig()
{
	var p1, p2;

	p1 = pop();

	if (!iscons(p1)) {
		push(p1);
		return;
	}

	push(p1);
	expform();
	numden();
	swap();
	divide();
	p2 = pop();

	if (simpler(p2, p1))
		push(p2);
	else
		push(p1);
}

function
simpler(p1, p2)
{
	var d1, d2;

	d1 = diameter(p1);
	d2 = diameter(p2);

	if (d1 == d2) {
		d1 = mass(p1);
		d2 = mass(p2);
	}

	return d1 < d2;
}

// for example, 1 / (x + y^2 / x) has diameter of 2

function
diameter(p)
{
	var max = 0, n;

	if (car(p) == symbol(POWER) && isnegativenumber(caddr(p)))
		return 1 + diameter(cadr(p));

	while (iscons(p)) {
		n = diameter(car(p));
		if (n > max)
			max = n;
		p = cdr(p);
	}

	return max;
}

function
mass(p)
{
	var n = 1;
	while (iscons(p)) {
		n += mass(car(p));
		p = cdr(p);
	}
	return n;
}

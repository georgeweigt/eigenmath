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
	var i, n, p1, p2;

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

	// mixed complex forms

	push(p1);
	polar();
	p2 = pop();
	if (iszero(p2)) {
		push_integer(0);
		return;
	}

	push(p1);
	simplify_trig();
	simplify_sum();
}

function
simplify_sum()
{
	var h, p1, p2, p3, NUM, DEN, R;

	p1 = pop();

	if (!iscons(p1)) {
		push(p1);
		return;
	}

	// recursive first

	h = stack.length;
	push(car(p1)); // function name
	p1 = cdr(p1);
	while (iscons(p1)) {
		push(car(p1));
		simplify_sum();
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

	if (car(DEN) != symbol(ADD)) {
		push(NUM);
		push(DEN);
		divide();
		p2 = pop();
		simpler(p1, p2);
		return;
	}

	// NUM / DEN = A / (B / C) = A C / B

	// for example, 1 / (x + y^2 / x) -> x / (x^2 + y^2)

	push(DEN);
	numden();
	DEN = pop();
	push(NUM);
	multiply();
	NUM = pop();

	// are NUM and DEN congruent sums?

	if (car(NUM) != symbol(ADD) || car(DEN) != symbol(ADD) || lengthf(NUM) != lengthf(DEN)) {
		push(NUM);
		push(DEN);
		divide();
		p2 = pop();
		simpler(p1, p2);
		return;
	}

	// search for R such that NUM = R DEN

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

	push(NUM);
	push(DEN);
	divide();
	p2 = pop();

	simpler(p1, p2);
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
	circexp();
	numden();
	swap();
	divide();
	p2 = pop();

	simpler(p1, p2);
}

function
simpler(p1, p2)
{
	var n1, n2;

	n1 = powdep(p1);
	n2 = powdep(p2);

	if (n1 == n2) {
		if (complexity(p1) <= complexity(p2))
			push(p1);
		else
			push(p2);
		return;
	}

	if (n1 < n2)
		push(p1);
	else
		push(p2);
}

// for example, 1 / (x + y^2 / x) has powdep of 2

function
powdep(p)
{
	var max = 0, n;

	if (car(p) == symbol(POWER) && isnegativenumber(caddr(p)))
		return 1 + powdep(cadr(p));

	while (iscons(p)) {
		n = powdep(car(p));
		if (n > max)
			max = n;
		p = cdr(p);
	}

	return max;
}

function
complexity(p)
{
	var n = 1;
	while (iscons(p)) {
		n += complexity(car(p));
		p = cdr(p);
	}
	return n;
}

void
eval_simplify(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	simplify();
}

void
simplify(void)
{
	int h, i, n;
	struct atom *p1, *p2;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			simplify();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	// already simple?

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

	// simplify depth first

	h = tos;
	push(car(p1));
	p1 = cdr(p1);
	while (iscons(p1)) {
		push(car(p1));
		simplify();
		p1 = cdr(p1);
	}
	list(tos - h);
	evalf();

	simplify_pass1();
	simplify_pass2(); // try exponential form
}

void
simplify_pass1(void)
{
	struct atom *p1, *NUM, *DEN, *R, *T;

	p1 = pop();

	// already simple?

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

	if (car(DEN) == symbol(ADD)) {
		push(DEN);
		rationalize(); // exp(i x) + exp(-i x) -> (exp(2 i x) + 1) / exp(i x)
		numden();
		DEN = pop();
		push(NUM);
		multiply();
		NUM = pop();
	}

	// are NUM and DEN congruent sums?

	if (car(NUM) != symbol(ADD) || car(DEN) != symbol(ADD) || lengthf(NUM) != lengthf(DEN)) {
		// no, but NUM over DEN might be simpler than p1
		push(NUM);
		push(DEN);
		divide();
		T = pop();
		if (divdepth(T) < divdepth(p1) || complexity(T) < complexity(p1))
			push(T);
		else
			push(p1);
		return;
	}

	push(cadr(NUM)); // push first term of numerator
	push(cadr(DEN)); // push first term of denominator
	divide();

	R = pop(); // provisional ratio

	push(R);
	push(DEN);
	multiply();

	push(NUM);
	subtract();

	T = pop();

	if (iszero(T))
		p1 = R;

	push(p1);
}

// try exponential form

void
simplify_pass2(void)
{
	struct atom *p1, *p2;

	p1 = pop();

	// already simple?

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

	if (divdepth(p1) <= divdepth(p2) && complexity(p1) <= complexity(p2))
		push(p1);
	else
		push(p2);
}

int
complexity(struct atom *p)
{
	int n = 1;
	while (iscons(p)) {
		n += complexity(car(p));
		p = cdr(p);
	}
	return n;
}

// for example, 1 / (x + y^2 / x) has depth of 2

int
divdepth(struct atom *p)
{
	int max = 0, n;

	if (car(p) == symbol(POWER) && isnegativenumber(caddr(p)))
		return divdepth(cadr(p)) + 1;

	while (iscons(p)) {
		n = divdepth(car(p));
		if (n > max)
			max = n;
		p = cdr(p);
	}

	return max;
}

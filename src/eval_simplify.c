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
	struct atom *p1, *p2, *NUM, *DEN, *R;

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

	if (car(DEN) != symbol(ADD)) {
		push(NUM);
		push(DEN);
		divide();
		p2 = pop();
		if (simpler(p1, p2))
			push(p1);
		else
			push(p2);
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
		if (simpler(p1, p2))
			push(p1);
		else
			push(p2);
		return;
	}

	// provisional ratio

	push(cadr(NUM)); // push first term of numerator
	push(cadr(DEN)); // push first term of denominator
	divide();
	R = pop();

	// check

	push(R);
	push(DEN);
	multiply();
	push(NUM);
	subtract();
	p2 = pop();

	if (iszero(p2)) {
		push(R);
		return;
	}

	push(NUM);
	push(DEN);
	divide();
	p2 = pop();

	if (simpler(p1, p2))
		push(p1);
	else
		push(p2);
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

	if (simpler(p1, p2))
		push(p1);
	else
		push(p2);
}

int
simpler(struct atom *p1, struct atom *p2)
{
	return divd(p1) <= divd(p2) && complexity(p1) <= complexity(p2);
}

// for example, 1 / (x + y^2 / x) has divd of 2

int
divd(struct atom *p)
{
	int max = 0, n;

	if (car(p) == symbol(POWER) && isnegativenumber(caddr(p)))
		return 1 + divd(cadr(p));

	while (iscons(p)) {
		n = divd(car(p));
		if (n > max)
			max = n;
		p = cdr(p);
	}

	return max;
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

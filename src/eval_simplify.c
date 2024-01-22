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
	struct atom *p1, *p2, *p3, *NUM, *DEN, *R;

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

	simpler(p1, p2);
}

void
simpler(struct atom *p1, struct atom *p2)
{
	int n1, n2;

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

int
powdep(struct atom *p)
{
	int max = 0, n;

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

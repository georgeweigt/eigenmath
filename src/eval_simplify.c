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
	int i, n;
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

void
simplify_nib(void)
{
	int h;
	struct atom *p1, *p2, *p3, *NUM, *DEN, *R;

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
	expform(0); // 0 means don't change arc functions
	rect();
	p2 = pop();
	if (simpler(p2, p1))
		p1 = p2;

	if (!iscons(p1)) {
		push(p1);
		return;
	}

	// depth first

	h = tos;
	push(car(p1)); // function name
	p1 = cdr(p1);
	while (iscons(p1)) {
		push(car(p1));
		simplify_nib();
		p1 = cdr(p1);
	}
	list(tos - h);
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

void
simplify_trig(void)
{
	struct atom *p1, *p2;

	p1 = pop();

	if (!iscons(p1)) {
		push(p1);
		return;
	}

	push(p1);
	expform(0); // 0 means don't change arc functions
	numden();
	swap();
	divide();
	p2 = pop();

	if (simpler(p2, p1))
		push(p2);
	else
		push(p1);
}

int
simpler(struct atom *p1, struct atom *p2)
{
	int d1, d2;

	d1 = diameter(p1);
	d2 = diameter(p2);

	if (d1 == d2) {
		d1 = mass(p1);
		d2 = mass(p2);
	}

	return d1 < d2;
}

// for example, 1 / (x + y^2 / x) has diameter of 2

int
diameter(struct atom *p)
{
	int max = 0, n;

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

int
mass(struct atom *p)
{
	int n = 1;
	while (iscons(p)) {
		n += mass(car(p));
		p = cdr(p);
	}
	return n;
}

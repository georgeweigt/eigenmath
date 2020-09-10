function
simplify()
{
	var h, i, n, p1;

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

	if (car(p1) == symbol(ADD)) {
		h = stack.length;
		p1 = cdr(p1);
		while (iscons(p1)) {
			push(car(p1));
			simplify_expr();
			p1 = cdr(p1);
		}
		add_terms(stack.length - h);
		p1 = pop();
		if (car(p1) == symbol(ADD)) {
			push(p1);
			simplify_expr(); // try rationalizing
			p1 = pop();
		}
		push(p1);
		if (find(p1, symbol(SIN)) || find(p1, symbol(COS)) || find(p1, symbol(TAN))
		|| find(p1, symbol(SINH)) || find(p1, symbol(COSH)) || find(p1, symbol(TANH)))
			simplify_trig();
		return;
	}

	// p1 is a term (factor or product of factors)

	push(p1);
	simplify_expr();
	p1 = pop();
	push(p1);

	if (find(p1, symbol(SIN)) || find(p1, symbol(COS)) || find(p1, symbol(TAN))
	|| find(p1, symbol(SINH)) || find(p1, symbol(COSH)) || find(p1, symbol(TANH)))
		simplify_trig();
}

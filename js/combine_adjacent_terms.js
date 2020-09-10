function
combine_adjacent_terms(i)
{
	var state, p1, p2, p3, p4;

	p1 = stack[i];
	p2 = stack[i + 1];

	if (istensor(p1) && istensor(p2)) {
		add_tensors(p1, p2);
		stack[i] = pop();
		return 1;
	}

	if (istensor(p1) || istensor(p2))
		stop("incompatible tensor arithmetic");

	if (isnum(p1) && isnum(p2)) {
		add_numbers(p1, p2);
		stack[i] = pop();
		return 1;
	}

	if (isnum(p1) || isnum(p2))
		return 0; // cannot add number and something else

	p3 = p1;
	p4 = p2;

	p1 = one; // coeff of 1st term
	p2 = one; // coeff of 2nd term

	state = 0;

	if (car(p3) == symbol(MULTIPLY)) {
		p3 = cdr(p3);
		state = 1; // p3 is now an improper expr
		if (isnum(car(p3))) {
			p1 = car(p3);
			p3 = cdr(p3);
			if (cdr(p3) == symbol(NIL)) {
				p3 = car(p3);
				state = 0; // p3 is now a proper expr
			}
		}
	}

	if (car(p4) == symbol(MULTIPLY)) {
		p4 = cdr(p4);
		if (isnum(car(p4))) {
			p2 = car(p4);
			p4 = cdr(p4);
			if (cdr(p4) == symbol(NIL))
				p4 = car(p4);
		}
	}

	if (!equal(p3, p4))
		return 0; // terms differ, cannot add

	add_numbers(p1, p2); // add coeffs p1 and p2

	if (state) {
		push_symbol(MULTIPLY);
		push(p3);
		cons();
	} else
		push(p3);

	multiply();

	stack[i] = pop();

	return 1;
}

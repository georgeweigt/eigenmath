function
eval_integral(p1)
{
	var i, n, p2;

	expanding++;

	// 1st arg

	p1 = cdr(p1);
	push(car(p1));
	evalf();

	// check for single arg

	p1 = cdr(p1);

	if (!iscons(p1)) {
		guess();
		integral();
		expanding--;
		return;
	}

	while (iscons(p1)) {

		// next arg should be a symbol

		push(car(p1)); // have to eval in case of $METAX
		evalf();
		p2 = pop();

		if (!isusersymbol(p2))
			stop("symbol expected");

		p1 = cdr(p1);

		// if next arg is a number then use it

		n = 1;

		if (isnum(car(p1))) {
			push(car(p1));
			n = pop_integer();
			if (n < 1)
				stop("nth integral: check n");
			p1 = cdr(p1);
		}

		for (i = 0; i < n; i++) {
			push(p2);
			integral();
		}
	}

	expanding--;
}

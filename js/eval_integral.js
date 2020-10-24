function
eval_integral(p1)
{
	var flag, i, n, X, Y;

	expanding++;

	push(cadr(p1));
	evalf();
	p1 = cddr(p1);

	if (!iscons(p1)) {
		push_symbol(SYMBOL_X);
		integral();
		expanding--;
		return;
	}

	flag = 0;

	while (iscons(p1) || flag) {

		if (flag) {
			X = Y;
			flag = 0;
		} else {
			push(car(p1));
			evalf();
			X = pop();
			p1 = cdr(p1);
		}

		if (isnum(X)) {
			push(X);
			n = pop_integer();
			push_symbol(SYMBOL_X);
			X = pop();
			for (i = 0; i < n; i++) {
				push(X);
				integral();
			}
			continue;
		}

		if (iscons(p1)) {

			push(car(p1));
			evalf();
			Y = pop();
			p1 = cdr(p1);

			if (isnum(Y)) {
				push(Y);
				n = pop_integer();
				for (i = 0; i < n; i++) {
					push(X);
					integral();
				}
				continue;
			}

			flag = 1;
		}

		push(X);
		integral();
	}

	expanding--;
}

function
eval_derivative(p1)
{
	var i, n, state, X, Y;

	push(cadr(p1));
	evalf();
	p1 = cddr(p1);

	if (!iscons(p1)) {
		guess();
		derivative();
		return;
	}

	state = 0;

	while (iscons(p1) || state == 1) {

		if (state == 1) {
			X = Y;
			state = 0;
		} else {
			push(car(p1));
			evalf();
			X = pop();
			p1 = cdr(p1);
		}

		if (isnum(X)) {
			push(X);
			n = pop_integer();
			guess();
			X = pop();
			for (i = 0; i < n; i++) {
				push(X);
				derivative();
			}
			continue;
		}

		if (!isusersymbol(X))
			stop("symbol expected");

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
					derivative();
				}
				continue;
			}

			state = 1;
		}

		push(X);
		derivative();
	}
}

// examples
//
// d(f)
// d(f,2)
// d(f,x)
// d(f,x,2)
// d(f,x,y)

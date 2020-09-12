function
eval_derivative(p1)
{
	var i, n, p2, state, N, X;

	push(cadr(p1));
	evalf();
	p1 = cddr(p1);

	if (!iscons(p1)) {
		guess();
		derivative();
		return;
	}

	state = 0;

	while (iscons(p1)) {

		if (state == 0) {
			push(car(p1));
			evalf();
			X = pop();
			p1 = cdr(p1);
		} else
			state = 0;

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

		push(car(p1));
		evalf();
		N = pop();
		p1 = cdr(p1);

		if (isnum(N)) {
			push(N);
			n = pop_integer();
			for (i = 0; i < n; i++) {
				push(X);
				derivative();
			}
			continue;
		}

		push(X);
		derivative();

		X = N;
		state = 1;
	}
}

// examples
//
// d(f)
// d(f,2)
// d(f,x)
// d(f,x,2)
// d(f,x,y)

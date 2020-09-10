function
eval_derivative(p1)
{
	var i, n, p2, F, N, X;

	// evaluate 1st arg to get function F

	p1 = cdr(p1);
	push(car(p1));
	evalf();

	// evaluate 2nd arg and then...

	// example	result of 2nd arg	what to do
	//
	// d(f)		nil			guess X, N = nil
	// d(f,2)	2			guess X, N = 2
	// d(f,x)	x			X = x, N = nil
	// d(f,x,2)	x			X = x, N = 2
	// d(f,x,y)	x			X = x, N = y

	p1 = cdr(p1);
	push(car(p1));
	evalf();

	p2 = pop();
	if (p2 == symbol(NIL)) {
		guess();
		push_symbol(NIL);
	} else if (isnum(p2)) {
		guess();
		push(p2);
	} else {
		push(p2);
		p1 = cdr(p1);
		push(car(p1));
		evalf();
	}

	N = pop();
	X = pop();
	F = pop();

	for (;;) {

		// N might be a symbol instead of a number

		if (isnum(N)) {
			push(N);
			n = pop_integer();
			if (n == ERR)
				stop("nth derivative: check n");
		} else
			n = 1;

		push(F);

		if (n >= 0) {
			for (i = 0; i < n; i++) {
				push(X);
				derivative();
			}
		} else {
			n = -n;
			for (i = 0; i < n; i++) {
				push(X);
				integral();
			}
		}

		F = pop();

		// if N is nil then arglist is exhausted

		if (N == symbol(NIL))
			break;

		// otherwise...

		// N		arg1		what to do
		//
		// number	nil		break
		// number	number		N = arg1, continue
		// number	symbol		X = arg1, N = arg2, continue
		//
		// symbol	nil		X = N, N = nil, continue
		// symbol	number		X = N, N = arg1, continue
		// symbol	symbol		X = N, N = arg1, continue

		if (isnum(N)) {
			p1 = cdr(p1);
			push(car(p1));
			evalf();
			N = pop();
			if (N == symbol(NIL))
				break;		// arglist exhausted
			if (isnum(N))
				;		// N = arg1
			else {
				X = N;		// X = arg1
				p1 = cdr(p1);
				push(car(p1));
				evalf();
				N = pop();	// N = arg2
			}
		} else {
			X = N;			// X = N
			p1 = cdr(p1);
			push(car(p1));
			evalf();
			N = pop();		// N = arg1
		}
	}

	push(F); // final result
}

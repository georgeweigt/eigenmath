function
arctan()
{
	var i, n, X, Y, Z;

	X = pop();
	Y = pop();

	if (isnum(X) && isnum(Y)) {
		arctan_numbers(X, Y);
		return;
	}

	if (istensor(Y)) {
		Y = copy_tensor(Y);
		n = Y.elem.length;
		for (i = 0; i < n; i++) {
			push(Y.elem[i]);
			push(X);
			arctan();
			Y.elem[i] = pop();
		}
		push(Y);
		return;
	}

	// arctan(z) = -1/2 i log((i - z) / (i + z))

	if (!iszero(X) && (isdoublez(X) || isdoublez(Y))) {
		push(Y);
		push(X);
		divide();
		Z = pop();
		push_double(-0.5);
		push(imaginaryunit);
		multiply();
		push(imaginaryunit);
		push(Z);
		subtract();
		push(imaginaryunit);
		push(Z);
		add();
		divide();
		log();
		multiply();
		return;
	}

	// arctan(-y,x) = -arctan(y,x)

	if (isnegativeterm(Y)) {
		push(Y);
		negate();
		push(X);
		arctan();
		negate();
		return;
	}

	if (car(Y) == symbol(TAN) && isplusone(X)) {
		push(cadr(Y)); // x of tan(x)
		return;
	}

	push_symbol(ARCTAN);
	push(Y);
	push(X);
	list(3);
}

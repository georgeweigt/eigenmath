function
power_complex_plus(X, Y, n)
{
	var i, PX, PY;

	PX = X;
	PY = Y;

	for (i = 1; i < n; i++) {

		push(PX);
		push(X);
		multiply();
		push(PY);
		push(Y);
		multiply();
		subtract();

		push(PX);
		push(Y);
		multiply();
		push(PY);
		push(X);
		multiply();
		add();

		PY = pop();
		PX = pop();
	}

	// X + i Y

	push(PX);
	push(imaginaryunit);
	push(PY);
	multiply();
	add();
}

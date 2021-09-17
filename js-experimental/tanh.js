function
tanh()
{
	var p1 = pop();

	if (isdouble(p1) || isdoublez(p1)) {
		push(p1);
		sinh();
		push(p1);
		cosh();
		divide();
		return;
	}

	if (iszero(p1)) {
		push_integer(0);
		return;
	}

	// tanh(-x) = -tanh(x)

	if (isnegative(p1)) {
		push(p1);
		negate();
		tanh();
		negate();
		return;
	}

	if (car(p1) == symbol(ARCTANH)) {
		push(cadr(p1));
		return;
	}

	push_symbol(TANH);
	push(p1);
	list(2);
}

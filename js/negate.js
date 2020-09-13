function
negate()
{
	var p1 = pop();

	if (isrational(p1)) {
		push_rational(-p1.a, p1.b);
		return;
	}

	if (isdouble(p1)) {
		push_double(-p1.d);
		return;
	}

	push_integer(-1);
	push(p1);
	multiply();
}

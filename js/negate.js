function
negate()
{
	if (isnum(stack[stack.length - 1])) {
		var p1 = pop();
		if (isrational(p1))
			push_rational(-p1.a, p1.b);
		else
			push_double(-p.d);
		return;
	}

	push_integer(-1);
	multiply();
}

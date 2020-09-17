function
cmp_args(p1)
{
	var p2;

	p1 = cdr(p1);
	push(car(p1));
	evalf();

	p1 = cdr(p1);
	push(car(p1));
	evalf();

	p2 = pop();
	p1 = pop();

	if (istensor(p1) || istensor(p2))
		stopf("tensor comparison");

	push(p1);
	push(p2);
	subtract();
	p1 = pop();

	if (!isnum(p1)) {
		push(p1);
		floatf(); // try converting pi and e
		p1 = pop();
		if (!isnum(p1))
			stopf("non-numerical comparison");
	}

	if (iszero(p1))
		return 0;

	if (isrational(p1)) {
		if (p1.a > 0)
			return 1;
		else
			return -1;
	}

	if (p1.d > 0)
		return 1;
	else
		return -1;
}

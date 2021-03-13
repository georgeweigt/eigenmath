function
abs()
{
	var p1;

	p1 = pop();

	if (istensor(p1)) {
		if (p1.dim.length != 1)
			stopf("abs");
		push(p1);
		push(p1);
		conj();
		inner();
		push_rational(1, 2);
		power();
		return;
	}

	push(p1);
	push(p1);
	conj();
	multiply();
	push_rational(1, 2);
	power();
}

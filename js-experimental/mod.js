function
mod()
{
	var d1, d2, p1, p2;

	p2 = pop();
	p1 = pop();

	if (!isnum(p1) || !isnum(p2) || iszero(p2)) {
		push_symbol(MOD);
		push(p1);
		push(p2);
		list(3);
		return;
	}

	if (isrational(p1) && isrational(p2)) {
		push(p1);
		push(p1);
		push(p2);
		divide();
		abs();
		floor();
		push(p2);
		multiply();
		if (p1.sign == p2.sign)
			negate(); // p1 and p2 have same sign
		add();
		return;
	}

	push(p1);
	d1 = pop_double();

	push(p2);
	d2 = pop_double();

	push_double(d1 % d2);
}

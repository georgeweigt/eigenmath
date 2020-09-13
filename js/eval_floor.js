function
eval_floor(p1)
{
	push(cadr(p1));
	evalf();
	p1 = pop();

	if (isrational(p1)) {
		push_integer(Math.floor(p1.a / p1.b));
		return;
	}

	if (isdouble(p1)) {
		push_double(Math.floor(p1.d));
		return;
	}

	push_symbol(FLOOR);
	push(p1);
	list(2);
}

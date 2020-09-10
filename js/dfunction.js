function
dfunction(p1, p2)
{
	var p3 = cdr(p1); // argument list

	if (p3 == symbol(NIL) || find(p3, p2)) {
		push_symbol(DERIVATIVE);
		push(p1);
		push(p2);
		list(3);
		return;
	}

	push_integer(0);
}

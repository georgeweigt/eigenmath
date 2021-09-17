function
static_negate()
{
	var p1 = pop();

	if (isnum(p1)) {
		push(p1);
		negate();
		return;
	}

	if (car(p1) == symbol(MULTIPLY)) {
		push_symbol(MULTIPLY);
		if (isnum(cadr(p1))) {
			push(cadr(p1));
			negate();
			push(cddr(p1));
		} else {
			push_integer(-1);
			push(cdr(p1));
		}
		cons();
		cons();
		return;
	}

	push_symbol(MULTIPLY);
	push_integer(-1);
	push(p1);
	list(3);
}

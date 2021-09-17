function
eval_ceiling(p1)
{
	push(cadr(p1));
	evalf();
	ceiling();
}

function
ceiling()
{
	var p1 = pop();

	if (isrational(p1)) {
		push_integer(Math.ceil(p1.a / p1.b));
		return;
	}

	if (isdouble(p1)) {
		push_double(Math.ceil(p1.d));
		return;
	}

	push_symbol(CEILING);
	push(p1);
	list(2);
}

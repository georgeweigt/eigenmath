function
eval_abs(p1)
{
	push(cadr(p1));
	evalf();
	absfunc();
}

function
absfunc()
{
	var p1;

	p1 = pop();

	if (istensor(p1)) {
		if (p1.dim.length > 1) {
			push_symbol(ABS);
			push(p1);
			list(2);
			return;
		}
		push(p1);
		push(p1);
		conjfunc();
		inner();
		sqrtfunc();
		return;
	}

	if (isnegativenumber(p1)) {
		push(p1);
		negate();
		return;
	}

	if (iscons(p1)) {
		push(p1);
		push(p1);
		conjfunc();
		multiply();
		sqrtfunc();
		return;
	}

	push(p1);
}

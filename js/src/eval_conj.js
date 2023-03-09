function
eval_conj(p1)
{
	push(cadr(p1));
	evalf();
	conjfunc();
}

function
conjfunc()
{
	conjfunc_subst();
	evalf();
}

function
conjfunc_subst()
{
	var h, i, n, p1;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1.elem.length;
		for (i = 0; i < n; i++) {
			push(p1.elem[i]);
			conjfunc_subst();
			p1.elem[i] = pop();
		}
		push(p1);
		return;
	}

	// (-1) ^ expr

	if (car(p1) == symbol(POWER) && isminusone(cadr(p1))) {
		push_symbol(POWER);
		push_integer(-1);
		push(caddr(p1));
		negate();
		list(3);
		return;
	}

	if (iscons(p1)) {
		h = stack.length;
		push(car(p1));
		p1 = cdr(p1);
		while (iscons(p1)) {
			push(car(p1));
			conjfunc_subst();
			p1 = cdr(p1);
		}
		list(stack.length - h);
		return;
	}

	push(p1);
}

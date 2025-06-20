function
eval_erfc(p1)
{
	push(cadr(p1));
	evalf();
	erfcfunc();
}

function
erfcfunc()
{
	var d, i, n, p1, p2;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1.elem.length;
		for (i = 0; i < n; i++) {
			push(p1.elem[i]);
			erfcfunc();
			p1.elem[i] = pop();
		}
		push(p1);
		return;
	}

	push(p1);
	floatfunc();
	p2 = pop();

	if (isnum(p2)) {
		push(p2);
		d = pop_double();
		d = erfc(d);
		push_double(d);
		return;
	}

	push_symbol(ERFC);
	push(p1);
	list(2);
}

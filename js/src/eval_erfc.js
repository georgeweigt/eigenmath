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
	var d, i, n, p1;

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

	if (isnum(p1)) {
		push(p1);
		d = pop_double();
		d = erfc(d);
		push_double(d);
		return;
	}

	push_symbol(ERFC);
	push(p1);
	list(2);
}

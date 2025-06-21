function
eval_tgamma(p1)
{
	push(cadr(p1));
	evalf();
	tgammafunc();
}

function
tgammafunc()
{
	var d, i, n, p1, p2;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1.elem.length;
		for (i = 0; i < n; i++) {
			push(p1.elem[i]);
			tgammafunc();
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
		d = gamma(d);
		push_double(d);
		return;
	}

	push_symbol(TGAMMA);
	push(p1);
	list(2);
}

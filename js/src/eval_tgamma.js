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
	var d, i, n, p1;

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

	if (isnum(p1)) {
		push(p1);
		d = pop_double();
		d = gamma(d);
		push_double(d);
		return;
	}

	push_symbol(TGAMMA);
	push(p1);
	list(2);
}

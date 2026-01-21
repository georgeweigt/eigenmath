function
eval_numerator(p1)
{
	push(cadr(p1));
	evalf();
	numerator();
}

function
numerator()
{
	var i, n, p1;
	p1 = pop();
	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1.elem.length;
		for (i = 0; i < n; i++) {
			push(p1.elem[i]);
			numerator();
			p1.elem[i] = pop();
		}
		push(p1);
		return;
	}
	while (numden_find_divisor(p1)) {
		push(p1);
		numden_cancel_factor();
		p1 = pop();
	}
	push(p1);
}

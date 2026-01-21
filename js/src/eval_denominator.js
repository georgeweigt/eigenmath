function
eval_denominator(p1)
{
	push(cadr(p1));
	evalf();
	denominator();
}

function
denominator()
{
	var i, n, p1;
	p1 = pop();
	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1.elem.length;
		for (i = 0; i < n; i++) {
			push(p1.elem[i]);
			denominator();
			p1.elem[i] = pop();
		}
		push(p1);
		return;
	}
	push(p1);
	numden();
	pop(); // discard numerator
}

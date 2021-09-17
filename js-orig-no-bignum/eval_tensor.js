function
eval_tensor(p1)
{
	var i, n;

	p1 = copy_tensor(p1);

	n = p1.elem.length;

	for (i = 0; i < n; i++) {
		push(p1.elem[i]);
		evalf();
		p1.elem[i] = pop();
	}

	push(p1);

	promote_tensor();
}

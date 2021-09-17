function
eval_transpose(p1)
{
	var t = expanding;
	expanding = 1;
	eval_transpose_nib(p1);
	expanding = t;
}

function
eval_transpose_nib(p1)
{
	var n, m, p2;

	push(cadr(p1));
	evalf();
	p2 = pop();
	push(p2);

	if (!istensor(p2) || p2.dim.length == 1)
		return;

	p1 = cddr(p1);

	if (!iscons(p1)) {
		transpose(1, 2);
		return;
	}

	while (iscons(p1)) {

		push(car(p1));
		evalf();
		n = pop_integer();

		push(cadr(p1));
		evalf();
		m = pop_integer();

		transpose(n, m);

		p1 = cddr(p1);
	}
}

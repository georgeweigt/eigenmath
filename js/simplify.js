function
simplify()
{
	var i, n, p1, p2;

	p1 = pop();

	if (istensor(p1)) {
		n = p1.elem.length;
		for (i = 0; i < n; i++) {
			push(p1.elem[i]);
			simplify();
			p1.elem[i] = pop();
		}
		push(p1);
		return;
	}

	push(p1);
	rationalize();
	evalf(); // to normalize
	p1 = pop();

	push(p1);
	circexp();
	p2 = pop();

	if (complexity(p2) < complexity(p1))
		p1 = p2;

	push(p1);
}

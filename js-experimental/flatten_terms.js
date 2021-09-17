function
flatten_terms(h)
{
	var i, n, p1;

	n = stack.length;

	for (i = h; i < n; i++) {

		p1 = stack[i];

		if (car(p1) == symbol(ADD)) {
			p1 = cdr(p1);
			stack[i] = car(p1);
			p1 = cdr(p1);
			while (iscons(p1)) {
				push(car(p1));
				p1 = cdr(p1);
			}
		}
	}
}

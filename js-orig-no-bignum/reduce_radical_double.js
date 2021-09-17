function
reduce_radical_double(h, COEFF)
{
	var a, b, c, i, n, p1;

	c = COEFF.d;

	n = stack.length;

	for (i = h; i < n; i++) {

		p1 = stack[i];

		if (isradical(p1)) {

			push(cadr(p1)); // base
			a = pop_double();

			push(caddr(p1)); // exponent
			b = pop_double();

			c = c * Math.pow(a, b); // a > 0 by isradical above

			stack.splice(i, 1); // remove factor

			i--; // use same index again
			n--;
		}
	}

	push_double(c);
	COEFF = pop();

	return COEFF;
}

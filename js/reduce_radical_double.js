function
reduce_radical_double(h, COEF)
{
	var a, b, c, i, j, n, p1;

	c = COEF.d

	n = stack.length;

	for (i = h; i < n; i++) {

		p1 = stack[i];

		if (isradical(p1)) {

			push(cadr(p1)); // base
			a = pop_double();

			push(caddr(p1)); // exponent
			b = pop_double();

			c = c * Math.pow(a, b); // a > 0 by isradical above

			// remove the factor

			for (j = i + 1; j < n; j++)
				stack[j - 1] = stack[j];

			i--;
			n--;
			stack.pop();
		}
	}

	push_double(c);
	COEF = pop();

	return COEF;
}

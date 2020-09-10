function
integral_search(F, h, table)
{
	var i, n, I, C;

	n = table.length;

	for (i = 0; i < n; i += 3) {

		scan1(table[i + 0]); // integrand
		I = pop();

		scan1(table[i + 2]); // condition
		C = pop();

		if (integral_search_nib(F, I, C, h))
			break;
	}

	stack.splice(h, stack.length); // pop all

	if (i == n)
		return 0;

	scan1(table[i + 1]); // answer
	eval();

	return 1;
}

// try all combinations of factors on stack

function
integral_search_nib(F, I, C, h)
{
	var i, j, n, p1;

	n = stack.length;

	for (i = h; i < n; i++) {

		set_binding(symbol(METAA), stack[i]);

		for (j = h; j < n; j++) {

			set_binding(symbol(METAB), stack[j]);

			push(C);			// condition ok?
			eval();
			p1 = pop();
			if (iszero(p1))
				continue;		// no, go to next j

			push(F);			// F = I?
			push(I);
			eval();
			subtract();
			p1 = pop();
			if (iszero(p1))
				return 1;		// yes
		}
	}

	return 0;					// no
}

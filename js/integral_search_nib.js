function
integral_search_nib(F, I, C, h)
{
	var i, j, n, p1;

	n = stack.length;

	for (i = h; i < n; i++) {

		set_symbol(symbol(METAA), stack[i], symbol(NIL));

		for (j = h; j < n; j++) {

			set_symbol(symbol(METAB), stack[j], symbol(NIL));

			push(C);			// condition ok?
			evalf();
			p1 = pop();
			if (iszero(p1))
				continue;		// no, go to next j

			push(F);			// F = I?
			push(I);
			evalf();
			subtract();
			p1 = pop();
			if (iszero(p1))
				return 1;		// yes
		}
	}

	return 0;					// no
}

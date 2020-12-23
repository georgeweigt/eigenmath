function
set_component(LVAL, RVAL, h)
{
	var i, k, m, n, t;

	if (!istensor(LVAL))
		stopf("index error");

	// n is number of indices

	n = stack.length - h;

	if (n > LVAL.dim.length)
		stopf("index error");

	// k is combined index

	k = 0;

	for (i = 0; i < n; i++) {
		push(stack[h + i]);
		t = pop_integer();
		if (t < 1 || t > LVAL.dim[i])
			stopf("index error");
		k = k * LVAL.dim[i] + t - 1;
	}

	stack.splice(h); // pop all indices

	if (istensor(RVAL)) {
		m = RVAL.dim.length;
		if (n + m != LVAL.dim.length)
			stopf("index error");
		for (i = 0; i < m; i++)
			if (LVAL.dim[n + i] != RVAL.dim[i])
				stopf("index error");
		m = RVAL.elem.length;
		for (i = 0; i < m; i++)
			LVAL.elem[m * k + i] = RVAL.elem[i];
	} else {
		if (n != LVAL.dim.length)
			stopf("index error");
		LVAL.elem[k] = RVAL;
	}
}

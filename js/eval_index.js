function
eval_index(p1)
{
	var h, n, T;

	T = cadr(p1);

	p1 = cddr(p1);

	h = stack.length;

	while (iscons(p1)) {
		push(car(p1));
		evalf();
		p1 = cdr(p1);
	}

	n = stack.length - h; // number of indices on stack

	// try to optimize by indexing before eval

	if (isusersymbol(T)) {
		p1 = get_binding(T);
		if (istensor(p1) && n <= p1.dim.length) {
			T = p1;
			indexfunc(T, h);
			evalf();
			return;
		}
	}

	push(T);
	evalf();
	T = pop();

	if (!istensor(T)) {
		stack.splice(h); // pop all
		push(T); // quirky, but EVA2.txt depends on it
		return;
	}

	indexfunc(T, h);
}

function
indexfunc(T, h)
{
	var i, k, m, n, t, w, p1;

	n = stack.length - h; // number of indices

	m = T.dim.length;

	if (n < 1 || n > m)
		stopf("index error");

	k = 0;

	for (i = 0; i < n; i++) {
		push(stack[h + i]);
		t = pop_integer();
		if (t < 1 || t > T.dim[i])
			stopf("index error");
		k = k * T.dim[i] + t - 1;
	}

	stack.splice(h); // pop all

	if (n == m) {
		push(T.elem[k]); // scalar result
		return;
	}

	w = 1;

	for (i = n; i < m; i++)
		w *= T.dim[i];

	p1 = alloc_tensor();

	k *= w;

	for (i = 0; i < w; i++)
		p1.elem[i] = T.elem[k + i];

	for (i = 0; i < m - n; i++)
		p1.dim[i] = T.dim[n + i];

	push(p1);
}

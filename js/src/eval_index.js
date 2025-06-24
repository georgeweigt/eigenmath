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

	// try to optimize by indexing before eval

	if (isusersymbol(T)) {
		p1 = get_binding(T);
		n = stack.length - h;
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
		stack.length = h; // pop all
		push(T); // quirky, but EVA2.txt depends on it
		return;
	}

	indexfunc(T, h);
}

function
indexfunc(T, h)
{
	var i, k, m, n, p1, r, t, w;

	m = T.dim.length;

	n = stack.length - h;

	r = m - n; // rank of result

	if (r < 0)
		stopf("index error");

	k = 0;

	for (i = 0; i < n; i++) {
		push(stack[h + i]);
		t = pop_integer();
		if (t < 1 || t > T.dim[i]) {
			if (shuntflag) {
				errorflag = 1;
				t = 1;
			} else
				stopf("index error");
		}
		k = k * T.dim[i] + t - 1;
	}

	stack.length = h; // pop all

	if (r == 0) {
		push(T.elem[k]); // scalar result
		return;
	}

	w = 1;

	for (i = n; i < m; i++)
		w *= T.dim[i];

	k *= w;

	p1 = alloc_tensor();

	for (i = 0; i < w; i++)
		p1.elem[i] = T.elem[k + i];

	for (i = 0; i < r; i++)
		p1.dim[i] = T.dim[n + i];

	push(p1);
}

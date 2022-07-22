void
eval_index(struct atom *p1)
{
	int h, n;
	struct atom *T;

	T = cadr(p1);

	p1 = cddr(p1);

	h = tos;

	while (iscons(p1)) {
		push(car(p1));
		eval();
		p1 = cdr(p1);
	}

	// try to optimize by indexing before eval

	if (isusersymbol(T)) {
		p1 = get_binding(T);
		n = tos - h;
		if (istensor(p1) && n <= p1->u.tensor->ndim) {
			T = p1;
			indexfunc(T, h);
			eval();
			return;
		}
	}

	push(T);
	eval();
	T = pop();

	if (!istensor(T)) {
		tos = h; // pop all
		push(T); // quirky, but EVA2.txt depends on it
		return;
	}

	indexfunc(T, h);
}

void
indexfunc(struct atom *T, int h)
{
	int i, k, m, n, r, t, w;
	struct atom *p1;

	m = T->u.tensor->ndim;

	n = tos - h;

	r = m - n; // rank of result

	if (r < 0)
		stop("index error");

	k = 0;

	for (i = 0; i < n; i++) {
		push(stack[h + i]);
		t = pop_integer();
		if (t < 1 || t > T->u.tensor->dim[i])
			stop("index error");
		k = k * T->u.tensor->dim[i] + t - 1;
	}

	tos = h; // pop all

	if (r == 0) {
		push(T->u.tensor->elem[k]); // scalar result
		return;
	}

	w = 1;

	for (i = n; i < m; i++)
		w *= T->u.tensor->dim[i];

	k *= w;

	p1 = alloc_tensor(w);

	for (i = 0; i < w; i++)
		p1->u.tensor->elem[i] = T->u.tensor->elem[k + i];

	p1->u.tensor->ndim = r;

	for (i = 0; i < r; i++)
		p1->u.tensor->dim[i] = T->u.tensor->dim[n + i];

	push(p1);
}

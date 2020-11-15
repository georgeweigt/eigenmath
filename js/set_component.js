function
set_component(LVAL, RVAL, h)
{
	var i, k, n, t, p1;

	if (!istensor(LVAL) || istensor(RVAL))
		stopf("index error");

	n = stack.length - h;

	if (n != LVAL.dim.length)
		stopf("index error");

	k = 0;

	for (i = 0; i < n; i++) {
		push(stack[h + i]);
		t = pop_integer();
		if (t < 1 || t > LVAL.dim[i])
			stopf("index error");
		k = k * LVAL.dim[i] + t - 1;
	}

	stack.splice(h); // pop all

	p1 = copy_tensor(LVAL);
	p1.elem[k] = RVAL;
	push(p1);
}

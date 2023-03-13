void
eval_dim(struct atom *p1)
{
	int k;
	struct atom *p2;

	push(cadr(p1));
	eval();
	p2 = pop();

	if (!istensor(p2)) {
		push_integer(1);
		return;
	}

	if (lengthf(p1) == 2)
		k = 1;
	else {
		push(caddr(p1));
		eval();
		k = pop_integer();
	}

	if (k < 1 || k > p2->u.tensor->ndim)
		stopf("dim 2nd arg: error");

	push_integer(p2->u.tensor->dim[k - 1]);
}

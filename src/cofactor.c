void
eval_cofactor(struct atom *p1)
{
	int i, j;
	struct atom *p2;

	push(cadr(p1));
	evalf();
	p2 = pop();

	push(caddr(p1));
	evalf();
	i = pop_integer();

	push(cadddr(p1));
	evalf();
	j = pop_integer();

	if (!istensor(p2) || p2->u.tensor->ndim != 2 || p2->u.tensor->dim[0] != p2->u.tensor->dim[1])
		stopf("cofactor");

	if (i < 1 || i > p2->u.tensor->dim[0] || j < 0 || j > p2->u.tensor->dim[1])
		stopf("cofactor");

	push(p2);

	minormatrix(i, j);

	det();

	if ((i + j) % 2)
		negate();
}

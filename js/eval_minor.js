function
eval_minor(p1)
{
	var i, j, p2;

	push(cadr(p1));
	evalf();
	p2 = pop();

	push(caddr(p1));
	evalf();
	i = pop_integer();

	push(cadddr(p1));
	evalf();
	j = pop_integer();

	if (!istensor(p2) || p2.dim.length != 2 || p2.dim[0] != p2.dim[1])
		stopf("minor");

	if (i < 1 || i > p2.dim[0] || j < 0 || j > p2.dim[1])
		stopf("minor");

	push(p2);

	minormatrix(i, j);

	det();
}

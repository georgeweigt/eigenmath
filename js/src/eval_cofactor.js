function
eval_cofactor(p1)
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

	if (!issquarematrix(p2))
		stopf("cofactor: square matrix expected");

	if (i < 1 || i > p2.dim[0] || j < 0 || j > p2.dim[1])
		stopf("cofactor: index err");

	push(p2);

	minormatrix(i, j);

	det();

	if ((i + j) % 2)
		negate();
}

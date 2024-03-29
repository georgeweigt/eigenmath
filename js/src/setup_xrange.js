function
setup_xrange()
{
	var p1, p2, p3;

	xmin = -10;
	xmax = 10;

	p1 = lookup("xrange");
	push(p1);
	eval_nonstop();
	floatfunc();
	p1 = pop();

	if (!istensor(p1) || p1.dim.length != 1 || p1.dim[0] != 2)
		return;

	p2 = p1.elem[0];
	p3 = p1.elem[1];

	if (!isnum(p2) || !isnum(p3))
		return;

	push(p2);
	xmin = pop_double();

	push(p3);
	xmax = pop_double();
}

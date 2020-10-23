function
draw_xrange()
{
	var p, p1, p2;

	xmin = -10;
	xmax = 10;

	p = lookup("xrange");
	push(p);
	evalf();
	floatf();
	p = pop();

	if (!istensor(p) || p.dim.length != 1 || p.dim[0] != 2)
		return;

	p1 = p.elem[0];
	p2 = p.elem[1];

	if (!isnum(p1) || !isnum(p2))
		return;

	push(p1);
	xmin = pop_double();

	push(p2);
	xmax = pop_double();
}

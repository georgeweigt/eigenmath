function
setup_trange()
{
	var p1, p2, p3;

	tmin = -10;
	tmax = 10;

	p1 = lookup("trange");
	push(p1);
	evalf();
	floatf();
	p1 = pop();

	if (!istensor(p1) || p1.dim.length != 1 || p1.dim[0] != 2)
		return;

	p2 = p1.elem[0];
	p3 = p1.elem[1];

	if (!isnum(p2) || !isnum(p3))
		return;

	push(p2);
	tmin = pop_double();

	push(p3);
	tmax = pop_double();
}

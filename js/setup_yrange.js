function
setup_yrange()
{
	var p1, p2, p3;

	ymin = -10;
	ymax = 10;

	p1 = lookup("yrange");
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
	ymin = pop_double();

	push(p3);
	ymax = pop_double();
}

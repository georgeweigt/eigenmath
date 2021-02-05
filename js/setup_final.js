function
setup_final(F, T)
{
	var p1;

	push_double(tmin);
	p1 = pop();
	set_binding(T, p1);

	push(F);
	eval_nonstop();
	p1 = pop();

	if (!istensor(p1)) {
		tmin = xmin;
		tmax = xmax;
	}
}

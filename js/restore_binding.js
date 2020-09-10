function
restore_binding(p)
{
	var p1, p2;

	p2 = frame.pop();
	p1 = frame.pop();

	set_binding_and_arglist(p, p1, p2);
}

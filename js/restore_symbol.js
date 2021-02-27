function
restore_symbol(p)
{
	var p1, p2;

	p2 = frame.pop();
	p1 = frame.pop();

	set_binding(p, p1);
	set_usrfunc(p, p2);
}

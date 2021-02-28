function
restore_symbol(p)
{
	var p1, p2;
	p2 = frame.pop();
	p1 = frame.pop();
	set_symbol(p, p1, p2);
}

function
restore_symbol()
{
	var p1, p2, p3;
	p3 = frame.pop();
	p2 = frame.pop();
	p1 = frame.pop();
	set_symbol(p1, p2, p3);
}

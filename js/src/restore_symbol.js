function
restore_symbol()
{
	var p1, p2, p3;
	p3 = stack.pop();
	p2 = stack.pop();
	p1 = stack.pop();
	set_symbol(p1, p2, p3);
}

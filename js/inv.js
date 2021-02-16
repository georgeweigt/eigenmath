function
inv()
{
	var p1;

	p1 = pop();

	if (!istensor(p1)) {
		push(p1);
		reciprocate();
		return;
	}

	if (p1.dim.length != 2 || p1.dim[0] != p1.dim[1])
		stopf("inv");

	push(p1);
	adj();

	push(p1);
	det();

	divide();
}

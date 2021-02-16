function
inv()
{
	var t, p1;

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

	// ensure inv(A) gives the same result as adj(A)/det(A)

	push(p1);
	t = expanding;
	expanding = 0;
	det();
	expanding = t;

	divide();
}

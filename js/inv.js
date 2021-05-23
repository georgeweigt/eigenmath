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

	if (!issquarematrix(p1))
		stopf("square matrix expected");

	push(p1);
	adj();

	push(p1);
	det();

	divide();
}

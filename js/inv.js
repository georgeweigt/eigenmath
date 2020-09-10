function
inv()
{
	var p1 = pop();

	if (!istensor(p1) || p1.dim.length != 2 || p1.dim[0] != p1.dim[1])
		stop("inv: square matrix expected");

	push(p1);
	adj();
	push(p1);
	det();
	p2 = pop();
	if (iszero(p2))
		stop("inv: singular matrix");
	push(p2);
	divide();
}

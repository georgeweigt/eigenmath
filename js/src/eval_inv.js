function
eval_inv(p1)
{
	push(cadr(p1));
	evalf();
	inv();
}

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
		stopf("inv: square matrix expected");

	push(p1);
	adj();

	push(p1);
	det();

	divide();
}

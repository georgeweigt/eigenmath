void
eval_inv(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	inv();
}

void
inv(void)
{
	struct atom *p1;

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

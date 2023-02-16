function
eval_rationalize(p1)
{
	push(cadr(p1));
	evalf();
	rationalize();
}

function
eval_simplify(p1)
{
	push(cadr(p1));
	evalf();
	rationalize();
	evalf();
}

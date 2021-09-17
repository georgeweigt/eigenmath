function
eval_check(p1)
{
	var t = expanding;
	expanding = 1;
	push(cadr(p1));
	evalp();
	p1 = pop();
	if (iszero(p1))
		stopf("check");
	push_symbol(NIL); // no result is printed
	expanding = t;
}

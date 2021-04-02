function
eval_test(p1)
{
	var t = expanding;
	expanding = 1;
	eval_test_nib(p1);
	expanding = t;
}

function
eval_test_nib(p1)
{
	var p2;
	p1 = cdr(p1);
	while (iscons(p1)) {
		if (!iscons(cdr(p1))) {
			push(car(p1)); // default case
			evalf();
			return;
		}
		push(car(p1));
		evalp();
		p2 = pop();
		if (!iszero(p2)) {
			push(cadr(p1));
			evalf();
			return;
		}
		p1 = cddr(p1);
	}
	push_symbol(NIL);
}

function
eval_or(p1)
{
	var t = expanding;
	expanding = 1;
	eval_or_nib(p1);
	expanding = t;
}

function
eval_or_nib(p1)
{
	var p2;
	p1 = cdr(p1);
	while (iscons(p1)) {
		push(car(p1));
		evalp();
		p2 = pop();
		if (!iszero(p2)) {
			push_integer(1);
			return;
		}
		p1 = cdr(p1);
	}
	push_integer(0);
}

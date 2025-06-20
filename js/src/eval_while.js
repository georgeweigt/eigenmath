function
eval_while(p1)
{
	var p2, p3;
	for (;;) {
		p2 = cdr(p1);
		push(car(p2));
		evalp();
		p3 = pop();
		if (iszero(p3))
			break;
		p2 = cdr(p2);
		while (iscons(p2)) {
			push(car(p2));
			evalf();
			p2 = cdr(p2);
		}
	}
	push_symbol(NIL);
}

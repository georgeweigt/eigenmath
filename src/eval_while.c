void
eval_while(struct atom *p1)
{
	struct atom *p2, *p3;
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
			evalg();
			p2 = cdr(p2);
		}
	}
	push_symbol(NIL);
}

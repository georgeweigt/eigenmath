void
eval_while(struct atom *p1)
{
	struct atom *p2;
	for (;;) {
		push(cadr(p1));
		evalp();
		p2 = pop();
		if (iszero(p2))
			break;
		p2 = cddr(p1);
		while (iscons(p2)) {
			push(car(p2));
			evalg();
			p2 = cdr(p2);
		}
	}
	push_symbol(NIL);
}

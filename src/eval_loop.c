void
eval_loop(struct atom *p1)
{
	int t;
	struct atom *p2;
	t = breakflag;
	breakflag = 0;
	if (lengthf(p1) < 2) {
		push_symbol(NIL);
		return;
	}
	for (;;) {
		p2 = cdr(p1);
		while (iscons(p2)) {
			push(car(p2));
			evalg();
			pop();
			p2 = cdr(p2);
			if (breakflag) {
				breakflag = t;
				push_symbol(NIL);
				return;
			}
		}
	}
}

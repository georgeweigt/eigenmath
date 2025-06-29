function
eval_loop(p1)
{
	var t, p2;
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
			evalf();
			pop();
			p2 = cdr(p2);
			if (breakflag || errorflag) {
				breakflag = t;
				push_symbol(NIL);
				return;
			}
		}
	}
}

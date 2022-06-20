function
eval_do(p1)
{
	push_symbol(NIL);
	p1 = cdr(p1);
	while (iscons(p1)) {
		pop();
		push(car(p1));
		evalf();
		p1 = cdr(p1);
	}
}

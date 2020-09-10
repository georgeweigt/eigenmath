function
eval_do(p1)
{
	push_integer(0);
	p1 = cdr(p1);
	while (iscons(p1)) {
		pop();
		push(car(p1));
		evalf();
		p1 = cdr(p1);
	}
}

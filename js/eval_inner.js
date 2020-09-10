function
eval_inner(p1)
{
	push(cadr(p1));
	evalf();
	p1 = cddr(p1);
	while (iscons(p1)) {
		push(car(p1));
		evalf();
		inner();
		p1 = cdr(p1);
	}
}

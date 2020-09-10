function
eval_eval(p1)
{
	push(cadr(p1));
	evalf();
	p1 = cddr(p1);
	while (iscons(p1)) {
		push(car(p1));
		evalf();
		push(cadr(p1));
		evalf();
		subst();
		p1 = cddr(p1);
	}
	evalf();
}

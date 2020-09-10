function
eval_contract(p1)
{
	push(cadr(p1));
	evalf();
	p1 = cddr(p1);
	if (iscons(p1)) {
		push(car(p1));
		evalf();
		push(cadr(p1));
		evalf();
	} else {
		push_integer(1);
		push_integer(2);
	}
	contract();
}

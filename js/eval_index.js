function
eval_index(p1)
{
	var k;

	p1 = cdr(p1);

	push(car(p1));
	p1 = cdr(p1);
	evalf();

	while (iscons(p1)) {

		push(car(p1));
		p1 = cdr(p1);
		evalf();
		k = pop_integer();

		index(k);
	}
}

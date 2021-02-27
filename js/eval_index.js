function
eval_index(p1)
{
	var k;

	push(cadr(p1));
	evalf();

	p1 = cddr(p1);

	while (iscons(p1)) {

		push(car(p1));
		evalf();
		k = pop_integer();

		index(k);

		p1 = cdr(p1);
	}
}

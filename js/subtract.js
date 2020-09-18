function
subtract()
{
	var p1, p2;

	p2 = pop();
	p1 = pop();

	if (isnum(p1) && isnum(p2)) {
		subtract_numbers(p1, p2);
		return;
	}

	push(p1);
	push(p2);
	negate();
	add();
}

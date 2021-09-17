function
eval_power(p1)
{
	var t, p2;

	push(caddr(p1)); // exponent
	evalf();
	p2 = pop();

	if (isnegativenumber(p2)) {
		// don't expand in denominators
		t = expanding;
		expanding = 0;
		push(cadr(p1)); // base
		evalf();
		push(p2);
		power();
		expanding = t;
	} else {
		push(cadr(p1)); // base
		evalf();
		push(p2);
		power();
	}
}

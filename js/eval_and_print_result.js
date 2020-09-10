function
eval_and_print_result()
{
	var p1, p2;

	p1 = pop();
	push(p1);
	evalf();
	p2 = pop();

	push(p1);
	push(p2);
	print_result();

	if (p2 != symbol(NIL))
		set_binding(symbol(LAST), p2);
}

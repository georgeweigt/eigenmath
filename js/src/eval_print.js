function
eval_print(p1)
{
	p1 = cdr(p1);
	while (iscons(p1)) {
		push(car(p1));
		push(car(p1));
		evalf();
		print_result();
		p1 = cdr(p1);
	}
	push_symbol(NIL);
}

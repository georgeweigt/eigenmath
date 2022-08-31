function
print_result()
{
	var p1, p2;

	p2 = pop(); // result
	p1 = pop(); // input

	if (p2 == symbol(NIL))
		return;

	if (annotate_result(p1, p2)) {
		push_symbol(SETQ);
		push(p1);
		push(p2);
		list(3);
		p2 = pop();
	}

	if (iszero(get_binding(symbol(TTY)))) {
		push(p2);
		display();
	} else
		print_infixform(p2);
}

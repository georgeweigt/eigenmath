void
eval_print(struct atom *p1)
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

void
print_result(void)
{
	struct atom *p1, *p2;

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

// returns 1 if result should be annotated

int
annotate_result(struct atom *p1, struct atom *p2)
{
	if (!isusersymbol(p1))
		return 0;

	if (p1 == p2)
		return 0; // A = A

	if (p1 == symbol(I_LOWER) && isimaginaryunit(p2))
		return 0;

	if (p1 == symbol(J_LOWER) && isimaginaryunit(p2))
		return 0;

	return 1;
}

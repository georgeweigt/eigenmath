void
eval_for(struct atom *p1)
{
	int j, k;
	struct atom *p2, *p3;

	loop_level++;

	push(p1); // protect p1 from garbage collection

	p2 = cadr(p1);
	if (!isusersymbol(p2))
		stopf("for: symbol error");

	push(caddr(p1));
	evalf();
	j = pop_integer();

	push(cadddr(p1));
	evalf();
	k = pop_integer();

	p1 = cddddr(p1);

	save_symbol(p2);

	for (;;) {
		push_integer(j);
		p3 = pop();
		set_symbol(p2, p3, symbol(NIL));
		p3 = p1;
		while (iscons(p3)) {
			gc_check();
			push(car(p3));
			evalf();
			pop(); // discard return value
			p3 = cdr(p3);
		}
		if (j < k)
			j++;
		else if (j > k)
			j--;
		else
			break;
	}

	restore_symbol(p2);

	pop();

	push_symbol(NIL); // return value

	loop_level--;
}

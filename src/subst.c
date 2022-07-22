// Substitute replacement for match in target expr.
//
// Input:	push	target expr
//
//		push	match
//
//		push	replacement
//
// Output:	Result on stack

void
subst(void)
{
	int h, i;
	struct atom *p1, *p2, *p3;

	p3 = pop();
	p2 = pop();

	if (p2 == symbol(NIL) || p3 == symbol(NIL))
		return;

	p1 = pop();

	if (istensor(p1)) {
		push(p1);
		copy_tensor();
		p1 = pop();
		for (i = 0; i < p1->u.tensor->nelem; i++) {
			push(p1->u.tensor->elem[i]);
			push(p2);
			push(p3);
			subst();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (equal(p1, p2)) {
		push(p3);
		return;
	}

	if (iscons(p1)) {
		h = tos;
		while (iscons(p1)) {
			push(car(p1));
			push(p2);
			push(p3);
			subst();
			p1 = cdr(p1);
		}
		list(tos - h);
		return;
	}

	push(p1);
}

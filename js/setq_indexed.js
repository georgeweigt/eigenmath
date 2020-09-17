function
setq_indexed(p1)
{
	var h, p2, S, LVAL, RVAL;

	S = cadadr(p1);

	if (!isusersymbol(S))
		stopf("symbol expected");

	push(S);
	evalf();
	LVAL = pop();

	push(caddr(p1));
	evalf();
	RVAL = pop();

	h = stack.length;

	p2 = cddadr(p1);

	while (iscons(p2)) {
		push(car(p2));
		evalf();
		p2 = cdr(p2);
	}

	set_component(LVAL, RVAL, h);

	p2 = pop();
	set_binding(S, p2);
}

// Example: a[1] = b
//
// p1----->cons--->cons------------------->cons
//         |       |                       |
//         setq    cons--->cons--->cons    b
//                 |       |       |
//                 index   a       1
//
// caadr(p1) = index
// cadadr(p1) = a
// caddr(p1) = b

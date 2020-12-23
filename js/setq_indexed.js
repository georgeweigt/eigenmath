function
setq_indexed(p1)
{
	var h, LVAL, RVAL, S;

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

	p1 = cddadr(p1);

	while (iscons(p1)) {
		push(car(p1));
		evalf();
		p1 = cdr(p1);
	}

	set_component(LVAL, RVAL, h);

	set_binding(S, LVAL);
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

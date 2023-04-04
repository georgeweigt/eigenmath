void
eval_setq(struct atom *p1)
{
	struct atom *p2;

	push_symbol(NIL); // return value

	if (caadr(p1) == symbol(INDEX)) {
		setq_indexed(p1);
		return;
	}

	if (iscons(cadr(p1))) {
		setq_usrfunc(p1);
		return;
	}

	if (!isusersymbol(cadr(p1)))
		stopf("user symbol expected");

	push(caddr(p1));
	evalg(p1); // p1 is protected from garbage collection
	p2 = pop();

	set_symbol(cadr(p1), p2, symbol(NIL));
}

//	Example: a[1] = b
//
//	p1----->cons--->cons------------------->cons
//		|	|			|
//		setq	cons--->cons--->cons	b
//			|	|	|
//			index	a	1
//
//	caadr(p1) = index
//	cadadr(p1) = a
//	caddr(p1) = b

void
setq_indexed(struct atom *p1)
{
	int h;
	struct atom *S, *LVAL, *RVAL;

	S = cadadr(p1);

	if (!isusersymbol(S))
		stopf("user symbol expected");

	push(caddr(p1));
	evalg(p1); // p1 is protected from garbage collection
	RVAL = pop();

	push(S);
	evalf();
	LVAL = pop();

	// eval indices

	p1 = cddadr(p1);

	h = tos;

	while (iscons(p1)) {
		push(car(p1));
		evalf();
		p1 = cdr(p1);
	}

	set_component(LVAL, RVAL, h);

	set_symbol(S, LVAL, symbol(NIL));
}

void
set_component(struct atom *LVAL, struct atom *RVAL, int h)
{
	int i, k, m, n, t;

	if (!istensor(LVAL))
		stopf("index error");

	// n is the number of indices

	n = tos - h;

	if (n < 1 || n > LVAL->u.tensor->ndim)
		stopf("index error");

	// k is the combined index

	k = 0;

	for (i = 0; i < n; i++) {
		push(stack[h + i]);
		t = pop_integer();
		if (t < 1 || t > LVAL->u.tensor->dim[i])
			stopf("index error");
		k = k * LVAL->u.tensor->dim[i] + t - 1;
	}

	tos = h; // pop all

	if (istensor(RVAL)) {
		m = RVAL->u.tensor->ndim;
		if (n + m != LVAL->u.tensor->ndim)
			stopf("index error");
		for (i = 0; i < m; i++)
			if (LVAL->u.tensor->dim[n + i] != RVAL->u.tensor->dim[i])
				stopf("index error");
		m = RVAL->u.tensor->nelem;
		for (i = 0; i < m; i++)
			LVAL->u.tensor->elem[m * k + i] = RVAL->u.tensor->elem[i];
	} else {
		if (n != LVAL->u.tensor->ndim)
			stopf("index error");
		LVAL->u.tensor->elem[k] = RVAL;
	}
}

// Example:
//
//      f(x,y)=x^y
//
// For this definition, p1 points to the following structure.
//
//     p1
//      |
//   ___v__    ______                        ______
//  |CONS  |->|CONS  |--------------------->|CONS  |
//  |______|  |______|                      |______|
//      |         |                             |
//   ___v__    ___v__    ______    ______    ___v__    ______    ______
//  |SETQ  |  |CONS  |->|CONS  |->|CONS  |  |CONS  |->|CONS  |->|CONS  |
//  |______|  |______|  |______|  |______|  |______|  |______|  |______|
//                |         |         |         |         |         |
//             ___v__    ___v__    ___v__    ___v__    ___v__    ___v__
//            |SYM f |  |SYM x |  |SYM y |  |POWER |  |SYM x |  |SYM y |
//            |______|  |______|  |______|  |______|  |______|  |______|
//
// We have
//
//	caadr(p1) points to f
//	cdadr(p1) points to the list (x y)
//	caddr(p1) points to (power x y)

void
setq_usrfunc(struct atom *p1)
{
	struct atom *F, *A, *B, *C;

	F = caadr(p1);
	A = cdadr(p1);
	B = caddr(p1);

	if (!isusersymbol(F))
		stopf("user symbol expected");

	if (lengthf(A) > 9)
		stopf("more than 9 arguments");

	push(B);
	convert_body(A);
	C = pop();

	set_symbol(F, B, C);
}

void
convert_body(struct atom *A)
{
	if (!iscons(A))
		return;

	push(car(A));
	push_symbol(ARG1);
	subst();

	A = cdr(A);
	if (!iscons(A))
		return;

	push(car(A));
	push_symbol(ARG2);
	subst();

	A = cdr(A);
	if (!iscons(A))
		return;

	push(car(A));
	push_symbol(ARG3);
	subst();

	A = cdr(A);
	if (!iscons(A))
		return;

	push(car(A));
	push_symbol(ARG4);
	subst();

	A = cdr(A);
	if (!iscons(A))
		return;

	push(car(A));
	push_symbol(ARG5);
	subst();

	A = cdr(A);

	if (!iscons(A))
		return;

	push(car(A));
	push_symbol(ARG6);
	subst();

	A = cdr(A);
	if (!iscons(A))
		return;

	push(car(A));
	push_symbol(ARG7);
	subst();

	A = cdr(A);
	if (!iscons(A))
		return;

	push(car(A));
	push_symbol(ARG8);
	subst();

	A = cdr(A);
	if (!iscons(A))
		return;

	push(car(A));
	push_symbol(ARG9);
	subst();
}

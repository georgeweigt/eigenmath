function
eval_userfunc(p1)
{
	var h, k, p2, p3, FUNC_NAME, FUNC_DEFN, FORMAL, ACTUAL, T;

	h = stack.length;

	FUNC_NAME = car(p1);
	FUNC_DEFN = get_binding(FUNC_NAME);

	FORMAL = get_arglist(FUNC_NAME);
	ACTUAL = cdr(p1);

	// use "derivative" instead of "d" if there is no user function "d"

	if (FUNC_NAME == symbol(SYMBOL_D) && get_arglist(symbol(SYMBOL_D)) == symbol(NIL)) {
		eval_derivative(p1);
		return;
	}

	// undefined function?

	if (FUNC_NAME == FUNC_DEFN || FUNC_DEFN == symbol(NIL)) {
		push(FUNC_NAME);
		p1 = ACTUAL;
		while (iscons(p1)) {
			push(car(p1));
			evalf();
			p1 = cdr(p1);
		}
		list(stack.length - h);
		return;
	}

	FUNC_DEFN = get_binding(ddual(FUNC_NAME)); // get binding of the dual

	// eval actual args (ACTUAL can be shorter than FORMAL, NIL is pushed for missing args)

	p1 = FORMAL;
	p2 = ACTUAL;

	while (iscons(p1)) {
		push(car(p2));
		evalf();
		p1 = cdr(p1);
		p2 = cdr(p2);
	}

	// assign actual to formal

	k = h;
	p1 = FORMAL;

	while (iscons(p1)) {
		p2 = car(p1);
		p3 = stack[k];
		stack[k] = get_binding(p2);
		set_binding(p2, p3);
		k++;
		p1 = cdr(p1);
	}

	// evaluate user function

	push(FUNC_DEFN);
	evalf();
	T = pop();

	// restore bindings

	k = h;
	p1 = FORMAL;

	while (iscons(p1)) {
		p2 = car(p1);
		p3 = stack[k];
		set_binding(p2, p3);
		k++;
		p1 = cdr(p1);
	}

	stack.splice(h); // pop all

	push(T);
}

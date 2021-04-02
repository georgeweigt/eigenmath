function
eval_user_function(p1)
{
	var t = expanding;
	expanding = 1;
	eval_user_function_nib(p1);
	expanding = t;
}

function
eval_user_function_nib(p1)
{
	var h, i, FUNC_NAME, FUNC_ARGS, FUNC_DEFN;

	FUNC_NAME = car(p1);
	FUNC_ARGS = cdr(p1);

	FUNC_DEFN = get_usrfunc(FUNC_NAME);

	// undefined function?

	if (FUNC_DEFN == symbol(NIL)) {
		if (FUNC_NAME == symbol(SYMBOL_D)) {
			eval_derivative(p1);
			return;
		}
		h = stack.length;
		push(FUNC_NAME);
		while (iscons(FUNC_ARGS)) {
			push(car(FUNC_ARGS));
			evalf();
			FUNC_ARGS = cdr(FUNC_ARGS);
		}
		list(stack.length - h);
		return;
	}

	// eval all args before changing bindings

	for (i = 0; i < 9; i++) {
		push(car(FUNC_ARGS));
		evalf();
		FUNC_ARGS = cdr(FUNC_ARGS);
	}

	save_symbol(symbol(ARG1));
	save_symbol(symbol(ARG2));
	save_symbol(symbol(ARG3));
	save_symbol(symbol(ARG4));
	save_symbol(symbol(ARG5));
	save_symbol(symbol(ARG6));
	save_symbol(symbol(ARG7));
	save_symbol(symbol(ARG8));
	save_symbol(symbol(ARG9));

	p1 = pop();
	set_symbol(symbol(ARG9), p1, symbol(NIL));

	p1 = pop();
	set_symbol(symbol(ARG8), p1, symbol(NIL));

	p1 = pop();
	set_symbol(symbol(ARG7), p1, symbol(NIL));

	p1 = pop();
	set_symbol(symbol(ARG6), p1, symbol(NIL));

	p1 = pop();
	set_symbol(symbol(ARG5), p1, symbol(NIL));

	p1 = pop();
	set_symbol(symbol(ARG4), p1, symbol(NIL));

	p1 = pop();
	set_symbol(symbol(ARG3), p1, symbol(NIL));

	p1 = pop();
	set_symbol(symbol(ARG2), p1, symbol(NIL));

	p1 = pop();
	set_symbol(symbol(ARG1), p1, symbol(NIL));

	push(FUNC_DEFN);
	evalf();

	restore_symbol(symbol(ARG9));
	restore_symbol(symbol(ARG8));
	restore_symbol(symbol(ARG7));
	restore_symbol(symbol(ARG6));
	restore_symbol(symbol(ARG5));
	restore_symbol(symbol(ARG4));
	restore_symbol(symbol(ARG3));
	restore_symbol(symbol(ARG2));
	restore_symbol(symbol(ARG1));
}

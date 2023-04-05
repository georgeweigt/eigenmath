void
eval_user_function(struct atom *p1)
{
	int h, i;
	struct atom *FUNC_NAME, *FUNC_ARGS, *FUNC_DEFN;

	FUNC_NAME = car(p1);
	FUNC_ARGS = cdr(p1);

	FUNC_DEFN = get_usrfunc(FUNC_NAME);

	// undefined function?

	if (FUNC_DEFN == symbol(NIL)) {
		if (FUNC_NAME == symbol(D_LOWER)) {
			expanding++;
			eval_derivative(p1);
			expanding--;
			return;
		}
		h = tos;
		push(FUNC_NAME);
		while (iscons(FUNC_ARGS)) {
			push(car(FUNC_ARGS));
			evalg();
			FUNC_ARGS = cdr(FUNC_ARGS);
		}
		list(tos - h);
		return;
	}

	push(FUNC_DEFN);

	// eval all args before changing bindings

	for (i = 0; i < 9; i++) {
		push(car(FUNC_ARGS));
		evalg();
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

	set_symbol(symbol(ARG9), pop(), symbol(NIL));
	set_symbol(symbol(ARG8), pop(), symbol(NIL));
	set_symbol(symbol(ARG7), pop(), symbol(NIL));
	set_symbol(symbol(ARG6), pop(), symbol(NIL));
	set_symbol(symbol(ARG5), pop(), symbol(NIL));
	set_symbol(symbol(ARG4), pop(), symbol(NIL));
	set_symbol(symbol(ARG3), pop(), symbol(NIL));
	set_symbol(symbol(ARG2), pop(), symbol(NIL));
	set_symbol(symbol(ARG1), pop(), symbol(NIL));

	evalg();

	restore_symbol();
	restore_symbol();
	restore_symbol();
	restore_symbol();
	restore_symbol();
	restore_symbol();
	restore_symbol();
	restore_symbol();
	restore_symbol();
}

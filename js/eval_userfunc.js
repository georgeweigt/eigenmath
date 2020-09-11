function
eval_userfunc(p1)
{
	var h, p2, F, A, B, S;

	// use "derivative" instead of "d" if there is no user function "d"

	if (car(p1) == symbol(SYMBOL_D) && get_arglist(symbol(SYMBOL_D)) == symbol(NIL)) {
		eval_derivative();
		return;
	}

	F = get_binding(car(p1));
	A = get_arglist(car(p1));
	B = cdr(p1);

	// undefined function?

	if (F == car(p1)) {
		h = stack.length;
		push(F);
		p1 = B;
		while (iscons(p1)) {
			push(car(p1));
			evalf();
			p1 = cdr(p1);
		}
		list(stack.length - h);
		return;
	}

	// create the argument substitution list S

	p1 = A;
	p2 = B;
	h = stack.length;
	while (iscons(p1) && iscons(p2)) {
		push(car(p1));
		push(car(p2));
		evalf();
		p1 = cdr(p1);
		p2 = cdr(p2);
	}
	list(stack.length - h);
	S = pop();

	// evaluate the function body

	push(F);

	if (iscons(S))
		rewrite(S);

p1 = pop();
print("eval_userfunc_1 stack.length=" + String(stack.length));
push(p1);
print(prefixform(p1));

	evalf();

p1 = pop();
print("eval_userfunc_2 stack.length=" + String(stack.length));
push(p1);
print(prefixform(p1));

}

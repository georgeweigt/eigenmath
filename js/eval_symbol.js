function
eval_symbol(p1)
{
	var p2;

	if (iscons(p1)) {
		eval_userfunc(p1);
		return;
	}

	p2 = get_binding(p1);

	if (p2 == symbol(NIL)) {
		push(p1);
		return; // undefined symbol evaluates to itself
	}

	set_binding(p1, symbol(NIL)); // prevent infinite loop

	push(p2); // eval symbol binding
	evalf();

	set_binding(p1, p2); // restore binding
}

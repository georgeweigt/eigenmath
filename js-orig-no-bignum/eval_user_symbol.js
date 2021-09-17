function
eval_user_symbol(p1)
{
	var p2;

	p2 = get_binding(p1);

	if (p1 == p2 || p2 == symbol(NIL))
		push(p1); // symbol evaluates to itself
	else {
		push(p2); // eval symbol binding
		evalf();
	}
}

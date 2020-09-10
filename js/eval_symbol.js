function
eval_symbol(p1)
{
	var p2;

	if (iscons(p1)) {
		eval_user_function(p1);
		return;
	}

	p2 = get_binding(p1);

	push(p2);

	if (p1 != p2)
		evalf();
}

function
scan_term()
{
	var h = stack.length, t;

	scan_power();

	while (another_factor_pending()) {

		t = token;

		if (token == '*' || token == '/')
			get_token_skip_newlines();

		scan_power();

		if (t == '/')
			static_reciprocate();
	}

	if (stack.length - h > 1) {
		list(stack.length - h);
		push_symbol(MULTIPLY);
		swap();
		cons();
	}
}

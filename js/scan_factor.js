function
scan_factor()
{
	var h, n;

	h = stack.length;

	switch (token) {

	case '(':
		scan_subexpr();
		break;

	case T_SYMBOL:
		scan_symbol();
		break;

	case T_FUNCTION:
		scan_function_call();
		break;

	case T_INTEGER:
		n = parseFloat(token_buf);
		if (n > 1e10)
			push_double(n);
		else
			push_integer(n);
		get_token();
		break;

	case T_DOUBLE:
		n = parseFloat(token_buf);
		push_double(n);
		get_token();
		break;

	case T_STRING:
		scan_string();
		break;

	default:
		scan_error("expected operand");
		break;
	}

	// index

	if (token == '[') {

		scan_level++;

		get_token(); // get token after '['
		push_symbol(INDEX);
		swap();

		scan_expression();

		while (token == ',') {
			get_token(); // get token after ','
			scan_expression();
		}

		if (token != ']')
			scan_error("expected ']'");

		scan_level--;

		get_token(); // get token after ']'

		list(stack.length - h);
	}

	while (token == '!') {
		get_token(); // get token after '!'
		push_symbol(FACTORIAL);
		swap();
		list(2);
	}
}

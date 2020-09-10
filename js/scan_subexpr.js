function
scan_subexpr()
{
	var h = stack.length;

	scan_level++;

	get_token(); // get token after '('

	scan_stmt();

	while (token == ',') {
		get_token(); // get token after ','
		scan_stmt();
	}

	if (token != ')')
		scan_error("expected ')'");

	scan_level--;

	get_token(); // get token after ')'

	if (stack.length - h > 1)
		build_tensor(h);
}

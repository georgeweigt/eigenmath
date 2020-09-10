function
scan_power()
{
	scan_factor();

	if (token == '^') {

		get_token_skip_newlines();

		push_symbol(POWER);
		swap();
		scan_power();
		list(3);
	}
}

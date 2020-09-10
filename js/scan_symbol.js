function
scan_symbol()
{
	if (scan_mode && strlen(token_buf) == 1)
		switch (token_buf[0]) {
		case 'a':
			push_symbol(METAA);
			break;
		case 'b':
			push_symbol(METAB);
			break;
		case 'x':
			push_symbol(METAX);
			break;
		default:
			push(lookup(token_buf));
			break;
		}
	else
		push(lookup(token_buf));
	get_token();
}

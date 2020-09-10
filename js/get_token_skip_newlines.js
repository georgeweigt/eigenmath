function
get_token_skip_newlines()
{
	scan_level++;
	get_token();
	scan_level--;
}

function
iskeyword(p)
{
	return issymbol(p) && p.func != eval_symbol;
}

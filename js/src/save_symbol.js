function
save_symbol(p)
{
	stack.push(p);
	stack.push(get_binding(p));
	stack.push(get_usrfunc(p));
}

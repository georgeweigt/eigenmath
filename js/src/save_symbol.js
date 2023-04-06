function
save_symbol(p)
{
	frame.push(p);
	frame.push(get_binding(p));
	frame.push(get_usrfunc(p));
}

function
save_symbol(p)
{
	frame.push(get_binding(p));
	frame.push(get_usrfunc(p));
	set_binding(p, symbol(NIL));
	set_usrfunc(p, symbol(NIL));
}

function
save_binding(p)
{
	frame.push(get_binding(p));
	frame.push(get_arglist(p));
}

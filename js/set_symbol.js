function
set_symbol(p, b, u)
{
	var t;
	if (journaling) {
		journal.push(p);
		t = get_binding(p);
		journal.push(t);
		t = get_usrfunc(p);
		journal.push(t);
	}
	binding[p.printname] = b;
	usrfunc[p.printname] = u;
}

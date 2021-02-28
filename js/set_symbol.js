function
set_symbol(p, b, u)
{
	if (journaling) {
		journal.push(p);
		journal.push(b);
		journal.push(u);
	}
	binding[p.printname] = b;
	usrfunc[p.printname] = u;
}

function
undo()
{
	var p, b, u;
	while (journal.length) {
		u = journal.pop();
		b = journal.pop();
		p = journal.pop();
		binding[p.printname] = b;
		usrfunc[p.printname] = u;
	}
}

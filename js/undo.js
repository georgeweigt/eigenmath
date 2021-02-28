function
undo()
{
	var p, b, u;
	journaling = 0;
	while (journal.length > 0) {
		u = journal.pop();
		b = journal.pop();
		p = journal.pop();
		set_symbol(p, b, u);
	}
}

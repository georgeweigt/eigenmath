function
cons()
{
	var p1, p2;
	p2 = pop();
	p1 = pop();
	push({car:p1, cdr:p2});
}

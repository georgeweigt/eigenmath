function
dpower(p1, p2)
{
	push(caddr(p1));	// v/u
	push(cadr(p1));
	divide();

	push(cadr(p1));		// du/dx
	push(p2);
	derivative();

	multiply();

	push(cadr(p1));		// log u
	log();

	push(caddr(p1));	// dv/dx
	push(p2);
	derivative();

	multiply();

	add();
	push(p1);		// u^v
	multiply();
}

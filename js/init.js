function
init()
{
	arglist = [];
	binding = [];

	prep();

	init_bignums();

	push_symbol(POWER);
	push_integer(-1);
	push_rational(1, 2);
	list(3);
	imaginaryunit = pop();

//	run_init_script();
}

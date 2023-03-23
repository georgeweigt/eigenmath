function
init()
{
	eval_level = 0;
	expanding = 1;
	drawing = 0;
	journaling = 0;

	stack = [];
	frame = [];
	journal = [];

	binding = {};
	usrfunc = {};

	push_integer(0);
	zero = pop();

	push_integer(1);
	one = pop();

	push_integer(-1);
	minusone = pop();

	push_symbol(POWER);
	push_integer(-1);
	push_rational(1, 2);
	list(3);
	imaginaryunit = pop();
}

var init_script = [
"i = sqrt(-1)",
"trace = 0",
];


function
init()
{
	var i, n;

	expanding = 1;
	evaldepth = 0;

	stack = [];
	frame = [];

	arglist = {};
	binding = {};

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

	n = init_script.length;

	for (i = 0; i < n; i++) {
		scan(init_script[i], 0);
		evalf();
		pop();
	}
}

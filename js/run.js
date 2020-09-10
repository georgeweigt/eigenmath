var stack;
var frame;
var binding;
var arglist;
var zero;
var one;
var minusone;
var imaginaryunit;
var expanding;
var evaldepth;
var inbuf;
var trace1;
var trace2;

function
run(s)
{
	try {
		run_nib(s);
	}

	catch(err) {
	}

	finally {
	}
}

function
run_nib(s)
{
	var k = 0;

	inbuf = s;

	init();

	for (;;) {

		k = scan_input(k);

		if (k == 0)
			break; // end of input

		eval_and_print_result();
	}
}

function
run_nib(s)
{
	var k = 0;

	inbuf = s;

	init();
	initscript();

	for (;;) {

		k = scan_input(k);

		if (k == 0)
			break; // end of input

		eval_and_print_result();
	}
}
